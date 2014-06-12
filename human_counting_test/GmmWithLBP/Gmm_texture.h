#pragma once
// GmmModel.h: interface for the GmmModel class.
//
//////////////////////////////////////////////////////////////////////



#define MaxNumOfModel 5 // max number of Gaussian model k=7
#define MaxDegreeOfFeature 3 // only support 3 channel rgb
#define large_variance 100 // a very large variance for each initial gaussian
#define inital_mean_value 30 //initial mean value for eahc initial model
#define low_prior_weight 0.01 //Andrew 071108

#include <vector>
using namespace std;


struct ModleforPixel_texture
{	
	int Model_bitvalue[MaxNumOfModel][9]; //�O���C��distribution��bitvalue
	int bit_count[MaxNumOfModel];
	int bit_1_count[MaxNumOfModel][9];
	float Model_WeightOfeachGaussian[MaxNumOfModel]; //�O���C��distribution��Weight��
	bool  Model_matchtable[MaxNumOfModel]; //�P�_input��match�쨺��distribution
	int   Model_matchModel; //����input�ȬO�_��distribution�P���ǰt
	int	  Model_GauModelRank[MaxNumOfModel]; //�O���C��distribution���ƦW����
	float Model_RankMeasureValue[MaxNumOfModel];//�O���C��distribution��(w/var) ��

	// double Model_Luma_Expectation[];

};



class GmmModelImprove  
{
public:
	bool GmmModelMemInitial();
	void GMMBackForeModel(BYTE *Inputimg,BYTE *ResultImage); //Andrew 2007_0514
	// constructor
	GmmModelImprove(int ImgWidth,int ImgHeight,int Bpp,int NumberOfGaussian,float ThBackForegound,float LearnRate,float ThdforMatchGaussian,int m_frame_Count,float low_var_bound);
	virtual ~GmmModelImprove();
	
	int m_Width; //�v���e��
	int m_Height;  //�v������ 
	long m_PixelNumber; //�v���`�I��
	int m_Bpp; // dimension for each pixel 1(gray) or 3(RGB) 
	int m_NumOfGaussian; //�]�wGaussian distribution���ƶq
	float m_ThBackForeground; // the threshold used for seperate the background Gaussian and foreground Gaussian => T value in paper 
	float m_FinalLearnRate; // the final learning rate => different to the initial learning rate
	float m_ThdforMatchGaussian; // threshold for matching test => how many times of standard deviation
	ModleforPixel_texture *m_GMMModel;// GMM model data for each pixel
	// use constant adaptive (learning) rate or in the inital  
	int m_frame_Count;// �Ψӽվ�Learning rate
	float m_var_low_bound;// low bound of variance => can't too small
	bool m_FLAG_learning_rate_is_stable;// learning rate is stable

private:
	float CurrentLearnRate;
	bool m_MemInitial;// flag for if the GMM model is initial or not


};

