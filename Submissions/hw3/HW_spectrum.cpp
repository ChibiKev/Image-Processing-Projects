#include "IP.h"
#include <stdio.h>
#include <algorithm>

using namespace IP;
using namespace std;

struct complexP {
     int len;	    // length of complex number list
     float *real;	// pointer to real number list
     float *imag;	// pointer to imaginary number list
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_spectrum:
//
// Convolve magnitude and phase spectrum from image I1.
// Output is in Imag and Iphase.
//

extern void HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase);
void paddedImage(ImagePtr I1, ImagePtr I1Padded);
void fft1D(complexP *q1, int dir, complexP *q2);
void fft1DRow(ImagePtr I1, ImagePtr Image1);
void fft1DColumn(ImagePtr I1, ImagePtr Image1, ImagePtr Image2);
void fft1DMagPhase(ImagePtr I1, ImagePtr Image2, float *Magnitude, float *Phase);
float getMin(float arr[], int total);
float getMax(float arr[], int total);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_spectrum:
//
// Compute magnitude and phase spectrum from input image I1.
// Save results in Imag and Iphase.
//
void
HW_spectrum(ImagePtr I1, ImagePtr Imag, ImagePtr Iphase)
{
     int w = I1->width();                              // Getting Width
     int h = I1->height();                             // Getting Height
     int total = w * h;                                // Getting Total, w * h 
     // compute FFT of the input image

// PUT YOUR CODE HERE...
     ImagePtr I2;                                      // In Case of Padding
     if (ceil(log2(w)) != floor(log2(w)) || ceil(log2(h)) != floor(log2(h))) {  // Check If Padding Is Needed
          paddedImage(I1, I2);                                                  // Pads I1 and Results in I2
          w = I2->width();                                                      // New Width
          h = I2->height();                                                     // New Height
          total = w * h;                                                        // New Total
     }
     else {
          I2 = I1;                                                              // I2 = I1
     }
#if 0 // 0 For Spectrum and 1 To Output Original Image. For Testing Purposes
     ChannelPtr<uchar> p1, p2, p3; // p1 points to I1 channels and p2 to I2 channels
     int type;
     IP_copyImageHeader(I2, Imag);
     IP_copyImageHeader(I2, Iphase);
     int newTotal = I2->width() * I2->height();
     for (int ch = 0; IP_getChannel(I2, ch, p1, type); ch++) {
          IP_getChannel(Imag, ch, p2, type);
          IP_getChannel(Iphase, ch, p3, type);
          for (int i = 0; i < newTotal; i++) {
               *p2++ = *p1++;
               *p3++ = 0;
          }
     }
#else
     ImagePtr Image1, Image2;                          // Get Two Different Images For Row and Column
     Image1->allocImage(w, h, FFT_TYPE);               // Allocate Image1
     Image2->allocImage(w, h, FFT_TYPE);               // Allocate Image2
     fft1DRow(I2, Image1);                             // FFT For Row
     fft1DColumn(I2, Image1, Image2);                  // FFT For Column
     
     // compute magnitute and phase spectrums from FFT image
     /*
     ImagePtr Im = NEWIMAGE;
     ImagePtr Ip = NEWIMAGE;
     */
     // PUT YOUR CODE HERE...
     float *Magnitude = new float[total];              // Declaring Magnitude
     float *Phase = new float[total];                  // Declairing Phase
     fft1DMagPhase(I2, Image2, Magnitude, Phase);      // Getting Magitude And Phase, Changes By Reference
     // find min and max of magnitude and phase spectrums

// PUT YOUR CODE HERE...
     float minMagnitude = getMin(Magnitude, total);    // Uses Magnitude To Get Min Magnitude
     float maxMagnitude = getMax(Magnitude, total);    // Uses Magnitude To Get Max Magnitude
     float minPhase = getMin(Phase, total);            // Uses Phase To Get Min Phase
     float maxPhase = getMax(Phase, total);            // Uses Phase To Get Max Phase
     /*
     // set imagetypes for single 8-bit channel
     Imag  ->setImageType(BW_IMAGE);
     Iphase->setImageType(BW_IMAGE);

     // get channel pointers for input magnitude and phase spectrums
     ChannelPtr<float>   magF = Im[0];
     ChannelPtr<float> phaseF = Ip[0];

     // get channel pointers for output magnitude and phase spectrums
     ChannelPtr<uchar> Pmag    = Imag  [0];
     ChannelPtr<uchar> Pphase  = Iphase[0];
     */
     // scale magnitude and phase to fit between [0, 255]

     // PUT YOUR CODE HERE...

     IP_copyImageHeader(I2, Imag);                                                                                      // copy image header (width, height) of input image I2 to output image Imag
     IP_copyImageHeader(I2, Iphase);                                                                                    // copy image header (width, height) of input image I2 to output image IPhase
     ChannelPtr<uchar> Pmag, Pphase;                                                                                    // declarations for image channel pointers and datatype
     maxMagnitude /= 256;                                                                                               // Normalize Max By 256
     int type;
     for (int ch = 0; IP_getChannel(Imag, ch, Pmag, type); ch++) {
          for (int i = 0; i < total; i++) {
               *Pmag++ = CLIP((Magnitude[i] - minMagnitude) / (maxMagnitude - minMagnitude) * MaxGray, 0, MaxGray);     // Scales Magnitude and Clipped
          }
     }
     
     for (int ch = 0; IP_getChannel(Iphase, ch, Pphase, type); ch++) {
          for (int i = 0; i < total; i++) {
               *Pphase++ = CLIP((Phase[i] - minPhase) / (maxPhase - minPhase) * MaxGray, 0, MaxGray);                   // Scales Phase and Clipped
          }
     }
#endif
}

