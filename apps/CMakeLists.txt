
add_executable(arcadiapp main.cpp)
target_compile_features(arcadiapp PRIVATE cxx_std_17)

target_link_libraries(arcadiapp PRIVATE arcadia)
target_link_libraries(arcadiapp PRIVATE GDAL::GDAL)
target_link_libraries(arcadiapp PRIVATE ${OpenCV_LIBS})