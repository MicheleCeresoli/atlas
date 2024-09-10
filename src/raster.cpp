
#include "raster.h"
#include "crsutils.h"
#include "utils.h"

#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <string>

/* -------------------------------------------------------
                        RASTER BAND
---------------------------------------------------------- */

RasterBand::RasterBand(std::shared_ptr<GDALDataset> pd, int i) {

    if (i > pd->GetRasterCount()) {
        throw std::range_error("the dataset does not contain the desired raster band.");
    }

    // Retrieve the raster band
    pBand = pd->GetRasterBand(i); 

    // Retrieve the block size 
    int xb, yb; 
    pBand->GetBlockSize(&xb, &yb);
    _xBlock = (uint)xb; 
    _yBlock = (uint)yb; 

    // Retrieve the band width and height (although it is already available in the dataset 
    // container. )
    _width  = (uint)pBand->GetXSize(); 
    _height = (uint)pBand->GetYSize(); 

    // Retrieve band scale and offset parameters
    _offset = pBand->GetOffset(); 
    _scale  = pBand->GetScale();

    // Compute the band minimum and maximum parameters 
    // Currently uses an APPROXIMATE computation
    int bMin, bMax; 

    double minMax[2];

    minMax[0] = pBand->GetMinimum(&bMin); 
    minMax[1] = pBand->GetMaximum(&bMax); 

    // If the information was not available compute it
    if (!(bMin && bMax)) {
        pBand->ComputeRasterMinMax(FALSE, minMax);
    }

    /* Now, since these datasets were made by fucking idiots the returned NoDataValue 
     * does not match the one that is actually used. Thus we re-set it to the minimum 
     * value found (which for CE'2 DEM is -99999) and then recompute the min\max values. 
     * 
     * The idea is the following, the lowest altitude on the Moon with respect to its 
     * reference radius is about 10km (Antoniadi crater). To be safe we assume that if 
     * the actual altitude is smaller than -50000, the minimum value is not valid */

    if (minMax[0] < -50000) {
        // Update the NoDataValue
        pBand->SetNoDataValue(minMax[0]); 
        // Recompute settings 
        pBand->ComputeRasterMinMax(FALSE, minMax); 
    }

    _vMin = minMax[0]; 
    _vMax = minMax[1];

    // Retrieve the value indicating no data
    _noDataVal = pBand->GetNoDataValue(); 

}

void RasterBand::loadData() {

    nLoadedElements = _width*_height;

    // Create a shared pointer
    data = std::shared_ptr<float>(
        new float[nLoadedElements], std::default_delete<float[]>()
    );

    // Load the data into the
    CPLErr err = pBand->RasterIO(
        GF_Read, 0, 0, _width, _height, data.get(), _width, _height, GDT_Float32, 0, 0
    );

    if (err != CE_None) {
        throw std::runtime_error("failed to retrieve raster band data");
    } 

    return; 
}

void RasterBand::unloadData() {
    nLoadedElements = 0; 
    data.reset();
}

double RasterBand::getData(uint i) const {

    if (i < nLoadedElements) {
        return _scale*(double)data.get()[i] + _offset;
    } else {
        throw std::range_error("raster band data does not have enough elements");
    }
}

double RasterBand::getData(uint u, uint v) const {
    return getData(v*_width + u);
}


/* -------------------------------------------------------
                    DATASET CONTAINER 
---------------------------------------------------------- */

