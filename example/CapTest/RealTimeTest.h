
// RealTimeTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRealTimeTestApp: 
// �йش����ʵ�֣������ RealTimeTest.cpp
//

class CRealTimeTestApp : public CWinApp
{
public:
	CRealTimeTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRealTimeTestApp theApp;