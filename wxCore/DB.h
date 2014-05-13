
// DB.h
/*
* Auth: Karl
* Date: 2014/04/28
* LastUpdate: 2014/04/28
*/

#ifndef  __LPR_DB_H
#define  __LPR_DB_H

//=====  STL ==============
#include <iostream>
#include <list>
#include <queue>
using namespace std;
//=========================

extern "C"
{
	#include "./sqlite3.h"
};
#include "tfcommon.h"


//===========  define ================================


#define SQLIT_NAME		"wx_kk_up.db"  //���ݿ��ļ�����
#define TABLE_NAME	"vehicle_info" //����ʶ�����洢������

#ifndef MAX_PATH
	#define		MAX_PATH	 512
#endif

#ifndef NULL
	#define		NULL	 0
#endif

#define MAX_SQL  4096

//===========  //define ================================



class TFDB
{
	
	char * GetAppPath(char *appPath);
	
public:
	sqlite3  *pSqlite3DB;


	sqlite3 *OpenDB();
	int CloseDB(sqlite3  *p=0);

	//����imagePath��ѯ�Ƿ��Ѿ�����
	bool CheckImageExist(char *imagePath, char* colName);

	//д�����ݿ�
	long Add(VehicleInfo *vehicleInfo);

	//��ȡδ�ϴ���ͼƬ
	int Query(queue<VehicleInfo*> &listIn);

	//�ϴ���Ϣ���
	bool Uploaded(int id);

	TFDB(void);
	~TFDB(void);
};


#endif