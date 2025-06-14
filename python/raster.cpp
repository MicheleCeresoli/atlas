
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "raster.h"

namespace py = pybind11;

void init_raster(py::module_ &m) {


    py::class_<RasterDescriptor>(m, "RasterDescriptor")

        .def(py::init([](
            const std::string& filename, const std::array<double, 2>& lon_bounds, 
            const std::array<double, 2>& lat_bounds, double res, double scale, 
            double offset
        ){

            RasterDescriptor d{
                    .filename = filename, 
                    .res = res, 
                    .scale=scale, 
                    .offset=offset
                };

                d.lon_bounds[0] = lon_bounds[0];
                d.lon_bounds[1] = lon_bounds[1]; 

                d.lat_bounds[0] = lat_bounds[0];
                d.lat_bounds[1] = lat_bounds[1];

                return d;

            }), 

            py::arg("filename"), py::arg("lon_bounds"), 
            py::arg("lat_bounds"), py::arg("res"), 
            py::arg("scale") = 1.0, py::arg("offset") = 0.0
        )

        .def_readwrite("filename", &RasterDescriptor::filename)
        .def_readwrite("res", &RasterDescriptor::res)
        .def_readwrite("scale", &RasterDescriptor::scale)
        .def_readwrite("offset", &RasterDescriptor::offset);

        // .def_readwrite("lon_bounds", &RasterDescriptor::lon_bounds)
        // .def_readwrite("lat_bounds", &RasterDescriptor::lat_bounds);


    py::class_<RasterBand>(m, "RasterBand")

        .def(py::init<RasterDescriptor, std::shared_ptr<GDALDataset>, int>())

        .def("min", &RasterBand::min)
        .def("max", &RasterBand::max)
        .def("offset", &RasterBand::offset)
        .def("scale", &RasterBand::scale)
        .def("noDataVal", &RasterBand::noDataVal)
        .def("loadData", &RasterBand::loadData)
        .def("unloadData", &RasterBand::unloadData)

        .def("getData", [](RasterBand& b, ui16_t i) {
            return b.getData(i);
        })

        .def("getData", [](RasterBand& b, ui16_t u, ui16_t v) {
            return b.getData(u, v); 
        });

    py::class_<RasterFile>(m, "RasterFile")

        .def(py::init<RasterDescriptor, size_t>(), py::arg("file"), py::arg("nThreads") = 1)

        .def("getFileName", &RasterFile::getFileName)

        .def("width", &RasterFile::width)
        .def("height", &RasterFile::height)
        .def("rasterCount", &RasterFile::rasterCount)

        .def("resolution", &RasterFile::resolution)

        .def("nThreads", &RasterFile::nThreads)

        .def("top", &RasterFile::top)
        .def("bottom", &RasterFile::bottom)
        .def("left", &RasterFile::left)
        .def("right", &RasterFile::right)

        .def("getAffine", &RasterFile::getAffine)
        .def("getInvAffine", &RasterFile::getInvAffine)

        .def("getLongitudeBounds", [](RasterFile& rf){
            std::array<double, 2> bounds;
            rf.getLongitudeBounds(bounds.begin()); 
            return bounds;
        })

        .def("getLatitudeBounds", [](RasterFile& rf){
            std::array<double, 2> bounds;
            rf.getLatitudeBounds(bounds.begin()); 
            return bounds;
        })

        .def("isWithinGeographicBounds", &RasterFile::isWithinGeographicBounds)

        .def("loadBand", &RasterFile::loadBand)
        .def("loadBands", &RasterFile::loadBands)

        .def("unloadBand", &RasterFile::unloadBand)
        .def("unloadBands", &RasterFile::unloadBands)

        .def("getBandNoDataValue", &RasterFile::getBandNoDataValue)
        .def("getBandData", &RasterFile::getBandData)

        .def("getRasterBand", &RasterFile::getRasterBand, py::return_value_policy::reference)

        .def("map2pix", &RasterFile::map2pix)
        .def("pix2map", &RasterFile::pix2map)

        .def("sph2map", &RasterFile::sph2map)
        .def("map2sph", &RasterFile::map2sph)

        .def("sph2pix", &RasterFile::sph2pix)
        .def("pix2sph", &RasterFile::pix2sph);


    py::class_<RasterContainer>(m, "RasterContainer")

        .def(py::init<double, size_t>(), py::arg("res"), py::arg("nThreads") = 1)

        .def("nRasters", &RasterContainer::nRasters)
        .def("getResolution", &RasterContainer::getResolution)
        .def("getData", &RasterContainer::getData)

        .def("getRasterFile", &RasterContainer::getRasterFile, 
            py::return_value_policy::reference)

        .def("loadRaster", &RasterContainer::loadRaster)
        .def("unloadRaster", &RasterContainer::unloadRaster)

        .def("loadRasters", &RasterContainer::loadRasters)
        .def("unloadRasters", &RasterContainer::unloadRasters)
        
        .def("cleanupRasters", &RasterContainer::cleanupRasters); 

    py::class_<RasterManager>(m, "RasterManager")

        .def(py::init<RasterDescriptor, size_t, bool>(), 
             py::arg("file"), py::arg("nThreads") = 1, py::arg("displayInfo") = false)

        .def(py::init<std::vector<RasterDescriptor>, size_t, bool>(), 
             py::arg("files"), py::arg("nThreads") = 1, py::arg("displayInfo") = false)

        .def("nRasters", &RasterManager::nRasters)
        .def("nContainers", &RasterManager::nContainers)

        .def("getMinResolution", &RasterManager::getMinResolution)
        .def("getMaxResolution", &RasterManager::getMaxResolution)
        .def("getResolutions", &RasterManager::getResolutions)

        .def("getData", &RasterManager::getData)
        .def("getLastResolution", &RasterManager::getLastResolution)

        .def("getRasterContainer", &RasterManager::getRasterContainer, 
            py::return_value_policy::reference)

        .def("loadRasters", &RasterManager::loadRasters)
        .def("unloadRasters", &RasterManager::unloadRasters)
        
        .def("cleanupRasters", &RasterManager::cleanupRasters); 

}
