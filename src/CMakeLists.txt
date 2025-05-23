


set(HEADER_DIR ../include/atlas)

set(HEADER_LIST 
    ${HEADER_DIR}/affine.h
    ${HEADER_DIR}/atlas.h
    ${HEADER_DIR}/camera.h
    ${HEADER_DIR}/crsutils.h
    ${HEADER_DIR}/raster.h
    ${HEADER_DIR}/dcm.h
    ${HEADER_DIR}/dom.h
    ${HEADER_DIR}/dem.h
    ${HEADER_DIR}/pixel.h
    ${HEADER_DIR}/pool.h
    ${HEADER_DIR}/ray.h
    ${HEADER_DIR}/renderer.h
    ${HEADER_DIR}/settings.h
    ${HEADER_DIR}/utils.h
    ${HEADER_DIR}/world.h
    ${HEADER_DIR}/vec2.h
    ${HEADER_DIR}/vec3.h
)

set(SOURCE_LIST
    affine.cpp
    atlas.cpp
    camera.cpp
    crsutils.cpp
    raster.cpp
    dcm.cpp
    dem.cpp
    dom.cpp
    pixel.cpp
    pool.cpp
    ray.cpp
    renderer.cpp
    settings.cpp
    utils.cpp
    world.cpp
    vec2.cpp
    vec3.cpp
)

# Make a library - static or dynamic based on user settings 
add_library(atlas SHARED ${SOURCE_LIST} ${HEADER_LIST})

# Add these directories (users won't use GDAL)
target_include_directories(atlas PUBLIC ${HEADER_DIR})
target_include_directories(atlas PRIVATE ${GDAL_INCLUDE_DIR})
target_include_directories(atlas PRIVATE ${OpenCV_INCLUDE_DIRS})

# Users require at least C++17
target_compile_features(atlas PUBLIC cxx_std_17)

# Set library version
set_target_properties(atlas PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# IDEs should put the headers in a nice place
source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)

# This allows to install the libatlas.so in the python wheel directory
install(TARGETS atlas 
        LIBRARY DESTINATION atlas)