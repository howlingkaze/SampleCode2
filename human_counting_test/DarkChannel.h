#pragma once
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;
using namespace std;

class DarkChannel
{	
	unsigned char threshold;
	bool isReversed;
	bool isGetMasked;
	int blockSize;
public :
	DarkChannel();
	~DarkChannel();	
	void process(Mat &img_input,Mat &out,Mat &Mask,unsigned char threshold);
	Mat out;	
};

