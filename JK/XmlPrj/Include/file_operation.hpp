////file_operation.hpp
/********************************************************************
file operation
created:	2014/06/14
author:		mlw
purpose:	This file is for file operation function
*********************************************************************/
#ifndef _WHU_FILE_OPERATION_MLW_20140614
#define _WHU_FILE_OPERATION_MLW_20140614
#define  MAX_FILEPATH 512
#include <stdio.h>
#include <string.h>
#include <vector>
#ifdef WIN32
#include <sys/stat.h >
#include <time.h>
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <netdb.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>
#endif // WIN32

namespace mlw
{
	void Unix2Dos(char *strCmd){
		int i, len = strlen(strCmd);
		for (i = 0; i < len; i++){ if (strCmd[i] == '/') strCmd[i] = '\\'; }
	}
	void Dos2Unix(char *strCmd){ int i, len = strlen(strCmd); for (i = 0; i < len; i++){ if (strCmd[i] == '\\') strCmd[i] = '/'; } }

	inline char* AddEndSlash(char *strPN){
		if (strlen(strPN) < 1) return NULL;
		char* pS = strPN + strlen(strPN) - 1;
		if (*pS == '/' || *pS == '\\') return pS;
		else if (strPN[1] != ':' && strPN[1] != '\\'){
			pS++;	*pS = '/'; *(pS + 1) = '\0';
		}
		else { pS++;	*pS = '\\'; *(pS + 1) = '\0'; }
		return pS;
	}
}


#ifndef _STRING
#define _STRING
typedef struct tagCHAR
{
	char str[512];
}STRING;
typedef std::vector<STRING> STRINGVec;
#endif

bool fSearchFile(char * dir, const char *suffix, STRINGVec& FilenameVec)
{
	if (*dir == '\0') return false;
	/*mlw::Dos2Unix(dir);
	char *pS = strrchr(dir,'/');
	if (*(pS + 1) == '\0')*(pS) = '\0';*/
	char strTemp[MAX_FILEPATH]; STRING strFilename;
#ifdef WIN32
	if (_chdir(dir) != 0){ printf("--ERROR-- Function _chdir() failed: %s\n", dir); return false; }//enter directory
	_finddata_t fileInfo;
	memset(&fileInfo, 0x0, sizeof(fileInfo));
	long hFile;
	if ((hFile = _findfirst(suffix, &fileInfo)) != -1L)
	{
		do
		{
			if (fileInfo.attrib != _A_SUBDIR)
			{
				sprintf(strFilename.str, "%s%s", dir,fileInfo.name);
				FilenameVec.push_back(strFilename);
			}
			else if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)//查找dir中的子目录//如果是,再检查是不是 . 或 .. 
			{
				sprintf(strTemp, "%s%s", dir, fileInfo.name);
				AddEndSlash(strTemp);
				fSearchFile(strTemp, suffix, FilenameVec);
			}
		} while (_findnext(hFile, &fileInfo) == 0);
		_findclose(hFile);
	}
	/*//查找dir中的子目录
	if (_chdir(dir) != 0){ printf("\terror: function _chdir() failed.\n"); return false; }//enter directory
	if ((hFile = _findfirst("*.*", &fileInfo)) != -1L)
	{
		do
		{
			if (fileInfo.attrib == _A_SUBDIR)//检查是不是目录
			{
				if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)//如果是,再检查是不是 . 或 .. 
				{
					sprintf(strTemp, "%s%s", dir, fileInfo.name);
					if (strTemp[strlen(strTemp)] != '/')sprintf(strTemp + strlen(strTemp), "%s", "/");
					fSearchFile(strTemp, suffix, FilenameVec);
				}
			}
		} while (_findnext(hFile, &fileInfo) == 0);
		_findclose(hFile);
	}*/
#else
	DIR* pDir = NULL;
	struct dirent* pDirent = NULL;
	pDir = opendir(dir);//打开一个目录
	if (pDir == NULL)
	{
	printf("Invalid File Handle");
	return false;
	}
	else
	{
	for (pDirent = readdir(pDir); pDirent != NULL; pDirent = readdir(pDir))
	{
	if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) continue;
	if (pDirent->d_type == 4)
	{
	sprintf(strTemp, "%s%s", dir, pDirent->d_name);
	if (strTemp[strlen(strTemp)] != '/')sprintf(strTemp + strlen(strTemp), "%s", "/");
	fSearchFile(strTemp, suffix, FilenameVec);
	}
	else {
	sprintf(strFilename.str, "%s%s", dir, pDirent->d_name);
	FilenameVec.push_back(strFilename);
	}
	}
	}
	closedir(pDir);
	#endif
	return true;
}
//
inline bool fCarryFile(const char* lpSrc, const char* lpDst){
	STRINGVec FilenameVec;
	if (!fSearchFile((char*)lpSrc, "*.*", FilenameVec))return false;//寻找指定的文件

	for (unsigned int i = 0; i<FilenameVec.size(); i++)
	{
		char strSrcFile[512], strDstFile[512];
		strcpy(strSrcFile, FilenameVec[i].str);
		mlw::Dos2Unix(strSrcFile);
		char *pS = strrchr(strSrcFile, '/');
		sprintf(strDstFile, "%s%s", lpDst, pS + 1);
		if (IsExist(strDstFile)) {
			if (filecmp_mtime(strSrcFile, strDstFile) > 0)	{
				remove(strDstFile);	CopyFile(strSrcFile, strDstFile,false);
			}
		}
		else CopyFile(strSrcFile, strDstFile, false);
	}
	return true;
}