RasterFile::RasterFile(const std::string& file, size_t nThreads) : _nThreads(nThreads)
{
    // Generate filepath object
    filepath = std::filesystem::path(file); 

    // Retrieve filename 
    filename = filepath.filename().string(); 

    pDataset = std::shared_ptr<GDALDataset>(
        (GDALDataset *) GDALOpen(file.c_str(), GA_ReadOnly), GDALClose
    );

    // Check whether it is a valid Dataset pointer
    if (pDataset == NULL) {
        throw std::runtime_error("failed to open the dataset");
    }

    // Retrieve dataset parameters. 
    _width  = (uint)pDataset->GetRasterXSize(); 
    _height = (uint)pDataset->GetRasterYSize(); 
    _rasterCount  = (size_t)pDataset->GetRasterCount(); 

    // Retrieve the Affine transformation of the projection
    double adf[6]; 

    if (pDataset->GetGeoTransform(adf) == CE_None) {
        transform = Affine(adf); 
    } else {
        transform = Affine(); 
    }

    iTransform = inverse(transform);

    // Compute the coordinates of the bottom-right pixel
    point2 p = transform*point2(_width, _height);

    // Compute the map projection coordinates limits 
    _left = transform[2];
    _top  = transform[5];

    _right  = p[0];
    _bottom = p[1];

    // Retrieve the raster highest resolution from the Affine transform 
    _resolution = MAX(fabs(transform[0]), fabs(transform[4]));

    // Update the raster's reference system projection
    updateReferenceSystem();

    // Setup the map projection to geographic transformations.
    setupTransformations(); 

    // Compute the raster longitude and latitude bounds
    computeRasterBounds(); 

    // Retrieve all raster bands
    bands.reserve((size_t)_rasterCount); 
    for (size_t k = 0; k < _rasterCount; k++) {
        bands.push_back(RasterBand(pDataset, (int)k+1));
    }

}

// Raster limits 

void RasterFile::getLongitudeBounds(double* bounds) const {
    bounds[0] = lon_bounds[0]; 
    bounds[1] = lon_bounds[1];
}

void RasterFile::getLatitudeBounds(double* bounds) const {
    bounds[0] = lat_bounds[0]; 
    bounds[1] = lat_bounds[1];
}

bool RasterFile::isWithinGeographicBounds(const point2& p) const {

    return ((lon_bounds[0] <= p[0] && p[0] <= lon_bounds[1]) && 
            (lat_bounds[0] <= p[1] && p[1] <= lat_bounds[1]));
            
}


// Raster Bands Interfaces

void RasterFile::loadBands() {
    for (size_t k = 0; k < _rasterCount; k++) {
        loadBand(k); 
    }
}

void RasterFile::unloadBands() {
    for (size_t k = 0; k < _rasterCount; k++) {
        unloadBand(k);
    }
}


// Transformation Functions
point2 RasterFile::sph2map(const point2& s, uint threadid) const {

    int flags[1]; 
    point2 m(s); 
    
    if(!s2mT[threadid]->Transform(1, &m.e[0], &m.e[1], nullptr, flags))
        std::clog << "Transformation failed." << std::endl; 
    
    return m;
}

point2 RasterFile::map2sph(const point2& m, uint threadid) const {

    int flags[1]; 
    point2 s(m);

    if(!m2sT[threadid]->Transform(1, &s.e[0], &s.e[1], nullptr, flags))
        std::clog << "Transformation failed." << std::endl; 
    
    return s;

}

point2 RasterFile::sph2pix(const point2& s, uint threadid) const {
    return map2pix(sph2map(s, threadid)); 
}

point2 RasterFile::pix2sph(const point2& p, uint threadid) const {
    return map2sph(pix2map(p), threadid);
}


// Private functions 

void RasterFile::updateReferenceSystem() { 

    // Read the map projection information from the associated .PRJ file
    std::string projFile = filepath.string().substr(0, filepath.string().size() - 3) + "prj";
    std::string wkt = readFileContent(projFile);  
    
    // Update the dataset reference system
    OGRSpatialReference mCRS(wkt.c_str());
    pDataset->SetSpatialRef(&mCRS);

}

void RasterFile::setupTransformations() {

    // Reserve enough space for all our threads
    s2mT.reserve(_nThreads); 
    m2sT.reserve(_nThreads);

    // Retrieve the map spatial reference system 
    const OGRSpatialReference mCRS = *pDataset->GetSpatialRef(); 

    // Generate a Moon's spherical reference system (with longitude first)
    OGRSpatialReference sCRS = MoonGeographicCRS();

    // Compute the transformation between a geographic and projected map and its inverse.
    for (size_t k = 0; k < _nThreads; k++) 
    {
        s2mT.push_back(
            std::shared_ptr<OGRCoordinateTransformation>(
                OGRCreateCoordinateTransformation(&sCRS, &mCRS)
            )
        ); 

        m2sT.push_back(
            std::shared_ptr<OGRCoordinateTransformation>(
                OGRCreateCoordinateTransformation(&mCRS, &sCRS)
            )
        );
    }

}


