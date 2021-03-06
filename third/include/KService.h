
// KService.h
/**
* Auth: Karl
* Date: 2014/5/8
* LastUpdate:2014/5/8
*/

#pragma once

#ifndef _KSERVICE_HEADER_
#define _KSERVICE_HEADER_

#include "winsvc.h"
#include <list>
using namespace std;

#define MAX_NAME 64
#define MAX_KPATH  512
#define MAX_SERVICE 256

#define ERROR	-1	//错误
#define NONE	-2	//空

struct Service
{
	char szServiceName[MAX_NAME];//服务名称
	char szServiceDisplayName[MAX_NAME];//显示名称
	char szServicePath[MAX_KPATH];//服务程序路径
	char szServiceDescription[MAX_KPATH];//服务描述
	char szStatus[MAX_NAME];
	char szUser[MAX_NAME];
	SERVICE_STATUS status;

	Service(){
		int len = sizeof(Service);
		memset(this, 0, len);
	}
};
typedef  struct Service Service;


// Start Type
static char StartType[5][32] = {"驱动程序","随系统启动","自动启动","服务控制管理器(SCM)启动","禁用"};

/*
#define SERVICE_BOOT_START             0x00000000
#define SERVICE_SYSTEM_START           0x00000001
#define SERVICE_AUTO_START             0x00000002
#define SERVICE_DEMAND_START           0x00000003
#define SERVICE_DISABLED               0x00000004
*/

class KService
{

public:
	KService();
	~KService();

private:
	
	SC_HANDLE hSCM;
	SC_HANDLE hService;
	SERVICE_STATUS status;

	Service service;
	Service services[MAX_SERVICE];
	list<Service> listService;

public:
	long SetService(Service sIn);//设置service
	long Install();//注册服务
	long UnInstall();//卸载服务
	long Start();//开启服务
	long Stop();//停止服务
	long ServiceExist(char *serviceName);//查询服务是否存在
	long QueryService(char *serviceName, Service *pService);//获取服务信息
	Service** GetServices();

};

#endif


/*
//枚举服务
Service** KService::GetServices()
{
	DeletItemAll();
	LPENUM_SERVICE_STATUS lst=NULL;//注意这个lst
	DWORD ret=0;
	DWORD size=0;
	ServiceInfo info;
	SC_HANDLE sc=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	SC_HANDLE sh;
	char* szInfo[1024*8];
	DWORD dwSize=1024*8;
	CString str;
	//第一次调用来得到需要多大的内存区
	EnumServicesStatus(sc,SERVICE_WIN32,SERVICE_STATE_ALL,lst,size,&size,&ret,NULL);
	//申请需要的内存
	lst=(LPENUM_SERVICE_STATUS)LocalAlloc(LPTR,size);
	EnumServicesStatus(sc,SERVICE_WIN32,SERVICE_STATE_ALL,lst,size,&size,&ret,NULL);
	//开始记录枚举出服务的信息
	for(DWORD i=0;i<ret;i++){
		 dwSize=1024*8;
		 ZeroMemory(szInfo,dwSize);
		 info.Name.Format("%s",lst[i].lpDisplayName);//看到了吧，lst[i]表示第i个服务器
		 info.serviceNmae.Format("%s",lst[i].lpServiceName);
		 info.State.Format("%d",lst[i].ServiceStatus.dwCurrentState);
		 sh=OpenService(sc,lst[i].lpServiceName,SERVICE_ALL_ACCESS);
		 //得到服务描述信息
		 QueryServiceConfig2(sh,SERVICE_CONFIG_DESCRIPTION,(LPBYTE)szInfo,dwSize,&dwSize);
		 info.Desc.Format("%s",((LPSERVICE_DESCRIPTION)szInfo)->lpDescription);
		 //得到服务的启动账户名
		 ZeroMemory(szInfo,dwSize);
		 dwSize=1024*8;
		 QueryServiceConfig(sh,(LPQUERY_SERVICE_CONFIG)szInfo,dwSize,&dwSize);
		 info.LoginUser.Format("%s",((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName);
		 CloseServiceHandle(sh);
		 //添加到信息队列中
		 ItemAdd(&info);
	}
	CloseServiceHandle(sc);

}

*/