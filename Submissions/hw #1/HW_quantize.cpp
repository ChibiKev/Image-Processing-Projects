#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
void HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
    // copy image header (width, height) of input image I1 to output image I2
    IP_copyImageHeader(I1, I2);
    
    // init vars for width, height, and total number of pixels
    int w = I1->width ();
    int h = I1->height();
    int total = w * h;
    
    // init lookup table
    int i, lut[MXGRAY];
    double scale = (double)MXGRAY/levels;
    double bias = 128.0/levels;
    for(i=0; i<MXGRAY; i++) lut[i] = MIN((scale * (int) (i/scale)) + bias,255); //Value can overflow so use MIN
        
    
    ChannelPtr<uchar> p1,p2; //p1 points to I1 channels and p2 to I2 channels
    int type;
    
    for(int ch=0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(int i=0; i<total; i++) {
            //add noise to input
            if(dither) {
                double ran = (double)rand() / RAND_MAX; //normalized random number in range [0...1]
                int noise = bias * (1 - (2 * ran)); //Creates range [-bias ..... bias]
                int newValue = (*p1++) + noise;
                //handle overflow
                if(newValue>255) newValue = 255;
                if(newValue<0) newValue = 0;
                *p2++ = lut[newValue];
            }
            else {
                *p2++ = lut[(*p1++)];
            }
        }
    }
    
}