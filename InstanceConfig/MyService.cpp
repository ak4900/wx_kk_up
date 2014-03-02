
 
#include "stdio.h"
#include "time.h"
#include "MyService.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

//log
char logFilePath[512]={0};
void log(char* format, ...);

void LogEvent(char *p)
{
	log("%s", p);
}




#define SLEEP_TIME 1000

MyService::MyService()
{
	memset(szServiceName, 0 , 256);
	strcpy_s(szServiceName, strlen("TF_wx_kk_Service"), "TF_wx_kk_Service");
}
MyService::~MyService()
{

}

void MyService::Init()
{

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
	snprintf( logFilePath, p-pre , "%s\\wx_kk_service.log", pre);
	strncat(  logFilePath, ("\\wx_kk_service.log"), strlen("\\wx_kk_service.log"));

	m_lpServicePathName[0] = '\"';
	m_lpServicePathName[strlen(m_lpServicePathName)] = '\"';

	log("Init");
}

BOOL MyService::Install()
{
	char szError[512]={0};
	bool bRet = false;
	//�����г���Ҫ�����������������Ŀ����ڴ������ҡ�
	//�򿪷�����ƹ�����

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	long ret = GetLastError();
	if(hSCM==NULL){
		log( "%s", GetLastErrorText(szError, 512) );
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
BOOL MyService::OpenService()
{
	char szError[512]={0};
	bool bRet = false;
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
					if(::OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS))
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



//ֹͣ����
BOOL MyService::StopService()
{
	char szError[512]={0};
	bool bRet = false;
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



BOOL MyService::Uninstall()
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




void log(char* format, ...)
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
		log( "fopen Error : %s",  GetLastErrorText(szError, 512) );
		return ;
	}

	time(&t);  
	pTime = asctime(localtime(&t));
	pTime[strlen(pTime)-1] = '\0';
	fprintf( fp, "[%s] %s ", pTime, buf);
	fclose(fp);

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
