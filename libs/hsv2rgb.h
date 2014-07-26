#ifndef _HSV2RGB_H
#define _HSV2RGB_H

/* hsv2rgb.h */

struct hsv_colour {
   float h;
   float s;
   float v;
}; /* end struct */

struct rgb_colour {
   float r;
   float g;
   float b;
}; /* end struct */

int hsv2rgb( struct hsv_colour *hsv, struct rgb_colour *rgb );

#endif
