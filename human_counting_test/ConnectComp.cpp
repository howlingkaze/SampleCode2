// ConnectComp.cpp: implementation of the CConnectComp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConnectComp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectComp::CConnectComp(int max_object_num)
{
	m_max_ObjNum = max_object_num;
	m_obj_pos = new OBJECT_INFORMATION [m_max_ObjNum];
	m_obj_number = 0;
}

CConnectComp::~CConnectComp()
{
	delete [] m_obj_pos;
}

int CConnectComp::ConnectedComponents(unsigned char *gray_image, int inw,
											  int inh, int threshold)
{
	int i,j,k;
	int index,*label_img,a,b,c,d; // 2006_1110 Comment by Devin: �s���ϰ�Φ��ɡAindex �n�[ 1
	unsigned char *m_img;
	int offset,boundary;
	

	POINT	*EqClass = new POINT[inw];
	int	indextable[65535];
	int	*temp_image = new int [inw*inh];
	int		counter[65535];

// first pass

	index = 1;
	for(i=0; i < 65535;i++)
		counter[i]=0;

	if( inw < 100 ) boundary = 3;	// ? why set boundary ?
	else boundary =5;

	offset = boundary * inw;

	label_img = temp_image + offset;
	m_img = gray_image + offset;

//	for(j=0;j< inw;j++)
//		label_img[j-inw]=0;	// �O�e�@��C���Ȭ��s(initalize)
	// ($) zhao 2003/08/28: initialize buffer
	for(i=0 ; i<inh ; i++)
		for(j=0 ; j<inw ; j++)
			temp_image[i*inw + j]=0;

/*
							������������������......������������������
							������������������......������������������
							������������������......������������������
							������������������......������������������
			initialize	�X�֡�����������������......������������������
			start		�X�֡�����������������......������������������
							������������������......������������������
							������������������......������������������
							�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G
							�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G�G
							������������������������������������������
=============================================================================

							offset = boundary*inw		label_image
						�z�X�X�X�X�X�X�X�X�X�X�X�X�X�X�X�{										    	
	temp_image:	������������......��������..������������......
											�|�X�X�X�X�X�}	
											label_img[j-inw]=0
*/
	for(i=boundary ; i< inh-boundary ; i++, label_img+=inw, m_img+=inw)
	{
		label_img[boundary-1]=0;	// �O�e�@��(������)���s(initalize)
		k=0;	 
		for(j=boundary ; j<inw-boundary ; j++)
		{
//			if(m_img[j] == 1 )
			if(m_img[j] > 0 )
			{
				a=label_img[j-inw-1];	// ���W
				b=label_img[j-inw];		// �W
				c=label_img[j-inw+1];	// �k�W
				d=label_img[j-1];		// ��
//				a b c
//				������
//			  d	����

				if( a != 0 )
				{
					label_img[j]=a;
					if( b!=0 && a!=b ) 
					{
						EqClass[k].x=a;
						EqClass[k].y=b;
						k++;
					}
					if( c!=0 && a != c)
					{
						EqClass[k].x=a;
						EqClass[k].y=c;
						k++;
					}
					if( d!=0 && a != d)
					{
						EqClass[k].x=a;
						EqClass[k].y=d;
						k++;
					}
				}
				else if( b != 0 )
				{
					label_img[j]=b;
					if( c!=0 && b != c)
					{
						EqClass[k].x=b;
						EqClass[k].y=c;
						k++;
					}
					if( d!=0 && b!=d )
					{
						EqClass[k].x=b;
						EqClass[k].y=d;
						k++;
					}
				}
				else if ( c !=0 )
				{
					label_img[j]=c;
					if( d!=0 && c != d )
					{
						EqClass[k].x=c;
						EqClass[k].y=d;
						k++;
					}
				}
				else if (d !=0)
					label_img[j]=d;

				// ��a,b,c,d��������0�A��ܷs���ϰ�Φ��A�ҥHnew index
				else
				{
					label_img[j]=index;		// new label -> new index
					indextable[index]=65535;
					if( index < 65535 ) 	
						index++;
					else
						AfxMessageBox("There are more than 65535 regions! The following process might be unstable!");
				}
			}
			else
				label_img[j]=0;
		}

		if( k > 0 )
		{
			Resolve(EqClass,k, indextable);
			for(j=boundary;j< inw-boundary;j++)
			{
//				if(m_img[j] == 1 )
				if(m_img[j] > 0 )
				{
					a=label_img[j];
					if( indextable[a] != 65535 )
					{
						b=indextable[a];
						if( b < index )	
							label_img[j]=b;
					}
				} 
			}
		} 
	}

// Second Pass ( backward, �ѤU�ӤW�ϱ��@�� )
 
	label_img = temp_image + (inh-1)*inw - offset;
	m_img = gray_image + (inh-1)*inw - offset;

	for(j=0;j<inw;j++)
		label_img[j+inw]=0;

	for(i=boundary;i< inh-boundary;i++,label_img-=inw,m_img-=inw)
	{
		k=0;
		label_img[inw-boundary]=0;
		for(j=inw-boundary-1;j > boundary;j--)
//			if(m_img[j] == 1 )
			if(m_img[j] > 0 )
			{ 
				a=label_img[j+inw+1];
				b=label_img[j+inw];
				c=label_img[j+inw-1];
				d=label_img[j+1];
				if( a != 0 )
				{
					label_img[j]=a;
					if( b!=0 && a!=b )	{EqClass[k].x=a;EqClass[k].y=b;k++;}
					if( c!=0 )
						if( a != c)	{EqClass[k].x=a;EqClass[k].y=c;k++;}
					if( d!=0 )
						if( a != d)	{EqClass[k].x=a;EqClass[k].y=d;k++;}
				}
				else if( b != 0 )
				{
					label_img[j]=b;
					if( c!=0 &&	b != c )	{EqClass[k].x=b;EqClass[k].y=c;k++;}
					if( d!=0 &&	b != d )	{EqClass[k].x=b;EqClass[k].y=d;k++;}
				}
				else if ( c !=0 )
				{
					label_img[j]=c;
					if( d!=0 &&	c != d )	{EqClass[k].x=c;EqClass[k].y=d;k++;}
				}
				else if( d!=0)	label_img[j]=d;
			}

		if( k > 0 )
			Resolve(EqClass,k, indextable);
	
		for(j=boundary;j< inw-boundary;j++)
//		if(m_img[j] == 1 )
		if(m_img[j] > 0 )
		{ 
			a=label_img[j];
			if( indextable[a] != 65535 )
				label_img[j]=indextable[a];
		}
	}	

	// chec test -------------------------------------
/*	CImageBuf testimg(inw, inh, 24);
	int pwidth = testimg.GetPaddedWidth();
	int bpp = pwidth/inw;
	unsigned char *psub;

	i=0;
	for(j=0 ; j<inh ; j++)
	{
		psub	= testimg.GetBitData() + (inh-1-j) * pwidth;
		int l = 0;

		for(k=0 ; k<inw ; k++)
		{
			l = k*bpp;

			psub[l] = temp_image[i];
			psub[l+1] = temp_image[i];
			psub[l+2] = temp_image[i];

			i++;
		}
	}
	testimg.SaveBMPFile("d:\\connectcom_temp1.bmp");
*/	// chec test -------------------------------------#




	// �ˬd�C�@�� label_img......
	index=1;
	label_img=temp_image;		// ��������
	label_img+=offset;
	indextable[1]=0;
	for(i=boundary ; i<inh-boundary ; i++, label_img+=inw)
		for(j=boundary ; j<inw-boundary ; j++)
			if( label_img[j] > 0 )
			{
				a=label_img[j-inw-1];
				b=label_img[j-inw];
				c=label_img[j-inw+1];
				d=label_img[j-1];
				if		( a > 0 ) { counter[a]++; label_img[j]=a;}
				else if	( b > 0 ) { counter[b]++; label_img[j]=b;}
				else if	( c > 0 ) { counter[c]++; label_img[j]=c;}
				else if	( d > 0 ) { counter[d]++; label_img[j]=d;}
				else
				{ 
					a= label_img[j]; b=0; 
					for(k=1 ; k<index ; k++) 
						if(a == indextable[k])
						{
				// debug: �����֤F�G�icounter[c]++�j;�ҥH�ϰ�pixel�Ʀ��~�t
							b=1;	c=k;	k=index;	counter[c]++;
						}
					if( b == 0 )
					{
						indextable[index]=a;
						label_img[j]=index;
						counter[index]=1;
						index++;
					}
					else 
						label_img[j]=c;
				}
			}

	label_img=temp_image + offset;		// ��������
	m_img=gray_image + offset;
	index=1;
//	counter[999]=0;
	for(i=boundary ; i<inh-boundary ; i++, m_img+=inw, label_img+=inw)
	{
		for(j=0; j <boundary;j++)
			m_img[j]=0;
		for(;j < inw-boundary;j++)
			if( label_img[j] > 0 )
			{
				a=label_img[j];
				if( counter[a] == -1 )
					m_img[j]=indextable[a];
				else
					if( counter[a] > threshold && a > 0)
					{
						m_img[j] = index;
						indextable[a]=index;
						m_obj_pos[index-1].c = counter[a];
						index++;
						counter[a]= -1;
					}
					else m_img[j]=0;
			}
			else m_img[j]=0;

		for(;j<inw;j++)
			m_img[j]=0;
	}

	for(;i< inh;i++,m_img+=inw, label_img+=inw)
		for(j=0;j<inw;j++) m_img[j]=0;

	delete [] EqClass;
	delete [] temp_image;


	// ---- (+) zhao 2003/08/15: for getting boundary of object

	// initialize both facebox and motion_face_positions.
	int max_count = 0;
	for(i=0 ; i<index-1 ; i++)
	{
		m_obj_pos[i].rect.top	= inh;
		m_obj_pos[i].rect.bottom= 0;
		m_obj_pos[i].rect.left	= inw;
		m_obj_pos[i].rect.right	= 0;
		m_obj_pos[i].label		= i+1;

		// get m_obj_pos label of max skin region
		if( m_obj_pos[i].c > max_count)
		{
			m_max_ObjLabel  = i;
			max_count	= m_obj_pos[i].c;
		}

		m_obj_pos[i].c = 0;
	}

	// get boundaries of each face position. �ѤW�ӤU,�A�ѥ��ܥk scan !
	unsigned char* ptr_img;
	for(j=boundary ; j<inw-boundary ; j++)
	{
		ptr_img = gray_image + boundary*inw + j;

		for(i=boundary ; i<inh-boundary ; i++, ptr_img+=inw)
		{
			// �Y��pixel�O����]�w�g�����P�ϰ즳���P index�A�Y *s_img�A�� 1 �}�l�^
			if( (*ptr_img) > 0 )
			{
				int a = (*ptr_img) - 1;	// �� 1 �}�l�A�ҥH -1 �� 0 �}�l

				if( m_obj_pos[a].rect.top	> i ) m_obj_pos[a].rect.top		= i;
				if( m_obj_pos[a].rect.bottom< i ) m_obj_pos[a].rect.bottom	= i;
				if( m_obj_pos[a].rect.left	> j ) m_obj_pos[a].rect.left	= j;
				if( m_obj_pos[a].rect.right	< j ) m_obj_pos[a].rect.right	= j;

				m_obj_pos[a].c++;
			}
		}
	}

	// ---- (+) zhao 2003/08/15: for getting boundary of object #


	return(index-1);
}

