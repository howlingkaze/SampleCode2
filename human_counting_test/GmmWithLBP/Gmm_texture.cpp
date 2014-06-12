#include "Gmm_texture.h"
#include  <math.h>
#include  <stdlib.h>
#include <vector>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GmmModelImprove::GmmModelImprove(int ImgWidth,int ImgHeight,int Bpp,int NumberOfGaussian,float ThBackForegound,float LearnRate,float ThdforMatchGaussian,int frame_Count,float low_var_bound)
{
	
	m_Width=ImgWidth;
	m_Height=ImgHeight;
	m_PixelNumber=m_Width*m_Height;
	m_Bpp=Bpp; // dimension for each pixel 1(gray) or 2 (HS) or 3(RGB) 
	m_NumOfGaussian=NumberOfGaussian;
	m_ThBackForeground=ThBackForegound; // the threshold used for seperate the background Gaussian and foreground Gaussian => T value in paper 
	m_FinalLearnRate=LearnRate; // the final learning rate => different with the initial learning rate
	m_ThdforMatchGaussian=ThdforMatchGaussian; // threshold for matching test => how many times of standard deviation
	m_frame_Count=frame_Count;
	m_GMMModel=new ModleforPixel_texture[m_PixelNumber];// GMM model data for each pixel
	// remember to delete memory
	m_MemInitial=false;
	m_FLAG_learning_rate_is_stable = false;
	m_var_low_bound=low_var_bound;
}

GmmModelImprove::~GmmModelImprove()
{
	if(m_GMMModel) delete[] m_GMMModel;
}


