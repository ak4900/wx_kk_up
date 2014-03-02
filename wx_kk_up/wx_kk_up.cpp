
// wx_kk_up.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "wx_kk_up.h"
#include "wx_kk_upDlg.h"

#include "TCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cwx_kk_upApp

BEGIN_MESSAGE_MAP(Cwx_kk_upApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Cwx_kk_upApp ����

Cwx_kk_upApp::Cwx_kk_upApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� Cwx_kk_upApp ����

Cwx_kk_upApp theApp;


// Cwx_kk_upApp ��ʼ��

BOOL Cwx_kk_upApp::InitInstance()
{
	FileUtil::CreateFolders("zlog");
	dzlog_init("logcfg.conf", "my_cat");

	dzlog_info("start this program!");

	CCrashHandler ch;//������������һ���쳣
	ch.SetProcessExceptionHandlers();//����һ��������������Ч���쳣����������SEH)
	ch.SetThreadExceptionHandlers();//���ý��Ե�ǰ���߳���Ч���쳣����������Ԥ�����쳣����ֹ��������
	
	dzlog_info("started carshHandler ");



	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));


		
	//��ȡ�ؼ�ע��״̬
	char temp[256]={0};
	HRESULT hr = -1;
	CLSID clsid = {0};
	char pClsid[256]={0};
	LONG lValue = 256;
	LPWSTR lpClsid;
	hr = CLSIDFromProgID(  L"HTAgent.HTAgentObj",&clsid);
	if(hr==S_OK){
		if( StringFromCLSID( clsid, &lpClsid) == S_OK)
		{
			wcharTochar( lpClsid, pClsid);
			//GetDllPath
			HKEY hKey;     
			BOOL bPresent;     
			TCHAR szPath[MAX_PATH];     
			DWORD dwRegType;     
			DWORD cbData   =   sizeof   szPath   *   sizeof   TCHAR;    
			sprintf(temp, "CLSID\\%s\\InprocServer32",pClsid); 
			if(RegOpenKeyEx(HKEY_CLASSES_ROOT, temp, 0, KEY_READ,&hKey)==ERROR_SUCCESS){
				if( RegQueryValue(hKey,NULL, temp, &lValue ) == ERROR_SUCCESS){
				//	m_dllPath.Format("%s",temp);
				}
			}
		}
	}else{
		MessageBox(0,"�ؼ�δע�ᣬ����ע��ؼ�","ϵͳ��ʾ", MB_OK | MB_ICONHAND );
		WinExec("InstanceConfig.exe",SW_SHOW);
		::PostQuitMessage(0);
	}


	Cwx_kk_upDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int Cwx_kk_upApp::ExitInstance()
{
	zlog_fini();

	return CWinAppEx::ExitInstance();
}
