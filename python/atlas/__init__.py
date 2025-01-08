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
    if 'n-threads' in config.keys(): 
        nthreads = config['n-threads']
    else: 
        nthreads = 1 
        
    # Retrieve log-level 
    if 'log-level' in config.keys():
        log_level = LogLevel(config['log-level'])
    else: 
        log_level = LogLevel.MINIMAL   

    opts = _atlas.RayTracerOptions(nthreads, log_level)
    
    # Retrieve rendering options
    if 'rendering' in config.keys(): 
        cfg_renderer = config['rendering'] 
        
        if 'batch-size' in cfg_renderer.keys(): 
            opts.optsRenderer.batchSize = cfg_renderer['batch-size']
            
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
        dem_path = ''
        if 'dem-path' in cfg_world.keys(): 
            dem_path = cfg_world['dem-path']

        if 'dem-files' in cfg_world.keys(): 
            
            dem_files = []
            for file in cfg_world['dem-files']: 
                dem_files += glob.glob(os.path.join(dem_path, file))
                
            # avoid duplicate files
            opts.optsWorld.demFiles = list(set(dem_files))

        # Retrieve all DOM files
        dom_path = ''
        if 'dom-path' in cfg_world.keys(): 
            dom_path = cfg_world['dom-path']
                            
        if 'dom-files' in cfg_world.keys(): 

            dom_files = []
            for file in cfg_world['dom-files']: 
                dom_files += glob.glob(os.path.join(dom_path, file))
                
            # avoid duplicate files
            opts.optsWorld.domFiles = list(set(dom_files))
        
        if 'raster-usage-threshold' in cfg_world.keys(): 
            opts.optsWorld.rasterUsageThreshold = cfg_world['raster-usage-threshold']
        
    
    return opts 
    