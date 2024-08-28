


set(HEADER_DIR ../include/arcadia)

set(HEADER_LIST 
    ${HEADER_DIR}/affine.h
    ${HEADER_DIR}/camera.h
    ${HEADER_DIR}/crsutils.h
    ${HEADER_DIR}/raster.h
    ${HEADER_DIR}/dcm.h
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
    camera.cpp
    crsutils.cpp
    raster.cpp
    dcm.cpp
    dem.cpp
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
add_library(arcadialib SHARED ${SOURCE_LIST} ${HEADER_LIST})

# Add these directories (users won't use GDAL)
target_include_directories(arcadialib PUBLIC ${HEADER_DIR})
target_include_directories(arcadialib PRIVATE ${GDAL_INCLUDE_DIR})

# Users require at least C++17
target_compile_features(arcadialib PUBLIC cxx_std_17)

# IDEs should put the headers in a nice place
source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)

# # Source 
# # set ( PYTHON_DIR python ) 

# # Include directories
# target_include_directories(arcadia PRIVATE include)
# target_include_directories(arcadia PRIVATE ${GDAL_INCLUDE_DIR})




# set (PYTHON_FILES 
#     ${PYTHON_DIR}/affine.cpp
# )


# # ----- Compiler Flags 

# if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

#     # add_compile_options(-Wconversion)
#     # add_compile_options(-Wfloat-conversion)
#     # add_compile_options(-Wsign-conversion)
#     add_compile_options(-Wmaybe-uninitialized) # Variable improperly intiialised
#     add_compile_options(-Wunused-variable) # Variable defined but unused 
#     add_compile_options(-Werror) 

# endif() 

# # Executables 
# # add_executable(arcadia ${SRC_FILES} ${HEADER_FILES})
# add_library(arcadia ${SRC_FILES} ${HEADER_FILES})

# # Include header files

# target_link_libraries(arcadia PRIVATE GDAL::GDAL)

# pybind11_add_module(arcadia ${PYTHON_FILES} ${HEADER_FILES} ${SOURCE_FILES})

# # install(TARGETS arcadia)