
#include "wx_kk_service.h"
#include "stdio.h"
#include <tlhelp32.h> 
#include "time.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

//#include "ShareModule.h"
//#pragma comment(lib, "ShareModule.lib")

wx_service * m_pService = 0;

//log
char logFilePath[512]={0};


BOOL wx_service::bInstall;
SERVICE_STATUS_HANDLE wx_service::hServiceStatus;
SERVICE_STATUS wx_service::status;;//SERVICE�����״̬struct   
DWORD wx_service::dwThreadID=0;

SC_HANDLE wx_service::hSCM={0};
SC_HANDLE wx_service::hService={0};

char   wx_service::m_lpServicePathName[512]={0};         //SERVICE�����EXE�ļ�·��   
char wx_service::szServiceName[256]={0};
BOOL wx_service::bPause=false;
BOOL wx_service::bStop=false;
char wx_service::MonitorExe[128]={"wx_kk_up.exe"};
#define SLEEP_TIME 1000


//������ģ����
void  wx_service::Run()
{
	char szError[512]={0};
	
	char path[512]={0};
	char MonitorExePath[512]={0};

	log( "wx_service::Runing ") ;

	GetModuleFileName(0, path, MAX_PATH);
	char *p = strrchr( path, '\\');
	if(p==0)
		return ;
	*p = '\0';
	int index = 0;

	DWORD id = -1;
	PROCESSENTRY32 pInfo = {0}; 
	HANDLE hSnapShot = 0;

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = {0};

	sprintf(MonitorExePath,"%s\\%s", path, MonitorExe);
	log("MonitorExePath = %s", MonitorExePath);

	while ( true )
	{		
		if(bStop) 
			goto end;

		while(bPause == true){
			if(bStop) goto end;
			Sleep(1000);
		}

		hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0) ;  
	//	log(" ===========         Query Process  ======================== ");
		pInfo.dwSize = sizeof(pInfo); 
		Process32First(hSnapShot, &pInfo) ;
		id = -1;
		do {
		//	log("Process [%s]   ", pInfo.szExeFile);
			if(strcmp(pInfo.szExeFile, MonitorExe) == 0) 
			{ 
				id = pInfo.th32ProcessID ;
				break ;
			} 
		}while( Process32Next(hSnapShot, &pInfo) != FALSE );
		CloseHandle(hSnapShot);

		if (id == -1)
		{
			log(" I think Process [%s] is dump ", MonitorExe);

			si.dwFlags = STARTF_USESHOWWINDOW; 
			si.wShowWindow = SW_SHOW; 

			BOOL bRet = CreateProcess (
				MonitorExePath,
				NULL,
				NULL,
				NULL,
				FALSE,
				DETACHED_PROCESS,
				NULL,
				path,
				&si,
				&pi) ;

			Sleep(SLEEP_TIME);

			if(bRet)
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				log(" CreateProcess success. dwProcessId:%d  dw Main ThreadId: %d ",pi.dwProcessId, pi.dwThreadId);
			}
			else
			{
				log(" CreateProcess failure : %s", GetLastErrorText(szError, 512) );
			}

		}
	 	Sleep( SLEEP_TIME );
	}

end:
	return ;
}


/*********************************************************************   
**   ��ϵͳ��ȡ���һ�δ�����룬��ת�����ַ�������   
*********************************************************************/   
LPTSTR   GetLastErrorText(   LPTSTR   lpszBuf,   DWORD   dwSize   )     
{   
	DWORD   dwRet;   
	LPTSTR   lpszTemp   =   NULL;   

	dwRet   =   FormatMessage(   FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ARGUMENT_ARRAY,   
		NULL,   
		GetLastError(),   
		LANG_NEUTRAL,   
		(LPTSTR)&lpszTemp,   
		0,   
		NULL   );   

	//   supplied   buffer   is   not   long   enough   
	if(!dwRet||((long)dwSize<(long)dwRet+14))   
		lpszBuf[0]   =   TEXT('\0');   
	else{   
		lpszTemp[lstrlen(lpszTemp)-2]   =   TEXT('\0');     //remove   cr   and   newline   character   
		sprintf(   lpszBuf,   TEXT("%s   (%ld)"),   lpszTemp,   GetLastError()   );   
	}   

	if   (   lpszTemp   )   
		LocalFree((HLOCAL)   lpszTemp   );   

	return   lpszBuf;   
}   

