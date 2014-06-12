// ConnectComp.h: interface for the CConnectComp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTCOMP_H__D05E3481_5D18_4D77_B812_B29DEFEB9915__INCLUDED_)
#define AFX_CONNECTCOMP_H__D05E3481_5D18_4D77_B812_B29DEFEB9915__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectInfo.h"

class CConnectComp  
{
	int m_max_ObjNum;

public:
	int m_max_ObjLabel, m_obj_number;
	OBJECT_INFORMATION *m_obj_pos;

	CConnectComp(int max_object_num);
	virtual ~CConnectComp();

	int ConnectedComponents(unsigned char *gray_image, int inw, int inh, int threshold);
	void Resolve(POINT *list,int number, int *table);

};

#endif // !defined(AFX_CONNECTCOMP_H__D05E3481_5D18_4D77_B812_B29DEFEB9915__INCLUDED_)
