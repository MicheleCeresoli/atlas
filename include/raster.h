#ifndef RasterFile_H 
#define RasterFile_H 

#include "affine.h"
#include "gdal_priv.h"

#include <memory>
#include <string>
#include <vector>

/* -------------------------------------------------------
                        RASTER BAND
---------------------------------------------------------- */

class RasterBand {

    public: 

        RasterBand(std::shared_ptr<GDALDataset> pd, int i);

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

        // We can't make this a shared_ptr because when the band is destroyed
        // it interferes with the original GDALDataset that container it, i guess..
        GDALRasterBand* pBand;
        std::shared_ptr<float> data;

        int _xBlock, _yBlock;      
        int _width, _height; 

        double _vMin, _vMax; 
        double _offset, _scale; 

        double _noDataVal;

        int nLoadedElements = 0; 
};



/* -------------------------------------------------------
                    RASTER CONTAINER 
---------------------------------------------------------- */


class RasterFile {

    public: 

        RasterFile(const std::string& filename, int nThreads = 1);

        std::string getFilename() const; 

        int width() const; 
        int height() const; 
        int rasterCount() const; 

        int nThreads() const; 

        double top() const; 
        double bottom() const; 
        double left() const; 
        double right() const; 

        Affine getAffine() const; 
        Affine getInvAffine() const; 

        // Raster Bands Interfaces 
        
        void loadBand(int i);
        void loadBands(); 

        double getBandData(int u, int v, int bandid = 0) const; 

        const RasterBand* getRasterBand(int i) const;

        // Transformation Functions

        point2 map2pix(const point2& m) const;
        point2 pix2map(const point2& p) const;

        point2 sph2map(const point2& s, int threadid = 0) const;
        point2 map2sph(const point2& m, int threadid = 0) const; 

        point2 sph2pix(const point2& s, int threadid = 0) const;
        point2 pix2sph(const point2& p, int threadid = 0) const; 

        const OGRSpatialReference* crs() const;

    private: 

        std::string filename;
        std::shared_ptr<GDALDataset> pDataset;

        int _nThreads; // Number of assigned threads

        int _width;  // Raster width
        int _height; // Raster height
        int _rasterCount;  // Raster bands

        double _top, _bottom; 
        double _left, _right; 

        Affine transform;  // Affine dataset transformation.
        Affine iTransform; // Inverse affine transformation.

        // A vector is used to store coordinate transformation objects so that each 
        // thread can safely access its own object.

        // // Transformation between a geographic (long\lat) and the projected system
        std::vector<std::shared_ptr<OGRCoordinateTransformation>> s2mT;

        // Inverse transformation (from projected to geographic) 
        std::vector<std::shared_ptr<OGRCoordinateTransformation>> m2sT; 

        // Store raster bands 
        std::vector<RasterBand> bands;

        void updateReferenceSystem(); 

        void setupTransformations(); 

};

#endif