
// InstanceConfig.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CInstanceConfigApp:
// �йش����ʵ�֣������ InstanceConfig.cpp
//

class CInstanceConfigApp : public CWinAppEx
{
public:
	CInstanceConfigApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CInstanceConfigApp theApp;