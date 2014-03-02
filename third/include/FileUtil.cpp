
// FileUtil.cpp : ʵ���ļ�

/**
* Auth: Karl
* Date: 2014/2/20
* LastUpdate:2014/5/8
*/
#include "stdafx.h"

#include "FileUtil.h"


using namespace std;

FileUtil::FileUtil(void)
{
}

FileUtil::~FileUtil(void)
{
}
//ɾ��Ŀ¼���ļ�
bool FileUtil::RemoveDir(const char* szFileDir)
{
	
	if(szFileDir==NULL  || !FindFirstFileExists(szFileDir, FILE_ATTRIBUTE_DIRECTORY) )
		return false;
	char *strDir =new char[256];
	char fileName[256] ={0}, temp[256]={0};
	memset(strDir,0, 256);
	memcpy(strDir, szFileDir, strlen(szFileDir));
	sprintf(temp, "%s\\*.*",strDir);
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile((temp),&wfd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	do
	{
		sprintf(fileName, "%s\\%s", strDir, wfd.cFileName);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (stricmp(wfd.cFileName,".") != 0 &&
				stricmp(wfd.cFileName,"..") != 0)
				RemoveDir(fileName);
		}
		else
		{
			DeleteFile(fileName);
		}
	}
	while (FindNextFile(hFind,&wfd));
		FindClose(hFind);
	RemoveDirectory(szFileDir);
	delete strDir;
	
	return true;
	
}

//�����ļ������Զ�����Ŀ¼
long FileUtil::CopyFileEx(const char* src, const char* dest, BOOL bFailIfExists )
{
	long lRet = 0;
	char temp[1024]={0};
	strcpy(temp, dest);
	char *pFileName = strrchr( temp, '\\');
	if(pFileName<=0)
		return 0;
	pFileName[0] = '\0';

	if( !FileUtil::FindFirstFileExists(temp, FILE_ATTRIBUTE_DIRECTORY) )
		lRet = FileUtil::CreateFolders(temp);
	lRet = CopyFile(src, dest, bFailIfExists );
	return lRet;
}

//�����༶Ŀ¼
bool FileUtil::CreateFolders(const char* folderPath)
{
	char  folder[MAX_PATH]={0};
	char  temp[MAX_PATH]={0};
	char *p=0;
	memcpy(folder, folderPath, strlen(folderPath));

	int len = strlen(folder);
	while( (p = strchr(folder, '\\'))!=NULL){
		*p='/';
	}
	if( folder[len-1]!='/')
		folder[len] = '/';
	len = strlen(folder);
	folder[len] = '\0';

	int index=2;
	BOOL isOk = true;
	
	p = folder;
	while((p = strchr(p, '/'))!=NULL)
	{
		memcpy(temp, folder, p-folder);
		p ++;
		isOk = (BOOL)CreateDirectory( temp, 0 );
		//if(!isOk�� 
		//	return false;
	}
	isOk = (BOOL)CreateDirectory( temp, 0 );
	
	return true;
}

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
list<char*>* FileUtil::ListFiles(char *baseDir, char *curDir, list<char*> &listResult, char *fileExt, int PathNameType, bool bAddDirectory, bool bListSub)
{
	if(baseDir==0 || strlen(baseDir)<1)
		return 0;

	char file[MAX_PATH] = {0};
	
	if( curDir!=0 && lstrlen(curDir)>0 ){
		sprintf(file,"%s\\%s", baseDir, curDir);
	}else{
		sprintf(file,"%s", baseDir);
	}
	lstrcat(file,"\\*.*");

	char _curDir[MAX_PATH] = {0};
	if(curDir!=0)
		sprintf(_curDir, "%s", curDir);
	
	WIN32_FIND_DATA wfd; 
	HANDLE Find = FindFirstFile(file,&wfd); 
	if(Find == INVALID_HANDLE_VALUE){
		return NULL;
	}
	char *pExtTemp=0;
	do
	{
		if (wfd.cFileName[0] == '.') 
		{
			continue;
		} 
		char *szFindPath = new char[MAX_PATH];
		memset(szFindPath, 0, MAX_PATH);

		//
		if(PathNameType==0)
			sprintf(szFindPath, "%s\\%s\\%s", baseDir, curDir, wfd.cFileName);//����Ŀ¼
		else if(PathNameType==1 && ( curDir!=0 && lstrlen(curDir)>0 )){
			sprintf(szFindPath, "%s\\%s", curDir, wfd.cFileName);//���Ŀ¼
		}else
			sprintf(szFindPath, "%s",  wfd.cFileName);//�ļ���
		
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{//���ļ���

			//�������Ŀ¼
			if( curDir!=0 && lstrlen(curDir)>0 ){
				sprintf(_curDir, "%s\\%s", curDir, wfd.cFileName);
			}else{
				sprintf(_curDir, "%s", wfd.cFileName);
			}

			if(bAddDirectory)
				listResult.push_back( szFindPath );//���Ŀ¼��list
				
			if(bListSub){ //�г���Ŀ¼�µ��ļ�
				ListFiles(baseDir, _curDir, listResult, fileExt, PathNameType, bAddDirectory, bListSub);  
			}	
		}else{ // ���ļ�
			if(fileExt==0 || strlen(fileExt)<1){
				listResult.push_back( szFindPath );
			}else {
				pExtTemp = strrchr(wfd.cFileName, '.');
				if(strstr((char*)fileExt, (char*)pExtTemp) > 0)
					listResult.push_back( szFindPath );
				else
					delete szFindPath;
			}
		}
	}while (FindNextFile(Find, &wfd));
	FindClose(Find);

	return &listResult;

}

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
    switch(uMsg)
    {
    case BFFM_INITIALIZED:
        ::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);
        break;
    }

    return 0;
}


