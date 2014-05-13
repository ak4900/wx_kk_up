
// wx_kk_upDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "wx_kk_up.h"
#include "wx_kk_upDlg.h"

#include "stdio.h"
#include  <io.h>
#include "time.h"

//#include "ShareModule.h"

#include "minIni.h"

#include "wxCore.h"

#include "ProcessState.h"
#include "HttpClient.h"
#include "TCode.h"



#if _MSC_VER
#define snprintf _snprintf
#endif
 

//����ͼƬ��׺��־(����ͼƬ)
#define PLATE_END  "_plate.jpg"

// === static var =========================================================

SystemConfig	Cwx_kk_upDlg::systemConfig = {0};//ϵͳ��������
KKConfig		Cwx_kk_upDlg::kkConfig = {0};//��������

// ==================================================================

TCHAR szTemp[TF_MAX_PATH_LEN]={0};//temp error info for zlog|tempvar

int	m_ExitCode = 0;//for thread if exit thread
bool threadRuning = false;

#define MAX_ERROR  1353
char LastError [MAX_ERROR][128] = {0};

char LastErrorInfo[512]={0};
char* GetLastErrorInfo()
{
	LPVOID lpMsgBuf;
	char *p = LastErrorInfo;
	long lError = GetLastError();
	if( lError < MAX_ERROR)
		sprintf( LastErrorInfo, "(%d, 0x%x)[%s]", lError, lError, LastError[lError]);
	else{
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			lError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
		sprintf( LastErrorInfo, "(%d, 0x%x)[%s]", lError, lError,  lpMsgBuf);
	}
	return p;
}



//===============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStaticHttpTeleframeCn();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::OnStnClickedStaticHttpTeleframeCn()
{
	ShellExecute(NULL, NULL, _T("http://teleframe.cn"), NULL, NULL, SW_SHOWNORMAL);
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_STN_CLICKED(IDC_STATIC_HTTP_TELEFRAME_CN, &CAboutDlg::OnStnClickedStaticHttpTeleframeCn)
END_MESSAGE_MAP()


// Cwx_kk_upDlg �Ի���




Cwx_kk_upDlg::Cwx_kk_upDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cwx_kk_upDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}
Cwx_kk_upDlg::~Cwx_kk_upDlg()
{
 
}

void Cwx_kk_upDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HTAGENTOBJ1, m_agent);
	DDX_Control(pDX, IDC_LIST1, m_LogList);
}

BEGIN_MESSAGE_MAP(Cwx_kk_upDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ABOUTBOX, &Cwx_kk_upDlg::OnAboutbox)
	ON_COMMAND(ID_SYSTEM_CONFIG_DLG, &Cwx_kk_upDlg::OnSystemConfigDlg)
	ON_COMMAND(ID_KK_CONFIG_DLG, &Cwx_kk_upDlg::OnKkConfigDlg)
	ON_COMMAND(ID_START_WATCH, &Cwx_kk_upDlg::OnStartWatch)
	ON_MESSAGE((WM_USER+40), OnLogMessage)
	ON_COMMAND(ID_EXIT, &Cwx_kk_upDlg::OnExit)
	ON_COMMAND(ID_RUN_ON_BACKGROUND, &Cwx_kk_upDlg::OnRunOnBackground)
	ON_COMMAND(ID_CHECK_UPDATE, &Cwx_kk_upDlg::OnCheckUpdate)
END_MESSAGE_MAP()


// Cwx_kk_upDlg ��Ϣ�������

BOOL Cwx_kk_upDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	dzlog_set_wnd_handle(this->m_hWnd, ZLOG_LEVEL_NOTICE  | ZLOG_LEVEL_ERROR | ZLOG_LEVEL_FATAL );

	Sleep(100);

	Init();

	m_agent.ShowWindow(SW_HIDE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}



