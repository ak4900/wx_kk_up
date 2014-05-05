#pragma once
#include "afxwin.h"


// SystemConfigDlg �Ի���

class SystemConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(SystemConfigDlg)

public:
	SystemConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SystemConfigDlg();

// �Ի�������
	enum { IDD = IDD_SYS_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public :
	SystemConfig systemConfig;
	void  setConfig(SystemConfig config);

	afx_msg void OnBnClickedOk();
	CString m_updateurl;
	CComboBox m_autorun;
	CString m_host;
	CComboBox m_devMode;
	virtual BOOL OnInitDialog();
	
};
