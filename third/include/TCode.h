
//TCode.h

#ifndef __TCode_H 
#define __TCode_H 

#include <WinSock2.h>
#include <string> 
 
 
using namespace std; 

//UTF-8��GB2312��ת��
char* U2G(const char* utf8);

//GB2312��UTF-8��ת��
char* G2U(const char* gb2312);

//�ַ���������ң��� strstr�෴
char* strrstr(char*source ,const char* match);
 
#endif 