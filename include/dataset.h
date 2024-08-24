#ifndef DATASETCONTAINER_H 
#define DATASETCONTAINER_H 

#include "affine.h"
#include "pool.h"
#include "vec2.h"

#include "gdal_priv.h"

#include <string>
#include <vector>

/* -------------------------------------------------------
                        RASTER BAND
---------------------------------------------------------- */

class RasterBand {

    public: 

        RasterBand(GDALDataset* pd, int i);
        ~RasterBand();

        // Retrieve the minimum raster value; 
        double min() const; 
        // Retrieve the maximum raster value;
        double max() const; 

        double offset() const; 
        double scale() const;
        double noDataVal() const; 

        // Read all the data inside the raster band
        void loadData(); 
        
        // Get a given pixel 
        double getData(int i) const;
        double getData(int u, int v) const;  

    private: 

        GDALRasterBand* pBand;

        int _xBlock, _yBlock;      
        int _width, _height; 

        double _vMin, _vMax; 
        double _offset, _scale; 

        double _noDataVal;

        float* data; 
        int nLoadedElements = 0; 
        bool loaded = false;
};



/* -------------------------------------------------------
                    DATASET CONTAINER 
---------------------------------------------------------- */


class DatasetContainer {

    public: 

        DatasetContainer(std::string filename, int nThreads = 1);
        DatasetContainer(const char* filename, int nThreads = 1);

        ~DatasetContainer(); // Destructor

        const char* getFilename() const; 

        int width() const; 
        int height() const; 
        int rasterCount() const; 

        double top() const; 
        double bottom() const; 
        double left() const; 
        double right() const; 

        const RasterBand* getRasterBand(int i) const;

        Affine getAffine() const; 
        Affine getInvAffine() const; 

        // Transformation Functions

        point2 map2pix(const point2& m) const;
        point2 pix2map(const point2& p) const;

        point2 sph2map(const point2& s) const;

        point2 sph2map(const point2& s, int threadid = 1) const;
        point2 map2sph(const point2& m, int threadid = 1) const; 

        point2 sph2pix(const point2& s, int threadid = 1) const;
        point2 pix2sph(const point2& p, int threadid = 1) const; 

        const OGRSpatialReference* crs() const; 

    private: 

        const char* filename; 
        GDALDataset* pDataset;

        int _width;  // Raster width
        int _height; // Raster height
        int _rasterCount;  // Raster bands

        double _top, _bottom; 
        double _left, _right; 

        Affine transform;  // Affine dataset transformation.
        Affine iTransform; // Inverse affine transformation.

        // A vector is used to store coordinate transformation objects so that each 
        // thread can safely access its own object.

        // Transformation between a geographic (long\lat) and the projected system
        std::vector<OGRCoordinateTransformation*> s2mT;
        // Inverse transformation (from projected to geographic) 
        std::vector<OGRCoordinateTransformation*> m2sT; 

        // Store raster bands 
        std::vector<RasterBand> bands;

};

#endif