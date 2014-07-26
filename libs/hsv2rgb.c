/* hsv2rgb.c
 * Convert Hue Saturation Value to Red Green Blue
 *
 * P.J. 08-Aug-98
 *
 * Reference:
 * D. F. Rogers
 * Procedural Elements for Computer Graphics
 * McGraw Hill 1985
 */

#include "hsv2rgb.h"
 
int hsv2rgb( struct hsv_colour *hsv, struct rgb_colour *rgb ) {
   /*
    * Purpose:
    * Convert HSV values to RGB values
    * All values are in the range [0.0 .. 1.0]
    */
   float S, H, V, F, M, N, K;
   int   I;
   
   S = hsv->s;  /* Saturation */
   H = hsv->h;  /* Hue */
   V = hsv->v;  /* value or brightness */
   
   if ( S == 0.0 ) {
      /* 
       * Achromatic case, set level of grey 
       */
      rgb->r = V;
      rgb->g = V;
      rgb->b = V;
   } else {
      /* 
       * Determine levels of primary colours. 
       */
      if (H >= 1.0) {
         H = 0.0;
      } else {
         H = H * 6;
      } /* end if */
      I = (int) H;   /* should be in the range 0..5 */
      F = H - I;     /* fractional part */

      M = V * (1 - S);
      N = V * (1 - S * F);
      K = V * (1 - S * (1 - F));

      if (I == 0) { rgb->r = V; rgb->g = K; rgb->b = M; }
      if (I == 1) { rgb->r = N; rgb->g = V; rgb->b = M; }
      if (I == 2) { rgb->r = M; rgb->g = V; rgb->b = K; }
      if (I == 3) { rgb->r = M; rgb->g = N; rgb->b = V; }
      if (I == 4) { rgb->r = K; rgb->g = M; rgb->b = V; }
      if (I == 5) { rgb->r = V; rgb->g = M; rgb->b = N; }
   } /* end if */

   return 0;
} /* end function hsv2rgb */
