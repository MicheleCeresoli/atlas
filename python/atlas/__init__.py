# import _atlas  # type: ignore

from ._atlas import PinholeCamera, RealCamera         # type: ignore
from ._atlas import RayTracer                    # type: ignore
from ._atlas import LogLevel                          # type: ignore

import os
import glob 

import yaml

def RayTracerOptions(filename: str): 
        
    # Configuration data 
    with open(filename, 'r') as file:
        config = yaml.safe_load(file)
    
    # Retrieve number of threads 
    nthreads = config.get('n-threads', 1)
    
    # Retrieve log-level 
    log_level = LogLevel(config.get('log-level', 1))

    # Initialise the options
    opts = _atlas.RayTracerOptions(nthreads, log_level)
    
    # Retrieve rendering options
    if 'rendering' in config.keys(): 
        cfg_renderer = config['rendering'] 
        
        if 'grid-width' in cfg_renderer.keys(): 
            opts.optsRenderer.gridWidth = cfg_renderer['grid-width']
        
        if 'grid-height' in cfg_renderer.keys(): 
            opts.optsRenderer.gridheight = cfg_renderer['grid-height']    
        
        if 'adaptive-tracing' in cfg_renderer.keys(): 
            opts.optsRenderer.adaptiveTracing = cfg_renderer['adaptive-tracing']
            
        # Retrieve Antialiasing (SSAA) options
        if 'ssaa' in cfg_renderer.keys():
            cfg_ssaa = cfg_renderer['ssaa']
            
            for (key, val) in cfg_ssaa.items():
                if key == 'active': 
                    opts.optsRenderer.ssaa.active = val
                    
                elif key == 'subsamples': 
                    opts.optsRenderer.ssaa.nSamples = val 
                    
                elif key == 'threshold': 
                    opts.optsRenderer.ssaa.threshold = val 
                    
                elif key == 'res-multiplier': 
                    opts.optsRenderer.ssaa.resMultiplier = val 
                    
                elif key == 'boundary-size': 
                    opts.optsRenderer.ssaa.boundarySize = val
   
    # Retrieve world settings 
    if 'world' in config.keys(): 
        cfg_world = config['world']
        
        # Retrieve all DEM files
        dem_path = cfg_world.get('dem-path', '')
        if 'dem-files' in cfg_world.keys(): 
            
            dem_files = []
            for file in cfg_world['dem-files']: 
                dem_files += glob.glob(os.path.join(dem_path, file))
                
            # avoid duplicate files
            opts.optsWorld.demFiles = list(set(dem_files))

        # Retrieve all DOM files
        dom_path = cfg_world.get('dom-path', '')
        if 'dom-files' in cfg_world.keys(): 

            dom_files = []
            for file in cfg_world['dom-files']: 
                dom_files += glob.glob(os.path.join(dom_path, file))
                
            # avoid duplicate files
            opts.optsWorld.domFiles = list(set(dom_files))
        
        if 'raster-usage-threshold' in cfg_world.keys(): 
            opts.optsWorld.rasterUsageThreshold = cfg_world['raster-usage-threshold']
        
        if 'min-resolution' in cfg_world.keys(): 
            opts.optsWorld.minRes = float(cfg_world['min-resolution'])
    
    return opts 
    