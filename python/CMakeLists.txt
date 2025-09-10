
set(PYTHON_FILES
    pyatlas.cpp
    affine.cpp
    atlas.cpp
    camera.cpp
    dcm.cpp
    dem.cpp
    dom.cpp
    grid.cpp
    pixel.cpp
    raster.cpp
    ray.cpp
    renderer.cpp
    settings.cpp
    vec2.cpp
    vec3.cpp
    world.cpp
)

pybind11_add_module(_atlas MODULE ${PYTHON_FILES})

target_link_libraries(_atlas PUBLIC atlas)
target_link_libraries(_atlas PRIVATE GDAL::GDAL)
target_link_libraries(_atlas PRIVATE ${OpenCV_LIBS})

# This is passing the version 
target_compile_definitions(_atlas PRIVATE VERSION_INFO=${PROJECT_VERSION})

# Set the RPATH (runtime library search path) for the _atlas target to 
# make sure it looks for the libatlas.so file in its own directory
set_target_properties(_atlas PROPERTIES 
        BUILD_WITH_INSTALL_RPATH TRUE 
        INSTALL_RPATH "$ORIGIN"
        INSTALL_RPATH_USE_LINK_PATH TRUE
)


# This installs the python shared library in the same wheel directory
install(TARGETS _atlas 
        LIBRARY DESTINATION atlas)
