// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// Resize.h - Resize widget
//
// Written by: George Wolberg, 2018
// ======================================================================

#ifndef RESIZE_H
#define RESIZE_H

#include "ImageFilter.h"

class Resize : public ImageFilter {
	Q_OBJECT

public:
	enum { BOX, TRIANGLE, CUBIC_CONV, LANCZOS, HANN, HAMMING };
	Resize				(QWidget *parent = 0);		// constructor
	QGroupBox*	controlPanel	();				// create control panel
	bool		applyFilter	(ImagePtr, bool, ImagePtr);	// apply filter to input
	void		reset		();				// reset parameters
	void		resize		(ImagePtr, int, int, int, double, ImagePtr);
	void		initShader	();
	void		gpuProgram	(int pass);	// use GPU program to apply filter
	void		setImageSize	(int w, int h);

protected slots:
	void		changeKernel	(int);
	void		changeWidth	();
	void		changeHeight	();
	void		changeParam	();
	void		checkParam	();

private:
	// widgets
	QLineEdit*	m_widthLinetEdit;	// new image width
	QLineEdit*	m_heightLinetEdit;	// new image height
	QLineEdit*	m_paramLineEdit;	// kernel free parameter
	QComboBox*	m_comboBoxKernels;	// list of kernels
	QCheckBox*	m_proportionChkBox;	// lock the proportion
	QGroupBox*	m_ctrlGrp;		// groupbox for panel
	int		m_width;		// input image width
	int		m_height;		// input image height
};

#endif	// RESIZE_H
