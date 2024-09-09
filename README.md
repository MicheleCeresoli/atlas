
# ARCADIA 

_Ray Tracing Lunar DEM Images_

A simple ray tracer C++ implementation for Lunar DEM images.


## Installation 

This package can be installed by manually building it on your system or by downloading the appropriate python wheel depending on your environment. In both cases, the package requires the GDAL and OpenCV libraries to be already installed on your system. If they are not available, they can be installed via the `apt install libgdal-dev` and `apt install libopencv-dev` commands.

The Python wheels can be found under the latest published release.

### Manual Library Build
Manually building the library requires a C++ compiler and a CMake version greater or equal to 3.15. Additionally, the GDAL and OpenCV library must already be installed. Then open a terminal in this folder and type the command `pip install .` to install the python package in your current active environment.

## Troubleshooting

### GLIBCXX 

If when importing the `arcadia` module into your python script you get an error stating `/lib/libstdc++.so.6: version GLIBCXX_3.4.32 not found`, it means that the library was built against a `GLIBCXX` version that is not listed in your runtime system's `libstdc++`. 

If the import is trying to use the miniconda's `libstdc++`, the problem can be solved by running either one of these two commands:

```
conda install -c conda-forge gcc=X.X
```
```
conda install -c conda-forge libstdcxx-ng=Y
```

where `X` and `Y` are the package versions that ensure the desired GLIBCXX version is available. Another solution involves linking with 
the system library via: 

```
ln -sf /usr/lib/x86_64-linux-gnu/libstdc++.so.6 ${CONDA_PREFIX}/lib/libstdc++.so.6
```