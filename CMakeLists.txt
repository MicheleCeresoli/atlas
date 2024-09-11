
cmake_minimum_required ( VERSION 3.15...3.27.0)

# This is to automatically update the vs-code settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(
    arcadia
    VERSION 0.2.1 
    DESCRIPTION "Ray Tracing Lunar DEM Images"
    LANGUAGES CXX)

# This command tells CMake to put the shared library in the build/lib folder
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Support folders in the IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Fetch required packages
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

find_package(GDAL REQUIRED)

find_package(OpenCV CONFIG REQUIRED)


# Include the compiled library code 
add_subdirectory(src)

# Include the Python bindings code 
add_subdirectory(python)

# Include the executable code 
add_subdirectory(apps)

# ----- Compiler Flags 
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    # add_compile_options(-Wconversion)
    # add_compile_options(-Wfloat-conversion)
    # add_compile_options(-Wsign-conversion)
    add_compile_options(-Wmaybe-uninitialized) # Variable improperly intiialised
    add_compile_options(-Wunused-variable) # Variable defined but unused 
    add_compile_options(-Werror) 

endif()