wx_service::wx_service()
{
	m_pService = this;
	memset(szServiceName, 0 , 256);
	strcpy(szServiceName, "TF_wx_kk_Service");
}
wx_service::~wx_service()
{

}

void wx_service::Init(){

	hServiceStatus = NULL;
	status.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
	status.dwCurrentState = SERVICE_STOPPED;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	status.dwWin32ExitCode = 0;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;

	memset(m_lpServicePathName, 0 , 512);
	GetModuleFileName(NULL,   &m_lpServicePathName[1],   512);   //ȡ��ǰִ���ļ�·�� 

	char *pre =  &m_lpServicePathName[1];
	char *p = strrchr( pre, '\\');
	snprintf( logFilePath, p-pre , "%s", pre);
	strncat(  logFilePath, ("\\wx_kk_up_service.log"), strlen("\\wx_kk_up_service.log"));

	m_lpServicePathName[0] = '\"';
	m_lpServicePathName[strlen(m_lpServicePathName)] = '\"';

	log("Init");
}

BOOL wx_service::Install()
{
	char szError[512]={0};
	bool bRet = false;
	//�����г���Ҫ�����������������Ŀ����ڴ������ҡ�
	//�򿪷�����ƹ�����

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	long ret = GetLastError();
	if(hSCM==NULL){
		LogEvent( GetLastErrorText(szError, 512) );
		return false;
	}

	//��������

	hService = ::CreateService(
		hSCM, szServiceName, szServiceName, // szServiceName  "TF-��֡APP������"
		SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		m_lpServicePathName, NULL, NULL, (""), NULL, NULL);
	if(hService)
		bRet = true;
	else
		LogEvent( GetLastErrorText(szError, 512) );

	::CloseServiceHandle(hService);

	::CloseServiceHandle(hSCM);

	return bRet;
}

//�򿪷���
BOOL wx_service::OpenService()
{
	char szError[512]={0};
	bool bRet = false;
	bPause = false;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if( hSCM)               //   �����SERVICE�������ɹ�   
	{   
		hService   =  ::OpenService(         //��ȡSERVICE���ƾ����API   
			hSCM,                         //SCM���������   
			szServiceName,                 //SERVICE�ڲ����ƣ���������   
			SERVICE_ALL_ACCESS);         //�򿪵�Ȩ�ޣ�ɾ����Ҫȫ��Ȩ��

		if(hService){
			if(QueryServiceStatus(hService,   &status)){
				if ( status.dwCurrentState==SERVICE_STOPPED )
				{
					if(::StartService(hService, NULL, NULL))
						bRet = true;
					else
						LogEvent( GetLastErrorText(szError, 512) );
				}
			}
			
		}else{
			LogEvent( GetLastErrorText(szError, 512) );
		}
end:
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
	}else{
		LogEvent( GetLastErrorText(szError, 512) );
	}

	return bRet;
}

//��ͣ����
BOOL wx_service::PauseService()
{
	bPause = true;
	return true;
}
BOOL wx_service::ResumeService()
{
	bPause = false;
	return true;
}

//ֹͣ����
BOOL wx_service::StopService()
{
	char szError[512]={0};
	bool bRet = false;
	bPause = true;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if( hSCM)               //   �����SERVICE�������ɹ�   
	{   
		hService   =  ::OpenService(         //��ȡSERVICE���ƾ����API   
			hSCM,                         //SCM���������   
			szServiceName,                 //SERVICE�ڲ����ƣ���������   
			SERVICE_ALL_ACCESS);         //�򿪵�Ȩ�ޣ�ɾ����Ҫȫ��Ȩ��

		if(hService){
			if(QueryServiceStatus(hService,   &status)){
				if ( status.dwCurrentState != SERVICE_STOPPED )
				{
					if( ControlService(hService,   SERVICE_CONTROL_STOP, &status) ){
						bRet = true;
					}else
						LogEvent( GetLastErrorText(szError, 512) );
				}
			}
		}else
			LogEvent( GetLastErrorText(szError, 512) );

		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
	}else{
		LogEvent( GetLastErrorText(szError, 512) );
	}
	return bRet;
}



