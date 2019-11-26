#include "IP.h"
#include <stdio.h>
#include <string>
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
void fft1D(complexP *q1, int dir, complexP *q2);
void fft1DRow(ImagePtr I1, ImagePtr IComplex);
void fft1DColumn(ImagePtr I1, ImagePtr IComplex, ImagePtr IComplex2);
void fft1DMagPhase(ImagePtr I1, ImagePtr IComplex2, float *magnitude, float *phase);
float getMin(float arr[], int n);
float getMax(float arr[], int n);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_spectrum:
//
// Compute magnitude and phase spectrum from input image I1.
// Save results in Imag and Iphase.
//
void
HW_spectrum(ImagePtr I1, ImagePtr Imag, ImagePtr Iphase)
{
     int w = I1->width();
     int h = I1->height();
     int total = w * h;
     // compute FFT of the input image

// PUT YOUR CODE HERE...
     ImagePtr IComplex, IComplex2;
     IComplex->allocImage(w, h, FFT_TYPE);
     IComplex2->allocImage(w, h, FFT_TYPE);
     ChannelPtr<float> real, img, real2, img2;
     int type;
     // * Compute FFT1D via rows.
     fft1DRow(I1, IComplex);

     // * Compute FFT1D via columns.
     fft1DColumn(I1, IComplex, IComplex2);

     // compute magnitute and phase spectrums from FFT image
     /*
     ImagePtr Im = NEWIMAGE;
     ImagePtr Ip = NEWIMAGE;
     */
     // PUT YOUR CODE HERE...
     float *magnitude = new float[total];
     float *phase = new float[total];
     fft1DMagPhase(I1, IComplex2, magnitude, phase);
     // find min and max of magnitude and phase spectrums

// PUT YOUR CODE HERE...
     float minMagnitude = getMin(magnitude, total);
     float maxMagnitude = getMax(magnitude, total);
     maxMagnitude = maxMagnitude / 256;
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
     IP_copyImageHeader(I1, Imag);
     ChannelPtr<uchar> Pmag;
     // PUT YOUR CODE HERE...
     for (int ch = 0; IP_getChannel(Imag, ch, Pmag, type); ch++)
     {
          for (int i = 0; i < total; ++i)
          {
               *Pmag++ = CLIP((magnitude[i] - minMagnitude) / (maxMagnitude - minMagnitude) * MaxGray, 0, MaxGray);
          }
     }

     //find min and max of phase 
     float minPhase = getMin(phase, total);
     float maxPhase = getMax(phase, total);

     //scale phase to fit between [0, 255]
     IP_copyImageHeader(I1, Iphase);
     ChannelPtr<uchar> Pphase;
     for (int ch = 0; IP_getChannel(Iphase, ch, Pphase, type); ch++)
     {
          for (int i = 0; i < total; ++i)
          {
               *Pphase++ = CLIP((phase[i] - minPhase) / (maxPhase - minPhase) * MaxGray, 0, MaxGray);
          }
     }

}

void fft1D(complexP *q1, int dir, complexP *q2) {
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
          // Prevent Memory Leak
          delete[] qa->real;
          delete[] qa->imag;
          delete[] qb->real;
          delete[] qb->imag;
     }

     if (!dir) {	// inverse : divide by logN
          for (i = 0; i < N; i++) {
               q2->real[i] = q2->real[i] / 2;
               q2->imag[i] = q2->imag[i] / 2;
          }
     }
}

void fft1DRow(ImagePtr I1, ImagePtr IComplex) {
     int w = I1->width();
     int h = I1->height();
     ChannelPtr<float> real, img, real2, img2;
     ChannelPtr<uchar> p1;
     int type;
     for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
          real = IComplex[0];
          img = IComplex[1];

          complexP c1, c2, *q1, *q2;
          q1 = &c1;
          q2 = &c2;
          q1->len = w;
          q1->real = new float[w];
          q1->imag = new float[w];
          q2->len = w;
          q2->real = new float[w];
          q2->imag = new float[w];

          // Visit each row
          for (int row = 0; row < h; row++)
          {
               for (int column = 0; column < q1->len; column++)
               {
                    // Put all pixel values from current row into q1->real.
                    q1->real[column] = *p1++;

                    // Since imaginary channel don't exist, we put 0's.
                    q1->imag[column] = 0;
               }

               // Now we apply FFT to this row.
               fft1D(q1, 0, q2);

               // Transfer the results in q2 to IComplex.
               for (int i = 0; i < q2->len; i++)
               {
                    *real++ = q2->real[i];
                    *img++ = q2->imag[i];
               }
          } // End visit loops.
          delete[] q1->real;
          delete[] q1->imag;
          delete[] q2->real;
          delete[] q2->imag;
     } // End IP_getChannel loop.
}

void fft1DColumn(ImagePtr I1, ImagePtr IComplex, ImagePtr IComplex2) {
     int w = I1->width();
     int h = I1->height();
     ChannelPtr<float> real, img, real2, img2;
     real = IComplex[0];
     img = IComplex[1];
     real2 = IComplex2[0];
     img2 = IComplex2[1];

     complexP c1, c2, *q1, *q2;
     q1 = &c1;
     q2 = &c2;
     q1->len = w;
     q1->real = new float[w];
     q1->imag = new float[w];
     q2->len = w;
     q2->real = new float[w];
     q2->imag = new float[w];

     for (int column = 0; column < w; column++)
     {
          // Temporary pointers to loop through current column.
          ChannelPtr<float> temp_real = real;
          ChannelPtr<float> temp_img = img;

          for (int row = 0; row < h; row++)
          {
               q1->real[row] = *temp_real;
               q1->imag[row] = *temp_img;

               // Go to next element in this column until last element.
               if (row < h - 1)
               {
                    temp_real += w;
                    temp_img += w;
               }
          }

          // Apply FFT1D to this column.
          fft1D(q1, 0, q2);

          // Transfer the results in q2 to IComplex2.
          for (int i = 0; i < q2->len; i++)
          {
               *real2++ = q2->real[i];
               *img2++ = q2->imag[i];
          }

          // After visitng all pixels of current column, go to next column.
          real++;
          img++;
     } // End visit loops.

     delete[] q1->real;
     delete[] q1->imag;
     delete[] q2->real;
     delete[] q2->imag;
}

void fft1DMagPhase(ImagePtr I1, ImagePtr IComplex2, float *magnitude, float *phase) {
     int w = I1->width();
     int h = I1->height();
     int total = w * h;
     ChannelPtr<float> real2, img2;
     real2 = IComplex2[0];
     img2 = IComplex2[1];

     for (int i = 0; i < total; i++)
     {
          float r2 = pow(*real2, 2);
          float i2 = pow(*img2, 2);
          float res = r2 + i2;

          magnitude[i] = sqrt(res);
          phase[i] = atan2(*img2, *real2);

          real2++;
          img2++;
     }
}

float getMin(float arr[], int n)
{
     float res = arr[0];
     for (int i = 1; i < n; i++)
          res = min(res, arr[i]);
     return res;
}

float getMax(float arr[], int n)
{
     float res = arr[0];
     for (int i = 1; i < n; i++)
          res = max(res, arr[i]);
     return res;
}
