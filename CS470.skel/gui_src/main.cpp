// ======================================================================
// Image Processing Homework Solutions
// Copyright (C) 2018 by George Wolberg
//
// main.c - main() function.
//
// Written by: George Wolberg, 2018
// ======================================================================

#include "MainWindow.h"

int main(int argc, char **argv)
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	QApplication::setStyle(QStyleFactory::create("fusion"));
	QApplication app(argc, argv);		// create application
	MainWindow window;			// create UI window
	window.setMinimumSize(QSize(1000, 640));// set size of minimized window
	window.showMaximized();			// display window
	return app.exec();			// infinite processing loop
}
