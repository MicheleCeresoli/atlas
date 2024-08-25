

#include "affine.h"
#include "vec2.h"
#include "vec3.h"
#include "utils.h"
#include "dcm.h"
#include "camera.h"
#include "renderer.h"
#include "world.h"

#include "pool.h"
#include "pixel.h"
#include "dataset.h"
#include "crsutils.h"

#include "gdal_priv.h"

#include <cmath>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <thread>

#define CAM_FOV (40) 

void write_pixel(std::ostream& out, const PixelData& data) {
    
    auto g = 0.0;
    auto b = 0.0;

    auto r = data.t;
    if (r == inf) {
        r = 0.0;
    } else {
        r /= 1738e3/sin(deg2rad(CAM_FOV/2));
        g = r; 
        b = r; 
    }

    // Translate the [0,1] component values to the byte range [0,255]
    int rbyte = int(255.999*r);
    int gbyte = int(255.999*g);
    int bbyte = int(255.999*b);

    // Write out the pixel color components. 
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n'; 

}

void toImage(Camera& cam, const std::vector<RenderedPixel> pixels)
{

    std::cout << "P3\n" << cam.width << ' ' << cam.height << "\n255\n"; 

    for (int j = 0; j < pixels.size(); j++)
    {
        write_pixel(std::cout, pixels[j].d);
    }

    std::clog << "\rDone. \n";

}

void test_size(double *x) {
    std::cout << sizeof(&x) / sizeof(x[0]) << std::endl;
}

void tryReferenceDC(DatasetContainer& dc) {
    point2 pj = dc.pix2sph(point2(0.0, 0.0), 0); 
    std::cout << pj << std::endl;
}


