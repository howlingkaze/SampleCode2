#include "DarkChannel.h"
#include <string>
#include <queue>
#include <functional>


class mycomparison
{
  bool reverse;
public:
  mycomparison(const bool& revparam=false)
    {reverse=revparam;}
  bool operator() (const int& lhs, const int&rhs) const
  {
    if (reverse) return (lhs>rhs);
    else return (lhs<rhs);
  }
};

DarkChannel::DarkChannel() :threshold(30),blockSize(5) ,isReversed(false) ,isGetMasked(true)
{
	cerr<<"DarkChannel()"<<endl;
}

DarkChannel::~DarkChannel()
{
	cerr<<"DarkChannel()"<<endl;
}

void DarkChannel::process(Mat &img_input,Mat &out,Mat &Mask,unsigned char u_threshold)
{
	if (img_input.empty())
	{
		cout<<"Input errror no data exist!"<<endl;
	}

	int Height=img_input.size().height;	
	int Width=img_input.size().width;

	Mat ChannelMap(img_input.size(), CV_8U(0));
	threshold=u_threshold;

	unsigned char *indata,*rawMap;
	unsigned char *temp= new unsigned char[Width*Height];
	
	memset(temp,255,sizeof(char)*Width*Height);

	indata=img_input.data;
	rawMap=ChannelMap.data;
	
	for(int y=0;y<Height;y++)
	{
		for(int x=0;x<Width;x++)
		{
			int pos=x+y*Width;
			if(temp[pos] > indata[3*pos]) temp[pos]=indata[3*pos];
			if(temp[pos] > indata[3*pos+1]) temp[pos]=indata[3*pos+1];
			if(temp[pos] > indata[3*pos+2]) temp[pos]=indata[3*pos+2];
		}
	}
	
	
	int px,py;
	//std::priority_queue<unsigned char> extremeValue;
	
	//priority_queue<unsigned char, vector<unsigned char>,greater<unsigned char> >extremeValue;
	for(int y=0;y<Height;y++)
	{
		for(int x=0;x<Width;x++)
		{		
			int tn= isReversed ? 0 : 255;
			for(int z=0;z<blockSize;z++)
			{
				for(int u=0;u<blockSize;u++)
				{
					py=y+z-(blockSize)/2;
					px=x+u-(blockSize)/2;

					if(py <Height && py >=0 && px<Width && px >=0)			
						tn =  tn < temp[py*Width+px] ? tn: temp[py*Width+px]; 
						//extremeValue.push( temp[py*Width+px] );					
				}
			}			
			rawMap[y*Width+x] = tn;
		}
	}	
	
	if(isGetMasked == true)
	{	
		Mat tMask(img_input.size(), CV_8U);
		unsigned char *t =  tMask.data;
		for(int y=0;y<Height;y++)
		{
			for(int x=0;x<Width;x++)
			{
				if(rawMap[y*Width+x] > threshold)
					t[y*Width+x]=0;
				else
					t[y*Width+x]=255;
			}
		}
		tMask.copyTo(Mask);
	}
	ChannelMap.copyTo(out);

	delete []temp;
}

