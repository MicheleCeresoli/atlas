
#include <pybind11/pybind11.h> 
#include <pybind11/numpy.h>
#include <pybind11/stl.h> 

#include "raster.h"

namespace py = pybind11;

void init_raster(py::module_ &m) {

    py::class_<RasterBand>(m, "RasterBand")

        .def(py::init<std::shared_ptr<GDALDataset>, int>())

        .def("min", &RasterBand::min)
        .def("max", &RasterBand::max)
        .def("offset", &RasterBand::offset)
        .def("scale", &RasterBand::scale)
        .def("noDataVal", &RasterBand::noDataVal)
        .def("loadData", &RasterBand::loadData)
        .def("unloadData", &RasterBand::unloadData)

        .def("getData", [](RasterBand& b, uint i) {
            return b.getData(i);
        })

        .def("getData", [](RasterBand& b, uint u, uint v) {
            return b.getData(u, v); 
        });

    py::class_<RasterFile>(m, "RasterFile")
        .def(py::init<std::string, size_t>(), py::arg("file"), py::arg("nThreads") = 1)

        .def("getFileName", &RasterFile::getFileName)
        .def("getFilePath", &RasterFile::getFilePath)

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

        .def("map2pix", &RasterFile::map2pix)
        .def("pix2map", &RasterFile::pix2map)

        .def("sph2map", &RasterFile::sph2map)
        .def("map2sph", &RasterFile::map2sph)

        .def("sph2pix", &RasterFile::sph2pix)
        .def("pix2sph", &RasterFile::pix2sph)

        ;
}