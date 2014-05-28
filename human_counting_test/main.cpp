#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "package_bgs/jmo/MultiLayerBGS.h"
#include "package_bgs/MixtureOfGaussianV1BGS.h"
#include "package_bgs/AdaptiveBackgroundLearning.h"
#include "package_bgs/AdaptiveSelectiveBackgroundLearning.h"
#include "package_bgs/FrameDifferenceBGS.h"
#include "CvblobTracking.h"

//include whatever you need in bgs library


using namespace std;
//using namespace cv;


int main()
{
	CvCapture *InputVideo = NULL;

	InputVideo = cvCaptureFromAVI("E:/TestImageStream/SYS Memorial Hal_normal_320x240.avi");

	if(!InputVideo){
		cout<<"File not found"<<endl;
		system("pause");
		return 0;
	}
	
	IplImage *CurrentFrame = cvQueryFrame(InputVideo);
	//read first frame

	int Width=CurrentFrame->width;
	int Height=CurrentFrame->height;

	//Test some differenct background segmentation
	//IBGS *bgs1 =new MixtureOfGaussianV1BGS;
	//IBGS *bgs2 = new MultiLayerBGS;
	IBGS *bgs1 =new AdaptiveSelectiveBackgroundLearning;
	//IBGS *bgs1 =new FrameDifferenceBGS;

	CvblobTracking *blobTracking=new CvblobTracking;
	
	while(CurrentFrame)
	{
		cv::Mat tFrame(CurrentFrame),foreground,background,blob;
		// convert iplimage to Mat;

		cv::imshow("input",tFrame);

		// preprocess for better result (reference kahlil's code)
		for ( int i = 1; i < 5; i = i + 2 )
		{ 
			medianBlur ( tFrame, tFrame, i );
		}

		bgs1->process(tFrame,foreground,background);
		// do the background model or moving object detection

		cv::imshow("fore",foreground);
		cv::imshow("background",background);

		blobTracking->process(tFrame,foreground,blob);

		
		cvWaitKey(20);
		// wait 20ms, let imshow have enough time to render on screen.

		CurrentFrame = cvQueryFrame(InputVideo); // read nect frame
	}
	cout<<"end file"<<endl;

	system("pause");

}