//ѡ���ļ��� �Ի���
#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif
char* FileUtil::SelectFolder(HWND hwnd, char* outDir, char* title, char* orgDir)
{
	char *szFolder = outDir; //new char[MAX_PATH]; //�õ��ļ�·��	
	if(szFolder==0)
		return 0;
//	memset(szFolder, 0, MAX_PATH);

	//HWND hwnd = hWnd->GetSafeHwnd();   //�õ����ھ��
#ifdef _SHGetMalloc_  
	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc) == NOERROR)		//ȡ��IMalloc�������ӿ�
	{   
		BROWSEINFO		bi;
		TCHAR			pszBuffer[MAX_PATH];
		LPITEMIDLIST	pidl;   

		bi.hwndOwner		= hwnd;
		bi.pidlRoot			= NULL;
		bi.pszDisplayName   = pszBuffer;
		bi.lpszTitle		= _T(title); //ѡ��Ŀ¼�Ի�����ϲ��ֵı���
		//����½��ļ��а�ť BIF_NEWDIALOGSTYLE
		bi.ulFlags			=  BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;
		bi.lpfn				= NULL;
		bi.lParam			= 0;
		bi.iImage			= 0;
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)  //ȡ��IMalloc�������ӿ�
		{   
			if (::SHGetPathFromIDList(pidl, pszBuffer)) //���һ���ļ�ϵͳ·��
			{
				sprintf(szFolder, "%s",  pszBuffer);
			}
			else szFolder = '\0';
			pMalloc->Free(pidl);	//�ͷ��ڴ�

			//	MessageBox(m_strPath);
		}
		pMalloc->Release();			//�ͷŽӿ�
	}
#else  
	BROWSEINFO bi;  
	TCHAR			pszBuffer[MAX_PATH];
	ZeroMemory(&bi,sizeof(BROWSEINFO));  
	bi.hwndOwner		= hwnd;
	bi.pidlRoot			= NULL;
	bi.lpszTitle		= (title); //ѡ��Ŀ¼�Ի�����ϲ��ֵı���
	//����½��ļ��а�ť BIF_NEWDIALOGSTYLE
	bi.ulFlags			=  BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_EDITBOX;
	bi.iImage			= 0;
	if(orgDir!=0)
	{
		bi.lpfn				= BrowseCallbackProc;
		bi.lParam			= (LPARAM)(LPCTSTR)orgDir;
	}else{
		bi.lpfn				= 0;
		bi.lParam			= 0;
	}

	LPITEMIDLIST pidl=(LPITEMIDLIST)CoTaskMemAlloc(sizeof(LPITEMIDLIST));  
	pidl = SHBrowseForFolder(&bi);  
	TCHAR * path = new TCHAR[MAX_PATH];  
	memset(path, 0, MAX_PATH);
	if(pidl != NULL)  
	{  
		if(SHGetPathFromIDList(pidl,path))
			sprintf(szFolder, "%s",  path);
		//MessageBox(NULL,path,TEXT(title),MB_OK);  
	}  
	else 
	{  
	//	delete szFolder;
	//	szFolder = 0;
		goto end;
	  //	MessageBox(NULL,TEXT("EMPTY"),TEXT("Choose"),MB_OK);  
	}  
	CoTaskMemFree(pidl);  
	delete path;   
#endif  

	return szFolder;
end:
	return NULL;
}


//�ж��ļ�/�ļ����Ƿ����
//�ж��ļ���	FindFirstFileExists(lpPath, FALSE);  
//�ж��ļ��У�	FindFirstFileExists(lpPath, FILE_ATTRIBUTE_DIRECTORY);  
BOOL FileUtil::FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)  
{  
	WIN32_FIND_DATA fd;  
	HANDLE hFind = FindFirstFile(lpPath, &fd);  
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;  
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;  
	FindClose(hFind);  
	return RetValue;  
}  