BOOL wx_service::Uninstall()
{
	char szError[512]={0};
	bool bRet = false;
	long times = 0;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if( hSCM)               //   �����SERVICE�������ɹ�   
	{   
		hService   =  ::OpenService(         //��ȡSERVICE���ƾ����API   
			hSCM,                         //SCM���������   
			szServiceName,                 //SERVICE�ڲ����ƣ���������   
			SERVICE_ALL_ACCESS);         //�򿪵�Ȩ�ޣ�ɾ����Ҫȫ��Ȩ��

		if(hService){
			if( ControlService(hService,   SERVICE_CONTROL_STOP, &status) ){
				//ֱ����SERVICE��STOP�������ܹ�ִ�е����˵��SERVICE������   
				//�Ǿ���Ҫֹͣ����ִ�к����ɾ��   
				Sleep(3000)   ;   //��3��ʹϵͳ��ʱ��ִ��STOP����  
				while( QueryServiceStatus(hService,   &status)   )   {
					times ++;
					if(status.dwCurrentState   ==   SERVICE_STOP_PENDING)   
					{         //���SERVICE������ִ��(PENDING)ֹͣ����   
						Sleep(1000)   ;         //�Ǿ͵�1���Ӻ��ټ��SERVICE�Ƿ�ֹͣOK  
					}else {
						break ; //STOP�������ϣ�����ѭ��   
					}
					if(times>10)
						break;// time out 

				}//ѭ�����SERVICE״̬����   
				
			}else
				LogEvent( GetLastErrorText(szError, 512) );

			if(status.dwCurrentState   !=   SERVICE_STOPPED)  
			{          //���SERVICE����STOP�����û��STOPPED   
				LogEvent( GetLastErrorText(szError, 512) );
				;         //�Ǿͷ���FALSE������GetLastErrorȡ�������   
			}else{
				//ɾ��ָ��������   
				if( ::DeleteService(hService))
					bRet = true;
				else
					LogEvent( GetLastErrorText(szError, 512) );
			} 

		}//if(hService)
		else
			LogEvent( GetLastErrorText(szError, 512) );
		

		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
	}else{
		LogEvent( GetLastErrorText(szError, 512) );
	}
	return bRet;
}


/*********************************************************************   
**   �˺�������ϵͳ������STOP��PAUSE����������Ӧ������Service_Main,   
**   ÿһ��ָ��ִ�к��豨�浱ǰ״̬��SCM������   
*********************************************************************/   

void   WINAPI   ServiceHandle(DWORD   dwControl)   
{   
	switch(dwControl){   
		  case   SERVICE_CONTROL_STOP:   //Ҫ����ֹͣ
			  m_pService->status.dwWaitHint = 5;
			  m_pService->status.dwCurrentState = SERVICE_STOP_PENDING;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			 // m_pService->StopService()  ;  
			  m_pService->bStop = true;
			  Sleep(3000);
			  m_pService->status.dwCurrentState = SERVICE_STOPPED;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  LogEvent(("Service stopped"));
			  log("Service stopped");
			 // PostThreadMessage( m_pService->dwThreadID,   WM_QUIT,   0,   0   )   ;   
			  break;   
		  case   SERVICE_CONTROL_PAUSE:   //Ҫ������ͣ
			  m_pService->status.dwCurrentState = SERVICE_PAUSE_PENDING;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  m_pService->PauseService();
			  m_pService->status.dwCurrentState = SERVICE_PAUSED;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  break;   
		  case   SERVICE_CONTROL_CONTINUE:   //Ҫ�������
			  m_pService->status.dwCurrentState = SERVICE_PAUSE_PENDING;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  m_pService->ResumeService();
			  m_pService->status.dwCurrentState = SERVICE_RUNNING;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  break;   
		  case   SERVICE_CONTROL_INTERROGATE:   //Ҫ�������ϱ�������״̬
			  break;   
		  case SERVICE_CONTROL_SHUTDOWN: //��Ҫ�ػ�
			  LogEvent(("��Ҫ�ػ�"));
			  log("��Ҫ�ػ�");
			  // 1.֪ͨ��صĳ�����йر�

			  // 2.ֹͣ����
			  m_pService->bStop = true;
			//  m_pService->StopService();
			  m_pService->status.dwCurrentState = SERVICE_STOPPED;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  // 3.�˳�
			  PostThreadMessage( m_pService->dwThreadID,   WM_QUIT,   0,   0   )   ; 
			  break;
		  default:   
			  break;   
	}   
}   



