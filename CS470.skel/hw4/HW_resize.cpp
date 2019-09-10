#include "IP.h"
using namespace IP;

enum FILTERS { BOX, TRIANGLE, CUBIC_CONV, LANCZOS, HANN, HAMMING };
double	boxFilter	(double, double);
double	triFilter	(double, double);
double	cubicConv	(double, double);
double	lanczos 	(double, double);
double	hann	 	(double, double);
double	hamming	 	(double, double);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// resize1D:
//
// Scale 1D scanline. float version.
// Input  consists of  len elements in src.
// Output consists of nlen elements in dst.
// The inter-pixel distance in src and dst is offst: 1 for rows or row_width for columns
// The resampling filter used is specified by mtd (BOX, TRIANGLE, CUBIC_CONV, LANCZOS, HANN, HAMMING)
//
void
resize1D(ChannelPtr<uchar> src, int len, int nlen, int offst, int mtd, double param,
	 ChannelPtr<uchar> dst)
{
	// copy src to dst if no scale change
	if(len == nlen) {
		for(int i = 0; i<len; ++i) {
			*dst = *src;
			src += offst;
			dst += offst;
		}
		return;
	}

	// filter function and filter support (kernel half-width) declarations
	double (*filter)(double, double);	// ptr to filter fct  
	double   filterSupport;			// 1-sided filter length  

	// default values for filter and its half-width support
	filter = triFilter;
	filterSupport =  1;

	// compute filterSupport: the half-width of the filter
	switch(mtd) {
	case FILTERS::BOX:
		filter = boxFilter;
		filterSupport = .5;
		break;
	case FILTERS::TRIANGLE:
		filter = triFilter;
		filterSupport = 1;
		break;
	case FILTERS::CUBIC_CONV:
		filter = cubicConv;
		filterSupport = 2;
		break;
	case FILTERS::LANCZOS:
		filter = lanczos;
		filterSupport = param;
		break;
	case FILTERS::HANN:
		filter = hann;
		filterSupport = param;
		break;
	case FILTERS::HAMMING:
		filter = hamming;
		filterSupport = param;
		break;
	default:
		fprintf(stderr, "resize1D: Bad filter choice %d\n", mtd);
		return;
	}

	// init filter amplitude (fscale) and width (fwidth), and scale change
	double fwidth = filterSupport;
	double fscale = 1.0;
	double  scale = (double) nlen / len;	// resampling scale factor

	// image minification: update fwidth and fscale;
	// else kernel remains intact for magnification
	if(scale < 1.0) {	// minification: h(x) -> h(x*scale) * scale
		fwidth = filterSupport / scale;	// broaden  filter
		fscale = scale;			// lower amplitude
	}

	// evaluate size of padding and buffer length
	int padlen = CEILING(fwidth);		// buffer pad length
	int buflen = len + 2 * padlen;		// buffer length

	// allocate buffer memory if necessary
	ImagePtr	 Ibuf;
	ChannelPtr<uchar> buf;
	int		  t=0;
	IP_getChannel(Ibuf, 0, buf, t);
	if(Ibuf->width() < buflen || t != UCHAR_TYPE)
		Ibuf->replaceChannel(0, buflen, 1, UCHAR_TYPE);

	// copy src into padded buffer
	buf = Ibuf[0];

	// copy src into dst; save space for padding
	ChannelPtr<uchar> p1 = src;
	ChannelPtr<uchar> p2 = buf + padlen;
	for(int x=0; x<len; ++x,p1+=offst) p2[x] = *p1;
	
	// pad left and right columns
	int  v1, v2;
	p1 = buf + padlen;
	p2 = p1  + len - 1;

	// replicate border
	v1 = p1[0];
	v2 = p2[0];
	for(int x=1; x<= padlen; ++x) {
		p1[-x] = v1;
		p2[ x] = v2;
	}


	// init srcp to point to first non-padded pixel in padded buffer
	ChannelPtr<uchar> srcp = buf + padlen;

	// compute all output pixels
	int	left, right;		// kernel extent in input 
	double	u;			// input coordinate u
	double	acc;			// convolution accumulator
	double	pixel;			// fetched pixel value
	double	weight;			// filter kernel weight
	for(int x = 0; x<nlen; ++x) {
		// map output x to input u: inverse mapping
		u = x / scale;

		// left and right extent of kernel centered at u
		if(u - fwidth < 0)
			left = FLOOR  (u - fwidth);
		else    left = CEILING(u - fwidth);
		right = FLOOR(u + fwidth);

		// reset acc for collecting convolution products
		acc = 0;

		// weigh input pixels around u with kernel
		double sumWeight = 0;
		for(int i=left; i <= right; ++i) {
			// fetch pixel
			// padding replaces pixel = srcp[CLIP(i, 0, len-1)] to pixel = srcp[i]
			pixel = srcp[i];

			// evaluate weight; multiply it with pixel and add it to accumulator
			weight = (*filter)((u-i) * fscale, param);
			sumWeight += weight;
			acc += (pixel * weight);
		}

		// assign weighted accumulator to dst
		*dst = (int) CLIP(ROUND(acc / sumWeight), 0, MaxGray);
		dst += offst;
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// boxFilter:
//
// Box (nearest neighbor) filter.
//
double
boxFilter(double t, double /*param*/)
{
	if((t > -.5) && (t <= .5)) return(1.0);
	return(0.0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// triFilter:
//
// Triangle filter (used for linear interpolation).
//
double
triFilter(double t, double /*param*/)
{
	if(t < 0) t = -t;
	if(t < 1.0) return(1.0 - t);
	return(0.0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// cubicConv:
//
// Cubic convolution filter.
//
double
cubicConv(double t, double param = -1)
{
	float	A; 
	double t2, t3;

	if(t < 0) t = -t;
	t2 = t  * t;
	t3 = t2 * t;

	A = param;
	if(t < 1.0) return((A + 2)*t3 - (A + 3)*t2 + 1);
	if(t < 2.0) return(A * (t3 - 5 * t2 + 8 * t - 4));
	return(0.0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sinc:
//
// Sinc function.
//
double
sinc(double t)
{
	t *= PI;
	if(t != 0) return(sin(t) / t);
	return(1.0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// lanczos:
//
// Lanczos filter.
//
double
lanczos(double t, double param=3)
{
	int	N;

	N = (int) param;
	if(t < 0) t = -t;
	if(t < N) return(sinc(t) * sinc(t / N));
	return(0.0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// hann:
//
// Hann windowed sinc function.
//
double
hann(double t, double param=3)
{
	int	N;

	N = (int) param;
	if(t < 0) t = -t;
	if(t < N) return(sinc(t) * (.5 + .5*cos(PI*t / N)));
	return(0.0);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// hamming:
//
// hamming windowed sinc function.
//
double
hamming(double t, double param=3)
{
	int	N;

	N = (int) param;
	if(t < 0) t = -t;
	if(t < N) return(sinc(t) * (.54 + .46*cos(PI*t / N)));
	return(0.0);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_resize:

void
HW_resize(ImagePtr I1, int ww, int hh, int kernelID, double param, ImagePtr I2)
{
	int w = I1->width ();
	int h = I1->height();

	// handle trivial case where dimensions remain unchanged
	if(ww == w && hh == h) {
		if(I1 != I2) IP_copyImage(I1, I2);
		return;
	}

	// init II
	ImagePtr   II;
	if(I1 == I2) IP_copyImage(I1, II);
	else II = I1;

	// init I2 info
	IP_copyImageHeaderOnly(I1, I2);
	I2->setWidth (ww);
	I2->setHeight(hh);
	I2->initChannels(I1->channelTypes());

	// create tmp image to hold horizontal pass
	ImagePtr I3;
	I3 = IP_allocImage(ww, h, UCHARCH_TYPE);

	// 2-pass scale algorithm
	int t;
	ChannelPtr<uchar> src, dst;
	for(int ch = 0; IP_getChannel(II, ch, src, t); ch++) {
		IP_getChannel(I2, ch, dst, t);

		// horizontal pass scales rows
		for(int y = 0; y<h; ++y) {

// PUT YOUR CODE HERE

		}

		// vertical pass scales columns
		for(int x = 0; x<ww; ++x) {

// PUT YOUR CODE HERE

		}
	}

	// free global buffer
	if(II != I1) II->freeImage();
	I3->freeImage();
}
