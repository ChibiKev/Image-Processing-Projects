#include "IP.h"
using namespace IP;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_spectrum:
//
// Convolve magnitude and phase spectrum from image I1.
// Output is in Imag and Iphase.
//

extern void HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase);

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
	// compute FFT of the input image
	
// PUT YOUR CODE HERE...
	
	// compute magnitute and phase spectrums from FFT image
	ImagePtr Im = NEWIMAGE;
	ImagePtr Ip = NEWIMAGE;
// PUT YOUR CODE HERE...

	// find min and max of magnitude and phase spectrums

// PUT YOUR CODE HERE...

	// allocate uchar image for displaying magnitude and phase
	Imag  ->allocImage(w, h, BW_TYPE);
	Iphase->allocImage(w, h, BW_TYPE);

	// set imagetypes for single 8-bit channel
	Imag  ->setImageType(BW_IMAGE);
	Iphase->setImageType(BW_IMAGE);

	// get channel pointers for input magnitude and phase spectrums
	ChannelPtr<float>   magF = Im[0];
	ChannelPtr<float> phaseF = Ip[0];

	// get channel pointers for output magnitude and phase spectrums
	ChannelPtr<uchar> mag    = Imag  [0];
	ChannelPtr<uchar> phase  = Iphase[0];

	// scale magnitude and phase to fit between [0, 255]

// PUT YOUR CODE HERE...
}
