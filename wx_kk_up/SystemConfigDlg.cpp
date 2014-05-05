// SystemConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "wx_kk_up.h"
#include "SystemConfigDlg.h"


// SystemConfigDlg �Ի���

IMPLEMENT_DYNAMIC(SystemConfigDlg, CDialog)

SystemConfigDlg::SystemConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SystemConfigDlg::IDD, pParent)
	, m_updateurl(_T(""))
	, m_host(_T(""))
{

}

SystemConfigDlg::~SystemConfigDlg()
{

}

void SystemConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_UPDATE_URL, m_updateurl);
	DDX_Control(pDX, IDC_COMBO_AUTORUN, m_autorun);
	DDX_Text(pDX, IDC_EDIT_HOST, m_host);
	DDX_Control(pDX, IDC_COMBO_RUNMODE, m_devMode);
}


BEGIN_MESSAGE_MAP(SystemConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, &SystemConfigDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SystemConfigDlg ��Ϣ�������

#define AUTORUN_MAX 2
#define DEVMODE_MAX 2

static char *autorun[AUTORUN_MAX]={"�ֶ�����","�Զ�����"};
static long  autorunValue[AUTORUN_MAX]={0,1};

static char *devMode[DEVMODE_MAX]={"��������","����ģʽ"};
static char *devModeValue[DEVMODE_MAX]={"wx",""};

//���浽����systemConfig������
void SystemConfigDlg::OnBnClickedOk()
{
	UpdateData(true);

	int index = m_autorun.GetCurSel();
	if(index>=0)
		systemConfig.autorun = autorunValue[index];

	index = m_devMode.GetCurSel();
	if(index>=0)
		strcpy(systemConfig.runMode, devModeValue[index]);

	strcpy(systemConfig.updateurl, m_updateurl.GetBuffer(m_updateurl.GetLength()) );
	strcpy(systemConfig.host, m_host.GetBuffer(m_host.GetLength()) );

	OnOK();
}

void  SystemConfigDlg::setConfig(SystemConfig config)
{
	systemConfig = config;
}


BOOL SystemConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_autorun.ResetContent();
	for(int i=0; i<AUTORUN_MAX; i++){
		m_autorun.AddString(autorun[i]);
	}
	m_autorun.SetCurSel(0);

	m_devMode.ResetContent();
	for(int j=0; j<DEVMODE_MAX; j++){
		m_devMode.AddString(devMode[j]);
	}
	m_devMode.SetCurSel(0);

	///
	m_updateurl.Format("%s", systemConfig.updateurl);
	m_host.Format("%s", systemConfig.host);
	
	if(systemConfig.autorun>=0){
		for(int i=0; i<AUTORUN_MAX; i++){
			if(autorunValue[i]==systemConfig.autorun)
				m_autorun.SetCurSel(i);
		}
	}

	for(int j=0; j<DEVMODE_MAX; j++){
		if( strcmp(devModeValue[j], systemConfig.runMode)==0 )
			m_devMode.SetCurSel(j);
	}


	UpdateData(false);

	return TRUE;  
}
