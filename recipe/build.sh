#!/bin/bash 
# Set the CMake generator to Ninja
export CMAKE_GENERATOR=Ninja
# We ensure it doesn't install any dependencies sine Conda handles them already
$PYTHON -m pip install . --no-deps