// DB.cpp
/*
* Auth: Karl
* Date: 2014/04/28
* LastUpdate: 2014/04/28
*/

//======================================================
#pragma once

#include "stdio.h"
#include "string.h"
#include "DB.h"
#include "FileUtil.h"
#include "TCode.h"
#include "zlog.h"



//===============================
char * TFDB::GetAppPath(char *appPath)
{
//	memset(appPath, 0,MAX_PATH);
	char temp[MAX_PATH]={0};
	GetModuleFileName(NULL,  temp, MAX_PATH);
	char *p = strrchr(temp,'\\');
	if(p != NULL)
	{
		memcpy(appPath, temp, sizeof(char)*(p-temp));
		return appPath;
	}
	return NULL;
}


char sqlitePath[MAX_PATH]={0};


sqlite3 * TFDB::OpenDB()
{
	char appPath[MAX_PATH] = {0};
	if(pSqlite3DB==NULL)
	{
		GetAppPath(appPath);
		memset(sqlitePath, 0, MAX_PATH);
		sprintf(sqlitePath, "%s/"SQLIT_NAME, appPath );
		int error = sqlite3_open(sqlitePath, &pSqlite3DB);
		if(error){
			dzlog_error("can't open database[%d]: %s\n",error, sqlite3_errmsg(pSqlite3DB), sqlite3_errstr(error));
		//	MessageBox(0, L"opendb success.", L"", 0);
			return NULL;
		}
	}

	return pSqlite3DB;
}



int  TFDB::CloseDB(sqlite3  *p )
{
	int ret = 0;
	if(p!=0 ){
		ret =sqlite3_close(p);
		p = 0;
	}
	return ret;
}


//����imagePath��ѯ�Ƿ��Ѿ�����
bool  TFDB::CheckImageExist(char *imagePath)
{
	pSqlite3DB = OpenDB();
	if(pSqlite3DB==NULL)
		return -1;

	sqlite3_stmt  *stmt = 0 ;

	char strsql[MAX_SQL]={0};
	sprintf(strsql, "select * from "TABLE_NAME" where tp1=?" ) ;

	dzlog_debug("strsql = %s \n", strsql);
		
	int error = sqlite3_prepare_v2(pSqlite3DB , strsql , strlen(strsql) , &stmt , NULL);
	if(error != SQLITE_OK)
	{
		dzlog_error("can't CheckImage[%d]: %s  \n", error,   sqlite3_errstr(error));
		if(stmt)
		{
			sqlite3_finalize(stmt);
		}
		CloseDB(pSqlite3DB);
		return -1;
	}
	char *temp = 0 ;
	temp = G2U(imagePath);//folder
	sqlite3_bind_text( stmt , 1 , temp , strlen(temp), NULL);

	int ret = sqlite3_step(stmt);

	sqlite3_finalize(stmt);
	//CloseDB(pSqlite3DB);
	if(ret != SQLITE_DONE)
	{  
		dzlog_error("sqlite3_step faile \n" );
		return -1;
	}      
	
	return (ret == SQLITE_ROW);
	return false;
}

//д�����ݿ�
long  TFDB::Add(VehicleInfo *v)
{
	int line=0;

	pSqlite3DB = OpenDB();
	if(pSqlite3DB==NULL)
		return -1;

	sqlite3_stmt  *stmt = 0 ;

	char strsql[MAX_SQL]={0};
	sprintf(strsql, "insert into "TABLE_NAME"(kkbh,fxlx,cdh,hphm,hpzl,gcsj,clsd,clxs,wfdm,cwkc,hpys,cllx, csys, tplj,tp1, tztp , status) \
						values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,1) " ) ;

	dzlog_debug("strsql = %s \n", strsql);
	
	int error = sqlite3_prepare_v2(pSqlite3DB , strsql , strlen(strsql) , &stmt , NULL);
	if(error != SQLITE_OK)
	{
		if(stmt)
		{
			dzlog_error("can't sqlite3_prepare_v2[%d]: %s  \n", error,   sqlite3_errstr(error));
			sqlite3_finalize(stmt);
		}
		CloseDB(pSqlite3DB);
		return -1;
	}
	char *temp = 0 ;
	
	int col = 1;
	
	temp = G2U(v->kkbh);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//���ڱ��
	
	temp = G2U(v->fxlx);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//��������
	
	sqlite3_bind_int( stmt , col++ , v->cdh);//������
	
	temp = G2U(v->hphm);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//���ƺ���
	
	temp = G2U(v->hpzl);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//��������
	
	temp = G2U(v->gcsj);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//����ʱ��
	
	sqlite3_bind_int( stmt , col++ , v->clsd);//����
	sqlite3_bind_int( stmt , col++ , v->clxs);//����
	
	temp = G2U(v->wfdm);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//Υ����Ϊ����
	
	sqlite3_bind_int( stmt , col++ , v->cwkc);//��������
	
	temp = G2U(v->hpys);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//������ɫ
	
	temp = G2U(v->cllx);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//��������
	
	temp = G2U(v->csys);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//������ɫ
 
	temp = G2U(v->tplj);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//ͨ��ͼƬ·��
	
	temp = G2U(v->tp1);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//ͨ��ͼƬ1

	temp = G2U(v->tztp);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//����ͼƬ
	
	int ret = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
//	CloseDB(pSqlite3DB);
	if(ret != SQLITE_DONE)
	{  
		dzlog_error("sqlite3_step faile \n" );
		return -1;
	}      

	return 1;
}


