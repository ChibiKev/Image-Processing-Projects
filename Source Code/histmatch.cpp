
// ================================================================
// histmatch.c - Histogram matching.
// Copyright (C) 2012 by George Wolberg
//
// Written by: George Wolberg, 2012
// ================================================================


#include "IP.h"

using namespace std;

imageP	readHistogram(char*);
int	histogramMatch(imageP I1, imageP histo, imageP I2);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine to apply histogram matching
//
int
main(int argc, char **argv)
{
	imageP I1, I2, histo;

	if(argc != 4) {
		cerr << "Usage: histmatch infile histogram outfile" << endl;
		exit(1);
	}

	I1 = IP_readImage(argv[1]);	// input image
	histo = readHistogram(argv[2]); // desired output histogram
	I2 = NEWIMAGE;			// make struct for output
	histogramMatch(I1, histo, I2);	// histogram matching
	IP_saveImage(I2, argv[3]);	// save result to file
	IP_freeImage(I1);		// free up input  memory
	IP_freeImage(I2);		// free up output memory

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// readHistogram:
//
// Read a 256-entry histogram from file. Format: 256 binary ints.
// Return pointer to image containing integer histogram array.
//
imageP
readHistogram(char *file)
{
	FILE *fp;
	imageP I;

	// read histogram
	if((fp = fopen(file, "r")) == NULL) {
		cerr << "readHistogram: Can't open " << file << endl;
		exit(1);
	}

	I = IP_allocImage(1, 256, sizeof(int));
	fread(I->image, 256, sizeof(int),  fp);
	fclose(fp);

	return(I);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// histogramMatch:
//
// Apply histogram matching to I1 by remapping intensities in I1 to
// have histogram specified in histo. Output goes to I2.
//
int
histogramMatch(imageP I1, imageP histo, imageP I2)
{
	int i, p, R;
	int left[MXGRAY], right[MXGRAY];
	int total, Hsum, Havg, h1[MXGRAY], *h2;
	unsigned char *in, *out;
	double scale;

	// total number of pixels in image
	total = (long) I1->height * I1->width;

	// init I2 dimensions and buffer
	I2->width  = I1->width;
	I2->height = I1->height;
	I2->image  = (unsigned char *) malloc(total);

	in  = I1->image;		// input image  buffer
	out = I2->image;		// output image buffer

	for(i=0; i<MXGRAY; i++) h1[i] = 0;	// clear histogram
	for(i=0; i<total; i++)  h1[in[i]]++;	// eval  histogram

	// target histogram
	h2 = (int *) histo->image;

	// normalize h2 to conform with dimensions of I1
	for(i=Havg=0; i<MXGRAY; i++) Havg += h2[i];
	scale = (double) total / Havg;
	if(scale != 1) for(i=0; i<MXGRAY; i++) h2[i] *= scale;

	R = 0;
	Hsum = 0;
	// evaluate remapping of all input gray levels;
	// Each input gray value maps to an interval of valid
	// output values. The endpoints of the intervals are
	// left[] and right[] */
	for(i=0; i<MXGRAY; i++) {
		left[i] = R;		// left end of interval
		Hsum += h1[i];		// cum. interval value

		// make interval wider, if necessary
		while(Hsum>h2[R] && R<MXGRAY-1) {
			Hsum -= h2[R];		// adjust Hsum
			R++;			// update
		}

		// init right end of interval
		right[i] = R;
	}

	// clear h1 and reuse it below
	for(i=0; i<MXGRAY; i++) h1[i] = 0;

	// visit all input pixels
	for(i=0; i<total; i++) {
		p = left[in[i]];
		if(h1[p] < h2[p])	// mapping satisfies h2
			out[i] = p;
		else	out[i] = p = left[in[i]] = MIN(p+1, right[in[i]]);
		h1[p]++;
	}
}