void paddedImage(ImagePtr I1, ImagePtr I1Padded) {
#if 0 // 0 For Padding Around, 1 For Padding Just To The Right and Bottom. For Testing Purposes
     int w = I1->width();                              // Getting Width
     int h = I1->height();                             // Getting Height
     int zerosW = 0;
     int upperBase = floor(log2(w)) + 1;
     zerosW = pow(2, upperBase) - w;            // Number of zeros to append
     int zerosH = 0;
     upperBase = floor(log2(h)) + 1;
     zerosH = pow(2, upperBase) - h;            // Number of zeros to append

     int paddingH = zerosH / 2;                 // Amount Need To Pad Top and Bottom
     int paddingW = zerosW / 2;                 // Amount Need To Pad Left and Right

     int newH = h + zerosH;                     // Max Height Adding Zeros
     int newW = w + zerosW;                     // Max Width Adding Zeros
     I1Padded->allocImage(newW, newH, BW_TYPE); // New Padded Image, With New Width and New Height
     ChannelPtr<uchar> in, out, start;

     int type;
     //Initialize buffer
     for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
          IP_getChannel(I1Padded, ch, out, type);        // get output pointer for channel ch
          for (int i = 0; i < newH; i++) { // Setting Up Buffer Values, Starting From First Row
               for (int j = 0; j < newW; j++) { // Setting Up Buffer Values, Starting From First Column
                    if (i < paddingH || i >= paddingH + h || j < paddingW || j >= paddingW + w) { // Buffer Is Outside The Image, The Padding Part
                         *out++ = 0; // Set Buffer Values To Zero
                    }
                    else { // Inside Image
                         *out++ = *in++; //  Set Buffer Values To Same As Image Values
                    }
               }
          }
     }
     /*
     if (zerosH % 2 != 0) {
          //ADD extra row of zeros
          for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
               IP_getChannel(I1Padded, ch, out, type);        // get output pointer for channel ch
               int lastRow = (w + zerosW)*(h + zerosH - 1);
               for (int j = 0; j < w + zerosW; j++) {
                    out[lastRow + j] = 0;
               }
          }
     }

     if (zerosW % 2 != 0) {
          //Add extra column of zeros
          for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
               IP_getChannel(I1Padded, ch, out, type);        // get output pointer for channel ch
               int lastColumn = w + zerosW - 1;
               for (int j = 0; j < w + zerosW; j++) {
                    int index = lastColumn * (j + 1);
                    out[index] = 0;
               }
          }
     }
     */
