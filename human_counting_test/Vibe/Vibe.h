#pragma once 
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cv.h"      // include core library interface
#include "highgui.h" // include GUI library interface



class VibeBGSmodel
{
	int N;
	int R;
	int threshold;
	int phi; // amount of random subsample
	unsigned char** samples;

public :
	VibeBGSmodel();
	~VibeBGSmodel();
	void initialize_background (unsigned char *input, int height, int width);
	void ViBE_BG_Seg(unsigned char *input, unsigned char *segmap, int height, int width);
};