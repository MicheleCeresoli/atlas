
# ARCADIA 

_Ray Tracing Lunar DEM Images_

A simple ray tracer C++ implementation for Lunar DEM images.


## Installation 

To install this library you need to have CMake 3.15 or greater installed on your system. You should also have the opencv2, gdal and pybind11 libraries properly installed. 

Then open a terminal in this folder and type the command `pip install .`
to install the python package in your current active environment.

## Troubleshooting

### GLIBCXX 

If when importing the `arcadia` module into your python script you get an error stating `/lib/libstdc++.so.6: version GLIBCXX_3.4.32 not found`, it means that the library was built against a `GLIBCXX` version that is not listed in your runtime system's `libstdc++`. If the import is trying to use the miniconda's `libstdc++`, the problem can be solved by running 

```
conda install -c conda-forge gcc=X.X
```

or 

```
conda install -c conda-forge libstdcxx-ng=Y
```

where `X` and `Y` are the package versions that ensure the desired GLIBCXX version is available.