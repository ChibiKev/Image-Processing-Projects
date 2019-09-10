
// ================================================================
// histeq.c - Histogram equalization.
// Copyright (C) 2012 by George Wolberg
//
// Written by: George Wolberg, 2012
// ================================================================
//

#include "IP.h"

using namespace std;

void histeq(imageP, imageP);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine to apply histogram equalization
//
int
main(int argc, char **argv)
{
	imageP I1, I2;

	if(argc != 3) {
		cerr << "Usage: histeq infile outfile" << endl;
		exit(1);
	}

	I1 = IP_readImage(argv[1]);
	I2 = NEWIMAGE;

	histeq(I1, I2);
	IP_saveImage(I2, argv[2]);

	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// histeq:
//
// Apply histogram equalization to I1. Output goes to I2.
//
void
histeq(imageP I1, imageP I2)
{
	int i, R;
	int left[MXGRAY], width[MXGRAY];
	uchar *in, *out;
	long total, Hsum, Havg, histo[MXGRAY];

	// total number of pixels in image
	total = (long) I1->width * I1->height;

	// init I2 dimensions and buffer
	I2->width  = I1->width;
	I2->height = I1->height;
	I2->image  = (uchar *) malloc(total);

	// init input and output pointers
	in  = I1->image;		// input  image buffer
	out = I2->image;		// output image buffer

	// compute histogram
	for(i=0; i<MXGRAY; i++) histo[i] = 0;	// clear histogram
	for(i=0; i<total;  i++) histo[in[i]]++;	// eval  histogram

	R    = 0;		// right end of interval
	Hsum = 0;		// cumulative value for interval
	Havg = total / MXGRAY;	// interval value for uniform histogram

	// evaluate remapping of all input gray levels;
	// Each input gray value maps to an interval of valid
	// output values. The endpoints of the intervals are
	// left[] and left[]+width[].
	for(i=0; i<MXGRAY; i++) {
		left[i] = R;		// left end of interval
		Hsum += histo[i];	// cum. interval value

		// make interval wider, if necessary
		while(Hsum>Havg && R<MXGRAY-1){
			Hsum -= Havg;	// adjust Hsum
			R++;		// update right end
		}

		// width of interval
		width[i] = R - left[i] + 1;
	}

	// visit all input pixels and remap intensities
	for(i=0; i<total; i++) {
		if(width[in[i]] == 1) out[i] = left[in[i]];
		else {	// in[i] spills over into width[] possible values
			// randomly pick R from 0 to width[in[i]]
			R = ((rand()&0x7fff)*width[in[i]])>>15;

			// add R to left end of interval
			out[i] = left[in[i]] + R;
		}
	}
}