void GmmModelImprove::GMMBackForeModel(BYTE *Inputimg,BYTE *ResultImage)  //Andrew 2007_0514
{
/*
% For each pixel, we will do the following: 
% 1) Find the best match (if any) for the new value
% 2) Update the priors 
% 3) Update the means
% 4) Update the variances
% 5) If we have no match, replace the smallest distribution
% 6) Determine which pixels are foreground 
% 7) Combine similar distributions (the means are too close, make them one) [EXPERIMENTAL STEP]
	*/
	
	float PixelValue[3];// now assume RGB color image input
	float Gray; // for gray model case 
	float mean_distance[MaxNumOfModel]; //紀錄input點與每個 distribution 平均值的距離
	float mean_distance_var_bpp3[MaxNumOfModel][MaxDegreeOfFeature]; //紀錄input點與每個distribution平均值的距離，為標準差的幾倍
	float tempMeasureValue;
	float distance;
	float WeightSum;
	float mean_var_ratio;
	float adapt_temp1,adapt_temp2,adapt_temp3;
	
	BYTE *InputimgCopy;
	BYTE *OutimgCopy;
	InputimgCopy=Inputimg;
	OutimgCopy=ResultImage;
	int bitvalue[9]={0,0,0,0,0,0,0,0,0};
	int RankIndex;
	int PixelCount=0;
	int RankCount;
	int tempk;
	int Replace_Model;
	int NumOfNoMatch;// if all samples don't match => other process 
	bool AlreadyMatch;
	bool occupy[MaxNumOfModel];
	ModleforPixel_texture CurrentPixel;
	
	long offset, i, j;
	int k,k1,k2; // Devin 2007_0414
	

	
	if(m_MemInitial) //only work when memory is initial
	{
		
		// -------------------- decide the current learn rate -------------------
		if (!m_FLAG_learning_rate_is_stable)
		{
			m_frame_Count++;
			//	time = (real_t-1)*0.1 + 1;     % Assume 10fps 
			CurrentLearnRate = 1/((float)m_frame_Count);
			if( CurrentLearnRate == 1)
			{
				CurrentLearnRate =(float) 0.9999;
			}
			else if (CurrentLearnRate < m_FinalLearnRate)
			{
				CurrentLearnRate = m_FinalLearnRate;   
				m_FLAG_learning_rate_is_stable = true;
			}
		}// end of m_FLAG_learning_rate_is_stable
		
		
		
		
		// ------------------------- Begin of processing each pixel ---------------------------
		for(i=0;i<m_Height;i++)
		{
			offset = i*m_Width;
			
			for(j=0;j<m_Width;j++)
			{
				
				//get a 3*3 input bit value
				int m=0;
				for(int p=-1;p<=1;p++)
			    {
				    for(int q=-1;q<=1;q++)
				    {
					    if(((i-1)>=0)&&((i+1)<m_Height)&&((j-1)>=0)&&((j+1)<=m_Width))
					    {
						  bitvalue[m]=InputimgCopy[(i+q)*m_Width+(j+p)];
						  m++;
					    }
						else
						{
						  bitvalue[m]=1;//超出frame邊界 令為1
						  m++;
						}
				    }
			    }
				
				// load the paramter of current pixel model
				CurrentPixel=m_GMMModel[PixelCount];
				// step one matching
				AlreadyMatch=false;
				NumOfNoMatch=0;
				CurrentPixel.Model_matchModel=-1;
				
				//occupy[MaxNumOfModel]={0,0,0,0,0};// rank table => this rank is occupied or not => for equal weight and variance
				for(k=0;k<m_NumOfGaussian;k++)
				{
					occupy[k]=false;
					//	CurrentPixel.Model_matchtable[k]=0;
				}
				
				// ----------------------- match current pixel with previous gaussian -------------------
				int closest_index=0;
			    int tmp_count1=0, tmp_count2=99;
				for( k=0;k<m_NumOfGaussian;k++)
			    {
				   for (int m=0; m<=9; m++)
				   {
				    if (bitvalue[m]!=CurrentPixel.Model_bitvalue[k][m])
					tmp_count1++;
				   }
				   if(tmp_count1 < tmp_count2)
				   {
					closest_index = k;
					tmp_count2 = tmp_count1;
				   }
				   tmp_count1=0;
			    }
			    if(tmp_count2 < m_ThdforMatchGaussian) //match
			    {
				   CurrentPixel.Model_matchModel=closest_index;
			    }
			    else
			    {
				     //	CurrentPixel.Model_matchtable[k]=false;
				   NumOfNoMatch = m_NumOfGaussian;
			    }
				
				
				for( k=0;k<m_NumOfGaussian;k++)
				{
					if(k==CurrentPixel.Model_matchModel) //判斷input值match到那個distribution
						CurrentPixel.Model_matchtable[k]=true; 
					else
						CurrentPixel.Model_matchtable[k]=false;
					
				}
				
				// ------------------------------ sorting Gau-model --------------------------------
				for(k1=0;k1<m_NumOfGaussian;k1++)
				{
					RankCount=0;
					tempMeasureValue=CurrentPixel.Model_RankMeasureValue[k1];
					for(k2=0;k2<m_NumOfGaussian;k2++)
					{	
						if (tempMeasureValue<CurrentPixel.Model_RankMeasureValue[k2])
						{
							RankCount++;
						}
						
					}//end k<m_NumOfGaussian level 2 
					while(occupy[RankCount]==true)
					{	
						RankCount++;
					}
					CurrentPixel.Model_GauModelRank[RankCount]=k1;// Rank from O, the more possible to be background
					occupy[RankCount]=true;
				}//end k<m_NumOfGaussian level 1 
				
				
				// background foreground seperate
				WeightSum=0;
				if(NumOfNoMatch==m_NumOfGaussian)
				{
					// foreground
					*OutimgCopy=255;
					OutimgCopy++;
				}
				else
				{	
					for( k=0;k<m_NumOfGaussian;k++)
					{	
						RankIndex=CurrentPixel.Model_GauModelRank[k];// Model_GauModelRank[k] tell you the k_th rank is Guassian model "RankIndex"
						
						
                        // Modified by Devin 2007_0412
						if (RankIndex==CurrentPixel.Model_matchModel)
						{
							if (WeightSum>m_ThBackForeground)
							{
								*OutimgCopy=255;
								OutimgCopy++;
								break;
							}
							else
							{
								*OutimgCopy=0;
								OutimgCopy++;
								break;
							}
						}
						WeightSum=WeightSum+CurrentPixel.Model_WeightOfeachGaussian[RankIndex];
					}	
				}//End of background foreground seperate
				
				
				
				// ---------------------------- Model update ---------------------------
				//
				// If no match, the lowest rank Gaussian will be replaced by 
				// (mean = current pixel value, large_variance)
				//
				if(NumOfNoMatch==m_NumOfGaussian)// no match at all
				{
					Replace_Model=CurrentPixel.Model_GauModelRank[m_NumOfGaussian-1];
				   // CurrentPixel.Model_matchModel=Replace_Model; //Andrew 2007_0503
				    if((m_Bpp==1))
			    	{
					  CurrentPixel.bit_count[Replace_Model]=1;
					  for (int m=0; m<=9; m++)
					  {
						CurrentPixel.Model_bitvalue[Replace_Model][m] = bitvalue[m];
						if( bitvalue[m] ==1)
						{
							CurrentPixel.bit_1_count[Replace_Model][m]=1;
						}
						else
							CurrentPixel.bit_1_count[Replace_Model][m]=0;
						
					  }
					  CurrentPixel.Model_WeightOfeachGaussian[Replace_Model] = (float)low_prior_weight;					  
				    }					
				}
				else
				{	
					//
					// update Model
					//
					for(k=0;k<m_NumOfGaussian;k++)
					{
						adapt_temp1=CurrentLearnRate*((float)CurrentPixel.Model_matchtable[k]);
						adapt_temp2=1-CurrentLearnRate;
						adapt_temp3=1-adapt_temp1;
						
						// update Model_WeightOfeachGaussian 
						CurrentPixel.Model_WeightOfeachGaussian[k]=adapt_temp2*CurrentPixel.Model_WeightOfeachGaussian[k]+adapt_temp1;//*;CurrentLearnRate
						
						if((m_Bpp==1))
						{
							if(CurrentPixel.Model_matchtable[k])
						    {
							  CurrentPixel.bit_count[k]++;							  
							  //update BTC bitmap
							  for (int m=0; m<=9; m++)
							  {
								if(bitvalue[m] ==1)
									CurrentPixel.bit_1_count[k][m]++;
								if (CurrentPixel.bit_1_count[k][m] / (float)CurrentPixel.bit_count[k] >0.1)
									CurrentPixel.Model_bitvalue[k][m] = 1;
								else
									CurrentPixel.Model_bitvalue[k][m] = 0;
							  }
							  //避免CurrentPixel.bit_count[k]的數值 overflow
							  if(CurrentPixel.bit_count[k] >= 100000)
							  {
								CurrentPixel.bit_count[k] /= 2;
								for (int m=0; m<9; m++)
									CurrentPixel.bit_1_count[k][m]/=2;
							  }
						    } 
						}				
						
					}// end of k=0;k<m_NumOfGaussian
				}// End of Model update
				
				// renormalized the weights of all Gaussian distributions
				WeightSum=0;
				for( k=0;k<m_NumOfGaussian;k++)
				{
					WeightSum=WeightSum+CurrentPixel.Model_WeightOfeachGaussian[k];
				}
				for( k=0;k<m_NumOfGaussian;k++)
				{
					CurrentPixel.Model_WeightOfeachGaussian[k]=CurrentPixel.Model_WeightOfeachGaussian[k]/WeightSum;
				}
				// save the 
				m_GMMModel[PixelCount]=CurrentPixel;
				
				PixelCount++;
			}
		}
		
		
		
		
		//		output result image BYTE 
		//		int MatchWhichModel = 0;// record the which model is matched in current pixel 
	}//end m_MemInitial
}

