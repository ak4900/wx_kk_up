#pragma once


// KKConfigDlg �Ի���




class KKConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(KKConfigDlg)

	

public:
	KKConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~KKConfigDlg();

	CWnd *pParentDlg;

// �Ի�������
	enum { IDD = IDD_KK_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public :
	KKConfig		kkConfig;
	void	setConfig(KKConfig config){ this->kkConfig = config; }

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CString id;//���ڱ��
	CString direction;// ��������
	long cdid;// ������
	long cdSpeedLimit;// ��������
	long cdSpeedLimitDefalut;// ��������Ĭ��ֵ
	CString path;// ���Ŀ¼

	afx_msg void OnBnClickedBrowserFolder();
	CString m_httpPath;
	afx_msg void OnBnClickedBrowserFolder2();
	afx_msg void OnBnClickedBtQuerySpeedlimit();
	CString http;
	CString fileExt;
};
