
// wx_kk_service.h

#pragma once

#ifndef WW_KK_SERVICE_H
#define WW_KK_SERVICE_H


#include "windows.h"


LPTSTR   GetLastErrorText(   LPTSTR   lpszBuf,   DWORD   dwSize   ) ;
void LogEvent(LPCTSTR pszFormat, ...);
void log(LPCTSTR format, ...);

class wx_service
{
public: 
	wx_service();
	~wx_service();

	
//����ȫ�ֺ�������

	static void Init();
	static BOOL IsInstalled();
	BOOL Install();
	BOOL Uninstall();

	static void WINAPI ServiceMain(DWORD   dwArgc, LPTSTR   *lpszArgv); 
	
	static void WINAPI ServiceStrl(DWORD dwOpcode);
	BOOL OpenService();//�򿪷���
	BOOL StopService();//ֹͣ����
	BOOL PauseService();//��ͣ����
	BOOL ResumeService();//��������

	static BOOL bInstall;
	static SERVICE_STATUS_HANDLE hServiceStatus;
	static SERVICE_STATUS status;;//SERVICE�����״̬struct   
	static DWORD dwThreadID;

	static SC_HANDLE hSCM;
	static SC_HANDLE hService;

	static char   m_lpServicePathName[512];         //SERVICE�����EXE�ļ�·��   
	static char szServiceName[256];
	static BOOL bPause;
	static BOOL bStop;
	static void Run();
	static char MonitorExe[128];

};


#endif