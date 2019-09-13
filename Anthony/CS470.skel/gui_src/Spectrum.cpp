// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// Spectrum.cpp - Spectrum widget.
//
// Written by: George Wolberg, 2018
// ======================================================================

#include "MainWindow.h"
#include "Spectrum.h"

extern MainWindow *g_mainWindowP;
extern void HW_spectrum(ImagePtr I1, ImagePtr Imag, ImagePtr Iphase);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spectrum::Spectrum:
//
// Constructor.
//
Spectrum::Spectrum(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spectrum::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Spectrum::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Spectrum");
	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spectrum::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Spectrum::applyFilter(ImagePtr I1, bool gpuFlag, ImagePtr I2)
{
	// error checking
	if(I1.isNull())		return 0;
	m_width  = I1->width();
	m_height = I1->height();
	//  spectrum
	ImagePtr Imag, Iphase;
	if(!(gpuFlag && m_shaderFlag)) {
		ImagePtr II;
		IP_copyImage(I1, II);
		IP_castImage(II, BW_IMAGE, II);
		spectrum(II, Imag=NEWIMAGE, Iphase=NEWIMAGE);
		int tx = 5;
		int ty = 5;
		int w  = Imag->width();
		int h  = Imag->height();
		I2->allocImage(2*w+3*tx, h+2*ty, UCHARCH_TYPE);
		I2->setImageType(BW_IMAGE);

		IP_clearImage(I2);

		IP_translate(Imag,       tx, ty, Imag);
		IP_translate(Iphase, w+2*tx, ty, Iphase);

		IP_addImage(Imag,   I2, I2);
		IP_addImage(Iphase, I2, I2);
	} else    
		g_mainWindowP->glw()->applyFilterGPU(m_nPasses);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spectrum::convolve:
//
// Spectrum image I1 with convolution filter in kernel.
// Output is in I2.
//
void
Spectrum::spectrum(ImagePtr I1, ImagePtr Imag, ImagePtr Iphase)
{
	HW_spectrum(I1, Imag, Iphase);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spectrum::initShader:
//
// init shader program and parameters.
//
void
Spectrum::initShader() 
{
	m_shaderFlag = false;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spectrum::gpuProgram:
//
// Invoke GPU program
//
void
Spectrum::gpuProgram(int) {}
