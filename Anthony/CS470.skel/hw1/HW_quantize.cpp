#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//

int getNoise(int bias);



void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
    //Same as threshold for testing purposes
    
    // copy image header (width, height) of input image I1 to output image I2
    IP_copyImageHeader(I1, I2);
    
    // init vars for width, height, and total number of pixels
    int w = I1->width ();
    int h = I1->height();
    int total = w * h;
    
    
    /* First Solution */
    // init lookup table
    int i, lut[MXGRAY];
    int scale = MXGRAY/levels;
    int bias = 128/levels;
    for(i=0; i<MXGRAY; i++) {
        int value = (scale * (int) (i/scale)) + bias; //value can overflow
        if(value>255) value = (scale * (int) (i/scale));
        lut[i] = value;
    }
    
    /* Alternative Solution: Matches his program more */
    // init lookup table
    //    int i, lut[MXGRAY];
    //    double scale = (double) MXGRAY/levels;
    //    int bias = 128/levels;
    //    for(i=0; i<MXGRAY; i++) lut[i] = ceil((scale * (int) (i/scale) )) + bias;
    

    
    ChannelPtr<uchar> p1,p2; //p1 points to I1 channels and p2 to I2 channels
    int type;
    
    for(int ch=0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(int i=0; i<total; i++) {
            //add noise
            if(dither) {
                int noise = getNoise(bias);
                int newValue = (*p1++) + noise;
                if(newValue>255) newValue = 255;
                if(newValue<0) newValue = 0;
                *p2++ = lut[newValue];
                //*p2++ = newValue;
            }
            else {
                *p2++ = lut[(*p1++)];
            }
        }
    }
    
}

int getNoise(int bias) {
    double ran = (double)rand()/RAND_MAX; //normalized random # [0...1]
    int result = bias*( 1 - (2*ran) ); //Creates range [-bias ..... bias]
    return result;
}