int copyfile(const char *srcFile, const char *dstFile)
{
	FILE *fpIn=0, *fpOut=0;
	fpIn = fopen(srcFile, "rb");
	fpOut = fopen(dstFile, "wb");

	if(fpIn==0 || fpOut==0){
		debug("copyfile faile @ src=%s  dest=%s", srcFile, dstFile);
		fclose(fpIn);
		fclose(fpOut);
		return false;
	}
	bool ret = true;
	unsigned char buffer[2]={0};
	int bufsize = 1;
	int size=0, size2=0;
	ret = true;
	while( !feof(fpIn))
	{
		size = fread(buffer, 1, 1, fpIn);
		size2 =  fwrite(buffer,1, 1, fpOut);
		//if(size ==0 || size!=size2){
		//	ret = false;
		//	break;
		//}
	}
	fclose(fpIn);
	fclose(fpOut);

	return ret;
}

//��ʽ����ͨͼƬ����
char* FileUtil::FormatFileName(const char *srcFilePath, int indexIn, bool fileTime, const char *destDir)
{
	if(srcFilePath==0)
		return NULL;
	char * newName = new char[512];
	try
	{
		long lTime;
		bool bFailed=false;
		CTime createtime;

		if(fileTime)
		{
			CFileStatus filestatus;
			CFile::GetStatus(srcFilePath,filestatus); //��ȡ�ļ���Ϣ
			createtime =filestatus.m_ctime;
		}else{
			createtime = CTime::GetCurrentTime();
		}
		char szTemp[512]={0};
		
		char extName[32]={0};
		sprintf(extName, "%s", strrchr(srcFilePath, '.'));
		if(destDir==0)
			memcpy(szTemp, srcFilePath, strrchr(srcFilePath, '\\')-srcFilePath);
		else
			sprintf(szTemp,"%s", destDir);
again:
		if(bFailed==false){
			sprintf(newName, "%s\\%s_location_index=%d%s", szTemp, createtime.Format("%Y%m%d%H%M%S") ,indexIn, extName);
		}else{
			lTime = getCurrentTime();
			sprintf(newName, "%s\\%s_location_index=%d_time=%d%s", szTemp, createtime.Format("%Y%m%d%H%M%S") ,indexIn, lTime, extName);
		}
		//CFile::Rename(path, newName);
		if(destDir==0){
			if(rename(srcFilePath, newName)==0){
				debug("newNamed : %s  of  %s", newName, srcFilePath);
			}
			else{
				debug("newName failed : %s  of  %s", newName, srcFilePath);
				bFailed = true;
				goto again;
			}
		}else{
			if(copyfile(srcFilePath, newName)){
				debug("newNamed : %s  of  %s", newName, srcFilePath);
			}
			else{
				debug("newName failed : %s  of  %s", newName, srcFilePath);
				bFailed = true;
				goto again;
			}
		}
	}
	catch( CFileException* e ){
		printf( "ERROR: %d File", e->m_cause);  
		delete newName;
		newName = 0;
		return 0;
	}
	return newName;
}



char *GetDateTime(char *timeString)
{
	struct tm *tmt = 0;
	time_t t = time(0);
	tmt = localtime(&t);
	try
	{
	if(timeString==0)
		timeString = new char[128];
	memset(timeString, 0, 128);
	sprintf(timeString,"%4d-%02d-%02d  %02d:%02d:%02d", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec );
	}catch(...)
	{
		return 0;
	}
	return timeString;
}


void __cdecl release(const char *format, ...)
{
	return ;

	char buf[4096]={0}, *p=buf;

	char* releaseTemp = new char[1024];
	if(GetDateTime(releaseTemp)==0)
		return;
	sprintf(p,"%s ",releaseTemp);
	delete releaseTemp;

	p += strlen(p);

    va_list args;
    va_start(args, format);
    p += _vsnprintf(p, sizeof buf - 1, format, args);
    va_end(args);
  // while ( p > buf && isspace(p[-1]) )    *--p = '\0';
    *p++ = '\r';
    *p++ = '\n';
    *p = '\0';

	printf(buf);
    OutputDebugString(buf);
}

void __cdecl debug(const char *format, ...)
{
	return ;

	char buf[4096]={0}, *p=buf;
	
	char *t = GetDateTime();
	sprintf(p,"%s ",t);
	p += strlen(p);

    va_list args;
    va_start(args, format);
    p += _vsnprintf(p, sizeof buf - 1, format, args);
    va_end(args);
  // while ( p > buf && isspace(p[-1]) )    *--p = '\0';
    *p++ = '\r';
    *p++ = '\n';
    *p = '\0';

    OutputDebugString(buf);
}





#include "time.h"
long getCurrentTime()    
{    
	SYSTEMTIME st;
	//GetSystemTime(&currentTime);
	GetLocalTime(&st);

   tm temptm = {st.wSecond, 
                     st.wMinute, 
                     st.wHour, 
                     st.wDay, 
                     st.wMonth - 1, 
                     st.wYear - 1900, 
                     st.wDayOfWeek, 
                     0, 
                     0};

	return mktime(&temptm)*1000 + st.wMilliseconds;
   //struct timeval tv;    
   //gettimeofday(&tv,NULL);    
   //return tv.tv_sec * 1000 + tv.tv_usec / 1000;    
}