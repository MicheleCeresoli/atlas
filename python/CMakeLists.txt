
set(PYTHON_FILES
    pyarcadia.cpp
    affine.cpp
    arcadia.cpp
    camera.cpp
    dcm.cpp
    dem.cpp
    dom.cpp
    pixel.cpp
    raster.cpp
    ray.cpp
    renderer.cpp
    settings.cpp
    vec2.cpp
    vec3.cpp
    world.cpp
)

pybind11_add_module(_arcadia MODULE ${PYTHON_FILES})

target_link_libraries(_arcadia PUBLIC arcadia)
target_link_libraries(_arcadia PRIVATE GDAL::GDAL)
target_link_libraries(_arcadia PRIVATE ${OpenCV_LIBS})

# This is passing the version 
target_compile_definitions(_arcadia PRIVATE VERSION_INFO=${PROJECT_VERSION})

# Set the RPATH (runtime library search path) for the _arcadia target to 
# make sure it looks for the libarcadia.so file in its own directory
set_target_properties(_arcadia PROPERTIES 
        BUILD_WITH_INSTALL_RPATH TRUE 
        INSTALL_RPATH "$ORIGIN"
        INSTALL_RPATH_USE_LINK_PATH TRUE
)


# This installs the python shared library in the same wheel directory
install(TARGETS _arcadia 
        LIBRARY DESTINATION arcadia)
