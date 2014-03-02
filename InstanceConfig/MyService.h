
// wx_kk_service.h

#pragma once

#ifndef WW_KK_SERVICE_H
#define WW_KK_SERVICE_H

#include "winsvc.h"
  
LPTSTR   GetLastErrorText(   LPTSTR   lpszBuf,   DWORD   dwSize   ) ;

class MyService
{
public: 
	MyService();
	~MyService();

	
//����ȫ�ֺ�������

	void Init();
	BOOL Install();
	BOOL Uninstall();

	BOOL OpenService();//�򿪷���
	BOOL StopService();//ֹͣ����

	SERVICE_STATUS_HANDLE hServiceStatus;
	SERVICE_STATUS status;;//SERVICE�����״̬struct   
	DWORD dwThreadID;

	SC_HANDLE hSCM;
	SC_HANDLE hService;

	char   m_lpServicePathName[512];         //SERVICE�����EXE�ļ�·��   
	char szServiceName[256];
	char MonitorExe[128];

};


#endif