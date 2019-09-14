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
    //Same as threshold for testing purposes
    
    // copy image header (width, height) of input image I1 to output image I2
    IP_copyImageHeader(I1, I2);
    int thr = levels;
    
    // init vars for width, height, and total number of pixels
    int w = I1->width ();
    int h = I1->height();
    int total = w * h;
    
    // init lookup table
    int i, lut[MXGRAY];
    for(i=0; i<thr && i<MXGRAY; i++) lut[i] = 0;
    for(; i<MXGRAY; i++) lut[i] = MaxGray;
    
    ChannelPtr<uchar> p1,p2; //p1 points to I1 channels and p2 to I2 channels
    int type;
    
    for(int ch=0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(int i=0; i<total; i++) *p2++ = lut[*p1++];
    }
    
}
