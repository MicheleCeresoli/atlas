
add_executable(arcadia main.cpp)
target_compile_features(arcadia PRIVATE cxx_std_17)

target_link_libraries(arcadia PRIVATE arcadiacxx)
target_link_libraries(arcadia PRIVATE GDAL::GDAL)
target_link_libraries(arcadia PRIVATE ${OpenCV_LIBS})