//��ӷ������̺߳����Ϳ��ƺ���
void WINAPI wx_service::ServiceMain(DWORD   dwArgc,   LPTSTR   *lpszArgv)
{
	log( "ServiceMain : ServeiceName: %s  args: %s ", szServiceName , lpszArgv);
	//ע��������
	hServiceStatus = RegisterServiceCtrlHandler( szServiceName, &ServiceHandle);
	if (hServiceStatus == NULL)
	{
		log( "ServiceHandle not installed" );
		LogEvent(("ServiceHandle not installed"));
		return;
	}
	log( "ServiceHandle installed & SetServiceStatus SERVICE_START_PENDING " );
	status.dwWaitHint = 10;
	status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;//���Ҫʹ�ã������㲻�ܿ���
	SetServiceStatus(hServiceStatus, &status);

	status.dwWin32ExitCode = S_OK;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	log( "SetServiceStatus SERVICE_RUNNING" );
	SetServiceStatus( hServiceStatus, &status);

	log( "RUNNING" );
	//���м��
	dwThreadID   =   GetCurrentThreadId()   ;   
	Run();
	log( "Run() returned" );
	 
	status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(hServiceStatus, &status);
	LogEvent(("Service stopped"));
	 

}

void log(LPCTSTR format, ...)
{
	char szError[512]={0};
	time_t t=0;  
	char *pTime = 0;

	// format msg 
	char buf[4096]={0}, *p=buf;
	va_list args;
	va_start(args, format);
	p += _vsnprintf(p, sizeof buf - 1, format, args);
	va_end(args);
	// while ( p > buf && isspace(p[-1]) )    *--p = '\0';
	*p++ = '\n';
	*p++ = '\0';


	// open  log file
	FILE *fp = fopen( logFilePath, "a+");
	if( fp == 0){
		LogEvent( "fopen Error : ",  GetLastErrorText(szError, 512) );
		return ;
	}

	time(&t);  
	pTime = asctime(localtime(&t));
	pTime[strlen(pTime)-1] = '\0';
	fprintf( fp, "[%s] %s ", pTime, buf);
	fclose(fp);

}

 /*********************************************************************   
  **   ��������Ϣ�ӽ�ϵͳEVENT��������;   
  *********************************************************************/   
void  LogEvent(LPCTSTR format, ...)
{
	TCHAR         lpszMsg[256];   
	HANDLE       hEventSource;   
	LPCTSTR     lpszStrings[2];   

	char buf[4096]={0}, *p=buf;
	va_list args;
	va_start(args, format);
	p += _vsnprintf(p, sizeof buf - 1, format, args);
	va_end(args);
	// while ( p > buf && isspace(p[-1]) )    *--p = '\0';
	*p = '\0';

	if(!m_pService->hServiceStatus){   
		printf("%s",   buf)   ;   
		return   ;   
	}   
	//   Use   event   logging   to   log   the   error.   

	hEventSource   =   RegisterEventSource( NULL ,   m_pService->szServiceName);   

	sprintf(lpszMsg,   TEXT("\n%s   ��ʾ��Ϣ:   %d"),   m_pService->szServiceName,   GetLastError());   
	lpszStrings[0]   =   buf;   
	lpszStrings[1]   =   lpszMsg;   

	if(hEventSource   !=   NULL){   

		ReportEvent(hEventSource, //   handle   of   event   source   
			EVENTLOG_ERROR_TYPE, //   event   type   
			0, //   event   category   
			0, //   event   ID   
			NULL, //   current   user's   SID   
			2, //   number   of   strings   in   lpszStrings   
			0, //   no   bytes   of   raw   data   
			lpszStrings, //   array   of   error   strings   
			buf); //   no   raw   data   
		(VOID)   DeregisterEventSource(hEventSource);   
	}   

}