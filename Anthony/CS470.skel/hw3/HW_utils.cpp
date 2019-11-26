// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_utils:
//
// Utility functions for converting between FFT coefficients and their
// magnitude and phase representations.
//

#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_fft2MagPhase:
//
// Convert a 2-channel FFT image into two 1-channel magnitude and phase images.
// 
void 
HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase)
{
	// init vars
	int w = Ifft->width ();
	int h = Ifft->height();
	int total = w*h;

	// allocate single-channel images for magnitude and phase
	Imag  ->allocImage(w, h, FLOATCH_TYPE);
	Iphase->allocImage(w, h, FLOATCH_TYPE);

	// get pointers for magnitude and phase channels
	ChannelPtr<float> mag   = Imag  [0];
	ChannelPtr<float> phase = Iphase[0];

	// get pointers for real and imaginary FFT channels
	ChannelPtr<float> real = Ifft[0];
	ChannelPtr<float> imag = Ifft[1];

	// compute mag = sqrt(real^2 + imag^2) and phase = atan2(imag/real)
	for(int i=0; i<total; ++i) {
        mag[i] = sqrt(pow(real[i], 2)+pow(imag[i],2));
        phase[i] = atan2(imag[i],real[i]);
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_MagPhase2fft:
//
// Convert 1-channel magnitute and phase images into a 2-channel FFT image.
//
void 
HW_MagPhase2fft(ImagePtr Imag, ImagePtr Iphase, ImagePtr Ifft)
{
	// init vars
	int w = Ifft->width();
	int h = Ifft->height();
	int total = w*h;

	// get pointers for magnitude and phase channels
	ChannelPtr<float> mag   = Imag  [0];
	ChannelPtr<float> phase = Iphase[0];

	// get pointers for real and imaginary FFT channels
	ChannelPtr<float> real = Ifft[0];
	ChannelPtr<float> imag = Ifft[1];
	
	// compute real and imaginary values from magnitude and phase 
	for(int i=0; i<total; ++i) {
        real[i] = mag[i]*cos(phase[i]);
        imag[i] = mag[i]*sin(phase[i]);
	}
}

