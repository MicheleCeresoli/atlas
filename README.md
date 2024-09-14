
# ATLAS 

_Ray Tracing Lunar DEM Images_

ATLAS is a C++ library with Python bindings that allows to generate highly realistic optical images, DEM measurements and depth maps via ray-tracing techniques. It is designed around [Chang'E-2](https://moon.bao.ac.cn/ce5web/searchOrder-ce2En.do) Digital Elevation Model (DEM) and Digital Orthophoto Model (DOM) products.


## Installation 

This package can either be installed by manually building the library on your system or by downloading the appropriate python wheel from the latest published package release. In both cases, except when using a Conda package, ATLAS requires the GDAL and OpenCV libraries to be installed. If they are not available, they can be installed with:

```
sudo apt install libgdal-dev
```
```
sudo apt install libopencv-dev
```

### From Conda Package 
From the latest release, download the appropriate `.tar.bz2` package according to your python version and system build. Then, once the conda environment has been activated, run:

```
conda install gdal opencv pyyaml
conda install ./{NAME}.tar.bz2
```

where NAME is the filename of the target conda package. Note that the first command manually installs the required dependencies since Conda environments are isolated and don't see the system-level libraries.

### From Python Wheel (pip)
Once a Python wheel has been downloaded, to install the ATLAS module in the current active environment simply run: `pip install {NAME}.whl`, where NAME is the filename of the target python wheel.

### From Sources
Manually building the library requires a C++ compiler and a CMake version greater or equal to 3.15. The GDAL and OpenCV libraries must already be installed, as well as the _pybind11_ package. To properly work, CMake requires a `pybind11Config.cmake` file which is automatically installed only if the package is installed via conda, i.e., with:

```
conda install pybind11 -c conda-forge
```

Then, open a terminal in this folder and build the library with: 
```
mkdir build 
cd build 
cmake --build .
```
Finally, activate your target python environment, and in the root directory of the package type `pip install .`

## Troubleshooting

### GLIBCXX 

If when importing the `atlas` module into your python script you get an error stating `/lib/libstdc++.so.6: version GLIBCXX_3.4.32 not found`, it means that the library was built against a `GLIBCXX` version that is not listed in your runtime system's `libstdc++`. 

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