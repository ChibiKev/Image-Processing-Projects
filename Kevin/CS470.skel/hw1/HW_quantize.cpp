#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
     // copy image header (width, height) of input image I1 to output image I2
     IP_copyImageHeader(I1, I2);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     // init lookup table
     int i, lut[MXGRAY];
     double scale = (double)MXGRAY / levels; // Intermediate gray values are increments of 256/levels
     double bias = 128 / levels; // Bias of 128/levels
     for (i = 0; i < MXGRAY; i++) lut[i] = (scale * (int) (i/scale)+ bias); // Quantization Equation

     
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
          if (dither) {
#if 0 // 1 For Solution, 0 For Mine
               int j, k, s;
               for (int y = 0; y < h; y++) {
                    s = (y % 2 ? 1 : -1);
                    for (int x = 0; x < w; x++) {
                         j = ((double)rand() / RAND_MAX) * bias;
                         k = *p1++ + j * s;
                         s *= -1;
                         if (k > MaxGray) k = 255;
                         if (k < 0) k = 0;
                         *p2++ = lut[k];
                    }
               }
#else
               int plusorminus = 1; // To alternate between 1 and -1
               for (i = 0; i < total; i++) {
                    double jitter = ((double)rand() / RAND_MAX)*bias; // Random number between 0 and 1, scaled with bias
                    int result = (*p1++) + (jitter * plusorminus); // Applying dither
                    if (result > MaxGray) result = 255; // In case it goes too far
                    if (result < 0) result = 0; // In case it goes too short
                    plusorminus *= -1; // Switch to -1 or 1
                    *p2++ = lut[result];
               }
#endif
          }
          else {
               for (i = 0; i < total; i++) *p2++ = lut[*p1++];	// use lut[] to eval output
          }
     }
}