//��ȡδ�ϴ���ͼƬ
int  TFDB::Query(queue<VehicleInfo*> &listIn)
{
	pSqlite3DB = OpenDB();
	if(pSqlite3DB==NULL)
		return -1;

	sqlite3_stmt  *stmt = 0 ;


	char strsql[MAX_SQL]={0};
	sprintf(strsql, "select id,kkbh,fxlx,cdh,hphm,hpzl,gcsj,clsd,clxs,wfdm,cwkc,hpys,cllx,fzhpzl,fzhphm,fzhpys,clpp,clwx,csys,tplj,tp1,tp2,tp3,tztp from "TABLE_NAME" where status=1 " ) ;

//	dzlog_debug("strsql = %s \n", strsql);
		
	int error = sqlite3_prepare_v2(pSqlite3DB , strsql , strlen(strsql) , &stmt , NULL);
	if(error != SQLITE_OK)
	{
		dzlog_error("can't getLPRList[%d]: %s  \n",error,   sqlite3_errstr(error));
		if(stmt)
		{
			sqlite3_finalize(stmt);
		}
		CloseDB(pSqlite3DB);
		return -1;
	}
	char *temp = 0 ;

	int nColumn = sqlite3_column_count(stmt);
	int vtype , i, row=0;
	int ret=0;
	int col=0;
	do{	
		error = sqlite3_step(stmt);
		if(error == SQLITE_ROW)
		{
			col = 0;
			row++;
			//select id,kkbh,fxlx,cdh,hphm,hpzl,gcsj,clsd,clxs,wfdm,cwkc,hpys,cllx,fzhpzl,fzhphm,fzhpys,clpp,clwx,csys,tplj,tp1,tp2,tp3,tztp from "TABLE_LPR_NAME" where status=1  
			VehicleInfo *vechileInfo = new VehicleInfo();
			vechileInfo->id = sqlite3_column_int(stmt , col++);//id
			temp = (char*)sqlite3_column_text(stmt , col++);//���ڱ��
			
			sprintf( vechileInfo->kkbh, "%s", U2G(temp));
			temp = (char*)sqlite3_column_text(stmt , col++);//��������
			sprintf( vechileInfo->fxlx, "%s", U2G(temp));
			vechileInfo->cdh = sqlite3_column_int(stmt , col++);//������
			temp = (char*)sqlite3_column_text(stmt , col++);//���ƺ���
			sprintf( vechileInfo->hphm, "%s", U2G(temp));
			temp = (char*)sqlite3_column_text(stmt , col++);//��������
			sprintf( vechileInfo->hpzl, "%s", U2G(temp));
			temp = (char*)sqlite3_column_text(stmt , col++);//����ʱ��
			sprintf( vechileInfo->gcsj, "%s", U2G(temp));
			vechileInfo->clsd = sqlite3_column_int(stmt , col++);//����
			vechileInfo->clxs = sqlite3_column_int(stmt , col++);//����
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->wfdm, "%s", U2G(temp));//Υ����Ϊ����
			vechileInfo->cwkc = sqlite3_column_int(stmt , col++);//��������
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->hpys, "%s", U2G(temp));//������ɫ
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->cllx, "%s", U2G(temp));//��������
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->fzhpzl, "%s", U2G(temp));//������������
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->fzhphm, "%s", U2G(temp));//�������ƺ���
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->fzhpys, "%s", U2G(temp));//����������ɫ
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->clpp, "%s", U2G(temp));//����Ʒ��
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->clwx, "%s", U2G(temp));//��������
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->csys, "%s", U2G(temp));//������ɫ
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tplj, "%s", U2G(temp));//ͨ��ͼƬ·��
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tp1, "%s", U2G(temp));//ͨ��ͼƬ1
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tp2, "%s", U2G(temp));//ͨ��ͼƬ2
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tp3, "%s", U2G(temp));//ͨ��ͼƬ3
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tztp, "%s", U2G(temp));//����ͼƬ

			listIn.push(vechileInfo);//�������
			
			
		}
		else if(error == SQLITE_DONE)
		{
		//	dzlog_info("Select finish\n");
			ret = 0;
			break;
		}
		else
		{
			dzlog_error("Select failed\n");
			ret = 0;
			break;
		}
	}while(1);
	sqlite3_finalize(stmt);
//	CloseDB(pSqlite3DB);

	return listIn.size();
}



//�ϴ���Ϣ���
bool  TFDB::Uploaded(int id)
{
	int line=0;

	pSqlite3DB = OpenDB();
	if(pSqlite3DB==NULL)
		return -1;

	sqlite3_stmt  *stmt = 0 ;


	char strsql[MAX_SQL]={0};
	sprintf(strsql, "update "TABLE_NAME" set status=0 where id=?" ) ;

	dzlog_debug("strsql = %s  %d \n", strsql, id);
	
	int error = sqlite3_prepare_v2(pSqlite3DB , strsql , strlen(strsql) , &stmt , NULL);
	if(error != SQLITE_OK)
	{
		if(stmt)
		{
			dzlog_error("can't sqlite3_prepare_v2[%d]: %s  \n", error,   sqlite3_errstr(error));
			sqlite3_finalize(stmt);
		}
		CloseDB(pSqlite3DB);
		return false;
	}

	sqlite3_bind_int( stmt , 1 , id);//ID
	
	int ret = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
//	CloseDB(pSqlite3DB);
	if(ret != SQLITE_DONE)
	{  
		dzlog_error("sqlite3_step faile \n" );
		return false;
	}      

	return true;
}

TFDB::TFDB(void)
{
	pSqlite3DB = 0;
	memset(sqlitePath, 0 , MAX_PATH);
//	stmt = 0;
}

TFDB::~TFDB(void)
{
}