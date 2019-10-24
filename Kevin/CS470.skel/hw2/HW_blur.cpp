#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
     // copy image header (width, height) of input image I1 to output image I2
     IP_copyImageHeader(I1, I2);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> in, out;
     int type;

     // Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
     // The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
     // It is ignored here since we assume that our input images consist exclusively of uchars.
     // IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

     // visit all image channels and evaluate output image
     int sum = 0;
     for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {	// get input  pointer for channel ch
          IP_getChannel(I2, ch, out, type);		// get output pointer for channel ch
          int below = filterW;
          int above = w - filterW;
          for (int i = 0; i < total; i++) {
               if (i < below || i > above) {
                    *out++ = *in++;
               }
               else {
                    int sum = 0;
                    int count = 0;
                    for (int i = -filterW; i < filterW; i++) {
                         sum += in[i];
                         count++;
                    }
                    int average = sum / count;
                    *out++ = average;
                    in++;
               }
               if (i == above) {
                    below += w;
                    above += w; 
               }
          }
     }
}
