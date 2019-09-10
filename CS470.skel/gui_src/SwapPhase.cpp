// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// SwapPhase.cpp - Swap phase spectrum widget.
//
// Written by: George Wolberg, 2018
// ======================================================================

#include "MainWindow.h"
#include "SwapPhase.h"

extern MainWindow *g_mainWindowP;
extern void HW_swapPhase(ImagePtr I1, ImagePtr I2, ImagePtr II1, ImagePtr II2);
enum { WSIZE, HSIZE, STEPX, STEPY, KERNEL, SAMPLER };

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SwapPhase::SwapPhase:
//
// Constructor.
//
SwapPhase::SwapPhase(QWidget *parent) : ImageFilter(parent)
{
	m_2ndImage = NULL;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SwapPhase::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
SwapPhase::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Swap Phase");

	// layout for assembling filter widget
	QVBoxLayout *vbox = new QVBoxLayout;

	// create file pushbutton
	m_button = new QPushButton("Second Image");

	// assemble dialog
	vbox->addWidget(m_button);
	m_ctrlGrp->setLayout(vbox);

	// init signal/slot connections
	connect(m_button, SIGNAL(clicked()), this, SLOT(load()));

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SwapPhase::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
SwapPhase::applyFilter(ImagePtr I1, bool gpuFlag, ImagePtr I2)
{
	// error checking
	if(I1.isNull())		return 0;
	if(m_2ndImage.isNull())	return 0;

	// init vars
	m_width  = I1->width();
	m_height = I1->height();

	// swap phase spectrums of two images
	if(!(gpuFlag && m_shaderFlag)) {
		ImagePtr II1 = NEWIMAGE;
		ImagePtr II2 = NEWIMAGE;
		ImagePtr firstImage;
		IP_copyImage(I1, firstImage);
		IP_castImage(firstImage, BW_IMAGE, firstImage);
		swapPhase(firstImage, m_2ndImage, II1, II2);

		int tx = 5;
		int ty = 5;
		int w  = II1->width();
		int h  = II1->height();

		I2->allocImage(2*w+3*tx, 2*h+3*ty, UCHARCH_TYPE);
		I2->setImageType(BW_IMAGE);

		IP_clearImage(I2);

		IP_translate(firstImage, tx, ty, firstImage);
		IP_translate(II1, tx, h+2*ty, II1);
		IP_translate(m_2ndImage, w+2*tx, ty, m_2ndImage);
		IP_translate(II2, w+2*tx, h+2*ty, II2);

		IP_addImage(firstImage, I2, I2);
		IP_addImage(m_2ndImage, I2, I2);
		IP_addImage(II1, I2, I2);
		IP_addImage(II2, I2, I2);
	} else   
		g_mainWindowP->glw()->applyFilterGPU(m_nPasses);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SwapPhase::swapPhase:
//
// Swap phase spectrums of I1 and I2.
// Output is in II1 and II2.
//
void
SwapPhase::swapPhase(ImagePtr I1,ImagePtr I2, ImagePtr II1, ImagePtr II2)
{
	HW_swapPhase(I1, I2, II1, II2);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SwapPhase::load:
//
// Slot to load second image from file.
//
int
SwapPhase::load()
{
	QFileDialog dialog(this);

	// open the last known working directory
	if(!m_currentDir.isEmpty())
		dialog.setDirectory(m_currentDir);

	// display existing files and directories
	dialog.setFileMode(QFileDialog::ExistingFile);

	// invoke native file browser to select file
	m_file = dialog.getOpenFileName(this,
		"Open File", m_currentDir,
		"Images (*.jpg *.png *.ppm *.pgm *.bmp);;All files (*)");

	// verify that file selection was made
	if(m_file.isNull()) return 0;

	// save current directory
	QFileInfo f(m_file);
	m_currentDir = f.absolutePath();

	// read kernel
	m_2ndImage = IP_readImage(qPrintable(m_file));
	IP_castImage(m_2ndImage, BW_IMAGE, m_2ndImage);

	// update button with filename (without path)
	m_button->setText(f.fileName());
	m_button->update();

	// apply filter to source image and display result
	g_mainWindowP->preview();

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SwapPhase::initShader:
//
// init shader program and parameters.
//
void
SwapPhase::initShader() 
{
	// flag to indicate shader availability
	m_shaderFlag = false;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SwapPhase::gpuProgram:
//
// Invoke GPU program
//
void
SwapPhase::gpuProgram(int) {}
