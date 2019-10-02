#include "IP.h"
using namespace IP;

void histoMatchApprox(ImagePtr, ImagePtr, ImagePtr);
ImagePtr scaleTargetHisto(ImagePtr, ImagePtr);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
void
HW_histoMatch(ImagePtr I1, ImagePtr targetHisto, bool approxAlg, ImagePtr I2)
{
	if(approxAlg) {
		histoMatchApprox(I1, targetHisto, I2);
		return;
	}

}

void histoMatchApprox(ImagePtr I1, ImagePtr targetHisto, ImagePtr I2)
{
	ChannelPtr<uchar> in,out;
    ChannelPtr<int>hist;
    int type;
    int i;
    int w = I1->width ();
    int h = I1->height();
    int total = w * h;
    int histogram[MXGRAY];
    int lut[MXGRAY];
    double normHisto[MXGRAY];
    double normHistoTarget[MXGRAY];
    double runninSumHist[MXGRAY];
    double runninSumHistTarget[MXGRAY];
    
    // init I2 dimensions and buffer
    IP_copyImageHeader(I1, I2);
    
    //Scale target Histogram properly
    targetHisto = scaleTargetHisto(I1,targetHisto);
    
    //Compute Normalize Histogram
    IP_getChannel(I1, 0, in, type);
    for(i=0; i<MXGRAY; i++) histogram[i] = 0;
    for(i=0; i<total; i++) histogram[in[i]]++;
    for(i=0; i<MXGRAY; i++) normHisto[i] = (double)histogram[i]/total;
    
    
    //Normalized target Histogram
    IP_getChannel(targetHisto, 0, hist, type);
    for(i=0; i<MXGRAY; i++) normHistoTarget[i] = (double)hist[i]/total;
    
    
    //Compute Running sum for normalize Histogram
    runninSumHist[0] = normHisto[0];
    for(i=1;i<MXGRAY;i++) runninSumHist[i] = runninSumHist[i-1] + normHisto[i];
        
    //Compute Running sum for normalized target Histogram
    runninSumHistTarget[0] = normHistoTarget[0];
    for(i=1;i<MXGRAY;i++) runninSumHistTarget[i] = runninSumHistTarget[i-1] + normHistoTarget[i];
    
    for(i=0; i<MXGRAY; i++) {
        int l = 255;
        do{
            lut[i] = l;
            l--;
        }while(l>=0 && runninSumHistTarget[l] > runninSumHist[i]);
    }
        
    //Copy to output image
    for(int ch=0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
        IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
        for(i=0; i<total; i++) *out++ = lut[*in++];    // use lut[] to eval output
    }
}

ImagePtr scaleTargetHisto(ImagePtr I1, ImagePtr targetHisto) {
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
        int missing = total - numOfPixels; //pixels that were not added successfully due to scale factor
        hist[MXGRAY-1] += missing; //Adding pixels so that: sum = total num of pixels in image
    }
    return targetHisto;
}
