
// KService.cpp
/**
* Auth: Karl
* Date: 2014/5/8
* LastUpdate:2014/5/8
*/

#include "stdafx.h"

#include "KService.h"


KService::KService()
{

}

KService::~KService()
{

}

long KService::SetService(Service sIn)
{
	this->service = sIn;
	return 0;
}

////ע�����
long KService::Install()
{
	long lRet = 0;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCM==NULL){
		return GetLastError();
	}
	SERVICE_DESCRIPTION sd; 
	//��������
	hService = ::CreateService(
		hSCM, 
		this->service.szServiceName,//��������
		this->service.szServiceDisplayName,//��ʾ����
		SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		service.szServicePath, //�������·��
		NULL, NULL, (""), NULL, NULL);
	if(hService){
		lRet = 0;
		sd.lpDescription = this->service.szServiceDescription;
		if( !ChangeServiceConfig2( 
			hService,                 // handle to service  
			SERVICE_CONFIG_DESCRIPTION, // change: description  
			&sd) )                      // new description  
		{ 
			//printf("ChangeServiceConfig2 failed\n"); 
			lRet = GetLastError();
		} 
	}else{
		lRet = GetLastError();
		CloseServiceHandle(hSCM);
		return lRet;
	}
	
end:
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	return lRet;

}

//�� [���������] �� [����] ���
//����ֵ:\
	#define ERROR	-1	//����\
	#define NONE	-2	//��\
//
long OpenServiceHandle(SC_HANDLE &hSCM, SC_HANDLE &hService, char* serviceName)
{
	//�� [���������] �� [����] ���
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCM==NULL){
		return GetLastError();
	}
	hService = ::OpenService(         //��ȡSERVICE���ƾ����API   
		hSCM,                         //SCM���������   
		serviceName,                 //SERVICE�ڲ����ƣ���������   
		SERVICE_ALL_ACCESS);         //�򿪵�Ȩ�ޣ�ɾ����Ҫȫ��Ȩ��
	if(hService==NULL){
		::CloseServiceHandle(hSCM);
		return GetLastError();
	}
	return 0;
}



//��ѯ�����Ƿ����
//����ֵ:\
	#define ERROR	-1	//����\
	#define NONE	0	//��\
	#define OK		1	//���ڣ�����
//
long KService::ServiceExist(char *serviceName)
{
	long lRet = OpenServiceHandle(hSCM, hService,serviceName);
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);
	return lRet;
}

