#include "IP.h"
using namespace IP;

void copyRowToCircBufferFloyd(int row, short* buf[], ChannelPtr<uchar> start, int w, int h);
void copyRowToCircBufferJarvis(int row, short* buf[], ChannelPtr<uchar> start, int w, int h);
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
    
    // copy image header (width, height) of input image I3 to output image I2
    IP_copyImageHeader(I3, I2);

    // init vars for width, height, and total number of pixels
    int w = I3->width ();
    int h = I3->height();
    
    // init lookup table
    int i, lut[MXGRAY];
    int thr = MXGRAY/2;
    for(i=0; i<thr && i<MXGRAY; ++i) lut[i] = 0;
    for(   ;          i<MXGRAY; ++i) lut[i] = MaxGray; //255 = MaxGray
        

    // declarations for image channel pointers and datatype
    ChannelPtr<uchar> in, out, start;
    int type;
    
    
    //Floyd-Steinberg Method ----------------------------------------------------------------
    if(method == 0) {
        //Our 2 buffers used for padding
        short *buf[2] = {}; //array of pointers
        buf[0] = new short[w+2];
        buf[1] = new short[w+2];
        
        //Initializing buffers to 0
        for(int i=0; i<w+2; i++) {
            buf[0][i] = 0;
            buf[1][i] = 0;
        }
        
        short *in1,*in2;
        
        //Raster scan
        if(!serpentine) {
            // visit all image channels of input and evaluate output image
            for(int ch=0; IP_getChannel(I3, ch, in, type); ch++) {    // get input  pointer for channel ch
                IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
                start = in; //making start point at the beginnig of image (0,0) to use as reference
                copyRowToCircBufferFloyd(0,buf, start, w, h);
                for(int y=0; y<h; y++) {
                    copyRowToCircBufferFloyd(y+1, buf, start, w, h);
                    in1 = buf[y%2] + 1; //current row
                    in2 = buf[(y+1)%2] + 1; //row bellow
                    //Go through columns for row y
                    for(int x=0; x<w; x++) {
                        int index = CLIP(*in1,0,255);
                        *out = lut[index];
                        int e = *in1 - *out; //error to spread to neighboors
                        in1[1] += (e*7/16.0); //err to E nbr
                        in2[-1] += (e*3/16.0); //err to SW nbr
                        in2[0] += (e*5/16.0); //err to S nbr
                        in2[1] += (e*1/16.0); //err to SE nbr
                        out++;
                        in1++;
                        in2++;
                    }
                    
                }
            }
        }
        else {
            //Serpentine Scan:
            
            // visit all image channels of input and evaluate output image
            for(int ch=0; IP_getChannel(I3, ch, in, type); ch++) {    // get input  pointer for channel ch
                IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
                start = in; //making start point at the beginnig of image (0,0) to use as reference
                copyRowToCircBufferFloyd(0,buf, start, w, h);
                for(int y=0; y<h; y++) {
                    copyRowToCircBufferFloyd(y+1, buf, start, w, h);
                    
                    //Even row: left to right scan
                    if(y%2 == 0) {
                        in1 = buf[y%2] + 1; //current row (@ start)
                        in2 = buf[(y+1)%2] + 1; //row bellow (@ start)
                        //Go through columns for row y
                        for(int x=0; x<w; x++) {
                            int index = CLIP(*in1,0,255);
                            *out = lut[index];
                            int e = *in1 - *out; //error to spread to neighboors
                            in1[1] += (e*7/16.0); //err to E nbr
                            in2[-1] += (e*3/16.0); //err to SW nbr
                            in2[0] += (e*5/16.0); //err to S nbr
                            in2[1] += (e*1/16.0); //err to SE nbr
                            out++;
                            in1++;
                            in2++;
                        }
                    } //Odd row: right to left scan
                    else {
                        in1 = buf[y%2] + w; //current row (@ end)
                        in2 = buf[(y+1)%2] + w; //row bellow (@ end)
                        out += w-1; //Making output pointer go to the end of the row
                        //Go through columns for row y
                        for(int x=0; x<w; x++) {
                            int index = CLIP(*in1,0,255);
                            *out = lut[index];
                            int e = *in1 - *out; //error to spread to neighboors
                            in1[-1] += (e*7/16.0); //err to W nbr
                            in2[-1] += (e*1/16.0); //err to SW nbr
                            in2[0] += (e*5/16.0); //err to S nbr
                            in2[1] += (e*3/16.0); //err to SE nbr
                            out--;
                            in1--;
                            in2--;
                        }
                        out += w+1; //Making out point to the next row
                    }
                }
            }
        }
    } 
    else { //Jarvis-Judice-Ninke Method -----------------------------------------------------------------------
        
        //Our 3 buffers used for padding
        short *buf[3] = {}; //array of pointers
        buf[0] = new short[w+4];
        buf[1] = new short[w+4];
        buf[2] = new short[w+4];
        short *in1,*in2,*in3;
        
        //Initializing buffers to 0
        for(int i=0; i<w+2; i++) {
            buf[0][i] = 0;
            buf[1][i] = 0;
            buf[2][i] = 0;
        }
        
        //raster scan:
        if(!serpentine) {
            // visit all image channels of input and evaluate output image
            for(int ch=0; IP_getChannel(I3, ch, in, type); ch++) {    // get input  pointer for channel ch
                IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
                start = in; //making start point at the beginnig of image (0,0) to use as reference
                copyRowToCircBufferJarvis(0,buf, start, w, h);
                copyRowToCircBufferJarvis(1,buf, start, w, h);
                for(int y=0; y<h; y++) {
                    copyRowToCircBufferJarvis(y+2, buf, start, w, h); //copy row bellow

                    in1 = buf[y%3] + 2; //current row
                    in2 = buf[(y+1)%3] + 2; // 1 row below current
                    in3 = buf[(y+2)%3] + 2; // 2 rows below current
                    
                    //Go through columns for row y
                    for(int x=0; x<w; x++) {
                        int index = CLIP(*in1,0,255);
                        *out = lut[index];
                        int e = *in1 - *out; //error to spread to neighboors
                        in1[1] += (e*7/48.0); //err to 1E nbr
                        in1[2] += (e*5/48.0); //err to 2E nbr
                        
                        in2[-2] += (e*3/48.0); //err to 2SW nbr
                        in2[-1] += (e*5/48.0); //err to SW nbr
                        in2[0] += (e*7/48.0); //err to S nbr
                        in2[1] += (e*5/48.0); //err to SE nbr
                        in2[2] += (e*3/48.0); //err to 2SE nbr
                        
                        in3[-2] += (e*1/48.0); //err to 2SSW nbr
                        in3[-1] += (e*3/48.0); //err to SSW nbr
                        in3[0] += (e*5/48.0); //err to SS nbr
                        in3[1] += (e*3/48.0); //err to SSE nbr
                        in3[2] += (e*1/48.0); //err to 2SSE nbr
                        
                        out++;
                        in1++;
                        in2++;
                        in3++;
                    }
                    
                }
            }
        }
        else { //Serpentine scan:
            
            // visit all image channels of input and evaluate output image
            for(int ch=0; IP_getChannel(I3, ch, in, type); ch++) {    // get input  pointer for channel ch
                IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
                start = in; //making start point at the beginnig of image (0,0) to use as reference
                copyRowToCircBufferJarvis(0,buf, start, w, h);
                copyRowToCircBufferJarvis(1,buf, start, w, h);
                for(int y=0; y<h; y++) {
                    copyRowToCircBufferJarvis(y+2, buf, start, w, h);
                    
                    //Even row: left to right scan
                    if(y%2 == 0) {
                        in1 = buf[y%3] + 2; //current row
                        in2 = buf[(y+1)%3] + 2; // 1 row below current
                        in3 = buf[(y+2)%3] + 2; // 2 rows below current
                        
                        //Go through columns for row y
                        for(int x=0; x<w; x++) {
                            int index = CLIP(*in1,0,255);
                            *out = lut[index];
                            int e = *in1 - *out; //error to spread to neighboors
                            in1[1] += (e*7/48.0); //err to 1E nbr
                            in1[2] += (e*5/48.0); //err to 2E nbr
                            
                            in2[-2] += (e*3/48.0); //err to 2SW nbr
                            in2[-1] += (e*5/48.0); //err to SW nbr
                            in2[0] += (e*7/48.0); //err to S nbr
                            in2[1] += (e*5/48.0); //err to SE nbr
                            in2[2] += (e*3/48.0); //err to 2SE nbr
                            
                            in3[-2] += (e*1/48.0); //err to 2SSW nbr
                            in3[-1] += (e*3/48.0); //err to SSW nbr
                            in3[0] += (e*5/48.0); //err to SS nbr
                            in3[1] += (e*3/48.0); //err to SSE nbr
                            in3[2] += (e*1/48.0); //err to 2SSE nbr
                            
                            out++;
                            in1++;
                            in2++;
                            in3++;
                        }
                    } //Odd row: right to left scan
                    else {
                        in1 = buf[y%3] + w + 1; //current row (@ end)
                        in2 = buf[(y+1)%3] + w + 1; // 1 row below current(@end)
                        in3 = buf[(y+2)%3] + w + 1; // 2 rows below current (@end)
                        
                        out += w-1; //Making output pointer go to the end of the row
                        //Go through columns for row y
                        for(int x=0; x<w; x++) {
                            int index = CLIP(*in1,0,255);
                            *out = lut[index];
                            int e = *in1 - *out; //error to spread to neighboors
                            
                            in1[-1] += (e*7/48.0); //err to 1E nbr
                            in1[-2] += (e*5/48.0); //err to 2E nbr
                            
                            in2[-2] += (e*3/48.0); //err to 2SW nbr
                            in2[-1] += (e*5/48.0); //err to SW nbr
                            in2[0] += (e*7/48.0); //err to S nbr
                            in2[1] += (e*5/48.0); //err to SE nbr
                            in2[2] += (e*3/48.0); //err to 2SE nbr
                            
                            in3[-2] += (e*1/48.0); //err to 2SSW nbr
                            in3[-1] += (e*3/48.0); //err to SSW nbr
                            in3[0] += (e*5/48.0); //err to SS nbr
                            in3[1] += (e*3/48.0); //err to SSE nbr
                            in3[2] += (e*1/48.0); //err to 2SSE nbr
                            
                            out--;
                            in1--;
                            in2--;
                            in3--;
                        }
                        out += w+1; //Making out point to the next row
                    }
                }
            }
        }
        
    }
    
    
}


void copyRowToCircBufferFloyd(int row, short* buf[], ChannelPtr<uchar> start, int w, int h) {
    if(row >= h) return; //Invalid row to copy [0,h-1]
    ChannelPtr<uchar> original = start; //saving original location
    short *ptr = buf[row%2]; //pointer to correct buffer
    start = (start+ w*row); //making start point to the right row
    for(int i=1; i<w+1;i++) {
        ptr[i] = *start++;
    }
    start = original; //preserving start
}

void copyRowToCircBufferJarvis(int row, short* buf[], ChannelPtr<uchar> start, int w, int h) {
    if(row >= h) return; //Invalid row to copy [0,h-1]
    ChannelPtr<uchar> original = start; //saving original location
    short *ptr = buf[row%3]; //pointer to correct buffer
    start = (start+ w*row); //making start point to the right row
    for(int i=2; i<w+2;i++) {
        ptr[i] = *start++;
    }
    start = original; //preserving start
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

