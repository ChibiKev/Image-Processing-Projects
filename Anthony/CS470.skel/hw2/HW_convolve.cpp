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
    // declarations for image channel pointers and datatype
    IP_copyImageHeader(I1, I2);
    ChannelPtr<uchar> in, out, start;
    ChannelPtr<float> kernel;
    int type;
    
    //Dimensions of input image
    int w = I1->width ();
    int h = I1->height();
    int total = w * h;
    
    //Declaring our buffer
    int kernelW = Ikernel->width();
    int kernelH = Ikernel->height();
    int paddingW = kernelW/2;
    int paddingH = kernelH/2;
    int maxH = paddingH + h + paddingH;
    int maxW = paddingW + w + paddingW;

    unsigned char **buffer = new unsigned char*[maxH];
    for(int i=0; i<maxH; i++) {
        buffer[i] = new unsigned char[maxW];
    }
    
    //Initialize buffer
    for(int ch=0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
        IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
        start = in; //making start point at the beginning of image (0,0) to use as reference
        for(int i=0; i<maxH; i++) {
            for(int j=0; j<maxW; j++) {
                if(i<paddingH || i>paddingH+h || j<paddingW || j>paddingW + w) {
                    buffer[i][j] = 0;
                }
                else{
                    buffer[i][j] = *start++;
                }
            }
        }
    }
    
    for(int i=0; i<maxH; i++) {
        for(int j=0; j<maxW; j++) {
            printf("%u ",(unsigned int) buffer[i][j]);
        }
        printf("\n");
    }
    

}