bool GmmModelImprove::GmmModelMemInitial()
{
	
	int i,j,k; 
	float initial_weight_of_each_Gaussian;
	initial_weight_of_each_Gaussian=1/((float)m_NumOfGaussian);//剛開始的每個distribution其weight都一樣;
	float meaninit;//nothing 
	float standinit;
	
	for(i=0;i<m_PixelNumber;i++) // m_ImgSize=m_Width*m_Height;
	{
		for(j=0;j<=m_NumOfGaussian;j++)	
		{
			meaninit=inital_mean_value+(float)30*j; // Now "inital_mean_value" is defined as 30
			standinit=large_variance+(float)2*j;
			
			m_GMMModel[i].Model_WeightOfeachGaussian[j]=initial_weight_of_each_Gaussian;
			m_GMMModel[i].Model_matchtable[j]=0;
			for(int m=0;m<=9;m++)
			{
				m_GMMModel[i].Model_bitvalue[j][m]=0;
				m_GMMModel[i].bit_1_count[j][m]=0;
				m_GMMModel[i].bit_count[j]=0;
			}
			//			m_GMMModel[i].Model_BackForegroundTable[j]=0;//which model is background 0, which is foreground 1
			m_GMMModel[i].Model_GauModelRank[j]=j; // eg. 3=Model_GauModelRank[0] => rank number 0 is Gau-model 3
			
		}
		m_GMMModel[i].Model_matchModel=-1;
	}
	m_MemInitial=true;
	return true;
}
