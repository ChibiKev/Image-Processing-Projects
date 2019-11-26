

#include "IP.h"
#include <stdio.h>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdint.h>

using namespace std;
using namespace IP;

// extern void HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase);
// extern void HW_magPhase2FFT(ImagePtr Imag, ImagePtr Iphase, ImagePtr Ifft);

struct complexP {
    int len;        // length of complex number list
    float * real;    // pointer to real number list
    float * imag;    // pointer to imaginary number list
};

// Function defined in my HW_spectrum.cpp
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
          a = r1[0] + r1[1];    // F(0)=f(0)+f(1);F(1)=f(0)-f(1)
          b = i1[0] + i1[1];    // a,b needed when r1=r2
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
          if (!dir)    // forward
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

     if (!dir) {    // inverse : divide by logN
          for (i = 0; i < N; i++) {
               q2->real[i] = q2->real[i] / 2;
               q2->imag[i] = q2->imag[i] / 2;
          }
     }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_swap:
//
// swap phase of I1 with I2.
// I1_mag   + I2_phase -> II1
// I1_phase + I2_mag   -> II2
void
HW_swap(ImagePtr I1, ImagePtr I2, ImagePtr II1, ImagePtr II2)
{
    ofstream file1, file2;

    // Debugging files.
    file1.open("File1.txt");
    file2.open("File2.txt");

    ImagePtr Ifft1, Ifft2, IinvFFT1, IinvFFT2;
    ImagePtr Imag1, Iphase1, Imag2, Iphase2;

    IP_copyImageHeader(I1, II1);
    IP_copyImageHeader(I1, II2);

    ChannelPtr<uchar> pII1, pII2;

    int w = I1->width();
    int h = I1->height();
    int total = w * h;

    ChannelPtr<uchar> p1, p2;
    int type;

    //-------------------------------------------------------------------------
    //    Compute FFT of I1: IComplex2 stores the FFT for I1.
    //-------------------------------------------------------------------------
    ImagePtr IComplex;  // Store result of FFT to rows.
    ImagePtr IComplex2; // Store result of FFT to columns.

    IComplex->allocImage(w, h, FFT_TYPE);
    IComplex2->allocImage(w, h, FFT_TYPE);

    ChannelPtr<float> real , img;  // Pointers for IComplex.
    ChannelPtr<float> real2, img2; // Pointers for IComplex2.

    // Compute FFT to rows.
    for (int ch = 0; IP_getChannel(I1, ch, p1, type); ++ch)
    {
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

        for (int row = 0; row < h; ++row)
        {
            for (int column = 0; column < w; ++column)
            {
                q1->real[column] = *p1++;
                q1->imag[column] = 0;
            }

            fft1D(q1, 0, q2);

            for (int i = 0; i < q2->len; ++i)
            {
                *real++ = q2->real[i];
                *img++ = q2->imag[i];
            }
        }

        delete[] q1->real;
        delete[] q1->imag;
        delete[] q2->real;
        delete[] q2->imag;
    }

    // Compute FFT to columns.
    {
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

        for (int column = 0; column < w; ++column)
        {
            ChannelPtr<float> temp_real = real;
            ChannelPtr<float> temp_img = img;

            for (int row = 0; row < h; ++row)
            {
                q1->real[row] = *temp_real;
                q1->imag[row] = *temp_img;

                if (row < h - 1)
                {
                    temp_real += w;
                    temp_img += w;
                }
            }

            fft1D(q1, 0, q2);

            for (int i = 0; i < q2->len; ++i)
            {
                *real2++ = q2->real[i];
                *img2++ = q2->imag[i];
            }

            real++;
            img++;
        }

        delete[] q1->real;
        delete[] q1->imag;
        delete[] q2->real;
        delete[] q2->imag;
    }

    //-------------------------------------------------------------------------
    //    Compute FFT of I2: IComplex4 contains the FFT for I2.
    //-------------------------------------------------------------------------
    ImagePtr IComplex3; // Store result of FFT to rows.
    ImagePtr IComplex4; // Store result of FFT to columns.

    IComplex3->allocImage(w, h, FFT_TYPE);
    IComplex4->allocImage(w, h, FFT_TYPE);

    // Compute FFT to rows.
    for (int ch = 0; IP_getChannel(I2, ch, p2, type); ++ch)
    {
        real = IComplex3[0];
        img = IComplex3[1];

        complexP c1, c2, *q1, *q2;
        q1 = &c1;
        q2 = &c2;
        q1->len = w;
        q1->real = new float[w];
        q1->imag = new float[w];
        q2->len = w;
        q2->real = new float[w];
        q2->imag = new float[w];

        for (int row = 0; row < h; ++row)
        {
            for (int column = 0; column < w; ++column)
            {
                q1->real[column] = *p2++;
                q1->imag[column] = 0;
            }

            fft1D(q1, 0, q2);

            for (int i = 0; i < q2->len; ++i)
            {
                *real++ = q2->real[i];
                *img++ = q2->imag[i];
            }
        }

        delete[] q1->real;
        delete[] q1->imag;
        delete[] q2->real;
        delete[] q2->imag;
    }
    
    // Compute FFT to columns.
    {
        real = IComplex3[0];
        img = IComplex3[1];
        real2 = IComplex4[0];
        img2 = IComplex4[1];

        complexP c1, c2, *q1, *q2;
        q1 = &c1;
        q2 = &c2;
        q1->len = w;
        q1->real = new float[w];
        q1->imag = new float[w];
        q2->len = w;
        q2->real = new float[w];
        q2->imag = new float[w];

        for (int column = 0; column < w; ++column)
        {
            ChannelPtr<float> temp_real = real;
            ChannelPtr<float> temp_img = img;

            for (int row = 0; row < h; ++row)
            {
                q1->real[row] = *temp_real;
                q1->imag[row] = *temp_img;

                if (row < h - 1)
                {
                    temp_real += w;
                    temp_img += w;
                }
            }

            fft1D(q1, 0, q2);

            for (int i = 0; i < q2->len; ++i)
            {
                *real2++ = q2->real[i];
                *img2++ = q2->imag[i];
            }

            real++;
            img++;
        }

        delete[] q1->real;
        delete[] q1->imag;
        delete[] q2->real;
        delete[] q2->imag;
    }

    //-------------------------------------------------------------------------
    //    Compute magnitude and phase from real and imaginary of FFT.
    //       * FFT of I1 is in IComplex2.
    //       * FFT of I2 is in IComplex4.
    //       * I1 has magnitude1 and phase1.
    //       * I2 has magnitude2 and phase2.
    //-------------------------------------------------------------------------
    real  = IComplex2[0];
    img   = IComplex2[1];
    real2 = IComplex4[0];
    img2  = IComplex4[1];

    float *magnitude1 = new float[total];
    float *phase1 = new float[total];
    float *magnitude2 = new float[total];
    float *phase2 = new float[total];

    for (int i = 0; i < total; ++i)
    {
        float r2 = pow(*real, 2);
        float i2 = pow(*img, 2);
        float res = r2 + i2;

        magnitude1[i] = sqrt(res);
        phase1[i] = atan2(*img, *real);

        r2 = pow(*real2, 2);
        i2 = pow(*img2, 2);
        res = r2 + i2;

        magnitude2[i] = sqrt(res);
        phase2[i] = atan2(*img2, *real2);

        real++;
        img++;
        real2++;
        img2++;
    }

    //-------------------------------------------------------------------------
    // Swap phases of FFT1 and FFT2:
    //    * II1: magnitude1 and phase2.
    //-------------------------------------------------------------------------

    // Reusing IComplex2 to store conversions.
    real = IComplex2[0];
    img = IComplex2[1];

    for (int i = 0; i < total; ++i)
    {
        // Convert magnitude and phase to complex number.
        *real++ = magnitude1[i] * cos(phase2[i]);
        *img++ = magnitude1[i] * sin(phase2[i]);
    }

    // IFFT by row.
    {
        real = IComplex2[0];
        img = IComplex2[1];
        real2 = IComplex[0];
        img2 = IComplex[1];

        complexP c1, c2, *q1, *q2;
        q1 = &c1;
        q2 = &c2;
        q1->len = w;
        q1->real = new float[w];
        q1->imag = new float[w];
        q2->len = w;
        q2->real = new float[w];
        q2->imag = new float[w];

        for (int row = 0; row < h; ++row)
        {
            for (int column = 0; column < w; ++column)
            {
                q1->real[column] = *real++;
                q1->imag[column] = *img++;
            }

            fft1D(q1, 1, q2);

            for (int i = 0; i < q2->len; ++i)
            {
                *real2++ = q2->real[i];
                *img2++ = q2->imag[i];
            }
        }
    }

    // IFFT by column.
    for (int ch = 0; IP_getChannel(II1, ch, pII1, type); ++ch)
    {
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

        for (int column = 0; column < w; ++column)
        {
            ChannelPtr<float> temp_real = real;
            ChannelPtr<float> temp_img = img;

            for (int row = 0; row < h; ++row)
            {
                q1->real[row] = *temp_real;
                q1->imag[row] = *temp_img;

                if (row < h - 1)
                {
                    temp_real += w;
                    temp_img += w;
                }
            }

            fft1D(q1, 1, q2);

            for (int i = 0; i < q2->len; ++i)
            {
                *pII1++ = CLIP(q2->real[i], 0, MaxGray);
                // *pII1++ = static_cast<unsigned char> (q2->real[i]);
            }

            real++;
            img++;
        }
    }

    //-------------------------------------------------------------------------
    // Swap Magnitude of FFT1 and FFT2:
    //    * II2: magnitude2 and phase1.
    //-------------------------------------------------------------------------

    // Reusing IComplex2 to store conversions.
    real = IComplex2[0];
    img = IComplex2[1];

    for (int i = 0; i < total; ++i)
    {
        // Convert magnitude and phase to complex number.
        *real++ = magnitude2[i] * cos(phase1[i]);
        *img++ = magnitude2[i] * sin(phase1[i]);
    }

    // IFFT by row.
    {
        real = IComplex2[0];
        img = IComplex2[1];
        real2 = IComplex[0];
        img2 = IComplex[1];

        complexP c1, c2, *q1, *q2;
        q1 = &c1;
        q2 = &c2;
        q1->len = w;
        q1->real = new float[w];
        q1->imag = new float[w];
        q2->len = w;
        q2->real = new float[w];
        q2->imag = new float[w];

        for (int row = 0; row < h; ++row)
        {
            for (int column = 0; column < w; ++column)
            {
                q1->real[column] = *real++;
                q1->imag[column] = *img++;
            }

            fft1D(q1, 1, q2);

            for (int i = 0; i < q2->len; ++i)
            {
                *real2++ = q2->real[i];
                *img2++ = q2->imag[i];
            }
        }
    }

    // IFFT by column.
    for (int ch = 0; IP_getChannel(II2, ch, pII2, type); ++ch)
    {
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

        for (int column = 0; column < w; ++column)
        {
            ChannelPtr<float> temp_real = real;
            ChannelPtr<float> temp_img = img;

            for (int row = 0; row < h; ++row)
            {
                q1->real[row] = *temp_real;
                q1->imag[row] = *temp_img;

                if (row < h - 1)
                {
                    temp_real += w;
                    temp_img += w;
                }
            }

            fft1D(q1, 1, q2);

            for (int i = 0; i < q2->len; ++i)
            {
                *pII2++ = CLIP(q2->real[i], 0, MaxGray);
                // *pII2++ = static_cast<unsigned char> (q2->real[i]);
            }

            real++;
            img++;
        }
    }
}