//��ȡ������Ϣ
long KService::QueryService(char *serviceName, Service *pService)
{
	if(pService==0){
	//	assert(pService);
		return GetLastError();
	}
	long lRet = OpenServiceHandle(hSCM, hService, serviceName);
	if( lRet != 0)
	{
		return  lRet;
	}

	DWORD dwSize=1024;
	char* szInfo[1024]={0};

	sprintf(pService->szServiceName, "%s", serviceName);//��������
	if( !QueryServiceStatus(hService,   &pService->status)){//��ѯ����״̬
		lRet =  GetLastError();
		goto end;
	}

	dwSize = 1024;
	QueryServiceConfig2( hService, SERVICE_CONFIG_DESCRIPTION,(LPBYTE)szInfo,dwSize,&dwSize);
	if( ((LPSERVICE_DESCRIPTION)szInfo)->lpDescription!=0 )
		sprintf( pService->szServiceDescription, "%s", ((LPSERVICE_DESCRIPTION)szInfo)->lpDescription);//�õ�����������Ϣ
	
	dwSize = 1024;
	QueryServiceConfig( hService,(LPQUERY_SERVICE_CONFIG)szInfo, dwSize, &dwSize);
	sprintf(pService->szServiceDisplayName,  "%s", ((LPQUERY_SERVICE_CONFIG)szInfo)->lpDisplayName);//������ʾ����
	sprintf(pService->szServicePath,  "%s", ((LPQUERY_SERVICE_CONFIG)szInfo)->lpBinaryPathName);//�������·��
	pService->status.dwServiceType = ((LPQUERY_SERVICE_CONFIG)szInfo)->dwServiceType;//��������
	sprintf(pService->szUser,  "%s", ((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName);//���������û�

end:
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);

	return 0;
}

Service** KService::GetServices()
{
	listService.clear();//clear

	LPENUM_SERVICE_STATUS lst=NULL;//ע�����lst
	DWORD ret=0;
	DWORD size=0;
	
	SC_HANDLE sc = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	SC_HANDLE sh;
	char* szInfo[1024*8];
	DWORD dwSize=1024*8;
	CString str;
	//��һ�ε������õ���Ҫ�����ڴ���
	EnumServicesStatus(sc,SERVICE_WIN32,SERVICE_STATE_ALL,lst,size,&size,&ret,NULL);
	//������Ҫ���ڴ�
	lst=(LPENUM_SERVICE_STATUS)LocalAlloc(LPTR,size);
	EnumServicesStatus(sc,SERVICE_WIN32,SERVICE_STATE_ALL,lst,size,&size,&ret,NULL);
	//��ʼ��¼ö�ٳ��������Ϣ
	for(DWORD i=0;i<ret;i++){
		dwSize=1024*8;
		ZeroMemory(szInfo,dwSize);
		Service *info = new Service(); 
		sprintf( info->szServiceName, "%s", lst[i].lpServiceName);
		sprintf( info->szServiceDisplayName, "%s", lst[i].lpDisplayName);
		info->status.dwCurrentState = lst[i].ServiceStatus.dwCurrentState;
		sh=OpenService(sc, lst[i].lpServiceName,SERVICE_ALL_ACCESS);
		//�õ�����������Ϣ
		QueryServiceConfig2(sh,SERVICE_CONFIG_DESCRIPTION,(LPBYTE)szInfo,dwSize,&dwSize);
		sprintf( info->szServiceDescription, "%s", ((LPSERVICE_DESCRIPTION)szInfo)->lpDescription);
		//�õ�����������˻���
		ZeroMemory(szInfo,dwSize);
		dwSize=1024*8;
		QueryServiceConfig(sh,(LPQUERY_SERVICE_CONFIG)szInfo,dwSize,&dwSize);
		sprintf( info->szUser, "%s", ((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName);
		CloseServiceHandle(sh);
		//��ӵ���Ϣ������
	//	ItemAdd(&info);
	}
	CloseServiceHandle(sc);

	return NULL;
}


//ж�ط���
long KService::UnInstall()
{
	long lRet = OpenServiceHandle(hSCM, hService, service.szServiceName);
	if( lRet != 0)
	{
		return  lRet;
	}
	if( !QueryServiceStatus(hService,   &status)){
		lRet =  GetLastError();
		goto end;
	}
	if( (status.dwCurrentState == SERVICE_RUNNING) )
	{
		if(  ControlService(hService,   SERVICE_CONTROL_STOP, &status) ){
			//ֱ����SERVICE��STOP�������ܹ�ִ�е����˵��SERVICE������   
			//�Ǿ���Ҫֹͣ����ִ�к����ɾ��   
			Sleep(2000)   ;   //��3��ʹϵͳ��ʱ��ִ��STOP����  
			int times=0;
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
		}else{
			goto end;
		}
	}
	if(status.dwCurrentState   !=   SERVICE_STOPPED)  
	{          
		//���SERVICE����STOP�����û��STOPPED   
		lRet =  GetLastError();
		goto end;
	}else{
		//ɾ��ָ��������   
		if( ::DeleteService(hService)){
			lRet = 0;
		}else{
			lRet = GetLastError();
			goto end;
		}
	} 

end:
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);

	return lRet;
}

// ��������
long KService::Start()
{
	long lRet = 0;
	
	if( OpenServiceHandle(hSCM, hService, service.szServiceName) != 0 ){
		return  GetLastError();
	}
	if( !QueryServiceStatus(hService,   &status)){
		goto end;
	}
	int  times = 0;
	// ��������
	if( status.dwCurrentState == SERVICE_STOPPED)
	{
		if( ::StartService( hService, NULL, NULL) == FALSE)
		{
			lRet =  GetLastError();
			goto end;
		}
		// �ȴ���������
		while( ::QueryServiceStatus( hService, &status) == TRUE)
		{
			times ++;
			::Sleep( status.dwWaitHint);
			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				lRet = 0;//���������ɹ�
				goto end;
			}
			if(times>10){
				lRet =  GetLastError();
				break;
			}
		}
	}

end:
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);
	return lRet;
}

//ֹͣ����
long KService::Stop()
{
	long lRet = 0;

	if( OpenServiceHandle(hSCM, hService, service.szServiceName) != 0 ){
		return  GetLastError();
	}
	if( !QueryServiceStatus(hService,   &status)){
		lRet =  GetLastError();
		goto end;
	}

	if( ControlService(hService,   SERVICE_CONTROL_STOP, &status) ){
		//ֱ����SERVICE��STOP�������ܹ�ִ�е����˵��SERVICE������   
		//�Ǿ���Ҫֹͣ����ִ�к����ɾ��   
		Sleep(2000)   ;   //��3��ʹϵͳ��ʱ��ִ��STOP����  
		int times=0;
		while( QueryServiceStatus(hService,   &status)   )   {
			times ++;
			if(status.dwCurrentState   ==   SERVICE_STOP_PENDING)   
			{         //���SERVICE������ִ��(PENDING)ֹͣ����   
				Sleep(500)   ;         //�Ǿ͵�1���Ӻ��ټ��SERVICE�Ƿ�ֹͣOK  
			}else {
				break ; //STOP�������ϣ�����ѭ��   
			}
			if(times>10){
				break;// time out 
			}
		}//ѭ�����SERVICE״̬����   
	}else{
		lRet =  GetLastError();
		goto end;
	}
	if(status.dwCurrentState   !=   SERVICE_STOPPED)  
	{          //���SERVICE����STOP�����û��STOPPED   
		lRet =  GetLastError();
		goto end;
	}else{
		lRet = 0;
	} 

end:
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);
	return lRet;
}