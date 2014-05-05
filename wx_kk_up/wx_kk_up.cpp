
// wx_kk_up.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "wx_kk_up.h"
#include "wx_kk_upDlg.h"


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
