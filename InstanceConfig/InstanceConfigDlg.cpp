
// InstanceConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "InstanceConfig.h"
#include "InstanceConfigDlg.h"

#include "FileUtil.h"
#include "TCode.h"

#include "KService.h"
 
char szWatchedExeName[64] = {"wx_kk_up.exe"};//����صĳ�������
char szServiceExeName[64] = {"wx_kk_up_service.exe"};//��ط����������
char szServiceName[64] = {"TF_wx_kk_Service"};//��ط�������
char m_lpServicePathName[512]={0};//��ط������·��

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CInstanceConfigDlg::CInstanceConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstanceConfigDlg::IDD, pParent)
	, m_dllPath(_T(""))
	, m_serviceName(_T(""))
	, m_serviceDisplayName(_T(""))
	, m_serviceDescription(_T(""))
	, m_servicePath(_T(""))
	, m_serviceParam(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInstanceConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DLL_PATH, m_dllPath);
	DDX_Text(pDX, IDC_SERVICE_NAME, m_serviceName);
	DDX_Text(pDX, IDC_SERVICE_DISPLAY_NAME, m_serviceDisplayName);
	DDX_Text(pDX, IDC_SERVICE_DESCRIPTION, m_serviceDescription);
	DDX_Control(pDX, IDC_SERVICE_STRAT_TYPE, m_serviceStartType);
	DDX_Text(pDX, IDC_SERVICE_PATH, m_servicePath);
	DDX_Control(pDX, IDC_SERVICE_SELECT, m_serviceType);
	DDX_Text(pDX, IDC_SERVICE_PARAM, m_serviceParam);
}

BEGIN_MESSAGE_MAP(CInstanceConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BT_INSTALL_SERVICE, &CInstanceConfigDlg::OnBnClickedBtInstallService)
	ON_BN_CLICKED(IDC_BT_UNISTALL_SERVICE, &CInstanceConfigDlg::OnBnClickedBtUnistallService)
	ON_BN_CLICKED(IDC_BT_START_SERVICE, &CInstanceConfigDlg::OnBnClickedBtStartService)
	ON_BN_CLICKED(IDC_BT_STOP_SERVICE, &CInstanceConfigDlg::OnBnClickedBtStopService)
	ON_BN_CLICKED(IDC_BT_CLOSE_PROCESS, &CInstanceConfigDlg::OnBnClickedBtCloseProcess)
	ON_BN_CLICKED(IDC_BT_BROWSER_DLL, &CInstanceConfigDlg::OnBnClickedBtBrowserDll)
	ON_BN_CLICKED(IDC_BT_DLL_REG, &CInstanceConfigDlg::OnBnClickedBtDllReg)
	ON_BN_CLICKED(IDC_BT_DLL_UNREG, &CInstanceConfigDlg::OnBnClickedBtDllUnreg)
	ON_BN_CLICKED(IDC_BT_BROWSER_SERVER_EXE, &CInstanceConfigDlg::OnBnClickedBtBrowserServerExe)
	ON_CBN_SELCHANGE(IDC_SERVICE_SELECT, &CInstanceConfigDlg::OnCbnSelchangeServiceSelect)
	ON_BN_CLICKED(IDC_BT_INSTALL_HTTP, &CInstanceConfigDlg::OnBnClickedBtInstallHttp)
	ON_BN_CLICKED(IDC_BT_HTTP_CONFIG, &CInstanceConfigDlg::OnBnClickedBtHttpConfig)
END_MESSAGE_MAP()


BOOL CInstanceConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	
	memset(thisPath, 0, MAX_PATH);
	//��ȡ��ǰ·��  ===============================
	::GetModuleFileName( 0, thisPath, 512);
	char *p = strrchr(thisPath, '\\');
	if(p)
		*p='\0';
	// end of ��ȡ��ǰ·�� ========================


	int i=0;

	//��������
	int size = sizeof(ServiceTypeName)/sizeof(ServiceTypeName[0]);
	for( i=0; i < size; i++)
		m_serviceType.AddString( ServiceTypeName[i]);
