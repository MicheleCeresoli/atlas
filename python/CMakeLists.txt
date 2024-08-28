
set(PYTHON_FILES
    affine.cpp
)

pybind11_add_module(pyarcadia MODULE 
    ${PYTHON_FILES}
)

target_link_libraries(pyarcadia PUBLIC arcadialib)
target_link_libraries(pyarcadia PRIVATE GDAL::GDAL)