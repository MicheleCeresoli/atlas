
set(PYTHON_FILES
    pyarcadia.cpp
    affine.cpp
    camera.cpp
    dcm.cpp
    dem.cpp
    pixel.cpp
    raster.cpp
    ray.cpp
    renderer.cpp
    settings.cpp
    vec2.cpp
    vec3.cpp
    world.cpp
)

pybind11_add_module(pyarcadia MODULE 
    ${PYTHON_FILES}
)

target_link_libraries(pyarcadia PUBLIC arcadialib)
target_link_libraries(pyarcadia PRIVATE GDAL::GDAL)