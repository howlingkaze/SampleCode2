#include "CvblobTracking.h"

CvblobTracking::CvblobTracking()
{
	cerr<<"cvblob_tracking constructor"<<endl;
}

CvblobTracking::~CvblobTracking()
{
	cerr<<"cvblob_tracking destructor"<<endl;
}

void CvblobTracking::process(const cv::Mat &in, const cv::Mat &fore, cv::Mat &out)
{
	if(in.empty() || fore.empty())
	{
		cout<<"Input/Forground images don't exist."<<endl;
		return;
	}

	IplImage* frame = new IplImage(in);
	IplImage* segmentated = new IplImage(fore);
	IplImage *labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);
	IplConvKernel* morphKernel = cvCreateStructuringElementEx(5, 5, 1, 1, CV_SHAPE_CROSS, NULL);



	cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_OPEN, 1);	
	cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_DILATE, 1);	
	cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_ERODE, 1);	
	
	
	//cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_CLOSE, 1);

	cvShowImage("morthologyed", segmentated);

	cvb::CvBlobs blobs;
    unsigned int result = cvLabel(segmentated, labelImg, blobs);

    cvFilterByArea(blobs, 500, 1000000);

    cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX);
	cvUpdateTracks(blobs, tracks, 200., 5);
    cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX);

	cvShowImage("labeled image", labelImg);
	cvShowImage("object_tracking", frame);




	 cv::Mat img_result(frame);
	 img_result.copyTo(out);

	cvReleaseBlobs(blobs);
}