void RasterFile::computeRasterBounds() {

    // This routine is super specific for Chang'e DEM files! 
    std::size_t len = filename.size(); 

    int lat_id = filename[len - 18UL];
    double dlon = 360.0; 

    if (lat_id == 65) {
        lat_bounds[0] = 84.0; 
        lat_bounds[1] = 90.0;
    } 
    else if (lat_id == 78) {
        lat_bounds[0] = -90; 
        lat_bounds[1] = -84;
    } 
    else {
        
        double dlat = (lat_id - 66)*14; 
        lat_bounds[0] = 70 - dlat;
        lat_bounds[1] = 84.0 - dlat;

        // Compute the longitude span of that tile 
        switch (lat_id) {

            case 66:
            case 77: 
                dlon = 45.0;
                break; 

            case 67: 
            case 76: 
                dlon = 30.0;
                break; 

            case 68: 
            case 75:
                dlon = 24;
                break; 

            case 69: 
            case 74: 
                dlon = 20.0;
                break;
            
            default: 
                dlon  = 18.0;
                break;

        }
        
    }

    int lon_id = (int)atof(filename.substr(len - 17UL, 3).c_str()); 
 
    lon_bounds[1] = lon_id*dlon - 180.0;
    lon_bounds[0] = lon_bounds[1] - dlon;


}



/* -------------------------------------------------------
                    RASTER CONTAINER
---------------------------------------------------------- */

// Constructors 

RasterContainer::RasterContainer(
    std::vector<std::string> files, size_t nThreads, bool displayLogs
) {

    // If there are no files, throw an error
    size_t nFiles = files.size(); 

    /* Register GDAL drivers to open raster datasets. Technically from the GDAL docs 
     * this function should be called just once at the start of the program, however 
     * (1) I don't see many scenarios in which one would use multiple DEMs (2) I don't 
     * it does any harm calling it more than once. */ 
    GDALAllRegister();

    // Initialise the resolution.
    _resolution  = inf; 
    double res;
    
    // Store current time
    auto t1 = std::chrono::high_resolution_clock::now();
    
    // Load up all the rasters
    rasters.reserve(nFiles); 
    for (auto f : files)
    {
        // Prevent opening an empty string.
        if (f.empty()) {
            nFiles--;
            continue;
        }

        // Add the raster and retrieve its name.
        rasters.push_back(RasterFile(f, nThreads));

        if (displayLogs) {
            displayLoadingStatus(RasterLoadingStatus::LOADING, nFiles);
        }
            
        // Update the minimum DEM resolution
        res = rasters.back().resolution();
        if (res < _resolution) {
            _resolution = res;
        }

    }    

    // Initialise the raster use and flags vectors 
    rastersUsed = std::vector<uint>(nFiles, 0);
    rastersFlag = std::vector<uint>(nFiles, 0); 

    // Retrieve time to compute rendering duration
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);

    if (displayLogs) {
        displayLoadingStatus(RasterLoadingStatus::COMPLETED, nFiles); 
    }

}

RasterContainer::RasterContainer(std::string filename, size_t nThreads, bool displayLogs) : 
    RasterContainer(std::vector<std::string>{filename}, nThreads, displayLogs) {}


void RasterContainer::loadRasters() {
    for (size_t k = 0; k < rasters.size(); k++) {
        rasters[k].loadBand(0);
    }
}

void RasterContainer::unloadRasters() {
    for (size_t k = 0; k < rasters.size(); k++) {
        rasters[k].unloadBand(0);
    }
}

