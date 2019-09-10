// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// Resize.cpp - Resize widget.
//
// Written by: George Wolberg, 2018
// ======================================================================

#include "MainWindow.h"
#include "Resize.h"

extern MainWindow *g_mainWindowP;
extern void HW_resize(ImagePtr I1, int ww, int hh, int kernelID, double param, ImagePtr I2);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::Resize:
//
// Constructor.
//
Resize::Resize(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Resize::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Resize");

	// layout for assembling filter widget
	QGridLayout *layout = new QGridLayout;

	// alloc array of labels
	QLabel *label[5];
	QStringList labelTexts = { 
		"New Image Width:",
		"New Image Height:",
		"Proportional:",
		"Kernel type:",
		"Param:"};

	for(int i = 0; i < 5; ++i) {
		label[i] = new QLabel(m_ctrlGrp);
		label[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
		label[i]->setText(labelTexts[i]);
	}

	m_widthLinetEdit  = new QLineEdit();
	m_widthLinetEdit->setMaximumHeight(30);
	m_widthLinetEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	m_heightLinetEdit = new QLineEdit();
	m_heightLinetEdit->setMaximumHeight(30);
	m_heightLinetEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	m_paramLineEdit = new QLineEdit();
	m_paramLineEdit->setMaximumHeight(30);
	m_paramLineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	m_paramLineEdit->setText("");	// default for cubic kernel

	m_proportionChkBox = new QCheckBox(m_ctrlGrp);
	m_proportionChkBox->setCheckable(true);
	m_proportionChkBox->setChecked(true);

	m_comboBoxKernels = new QComboBox;
	m_comboBoxKernels->addItem("Box filter");
	m_comboBoxKernels->addItem("Triangle filter");
	m_comboBoxKernels->addItem("Cubic Convolution");
	m_comboBoxKernels->addItem("Lanczos windowed sinc");
	m_comboBoxKernels->addItem("Hann windowed sinc");
	m_comboBoxKernels->addItem("Hamming windowed sinc");

	// assemble dialog
	layout->setSizeConstraint(QLayout::SetMinimumSize);
	layout->addWidget(label[0], 0, 0);
	layout->addWidget(m_widthLinetEdit, 0, 1);
	layout->addWidget(label[1], 1, 0);
	layout->addWidget(m_heightLinetEdit, 1, 1);
	layout->addWidget(label[2], 2, 0);
	layout->addWidget(m_proportionChkBox, 2, 1);
	layout->addWidget(label[3], 3, 0);
	layout->addWidget(m_comboBoxKernels, 3, 1);
	layout->addWidget(label[4], 4, 0);
	layout->addWidget(m_paramLineEdit, 4, 1);

	// init signal/slot connections
	connect(m_comboBoxKernels, SIGNAL(currentIndexChanged(int)), this, SLOT(changeKernel(int)));
	connect(m_widthLinetEdit,  SIGNAL(editingFinished()), this, SLOT(changeWidth()));
	connect(m_heightLinetEdit, SIGNAL(editingFinished()), this, SLOT(changeHeight()));
	connect(m_paramLineEdit,   SIGNAL(editingFinished()), this, SLOT(changeParam()));

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Resize::applyFilter(ImagePtr I1, bool /*gpuFlag*/, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	int newW = m_widthLinetEdit ->text().toInt();
	int newH = m_heightLinetEdit->text().toInt();
	int kernelID = m_comboBoxKernels->currentIndex();
	double param = m_paramLineEdit->text().toFloat();
	checkParam();
	resize(I1, newW, newH, kernelID, param, I2);
	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::Resize:
//
// Apply Resize filter on image I1. Resize filter has size sz x sz.
// Output is in I2.
//
void
Resize::resize(ImagePtr I1, int newW, int newH, int kernelID, double param, ImagePtr I2)
{
	HW_resize(I1, newW, newH, kernelID, param, I2);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::changeWidth:
//
// Slot to process change in width.
//
void
Resize::changeWidth()
{

	// init vars
	int w = m_widthLinetEdit ->text().toInt();
	int h = m_heightLinetEdit->text().toInt();

	// error checking
	if(w <= 0 || w > 10 * m_width)
		w = m_width;

	if(m_proportionChkBox->isChecked())
		h = ((float) w /m_width) * m_height;

	m_widthLinetEdit ->setText(QString::number(w));
	m_heightLinetEdit->setText(QString::number(h));

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::changeHeight:
//
//  Slot to process change in height.
//
void
Resize::changeHeight()
{
	// init vars
	int w = m_widthLinetEdit ->text().toInt();
	int h = m_heightLinetEdit->text().toInt();

	// error checking
	if(h <= 0 || h > 10 * m_width)
		h = m_height;

	if(m_proportionChkBox->isChecked())
		w = ((float) h / m_height) * m_width;

	m_widthLinetEdit ->setText(QString::number(w));
	m_heightLinetEdit->setText(QString::number(h));

	// apply filter to source image and display result
	g_mainWindowP->preview();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::changeKernel:
//
//  Slot to process change in height.
//
void
Resize::changeKernel(int /*value*/)
{
	checkParam();	// error checking

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::changeParam:
//
//  Slot to process change in free param.
//
void
Resize::changeParam()
{
	checkParam();	// error checking

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::checkParam:
//
//  Slot to process to check free param init to right value for the selected filter 
//
void
Resize::checkParam()
{
	// error checking
	float param  = m_paramLineEdit->text().toFloat();
	int kernelID = m_comboBoxKernels->currentIndex();
	switch(kernelID) {
	case BOX:
	case TRIANGLE:
		m_paramLineEdit->setText("");
		break;
	case CUBIC_CONV:
		if(param <= -1.0 || param >= 1.0 || m_paramLineEdit->text() == "")
			param = -1;
		m_paramLineEdit->setText(QString::number(param));
		break;
	case LANCZOS:
	case HANN:
	case HAMMING:
		if(param < 1)
			param = 2;
		m_paramLineEdit->setText(QString::number(param));
		break;
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::reset:
//
// Reset parameters.
//
void
Resize::reset()
{
	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::initShader:
//
// init shader program and parameters.
//
void
Resize::initShader() 
{
	// flag to indicate shader availability
	m_shaderFlag = false;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Resize::gpuProgram:
//
// Invoke GPU program
//
void
Resize::gpuProgram(int /*pass*/) {}



void		
Resize::setImageSize(int w, int h) 
{
	m_width  = w; 
	m_height = h; 
	m_widthLinetEdit ->setText(QString::number(w));
	m_heightLinetEdit->setText(QString::number(h));
}
