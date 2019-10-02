#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_gammaCorrect:
//
// Gamma correct image I1. Output is in I2.
//
void
HW_gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
     // copy image header (width, height) of input image I1 to output image I2
     IP_copyImageHeader(I1, I2);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     // init lookup table
     int i, lut[MXGRAY];

#if 0 // 0 For Second Code, 1 For First Code, I think the First code is better.
     // Set grayscale to a value between 0 and 1, then apply gamma correction, then multiply back to 255 for grayscale.
     for (i = 0; i < MXGRAY; i++) lut[i] = pow((double)i / 255, 1 / gamma) * 255;
#else
     for (i = 0; i < MXGRAY; ++i) {
          double result = i / 255.0;
          result = pow(result, 1 / gamma);
          result *= 255;
          lut[i] = result;
     }
#endif

     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> p1, p2;
     int type;

     // Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
     // The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
     // It is ignored here since we assume that our input images consist exclusively of uchars.
     // IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
          IP_getChannel(I2, ch, p2, type);		// get output pointer for channel ch
          for (i = 0; i < total; i++) *p2++ = lut[*p1++];	// use lut[] to eval output
     }
}
