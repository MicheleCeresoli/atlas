#ifndef RasterFile_H 
#define RasterFile_H 

#include "affine.h"
#include "gdal_priv.h"
#include "vec2.h"

#include <filesystem>
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

        // Load/unload all the data inside the raster band
        void loadData(); 
        void unloadData();
        
        // Get a given pixel 
        double getData(uint i) const;
        double getData(uint u, uint v) const;  


    private: 

        // We can't make this a shared_ptr because when the band is destroyed
        // it interferes with the original GDALDataset that container it, i guess..
        GDALRasterBand* pBand;
        std::shared_ptr<float> data;

        uint _xBlock, _yBlock;      
        uint _width, _height; 

        double _vMin, _vMax; 
        double _offset, _scale; 

        double _noDataVal;

        uint nLoadedElements = 0; 
};



/* -------------------------------------------------------
                    RASTER FILE 
---------------------------------------------------------- */


class RasterFile {

    public: 

        RasterFile(const std::string& file, size_t nThreads = 1);

        std::string getFileName() const; 
        std::filesystem::path getFilePath() const; 

        inline uint width() const { return _width; }
        inline uint height() const { return _height; }
        inline size_t rasterCount() const { return _rasterCount; }; 

        double resolution() const; 

        size_t nThreads() const; 

        double top() const; 
        double bottom() const; 
        double left() const; 
        double right() const; 

        Affine getAffine() const; 
        Affine getInvAffine() const; 

        // Raster limits 

        void getLongitudeBounds(double* bounds) const; 
        void getLatitudeBounds(double* bounds) const;

        bool isWithinGeographicBounds(const point2& p) const; 

        // Raster Bands Interfaces 
        
        void loadBand(size_t i);
        void loadBands(); 

        void unloadBand(size_t i); 
        void unloadBands(); 

        double getBandNoDataValue(uint bandid) const;
        double getBandData(uint u, uint v, uint bandid = 0) const; 

        const RasterBand* getRasterBand(uint i) const;

        // Transformation Functions

        point2 map2pix(const point2& m) const;
        point2 pix2map(const point2& p) const;

        point2 sph2map(const point2& s, uint threadid = 0) const;
        point2 map2sph(const point2& m, uint threadid = 0) const; 

        point2 sph2pix(const point2& s, uint threadid = 0) const;
        point2 pix2sph(const point2& p, uint threadid = 0) const; 

        const OGRSpatialReference* crs() const;

    private: 

        std::filesystem::path filepath;
        std::shared_ptr<GDALDataset> pDataset;

        std::string filename; 

        size_t _nThreads; // Number of assigned threads

        uint _width;  // Raster width
        uint _height; // Raster height
        size_t _rasterCount;  // Raster bands

        double _top, _bottom; 
        double _left, _right; 

        double _resolution;     

        double lon_bounds[2];  // Raster longitude bounds
        double lat_bounds[2];  // Raster latitude bounds

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

        void computeRasterBounds(); 

};



/* -------------------------------------------------------
                    RASTER CONTAINER
---------------------------------------------------------- */

enum class RasterLoadingStatus {
    LOADING, 
    COMPLETED
};

class RasterContainer {

    public: 

        RasterContainer(
            std::string filename, size_t nThreads = 1, bool displayInfo = false
        ); 

        RasterContainer(
            std::vector<std::string> files, 
            size_t nThreads = 1, 
            bool displayInfo = false
        );

        virtual ~RasterContainer() = default;

        inline size_t nRasters() const { return rasters.size(); }; 

        inline double getResolution() const { return _resolution; }; 
        double getData(const point2& s, bool interp, uint threadid = 0) const;

        const RasterFile* getRasterFile(size_t i) const; 

        void loadRaster(size_t i); 
        void unloadRaster(size_t i); 

        void loadRasters(); 
        void unloadRasters(); 

    protected:

        std::vector<RasterFile> rasters;
        double _resolution;

        double interpolateRaster(const point2& pix, size_t rid, int tid) const;

    private: 

        std::string displayMessage;
        void displayLoadingStatus(RasterLoadingStatus s, size_t nFiles);

};


#endif