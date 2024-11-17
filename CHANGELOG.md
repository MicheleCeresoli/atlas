# All Releases 

## v0.4.3 
- Fixed `getAltitude` not returning the proper altitude. 
- Added `meanRadius` to `World`.
- Added option to `generateImageDEM` to normalize the values relative to the image content.

## v0.4.2 
- Added documentation. 
- Added `unload` to `LunarRayTracer` to unload all raster bands from memory.

## v0.4.1
- Replaced all `ui16_t` instances to `ui32_t` to avoid overflow errors.
- Added error information when tracing without any loaded DEM rasters.

## v0.4.0
- Added conda recipe.
- Added `dem-path` and `dom-path` settings to YAML configuration file.
- Changed camera coordinate axes: the Z-axis is directed towards the nadir, the Y-axis now points upwards and the X-axis completes the right-handed triad.
- Replaced `uint` with the standard C\C++ type definitions. 

## v0.3.0
- Renamed project to _ATLAS_
- Added ray resolution logging. 
- Fixed adaptive ray tracing algorithm not returning the correct values in certain scenarios.
- Exported ray-traced data files now have the `.brd` (Binary Raytraced Data) extension.
- Ray impact position error is now ensured to always be smaller than the DEM resolution.
- The `RayTracerOptions` class can now be created using a YAML configuration file.

## v0.2.0 
- Added `exportRayTracedInfo` and `importRayTracedInfo` to store in binary files the ray traced data from the DEM model. 
- Added `LogLevel` to handle log status display.
- Maximum ray distance threshold in post-processing is now infinite. 
- Added additional SSAA settings customisation.
- Minor improvements.

## v0.1.1 
- Fixed pixel boundaries computation. 
- Fixed `getAltitude` not returning the actual camera altitude.

## v0.1.0 
- First release of the ARCADIA package. 
- Support for Optical, DEM and Depth image generation. 