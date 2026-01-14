
# Manual Installation

When manually installing the library on Linux without access to `apt`, please follow the instructions below.

## 1. Directory & Tool Setup
Define your root directory and create the folder structure. Replace `<user>` with your actual username. This creates a local folder in which the libraries are installed. If an independent folder already exists, `LOCAL_ROOT` should be updated accordingly.

```bash
export LOCAL_ROOT="/staff/<user>/local"

mkdir -p $LOCAL_ROOT/src
mkdir -p $LOCAL_ROOT/openssl
mkdir -p $LOCAL_ROOT/cmake
mkdir -p $LOCAL_ROOT/zstd
mkdir -p $LOCAL_ROOT/sqlite
mkdir -p $LOCAL_ROOT/libtiff
mkdir -p $LOCAL_ROOT/libcurl
mkdir -p $LOCAL_ROOT/proj
mkdir -p $LOCAL_ROOT/gdal
```

## 2. Base Utilities (Security & Build Tools)

### OpenSSL

```bash
cd $LOCAL_ROOT/src
wget [https://github.com/openssl/openssl/releases/download/openssl-3.4.0/openssl-3.4.0.tar.gz](https://github.com/openssl/openssl/releases/download/openssl-3.4.0/openssl-3.4.0.tar.gz)
tar -zxvf openssl-3.4.0.tar.gz && cd openssl-3.4.0
./config --prefix=$LOCAL_ROOT/openssl --openssldir=$LOCAL_ROOT/openssl
make -j$(nproc) && make install
```

### CMake

```bash
cd $LOCAL_ROOT/src
wget [https://github.com/Kitware/CMake/releases/download/v3.31.3/cmake-3.31.3.tar.gz](https://github.com/Kitware/CMake/releases/download/v3.31.3/cmake-3.31.3.tar.gz)
tar -zxvf cmake-3.31.3.tar.gz && cd cmake-3.31.3
./bootstrap --prefix=$LOCAL_ROOT/cmake -- -DCMAKE_USE_OPENSSL=ON -DOPENSSL_ROOT_DIR=$LOCAL_ROOT/openssl
make -j$(nproc) && make install
export PATH=$LOCAL_ROOT/cmake/bin:$PATH
```


## 3. Dependency Stack

### Zstd (Compression)

```bash
cd $LOCAL_ROOT/src
wget [https://github.com/facebook/zstd/releases/download/v1.5.6/zstd-1.5.6.tar.gz](https://github.com/facebook/zstd/releases/download/v1.5.6/zstd-1.5.6.tar.gz)
tar -zxvf zstd-1.5.6.tar.gz && cd zstd-1.5.6
make -j$(nproc) PREFIX=$LOCAL_ROOT/zstd install
```

### SQLite 3 (With R-Tree Support)

```bash
cd $LOCAL_ROOT/src
wget [https://www.sqlite.org/2025/sqlite-autoconf-3510200.tar.gz](https://www.sqlite.org/2025/sqlite-autoconf-3510200.tar.gz)
tar -zxvf sqlite-autoconf-3510200.tar.gz && cd sqlite-autoconf-3510200
CPPFLAGS="-DSQLITE_ENABLE_RTREE=1" ./configure --prefix=$LOCAL_ROOT/sqlite
make -j$(nproc) && make install
```

### LibTIFF

```bash
cd $LOCAL_ROOT/src
wget [https://download.osgeo.org/libtiff/tiff-4.7.0.tar.gz](https://download.osgeo.org/libtiff/tiff-4.7.0.tar.gz)
tar -zxvf tiff-4.7.0.tar.gz && cd tiff-4.7.0
./configure --prefix=$LOCAL_ROOT/libtiff \
            --with-zstd-lib=$LOCAL_ROOT/zstd/lib \
            --with-zstd-include=$LOCAL_ROOT/zstd/include
make -j$(nproc) && make install
```

### Libcurl (Network/Cloud Support)

```bash
cd $LOCAL_ROOT/src
wget [https://curl.se/download/curl-8.11.1.tar.gz](https://curl.se/download/curl-8.11.1.tar.gz)
tar -zxvf curl-8.11.1.tar.gz && cd curl-8.11.1
./configure --prefix=$LOCAL_ROOT/libcurl --with-openssl=$LOCAL_ROOT/openssl
make -j$(nproc) && make install
```


## 4. Geospatial Core

### PROJ (v9.5.1)

```bash
cd $LOCAL_ROOT/src
wget [https://download.osgeo.org/proj/proj-9.5.1.tar.gz](https://download.osgeo.org/proj/proj-9.5.1.tar.gz)
tar -zxvf proj-9.5.1.tar.gz && cd proj-9.5.1
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$LOCAL_ROOT/proj \
      -DSQLITE3_INCLUDE_DIR=$LOCAL_ROOT/sqlite/include \
      -DSQLITE3_LIBRARY=$LOCAL_ROOT/sqlite/lib/libsqlite3.so \
      ..
make -j$(nproc) && make install
```

### GDAL (v3.4.3)

**ABI Compatibility:** This version is required for the `atlas` wheel.

```bash
cd $LOCAL_ROOT/src
wget [https://github.com/OSGeo/gdal/releases/download/v3.4.3/gdal-3.4.3.tar.gz](https://github.com/OSGeo/gdal/releases/download/v3.4.3/gdal-3.4.3.tar.gz)
tar -zxvf gdal-3.4.3.tar.gz && cd gdal-3.4.3
./configure --prefix=$LOCAL_ROOT/gdal \
            --with-proj=$LOCAL_ROOT/proj \
            --with-sqlite3=$LOCAL_ROOT/sqlite \
            --with-libtiff=$LOCAL_ROOT/libtiff \
            --with-curl=$LOCAL_ROOT/libcurl/bin/curl-config
make -j$(nproc) && make install

# Create the symlink for the SOVERSION expected by the wheel
ln -s $LOCAL_ROOT/gdal/lib/libgdal.so.30 $LOCAL_ROOT/gdal/lib/libgdal.so.34
```

## 5. Python Environment Integration

### Conda Environment Setup

Replace `<ENVNAME>` with the desired name of the Python environment. The Python version should be compatible with that of the selected atlas wheel. For Python 3.11:

```bash
conda create -n <ENVNAME> python=3.11 -y
conda activate <ENVNAME>
```

The package requires the opencv library, which must be installed through pip. The installation of opencv through the Conda package manager is not supported. 
```bash
pip install opencv-python 
```

### Permanent Environment Configuration

```bash
conda env config vars set LD_LIBRARY_PATH="$CONDA_PREFIX/lib:$LOCAL_ROOT/gdal/lib:$LOCAL_ROOT/proj/lib:$LOCAL_ROOT/sqlite/lib:$LOCAL_ROOT/openssl/lib64"
conda env config vars set PROJ_DATA="$LOCAL_ROOT/proj/share/proj"
conda env config vars set GDAL_DATA="$LOCAL_ROOT/gdal/share/gdal"

# Reactivate environment to load new variables
conda deactivate && conda activate <ENVNAME>
```

### Install Atlas Wheel

The atlas whell should be compatible with the Python version used for the local environment.

```bash
pip install atlas-0.5.0-cp311-cp311-linux_x86_64.whl
```

## 6. Verification

```bash
python -c "import atlas; print('Atlas successfully imported.')"
```
