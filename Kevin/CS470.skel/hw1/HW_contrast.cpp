#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_contrast:
//
// Apply contrast enhancement to I1. Output is in I2.
// Stretch intensity difference from reference value (128) by multiplying
// difference by "contrast" and adding it back to 128. Shift result by
// adding "brightness" value.
//
void
HW_contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2)
{
     // copy image header (width, height) of input image I1 to output image I2
     IP_copyImageHeader(I1, I2);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     // init lookup table
     int i, lut[MXGRAY];
     for (i = 0; i < MXGRAY; i++) {
          lut[i] = round((i-128) * contrast) + 128; // Contrast Equation
          lut[i] += brightness; // Shifting Based on Brightness
          if (lut[i] > MaxGray) lut[i] = 255; // In case it goes too far
          if (lut[i] < 0) lut[i] = 0; // In case it goes too short
     }

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
