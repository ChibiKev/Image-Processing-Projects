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

extern void HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase);

struct Complex {
    int len;        // length of complex number list
    float * real;    // pointer to real number list
    float * imag;    // pointer to imaginary number list
};

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

void fft1D(Complex *q1, int dir, Complex *q2)
{
    int i, N, N2;
    float *r1, *i1, *r2, *i2, *ra, *ia, *rb, *ib;
    double FCTR, fctr, a, b, c, s, num[2];

    Complex ca, cb;
    Complex *qa, *qb;

    qa = &ca;
    qb = &cb;

    N = q1->len;
    r1 = q1->real;
    i1 = q1->imag;
    r2 = q2->real;
    i2 = q2->imag;

    if (N == 2)
    {
        a = r1[0] + r1[1];
        b = i1[0] + i1[1];
        r2[1] = r1[0] - r1[1];
        i2[1] = i1[0] - i1[1];
        r2[0] = a;
        i2[0] = b;
    }
    else
    {
        N2 = N / 2;
        
        Complex aa;
        qa = &aa;

        qa->len = N2;
        qa->real = new float[N2];
        qa->imag = new float[N2];

        Complex bb;
        qb = &bb;
        qb->len = N2;
        qb->real = new float[N2];
        qb->imag = new float[N2];

        ra = qa->real;
        ia = qa->imag;
        rb = qb->real;
        ib = qb->imag;

        // Split list into 2 halves; even and odd.
        for (i = 0; i<N2; i++)
        {
            ra[i] = *r1++;
            ia[i] = *i1++;
            rb[i] = *r1++;
            ib[i] = *i1++;
        }

        // Compute on both lists.
        fft1D(qa, dir, qa);
        fft1D(qb, dir, qb);

        // Build up coefficients.
        if (!dir) // Forward
            FCTR = -2 * PI / N;
        else FCTR = 2 * PI / N;

        for (fctr = i = 0; i < N2; i++, fctr += FCTR)
        {
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

    if (!dir)
    { // inverse : divide by logN
        for (i = 0; i < N; i++)
        {
            q2->real[i] = q2->real[i] / 2;
            q2->imag[i] = q2->imag[i] / 2;
        }
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_Spectrum:
//
// Compute magnitude and phase of I1 fourier transform.
//
//
void
HW_spectrum(ImagePtr I1, ImagePtr Imag, ImagePtr Iphase)
{
    ofstream file;
    file.open("meow.txt");

    int w = I1->width();
    int h = I1->height();
    int total = w * h;

    ChannelPtr<uchar> p1;
    int type;

    // Compute FFT of the input image:
    ImagePtr IComplex, IComplex2; // To store Real and Imaginary float values.
    IComplex->allocImage(w, h, FFT_TYPE);
    IComplex2->allocImage(w, h, FFT_TYPE);

    // The image IComplex now has two float channels.
    ChannelPtr<float> real, img, real2, img2;

    // * Compute FFT1D via rows.
    for (int ch = 0; IP_getChannel(I1, ch, p1, type); ++ch)
    {
        real = IComplex[0];
        img = IComplex[1];

        Complex c1, c2, *q1, *q2;
        q1 = &c1;
        q2 = &c2;
        q1->len = w;
        q1->real = new float[w];
        q1->imag = new float[w];
        q2->len = w;
        q2->real = new float[w];
        q2->imag = new float[w];

        // Visit each row
        for (int row = 0; row < h; ++row)
        {
            for (int column = 0; column < w; ++column)
            {
                // Put all pixel values from current row into q1->real.
                q1->real[column] = *p1++;

                // Since imaginary channel don't exist, we put 0's.
                q1->imag[column] = 0;
            }

            // Now we apply FFT to this row.
            fft1D(q1, 0, q2);
            
            // Transfer the results in q2 to IComplex.
            for (int i = 0; i < q2->len; ++i)
            {
                *real++ = q2->real[i];
                *img++  = q2->imag[i];
            }
        } // End visit loops.
        delete[] q1->real;
        delete[] q1->imag;
        delete[] q2->real;
        delete[] q2->imag;
    } // End IP_getChannel loop.

    // * Compute FFT1D via columns.
    real = IComplex[0];
    img = IComplex[1];
    real2 = IComplex2[0];
    img2 = IComplex2[1];

    Complex c1, c2, *q1, *q2;
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
        // Temporary pointers to loop through current column.
        ChannelPtr<float> temp_real = real;
        ChannelPtr<float> temp_img = img;

        for (int row = 0; row < h; ++row)
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
        for (int i = 0; i < q2->len; ++i)
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

    // compute magnitute and phase of FFT
    real2 = IComplex2[0];
    img2 = IComplex2[1];

    float *magnitude = new float[total];
    float *phase = new float[total];

    for (int i = 0; i < total; ++i)
    {
        float r2 = pow(*real2, 2);
        float i2 = pow(*img2, 2);
        float res = r2 + i2;

        magnitude[i] = sqrt(res);
        phase[i] = atan2(*img2, *real2);

        real2++;
        img2++;
    }

    // find min and max of magnitude
    float minMagnitude = getMin(magnitude, total);
    float maxMagnitude = getMax(magnitude, total);

    // Taking the advice from: https://piazza.com/class/jkqfs1lkw579i?cid=36.
    // "You somehow have to normalize the max by 256. That worked for me"
    maxMagnitude = maxMagnitude / 256;

    // allocate uchar image for displaying magnitude
    IP_copyImageHeader(I1, Imag);
    ChannelPtr<uchar> pImag;
    
    //scale magnitude to fit between [0, 255]
    for (int ch = 0; IP_getChannel(Imag, ch, pImag, type); ++ch)
    {
        for (int i = 0; i < total; ++i)
        {
            *pImag++ = CLIP((magnitude[i]-minMagnitude)/(maxMagnitude-minMagnitude) * MaxGray, 0, MaxGray);
        }
    }
    
    //find min and max of phase
    float minPhase = getMin(phase, total);
    float maxPhase = getMax(phase, total);
    
    // allocate uchar image for displaying phase
    IP_copyImageHeader(I1, Iphase);
    ChannelPtr<uchar> pIphase;
    
    //scale phase to fit between [0, 255]
    
    for (int ch = 0; IP_getChannel(Iphase, ch, pIphase, type); ++ch)
    {
        for (int i = 0; i < total; ++i)
        {
            *pIphase++ = CLIP((phase[i]-minPhase)/(maxPhase-minPhase) * MaxGray, 0, MaxGray);
        }
    }
    
    
}
