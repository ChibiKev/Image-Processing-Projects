#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_filter:
//
// Convolve I1 with filter kernel in Ikernel.
// Output is in I2.
//

extern void HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_filter:
//
// Convolve a box filter of size (sx, sy) using FFT.
// Convolution in the spatial domain is multiplication in frequency domain.
// straddle = true , split box filter to four segments and put at corners
// straddle = false, put box filter in the center
//
void
HW_filter(ImagePtr I1, int sx, int sy, bool straddle, ImagePtr I2)
{
	int w = I1->width();
	int h = I1->height();
	
	// allocate a kernel image the same size as input image
	ImagePtr Ikernel = IP_allocImage(w, h, FLOATCH_TYPE);

	// clear kernel image
	IP_clearImage(Ikernel);

	// init pointer to kernel image
	ChannelPtr<float> p = Ikernel[0];

	// normalize box filter values by scaling to total size of input image
	float norm = (float(w*h)) / (sx*sy);

	// init kernel image with normalized box filter values
	if(straddle) {		// straddle four corners

// PUT YOUR CODE HERE

	} else {		// center box filter in center

// PUT YOUR CODE HERE

	}

	ImagePtr Ifft1, Ifft2, Ifft3, IinvFFT3;
	ImagePtr Imag, Iphase;

	// convolve input image and kernel by multiplying their FFTs in the frequency domain
	// PUT YOUR CODE HERE			// compute FFT of input image
	// PUT YOUR CODE HERE			// compute FFT of kernel
	IP_multiplyCmplx(Ifft1, Ifft2, Ifft3);	// complex multiplication of the two FFTs

	// compute the magnitude spectrum of the inverse FFT
	IP_fft2D(Ifft3, -1, IinvFFT3);		// compute inverse FFT of Ifft3
	// PUT YOUR CODE HERE			// get magnitude spectrum from IinvFFT3

	// allocate output image and save magnitude spectrum for display
	I2->allocImage(Imag->width(), Imag->height(), BW_TYPE);
	I2->setImageType(BW_IMAGE);
	IP_castChannel(Imag, 0, I2, 0, UCHAR_TYPE);
}
