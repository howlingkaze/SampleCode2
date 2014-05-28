#pragma once
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "cvblob/cvblob.h"

using namespace std;

class CvblobTracking
{
	int maxArea;
	int minArea;
	cvb::CvTracks tracks;
public:
	CvblobTracking();
	~CvblobTracking();
	void process(const cv::Mat &in, const cv::Mat &fore, cv::Mat &out);
};
