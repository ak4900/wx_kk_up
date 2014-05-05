
// tfcommon.h
#pragma once

#ifndef _TF_COMMMON_HEADER_
#define _TF_COMMMON_HEADER_


//=========== define errors  ================

#define		OK			1  //�ɹ�
#define		FALIED		-1 //ʧ��
#define		REFUSED		0 // Refuse
#define		CONNECTED	2  //���ӳɹ�
#define		CONNECT_ERROR	-2 //����ʧ��
#define		CONNECT_TIMEOUT -1 //ͨѶ��ʱ

//===========================================



#define TF_MAX_PATH_LEN  512

//ϵͳ��������
typedef struct SystemConfig{
	char	updateurl[TF_MAX_PATH_LEN];//������µ�ַ
	int		autorun;//����������Զ�����
	char	host[TF_MAX_PATH_LEN];//������Ϣ�ϴ���������ַ
	char	runMode[64];//#����ģʽ,{"��������":"wx", "":""}
}SystemConfig;

//��������
typedef struct KKConfig{
	char	id[TF_MAX_PATH_LEN];//���ڱ��
	char	direction[TF_MAX_PATH_LEN];//��������
	long	cdid;//������
	long	cdSpeedLimit;//��������
	long	cdSpeedLimitDefalut;//��������Ĭ��ֵ
	void *  hDir; //�ļ���ؾ��
	char	ftpPath[TF_MAX_PATH_LEN];//��Ӧ�ļ��FTPĿ¼
	char	httpPath[TF_MAX_PATH_LEN];//��Ӧ��HTTPĿ¼
	char	http[TF_MAX_PATH_LEN];//http����
	char	fileExt[128];//FTPĿ¼����Ҫ����ļ���չ��:e.g:  "*.jpg;*.bmp;*.png"
}KKConfig;


//������Ϣ,�ϴ����鲼��ϵͳ
typedef struct VehicleInfo
{
	long	id;//for db

	char	kkbh[64];//���ڱ��  ����Ϊ��
	char	fxlx[64];//	��������	����Ϊ��
	long	cdh;//	������	����Ϊ��
	char	hphm[64];//	���ƺ���	���ơ�δʶ���޷�ʶ����ð�ǡ�-����ʾ�������޺���Ҫע��hpzl��41
	char	hpzl[64];//	��������	�ο�GA24.7����01-����������02-С��������25-ũ�����ƣ�41-�޺��ƣ�42-�ٺ��ƣ�99-�������ƣ�������Ϊ�գ�
	char	gcsj[64];//	����ʱ��	���ڸ�ʽ�ַ�����yyyy-mm-dd hh24:mi:ss��,���硰2003-09-11 11:07:23��������Ϊ��
	long	clsd;//	�����ٶ�	�3λ����λ������/Сʱ
	long	clxs;//	��������	�3λ����λ������/Сʱ
	char	wfdm[16];//	Υ����Ϊ����	�ο�GA408.1
	long	cwkc;//��������,�5λ��������Ϊ��λ
	char	hpys[16];//	������ɫ	0-��ɫ��1-��ɫ��2-��ɫ��3-��ɫ��4-��ɫ��9-������ɫ������Ϊ�գ�
	char	cllx[64];//	��������	�ο�GA24.4��K11-������ͨ�ͳ���K21-������ͨ�ͳ���K31-С����ͨ�ͳ���K33-�γ���H11-������ͨ�ͳ���H21-������ͨ�ͳ���M22-������Ħ�г���
	char	fzhpzl[64];//	������������	�ο�GA24.7
	char	fzhphm[16];//	�������ƺ���	���ơ�δʶ���޷�ʶ����ð�ǡ�-����ʾ�������޺���Ҫע��hpzl��41
	char	fzhpys[16];//	����������ɫ	0-��ɫ��1-��ɫ��2-��ɫ��3-��ɫ��4-��ɫ��9-������ɫ
	char	clpp[64];//	����Ʒ��	
	char	clwx[64];//	��������	
	char	csys[16];//	������ɫ	�ο�GA24.8��A-�ף�B-�ң�C-�ƣ�D-�ۣ�E-�죬F-�ϣ�G-�̣�H-����I-�أ�J-�ڣ�Z-������
	char	tplj[256];//ͨ��ͼƬ·��	ͼƬ����·���й̶����֣������޶����ܳ���100���ֽڣ����磺FTP����·����ftp://user:666@192.168.1.1/001.jpg������ͼƬ·��Ϊftp://user:666@192.168.1.1/  
	char	tp3[256];
	char	tp2[256];
	char	tp1[256];//	ͨ��ͼƬ1	ͼƬ����·���б仯�Ĳ��֣������޶����ܳ���60���ֽڣ����磺FTP����·����ftp://user:666@192.168.1.1/001.jpg������ͼƬ����Ϊ001.jpg��
	
	char	tztp[256];//	����ͼƬ	��չԤ����Ϊ��
}VehicleInfo;

#endif