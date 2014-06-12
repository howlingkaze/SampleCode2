#pragma once
#include "CvblobTracking.h"
#include "DarkChannel.h"

CvblobTracking::CvblobTracking()
{
	cerr<<"cvblob_tracking constructor"<<endl;
}

CvblobTracking::~CvblobTracking()
{
	cerr<<"cvblob_tracking destructor"<<endl;
}

void CvblobTracking::process(const cv::Mat &in, const cv::Mat &fore, cv::Mat &out,int frame_num)
{
	if(in.empty() || fore.empty())
	{
		cout<<"Input/Forground images don't exist."<<endl;
		return;
	}

	IplImage* frame = new IplImage(in);
	IplImage* segmentated = new IplImage(fore);


	IplImage *labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);


	IplConvKernel* morphKernel = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, NULL);
	IplConvKernel* morphKernel2 = cvCreateStructuringElementEx(7, 7, 1, 1, CV_SHAPE_RECT, NULL);

		
	cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_ERODE, 1);	
	cvMorphologyEx(segmentated, segmentated, NULL, morphKernel2, CV_MOP_DILATE, 1);	
	cvMorphologyEx(segmentated, segmentated, NULL, morphKernel2, CV_MOP_ERODE, 1);	

	//cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_CLOSE, 1);
	

	
	//cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_OPEN, 3);
	
	//

	

	cvb::CvBlobs blobs;
    unsigned int result = cvLabel(segmentated, labelImg, blobs);

    cvFilterByArea(blobs, 930, 1000000);

	cvShowImage("morthologyed", segmentated);
	
	int count = 0;
	for (cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
	{
		count++;
		cvb::CvBlob *blob=(*it).second;
		cv::Point a = cv::Point (blob->minx,blob->miny);
		cv::Point b = cv::Point (blob->maxx,blob->maxy);		
		if(frame_num > 220 && frame_num < 480)
			cvRectangle(frame,a,b,cv::Scalar(0,0,255),1,8,0);
		else			
			cvRectangle(frame,a,b,cv::Scalar(0,255,0),1,8,0);
	}	
	cout <<count<<endl;


    //cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX);
	

	cvUpdateTracks(blobs, tracks, 200., 5);

    //cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX);


	cvShowImage("labeled image", labelImg);
	cvShowImage("object_tracking", frame);

	cv::Mat img_result(frame);
	img_result.copyTo(out);

	cvReleaseBlobs(blobs);
}