/************************************************************************/
/*      note:get creation time of file <sys/stat.h > <time.h><string.h>                                             */
/*      [in]:  filename--a file that needs to query                                                                                   */
/*      [in]:  FileCreationTime--a string  of file creation time example: 2014-06-14 20:35           */
/*      [return value]:  true-get  creation time of file;false:get null of file                                         */
/*      [code example]:   tGetCreationTimeOfFile(strImageFile);                                                     */
/************************************************************************/
bool tGetCreationTimeOfFile(char*filename, char*FileCreationTime)
{
	struct stat filetime;
	int c = stat(filename, &filetime);
	if (c) { printf("\tGet time of file failed: %s\n", filename); strcpy(FileCreationTime, "null"); return false; }
	else{
		struct tm *pFileTime = localtime(&(filetime.st_mtime));
		char strTimeYear[512], strTimeMonth[512], strTimeDay[512], strTimeHour[512], strTimeMinute[512];
		sprintf(strTimeYear, "%d", pFileTime->tm_year + 1900);
		if (pFileTime->tm_mon + 1 < 10)sprintf(strTimeMonth, "0%d", pFileTime->tm_mon + 1);
		else sprintf(strTimeMonth, "%d", pFileTime->tm_mon + 1);
		if (pFileTime->tm_mday < 10)sprintf(strTimeDay, "0%d", pFileTime->tm_mday);
		else sprintf(strTimeDay, "%d", pFileTime->tm_mday);
		if (pFileTime->tm_hour < 10)sprintf(strTimeHour, "0%d", pFileTime->tm_hour);
		else sprintf(strTimeHour, "%d", pFileTime->tm_hour);
		if (pFileTime->tm_min < 10)sprintf(strTimeMinute, "0%d", pFileTime->tm_min);
		else sprintf(strTimeMinute, "%d", pFileTime->tm_min);
		sprintf(FileCreationTime, "%s-%s-%s %s:%s", strTimeYear, strTimeMonth, strTimeDay, strTimeHour, strTimeMinute);
		return true;
	}
}
/************************************************************************/
/*      To query whether a file exists                                                                                                     */
/*      [in]:  filename--a file that needs to query                                                                                  */
/*      [return value]:true:file exists;false:file does not exist                                                             */
/*      [code example]:   fFileIsExist(strImageFile);                                                                         */
/************************************************************************/
bool fFileIsExist(char *filename){ 
#ifdef  WIN32
	return ((_access(filename, 0) == 0) ? true : false); 
#else
	return false;
#endif
}
/************************************************************************/
/*      Rewrite the filename suffix                                                                                                         */
/*      [in]:  filename--a file that needs to rewrite                                                                                */
/*      [in]:  suffix--example:".xml"(must add charcteristic '.')                                                            */
/*      [return value]:   a string  of new file name of new suffix                                                          */
/*      [code example]:   fRewriteFileSuffix(filename,".xml") ;                                                          */
/************************************************************************/
char*fRewriteFileSuffix(char*filename, char *suffix){ char*pS = strrchr(filename, '.'); sprintf(pS, "%s", suffix); return filename; }
/************************************************************************/
/*      Create Directory                                                                                                         */
/*      [in]:  strDir--path name                                                                                */
/*      [return value]:true:create success;false:create fail                                                             */
/*      [code example]:   fCreatePath(strPath) ;                                                          */
/************************************************************************/
bool fCreateDir(char*strDir)
{
#ifdef WIN32
	if (!fFileIsExist(strDir))
	{
		if (_mkdir(strDir) == 0)
			return true;
		else
		{
			printf("ERROR: Directory created failed: %s.\n", strDir); return false;
		}
	}
	else
	{
		printf("Warnning: Directory existed: %s.\n", strDir); return true;
	}
#else
	return false;
#endif
}
#endif