void Cwx_kk_upDlg::OnAboutbox()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void Cwx_kk_upDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	dzlog_debug("UINT_ID = %d", nID);
	if ((nID & 0xFFF0) == IDM_ABOUTBOX  || nID==ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cwx_kk_upDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cwx_kk_upDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*================================================================================================= 
* Init()��ʼ������
*
* ��Ҫ��������:
*
*  1.ReadConfig();
*  2.InitTrans();��ʼ������/������Ϣע��
*  3.QuerySyncTime();ʱ��ͬ�� ��ȡ������ʱ�䣬Ȼ��ͬ��������ͷ
*  4.QueryLimitSpeed();����ͬ�� 
*  5.CamerSDK:SetupDeviceTime ʱ��У׼
*  6.SaveConfig();����������Ϣ
*
==================================================================================================== */

void Cwx_kk_upDlg::Init()
{
	CString g_szOcxPath = theApp.m_pszHelpFilePath;
	CString g_szOcxName = theApp.m_pszExeName;

	g_szOcxName += ".HLP";
	int nTmp = g_szOcxName.GetLength();
	nTmp = g_szOcxPath.GetLength() - nTmp;
	g_szOcxPath = g_szOcxPath.Left(nTmp-1);

	m_appPath = g_szOcxPath;//��ȡ����������·��

	ReadConfig();//��ȡ������Ϣ
	if(systemConfig.autorun==1){//�������Զ������ϴ�
		AgentInit();//��ʼ��
		OnStartWatch();//��ʼ����
	}

	

}

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <dos.h>
////ʱ��ͬ�������ñ���ʱ��
long SetupDeviceTime(const char *csSyncTime)
{
	if(csSyncTime==0)
		return 0;
	if(strlen(csSyncTime)<18)
		return 0;

	char temp[32]={0};
	sprintf(temp, "%s", csSyncTime);
	int csDate[4] = {0};
	int csTime[4] = {0};
	char *pDate = temp;
	char *pTime = 0;
	char *p = 0;

	struct tm tmTime = {0};
	time_t t = 0;
	char errorInfo[256] = {0};

	try{
		pTime = strstr(temp, " ");
		*pTime = '\0';
		pTime ++;

		p = pTime;
		p = strrchr(pTime, ':');
		tmTime.tm_sec = atoi(p+1);//�� 
		*p = '\0';
		p = strrchr(pTime, ':');
		tmTime.tm_min = atoi(p+1);//��
		*p = '\0';
		tmTime.tm_hour = atoi(pTime);//ʱ 

		p = pDate;
		p = strrchr(pDate, '-');
		tmTime.tm_mday = atoi(p+1);//��
		*p = '\0';
		p = strrchr(pDate, '-');
		tmTime.tm_mon = atoi(p+1) - 1;//��
		*p = '\0';
		tmTime.tm_year = atoi(pDate) - 1900;//��


		t=mktime(&tmTime);
		//	stime(&t);// linux
		
		SYSTEMTIME time1;
		GetLocalTime(&time1);
		time1.wYear = tmTime.tm_year + 1900;
		time1.wMonth = tmTime.tm_mon + 1;
		time1.wDay = tmTime.tm_mday;
		time1.wHour = tmTime.tm_hour;
		time1.wMinute = tmTime.tm_min;
		time1.wSecond = tmTime.tm_sec;

		if( SetLocalTime(&time1) )//windows
			sprintf(errorInfo, "ʱ�����óɹ�:%s",ctime(&t));
		else 
			sprintf(errorInfo, "ʱ������ʧ��:%s  ERROR: %s ", csSyncTime, GetLastErrorInfo());
		dzlog_notice(errorInfo);

	}catch(...){
		sprintf(errorInfo,"ʱ������ʧ�ܣ�ʱ���ʽ�쳣:%s  ERROR: %s ", csSyncTime, GetLastErrorInfo());
		dzlog_notice(errorInfo);
		return -1;
	}

	return t;
}

/*================================================================================================= 
* AgentInit()��ʼ��Agent����
* 1.InitTrans();��ʼ������/������Ϣע��
* 2.QuerySyncTime();ʱ��ͬ�� ��ȡ������ʱ�䣬Ȼ��ͬ��������ͷ
* 3.QueryLimitSpeed();����ͬ�� 
* 4.CamerSDK:SetupDeviceTime ʱ��У׼
* 5.SaveConfig();����������Ϣ
*
==================================================================================================== */
bool Cwx_kk_upDlg::AgentInit()
{
	TCHAR *m_kkbh = ( kkConfig.id);//���ڱ��
	TCHAR * m_fxlx = (kkConfig.direction);//��������
	LONG m_cdh = kkConfig.cdid;//������
    TCHAR * info = ("<info><jkid>62C01</jkid><jkxlh>791F09090306170309158E9882DB8B8E8FEEF681FF8B9AEB92CFD0A1A1C5A5D3CCA8</jkxlh></info>");//������Ϣ
	TCHAR * m_hostport = ( systemConfig.host );//��������ַ
	LONG ret = 1 ;
	if( strcmp(systemConfig.runMode, "wx") ==0)
		ret = m_agent.InitTrans(m_kkbh,m_fxlx,m_cdh, info ,m_hostport);//ע�ᳵ��

	if(ret<=0){
		sprintf(errorInfo,  "���ӷ�����ʧ��[%d]",ret);
		dzlog_error(errorInfo);
		MessageBox(errorInfo);
	}

	//ʱ��ͬ��
	CString csSyncTime = "2014-03-02 10:25:50";
	dzlog_notice("ʱ��ͬ��");
	if( strcmp(systemConfig.runMode, "wx") ==0){
		csSyncTime = m_agent.QuerySyncTime();//ʱ��ͬ��
		csSyncTime = csSyncTime.Trim();
		if( !csSyncTime.IsEmpty() )
			SetupDeviceTime(csSyncTime.GetBuffer(csSyncTime.GetLength())); //ʱ��ͬ�������ñ���ʱ��
	}
	
	//����ͬ��
	char *cllx = "1";//��������	0-�󳵣�1-С����2-��������
	long lLimitSpeed = 40; 
	if( strcmp(systemConfig.runMode, "wx") ==0){
		lLimitSpeed = m_agent.QueryLimitSpeed(m_kkbh,m_fxlx,m_cdh, cllx);////����ͬ��
	}
	return true;
}

#define R_OK 4 /* Test for read permission. */
#define W_OK 2 /* Test for write permission. */
#define X_OK 1 /* Test for execute permission. */
#define F_OK 0 /* Test for existence. */

//��ȡ������Ϣ
bool Cwx_kk_upDlg::ReadConfig()
{
	dzlog_notice("��ȡ����");
	char  cIniFullPath[TF_MAX_PATH_LEN] = {0};
	sprintf(cIniFullPath,"%s\\%s.ini", m_appPath,theApp.m_pszProfileName);

	//	R_OK ֻ�ж��Ƿ��ж�Ȩ�� \
		W_OK ֻ�ж��Ƿ���дȨ�� \
		X_OK �ж��Ƿ���ִ��Ȩ�� \
		F_OK ֻ�ж��Ƿ����
	if(_access(cIniFullPath, R_OK) == -1)
	{
		sprintf(errorInfo, "��ȡ�����ļ�ʧ��[%s]", cIniFullPath);
		dzlog_error(errorInfo);
		MessageBox(errorInfo);
		return false;
	}
	
	// get system config from ini
	ini_gets("system", "updateurl", "http://teleframe.xicp.net/update/?update=true&app_name=wx_kk_up", systemConfig.updateurl, TF_MAX_PATH_LEN,  cIniFullPath);
	systemConfig.autorun = ini_getl("system", "autorun",	 1,  cIniFullPath);
	ini_gets("system", "host", "10.2.44.68:9080", systemConfig.host, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("system", "rumMode", "", systemConfig.runMode, TF_MAX_PATH_LEN,  cIniFullPath);

	// get kk config from ini
	ini_gets("kk", "kk.id", "", kkConfig.id, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("kk", "kk.direction", "", kkConfig.direction, TF_MAX_PATH_LEN,  cIniFullPath);
	kkConfig.cdid = ini_getl("kk", "kk.cd.id",  1,  cIniFullPath);
	kkConfig.cdSpeedLimit = ini_getl("kk", "kk.cd.speedLimit",  40,  cIniFullPath);
	kkConfig.cdSpeedLimitDefalut = ini_getl("kk", "kk.cd.speedLimitDefault",  40,  cIniFullPath);
	ini_gets("kk", "kk.ftpPath", "", kkConfig.ftpPath, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("kk", "kk.httpPath", "", kkConfig.httpPath, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("kk", "kk.http", "", kkConfig.http, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("kk", "kk.fileExt", "", kkConfig.fileExt, TF_MAX_PATH_LEN,  cIniFullPath);

	FileUtil::CreateFolders(kkConfig.ftpPath);
	FileUtil::CreateFolders(kkConfig.httpPath);

	//read lasterror
	sprintf(cIniFullPath,"%s\\GetLastError.dat", m_appPath );
	long ErrorSum = ini_getl("error", "sum", 0, cIniFullPath);

	ErrorSum = ErrorSum > MAX_ERROR ? MAX_ERROR : ErrorSum ;

	for(int i=1; i<=ErrorSum; i++)
	{
		sprintf(errorInfo,"%d",i);
		ini_gets("error", errorInfo, "NON", LastError[i], 128,  cIniFullPath);
	}

	dzlog_notice("��ȡ�����ļ����[%s]",cIniFullPath);

	return true;
}

//����������Ϣ
bool Cwx_kk_upDlg::SaveConfig()
{
	dzlog_notice("��������");

	char  cIniFullPath[TF_MAX_PATH_LEN] = {0};
	sprintf(cIniFullPath,"%s\\%s.ini", m_appPath,theApp.m_pszProfileName);

	//	R_OK ֻ�ж��Ƿ��ж�Ȩ�� \
		W_OK ֻ�ж��Ƿ���дȨ�� \
		X_OK �ж��Ƿ���ִ��Ȩ�� \
		F_OK ֻ�ж��Ƿ����
	if(_access(cIniFullPath, W_OK) == -1)
	{
		sprintf(errorInfo, "д�������ļ�ʧ��,[%s]", cIniFullPath);
		dzlog_error(errorInfo);
		MessageBox(errorInfo);
		return false;
	}

	// put system config to ini
	ini_puts("system", "updateurl",  systemConfig.updateurl,  cIniFullPath);
	ini_putl("system", "autorun",	 systemConfig.autorun,  cIniFullPath);
	ini_puts("system", "host",		 systemConfig.host,  cIniFullPath);
	ini_puts("system", "rumMode",	 systemConfig.runMode,  cIniFullPath);

	// get kk config from ini
	ini_puts("kk", "kk.id",   kkConfig.id,  cIniFullPath);
	ini_puts("kk", "kk.direction",  kkConfig.direction,  cIniFullPath);
	ini_putl("kk", "kk.cd.id",  kkConfig.cdid,  cIniFullPath);
	ini_putl("kk", "kk.cd.speedLimit",  kkConfig.cdSpeedLimit,  cIniFullPath);
	ini_putl("kk", "kk.cd.speedLimitDefault",  kkConfig.cdSpeedLimitDefalut,  cIniFullPath);
	ini_puts("kk", "kk.ftpPath",   kkConfig.ftpPath,  cIniFullPath);
	ini_puts("kk", "kk.httpPath",   kkConfig.httpPath,  cIniFullPath);
	ini_puts("kk", "kk.http",   kkConfig.http,  cIniFullPath);
	ini_puts("kk", "kk.fileExt",   kkConfig.fileExt,  cIniFullPath);

	dzlog_notice("���������ļ����[%s]",cIniFullPath);

	return true;
}



//ϵͳ����
void Cwx_kk_upDlg::OnSystemConfigDlg()
{
	dzlog_notice("ϵͳ����");
	systemConfigDlg.setConfig(systemConfig);
	if(systemConfigDlg.DoModal()==IDOK){
		systemConfig = systemConfigDlg.systemConfig;
		dzlog_notice("�޸�ϵͳ����");
		this->SaveConfig();
	}
}

//��������
void Cwx_kk_upDlg::OnKkConfigDlg()
{
	dzlog_notice("��������");
	kkConfigDlg.pParentDlg = this;
	kkConfigDlg.setConfig(kkConfig);
	if(kkConfigDlg.DoModal()==IDOK){
		kkConfig = kkConfigDlg.kkConfig;	
		dzlog_notice("�޸Ŀ�������");
		this->SaveConfig();
	}
}



//�ٿ�ʼ���Ŀ¼
//  ����ص����ļ�����list
void* ThreadWatcher(void* pParam)
{
	dzlog_notice("[�߳̿���]��ʼ���Ŀ¼ ThreadWatcher  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	
	char notify[4096]={0}; 
	DWORD cbBytes = 0; 
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	FILE_NOTIFY_INFORMATION *pNotify=(FILE_NOTIFY_INFORMATION *)notify;

	// **** ��Ҫ ****
	// ***  ��CreateFileʱָ��FILE_FLAG_OVERLAPPED��־	\
			ReadDirectoryChangesWʱʹ��lpOverlapped����
	OVERLAPPED ov;//֪ͨ�߳��˳� ReadDirectoryChangesW
	memset(&ov, 0, sizeof(ov));
    ov.hEvent = CreateEvent(NULL, false, NULL, NULL);

	//	GetCurrentDirectory(MAX_PATH,path.GetBuffer(MAX_PATH+1));
	
	CString str;

	WCHAR  wcFileName[TF_MAX_PATH_LEN]={0};
	DWORD dwFileNameLength ;
	bool bDeleteFileName = true;
	char psTemp[TF_MAX_PATH_LEN]={0};
	long lRet;

//	SetEvent(eventStarted);//release signal
	while (TRUE) 
	{ 
		if(m_ExitCode==1)
			goto end;

		char *cFileName = new char[TF_MAX_PATH_LEN];
		memset(cFileName, 0 , TF_MAX_PATH_LEN);
		memset(wcFileName, 0 , TF_MAX_PATH_LEN);

		bDeleteFileName = true;

	//for (i=0;i<numDirs;i++)
		lRet = ReadDirectoryChangesW( dlg->kkConfig.hDir, &notify, sizeof(notify),
			true, FILE_NOTIFY_CHANGE_LAST_WRITE, 
			&cbBytes,0, NULL);

		if(lRet)
		{ 
			memcpy( wcFileName, pNotify->FileName, pNotify->FileNameLength );
			WideCharToMultiByte( CP_ACP, 0, wcFileName, -1, cFileName, TF_MAX_PATH_LEN, NULL, NULL );

			sprintf( psTemp, "%s/%s", dlg->kkConfig.ftpPath, cFileName );
			if(FileUtil::FindFirstFileExists( psTemp, FILE_ATTRIBUTE_DIRECTORY))
				continue;

			switch(pNotify->Action) 
			{ 
			case FILE_ACTION_ADDED: 
				str.Format("Directory/File added - %s",cFileName);
				break; 
			case FILE_ACTION_REMOVED:
				str.Format("Directory/File removed - %s",cFileName);
				break; 
			case FILE_ACTION_MODIFIED: 
			//	WaitForSingleObject(hMutex,-1);//waiting
				dlg->m_images.push( cFileName );
			//	ReleaseMutex(hMutex);//release 
				str.Format("Directory/File modified - %s",cFileName);
				bDeleteFileName = false;
				break; 
			case FILE_ACTION_RENAMED_OLD_NAME: 
				str.Format("Directory/File rename - %s",cFileName);
				break; 
			case FILE_ACTION_RENAMED_NEW_NAME: 
				str.Format("Directory/File new name - %s",cFileName);
				break; 
			default: 
				break; 
			} 
		}
		dzlog_notice(str);
		if(bDeleteFileName)
			delete cFileName;
	}

end:
	sprintf(errorInfo, "[�߳��˳�]��ʼ���Ŀ¼ ThreadWatcher  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	CloseHandle(dlg->kkConfig.hDir);

	pthread_exit(errorInfo);
	return 0;
}

//ͨ��ͼƬ���ƻ�ȡ���Ƶ���Ϣ
long  ParseVehicleFromPicture(KKConfig kkConfig, char *imagePath, VehicleInfo &vehicleInfo)
{
	//	ͼƬ������Ϊ�� ʱ��_����_��������_������ɫ_�����ٶ�.bmp
	//	��: 2014-05-04_16.35.48_��N56Y22_02_2_0.bmp"   "2014-05-04_16.35.48_��N56Y22_02_2_0_moreInfo.bmp"

	//����ͼƬ���ƻ�ȡͼƬ��ʱ����Ϣ�� FORMAT  ����ʱ��_����_������ɫ_����_Υ������ eg: 2014-1-4_15.02.18_location.jpg
	char fileName[256]={0};
	long lTime=0;
	sprintf( fileName, "%s", (strrchr(imagePath,'\\')+1) );
	//fileName	0x0aa8f6f8 "20140504162515_��N56Y22_02_2_0.bmp"	char [256]
	char *p = strstr(fileName, "_");
	char *pre=0;
	char DateTime[32]={0};
	char *pDateTime = DateTime;
	
	try{
		// Date
		memcpy( pDateTime, fileName, p-fileName);
		pDateTime += (p-fileName);
		*pDateTime = ' ';
		pDateTime ++;

		//Time
		pre = p+1;
		p = strstr(pre, "_");
		memcpy( pDateTime, pre, p-pre);
		pre = pDateTime;
		while( *pre!='\0'){
			if( *pre == '.')
				*pre = ':';
			pre++;
		}
	}catch(...){
		dzlog_error("����[����ʱ��ʧ��:%s]", fileName);
	}

	//plate
	char plate[32]={0};
	try{
		pre = p+1;
		p = strstr(pre, "_");
		memcpy(plate, pre, p-pre);
	}catch(...){
		dzlog_error("����[����ʧ��:%s]", fileName);
	}
	
	//hpzl ��������
	char hpzl[8]={0};
	try{
		pre = p+1;
		p = strstr(pre, "_");
		memcpy(hpzl, pre, p-pre);
	}catch(...){
		dzlog_error("����[��������ʧ��:%s]", fileName);
	}

	//������ɫ
	char hpys[8]={0};
	try{
		pre = p+1;
		p = strstr(pre, "_");
		memcpy(hpys, pre, p-pre);
	}catch(...){
		dzlog_error("����[������ɫʧ��:%s]", fileName);
	}
	// ==================================================

//	VehicleInfo *vehicleInfo = new VehicleInfo();
	vehicleInfo.cdh = 1;//������
	sprintf(vehicleInfo.kkbh, kkConfig.id);//���ڱ��
	sprintf(vehicleInfo.fxlx, kkConfig.direction);//��������
	sprintf(vehicleInfo.hphm, plate ); //���� ���ơ�δʶ���޷�ʶ����ð�ǡ�-����ʾ�������޺���Ҫע��hpzl��41
	sprintf(vehicleInfo.hpzl, hpzl );//�������� �ο�GA24.7����01-����������02-С��������25-ũ�����ƣ�41-�޺��ƣ�42-�ٺ��ƣ�99-�������ƣ�������Ϊ�գ�
	sprintf(vehicleInfo.hpys, hpys );//������ɫ	0-��ɫ��1-��ɫ��2-��ɫ��3-��ɫ��4-��ɫ��9-������ɫ������Ϊ��

	sprintf(vehicleInfo.gcsj, DateTime ); //����ʱ��,e.g: "2003-09-11 11:07:23"
	vehicleInfo.clsd = 0;//�����ٶ� �3λ����λ������/Сʱ
	vehicleInfo.clxs = kkConfig.cdSpeedLimit;//�������� �3λ����λ������/Сʱ
	sprintf(vehicleInfo.wfdm, "1"); //Υ������  Υ����Ϊ���� �ο�GA408.1
	vehicleInfo.cwkc = 0;//�������� �5λ��������Ϊ��λ
	
	sprintf(vehicleInfo.cllx, "K33");//�������� �ο�GA24.4��K11-������ͨ�ͳ���K21-������ͨ�ͳ���K31-С����ͨ�ͳ���K33-�γ���H11-������ͨ�ͳ���H21-������ͨ�ͳ���M22-������Ħ�г���
	sprintf(vehicleInfo.fzhphm, "-");//�������ƺ���	���ơ�δʶ���޷�ʶ����ð�ǡ�-����ʾ�������޺���Ҫע��hpzl��41
	
	sprintf(vehicleInfo.csys, "-");//������ɫ
	sprintf(vehicleInfo.tplj,"%s", kkConfig.http);//ͨ��ͼƬ·��  �̶�����
	sprintf(vehicleInfo.tp1, "%s",imagePath);//ͨ��ͼƬ1  �仯�Ĳ���
	sprintf(vehicleInfo.tztp, "%s_plate.jpg",imagePath);//ͨ��ͼƬ1  �仯�Ĳ���

	return 1;
}

//�ڴ����ص�������
//  ��ȡ���Ƶ���Ϣ��������ݿ����Ƿ���ڸ�ͼƬ�������������������һ����������������ͼƬ��ftp��http,д�����ݿ⣬ɾ��ftp�µ��ļ�
void* ThreadProcessWatchedFiles(void* pParam)
{
	dzlog_notice("[�߳̿���]�����ص������� ThreadProcessWatchedFiles  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	char *imagePath = 0;
	char plateImagePath[512]={0};
	char ftpFilePath[512]={0};
	char ftpPlateFilePath[512]={0};
	char httpFilePath[512]={0};
	char httpPlateFilePath[512]={0};
	bool bExist = false;
	char temp[512]={0};
	long lRet=0;
	char errorValue[512]={0};

	VehicleInfo vehicleInfo={0};
	char *ptr;

	TFDB db;

	while(true){

		if(m_ExitCode==1)
			goto end;
		
		if(dlg->m_images.size()<1){
			dlg->GetDlgItem( IDC_STATUS )->SetWindowText("���еȴ�");
			Sleep(10);
			continue;
		}

		sprintf(temp, "���ڴ����ļ����У�ʣ���ļ�����: %d ", dlg->m_images.size() );
		dlg->GetDlgItem( IDC_STATUS )->SetWindowText(temp);

		if(dlg->m_images.empty())
			continue;

		imagePath = dlg->m_images.front();
		dlg->m_images.pop();

		if(imagePath==0)
			continue;

		ptr = strrstr(imagePath, PLATE_END);
		if(ptr>0){
			try{
				*ptr = '\0';
			//	delete 	imagePath;
			}catch(...){	}
		//	continue;
		}
		sprintf( plateImagePath, "%s"PLATE_END, imagePath);//�������·��

		sprintf(ftpFilePath, "%s\\%s", dlg->kkConfig.ftpPath, imagePath);// ������дͼFTP
		sprintf(httpFilePath, "%s\\%s", dlg->kkConfig.httpPath, imagePath);// ������дͼHTTP

		sprintf(ftpPlateFilePath,	"%s\\%s"PLATE_END, dlg->kkConfig.ftpPath, imagePath);// ����ͼƬFTP
		sprintf(httpPlateFilePath,  "%s\\%s"PLATE_END, dlg->kkConfig.httpPath, imagePath);//����ͼƬHTTP

		//check is file 
		if( FileUtil::FindFirstFileExists( ftpFilePath, FALSE) ||
			FileUtil::FindFirstFileExists( ftpPlateFilePath, FALSE) )
		{
			//check can access
			if( ( _access(ftpFilePath, R_OK) == 0 ) ||
				( _access(ftpPlateFilePath, R_OK) == 0 ) )
			{
				lRet = ParseVehicleFromPicture(dlg->kkConfig, imagePath, vehicleInfo);//ͨ��ͼƬ���ƻ�ȡ���Ƶ���Ϣ
				if(lRet != 1)
					continue;

				bExist = db.CheckImageExist(imagePath, "tp1");//������ͼƬ�Ƿ��Ѿ�������,��������д��
				if( ! bExist){
					lRet = db.Add(&vehicleInfo);//�����ڣ������������ݿ�
				}
				//����Ƿ���ڼ�¼�����ݳ���ͼƬ��
				bExist = db.CheckImageExist(imagePath, "tp1");//��������Ҫ���ȷ���ɹ�д�����ݿ�
				if(bExist){
CopyFile:			// ������дͼ
					lRet = FileUtil::CopyFileEx(ftpFilePath, httpFilePath, true);
					if(lRet==true){
						dzlog_error("Copyed file from [%s] to [%s]", ftpFilePath, httpFilePath);
						DeleteFile(ftpFilePath);//������ɺ�ɾ��FTP�µ��ļ�
					}else{
						dzlog_error("Copy file failed [GetLastError %s] from [%s] to [%s]", GetLastErrorInfo() , ftpFilePath, httpFilePath);
					}
				}
				//����Ƿ���ڼ�¼�����ݳ���ͼƬ��
				bExist = db.CheckImageExist(plateImagePath, "tztp");//��������Ҫ���ȷ���ɹ�д�����ݿ�
				if(bExist){
CopyPlateFile:		///����ͼƬ
					lRet = FileUtil::CopyFileEx(ftpPlateFilePath, httpPlateFilePath, true);
					if(lRet==true){
						dzlog_error("Copyed file from [%s] to [%s]", ftpFilePath, httpFilePath);
						DeleteFile(ftpPlateFilePath);//������ɺ�ɾ��FTP�µ��ļ�
					}else{
						dzlog_error("Copy file failed [GetLastError %s] from [%s] to [%s]", GetLastErrorInfo() , ftpFilePath, httpFilePath);
					}
				}
			}else{
				dzlog_error("cannot access file [%s]  [GetLastError %s] ", ftpFilePath,  GetLastErrorInfo());
			}
		}

		try{
			if(imagePath)
				delete imagePath;
		}catch(...){	}

		Sleep(10);
	}

end:
	sprintf(errorInfo, "[�߳��˳�]�����ص������� ThreadProcessWatchedFiles  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);
	return 0;
}


//���ϴ�����
//  ��ȡ���ݿ⣬��ȡδ�ϴ���ͼƬ�����ϴ������http�µ�ͼƬ�ļ������ڣ����ȡftp�µ�ͼƬ�������ϴ�
void* ThreadUpload(void* pParam)
{
	dzlog_notice("[�߳̿���]�ϴ����� ThreadUpload  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	queue<VehicleInfo*> listIn;
	VehicleInfo *vehicleInfo = 0;
	long lRet = 0;
	int id = 0;
	bool connected = false;
	TFDB db;

	while(true){

		db.Query(listIn);// ��ѯδ�ϴ�������

next:
		if(m_ExitCode==1)
			goto end;

		if(listIn.empty()){
			Sleep(100);
			continue;
		}

		vehicleInfo = listIn.front();
		listIn.pop();

		if(vehicleInfo==0)
			goto next;

connect:
		if( !connected )
			lRet = dlg->InitTrans(vehicleInfo);//��ʼ�����ӣ�ע�ᳵ��

		if( lRet == CONNECT_ERROR){
			dzlog_error("��������ʧ��") ;
			connected = false;
			Sleep(2000);
			goto connect;
		}	
		connected = true;

		id = vehicleInfo->id;
		lRet = dlg->UploadInfo(vehicleInfo);//�ϴ�����
		if(vehicleInfo)
			delete vehicleInfo;

		if( lRet == OK)
			db.Uploaded( id );//�ϴ��ɹ�
		else{
			//�ϴ�ʧ��
			if( lRet == FALIED) { //���ӳɹ������ϴ�ʧ��
				dzlog_error("���ӳɹ������ϴ�ʧ�� @ id=%d ", id) ; // to do something
			}else if( lRet == CONNECT_ERROR){
				dzlog_error("��������ʧ�� @ id=%d ",  id) ;
				connected = false;
				Sleep(100);
				goto connect; //��������
			}	
		}
		goto next;

		Sleep(100);
	}
end:
	sprintf(errorInfo, "[�߳��˳�]�ϴ����� ThreadUpload PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);
	return 0;
}


#define MAX_THREAD 10
pthread_t thread[MAX_THREAD]={0};  
void*	pth_join_ret[MAX_THREAD];


/* =========================================================================
*  �����ļ������߳�
*  �����ϴ��˳���FTPδ������ļ�
*    �����������ˣ��Լ����������ȴ����ϴ�������FTP�ϴ����ļ�
*    1.��FileUtil::ListFiles��ȡFTPĿ¼��δ������ļ�
*    2.����list���� 
===========================================================================*/
void* ThreadProcessLast(void *pParam)
{
	dzlog_notice("[�߳̿���]�����ļ������߳� ThreadProcessLast  PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};
	
	list<char *> list;
	char *imagePath = 0;

	while(true){
		if(m_ExitCode==1)
			goto end;

		FileUtil::ListFiles(dlg->kkConfig.ftpPath, NULL, list, dlg->kkConfig.fileExt, 1, false, true);

		while( !list.empty()){
			if(m_ExitCode==1)
				goto end;
			imagePath = list.front();
			list.pop_front();
			if(imagePath!=0 && dlg->m_images.size()<500)
				dlg->m_images.push(imagePath);
		}
		Sleep(10*1000);
	}
end:
	sprintf(errorInfo, "[�߳��˳�]�����ļ������߳� ThreadProcessLast PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);
	return 0;
}


typedef void (*_UpdateShareTime)(); //�궨�庯��ָ������
typedef time_t (*_GetNowTime)();
typedef time_t (*_GetShareTime)();


//ϵͳ��Դ���
void* ProcessMonitorThread(void *pParam)
{
start:
	dzlog_notice("[�߳̿���]ϵͳ��Դ����߳� ThreadProcessLast  PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	char temp[512]={0};
	uint64_t  mem=0;//�ڴ�ʹ��
	uint64_t  vmem=0;//�����ڴ�ʹ��
	int cpu =0;
	int ret=0;
	char tempsize[64]={0};
	time_t t=0;

	time_t	ShareTime = 0;
	time_t	NowTime = 0;
	char	pNowTime[64] = {0}; 
	char	pShareTime[64] = {0}; 

	HINSTANCE hDll; //DLL��� 
	_UpdateShareTime	UpdateShareTime; //����ָ��
	_GetNowTime			GetNowTime;
	_GetShareTime		GetShareTime;
	hDll = LoadLibrary("ShareModule.dll");
	if (hDll == NULL){
		dzlog_error(" hDll = LoadLibrary(ShareModule.dll) failed");
		return 0;
	}

	UpdateShareTime = (_UpdateShareTime)GetProcAddress(hDll, "UpdateShareTime");
	if (UpdateShareTime == NULL){
		FreeLibrary(hDll);
		dzlog_error(" hDll = GetProcAddress(UpdateShareTime) failed");
		return 0;
	}
	GetNowTime = (_GetNowTime)GetProcAddress(hDll, "GetNowTime");
	GetShareTime  = (_GetShareTime)GetProcAddress(hDll, "GetShareTime");

	try
	{
		while( true )
		{
			if(m_ExitCode==1)
				goto end;

		//	UpdateShareTime();//���ô��ʱ��

			NowTime = GetNowTime();
			sprintf(pNowTime,"%s", asctime(localtime(&NowTime)));
			if( strlen(pNowTime) > 2)
				pNowTime[strlen(pNowTime)-1] = '\0';

			ShareTime = GetShareTime();
			sprintf(pShareTime,"%s", asctime(localtime(&ShareTime)));
			if( strlen(pShareTime) > 2)
				pShareTime[strlen(pShareTime)-1] = '\0';

			sprintf(errorInfo, "ShareTime = %d [ %s ]  NowTime = %d [ %s ]  diff = %d ", (long)ShareTime, pShareTime, (long)NowTime, pNowTime, NowTime-ShareTime );
		//	dlg->GetDlgItem(ID_PROCESS_STATE)->SetWindowText(errorInfo);

			cpu = get_cpu_usage();
			ret = get_memory_usage( &mem, &vmem);
			if( cpu>=0)
				sprintf(temp, "CPU: %d%% ", cpu );
			if( ret>=0){
				sprintf(temp, "%s  Memery: %s | %s ",temp, SizeFormat(mem, tempsize) , SizeFormat(vmem, tempsize) );
			}
			sprintf(errorInfo, "ϵͳ��Դ ProcessMonitorThread [%s] %s ",temp, GetLastErrorInfo());
			dzlog_info("%s", errorInfo);
			dlg->GetDlgItem(ID_PROCESS_STATE)->SetWindowText(temp);
			Sleep(1000);
		}

	}catch(...){
		sprintf(errorInfo, "[�߳��˳�]ϵͳ��Դ����߳� ProcessMonitorThread PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
		dzlog_notice("%s",errorInfo);
		goto start;
	}

end:
	sprintf(errorInfo, "[�߳��˳�]ϵͳ��Դ����߳� ProcessMonitorThread PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);
	return 0;
}

//��ʼ�����߳�
void * ThreadStartAll(void *pParam)
{
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	threadRuning = !threadRuning;

	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	CMenu *pMenu = AfxGetApp()->m_pMainWnd->GetMenu(); 
	CMenu *pSubMenu = pMenu->GetSubMenu(2);

	dzlog_notice("======================================================================");
	if(threadRuning){
		dzlog_notice("��ʼ�����߳� StartWatch");
		m_ExitCode = 0;
		pthread_create(&thread[0], NULL, ThreadProcessLast, pParam);//�����ļ������߳�
		pthread_create(&thread[1], NULL, ThreadWatcher, pParam); //�������Ŀ¼�߳�
		pthread_create(&thread[2], NULL, ThreadProcessWatchedFiles, pParam); //���������ص������ݴ����߳�
		pthread_create(&thread[3], NULL, ThreadUpload, pParam); //���������ϴ��߳�
		pthread_create(&thread[4], NULL, ProcessMonitorThread, pParam); //ϵͳ��Դ���

		//AfxBeginThread(ThreadWatcher, this);//�������Ŀ¼�߳�
		//AfxBeginThread(ThreadProcessWatchedFiles, this);//���������ص������ݴ����߳�
		//AfxBeginThread(ThreadUpload, this);//���������ϴ��߳�
		pSubMenu->ModifyMenu(ID_START_WATCH, MF_BYCOMMAND ,ID_START_WATCH,"�رռ��");
	}else{
		dzlog_notice("�رչ����߳� OnStartWatch");
		m_ExitCode = 1;
		Sleep(500);
		pthread_join( thread[0], &pth_join_ret[0]);
		dzlog_notice("�ر��߳� thread0[�����ļ������߳�]   retruns : %s ", pth_join_ret[0]);
		pthread_join( thread[1], &pth_join_ret[1]);
		dzlog_notice("�ر��߳� thread1[���Ŀ¼�߳�]   retruns : %s ", pth_join_ret[1]);
		pthread_join( thread[2], &pth_join_ret[2]);
		dzlog_notice("�ر��߳� thread2[���ݴ����߳�]   retruns : %s ", pth_join_ret[2]);
		pthread_join( thread[3], &pth_join_ret[3]);
		dzlog_notice("�ر��߳� thread3[�����ϴ��߳�]   retruns : %s ", pth_join_ret[3]);
		pthread_join( thread[4], &pth_join_ret[4]);
		dzlog_notice("�ر��߳� thread4[ϵͳ��Դ����߳�]   retruns : %s ", pth_join_ret[4]);

	//	
		pSubMenu->ModifyMenu(ID_START_WATCH, MF_BYCOMMAND ,ID_START_WATCH,"�������");
	}
	sprintf(errorInfo, "[�߳��˳�] ��ʼ�����߳� ThreadStartAll  PID : 0x%x ", pthread_self() );
	pthread_exit(errorInfo);

	return 0;
}



//��ʼ���Ŀ¼
void Cwx_kk_upDlg::OnStartWatch()
{
	//
	if( !threadRuning){
		
		FileUtil::CreateFolders(kkConfig.ftpPath);
		FileUtil::CreateFolders(kkConfig.httpPath);

		kkConfig.hDir = CreateFile( kkConfig.ftpPath ,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ |
			FILE_SHARE_WRITE |
			FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		if( kkConfig.hDir == INVALID_HANDLE_VALUE )
		{
			sprintf(errorInfo, "�򿪼��Ŀ¼ʧ��[%s][%s]",kkConfig.ftpPath, GetLastErrorInfo() );
			dzlog_error(errorInfo);
			MessageBox(errorInfo);
			return ;
		}

	}

	pthread_t thread;
	pthread_create(&thread, NULL, ThreadStartAll, this); //���������߳�
}
void Cwx_kk_upDlg::OnCancel()
{
//	SetEvent(ov.hEvent);  //ʹ�����˳�
	OnExit();
}

/*	��ʼ���ϴ��ӿ�
*
*		#define		CONNECTED	2  //���ӳɹ�
*		#define		CONNECT_ERROR	3 //����ʧ��
*/
long Cwx_kk_upDlg::InitTrans(VehicleInfo *vehicleInfo)
{
	TCHAR *m_kkbh = ( vehicleInfo->kkbh );//���ڱ��
	TCHAR * m_fxlx = (vehicleInfo->fxlx);//��������
	LONG m_cdh = vehicleInfo->cdh;//������
	TCHAR * info = ("<info><jkid>62C01</jkid><jkxlh>791F09090306170309158E9882DB8B8E8FEEF681FF8B9AEB92CFD0A1A1C5A5D3CCA8</jkxlh></info>");//������Ϣ
	TCHAR * m_hostport = ( systemConfig.host );//��������ַ
	LONG lRet = 1 ;
	if( strcmp(systemConfig.runMode, "wx") ==0)
		lRet = m_agent.InitTrans(m_kkbh,m_fxlx,m_cdh, info ,m_hostport);//ע�ᳵ��

	if( lRet <= 0){
		dzlog_error("���ӷ�����ʧ��[%d] [GetLastError:%s] ", lRet, GetLastErrorInfo());

		return CONNECT_ERROR;
	}
	return CONNECTED;
}

/*
* �ϴ�����
* return: 
*	OK : OK
*	CONNECT_ERROR : connect server error
*   FALIED : connected, but upload failed
*
*	Ref:
		#define		OK			1  //�ɹ�
		#define		FALIED		-1 //ʧ��
		#define		CONNECTED	2  //���ӳɹ�
		#define		CONNECT_ERROR	3 //����ʧ��
*/
long Cwx_kk_upDlg::UploadInfo(VehicleInfo *vehicleInfo)
{

	long lRet = OK;
	char filePath[512]={0};
	
	if( strcmp(systemConfig.runMode, "wx") ==0)
	{
		//�ϴ��ı�
		lRet = m_agent.WriteVehicleInfo(
			vehicleInfo->kkbh, //���ڱ��
			vehicleInfo->fxlx, //	��������
			vehicleInfo->cdh, //	������
			vehicleInfo->hphm,//	���ƺ���
			vehicleInfo->hpzl,//	��������
			vehicleInfo->gcsj,//	����ʱ��
			vehicleInfo->clsd, //	�����ٶ�
			vehicleInfo->clxs, //	��������
			vehicleInfo->wfdm,//	Υ����Ϊ����
			vehicleInfo->cwkc, //��������
			vehicleInfo->hpys, //	������ɫ
			vehicleInfo->cllx,//	��������
			"","","","","","",
			vehicleInfo->tztp,//	ͨ��ͼƬ·��
			vehicleInfo->tp1,//	ͨ��ͼƬ1
			"", "", "");
		if(lRet<1)
			return lRet;

		//�ϴ�����ͼƬ
		sprintf(filePath, "%s\\", kkConfig.httpPath, vehicleInfo->tp1);
		if( !FileUtil::FindFirstFileExists( filePath, false) ){
			sprintf(filePath, "%s\\", kkConfig.ftpPath, vehicleInfo->tp1);
		}
		if( !FileUtil::FindFirstFileExists( filePath, false) ){
			dzlog_error("����ͼƬ������: %s", vehicleInfo->tp1);
		}else{
			lRet = m_agent.SavePicture(
				vehicleInfo->kkbh, //���ڱ��
				vehicleInfo->fxlx, //	��������
				vehicleInfo->cdh, //	������
				vehicleInfo->hphm,//	���ƺ���
				vehicleInfo->gcsj,//	����ʱ��
				1, // ͼƬ���	1-ͼƬ1��2-ͼƬ2��3-ͼƬ3
				filePath  //����ͼƬ
			);
		}
		//�ϴ�����ͼƬ������ͼƬ
		sprintf(filePath, "%s\\", kkConfig.httpPath, vehicleInfo->tztp);
		if( !FileUtil::FindFirstFileExists( filePath, false) ){
			sprintf(filePath, "%s\\", kkConfig.ftpPath, vehicleInfo->tztp);
		}
		if( !FileUtil::FindFirstFileExists( filePath, false) ){
			dzlog_error("����ͼƬ������: %s", vehicleInfo->tztp);
		}else{
			lRet = m_agent.SaveTzPicture(
				vehicleInfo->kkbh, //���ڱ��
				vehicleInfo->fxlx, //	��������
				vehicleInfo->cdh, //	������
				vehicleInfo->hphm,//	���ƺ���
				vehicleInfo->gcsj,//	����ʱ��
				filePath  //����ͼƬ
			);
		}
	}
	return lRet;
}

//��ʾlog
bool  InitedLogGUI = false;
char lastMsg[2048] = {0}; 
LRESULT Cwx_kk_upDlg::OnLogMessage(WPARAM wParam, LPARAM lParam)
{
	if( !InitedLogGUI )
	{
		m_LogList.SendMessage (LB_SETHORIZONTALEXTENT , 4096, 0);
		m_LogList.EnableScrollBarCtrl(TRUE);
		InitedLogGUI = true;
	}
	long curIndex = m_LogList.GetCurSel();
	long count = m_LogList.GetCount(); 

	LPCTSTR lpsz = (LPCTSTR)wParam;

	OutputDebugString(lpsz);
	char* pReturnChar = (char *)strchr(lpsz,'\r');
	if(pReturnChar != NULL)
		*pReturnChar = 0;

	
	// set the message bar text
	if(m_LogList.GetCount()>2000)
		m_LogList.ResetContent();
	if (lpsz != 0)
	{
		if( strcmp(lpsz, lastMsg) == 0 )
			;//ȥ���ظ�����Ϣ
		else
		{
			m_LogList.AddString(lpsz);
			//���û�Ϊѡ��������Ϣ����ѡ��������Ϣ
			sprintf(lastMsg, "%s", lpsz);
			if(curIndex==(count-1))
				m_LogList.SetCurSel(count);//ѡ��������Ϣ
		}
	}

	return 0;

}

void Cwx_kk_upDlg::OnExit()
{
	//�˳�
	if(threadRuning)
		OnStartWatch();

	::PostQuitMessage(0);
}

void Cwx_kk_upDlg::OnRunOnBackground()
{
	//��̨����
}

#include <winsock2.h>                   //socketͷ�ļ�
#pragma comment (lib, "ws2_32")         //socket���ļ�

#include "TCode.h" 

//������
void *ThreadUpdate(void *pParam)
{
	dzlog_notice("[�߳̿���]ϵͳ��Դ����߳� ThreadProcessLast  PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	// 1. socket 
#ifdef  USE_SOCKET

	long lRet;
	WSADATA wsaData;
    lRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (lRet)
	{
		sprintf(errorInfo, "WSAStartup() Error: %s\n", GetLastErrorInfo());
		WSACleanup();
		goto end;
	}

	sockaddr_in sockaddr;

	IN_ADDR  iaHost;
	LPHOSTENT lpHostEntry;

	iaHost.s_addr = inet_addr( dlg->systemConfig.updateurl );
	if (iaHost.s_addr == INADDR_NONE)
	{
		// Wasn't an IP address string, assume it is a name
		lpHostEntry = gethostbyname(dlg->systemConfig.updateurl);
	}
	else
	{
		// It was a valid IP address string
		lpHostEntry = gethostbyaddr((const char *)&iaHost,
			sizeof(struct in_addr), AF_INET);
	}

	if (lpHostEntry == NULL)
	{
		sprintf(errorInfo, "gethostbyname() |gethostbyaddr() Error: %s ", GetLastErrorInfo());
		goto end;
	}

	
	// 
	// Create a TCP/IP stream socket
	//
	SOCKET Socket; 

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		sprintf(errorInfo, "Error socket() %s ", GetLastErrorInfo());
		goto end;
	}

	//
	// Find the port number for the HTTP service on TCP
	//
	LPSERVENT lpServEnt;
	SOCKADDR_IN saServer;

	lpServEnt = getservbyname("http", "tcp");
	if (lpServEnt == NULL)
		saServer.sin_port = htons(80);
	else
		saServer.sin_port = lpServEnt->s_port;


	//
	// Fill in the rest of the server address structure
	//
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);


	//
	// Connect the socket
	//
	int nRet;

	nRet = connect(Socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		sprintf(errorInfo, "Error connect() %s ", GetLastErrorInfo());
		closesocket(Socket);
		goto end;
	}


	//
	// Format the HTTP request
	//
	char szBuffer[1024];

//	sprintf(szBuffer, "GET %s\n", lpFileName);
	nRet = send(Socket, szBuffer, strlen(szBuffer), 0);
	if (nRet == SOCKET_ERROR)
	{
		sprintf(errorInfo, "Error send()  %s ", GetLastErrorInfo());
		closesocket(Socket); 
		goto end;
	}

	//
	// Receive the file contents and print to stdout
	//
	while(1)
	{
		// Wait to receive, nRet = NumberOfBytesReceived
		nRet = recv(Socket, szBuffer, sizeof(szBuffer), 0);
		if (nRet == SOCKET_ERROR)
		{
			sprintf(errorInfo, "Error recv()  %s ", GetLastErrorInfo());
			break;
		}

		fprintf(stderr,"\nrecv() returned %d bytes", nRet);

		// Did the server close the connection?
		if (nRet == 0)
			break;
		// Write to stdout
		fwrite(szBuffer, nRet, 1, stdout);
	}
	closesocket(Socket); 

#else
	// user httpclient

	//	1��get��ʽ   
	CHttpClient   conn;   
	CString   value;
	value.Format( "%s", dlg->systemConfig.updateurl );    
	value=conn.doGet(value);  
	value = U2G(value);
	dzlog_notice("CHttpClient Recv:  %s", value);

	//2��post��ʽ   
	/*
	CHttpClient   conn;   
	CString   value;
	value.Format( "%s", dlg->systemConfig.updateurl );    
	conn.addParam("name1","value1");   
	conn.addParam("name2","value2");   
	conn.addParam("name3","value3");   
	conn.addParam("name4","value4");   
	conn.addParam("name5","value5");   
	conn.addParam("name6","value6");   
	value=conn.doPost(value);         
	*/

#endif

	sprintf(errorInfo, "[�߳��˳�] ThreadUpdate PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());

end:
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);

	return 0;
}
void Cwx_kk_upDlg::OnCheckUpdate()
{
	//������
	pthread_t thread;
	pthread_create(&thread, NULL, ThreadUpdate, this);//�����ļ������߳�

}
