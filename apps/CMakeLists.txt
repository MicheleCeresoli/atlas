
add_executable(atlasapp main.cpp)
target_compile_features(atlasapp PRIVATE cxx_std_17)

target_link_libraries(atlasapp PRIVATE atlas)
target_link_libraries(atlasapp PRIVATE GDAL::GDAL)
target_link_libraries(atlasapp PRIVATE ${OpenCV_LIBS})