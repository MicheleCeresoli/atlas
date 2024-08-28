
cmake_minimum_required ( VERSION 3.1.0...3.27.0)

project ( RAYTRACING_DEMO LANGUAGES CXX )

find_package(GDAL CONFIG)
if (NOT GDAL_FOUND) 
    find_package(GDAL REQUIRED)
endif()

# This is to automatically update the vs-code settings
set ( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set ( CMAKE_CXX_STANDARD 17)
set ( CMAKE_CXX_STANDARD_REQUIRED ON)

# Source 
set ( HEADER_DIR include )
set ( SOURCE_DIR src )

set ( HEADER_FILES 
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

set ( SRC_FILES 
    ${SOURCE_DIR}/affine.cpp
    ${SOURCE_DIR}/camera.cpp
    ${SOURCE_DIR}/crsutils.cpp
    ${SOURCE_DIR}/raster.cpp
    ${SOURCE_DIR}/dcm.cpp
    ${SOURCE_DIR}/dem.cpp
    ${SOURCE_DIR}/pixel.cpp
    ${SOURCE_DIR}/pool.cpp
    ${SOURCE_DIR}/ray.cpp
    ${SOURCE_DIR}/renderer.cpp
    ${SOURCE_DIR}/settings.cpp
    ${SOURCE_DIR}/utils.cpp
    ${SOURCE_DIR}/world.cpp
    ${SOURCE_DIR}/vec2.cpp
    ${SOURCE_DIR}/vec3.cpp
    
    ${SOURCE_DIR}/main.cpp
)



# ----- Compiler Flags 

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    # add_compile_options(-Wconversion)
    # add_compile_options(-Wfloat-conversion)
    # add_compile_options(-Wsign-conversion)
    add_compile_options(-Wmaybe-uninitialized) # Variable improperly intiialised
    add_compile_options(-Wunused-variable) # Variable defined but unused 
    add_compile_options(-Werror) 

endif() 

# Executables 
add_executable(arcadia ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(arcadia PRIVATE GDAL::GDAL)

# Include header files
target_include_directories(arcadia PRIVATE ${HEADER_DIR})
target_include_directories(arcadia PRIVATE ${GDAL_INCLUDE_DIR})
