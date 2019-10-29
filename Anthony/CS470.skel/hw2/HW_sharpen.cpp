#include "IP.h"
using namespace IP;
void HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen:
//
// Sharpen image I1. Output is in I2.
//
void
HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
     ImagePtr I3; // ImagePtr For Blur
     if (size % 2 == 0) size++; // Makes sure size is odd
     HW_blur(I1, size, size, I3); // Blur Function, Save Into I3
     // copy image header (width, height) of input image I1 to output image I2
     IP_copyImageHeader(I1, I2);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> in, out, blur;
     int type;

     // Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
     // The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
     // It is ignored here since we assume that our input images consist exclusively of uchars.
     // IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
          IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
          IP_getChannel(I3, ch, blur, type);      // get blur pointer for channel ch
          for (int i = 0; i < total; i++, in++, blur++) {
               // (Current - Blur) * Factor + (Current), Clip Between 0 and 255
               *out++ = CLIP((*in - *blur) * factor + *in , 0, 255); // Equation For Sharpen
          }
     }
}

