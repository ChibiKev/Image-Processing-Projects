#include "IP.h"
using namespace IP;

void histoMatchApprox(ImagePtr, ImagePtr, ImagePtr);
ImagePtr scaleTargetHisto(ImagePtr, ImagePtr);



using namespace std;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
void HW_histoMatch(ImagePtr I1, ImagePtr targetHisto, bool approxAlg, ImagePtr I2)
{
  if(approxAlg) {
    histoMatchApprox(I1, targetHisto, I2);
    return;
  }
    
    // copy image header (width, height) of input image I1 to output image I2
    IP_copyImageHeader(I1, I2);
    int i, p, R;
    int left[MXGRAY], right[MXGRAY];
    int total, Hsum, histogram[MXGRAY];
    ChannelPtr<uchar> p1, p2;
    int type;
    
    // Total number of pixels in image
    int w = I1->width();
    int h = I1->height();
    total = w * h;
    
    //clear histogram
    for (i = 0; i < MXGRAY; i++) histogram[i] = 0;

    //Evaluate histogram
    IP_getChannel(I1, 0, p1, type);
    for (i = 0; i < total; i++) histogram[p1[i]]++;
    
    // normalize h2 to conform with dimensions of I1
    targetHisto = scaleTargetHisto(I1, targetHisto);

    // target histogram & normalize to match the dimensions of I1
    ChannelPtr<int> h2;
    IP_getChannel(targetHisto, 0, h2, type);
    
    R = 0;
    Hsum = 0;

    int preserve[MXGRAY], left2[MXGRAY];
    // Evaluate remapping of all input gray levels;
    // Each input gray value maps to an interval of valid output values.
    // The endpoints of the intervals are left[] and right[]
    for (i = 0; i < MXGRAY; i++) {
         left[i] = left2[i] = R;                    // left end of interval
         preserve[i] = h2[R] - Hsum;                // Keep what is going to be lost after entering the while loop
         Hsum += histogram[i];                      // cumulative value for interval
         while (Hsum > h2[R] && R < MXGRAY - 1) {   // compute width of interval
              Hsum -= h2[R];                        // adjust Hsum as interval widens
              R++;                                  // update
         }
         right[i] = R;                              // init right end of interval
    }
    // clear h1 and reuse it below
    for (i = 0; i < MXGRAY; i++) histogram[i] = 0;
    // visit all input pixels
    for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {    // get input  pointer for channel ch
         IP_getChannel(I2, ch, p2, type);                       // get output pointer for channel ch
         for (i = 0; i < total; i++) {
              p = left[*p1];
              if (histogram[p] < h2[p]) {                      // mapping satisfies h2
                   if (left[*p1] != right[*p1] && left[*p1] == left2[*p1]) { // when left is not equal to right and left haven't been modified
                        if (preserve[*p1]-- > 0) {                           // uses preserve and subtracts it by 1
                             *p2++ = p;
                        }
                        else {
                             *p2++ = p = left[*p1] = MIN(p + 1, right[*p1]);
                        }
                   }
                   else {
                        *p2++ = p;
                   }
              }
              else {
                   *p2++ = p = left[*p1] = MIN(p + 1, right[*p1]);
              }
              histogram[p]++;
              p1++;
         }
    }

}

void histoMatchApprox(ImagePtr I1, ImagePtr targetHisto, ImagePtr I2)
{
    // copy image header (width, height) of input image I1 to output image I2
    IP_copyImageHeader(I1, I2);
    int i, p, R;
    int left[MXGRAY], right[MXGRAY];
    int total, Hsum, h1[MXGRAY];
    
    // Total number of pixels in image
    int w = I1->width();
    int h = I1->height();
    total = w * h;

    ChannelPtr<uchar> p1, p2;
    int type;

    for (i = 0; i < MXGRAY; i++) h1[i] = 0;

    for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {    // get input pointer for channel ch
         for (i = 0; i < total; i++) h1[*p1++]++;
    }

    // normalize h2 to conform with dimensions of I1
    targetHisto = scaleTargetHisto(I1, targetHisto);

    // target histogram
    ChannelPtr<int> h2;
    IP_getChannel(targetHisto, 0, h2, type);
  
    R = 0;
    Hsum = 0;

    // Evaluate remapping of all input gray levels;
    // Each input gray value maps to an interval of valid output values.
    // The endpoints of the intervals are left[] and right[]
    for (i = 0; i < MXGRAY; i++) {
         left[i] = R;                             // left end of interval
         Hsum += h1[i];                           // cumulative value for interval
         while (Hsum > h2[R] && R < MXGRAY - 1) { // compute width of interval
              Hsum -= h2[R];                      // adjust Hsum as interval widens
              R++;                                // update
         }
         right[i] = R; // init right end of interval
    }
    
    // clear h1 and reuse it below
    for (i = 0; i < MXGRAY; i++) h1[i] = 0;
    
    // visit all input pixels
    for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {    // get input  pointer for channel ch
         IP_getChannel(I2, ch, p2, type);                        // get output pointer for channel ch
         for (i = 0; i < total; i++, p1++, p2++) {
              p = left[*p1];
              if (h1[p] < h2[p]) {                               // mapping satisfies h2
                   *p2 = p;
              }
              else {
                   *p2 = p = left[*p1] = MIN(p + 1, right[*p1]);
              }
              h1[p]++;
         }
    }
    
}


ImagePtr scaleTargetHisto(ImagePtr I1, ImagePtr targetHisto) { // scaled properly so that the sum is equal to the total number of pixels in the input image
    int w = I1->width ();
    int h = I1->height();
    int total = w * h; //total number of pixels
    ChannelPtr<int> hist;
    int type;
    int i;
    
    int Havg = 0;
    IP_getChannel(targetHisto, 0, hist, type);
    for(i=0;i<MXGRAY; i++) Havg+= hist[i];
    double scale = (double)total/Havg;
    
    if(scale!=1) {
        int numOfPixels = 0;
        for(i=0;i<MXGRAY;i++) {
            hist[i] *= scale;
            numOfPixels += hist[i];
        }
        int missing = total - numOfPixels; // pixels that were not added successfully due to scale factor
        hist[MXGRAY-1] += missing;         // adding pixels so that: sum = total num of pixels in image
    }
    return targetHisto;
}
