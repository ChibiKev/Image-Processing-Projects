// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// SwapPhase.h - Swap phase spectrum widget
//
// Written by: George Wolberg, 2018
// ======================================================================

#ifndef SWAPPHASE_H
#define SWAPPHASE_H

#include "ImageFilter.h"

class SwapPhase : public ImageFilter {
	Q_OBJECT

public:
	SwapPhase			(QWidget *parent = 0);		// constructor
	QGroupBox*	controlPanel	();				// create control panel
	bool		applyFilter	(ImagePtr, bool, ImagePtr);	// apply filter to input
	void		swapPhase	(ImagePtr, ImagePtr, ImagePtr, ImagePtr);
	void		initShader();
	void		gpuProgram(int pass);	// use GPU program to apply filter

protected slots:
	int		load		();

private:
	// widgets
	QPushButton*	m_button;	// Swap pushbutton
	QTextEdit*	m_values;	// text field for kernel values
	QGroupBox*	m_ctrlGrp;	// groupbox for panel

	// variables
	QString		m_file;
	QString		m_currentDir;
	ImagePtr	m_2ndImage;
	int		m_width;	// input image width
	int		m_height;	// input image height
};

#endif	// SWAPPHASE_H
