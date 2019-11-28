#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_swapPhase:
//
// Swap the phase channels of I1 and I2.
// Output is in II1 and II2.
//

struct complexP {
     int len;        // length of complex number list
     float * real;    // pointer to real number list
     float * imag;    // pointer to imaginary number list
};

extern void HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase);
extern void HW_MagPhase2fft(ImagePtr Imag, ImagePtr Iphase, ImagePtr Ifft);
extern void fft1D(complexP *q1, int dir, complexP *q2);
extern void fft1DRow(ImagePtr I1, ImagePtr Image1);
extern void fft1DColumn(ImagePtr I1, ImagePtr Image1, ImagePtr Image2);
extern void fft1DMagPhase(ImagePtr I1, ImagePtr Image2, float *Magnitude, float *Phase);
extern float getMin(float arr[], int total);
extern float getMax(float arr[], int total);

void swapPhase(ImagePtr I, float * magnitude ,float * newPhase, int total); //Puts new phase on image I
void Ifft1DRow(ImagePtr I, ImagePtr IFFT, ImagePtr IFFTout);
void Ifft1DColumn(ImagePtr I, ImagePtr rowI, ImagePtr IFFTout, ImagePtr resultPtr);


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_swapPhase:
//
// Swap phase of I1 with I2.
// (I1_mag, I2_phase) -> II1
// (I1_phase, I2_mag) -> II2
//
void
HW_swapPhase(ImagePtr I1, ImagePtr I2, ImagePtr II1, ImagePtr II2)
{
    //Dimensions of I1
    int w1 = I1->width();
    int h1 = I1->height();
    int total1 = w1 * h1;

    //Dimensions of I2
    int w2 = I2->width();
    int h2 = I2->height();
    int total2 = w2 * h2;

    //Checking that the images have same dimensions
    if(w1 != w2 || h1 != h2) {
        printf("Dimensions of I1: %d x %d \nDimensions of I2: %d x %d\nAre not equal\n",w1,h1,w2,h2);
        return;
    }
    
    ImagePtr Ifft1, Ifft2, IinvFFT1, IinvFFT2;
    ImagePtr Imag1, Iphase1, Imag2, Iphase2;
    
    // copy image header (width, height) of input image I1 to output image II1 & II2
    IP_copyImageHeader(I1, II1);
    IP_copyImageHeader(I1, II2);
    
    // Input and output channels
    ChannelPtr<uchar> in, out;
    
    // compute FFT of I1 and I2
    ImagePtr I1FFTTemp;
    ImagePtr I1FFT; //Final FFT
    
    ImagePtr I2FFT;
    ImagePtr I2FFTTemp;
    
    I1FFT->allocImage(w1,h1,FFT_TYPE); //FFT for I1
    I1FFTTemp->allocImage(w1, h1, FFT_TYPE);
    I2FFT->allocImage(w2,h2,FFT_TYPE); //FFT for I2
    I2FFTTemp->allocImage(w2, h2, FFT_TYPE);
    
    fft1DRow(I1,I1FFTTemp);
    fft1DColumn(I1, I1FFTTemp, I1FFT);
    fft1DRow(I2, I2FFTTemp);
    fft1DColumn(I2, I2FFTTemp, I2FFT);

    // compute magnitude and phase from real and imaginary FFT channels
    float *magnitude1 = new float[total1];              // Declaring Magnitude for Img1
    float *phase1 = new float[total1];                  // Declairing Phase for Img1
    fft1DMagPhase(I1, I1FFT, magnitude1 , phase1);

    float *magnitude2 = new float[total2];              // Declaring Magnitude for Img2
    float *phase2 = new float[total2];                  // Declairing Phase for Img2
    fft1DMagPhase(I2, I2FFT, magnitude2 , phase2);

    // swap phases and convert back to FFT images, compute inverse FFT, extract magnitude from resulting images, allocate uchar image and cast float channel to uchar for mag1
    ImagePtr I1IFFTTemp;
    ImagePtr I1Ifft;
    swapPhase(I1FFT,magnitude1,phase2,total1);
    Ifft1DRow(I1,I1FFT, I1IFFTTemp);
    Ifft1DColumn(I1, I1IFFTTemp, I1Ifft, II1);
    
    // swap phases and convert back to FFT images, compute inverse FFT, extract magnitude from resulting images, allocate uchar image and cast float channel to uchar for mag2
    ImagePtr I2IFFTTemp;
    ImagePtr I2Ifft;
    swapPhase(I2FFT,magnitude2,phase1,total2);
    Ifft1DRow(I2,I2FFT, I2IFFTTemp);
    Ifft1DColumn(I2, I2IFFTTemp, I2Ifft, II2);
}

void swapPhase(ImagePtr I, float * magnitude ,float * newPhase, int total) {
    ChannelPtr<float> real, img;
    real = I[0];
    img = I[1];
    
    for (int i = 0; i < total; ++i){
        *real++ = magnitude[i] * cos(newPhase[i]);
        *img++ = magnitude[i] * sin(newPhase[i]);
    }
}

void Ifft1DRow(ImagePtr I, ImagePtr IFFT ,ImagePtr IFFTout) {
    int w = I->width();
    int h = I->height();
    ChannelPtr<float> real, img, real2, img2;
    IFFTout->allocImage(w, h, FFT_TYPE);
    
    real = IFFT[0];
    img = IFFT[1];
    real2 = IFFTout[0];
    img2 = IFFTout[1];
    
    complexP c1, c2, *q1, *q2;
    q1 = &c1;
    q2 = &c2;
    q1->len = w;
    q1->real = new float[w];
    q1->imag = new float[w];
    q2->len = w;
    q2->real = new float[w];
    q2->imag = new float[w];

    for (int row = 0; row < h; ++row) {
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

void Ifft1DColumn(ImagePtr I, ImagePtr IFFT ,ImagePtr IFFTout, ImagePtr II1) {
    int w = I->width();
    int h = I->height();
    ChannelPtr<float> real, img, real2, img2;
    ChannelPtr<uchar> pII1;
    int type;
    IFFTout->allocImage(w, h, FFT_TYPE);
    
    real2 = IFFTout[0];
    img2 = IFFTout[1];
    
    
    for (int ch = 0; IP_getChannel(II1, ch, pII1, type); ++ch)
    {
         real = IFFT[0];
         img = IFFT[1];

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
    
}
