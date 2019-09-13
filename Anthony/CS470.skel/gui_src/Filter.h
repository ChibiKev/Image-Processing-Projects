// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// Filter.h - Filter widget
//
// Written by: George Wolberg, 2018
// ======================================================================

#ifndef FILTER_H
#define FILTER_H

#include "ImageFilter.h"

class Filter : public ImageFilter {
	Q_OBJECT

public:
	Filter				(QWidget *parent = 0);		// constructor
	QGroupBox*	controlPanel	();				// create control panel
	bool		applyFilter	(ImagePtr, bool, ImagePtr);	// apply filter to input
	void		reset		();				// reset parameters
	void		filter		(ImagePtr, int, int, ImagePtr);
	void		initShader();
	void		gpuProgram(int pass);	// use GPU program to apply filter

protected slots:
	void		changeFilterW	(int);
	void		changeFilterH	(int);
	void		setLock		(int);
	void		setStraddle     (int);

private:
	// widgets
	QSlider*	m_slider [2];	// Filter sliders
	QSpinBox*	m_spinBox[2];	// Filter spin boxes
	QCheckBox*	m_checkBox;	// Filter check box
	QCheckBox*	m_checkBox1;	// Filter straddle check box
	QGroupBox*	m_ctrlGrp;	// groupbox for panel
	int		m_width;	// input image width
	int		m_height;	// input image height
	bool		m_straddle;
};

#endif	// FILTER_H
