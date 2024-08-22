#ifndef COLOR_H 
#define COLOR_H 

#include <iostream> 
#include "vec3.h"

using Color = vec3; 

void write_color(std::ostream& out, const Color& pixel_color);

#endif 