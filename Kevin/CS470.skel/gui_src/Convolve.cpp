// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2018 by George Wolberg
//
// Convolve.cpp - Convolve widget.
//
// Written by: George Wolberg, 2018
// ======================================================================

#include "MainWindow.h"
#include "Convolve.h"

extern MainWindow *g_mainWindowP;
extern void HW_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2);
enum { WSIZE, HSIZE, STEPX, STEPY, KERNEL, SAMPLER };

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convolve::Convolve:
//
// Constructor.
//
Convolve::Convolve(QWidget *parent) : ImageFilter(parent)
{
	m_kernel = NULL;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convolve::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Convolve::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Convolve");

	// layout for assembling filter widget
	QVBoxLayout *vbox = new QVBoxLayout;

	// create file pushbutton
	m_button = new QPushButton("File");

	// create text edit widget
	m_values = new QTextEdit();
	m_values->setReadOnly(1);

	// assemble dialog
	vbox->addWidget(m_button);
	vbox->addWidget(m_values);
	m_ctrlGrp->setLayout(vbox);

	// init signal/slot connections
	connect(m_button, SIGNAL(clicked()), this, SLOT(load()));

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convolve::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Convolve::applyFilter(ImagePtr I1, bool gpuFlag, ImagePtr I2)
{
	// error checking
	if(I1.isNull())		return 0;
	if(m_kernel.isNull())	return 0;
	m_width  = I1->width();
	m_height = I1->height();
	// convolve image
	if(!(gpuFlag && m_shaderFlag))
		convolve(I1, m_kernel, I2);
	else    g_mainWindowP->glw()->applyFilterGPU(m_nPasses);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convolve::convolve:
//
// Convolve image I1 with convolution filter in kernel.
// Output is in I2.
//
void
Convolve::convolve(ImagePtr I1, ImagePtr kernel, ImagePtr I2)
{
	HW_convolve(I1, kernel, I2);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convolve::load:
//
// Slot to load filter kernel from file.
//
int
Convolve::load()
{
	QFileDialog dialog(this);

	// open the last known working directory
	if(!m_currentDir.isEmpty())
		dialog.setDirectory(m_currentDir);

	// display existing files and directories
	dialog.setFileMode(QFileDialog::ExistingFile);

	// invoke native file browser to select file
	m_file =  dialog.getOpenFileName(this,
		"Open File", m_currentDir,
		"Images (*.AF);;All files (*)");

	// verify that file selection was made
	if(m_file.isNull()) return 0;

	// save current directory
	QFileInfo f(m_file);
	m_currentDir = f.absolutePath();

	// read kernel
	m_kernel = IP_readImage(qPrintable(m_file));

	// init vars
	int w = m_kernel->width ();
	int h = m_kernel->height();

	// update button with filename (without path)
	m_button->setText(f.fileName());
	m_button->update();

	// declarations
	int type;
	ChannelPtr<float> p;
	QString s;

	// get pointer to kernel values
	IP_getChannel(m_kernel, 0, p, type);

	// display kernel values
	m_values->clear();			// clear text edit field (kernel values)
	for(int y=0; y<h; y++) {		// process all kernel rows
		s.clear();			// clear string
		for(int x=0; x<w; x++)		// append kernel values to string
			s.append(QString("%1   ").arg(*p++));
		m_values->append(s);		// append string to text edit widget
	}

	// apply filter to source image and display result
	g_mainWindowP->preview();

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convolve::initShader:
//
// init shader program and parameters.
//
void
Convolve::initShader() 
{
	// number of passes in filter cascade
	m_nPasses = 1;

	// initialize GL function resolution for current context
	initializeGLFunctions();

	// init uniform hash table based on uniform variable names and location IDs
	UniformMap uniforms;
	uniforms["u_Wsize"  ] = WSIZE;
	uniforms["u_Hsize"  ] = HSIZE;
	uniforms["u_StepX"  ] = STEPX;
	uniforms["u_StepY"  ] = STEPY;
	uniforms["u_Kernel" ] = KERNEL;
	uniforms["u_Sampler"] = SAMPLER;

        QString v_name = ":/vshader_passthrough.glsl";
        QString f_name = ":/fshader_convolve.glsl";
 
	// compile shader, bind attribute vars, link shader, and initialize uniform var table
	g_mainWindowP->glw()->initShader(m_program[PASS1], 
	                                 v_name, 
	                                 f_name,
					 uniforms,
					 m_uniform[PASS1]);

	// flag to indicate shader availability
	m_shaderFlag = true;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convolve::gpuProgram:
//
// Invoke GPU program
//
void
Convolve::gpuProgram(int pass) 
{
	// collect parameters
	int w_size   = m_kernel->width();
	int h_size   = m_kernel->height();
	ChannelPtr<float> p;
	int type;
	IP_getChannel(m_kernel, 0, p, type);

	// pass parameters to shader
	glUseProgram(m_program[pass].programId());
	glUniform1i (m_uniform[pass][WSIZE], w_size);
	glUniform1i (m_uniform[pass][HSIZE], h_size);
	glUniform1fv(m_uniform[pass][KERNEL], 128, (GLfloat *)&p[0]);
	glUniform1f (m_uniform[pass][STEPX], (GLfloat) 1.0f / m_width);
	glUniform1f (m_uniform[pass][STEPY], (GLfloat) 1.0f / m_height);
	glUniform1i (m_uniform[pass][SAMPLER], 0);
}
