TEMPLATE    = app
QT 	   += widgets printsupport opengl
RESOURCES   = CS470.qrc
CONFIG     += qt debug_and_release
CONFIG     -= flat

Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR     = release/.moc
Release:RCC_DIR     = release/.obj
Debug:OBJECTS_DIR   = debug/.obj
Debug:MOC_DIR       = debug/.moc
Debug:RCC_DIR       = debug/.obj



win32{
	LIBS 		+= -L$${LIBPATH}/win/lib
	CONFIG(release, debug|release) {
		LIBS += -lIP
	} else {
		LIBS += -lIP_d 
	}
	LIBS 		+= -lopengl32
	QMAKE_CXXFLAGS	+= /MP /Zi
	QMAKE_LFLAGS	+= /MACHINE:X64
	RC_FILE		+= CS470.rc
}

macx{
	LIBS            += -L$${LIBPATH}/mac/lib
	CONFIG(release, debug|release) {
		LIBS += -lIP
	} else {
		LIBS += -lIP_d 
	}
	QMAKE_SONAME_PREFIX = @executable_path/../Frameworks
	QMAKE_LFLAGS   += -Wl,-rpath,@executable_path/../Frameworks
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
	ICON = CS470.icns
}

unix:!macx{
	CONFIG += C++11
	LIBS        += -L$${LIBPATH}/linux/lib
	CONFIG(release, debug|release) {
		LIBS += -lIP
	} else {
		LIBS += -lIP_d 
	}
}

win32-g++{
	LIBS 		+= -L$${LIBPATH}/mingw/lib
	CONFIG(release, debug|release) {
		LIBS += -lIP
	} else {
		LIBS += -lIP_d 
	}
	LIBS 		+= -lopengl32
	RC_FILE		+= CS470.rc
}


# Input
HEADERS +=	$${GUIPATH}/MainWindow.h	\
		$${GUIPATH}/ImageFilter.h	\
		$${GUIPATH}/qcustomplot.h	\
		$${GUIPATH}/Dummy.h		\
		$${GUIPATH}/Threshold.h	\
		$${GUIPATH}/Clip.h		\
		$${GUIPATH}/Quantize.h	\
		$${GUIPATH}/Gamma.h		\
		$${GUIPATH}/Contrast.h	\
		$${GUIPATH}/HistoStretch.h	\
		$${GUIPATH}/HistoMatch.h	\
		$${GUIPATH}/ErrDiffusion.h	\
		$${GUIPATH}/Blur.h		\
		$${GUIPATH}/Sharpen.h	\
		$${GUIPATH}/Median.h	\
		$${GUIPATH}/GLWidget.h	\
		$${GUIPATH}/Convolve.h	\
		$${GUIPATH}/Spectrum.h	\
		$${GUIPATH}/SwapPhase.h	\
		$${GUIPATH}/Filter.h	\
		$${GUIPATH}/Resize.h

		
SOURCES +=	$${GUIPATH}/main.cpp	\ 
		$${GUIPATH}/MainWindow.cpp 	\
		$${GUIPATH}/ImageFilter.cpp	\
		$${GUIPATH}/qcustomplot.cpp	\
		$${GUIPATH}/Dummy.cpp	\
		$${GUIPATH}/Threshold.cpp	\
		$${GUIPATH}/Clip.cpp	\
		$${GUIPATH}/Quantize.cpp	\
		$${GUIPATH}/Gamma.cpp	\
		$${GUIPATH}/Contrast.cpp	\
		$${GUIPATH}/HistoStretch.cpp\
		$${GUIPATH}/HistoMatch.cpp	\
		$${GUIPATH}/ErrDiffusion.cpp\
		$${GUIPATH}/Blur.cpp	\
		$${GUIPATH}/Sharpen.cpp	\
		$${GUIPATH}/Median.cpp	\
		$${GUIPATH}/GLWidget.cpp	\
		$${GUIPATH}/Convolve.cpp	\
		$${GUIPATH}/Spectrum.cpp	\
		$${GUIPATH}/SwapPhase.cpp	\
		$${GUIPATH}/Filter.cpp	\
		$${GUIPATH}/Resize.cpp