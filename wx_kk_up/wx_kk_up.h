
// wx_kk_up.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#include "zlog.h"
#include "pthread.h"
#include "FileUtil.h"

#include "CrashHandler.h"

#include "sqlite3.h"
#include "mysql.h"
#include "md5.h"
#include "tfcommon.h"


// Cwx_kk_upApp:
// �йش����ʵ�֣������ wx_kk_up.cpp
//

class Cwx_kk_upApp : public CWinAppEx
{
public:
	Cwx_kk_upApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern Cwx_kk_upApp theApp;