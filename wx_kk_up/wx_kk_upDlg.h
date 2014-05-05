
// wx_kk_upDlg.h : ͷ�ļ�
//

#pragma once

#ifndef _WX_KK_UPDLG_HEADER_
#define _WX_KK_UPDLG_HEADER_

#include "htagentobj1.h"

#include "systemconfigdlg.h"
#include "kkconfigdlg.h"

#include "queue"
#include "afxwin.h"
using namespace std;

// Cwx_kk_upDlg �Ի���
class Cwx_kk_upDlg : public CDialog
{
// ����
public:
	Cwx_kk_upDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~Cwx_kk_upDlg();

// �Ի�������
	enum { IDD = IDD_WX_KK_UP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CHtagentobj1 m_agent;

	void Init();//��ʼ������ȡ�����ļ�->InitTrans()->QuerySyncTime()->QueryLimitSpeed-> DH_SDK::ENT_SetupDeviceTime�豸Уʱ
	bool AgentInit();
	bool ReadConfig();
	bool SaveConfig();
	
	CString m_appPath;//����������·��

	static SystemConfig	systemConfig;//ϵͳ��������
	static KKConfig		kkConfig;//��������
	VehicleInfo		vehicleInfo;
	queue<char* >  m_images;
	

	afx_msg void OnAboutbox();
	afx_msg void OnSystemConfigDlg();
	afx_msg void OnKkConfigDlg();

	SystemConfigDlg systemConfigDlg;
	KKConfigDlg		kkConfigDlg;
	afx_msg void OnStartWatch();
	
	TCHAR errorInfo[TF_MAX_PATH_LEN] ;

	long InitTrans(VehicleInfo *vehicleInfo);

	/*
	* �ϴ�����
	* return: 
	*	1 : OK
	*	2 : connect server error
	*   3 : connected, but upload failed
	*/
	long UploadInfo(VehicleInfo *vehicleInfo);

	afx_msg LRESULT OnLogMessage(WPARAM wParam, LPARAM lParam = 0L);

protected:
	virtual void OnCancel();
public:
	CListBox m_LogList;
	afx_msg void OnExit();
	afx_msg void OnRunOnBackground();
	afx_msg void OnCheckUpdate();
};

#endif 