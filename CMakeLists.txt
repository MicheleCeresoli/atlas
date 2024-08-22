
cmake_minimum_required ( VERSION 3.1.0...3.27.0)

project ( RAYTRACING_DEMO LANGUAGES CXX )


# Source 
set ( HEADER_DIR include )
set ( SOURCE_DIR src )

set ( HEADER_FILES 
    ${HEADER_DIR}/camera.h
    ${HEADER_DIR}/color.h
    ${HEADER_DIR}/dcm.h
    ${HEADER_DIR}/pixel.h
    ${HEADER_DIR}/pool.h
    ${HEADER_DIR}/ray.h
    ${HEADER_DIR}/renderer.h
    ${HEADER_DIR}/utils.h
    ${HEADER_DIR}/world.h
    ${HEADER_DIR}/vec3.h
)

set ( SRC_FILES 
    ${SOURCE_DIR}/ex1.cpp
    ${SOURCE_DIR}/camera.cpp
    ${SOURCE_DIR}/color.cpp
    ${SOURCE_DIR}/dcm.cpp
    ${SOURCE_DIR}/pool.cpp
    ${SOURCE_DIR}/ray.cpp
    ${SOURCE_DIR}/utils.cpp
    ${SOURCE_DIR}/world.cpp
    ${SOURCE_DIR}/vec3.cpp
)

include_directories(${HEADER_DIR})

# ----- Compiler Flags 

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    add_compile_options(-Wmaybe-uninitialized) # Variable improperly intiialised
    add_compile_options(-Wunused-variable) # Variable defined but unused 
    add_compile_options(-Werror) 

endif() 

# Executables 
add_executable(ex1 ${SRC_FILES} ${HEADER_FILES})