//	m_serviceType.SetCurSel(0);

	//������������
	int sizeOfStartType = sizeof(StartType)/sizeof(StartType[0]);
	for( i=0; i < sizeOfStartType; i++)
		m_serviceStartType.AddString( StartType[i]);
	m_serviceStartType.SetCurSel(2);

	//�������·��
	//m_servicePath.Format("%s\\wx_kk_up_service.exe", thisPath);

	//��ȡ�ؼ�ע��״̬
	char temp[256]={0};
	HRESULT hr = -1;
	CLSID clsid = {0};
	char pClsid[256]={0};
	LONG lValue = 256;
	LPWSTR lpClsid;
	hr = CLSIDFromProgID(  L"HTAgent.HTAgentObj",&clsid);
	if(hr==S_OK){
		GetDlgItem(IDC_BT_DLL_REG)->EnableWindow(false);
		GetDlgItem(IDC_BT_DLL_UNREG)->EnableWindow(true);
		if( StringFromCLSID( clsid, &lpClsid) == S_OK)
		{
			wcharTochar( lpClsid, pClsid);
			//GetDllPath
			HKEY hKey;     
			BOOL bPresent;     
			TCHAR szPath[_MAX_PATH];     
			DWORD dwRegType;     
			DWORD cbData   =   sizeof   szPath   *   sizeof   TCHAR;    
			sprintf(temp, "CLSID\\%s\\InprocServer32",pClsid); 
			if(RegOpenKeyEx(HKEY_CLASSES_ROOT, temp, 0, KEY_READ,&hKey)==ERROR_SUCCESS){
				if( RegQueryValue(hKey,NULL, temp, &lValue ) == ERROR_SUCCESS){
					m_dllPath.Format("%s",temp);
					UpdateData(false);
				}
			}
		}
	}else{
		GetDlgItem(IDC_BT_DLL_REG)->EnableWindow(true);
		GetDlgItem(IDC_BT_DLL_UNREG)->EnableWindow(false);
	}

	UpdateStatus();

	return TRUE;  
}


void CInstanceConfigDlg::OnPaint()
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
HCURSOR CInstanceConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CInstanceConfigDlg::OnBnClickedBtBrowserServerExe()
{
	//����ؼ�EXE
	UpdateData(true);
	char path[512] = {0};
	GetModuleFileName( NULL, path, 512);
	if( !m_dllPath.Trim().IsEmpty() )
		sprintf(path, "%s", m_dllPath.GetBuffer(m_dllPath.GetLength()) );
	CFileDialog fDlg(TRUE, "�������(*.exe)|*.exe | �������(*.dll;*.ocx;*.sys)|*.dll;*.ocx;*.sys|All Files (*.*)|*.*||",  path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );
	if( fDlg.DoModal() == IDOK )
	{
		if( !m_serviceName.IsEmpty()){
			m_servicePath.Format("\"%s\" %s", fDlg.GetPathName(), m_serviceParam);
		}else{
			MessageBox("������д��������");
		}
	}
	UpdateData(false);
}


bool CInstanceConfigDlg::UpdateStatus()
{
	GetDlgItem(IDC_BT_INSTALL_SERVICE)->EnableWindow(false);
	GetDlgItem(IDC_BT_UNISTALL_SERVICE)->EnableWindow(false);
	GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
	GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);

	char szError[512]={0};
	bool bRet = false;
	long lRet = 0;
	KService ksv;
	
	if(m_serviceName.IsEmpty())
		goto end;

	m_serviceDisplayName = "";
	m_serviceDescription = "";
	m_servicePath = "";
	m_serviceParam = "";

	//��ѯ�����Ƿ��Ѿ���װ
	lRet = ksv.ServiceExist( m_serviceName.GetBuffer(m_serviceName.GetLength()) );
	if( lRet == 0 )
	{
		//�Ѿ���װ
		GetDlgItem(IDC_BT_UNISTALL_SERVICE)->EnableWindow(true);

		//���ð�ť�ͷ���״̬��Ϣ
		Service sv;
		lRet = ksv.QueryService( m_serviceName.GetBuffer(m_serviceName.GetLength()), &sv);
		if(lRet==0)
		{
			m_serviceName.Format("%s", sv.szServiceName);
			m_serviceDisplayName.Format("%s", sv.szServiceDisplayName);
			m_serviceDescription.Format("%s", sv.szServiceDescription);
			m_servicePath.Format("%s", sv.szServicePath);
			char *p = strchr( sv.szServicePath, ' ');
			if(p)
				m_serviceParam.Format("%s", p);

			if( sv.status.dwCurrentState == SERVICE_RUNNING){
				GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
				GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(true);
				GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("��������...");
			}else if( sv.status.dwCurrentState == SERVICE_STOPPED){
				GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(true);
				GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
				GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("��ֹͣ");
			}else{
				GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
				GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
				GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("״̬������!");
			}
		}else{
			MessageBox( GetLastErrorText( lRet, szError, 512) , 0, MB_ICONHAND  );
		}
		
	}else if( lRet == 1060 ){
		 //���񲻴���
		GetDlgItem(IDC_BT_INSTALL_SERVICE)->EnableWindow(true);
	}else{
		//����
		MessageBox( GetLastErrorText( lRet, szError, 512) , 0, MB_ICONHAND  );
		goto end;
	}

end:
	return lRet;
}

