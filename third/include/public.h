
//public.h

/**
* Auth: Karl
* Date: 2014/2/20
* LastUpdate: 2014/2/24
*/

#pragma once
#ifndef HEADER_PUBLIC
#define HEADER_PUBLIC

#include <winsock2.h>

static int	lastErrorCode =0;
static char lastError[256]={0};
char*		getLastError();
int			setLastError(char *info);
int			getLastErrorCode();
int			setLastErrorCode(int code);



void alert(char *text);


/*************************************************/


TCHAR* g_GetIniPath(void);
char* ConvertInfo(char* strText);

//Set static text in dialogue box (English->current language)

TCHAR* g_GetSetPath(void);

void EventLog(char* info);//��ʷ��¼�ļ�

//ע�����ز���
bool		OpenReg(HKEY &hKEY, char *data_Set);
BYTE*		QueryReg(char *key, DWORD dataLen, HKEY hKEY);
long		CreateReg(HKEY hKEY,const char *subkey);
bool		WriteReg(HKEY &hKEY, char *data_Set);
bool		SetReg(char *key, BYTE* value, DWORD dataLen, HKEY hKEY);
//\**************************************************************/

//******** date time classes **************
char *formatTime(time_t* t);
char *formatTMTime(struct tm *time);
void Encryption(BYTE *data, long lenData,  BYTE *key, long lenKey);
void Decryption(BYTE *data, long lenData,  BYTE *key, long lenKey);

/**********  math *************/



/**********************************/


#endif