#else
     int w = I1->width();                              // Getting Width
     int h = I1->height();                             // Getting Height
     int zerosW = 0;
     int upperBase = floor(log2(w)) + 1;
     zerosW = pow(2, upperBase) - w;            // Number of zeros to append
     int zerosH = 0;
     upperBase = floor(log2(h)) + 1;
     zerosH = pow(2, upperBase) - h;            // Number of zeros to append

     int newH = h + zerosH;                     // Max Height Adding Zeros
     int newW = w + zerosW;                     // Max Width Adding Zeros
     I1Padded->allocImage(newW, newH, BW_TYPE); // New Padded Image, With New Width and New Height
     ChannelPtr<uchar> in, out, start;

     int type;
     //Initialize buffer
     for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
          IP_getChannel(I1Padded, ch, out, type);        // get output pointer for channel ch
          for (int i = 0; i < newH; i++) { // Runs Through Entire Height
               for (int j = 0; j < newW; j++) { // Runs Through Entire Width
                    if(i < h && j < w){ // Output Is Original, At Original Position
                         *out++ = *in++; //  Output Is Input
                    }
                    else { // Pads To The Right And Bottom
                         *out++ = 0; // Padded Value Is Zero
                    }
               }
          }
     }


#endif
}

void fft1D(complexP *q1, int dir, complexP *q2) { // Converted to cpp from fft1D.c
     int i, N, N2;
     float *r1, *i1, *r2, *i2, *ra, *ia, *rb, *ib;
     float FCTR, fctr, a, b, c, s;
     complexP *qa, *qb;

     N = q1->len;
     r1 = q1->real;
     i1 = q1->imag;
     r2 = q2->real;
     i2 = q2->imag;

     if (N == 2) {
          a = r1[0] + r1[1];	// F(0)=f(0)+f(1);F(1)=f(0)-f(1)
          b = i1[0] + i1[1];	// a,b needed when r1=r2
          r2[1] = r1[0] - r1[1];
          i2[1] = i1[0] - i1[1];
          r2[0] = a;
          i2[0] = b;
     }
     else {
          N2 = N / 2;

          complexP first;
          qa = &first;
          qa->len = N2;
          qa->real = new float[N2];
          qa->imag = new float[N2];

          complexP second;
          qb = &second;
          qb->len = N2;
          qb->real = new float[N2];
          qb->imag = new float[N2];

          ra = qa->real;
          ia = qa->imag;
          rb = qb->real;
          ib = qb->imag;

          // split list into 2 halves; even and odd
          for (i = 0; i < N2; i++) {
               ra[i] = *r1++;
               ia[i] = *i1++;
               rb[i] = *r1++;
               ib[i] = *i1++;
          }

          // compute fft on both lists
          fft1D(qa, dir, qa);
          fft1D(qb, dir, qb);

          // build up coefficients
          if (!dir)	// forward
               FCTR = -2 * PI / N;
          else
               FCTR = 2 * PI / N;
          for (fctr = i = 0; i < N2; i++, fctr += FCTR) {
               c = cos(fctr);
               s = sin(fctr);
               a = c * rb[i] - s * ib[i];
               r2[i] = ra[i] + a;
               r2[i + N2] = ra[i] - a;

               a = s * rb[i] + c * ib[i];
               i2[i] = ia[i] + a;
               i2[i + N2] = ia[i] - a;
          }
          delete[] qa->real; // Free Memory
          delete[] qa->imag; // Free Memory
          delete[] qb->real; // Free Memory
          delete[] qb->imag; // Free Memory
     } 

     if (!dir) {	// inverse : divide by logN
          for (i = 0; i < N; i++) {
               q2->real[i] = q2->real[i] / 2;
               q2->imag[i] = q2->imag[i] / 2;
          }
     }
} 

void fft1DRow(ImagePtr I1, ImagePtr Image1) {
     int w = I1->width();                         // Getting Width
     int h = I1->height();                        // Getting Height
     ChannelPtr<float> real, img;                 
     ChannelPtr<uchar> p1;                        // declarations for image channel pointers and datatype
     int type;
     real = Image1[0];                            // Image1[0] Is Real
     img = Image1[1];                             // Image1[1] Is Imaginary

     complexP c1, c2, *q1, *q2;
     q1 = &c1;
     q2 = &c2;
     q1->len = w;                            // length of complex number list
     q1->real = new float[w];                // pointer to real number list
     q1->imag = new float[w];                // pointer to imaginary number list
     q2->len = w;                            // length of complex number list
     q2->real = new float[w];                // pointer to real number list
     q2->imag = new float[w];                // pointer to imaginary number list

     for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {          
          for (int row = 0; row < h; row++) { // FFT Row by Row
               for (int column = 0; column < q1->len; column++){ // FFT Columns
                    q1->real[column] = *p1++;     // Storing Real Values For Current Row into q1 -> real
                    q1->imag[column] = 0;         // Storing Imaginary Value For Current Row into q1 -> imag. It's 0 Because Original Has 0 Imaginary Values
               }

               fft1D(q1, 0, q2);                  // Apply FFT 1D To Current Row and Output To q2.

               for (int i = 0; i < q2->len; i++){ // Run Through Output
                    *real++ = q2->real[i];        // Sets Real Output to Image1
                    *img++ = q2->imag[i];         // Sets Imaginary Output to Image1
               }
          } 
          delete[] q1->real;                      // Free Memory
          delete[] q1->imag;                      // Free Memory
          delete[] q2->real;                      // Free Memory
          delete[] q2->imag;                      // Free Memory
     } 
}

