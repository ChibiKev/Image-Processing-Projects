TARGET      = CS470_HW

INCLUDEPATH += . ./IP/header ./gui_src
LIBPATH = ./IP
GUIPATH = ./gui_src
DESTDIR = ./
include(CS470_common.pro)


SOURCES +=	hw1/HW_clip.cpp		\
		hw1/HW_contrast.cpp	\
		hw1/HW_gamma.cpp	\
		hw1/HW_histoMatch.cpp	\
		hw1/HW_histoStretch.cpp	\
		hw1/HW_quantize.cpp	\
		hw1/HW_threshold.cpp	\
		hw2/HW_blur.cpp		\
		hw2/HW_convolve.cpp	\
		hw2/HW_errDiffusion.cpp	\
		hw2/HW_median.cpp	\
		hw2/HW_sharpen.cpp	\
		hw3/HW_filter.cpp	\
		hw3/HW_spectrum.cpp	\
		hw3/HW_swapPhase.cpp	\
		hw3/HW_utils.cpp	\
		hw4/HW_resize.cpp