double RasterContainer::getData(const point2& s, bool interp, uint tid) {

    point2 pix;  
    for (size_t k = 0; k < rasters.size(); k++) {
            
        if (rasters[k].isWithinGeographicBounds(s)) {
            
            /* In the worst case scenario, this lock is acquired just once per 
             * image per used raster, so we shouldn't have many performance drawbacks. */
            if (rastersUsed[k] == 0) {
                
                std::unique_lock<std::mutex> lock(rasterUpdateMutex);

                // If the rasters band is not loaded, load it! 
                if (!rasters[k].isBandLoaded(0)) {
                    rasters[k].loadBand(0);
                }

                // Update the number of times the raster has been used 
                rastersUsed[k]++; 
            }

            // Retrieve the pixel data value
            pix = rasters[k].sph2pix(s, tid); 
            return interp ? interpolateRaster(pix, k, tid) : 
                            rasters[k].getBandData(pix[0], pix[1], 0);         
        }

    }

    return -inf; 
}

void RasterContainer::cleanupRasters(uint threshold) {

    for (size_t k; k < rasters.size(); k++) 
    {
        if (rastersUsed[k] == 0) {
            if (++rastersFlag[k] >= threshold && rasters[k].isBandLoaded(0)) {
                rasters[k].unloadBand(0);
                rastersFlag[k] = 0; 
            }
        } else {
            rastersFlag[k] = 0; 
            rastersUsed[k] = 0;
        }
    }
}

double RasterContainer::interpolateRaster(const point2& pix, size_t rid, int tid) const {

    int u = pix[0], v = pix[1]; 

    // These are the upper-left (dr) and bottom-right (dl) points
    point2 dl(u, v); 
    point2 dr = dl + 1;

    dl = pix - dl; 
    dr = dr - pix; 
    
    // Retrieve no data value
    double noDataVal = rasters[rid].getBandNoDataValue(0); 

    // Retrieve raster width and height 
    double s = sqrt(2), sk; 
    double h, n = 0.0, d = 0.0; 

    bool hu = (u+1) < rasters[rid].width(); 
    bool hv = (v+1) < rasters[rid].height(); 

    for (size_t j = 0; j < 4; j++) {

        switch (j) {
            case 0:
                sk = dl.norm();
                h = rasters[rid].getBandData(u, v, 0); 
                break;

            case 1: 
                sk = sqrt(dr[0]*dr[0] + dl[1]*dl[1]);
                h = hu ? rasters[rid].getBandData(u+1, v, 0) : noDataVal; 
                break;

            case 2: 
                sk = sqrt(dl[0]*dl[0] + dr[1]*dr[1]); 
                h = hv ? rasters[rid].getBandData(u, v+1, 0) : noDataVal; 
                break;

            default: 
                sk = dr.norm(); 
                h = (hu && hv) ? rasters[rid].getBandData(u+1, v+1, 0) : noDataVal; 
                break;
        }

        if (h != noDataVal) {
            sk = 1 - sk/s; 
            n += sk*h;
            d += sk; 
        } 

    }

    return n/d; 
}

void RasterContainer::displayLoadingStatus(RasterLoadingStatus s, size_t nFiles) {

    if (nFiles == 0)
        return;  

    int dl; 
    std::string filename = rasters.back().getFileName(); 

    if (s == RasterLoadingStatus::LOADING && nFiles > 0) {

        // Update loading status
        int progress = (int)(100*(double)rasters.size()/nFiles);

        // Compute message 
        dl = displayMessage.length() - filename.length(); 
        displayMessage = dl > 0 ? filename + std::string(dl, ' ') : filename;
        
        // Print the message to terminal
        std::clog << "\r[" <<  std::setw(3) << progress 
                  << "%] \033[32mLoading raster file:\033[0m " 
                  << displayMessage << std::flush;

    } else {

        // Generate completion message. 
        displayMessage = "\r[100%] Raster files loaded.";
        dl = 29 + filename.length() - displayMessage.length();

        if (dl > 0) 
            displayMessage += std::string(dl, ' '); 
        
        std::clog << displayMessage << std::endl;
    }

}