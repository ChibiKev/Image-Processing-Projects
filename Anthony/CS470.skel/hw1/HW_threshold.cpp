#include "IP.h"
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_threshold:
//
// Threshold I1 using threshold thr. Output is in I2.
// input<thr: output=0;	 input >= thr: output=MaxGray (255)
//
void
HW_threshold(ImagePtr I1, int thr, ImagePtr I2)
{
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and total number of pixels
    int w = I1->width ();
	int h = I1->height();
	int total = w * h;
    
	// init lookup table
    int i;
    
    //File to write output
    //char * out;
    FILE * output;
    output = fopen("output.txt","w");
    fprintf(output, "%d\t%d\n", 2, w);
    
	// declarations for image channel pointers and datatype
    ChannelPtr<uchar> input;
	int type;
    
    IP_getChannel(I1, 0, input, type);
	// visit all image channels of input and evaluate output image
    
    int row = w * 128; //row 128
    
    for(int column=0; column<w; column++) {
        int value = input[row+column];
        fprintf(output, "%d\t%d\n", value, 0);
    }
    
    fclose(output);
	
}
