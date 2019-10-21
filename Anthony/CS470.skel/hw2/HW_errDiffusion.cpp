#include "IP.h"
using namespace IP;

void copyRowToCircBuffer(int row, short* buf[], ChannelPtr<uchar> start, int w, int h);
void printCircBuffer(short* buf[], int w);
ImagePtr HW_gammaCorrect(ImagePtr I1, double gamma);  //To apply gamma correction
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.
//
void HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2)
{
    ImagePtr I3;
    I3 = HW_gammaCorrect(I1, gamma);
    
    // copy image header (width, height) of input image I1 to output image I2
    IP_copyImageHeader(I3, I2);

    // init vars for width, height, and total number of pixels
    int w = I3->width ();
    int h = I3->height();

    //Applying gamma correction to I1 first
    //HW_gammaCorrect(I1, gamma);
    
    // init lookup table
    int i, lut[MXGRAY];
    int thr = MXGRAY/2;
    

    
    for(i=0; i<thr && i<MXGRAY; ++i) lut[i] = 0;
    for(   ;          i<MXGRAY; ++i) lut[i] = MaxGray; //255 = MaxGray
    

    // declarations for image channel pointers and datatype
    ChannelPtr<uchar> in, out, start;
    int type;
    
    //Our buffer
    short *buf[2] = {}; //array of pointers
    buf[0] = new short[w+2];
    buf[1] = new short[w+2];
    short *in1,*in2;
    
    // visit all image channels of input and evaluate output image
    for(int ch=0; IP_getChannel(I3, ch, in, type); ch++) {    // get input  pointer for channel ch
        IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
        start = in; //making start point at the beginnig of image
        copyRowToCircBuffer(0,buf, start, w, h);
        for(int y=0; y<h; y++) {
            copyRowToCircBuffer(y+1, buf, start, w, h);
            in1 = buf[y%2] + 1;
            in2 = buf[(y+1)%2] + 1;
            //printCircBuffer(buf, w);
            for(int x=0; x<w; x++) {
                int index = CLIP(*in1,0,255);
                *out = lut[index];
                int e = *in1 - *out;
                in1[1] += (e*7/16.0);
                in2[-1] += (e*3/16.0);
                in2[0] += (e*5/16.0);
                in2[1] += (e*1/16.0);
                out++;
                in1++;
                in2++;
            }
            
        }
    }
}


void copyRowToCircBuffer(int row, short* buf[], ChannelPtr<uchar> start, int w, int h) {
    if(row >= h) return; //valid row to copy
    ChannelPtr<uchar> original = start; //saving original location
    int index = row%2;
    start = (start+ w*row); //making start point to the right row
    for(int i=1; i<w+1;i++) {
        short *ptr = buf[index];
        ptr[i] = *start++;
    }
    start = original; //preserving in
}

void printCircBuffer(short* buf[], int w) {
    short *ptr1 = buf[0];
    short *ptr2 = buf[1];
    int i;
    for(i=0; i<w+2; i++) {
        std::cout << ptr1[i] << " ";
    }
    std::cout << std::endl;
    for(i=0; i<w+2; i++) {
        std::cout << ptr2[i] << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

}
ImagePtr HW_gammaCorrect(ImagePtr I1, double gamma) {
     // copy image header (width, height) of input image I1 to output image I2
    ImagePtr I3;
    IP_copyImageHeader(I1, I3);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     // init lookup table
     int i, lut[MXGRAY];

     // run Loop From 0 to 255 to Set Up LUT
     // set Grayscale to a Value Between 0 and 1, Then Apply Gamma Correction, Then Multiply Back to 255 For Grayscale.
     for (i = 0; i < MXGRAY; i++) lut[i] = pow(i / 255.0, 1 / gamma) * 255;


     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> p1, p2;
     int type;

     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {    // get input  pointer for channel ch
          IP_getChannel(I3, ch, p2, type);                       // get output pointer for channel ch
          for (i = 0; i < total; i++) *p2++ = lut[*p1++];         // use lut[] to eval output
     }
    return I3;
}

