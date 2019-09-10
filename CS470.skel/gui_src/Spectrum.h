// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// Spectrum.h - Spectrum widget
//
// Written by: George Wolberg, 2018
// ======================================================================

#ifndef Spectrum_H
#define Spectrum_H

#include "ImageFilter.h"

class Spectrum : public ImageFilter {
	Q_OBJECT

public:
	Spectrum			(QWidget *parent = 0);	// constructor
	QGroupBox*	controlPanel	();			// create control panel
	bool		applyFilter	(ImagePtr, bool, ImagePtr);	// apply filter to input
	void		spectrum	(ImagePtr, ImagePtr, ImagePtr);
	void		initShader();
	void		gpuProgram(int pass);	// use GPU program to apply filter

protected slots:

private:
	// widgets
	QGroupBox*	m_ctrlGrp;	// groupbox for panel

	// variables
	int		m_width;	// input image width
	int		m_height;	// input image height
};

#endif	// Spectrum_H
