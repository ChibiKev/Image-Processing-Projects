// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// IPblur.tpp - Template functions for blurring 1D scanlines.
//
// Written by: George Wolberg, 2016
// ======================================================================

//! \file	IPblur.tpp
//! \brief	Image blurring template functions.
//! \author	George Wolberg, 2016

using namespace IP;

extern Image *IP_allocImage(int, int, int*);
template<class T> void blur1D_odd(ChannelPtr<T>, int, int, double, ChannelPtr<T>);

//! \addtogroup filtnbr
//@{

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IP_blur1D:
//
// dst <- Blur src with 1D box filter of width ww.
//! \brief	Image blurring (1D version).
//! \details	\a dst <- Blur \a src with 1D box filter of width \a ww.
//! param[in]	src - ChannelPtr to input scanline.
//! param[in]	len - Number of pixels in \a src.
//! param[in]	stride - Distance between successive input pixels (in pixels).
//!		stride = 1 for processing rows.
//!		stride = scanline width for processing columns.
//! param[in]	ww - Width of blurring kernel.
//! param[out]	dst - ChannelPtr to output scanline.
//!		Distance between successive output pixels is \a stride.
//
template<class T>
void
IP_blur1D(ChannelPtr<T> src, int len, int stride, int ww, ChannelPtr<T> dst)
{
	// allocate sufficiently large working buffer
	ImagePtr II = IP_allocImage(len+ww, 1, FLOATCH_TYPE);

	ChannelPtr<T> buffer = II[0];	// buffer channel
	ChannelPtr<T> p = buffer;

	// copy input into buffer
	int padLen = ww / 2;		// length of padding on each side
	p += padLen;			// skip past padding
	for(int i=0; i<len; i++)	// copy input to buffer
		p[i] = src[i*stride];

	// pad both sides using pixel replication
	ChannelPtr<T> p1, p2;
	p1 = buffer;			// pointer to left  padding
	p2 = buffer + len + padLen;	// pointer to right padding
	for(int i=0; i<padLen; i++) {
		p1[i] = p[0];		// pad left  side
		p2[i] = p[len-1];	// pad right side
	}

	// init sum with first set of pixels (not including last one)
	int sum = 0;
	for(int i=0; i<ww-1; i++)
		sum += buffer[i];

	// compute output
	int i = ww-1;
	for(int x=0; x<len; x++) {
		sum += buffer[i++];	// add incoming pixel
		dst[x*stride] = sum/ww;	// init output with average
		sum -= buffer[i-ww];	// subtract outgoing pixel
	}
}
//@}
