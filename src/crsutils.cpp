
#include "crsutils.h"


// Create a geographic reference system for the Moon
OGRSpatialReference MoonGeographicCRS() {

    OGRSpatialReference crs; 

    crs.SetGeogCS( 
        "GCS_Moon_2000",
        "D_Moon_2000", 
        "MOON_2000_IAU_IAG", 
        1737400.0, 0.0, 
        "Reference Meridian", 0.0, 
        SRS_UA_DEGREE, atof(SRS_UA_DEGREE_CONV)
    );

    // Change mapping to longitude first
    crs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
    return crs;

}