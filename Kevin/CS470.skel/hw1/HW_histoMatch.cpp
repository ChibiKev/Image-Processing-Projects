#include "IP.h"
using namespace IP;

void histoMatchApprox(ImagePtr, ImagePtr, ImagePtr);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
void
HW_histoMatch(ImagePtr I1, ImagePtr targetHisto, bool approxAlg, ImagePtr I2)
{
     IP_copyImageHeader(I1, I2);
	if(approxAlg) {
		histoMatchApprox(I1, targetHisto, I2);
		return;
	}
     int i, p, R;
     int left[MXGRAY], right[MXGRAY];
     int total, Hsum, Havg, h1[MXGRAY];
     double scale;
     // Total number of pixels in image
     //total = (long)I1->height * I1->width;
     int w = I1->width();
     int h = I1->height();
     total = w * h;
     // Init I2 dimensions and buffer
     //I2->width = I1->width;
     //I2->height = I1->height;
     //I2->image = (unsigned char*)malloc(total);
     //in = I1->image;
     //out = I2->image;
     ChannelPtr<uchar> p1, p2;
     int type;
     for (i = 0; i < MXGRAY; i++) h1[i] = 0;
     for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
          for (i = 0; i < total; i++) h1[*p1]++;	// use lut[] to eval output
     }


     // target histogram
     //h2 = (int *) histo->image;
     ChannelPtr<int> h2;
     IP_getChannel(targetHisto, 0, h2, type);
     // normalize h2 to conform with dimensions of I!
     for (i = Havg = 0; i < MXGRAY; i++) Havg += h2[i];
     scale = (double)total / Havg;
     if (scale != 1) for (i = 0; i < MXGRAY; i++) h2[i] *= scale;
     R = 0;
     Hsum = 0;
     // Evaluate remapping of all input gray levels;
     // Each input gray value maps to an interval of valid output values.
     // The endpoints of the intervals are left[] and right[]
     for (i = 0; i < MXGRAY; i++) {
          left[i] = R; // left end of interval
          Hsum += h1[i]; // cumulative value for interval
          while (Hsum > h2[R] && R < MXGRAY - 1) { // compute width of interval
               Hsum -= h2[R]; // adjust Hsum as interval widens
               R++; // update
          }
          right[i] = R; // init right end of interval
     }
     // clear h1 and reuse it below
     for (i = 0; i < MXGRAY; i++) h1[i] = 0;
     // visit all input pixels
     for (i = 0; i < total; i++) {
          p = left[*p1++];
          if (h1[p] < h2[p]) // mapping satisfies h2
               *p2++ = p;
          else
               *p2++ = p = left[*p1++] = MIN(p + 1, right[*p1++]);
          h1[p]++;
     }
}

void
histoMatchApprox(ImagePtr I1, ImagePtr targetHisto, ImagePtr I2)
{
}
