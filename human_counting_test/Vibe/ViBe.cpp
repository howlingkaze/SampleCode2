#pragma once
#include "Vibe.h"
//#include "ConnectComp.h"
#include <iostream>
#include <cmath>
#include <string.h>
#include <queue>
#include <fstream>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


#define NEW2D(H, W, TYPE) (TYPE **)new2d(H, W, sizeof(TYPE))

void* new2d(int h, int w, int size)
{
    register int i;
    void **p;

    p = (void**)new char[h*sizeof(void*) + h*w*size];
    for(i = 0; i < h; i++)
    {
        p[i] = ((char *)(p + h)) + i*w*size; 
    }

    return p;
}

VibeBGSmodel::VibeBGSmodel() :N(20), R(20), threshold(2), phi(16)
{
	cerr<<"In VibeBGSmodel()";
}

VibeBGSmodel::~VibeBGSmodel()
{
	cerr<<"In ~VibeBGSmodel()";
	delete []samples;
}



void VibeBGSmodel::initialize_background (unsigned char *input, int height, int Width)
{


	samples = NEW2D(height*Width*3,N, unsigned char);

	for(int i=0;i<height;i++)
	{
		for(int j=0;j<Width;j++)
		{
			
			int candidate[9][3];
			for (int k=0;k<9;k++)
			{
				for (int x=-1;x<=1;x++)
				{
					for(int y=-1;y<=1;y++)
					{
						/*
					//	if(((i+x)>=0) && ((j+y) >=0) && ((i+x)<height) && ((j+y)<Width))
						{
						candidate[k][0] = input[(i+x)*Width*3+(j+y)*3+0];
						candidate[k][1] = input[(i+x)*Width*3+(j+y)*3+1];
						candidate[k][2] = input[(i+x)*Width*3+(j+y)*3+2];
						}
						*/
					
						//else
						{
						candidate[k][0] = input[(i)*Width*3+(j)*3+0];
						candidate[k][1] = input[(i)*Width*3+(j)*3+1];
						candidate[k][2] = input[(i)*Width*3+(j)*3+2];
						}
					
					}
				}
			}
			for(int n=0; n< N;n++)
			{
				srand(time(NULL));
				int a =(rand()%9);
				samples[(i)*Width*3+(j)*3+0][n] = candidate[a][0];
				samples[(i)*Width*3+(j)*3+1][n] = candidate[a][1];
				samples[(i)*Width*3+(j)*3+2][n] = candidate[a][2];
			}

		}
	}

}

void VibeBGSmodel::ViBE_BG_Seg(unsigned char *input, unsigned char *segmap, int height, int Width)
{
	int rand_1;
	int rand_2;
	srand(time(NULL));
	for(int j=0;j<Width;j++)
	{
		for(int i=0;i<height;i++)
		{
			int count =0;
			int index =0;
			int dist =0;
			while((count<threshold)&&(index<N))
			{
				dist = abs(input[(i)*Width*3+(j)*3+0] - samples[(i)*Width*3+(j)*3+0][index])+
					   abs(input[(i)*Width*3+(j)*3+1] - samples[(i)*Width*3+(j)*3+1][index])+
					   abs(input[(i)*Width*3+(j)*3+2] - samples[(i)*Width*3+(j)*3+2][index]);
				if(dist<R)
				{
					count++;
				}
				index++;
			}
			if(count>=threshold) //background
			{
				segmap[(i)*Width+(j)]=0;				
				int rand_1 = (rand()%phi);
				if(rand_1==0)
				{
					int rand_2 = (rand()%N);
					samples[(i)*Width*3+(j)*3+0][rand_2] = input[(i)*Width*3+(j)*3+0];
					samples[(i)*Width*3+(j)*3+1][rand_2] = input[(i)*Width*3+(j)*3+1];
					samples[(i)*Width*3+(j)*3+2][rand_2] = input[(i)*Width*3+(j)*3+2];
				}				
				rand_1 = (rand()%2);
				
				if(rand_1 ==0) // update neighbor
				{				
					int xn=0;
					int yn=0;
					while((xn==0)&&(yn==0))
					{
						xn = (rand()%3);
						yn = (rand()%3);
						xn = xn -1;
						yn = yn -1;
					}					
					rand_2 = (rand()%20);
					if(((i+yn)>=0) && ((j+xn) >=0) && ((i+yn)<height) && ((j+xn)<Width))
					{
					samples[(i+yn)*Width*3+(j+xn)*3+0][rand_2] = input[(i)*Width*3+(j)*3+0];
					samples[(i+yn)*Width*3+(j+xn)*3+1][rand_2] = input[(i)*Width*3+(j)*3+1];
					samples[(i+yn)*Width*3+(j+xn)*3+2][rand_2] = input[(i)*Width*3+(j)*3+2];
					}

				}
			}
			else
			{
				segmap[(i)*Width+(j)]=255;
			}
		}
	}
}
