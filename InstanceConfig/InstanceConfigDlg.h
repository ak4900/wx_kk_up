
// InstanceConfigDlg.h : ͷ�ļ�
//

#pragma once

#include "winsvc.h"
#include "afxwin.h"


#define MAX_SERVICE_TYPE 2
static char ServiceTypeName[MAX_SERVICE_TYPE][64] = { "�����ϴ���ط���", "HTTP����" };
static char ServiceName[MAX_SERVICE_TYPE][64] = { "TF_wx_kk_up", "TF_httpd" };

// CInstanceConfigDlg �Ի���
class CInstanceConfigDlg : public CDialog
{
// ����
public:
	CInstanceConfigDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_INSTANCECONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	char thisPath[MAX_PATH];//��ǰ·��

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtInstallService();

	bool UpdateStatus();
	afx_msg void OnBnClickedBtUnistallService();
	afx_msg void OnBnClickedBtStartService();

	afx_msg void OnBnClickedBtStopService();
	afx_msg void OnBnClickedBtCloseProcess();
	afx_msg void OnBnClickedBtBrowserDll();
	afx_msg void OnBnClickedBtDllReg();
	afx_msg void OnBnClickedBtDllUnreg();
	CString m_dllPath;
	CString m_serviceName;
	CString m_serviceDisplayName;
	CString m_serviceDescription;
	CComboBox m_serviceStartType;
	CString m_servicePath;
	afx_msg void OnBnClickedBtBrowserServerExe();
	CComboBox m_serviceType;
	afx_msg void OnCbnSelchangeServiceSelect();
	afx_msg void OnBnClickedBtInstallHttp();
	CString m_serviceParam;
	afx_msg void OnBnClickedBtHttpConfig();
};