//ע�����
void CInstanceConfigDlg::OnBnClickedBtInstallService()
{
	UpdateData(true);
	char szError[512]={0};
	long lRet = 0;

	KService ksv;
	Service sv;

	if(m_serviceName.IsEmpty()){
		MessageBox("����д��������");
		return;
	}
	if(m_serviceDisplayName.IsEmpty()){
		MessageBox("����д������ʾ����");
		return;
	}
	if(m_servicePath.IsEmpty()){
		MessageBox("��ѡ��������");
		return;
	}
	sprintf(sv.szServiceName, "%s", m_serviceName);
	sprintf(sv.szServiceDisplayName,"%s", m_serviceDisplayName);
	sprintf(sv.szServicePath, "%s", m_servicePath);
	sprintf(sv.szServiceDescription, "%s", m_serviceDescription);
	ksv.SetService(sv);
	lRet = ksv.Install();
	if( lRet==0 )
	{
		GetDlgItem(IDC_BT_UNISTALL_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
		int ret = MessageBox("����װ�ɹ�, �Ƿ����ڴ򿪷���?", NULL, MB_OKCANCEL ) ;
		if( ret == IDOK ){
			OnBnClickedBtStartService();
		}
	}else{
		MessageBox( GetLastErrorText(lRet, szError, 512) , 0, MB_ICONHAND  );
	}
}

//ж�ط���
void CInstanceConfigDlg::OnBnClickedBtUnistallService()
{
	UpdateData(true);
	char szError[512]={0};
	long lRet = 0;
	//ж�ط���
	KService ksv;
	Service sv;
	sprintf(sv.szServiceName, "%s", m_serviceName);
	lRet = ksv.SetService(sv);
	GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("����ж��...");
	lRet = ksv.UnInstall();
	if( lRet==0 ){
		GetDlgItem(IDC_BT_INSTALL_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_BT_UNISTALL_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("������ж��!");
		MessageBox("ж�سɹ�");
	}else{
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("...");
		MessageBox( GetLastErrorText(lRet,szError, 512) , 0, MB_ICONHAND  );
	}
}

// ��������
void CInstanceConfigDlg::OnBnClickedBtStartService()
{
	UpdateData(true);
	char szError[512]={0};
	long lRet = 0;
	// ��������

	KService ksv;
	Service sv;
	sprintf(sv.szServiceName, "%s", m_serviceName);
	ksv.SetService(sv);
	GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("���ڿ���...");
	lRet = ksv.Start();
	if( lRet==0 )
	{
		GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("�����ѿ���");
	}else{
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("...");
		MessageBox( GetLastErrorText( lRet, szError, 512) , 0, MB_ICONHAND  );
	}

}

void CInstanceConfigDlg::OnBnClickedBtStopService()
{
	UpdateData(true);
	//ֹͣ����
	char szError[512]={0};
	long lRet = 0;

	KService ksv;
	Service sv;
	sprintf(sv.szServiceName, "%s", m_serviceName);
	ksv.SetService(sv);
	GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("����ֹͣ...");
	lRet = ksv.Stop();
	if( lRet==0 )
	{
		GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("������ֹͣ");
	}else{
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("...");
		MessageBox( GetLastErrorText( lRet, szError, 512) , 0, MB_ICONHAND  );
	}
}

void CInstanceConfigDlg::OnBnClickedBtCloseProcess()
{
	//�رպ�̨���е��ϴ�����

	char cmd[512] = {0};
	sprintf(cmd, "taskkill /f /im  %s ",szWatchedExeName); 
	system(cmd);

}

//�ж��Ƿ�ע��  
BOOL IsRegistedDll(char *CLSID)
{
	HKEY hKey;     
	BOOL bPresent;     
	TCHAR szPath[_MAX_PATH];     
	DWORD dwRegType;     
	DWORD cbData   =   sizeof   szPath   *   sizeof   TCHAR;    

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"ActiveX.ActiveXControl//Clsid",0,KEY_READ,&hKey)!=ERROR_SUCCESS)
		return false;
	else
		return true;
	//ActiveX.ActiveXControlΪ�ؼ������ƺ��ڲ�����   ��"NTGRAPH.NTGraphCtrl.1//CLSID"

}

void CInstanceConfigDlg::OnBnClickedBtBrowserDll()
{
	//����ؼ�
	UpdateData(true);
	char path[512] = {0};
	GetModuleFileName( NULL, path, 512);
	if( !m_dllPath.Trim().IsEmpty() )
		sprintf(path, "%s", m_dllPath.GetBuffer(m_dllPath.GetLength()) );
	CFileDialog fDlg(TRUE, "�ؼ�(*.dll;*.ocx;*.sys)|*.dll;*.ocx;*.sys|All Files (*.*)|*.*||",  path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );
	if( fDlg.DoModal() == IDOK )
	{
		m_dllPath.Format("%s", fDlg.GetPathName());
	}
	UpdateData(false);
}

