#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoStretch:
//
// Apply histogram stretching to I1. Output is in I2.
// Stretch intensity values between t1 and t2 to fill the range [0,255].
//
void HW_histoStretch(ImagePtr I1, int t1, int t2, ImagePtr I2)
{
     // copy image header (width, height) of input image I1 to output image I2
     IP_copyImageHeader(I1, I2);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;
    
    // look up table variables
    int i, lut[MXGRAY];
    
    // declarations for image channel pointers and datatype
    ChannelPtr<uchar> p1, p2;
    int type;
     
     //t2 == t1 case
     if(t2 == t1) {
         std::cout << "Error t2 cannot be equal to t1\n";
         // making the output the same as the input
         for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {    // get input  pointer for channel ch
              IP_getChannel(I2, ch, p2, type);                        // get output pointer for channel ch
              for (i = 0; i < total; i++) *p2++ = *p1++;              // use lut[] to eval output
         }
         return;
     }

     // init lookup table
     for (i = 0; i < MXGRAY; i++) lut[i] = (int) CLIP((255.0 * (i - t1)) / (t2 - t1),0,255); // histoStretch Equation
    
     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {    // get input  pointer for channel ch
          IP_getChannel(I2, ch, p2, type);                        // get output pointer for channel ch
          for (i = 0; i < total; i++) *p2++ = lut[*p1++];         // use lut[] to eval output
     }
}