void CConnectComp::Resolve(POINT *list,int number, int *table)
{
	int i,a,b,c,flag,flag1;

 // initial �]�n�����I�h�l�^
	for(i=0; i < number;i++)
	{
		table[ list[i].x ] = 65535;
		table[ list[i].y ] = 65535; 
	}

	for(i=0; i < number;i++)
	{
		if( list[i].x > list[i].y )	// a=�j, b=�p
		{
			a=list[i].x;	b=list[i].y;
		}
		else
		{
			b=list[i].x;	a=list[i].y;
		}

		flag=TRUE;
		while(flag)
		{
			if( table[a] == 65535 )
			{
				table[a] = b;
				flag=FALSE;
			}
			else
			{
				if( table[b] == 65535 )
				{
					flag1=TRUE;flag=FALSE;
					while(flag1)
						if( table[a] < b )	
						{
							table[b]=table[a];flag1=FALSE;
						}
						else 
							if( table[a] > b )
							{
								a=table[a];
								if( table[a] == 65535 ) {table[a] = b;flag1=FALSE;}
							}
							else flag1=FALSE;
				}
				else
				{
					if( table[a] > table[b] ) {a=table[a];b=table[b];}
					else if( table[a] < table[b] )
					{
						c=table[b];b=table[a];a=c;
					}
					else flag=FALSE;
				}
			}
		}
	}

	for(i=0; i < number;i++)
	{ 
		a=list[i].x;
		c=b=table[a];
		while( b != 65535 )
		{
			c=b;
			b=table[c];
		}
		table[a]=c;

		a=list[i].y;
		c=b=table[a];
		while( b != 65535 )
		{
			c=b;
			b=table[c];
		}
		table[a]=c;
	}
}