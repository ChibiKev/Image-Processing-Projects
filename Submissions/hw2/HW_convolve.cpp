#include "IP.h"
#include <vector>
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_convolve:
//
// Convolve I1 with filter kernel in Ikernel.
// Output is in I2.
//
void
HW_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2)
{
     // copy image header (width, height) of input image I4 to output image I2
     IP_copyImageHeader(I1, I2);
     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> in, out, start;
     ChannelPtr<float> kernel, kernelP; //Find The Type That The Kernel Points To
     int type;

     //Dimensions of input image
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     //Declaring our buffer
     int kernelW = Ikernel->width(); // Getting Kernel Width
     int kernelH = Ikernel->height(); // Getting Kernel Height
     int paddingW = kernelW / 2; // Padding For Kernel Width
     int paddingH = kernelH / 2; // Padding For Kernel Width
     int maxH = paddingH + h + paddingH; // Max Height For Buffer Height
     int maxW = paddingW + w + paddingW; // Max Width For Buffer Width

     unsigned char **buffer = new unsigned char*[maxH]; // Setting 2D Buffer
     for (int i = 0; i < maxH; i++) { // Setting Up Height Buffer
          buffer[i] = new unsigned char[maxW]; // Buffer Width Values For Each Row
     }


     //Initialize buffer
     for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
          IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
          start = in; //making start point at the beginning of image (0,0) to use as reference
          for (int i = 0; i < maxH; i++) { // Setting Up Buffer Values, Starting From First Row
               for (int j = 0; j < maxW; j++) { // Setting Up Buffer Values, Starting From First Column
                    if (i < paddingH || i >= paddingH + h || j < paddingW || j >= paddingW + w) { // Buffer Is Outside The Image, The Padding Part
                         buffer[i][j] = 0; // Set Buffer Values To Zero
                    }
                    else { // Inside Image
                         buffer[i][j] = *start++; //  Set Buffer Values To Same As Image Values
                    }
               }
          }
          for (int i = paddingH; i < h + paddingH; i++) { // Work On The Image. Starting From First Row Of The Image
               for (int j = paddingW; j < w + paddingW; j++) { // Work On The Image. Starting From First Column Of The Image
                    double sum = 0; // Reset Sum To Zero Everytime It Loops
                    IP_getChannel(Ikernel, 0, kernelP, type); // Setting Ikernel To kernelP
                    for (int kr = 0; kr < kernelH; kr++) { //All The Rows Of The Kernel
                         for (int kc = 0; kc < kernelW; kc++) { //Each Column of The kernel
                              sum += buffer[i - paddingH + kr][j - paddingW + kc] * (*kernelP++); // Convolve Is The Sum Of The Products. Sum = Buffer[i][j] * KernelP
                         }
                    }
                    *out++ = CLIP(sum, 0, 255); // Output Is The Clip Of The Sum. 
               }
          }
     }
     for (int i = 0; i < maxH; i++) // Delete Buffer
     {
          delete[] buffer[i];
     }
     delete[] buffer; // Delete Final Buffer
}
