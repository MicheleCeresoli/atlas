
#include "dataset.h"
#include "crsutils.h"

#include <stdexcept>
#include <errno.h>
#include <thread>

/* -------------------------------------------------------
                        RASTER BAND
---------------------------------------------------------- */

RasterBand::RasterBand(GDALDataset* pd, int i) {

    if (i > pd->GetRasterCount()) {
        std::range_error("the dataset does not contain the desired raster band.");
    }

    // Retrieve the raster band
    pBand = pd->GetRasterBand(i); 

    // Retrieve the block size 
    pBand->GetBlockSize(&_xBlock, &_yBlock); 

    // Retrieve the band width and height (although it is already available in the dataset 
    // container. )
    _width  = pBand->GetXSize(); 
    _height = pBand->GetYSize(); 

    // Retrieve band scale and offset parameters
    _offset = pBand->GetOffset(); 
    _scale  = pBand->GetScale();

    // Compute the band minimum and maximum parameters 
    // Currently uses an APPROXIMATE computation
    int bMin, bMax; 

    double bMinMax[2];

    bMinMax[0] = pBand->GetMinimum(&bMin); 
    bMinMax[1] = pBand->GetMaximum(&bMax); 

    // If the information was not available compute it
    if (!(bMin && bMax)) {
        GDALComputeRasterMinMax((GDALRasterBandH)pBand, TRUE, bMinMax);
    }

    _vMin = bMinMax[0]; 
    _vMax = bMinMax[1];

    // Retrieve the value indicating no data
    _noDataVal = pBand->GetNoDataValue(); 

}

RasterBand::~RasterBand() {
    if (loaded) {
        CPLFree(data);
    }
}

double RasterBand::min() const { return _vMin; }
double RasterBand::max() const { return _vMax; }

double RasterBand::offset() const { return _offset; }
double RasterBand::scale() const { return _scale; }

double RasterBand::noDataVal() const { return _noDataVal; }

void RasterBand::loadData() {

    nLoadedElements = _width*_height;
    data = (float *) CPLMalloc(sizeof(float)*nLoadedElements); 

    CPLErr err = pBand->RasterIO(
        GF_Read, 0, 0, _width, _height, data, _width, _height, GDT_Float32, 0, 0
    );

    if (err != CE_None) {
        std::runtime_error("failed to retrieve raster band data");
    } 

    loaded = true;
    return; 
}

double RasterBand::getData(int i) const {

    if (i < nLoadedElements) {
        return _scale*(double)data[i] + _offset;
    } else {
        std::range_error("raster band data does not have enough elements");
        return _noDataVal;
    }
}

double RasterBand::getData(int u, int v) const {
    return getData(v*_width + u);
}


/* -------------------------------------------------------
                    DATASET CONTAINER 
---------------------------------------------------------- */

DatasetContainer::DatasetContainer(const char *filename, int nThreads) : filename(filename) {

    pDataset = (GDALDataset *) GDALOpen(filename, GA_ReadOnly); 

    // Check whether it is a valid Dataset pointer
    if (pDataset == NULL) {
        std::runtime_error("failed to open the dataset");
    }


    _width  = pDataset->GetRasterXSize(); 
    _height = pDataset->GetRasterYSize(); 
    _rasterCount  = pDataset->GetRasterCount(); 

    // Retrieve all raster bands
    bands.reserve(_rasterCount); 
    for (int k = 0; k < _rasterCount; k++) {
        bands[k] = RasterBand(pDataset, k+1);
    }

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

    // Reserve enough space for all our threads
    s2mT.reserve(nThreads); 
    m2sT.reserve(nThreads);
    
    // Retrieve the map spatial reference system 
    const OGRSpatialReference *mCRS = pDataset->GetSpatialRef(); 

    // Generate a Moon's spherical reference system (with longitude first)
    OGRSpatialReference sCRS = MoonGeographicCRS();

    // Compute the transformation between a geographic and projected map and its inverse.
    for (int k = 0; k < nThreads; k++) 
    {
        s2mT[k] = OGRCreateCoordinateTransformation(sCRS.Clone(), mCRS->Clone()); 
        m2sT[k] = OGRCreateCoordinateTransformation(mCRS->Clone(), sCRS.Clone());
    }

}

DatasetContainer::DatasetContainer(std::string filename, int nThreads) : 
    DatasetContainer(filename.c_str(), nThreads) {}


DatasetContainer::~DatasetContainer() {
    // Close the GDAL dataset
    GDALClose(pDataset); 
}

const char* DatasetContainer::getFilename() const { return filename; }

int DatasetContainer::width() const { return _width; }
int DatasetContainer::height() const { return _height; }
int DatasetContainer::rasterCount() const { return _rasterCount; }

double DatasetContainer::top() const { return _top; }
double DatasetContainer::bottom() const { return _bottom; }
double DatasetContainer::left() const { return _left; }
double DatasetContainer::right() const { return _right; }

Affine DatasetContainer::getAffine() const { return transform; }
Affine DatasetContainer::getInvAffine() const { return iTransform; }

const RasterBand* DatasetContainer::getRasterBand(int i) const {
    return &(bands[i]);
}

const OGRSpatialReference* DatasetContainer::crs() const {
    return pDataset->GetSpatialRef(); 
}


// Conversion Functions

point2 DatasetContainer::pix2map(const point2& p) const {
    return transform*p;
}

point2 DatasetContainer::map2pix(const point2& m) const {
    return iTransform*m;
}

point2 DatasetContainer::sph2map(const point2& s, int threadid) const {
    point2 m = point2(s.x(), s.y()); 

    if (!s2mT[threadid]->Transform(1, &m.e[0], &m.e[1])) {
       std::clog << "Transformation failed." << std::endl;
    }
    
    return m;
}

point2 DatasetContainer::map2sph(const point2& m, int threadid) const {
    point2 s = point2(m.x(), m.y()); 
    
    if (!m2sT[threadid]->Transform(1, &s.e[0], &s.e[1])) {
       std::clog << "Transformation failed." << std::endl;
    }
    return s; 

}

point2 DatasetContainer::sph2pix(const point2& s, int threadid) const {
    return map2pix(sph2map(s, threadid)); 
}

point2 DatasetContainer::pix2sph(const point2& p, int threadid) const {
    return map2sph(pix2map(p), threadid);
}