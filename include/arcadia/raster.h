#ifndef RASTERFILE_H 
#define RASTERFILE_H 

#include "affine.h"
#include "gdal_priv.h"
#include "vec2.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

/* -------------------------------------------------------
                        RASTER BAND
---------------------------------------------------------- */

class RasterBand {

    public: 

        RasterBand(std::shared_ptr<GDALDataset> pd, int i);

        // Retrieve the minimum raster value; 
        inline double min() const { return _vMin; }
        // Retrieve the maximum raster value;
        inline double max() const { return _vMax; }

        inline double offset() const { return _offset; }; 
        inline double scale() const { return _scale; }
        inline double noDataVal() const { return _noDataVal; } 

        inline bool isLoaded() const { return nLoadedElements > 0; }

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

        inline std::string getFileName() const { return filename; }
        inline std::filesystem::path getFilePath() const { return filepath; }

        inline uint width() const { return _width; }
        inline uint height() const { return _height; }
        inline size_t rasterCount() const { return _rasterCount; }; 

        /* Return the raster lowest resolution. In this case lowest means the one which 
         * expresses the lowest accuracy. For example, if it had 20m on the x-axis and 50m 
         * on the y-axis, the 50m resolution would be returned. */
        inline double resolution() const { return _resolution; }

        inline size_t nThreads() const { return _nThreads; }

        inline double top() const { return _top; }; 
        inline double bottom() const { return _bottom; }
        inline double left() const { return _left; }
        inline double right() const { return _right; }

        inline Affine getAffine() const { return transform; }
        inline Affine getInvAffine() const { return iTransform; }

        // Raster limits 

        void getLongitudeBounds(double* bounds) const; 
        void getLatitudeBounds(double* bounds) const;

        bool isWithinGeographicBounds(const point2& p) const; 

        // Raster Bands Interfaces 
        
        inline void loadBand(size_t i) { bands[i].loadData(); };
        inline void unloadBand(size_t i) { bands[i].unloadData(); }; 
        inline bool isBandLoaded(size_t i) const { return bands[i].isLoaded(); }; 

        void loadBands(); 
        void unloadBands(); 

        inline double getBandNoDataValue(uint i) const { return bands[i].noDataVal(); }

        inline double getBandData(uint u, uint v, uint i = 0) const { 
            return bands[i].getData(u, v);
        }

        inline const RasterBand* getRasterBand(uint i) const { return &bands[i]; }

        // Transformation Functions

        inline point2 pix2map(const point2& p) const { return transform*p; }
        inline point2 map2pix(const point2& m) const { return iTransform*m; }

        point2 sph2map(const point2& s, uint threadid = 0) const;
        point2 map2sph(const point2& m, uint threadid = 0) const; 

        point2 sph2pix(const point2& s, uint threadid = 0) const;
        point2 pix2sph(const point2& p, uint threadid = 0) const; 

        inline const OGRSpatialReference* crs() const { return pDataset->GetSpatialRef(); }

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
        double getData(const point2& s, bool interp, uint threadid = 0);

        inline const RasterFile* getRasterFile(size_t i) const { return &rasters[i]; }

        inline void loadRaster(size_t i) { rasters[i].loadBand(0); }
        inline void unloadRaster(size_t i) { rasters[i].unloadBand(0); }

        void loadRasters(); 
        void unloadRasters();

        void cleanupRasters(uint threshold); 

    protected:

        std::vector<RasterFile> rasters;
        double _resolution;

        // // Mutex to handle raster loading\unloading. 
        std::mutex rasterUpdateMutex;
        std::vector<uint> rastersUsed; 
        std::vector<uint> rastersFlag;

        double interpolateRaster(const point2& pix, size_t rid, int tid) const;

    private: 

        std::string displayMessage;
        void displayLoadingStatus(RasterLoadingStatus s, size_t nFiles);

};


#endif