void CInstanceConfigDlg::OnBnClickedBtDllReg()
{
	// ע��ؼ�
	UpdateData(true);
	bool bRet = false;
	HINSTANCE hLib = LoadLibrary( m_dllPath);      //ָ��Ҫע���ocx�ļ���·�����ļ�����

	if (hLib == NULL)
	{
		MessageBox("��������ؼ��ļ�!");
		return;
	}

	//��ȡע�ắ��DllRegisterServer��ַ
	FARPROC lpDllEntryPoint;
	lpDllEntryPoint = GetProcAddress(hLib,"DllRegisterServer");

	//����ע�ắ��DllRegisterServer
	if(lpDllEntryPoint!=NULL)
	{
		if(FAILED((*lpDllEntryPoint)()))
		{
			//MessageBox(Handle,"����DllRegisterServerʧ��!","ʧ��ing..",MB_OK);
			bRet = false;
			MessageBox("�ؼ�ע��ʧ��!");
			FreeLibrary(hLib);
			goto end;
		};
		//MessageBox(Handle,"ע��ɹ�!","Reg",MB_OK);
		bRet = true;
		MessageBox("�ؼ�ע��ɹ�!");
	}
	else{
		bRet = false;
		MessageBox("�ؼ�ע��ʧ��!");
		//MessageBox(Handle,"����DllRegisterServerʧ��!","ʧ��ing..",MB_OK);
	}
end:
	GetDlgItem(IDC_BT_DLL_REG)->EnableWindow( !bRet);
	GetDlgItem(IDC_BT_DLL_UNREG)->EnableWindow(bRet);
}

void CInstanceConfigDlg::OnBnClickedBtDllUnreg()
{
	UpdateData(true);
	bool bRet = false;
	// ж�ؿؼ�
	HINSTANCE hLib = LoadLibrary( m_dllPath );          //ָ��Ҫע���ocx�ļ���·�����ļ�����
	if (hLib==NULL)
	{
		//MessageBox(Handle,"��������Dll�ļ�!","ʧ��ing..",MB_OK);
		MessageBox("��������ؼ��ļ�!");
		return;
	}
	//��ȡע�ắ��DllRegisterServer��ַ
	FARPROC lpDllEntryPoint;
	lpDllEntryPoint = GetProcAddress(hLib,"DllUnregisterServer");//ע��������DllUnregisterServer��

	//����ע�ắ��DllRegisterServer
	if(lpDllEntryPoint!=NULL)
	{
		if(((*lpDllEntryPoint)()))
		{
			bRet = false;
			//MessageBox(Handle,"����DllUnRegisterServerʧ�ܣ�","ʧ��ing..",MB_OK);
			MessageBox("���ÿؼ�DllUnRegisterServerʧ��!!!");
			FreeLibrary(hLib);
			goto end;
		};
		bRet = true;
		//MessageBox(Handle,"ע���ɹ���","Unreg",MB_OK);
		MessageBox("ע���ؼ��ɹ���");
	}
	else{
		bRet = false;
		//MessageBox(Handle,"����DllUnRegisterServerʧ��!!!","���",MB_OK);
		MessageBox("���ÿؼ�DllUnRegisterServerʧ��!!!");
		goto end;
	}
end:
	GetDlgItem(IDC_BT_DLL_REG)->EnableWindow(bRet);
	GetDlgItem(IDC_BT_DLL_UNREG)->EnableWindow(!bRet);
}


void CInstanceConfigDlg::OnCbnSelchangeServiceSelect()
{
	//ѡ�����
	int len = sizeof(ServiceTypeName) / sizeof(ServiceTypeName[0]);
	int index = m_serviceType.GetCurSel();
	if(index >=0 && index<len){
		m_serviceName.Format("%s", ServiceName[index]);
		GetDlgItem(IDC_SERVICE_SELECT)->EnableWindow(false);
		//m_serviceType.EnableWindow(false);
		UpdateStatus();
		m_serviceType.EnableWindow(true);
		//MessageBox(ServiceTypeName[index]);
		UpdateData(false);
	}
}

void CInstanceConfigDlg::OnBnClickedBtInstallHttp()
{
	//HTTP����װ
	//Install Http
	char cmd[512]={0};
	sprintf(cmd, "\"%s\\httpd\\bin\\Apache.exe\" -k install -n %s & pause", thisPath, m_serviceName);
	system(cmd);
}

void CInstanceConfigDlg::OnBnClickedBtHttpConfig()
{
	//HTTP��������
	char cmd[512]={0};
	sprintf(cmd, "notepad.exe  \"%s\\httpd\\conf\\httpd.conf\"", thisPath);
	WinExec(cmd, SW_SHOW);
}
