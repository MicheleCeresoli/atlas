
Installation 
==============
This package can either be installed by manually building the library on your system or by 
downloading the appropriate python wheel from the latest published package release. 
In both cases, except when using a Conda package, ATLAS requires the `GDAL`_ and `OpenCV`_ 
libraries to be installed. If they are not available, they can be installed with:

.. prompt:: bash $

   sudo apt install libgdal-dev

.. prompt:: bash $
   
   sudo apt install libopencv-dev

.. _GDAL: https://github.com/OSGeo/gdal
.. _OpenCV: https://opencv.org/


From Conda Package 
---------------------------

From the latest release, download the appropriate ``.tar.bz2`` package according to your
python version and system build. Then, once the conda environment has been activated, run:

.. prompt:: bash $

   conda install gdal opencv pyyaml
   conda install ./atlas-0.4.1-py312h2bc3f7f_0.tar.bz2

where ``atlas-0.4.1-py312h2bc3f7f_0.tar.bz2`` is the filename of the target conda package. 

.. note:: Note that the first command manually installs the required dependencies since Conda 
   environments are isolated and don't see the system-level libraries.

From Python Wheel (pip) 
---------------------------

Once a Python wheel has been downloaded, to install the ATLAS module in the current active 
environment simply run: 

.. prompt:: bash $

   pip install atlas-0.4.1-cp312-cp312-linux_x86_64.whl
   
where ``atlas-0.4.1-cp312-cp312-linux_x86_64.whl`` is the filename of your target python wheel.


From Sources 
---------------------------

Manually building the library requires a C++ compiler and a `CMake`_ version greater or 
equal to 3.15. The GDAL and OpenCV libraries must already be installed, as well as 
the `pybind11`_ package. To properly work, 
CMake requires a ``pybind11Config.cmake`` file which is automatically installed only if the 
package is installed via conda, i.e., with:

.. prompt:: bash $

   conda install pybind11 -c conda-forge


Then, open a terminal in this folder and build the library with: 

.. prompt:: bash $

   mkdir build 
   cd build 
   cmake --build .

Finally, activate your target python environment, and in the root directory of the 
package type: 

.. prompt:: bash $ 
   
   pip install .

.. _CMake: https://cmake.org/
.. _pybind11: https://github.com/pybind/pybind11