void fft1DColumn(ImagePtr I1, ImagePtr Image1, ImagePtr Image2) {
     int w = I1->width();                              // Getting Width
     int h = I1->height();                             // Getting Height
     ChannelPtr<float> real, img, real2, img2;         
     real = Image1[0];                                 // Image1[0] Is Real
     img = Image1[1];                                  // Image1[1] Is Imaginary
     real2 = Image2[0];                                // Image2[0] Is Real
     img2 = Image2[1];                                 // Image2[1] Is Imaginary

     complexP c1, c2, *q1, *q2;
     q1 = &c1;
     q2 = &c2;
     q1->len = w;                            // length of complex number list
     q1->real = new float[w];                // pointer to real number list
     q1->imag = new float[w];                // pointer to imaginary number list
     q2->len = w;                            // length of complex number list
     q2->real = new float[w];                // pointer to real number list
     q2->imag = new float[w];                // pointer to imaginary number list

     for (int column = 0; column < w; column++) { // FFT Column By Column
          ChannelPtr<float> temp_real = real;     // Creates Temp For Real For Future Row Calculations
          ChannelPtr<float> temp_img = img;       // Creates Temp For Imaginary For Future Row Calculations

          for (int row = 0; row < h; row++) {     // FFT Rows
               q1->real[row] = *temp_real;        // Storing Real Values For Current Column into q1 -> real
               q1->imag[row] = *temp_img;         // Storing Imaginary Value For Current Column into q1 -> imag.
               if (row < h - 1) {                 // Keep Storing Until Last Row Of The Column
                    temp_real += w;               // Goes To Next Row
                    temp_img += w;                // Goes To Next Row
               }
          }

          fft1D(q1, 0, q2);                       // Apply FFT 1D To Current Column and Output To q2.

          for (int i = 0; i < h; i++) {           // Run Through Output
               *real2++ = q2->real[i];        // Sets Real Output to Image2
               *img2++ = q2->imag[i];         // Sets Imaginary Output to Image2
          }
          real++;                             // Next Column For Real
          img++;                              // Next Column For Imaginary
     } 
     delete[] q1->real;                      // Free Memory
     delete[] q1->imag;                      // Free Memory
     delete[] q2->real;                      // Free Memory
     delete[] q2->imag;                      // Free Memory
}

void fft1DMagPhase(ImagePtr I1, ImagePtr Image2, float *Magnitude, float *Phase) {
     int w = I1->width();                                                       // Getting Width
     int h = I1->height();                                                      // Getting Height
     int total = w * h;                                                         // Getting Total, w * h 
     ChannelPtr<float> real2, img2;
     real2 = Image2[0];                                                         // Image2[0] Is Real
     img2 = Image2[1];                                                          // Image2[1] Is Imaginary

     for (int i = 0; i < total; i++, real2++, img2++) {                         // From 0 to Total, Increment i++ and real2++ and img2++
          Magnitude[i] = sqrt(pow(*real2, 2) + pow(*img2, 2));                   // Magnitude Formula
          Phase[i] = atan2(*img2, *real2);                                      // Phase Formula
     }
}

float getMin(float arr[], int total) {
     float minValue = arr[0];                // Assume First Value is Min
     for (int i = 1; i < total; i++)         // For Loop To Compare Each Value
          minValue = min(minValue, arr[i]);  // Min is set to minValue
     return minValue;                        // Output minValue
}

float getMax(float arr[], int total) {
     float maxValue = arr[0];                // Assume First Value is Max
     for (int i = 1; i < total; i++)         // For Loop To Compare Each Value
          maxValue = max(maxValue, arr[i]);  // Max is set to maxValue
     return maxValue;                        // Output maxValue
}