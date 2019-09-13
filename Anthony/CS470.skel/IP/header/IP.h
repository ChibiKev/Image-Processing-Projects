// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// IP.h - Header for IP*.cpp files
//
// Written by: George Wolberg, 2016
// ======================================================================

//! \mainpage	Image Processing Package
//! \author	George Wolberg
//! \date	2016
//! \par	References:
//!		This code is part of the
//!		IMPROC Image Processing Software Package\n
//!		Copyright &copy; 2016 by George Wolberg

//! \file	IP.h
//! \brief	Image Processing Package main header file.
//! \author	George Wolberg and Gene Yu, 2016

#ifndef IP_H
#define IP_H

#include "IPdefs.h"
#include "Channel.h"
#include "ChannelPtr.h"
#include "Image.h"
#include "ImagePtr.h"
#include <QtWidgets>

namespace IP {

// ----------------------------------------------------------------------
// system variables (set/fixed at start of session)
//
extern int	 MaxGray;
extern int	 Verbose;
extern int	 SIZEOF[];

extern int	 NULL_TYPE[];
extern int	   BW_TYPE[];
extern int	  BWA_TYPE[];
extern int	  RGB_TYPE[];
extern int	 RGBA_TYPE[];
extern int	  HSV_TYPE[];
extern int	  YIQ_TYPE[];
extern int	  LUT_TYPE[];
extern int	  MAT_TYPE[];
extern int	  FFT_TYPE[];

extern int	  UCHARCH_TYPE[];
extern int	  SHORTCH_TYPE[];
extern int	    INTCH_TYPE[];
extern int	   LONGCH_TYPE[];
extern int	  FLOATCH_TYPE[];
extern int	 DOUBLECH_TYPE[];

extern int	  INT2_TYPE[];
extern int	FLOAT2_TYPE[];
extern int	  INT3_TYPE[];
extern int	FLOAT3_TYPE[];
extern int	  INT4_TYPE[];
extern int	FLOAT4_TYPE[];

extern int	*CHTYPE_CH[];
extern int	*CHTYPE_IMAGE[];
extern char	*CHTYPE_NAME[];
extern char	 CHTYPE_LTR[]; 

extern uchar	 BITMASK[];



// ----------------------------------------------------------------------
// remaining global variables (may change during session)
//

extern int	AbsVal;
extern int	BorderVal;
extern int	HistoFit;
extern int	ImagePadMtd;
extern double	PlotScl;
extern int	SavePixmap;
extern int	Serpentine;
extern int	JpgQuality;
extern int	SwapBytes;



// ----------------------------------------------------------------------
// function externs
//
//		IParith.cpp	- arithmetic image operations
extern int	IP_addImage	(ImagePtr, ImagePtr, ImagePtr);
extern int	IP_addConst	(ImagePtr, double *, ImagePtr);
extern int	IP_subtractImage(ImagePtr, ImagePtr, ImagePtr);
extern int	IP_subtractConst(ImagePtr, double *, ImagePtr);
extern int	IP_multiplyImage(ImagePtr, ImagePtr, ImagePtr);
extern bool	IP_multiplyCmplx(ImagePtr, ImagePtr, ImagePtr);
extern int	IP_multiplyConst(ImagePtr, double *, ImagePtr);
extern int	IP_divideImage	(ImagePtr, ImagePtr, ImagePtr);
extern int	IP_divideConst	(ImagePtr, double *, ImagePtr);
extern void	IP_overlayImage	(ImagePtr, ImagePtr, ImagePtr);
extern int	IP_absoluteValue(ImagePtr, ImagePtr);
extern int	IP_magnitude	(ImagePtr, ImagePtr, ImagePtr);
extern int	IP_magnitudeAbsVal(ImagePtr,ImagePtr,ImagePtr);
extern ImagePtr operator+	(ImagePtr, ImagePtr);
extern ImagePtr operator-	(ImagePtr, ImagePtr);
extern ImagePtr operator*	(ImagePtr, ImagePtr);
extern ImagePtr operator/	(ImagePtr, ImagePtr);

//		IPbit.cpp	- bitwise image operations
extern void	IP_andImage	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_andConst	(ImagePtr, uchar  *, ImagePtr);
extern void	IP_orImage	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_orConst	(ImagePtr, uchar  *, ImagePtr);
extern void	IP_xorImage	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_xorConst	(ImagePtr, uchar  *, ImagePtr);
extern void	IP_bicImage	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_notImage	(ImagePtr, ImagePtr);

//		IPblur.cpp	- blurring functions
#include "IPblur.tpp"
extern void	IP_blur		(ImagePtr, int, int, ImagePtr);

//		IPcanny.cpp	- Canny edge operator
extern void	IP_canny	(ImagePtr, double, int, int, ImagePtr);
extern void	IP_cannyGradient(ImagePtr, double, int, int, ImagePtr,
				 ImagePtr, ImagePtr);

//		IPcastimg.cpp	- type casting routines for images
extern void	IP_castImage	   (ImagePtr, int, ImagePtr);
extern void	IP_initAlphaChannel(ImagePtr, int);

//		IPcastmem.cpp	- type casting routines for memory blocks
extern void	IP_castMemory	(void*, int, int, void*, int, int, int);

//		IPcolorconv.cpp	- conversion between color representations
extern void	IP_RGBtoYIQ1	(int, int, int, int&, int&, int&);
extern void	IP_YIQtoRGB1	(int, int, int, int&, int&, int&);
extern void	IP_RGBtoHSV1	(int, int, int, int&, int&, int&);
extern void	IP_HSVtoRGB1	(int, int, int, int&, int&, int&);
extern void	IP_clipRGB	(int&, int&, int&);

//		IPconvolve.cpp	- convolution and correlation functions
extern void	IP_convolve	(ImagePtr, ImagePtr, ImagePtr);
extern float	IP_correlation	(ImagePtr, ImagePtr, int, int, int&, int&);

//		IPdisplay.cpp	- display routines
extern void	IP_displayImage	      (void);
extern void	IP_displayImageInPlace(void);

//		IPdither.cpp	- dithering algorithms
extern void	IP_ditherOrdered  (ImagePtr, int, double,   ImagePtr);
extern void	IP_halftone	  (ImagePtr, int,int,double,ImagePtr);
extern void	IP_ditherDiffuse  (ImagePtr, int, double,   ImagePtr);
extern void	IP_ditherED	  (ImagePtr,      double,   ImagePtr);

//		IPfft.cpp	- 1D and 2D Fourier transforms
extern void	IP_fft2D	(ImagePtr, int, ImagePtr);
extern void	IP_fft1D	(ImagePtr, int, ImagePtr);
extern int	IP_displayFFT	(ImagePtr, int, int, double, ImagePtr);
extern void	IP_fftRecenter	(ImagePtr, ImagePtr);
extern void	IP_complexConjugate(ImagePtr, ImagePtr);

//		IPfiltnbr.cpp	- Neighborhood Ops
extern void	IP_blurGaussian	(ImagePtr, double, ImagePtr);
extern void	IP_blurEdgePreserve(ImagePtr, int, int, double, ImagePtr);
extern void	IP_blurMask	(ImagePtr, ImagePtr, ImagePtr, double, ImagePtr);
extern void	IP_blurMatte	(ImagePtr, int, int, ImagePtr);
extern void	IP_sharpen	(ImagePtr, int, int, double, ImagePtr);
extern void	IP_edgeDOB	(ImagePtr, int, int, double, ImagePtr);
extern void	IP_median	(ImagePtr, int, int, ImagePtr);
extern void	IP_median2	(ImagePtr, int, ImagePtr);
extern void	IP_gradient	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_sobel	(ImagePtr, ImagePtr);
extern void	IP_laplacian	(ImagePtr, ImagePtr);
extern void	IP_laplacianThresh(ImagePtr, double*, ImagePtr);

//		IPfiltpt.cpp	- Point Ops
extern void	IP_threshold	(ImagePtr, double, double, double, double, double, ImagePtr);
extern void	IP_thresholdOtsu(ImagePtr, int*, ImagePtr);
extern void	IP_quantize	(ImagePtr, int ,     bool, ImagePtr);
extern void	IP_clip		(ImagePtr, double, double, ImagePtr);
extern void	IP_scaleRange	(ImagePtr, double, double, ImagePtr);
extern void	IP_embedRange	(ImagePtr, double, double, ImagePtr);
extern void	IP_gammaCorrect	(ImagePtr, double, ImagePtr);
extern void	IP_contrast	(ImagePtr, double, double, double, ImagePtr);
extern void	IP_medianOfChannels(ImagePtr, ImagePtr);

//		IPgeo.cpp	- Geometric Ops
extern void	IP_resize	(ImagePtr, int, int, int, double, ImagePtr);
extern void	IP_resize1D	(ChannelPtr<uchar>, int, int, int, int, double,
				 ChannelPtr<uchar>);
extern void	IP_fresize1D	(ChannelPtr<float>, int, int, int, int, double,
				 ChannelPtr<float>);
extern void	IP_rotate	(ImagePtr, double, ImagePtr);
extern void	IP_translate	(ImagePtr, double, double, ImagePtr);
extern void	IP_reflect	(ImagePtr, int, ImagePtr);
extern void	IP_tile		(ImagePtr, int, int, ImagePtr);
extern void	IP_skew1D	(ChannelPtr<uchar>, int, int, double, int,
			 	 ChannelPtr<uchar>);
extern void	IP_fskew1D	(ChannelPtr<float>, int, int, double, int,
				 ChannelPtr<float>);

//		IPhisto.cpp	- histogram evaluation, manipulation, display
extern void	IP_histogram	(ImagePtr, int, int*, int, double&,double&);
extern void	IP_histoEqualize(ImagePtr, ImagePtr);
extern void	IP_histoMatch   (ImagePtr, ImagePtr, bool, ImagePtr);

//		IPhough.cpp	- Hough transform
extern void	IP_houghLines	(ImagePtr, int, ImagePtr, ImagePtr);
extern void	IP_houghCircles	(ImagePtr, ImagePtr, ImagePtr, int,
				 int, int, ImagePtr);

//		IPinit.cpp	- initialize global IP data structures
extern void	IP_init		(void);

//		IPio.cpp	- image input/output
extern ImagePtr	IP_readImage	      (const char*);
extern bool	IP_readImageIntoI     (const char*, ImagePtr);
extern bool	IP_readImageDimensions(const char*, int&, int&);
extern bool	IP_saveImage	      (ImagePtr, const char*, const char*);

//		IPlut.cpp	- lookup tables
extern void	IP_makeLut	(ImagePtr);
extern void	IP_editLut	(ImagePtr, ImagePtr);
extern void	IP_applyLut	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_applyLutIntrp(ImagePtr, ImagePtr, ImagePtr);

//		IPmatte.cpp	- matte operations
extern void	IP_matteExtract	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_matteOverlay	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_matteOverlayAvg(ImagePtr,ImagePtr,ImagePtr);
extern void	IP_matteCut	(ImagePtr, int,      ImagePtr);
extern void	IP_matteOver	(ImagePtr, ImagePtr, int, ImagePtr);
extern void	IP_matteIn	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_matteOut	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_matteAtop	(ImagePtr, ImagePtr, int, ImagePtr);
extern void	IP_matteXor	(ImagePtr, ImagePtr, int, ImagePtr);
extern void	IP_matteHicon	(ImagePtr, ImagePtr);
extern void	IP_matteDarken	(ImagePtr, double, ImagePtr);
extern void	IP_matteOpaque	(ImagePtr, double, ImagePtr);
extern void	IP_matteDissolve(ImagePtr, double, ImagePtr);
extern void	IP_matteMultiply(ImagePtr, ImagePtr, int, ImagePtr);
extern void	IP_matteMaxAlpha(ImagePtr, ImagePtr, ImagePtr);

//		IPmipmap.cpp	- mipmap routines
extern int	IP_mipmap	(Image, Image&);

//		IPmmch.cpp	- channel memory management
#include "IPmmch.tpp"
template<class T>
	bool	IP_getChannel 	  (ImagePtr, int, ChannelPtr<T>&, int &);
template<class T1, class T2>
	void	IP_copyFromRow	  (ChannelPtr<T1>, int, int, ChannelPtr<T2>);
template<class T>
	void	IP_copyFromRow	  (ImagePtr, int, int, ChannelPtr<T>);
template<class T>
	void    IP_copyToRow	  (ChannelPtr<T>, ImagePtr , int, int);
template<class T1, class T2>
	void	IP_copyToRow	  (ChannelPtr<T1>, ChannelPtr<T2>, int, int);

extern void	IP_copyChannel	  (ImagePtr, int, ImagePtr, int );
extern void	IP_copyChannelEnd (ImagePtr, int, ImagePtr);
extern void	IP_castChannel	  (ImagePtr, int, ImagePtr, int, int);
extern void	IP_castChannels	  (ImagePtr, int*,ImagePtr);
extern void	IP_castChannelsEq (ImagePtr, int, ImagePtr);
extern void	IP_castChannelsMin(ImagePtr, int, ImagePtr);
extern void	IP_castChannelsMax(ImagePtr, int, ImagePtr);
extern void	IP_castChannelsImg(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_clearChannel	  (ImagePtr, int, ImagePtr);
extern int	IP_checkDimensions(ImagePtr, ImagePtr);

//	IPmmimg.cpp
extern Image   *IP_allocImage	(int, int, int*);
extern Image   *IP_allocImage_I	(ImagePtr);
extern void	IP_allocImageInI(ImagePtr, int, int, int*);
extern void	IP_copyImage	(ImagePtr, ImagePtr);
extern void	IP_copyHeader	(ImagePtr, int, ImagePtr);
extern void	IP_copyHeader2	(ImagePtr, ImagePtr, int, ImagePtr);
extern void	IP_copyImageBuffer(ImagePtr, ImagePtr);

//	IPmorph.cpp
extern void	IP_shrink	(ImagePtr, int, ImagePtr);
extern void	IP_dilate	(ImagePtr, int, ImagePtr);

//	IPpolar.cpp
extern void	IP_polarToRect	 (ImagePtr, ImagePtr);
extern void	IP_rectToPolar	 (ImagePtr, ImagePtr);
extern void	IP_rectToLogPolar(ImagePtr, ImagePtr);

//	IPthin.cpp
extern void	IP_thin		(ImagePtr, int, int*, int*);
extern void	IP_thinBridge	(ImagePtr, int, int, int, int, int*, int*);

//	IPutil.cpp
extern void	IP_interleave	(ImagePtr, ImagePtr);
extern void	IP_interleave4	(ImagePtr, int, int, int, int, ImagePtr);
extern void	IP_uninterleave	(ImagePtr, ImagePtr);
extern void	IP_normalizeDimensions(ImagePtr, ImagePtr);
extern void	IP_crop		(ImagePtr, int, int, int, int, ImagePtr);
extern void	IP_shift	(ImagePtr, int,  int, ImagePtr);
extern void	IP_pad		(ImagePtr, int*, int, ImagePtr);
extern void	IP_pad1D	(ChannelPtr<uchar>, int, int, int, int,
				 ChannelPtr<uchar>&);
extern void	IP_fpad1D	(ChannelPtr<float>, int, int, int, int,
				 ChannelPtr<float>&);
extern void	IP_clearImage	(ImagePtr);
extern void	IP_setImage	(ImagePtr, double*);
extern void	IP_minmaxImage	(ImagePtr, double&, double&);
extern void	IP_minmaxChannel(ImagePtr, int, double&, double&);
extern void	IP_RGBdecouple	(ImagePtr, ImagePtr &, bool decouple=true);
extern void	IP_RGBcouple	(ImagePtr, ImagePtr, ImagePtr);
extern void	IP_bailout	(const char*, ...);

//	IPwarp*.cpp
extern void	IP_warpCoons	  (void);
extern void	IP_warpSpatialLuts(ImagePtr,ImagePtr,ImagePtr,ImagePtr,ImagePtr);
extern void	IP_warpMesh(ImagePtr,ImagePtr,ImagePtr,ImagePtr,ImagePtr,ImagePtr);

//      IPdither.cpp
extern void     IP_ditherUnordered(ImagePtr, int, double, ImagePtr);
extern void     IP_ditherOrdered(ImagePtr, int, double, ImagePtr);
extern void     IP_halftone(ImagePtr, int, int, double, ImagePtr);
extern void     IP_ditherDiffuse(ImagePtr, int, double, ImagePtr);

//      IPtoUI.cpp
extern void     IP_printfErr(const char *fmt, ...);

}	// namespace IP

#endif	// IP_H
