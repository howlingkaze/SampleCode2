#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

#include "package_bgs/jmo/MultiLayerBGS.h"
#include "package_bgs/MixtureOfGaussianV1BGS.h"
#include "package_bgs/AdaptiveBackgroundLearning.h"
#include "package_bgs/AdaptiveSelectiveBackgroundLearning.h"
#include "package_bgs/FrameDifferenceBGS.h"
#include "CvblobTracking.h"
#include "DarkChannel.h"
#include "Vibe/ViBe.h"

#include <boost/config/user.hpp>
#include <boost/lexical_cast.hpp>

//include whatever you need in bgs library

using namespace std;
using namespace cv;

int absChar(unsigned char &a,unsigned char &b)
{
	return a>b ? a-b : b-a;
}

int block_var(unsigned char* a, unsigned char*b,int initX,int initY,int block,int width,int height)
{
	int diff=0;
	for(int y=initY; y<initY+block;y++)
	{
		for(int x=initX; x<initX+block;x++)
		{
			if(y>=0 && x>=0 && y<height && x<width)
				diff += absChar(a[y*width+x],b[y*width+x]);
		}
	}
	return diff;
}




void fullSearchFlow(Mat &prevFrrame,Mat &nowFrame,Mat &out_img,int block,int range,int difference)
{
	int width = prevFrrame.size().width;
	int height = prevFrrame.size().height;
	unsigned char *prev = prevFrrame.data,*now = nowFrame.data,*out = out_img.data;
	
	for(int y=0; y< height; y+=block)
	{
		for(int x=0; x< width; x+=block)
		{
			int px=0,py=0,min= INT_MAX,vx=0,vy=0;

			for(int z=0; z< range; z++)
			{
				for (int u=0; u<range; u++)
				{
					px= x+z-(range)/2;
					py= y+u-(range)/2;					
					int t= block_var(prev,now,px,py,block,width,height);
					if ( min > t )
					{
						min = t;vx =u;vy=z;
					}
				}
			}			
			//print the vector on prev
			putText(out_img, boost::lexical_cast<std::string>(vx)+boost::lexical_cast<std::string>(vy),
				cv::Point(px,py+block/2), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255,255,255));
		}
	}
	imshow("labeled motion",out_img);

}


int main()
{

	cv::VideoCapture type("E:/TestImageStream/M4H02584.avi"); 
	int ex = static_cast<int>(type.get(CV_CAP_PROP_FOURCC));
	type.release();




	CvCapture *InputVideo = NULL;
	InputVideo = cvCaptureFromAVI("E:/TestImageStream/SYS Memorial Hal_advanced_320x240.avi");
	//InputVideo = cvCaptureFromFile("E:/TestImageStream/M4H02584.avi");

	 
	
	if(!InputVideo){
		cout<<"File not found"<<endl;
		system("pause");
		return 0;
	}
	
	IplImage *CurrentFrame = cvQueryFrame(InputVideo);
	//read first frame

	int Width=CurrentFrame->width;
	int Height=CurrentFrame->height;
	//Width/=2;Height/=2;



	//Test some differenct background segmentation
	IBGS *bgs1 =new MixtureOfGaussianV1BGS;
	//IBGS *bgs2 = new MultiLayerBGS;
	//IBGS *bgs1 =new AdaptiveSelectiveBackgroundLearning;
	//IBGS *bgs1 =new FrameDifferenceBGS;

	CvblobTracking *blobTracking=new CvblobTracking;
	bool sample=false; 

	cv::VideoWriter outputVideo;
	outputVideo.open("E:/TestImageStream/output.avi", ex, 29,cv::Size(Width,Height) , true);


	//for vibe
	VibeBGSmodel vibe;
	unsigned char* input_data = reinterpret_cast<unsigned char *>(CurrentFrame->imageData);
	IplImage *segmap = cvCreateImage(cvSize( Width,  Height),IPL_DEPTH_8U, 1);	
	unsigned char*	c_segmap = reinterpret_cast<unsigned char *>(segmap->imageData);

	int fram_num=0;
	DarkChannel test;

	cv::Mat preGreyFrame(Size(Width,Height),CV_8U);
	
	
	while(CurrentFrame)
	{		

		cv::Mat tFrame(CurrentFrame,0);

		cv::Mat foreground(tFrame.size(),CV_8U),background,blob,tFrameGrey,edgeMask;
				
		unsigned char*	c_segmap = foreground.data;
		
		cv::cvtColor(tFrame, tFrameGrey, CV_BGR2GRAY);

		cv::Mat copy;
		tFrame.copyTo(copy);

		if (fram_num!=1)
		{			
			fullSearchFlow(preGreyFrame,tFrameGrey,copy,20,9,1000);		
		}


		tFrameGrey.copyTo(preGreyFrame);
		
		cvWaitKey(20);
		// wait 20ms, let imshow have enough time to render on screen.

		CurrentFrame = cvQueryFrame(InputVideo); // read nect frame
		fram_num ++;
		outputVideo<<blob;
	}
	cout<<"end file"<<endl;
		

}
