// KKConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "wx_kk_up.h"
#include "KKConfigDlg.h"



// KKConfigDlg �Ի���

IMPLEMENT_DYNAMIC(KKConfigDlg, CDialog)

KKConfigDlg::KKConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KKConfigDlg::IDD, pParent)
	, id(_T(""))
	, direction(_T(""))
	, cdid(0)
	, cdSpeedLimit(0)
	, cdSpeedLimitDefalut(0)
	, path(_T(""))
	, m_httpPath(_T(""))
	, http(_T(""))
	, fileExt(_T("\"*.jpg;*.bmp;*.png\""))
{
	pParentDlg = 0;
//	dzlog_info("init KKConfigDlg pParent: 0x%x ", pParent);

}

KKConfigDlg::~KKConfigDlg()
{
}

void KKConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, id);
	DDX_Text(pDX, IDC_EDIT2, direction);
	DDX_Text(pDX, IDC_EDIT3, cdid);
	DDV_MinMaxLong(pDX, cdid, 1, 10);
	DDX_Text(pDX, IDC_EDIT4, cdSpeedLimit);
	DDV_MinMaxLong(pDX, cdSpeedLimit, 0, 1000);
	DDX_Text(pDX, IDC_EDIT5, cdSpeedLimitDefalut);
	DDV_MinMaxLong(pDX, cdSpeedLimitDefalut, 0, 1000);
	DDX_Text(pDX, IDC_EDIT6, path);
	DDX_Text(pDX, IDC_EDIT7, m_httpPath);
	DDX_Text(pDX, IDC_EDIT8, http);
	DDX_Text(pDX, IDC_EDIT9, fileExt);
}

BEGIN_MESSAGE_MAP(KKConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, &KKConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(BT_BROWSER_FOLDER, &KKConfigDlg::OnBnClickedBrowserFolder)
	ON_BN_CLICKED(BT_BROWSER_FOLDER2, &KKConfigDlg::OnBnClickedBrowserFolder2)
	ON_BN_CLICKED(IDC_BT_QUERY_SPEEDLIMIT, &KKConfigDlg::OnBnClickedBtQuerySpeedlimit)
END_MESSAGE_MAP()


// KKConfigDlg ��Ϣ�������

void KKConfigDlg::OnBnClickedOk()
{
	UpdateData(true);

	sprintf( kkConfig.id, id.GetBuffer(id.GetLength()));
	sprintf( kkConfig.direction, direction.GetBuffer(direction.GetLength()));
	kkConfig.cdid = cdid;
	kkConfig.cdSpeedLimit = cdSpeedLimit;
	kkConfig.cdSpeedLimitDefalut = cdSpeedLimitDefalut;
	sprintf( kkConfig.ftpPath, path);
	sprintf( kkConfig.httpPath, m_httpPath);
	sprintf( kkConfig.http , http);
	sprintf( kkConfig.fileExt, fileExt);

	OnOK();
}

BOOL KKConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	id.Format("%s", kkConfig.id);
	direction.Format("%s", kkConfig.direction);
	cdid = kkConfig.cdid ;
	cdSpeedLimit = kkConfig.cdSpeedLimit;
	cdSpeedLimitDefalut = kkConfig.cdSpeedLimitDefalut;
	path.Format("%s", kkConfig.ftpPath);
	m_httpPath.Format("%s", kkConfig.httpPath);
	http.Format("%s", kkConfig.http);
	fileExt.Format("%s", kkConfig.fileExt);

	UpdateData(false);

	return TRUE;  
}

#include "FileUtil.h"

void KKConfigDlg::OnBnClickedBrowserFolder()
{
	UpdateData(true);
	char *temp = FileUtil::SelectFolder(this->m_hWnd, "", path.GetBuffer(path.GetLength()) );
	if(temp!=0){
		path.Format("%s", temp);
	}
	UpdateData(false);
}

void KKConfigDlg::OnBnClickedBrowserFolder2()
{
	UpdateData(true);
	char *temp = FileUtil::SelectFolder(this->m_hWnd, "", m_httpPath.GetBuffer(m_httpPath.GetLength()) );
	if(temp!=0){
		m_httpPath.Format("%s", temp);
	}
	UpdateData(false);
}


#include "wx_kk_upDlg.h"

void KKConfigDlg::OnBnClickedBtQuerySpeedlimit()
{
	// ��ѯ����ֵ
	if(pParentDlg==0)
		return ;

	Cwx_kk_upDlg* pDlg = (Cwx_kk_upDlg *)pParentDlg;

	char errorInfo[256]={0};

	UpdateData(true);

	sprintf( kkConfig.id, id.GetBuffer(id.GetLength()));//���ڱ��
	sprintf( kkConfig.direction, direction.GetBuffer(direction.GetLength()));//����
	kkConfig.cdid = cdid;//������
	kkConfig.cdSpeedLimit = cdSpeedLimit;//����
	kkConfig.cdSpeedLimitDefalut = cdSpeedLimitDefalut;//����Ĭ��ֵ

	TCHAR *m_kkbh = ( kkConfig.id);//���ڱ��
	TCHAR * m_fxlx = (kkConfig.direction);//��������
	LONG m_cdh = kkConfig.cdid;//������
    TCHAR * info = ("<info><jkid>62C01</jkid><jkxlh>791F09090306170309158E9882DB8B8E8FEEF681FF8B9AEB92CFD0A1A1C5A5D3CCA8</jkxlh></info>");//������Ϣ
	TCHAR * m_hostport = ( pDlg->systemConfig.host );//��������ַ
	LONG ret = 1 ;
	if( strcmp(pDlg->systemConfig.runMode, "wx") ==0)
		ret = pDlg->m_agent.InitTrans(m_kkbh,m_fxlx,m_cdh, info ,m_hostport);//ע�ᳵ��
	
	if(ret<=0){
		sprintf(errorInfo,  "���ӷ�����ʧ��[%d]",ret);
		dzlog_error(errorInfo);
		MessageBox(errorInfo);
	}

	/*
	//ʱ��ͬ��
	CString csSyncTime = "";
	if( strcmp(systemConfig.runMode, "wx") ==0)
		csSyncTime = m_agent.QuerySyncTime();//ʱ��ͬ��
	//SetupDeviceTime()
	*/
	
	//����ͬ��
	TCHAR *cllx = "1";//��������	0-�󳵣�1-С����2-��������
	long lLimitSpeed = 40; 
	if( strcmp(pDlg->systemConfig.runMode, "wx") ==0){
		lLimitSpeed = pDlg->m_agent.QueryLimitSpeed(m_kkbh,m_fxlx,m_cdh, cllx);////����ͬ��
		if( lLimitSpeed < 5 ) //Ϊ������ϵͳ��³���Կ��ǣ�����������ص�����ֵΪ0����(��С��5����)������ʹ�ñ���ȱʡ������ֵ����������ζ�����Ŀ�û��׼ȷ��¼����ֵ��
			cdSpeedLimit = kkConfig.cdSpeedLimitDefalut;
		else
			cdSpeedLimit = lLimitSpeed;
	}
	UpdateData(true);

}