int main(int argc, const char* argv[])
{

    // double x[3] = {0, 0, 0}; 
    // std::cout << sizeof(x) / sizeof(x[0]) << std::endl; 
    // test_size(x); 

    // // Test Affine stuff 
    // std::cout << Affine() << std::endl;
    // std::cout << Affine(1.0, 2.0, 3.0, 4, 5, 6) << std::endl;

    // std::cout << Affine::identity() << std::endl; 
    // std::cout << Affine::scale(2.0) << std::endl; 
    // std::cout << Affine::scale(2.0, 3.0) << std::endl; 

    // auto a = Affine(2.0, 1.0, -4.0, 5.0, 2.0, 1.5);
    // std::cout << inverse(a) << std::endl; 

    // point2 p2(1.0, 3.0); 
    // std::cout << a*p2 << std::endl;

    // std::cout << a.xoff() << ' ' << a.yoff() << std::endl;

    // std::cout << inverse(a)*a << std::endl; 

    // std::cout << a << std::endl;
    // a*=inverse(a);
    // std::cout << a << std::endl;

    const char* pszFilename = "../CE2_GRAS_DEM_50m_B001_77N158W_A.tif";
    // double adfGeoTransform[6];

    GDALAllRegister();

    ThreadPool pool = ThreadPool(4); 
    DatasetContainer dc = DatasetContainer(pszFilename, pool.nThreads()); 

    std::cout << pool.nThreads() << std::endl; 

    std::cout << dc.pix2sph(point2(0.0, 0.0), 0) << std::endl;
    std::cout << dc.pix2sph(point2(0.0, 0.0), 0) << std::endl;
    std::cout << dc.pix2sph(point2(0.0, 0.0), 0) << std::endl;

    DatasetContainer dc2 = dc; 
    std::cout << dc2.pix2sph(point2(0.0, 0.0), 0) << std::endl;
    std::cout << dc2.pix2sph(point2(0.0, 0.0), 0) << std::endl;
    std::cout << dc2.pix2sph(point2(0.0, 0.0), 0) << std::endl;
    std::cout << "\n -------- Outside thread-pool everything works fine...\n\n"; 


    for (int j = 0; j < 5; j++)
        tryReferenceDC(dc); 

    std::cout << "\n -------- With simple reference it works...\n\n";

    // // Enqueue tasks for execution 
    for (int j = 0; j < 50; j++)
    {
        pool.addTask([j, &dc] (const ThreadWorker& wk) {

            std::cout << "In task: " << 0 << " with threads: " << wk.id() << std::endl; 
            std::cout << dc.width() << std::endl;

            point2 pj = dc.pix2sph(point2(0.0, 0.0), wk.id()); 
            std::cout << pj << std::endl;
        });
    }

    pool.startPool(); 
    pool.stopPool();

    // std::cout << dc.getFilename() << std::endl; 
    // std::cout << dc.getAffine() << std::endl; 
    // std::cout << dc.getInvAffine() << std::endl; 
    // std::cout << dc.height() << "x" << dc.width() << "x" << dc.rasterCount() << std::endl; 
    // std::cout << dc.top() << std::endl; 
    // std::cout << dc.bottom() << std::endl; 
    // std::cout << dc.right() << std::endl; 
    // std::cout << dc.left() << std::endl; 

    // std::cout << "\n\n ---------------------------------------------- \n"; 

    // const GDALAccess eAccess = GA_ReadOnly; 

    // GDALDataset *poDataset = (GDALDataset *) GDALOpen(pszFilename, eAccess); 
    // if (poDataset == NULL) 
    // {
    //     std::cout << "Could not open dataset" << std::endl; 
    // } 
    // else
    // {
    //     std::cout << "Dataset successfully opened." << std::endl; 
    //     std::cout << "Dataset desc: " << poDataset->GetDescription() << std::endl; 
    //     std::cout << "Format: " << poDataset->GetDriver()->GetDescription() << std::endl; 
        
    //     // Retrieve General Raster Properties
    //     std::cout << "Raster XSize: " << poDataset->GetRasterXSize() << std::endl; 
    //     std::cout << "Raster YSize: " << poDataset->GetRasterYSize() << std::endl; 
    //     std::cout << "Raster Count: " << poDataset->GetRasterCount() << std::endl; 

    //     // Retrieve Reference SYstem
    //     if(poDataset->GetProjectionRef() != NULL)
    //         std::cout << "Projection: " << poDataset->GetProjectionRef() << std::endl; 

    //     // Display Geotransform Infos 
    //     if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None) 
    //     {
    //         std::cout << "Origin: (" << adfGeoTransform[0] << ", " 
    //                   << adfGeoTransform[3] << ")" << std::endl;

    //         std::cout << "Pixel Size: (" << adfGeoTransform[1] << ", " 
    //                   << adfGeoTransform[5] << ")" << std::endl;
    //     }

    //     std::cout << std::endl << "Fetching Raster Band Data: " << std::endl << std::endl; 

    //     // Here we fetch raster band data   
    //     GDALRasterBand *poBand;
    //     int nBlockXSize, nBlockYSize; 
    //     int bGotMin, bGotMax; 
    //     double adfMinMax[2]; 

    //     // The first band is number 1 not 0!
    //     poBand = poDataset->GetRasterBand(1);
    //     poBand->GetBlockSize(&nBlockXSize, &nBlockYSize); 

    //     std::cout << "Block: " << nBlockXSize << "x" << nBlockYSize << std::endl;
    //     std::cout << "Size: " << poBand->GetXSize() << "x" << poBand->GetYSize() << std::endl;
    //     std::cout << "Type: " << GDALGetDataTypeName(poBand->GetRasterDataType()) << std::endl; 
    //     std::cout << "Color: " << 
    //         GDALGetColorInterpretationName(poBand->GetColorInterpretation()) << std::endl; 
        
    //     adfMinMax[0] = poBand->GetMinimum(&bGotMin); 
    //     adfMinMax[1] = poBand->GetMaximum(&bGotMax); 

    //     if (!(bGotMin && bGotMax)) {
    //         std::cout << "Computing min/max values..." << std::endl; 
    //         GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax); 
    //     }

    //     std::cout << "Min: " << adfMinMax[0] << std::endl;
    //     std::cout << "Max: " << adfMinMax[1] << std::endl;

    //     std::cout << "Offset: " << poBand->GetOffset() << std::endl; 
    //     std::cout << "Scale: " << poBand->GetScale() << std::endl;
    //     std::cout << "Unit: " << poBand->GetUnitType() << std::endl;

    //     std::cout << "NoDataValue: " << poBand->GetNoDataValue() << std::endl; 

    //     if (poBand->GetOverviewCount() > 0) {
    //         std::cout << "Band has " << poBand->GetOverviewCount() << 
    //             " overviews." << std::endl; 
    //     }

    //     if (poBand->GetColorTable() != NULL) {
    //         std::cout << "Band has a color table with " << 
    //             poBand->GetColorTable()->GetColorEntryCount() << " entries" << std::endl;

    //     }

    //     GDALRasterBand *mask = poBand->GetMaskBand(); 
    //     std::cout << "Mask XSize: " << mask->GetXSize() << std::endl;
    //     std::cout << "Mask YSize: " << mask->GetYSize() << std::endl;

    //     // Valid data is represented with 255, invalid data with 0.
    //     std::cout << "Mask Type: " << GDALGetDataTypeName(mask->GetRasterDataType()) << std::endl; 
    //     std::cout << "Mask min: " << mask->GetMinimum() << std::endl; 
    //     std::cout << "Mask max: " << mask->GetMaximum() << std::endl; 

    //     std::cout << "\nReading Raster Data..." << std::endl << std::endl; 

    //     // Read the data stored in the raster. 
    //     float *pRasterData; 

    //     int xSize = poBand->GetXSize(); 
    //     int ySize = poBand->GetYSize(); 

    //     std::cout << "XSize: " << xSize << " YSize: " << ySize << std::endl;

    //     pRasterData = (float *) CPLMalloc(sizeof(float)*xSize*ySize); 

    //     // GF_Read is used to tell whether to read or write. 
    //     // First 4 describe the window to read.

    //     // Then you have the size of the buffer you are storing saved data into.
    //     // Last two leave to 0
    //     // CPLErr err = poBand->RasterIO(
    //         // GF_Read, 0, 0, xSize, ySize, pRasterData, xSize, ySize, GDT_Float32, 0, 0
    //     // );
        
    //     // if (err == CE_None){
    //         // std::cout << "First Element: " << pRasterData[0] << std::endl; 
    //     // }

    //     CPLFree(pRasterData);

    //     // Retrieve the CRS of the dataset 
    //     const OGRSpatialReference *oSRS = poDataset->GetSpatialRef();

    //     char *pszWKT = NULL; 
    //     oSRS->exportToWkt(&pszWKT); 
    //     std::cout << "CRS: " << pszWKT << std::endl; 
    //     CPLFree(pszWKT); 

    //     std::cout << "CRS is Geographic: " << oSRS->IsGeographic() << std::endl;

    //     // Create a geographic CRS 
    //     OGRSpatialReference oDST; 

    //     oDST.SetGeogCS( 
    //         "GCS_Moon_2000",
    //         "D_Moon_2000", 
    //         "MOON_2000_IAU_IAG", 
    //         1737400.0, 0.0, 
    //         "Reference Meridian", 0.0, 
    //         SRS_UA_DEGREE, atof(SRS_UA_DEGREE_CONV)
    //     );

    //     // Check whether it was properly created. 

    //     oDST.exportToWkt(&pszWKT); 
    //     std::cout << "CRS: " << pszWKT << std::endl; 
    //     CPLFree(pszWKT); 

    //     // Change mapping to longitude first
    //     oDST.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);

    //     std::cout << "Axis Mapping: " << oDST.GetAxisMappingStrategy() << std::endl;
    //     std::cout << "CRS is Geographic: " << oDST.IsGeographic() << std::endl;


    //     // Lets try to transform coordinates between these two reference systems 
    //     OGRCoordinateTransformation *poCT; 

    //     double x = adfGeoTransform[0]; 
    //     double y = adfGeoTransform[3]; 
        
    //     // Create the transformation
    //     poCT = OGRCreateCoordinateTransformation(oSRS, &oDST);

    //     if (poCT == NULL || !poCT->Transform(1, &x, &y)) {
    //         std::cout << "Transformation failed." << std::endl; 
    //     } else {
    //         std::cout << "Output: " << x << ", " << y << std::endl; 
    //     }

    //     // Lets compute the transformation that returns the pixel row column from a 
    //     // given set of map coordinates 

    //     double sa = adfGeoTransform[1], sb = adfGeoTransform[2], sc = adfGeoTransform[0]; 
    //     double sd = adfGeoTransform[4], se = adfGeoTransform[5], sf = adfGeoTransform[3];

    //     double det = 1.0/(sa*se - sb*sd); 
    //     double a = se*det; 
    //     double b = -sb*det; 
    //     double c = -b*sf - a*sc;
    //     double d = -sd*det;
    //     double e = sa*det;
    //     double f = -sc*d - sf*e;

    //     std::cout << std::fixed << std::setprecision(4) << std::endl;
    //     std::cout << "Original: " << std::endl << sa << ", " << sb << ", "  << sc << std::endl; 

    //     std::cout << sd << ", " << se << ", " << sf << std::endl;

    //     std::cout << "Inverse: " << std::endl << a << ", " << b << ", " << c << std::endl; 
    //     std::cout << d << ", " << e << ", " << f << std::endl;

    // }

    // GDALClose((GDALDatasetH)poDataset);

    // unsigned int nthreads = std::thread::hardware_concurrency(); 
    // std::clog << "Number of available threads: " << nthreads << std::endl; 


    // int    cam_res = 640;
    // double cam_fov = deg2rad(CAM_FOV); 

    // double h = 1.1*1738e3/std::sin(cam_fov/2);

    // point3 cam_pos = point3(h, 0, 0); 
    // dcm    cam_dcm = dcm(0, 0, -1, 0, 1, 0, 1, 0, 0);

    // // Initialise the camera object
    // Camera cam(cam_res, cam_fov);
    // cam.set_dcm(cam_dcm); 
    // cam.set_pos(cam_pos); 

    // World w = World(); 

    // // Create the Renderer (1 thread, batch size 64)
    // Renderer renderer(7, 640);

    // // Render the image
    // auto t1 = std::chrono::high_resolution_clock::now();
    // std::vector<RenderedPixel> pixels = renderer.render(cam, w); 
    // auto t2 = std::chrono::high_resolution_clock::now();

    // // Write the pixels to a PPM image file
    // toImage(cam, pixels); 

    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    
    // // // To get the value of duration use the count()
    // // // member function on the duration object
    // // std::clog << "The rendering took: " << duration.count() << " ms" << std::endl;

    // GDALAllRegister(); 
    // // ThreadPool pool(7); 

    // DatasetContainer dc = DatasetContainer(pszFilename);

    // OGRSpatialReference* crs = dc.crs()->Clone(); 
    // OGRSpatialReference nCRS = *crs; 

    // char *pszWKT = NULL; 
    // nCRS.exportToWkt(&pszWKT); 
    // std::cout << "CRS: " << pszWKT << std::endl; 
    // CPLFree(pszWKT); 

    // nCRS.SetGeogCS( 
    //     "GCS_Moon_2000",
    //     "BELLA", 
    //     "CIAO", 
    //     1737400.0, 0.0, 
    //     "Reference Meridian", 0.0, 
    //     SRS_UA_DEGREE, atof(SRS_UA_DEGREE_CONV)
    // );

    // nCRS.exportToWkt(&pszWKT); 
    // std::cout << "CRS: " << pszWKT << std::endl; 
    // CPLFree(pszWKT); 

    // crs->exportToWkt(&pszWKT); 
    // std::cout << "CRS: " << pszWKT << std::endl; 
    // CPLFree(pszWKT); 

    // // Enqueue tasks for execution 
    // for (int j = 0; j < 1000; j++)
    // {
    //     pool.addTask([j, &dc]() {

    //         std::cout << "Task " << j << ": " << dc.pix2sph(point2(j % 2, 0.0)) << std::endl;

    //         // std::cout << "Task " << j << " is running on thread " 
    //         //      << std::this_thread::get_id() << std::endl;

    //         // // Simulate some work 
    //         // std::this_thread::sleep_for(std::chrono::milliseconds(100));  
    //     });
    // }

    // std::cout << "Starting Jobs Queue" << std::endl; 
    // pool.startPool(); 


    // // Render the image
    // auto t1 = std::chrono::high_resolution_clock::now();
    
    // std::thread::id id;
    // int n = 100000; 
    // for (int j = 0; j < n; j++)
    //     id = std::this_thread::get_id();
    
    // auto t2 = std::chrono::high_resolution_clock::now();

    // // // Write the pixels to a PPM image file
    // // toImage(cam, pixels); 

    // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    // std::clog << "The function took: " << ((double)duration.count())/n << " ns" << std::endl;


    return 0;
}