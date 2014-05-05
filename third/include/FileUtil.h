// FileUtil.h

/**
* Auth: Karl
* Date: 2014/2/20
* LastUpdate: 2014/2/24
*/

#pragma once

#ifndef HEADER_FILEUTIL
#define HEADER_FILEUTIL

//#include <list>
//#include <string>
//using namespace std;

#include "afxwin.h"
#include <windows.h>  
#include <ShlObj.h> 
#include <stdio.h>
#include <time.h>   
#include <string>
#include <list>

using namespace std;

#ifndef MAX_PATH
	#define MAX_PATH 512
#endif

class FileUtil
{
public:
	FileUtil(void);
	~FileUtil(void);

	/**
	* �г�Ŀ¼�µ������ļ�
	* @baseDir : �ļ���Ŀ¼
	* @curDir �����Ŀ¼������·��Ϊ baseDir + "\\" + curDir + "\\" + wfd.cFileName
	* @listResult: ���ص��ļ��б�
	* @fileExt: �ļ���չ���ƣ��� "*.jpg;*.bmp;*.png"�� ���Ϊ�� NULL | "" �򲻹����ļ�
	* @PathNameType: ���ص��ļ����Ʒ�ʽ, 0��ȫ·��  1:���·��(curDir + "\\" + wfd.cFileName) 2:�ļ���(wfd.cFileName)
	* @bAddDirectory: �ļ��������Ƿ�����ļ��б�
	* @bListSub: �Ƿ��г����ļ����е��ļ�
	*
	* @Return : &listResult
	*
	* e.g:
	*	 list<char *> list; 
	*	 FileUtil::ListFiles( "D:\\upload", "", list, "*.jpg;*.bmp;*.png", true, false, true);
	*
	*/
	static list<char*>* ListFiles(char *baseDir, char *curDir, list<char*> &listResult, char *fileExt = 0, int PathNameType=0, bool bAddDirectory=false, bool bListSub=false);

	
	//����ѡ���ļ��жԻ���
	static char* SelectFolder(HWND hwnd, char* title="Select Folder", char* dir=0);

	//�ж��ļ�/�ļ����Ƿ����
	//�ж��ļ���	FindFirstFileExists(lpPath, FALSE);  
	//�ж��ļ��У�	FindFirstFileExists(lpPath, FILE_ATTRIBUTE_DIRECTORY);  
	static BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter);

	//�����༶Ŀ¼
	static bool CreateFolders(const char* folderPath);

	//�����ļ������Զ�����Ŀ¼
	static long	CopyFileEx(const char* src, const char* dest, BOOL bFailIfExists ); 

	//ɾ���༶Ŀ¼�������ļ���
	static bool RemoveDir(const char* szFileDir);

	static char *FormatFileName(const char *path, int indexIn=0, bool fileTime=true, const char *destDir=0);

private:
	


};

char *GetDateTime(char *timeString=0);
void __cdecl debug(const char *format, ...);
void __cdecl release(const char *format, ...);

long __cdecl getCurrentTime();

#endif