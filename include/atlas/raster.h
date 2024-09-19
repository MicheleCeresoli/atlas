#ifndef RASTERFILE_H 
#define RASTERFILE_H 

#include "affine.h"
#include "gdal_priv.h"
#include "types.h"
#include "vec2.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

/* -------------------------------------------------------
                        RASTER BAND
---------------------------------------------------------- */

/**
 * @class RasterBand
 * @brief Class representing a single data band of a raster file.
 */
class RasterBand {

    public: 

        /**
         * @brief Construct a new RasterBand object from a target raster file.
         * 
         * @param pd Pointer to the raster file dataset.
         * @param i Target raster band. The indexing follows a one-based notation.
         * 
         * @note Creating a raster band object does NOT automatically load its underlying 
         * data into memory. 
         */
        RasterBand(std::shared_ptr<GDALDataset> pd, int i);

        // Retrieve the minimum raster value; 

        /**
         * @brief Return the minimum value for this band.
         * @details For file formats that don't know this value instrinsically, the minimum 
         * value is computed by analysing all the underlying data.
         * 
         * @return double Minimum raster value (exluding no data pixels).
         */
        inline double min() const { return _vMin; }
        
        /**
         * @brief Return the maximum value for this band.
         * @details For file formats that don't know this value instrinsically, the minimum 
         * value is computed by analysing all the underlying data.
         * 
         * @return double Maximum raster value (excluding no data pixels).
         */
        inline double max() const { return _vMax; }

        /**
         * @brief Return the raster offset value. 
         * @details This value (in combination with the scale() value) can be used to 
         * transform raw pixel values into the target units, according to: 
         * 
         * Units value = (raw value * scale) + offset
         * 
         * @return double Raster offset.
         */
        inline double offset() const { return _offset; }; 

        /**
         * @brief Return the raster scale value.
         * @details This value (in combination with the offset() value) can be used to 
         * transform raw pixel values into the target units, according to: 
         * 
         * Units value = (raw value * scale) + offset
         * 
         * @return double Raster scale.
         */
        inline double scale() const { return _scale; }

        /**
         * @brief Return the no data value for this raster band.
         * @details The no data value for a band is a special marker value that is used to 
         * identify pixels that are not valid data. 
         * 
         * @return double no data value.
         * 
         * @note Currently, this function is designed for the Chang-E'2 raster files, which 
         * use a NoData value of -99999. However, this value is manually assigned because 
         * the rasters do not contain such information. If one wishes to support other 
         * kinds of raster data products, this function shall be modified accordingly. 
         */
        inline double noDataVal() const { return _noDataVal; } 

        /**
         * @brief Return true if the underlying data has been loaded into memory.
         */
        inline bool isLoaded() const { return nLoadedElements > 0; }

        /**
         * @brief Load the raster band data into memory.
         */
        void loadData(); 

        /**
         * @brief Unload the raster band data from memory.
         */
        void unloadData();

        /**
         * @brief Get the raster data associated to a given index.
         * @details Return the data, in physical units (i.e., after applying the offset 
         * and scale values) corresponding to a given element. 
         * 
         * @param i Element index, in zero-based notation.
         * @return double Target element data value.
         */
        double getData(ui32_t i) const;

        /**
         * @brief Get the raster data associated to given map coordinates.
         * @details Return the data, in physical units (i.e., after applying the offset 
         * and scale values) corresponding to the given pixel coordinates.
         * 
         * @param u Horizontal pixel coordinates.
         * @param v Vertical pixel coordinates.
         * @return double Target pixel data value.
         */
        double getData(ui32_t u, ui32_t v) const;  


    private: 

        // We can't make this a shared_ptr because when the band is destroyed
        // it interferes with the original GDALDataset that container it, i guess..
        GDALRasterBand* pBand;
        std::shared_ptr<float> data;

        ui32_t _xBlock, _yBlock;      
        ui32_t _width, _height; 

        double _vMin, _vMax; 
        double _offset, _scale; 

        double _noDataVal;

        ui32_t nLoadedElements = 0; 
};



/* -------------------------------------------------------
                    RASTER FILE 
---------------------------------------------------------- */


class RasterFile {

    public: 

        RasterFile(const std::string& file, size_t nThreads = 1);

        inline std::string getFileName() const { return filename; }
        inline std::filesystem::path getFilePath() const { return filepath; }

        inline ui32_t width() const { return _width; }
        inline ui32_t height() const { return _height; }
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

        inline double getBandNoDataValue(ui32_t i) const { return bands[i].noDataVal(); }

        inline double getBandData(ui32_t u, ui32_t v, ui32_t i = 0) const { 
            return bands[i].getData(u, v);
        }

        inline const RasterBand* getRasterBand(ui32_t i) const { return &bands[i]; }

        // Transformation Functions

        inline point2 pix2map(const point2& p) const { return transform*p; }
        inline point2 map2pix(const point2& m) const { return iTransform*m; }

        point2 sph2map(const point2& s, ui32_t threadid = 0) const;
        point2 map2sph(const point2& m, ui32_t threadid = 0) const; 

        point2 sph2pix(const point2& s, ui32_t threadid = 0) const;
        point2 pix2sph(const point2& p, ui32_t threadid = 0) const; 

        inline const OGRSpatialReference* crs() const { return pDataset->GetSpatialRef(); }

    private: 

        std::filesystem::path filepath;
        std::shared_ptr<GDALDataset> pDataset;

        std::string filename; 

        size_t _nThreads; // Number of assigned threads

        ui32_t _width;  // Raster width
        ui32_t _height; // Raster height
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
            std::string filename, size_t nThreads = 1, bool displayLogs = false
        ); 

        RasterContainer(
            std::vector<std::string> files, 
            size_t nThreads = 1, 
            bool displayLogs = false
        );

        virtual ~RasterContainer() = default;

        inline size_t nRasters() const { return rasters.size(); }; 

        inline double getResolution() const { return _resolution; }; 
        double getData(const point2& s, bool interp, ui32_t threadid = 0);

        inline const RasterFile* getRasterFile(size_t i) const { return &rasters[i]; }

        inline void loadRaster(size_t i) { rasters[i].loadBand(0); }
        inline void unloadRaster(size_t i) { rasters[i].unloadBand(0); }

        void loadRasters(); 
        void unloadRasters();

        void cleanupRasters(ui32_t threshold); 

    protected:

        std::vector<RasterFile> rasters;
        double _resolution;

        // // Mutex to handle raster loading\unloading. 
        std::mutex rasterUpdateMutex;
        std::vector<ui8_t> rastersUsed; 
        std::vector<ui8_t> rastersFlag;

        double interpolateRaster(const point2& pix, size_t rid) const;

    private: 

        std::string displayMessage;
        void displayLoadingStatus(RasterLoadingStatus s, size_t nFiles);

};


#endif