////SearchFile.hpp
/********************************************************************
	SearchFile
	created:	2013/01/29
	author:		LX
	purpose:	This file is for SearchFile function
*********************************************************************/
#if !defined SearchFile_hpp__LX_2013_1_29
#define SearchFile_hpp__LX_2013_1_29
#include <string.h>
#include <stdio.h>

#define MAX_FILEPATH		512//
#define MAX_NAME			100
#define MAX_EXT				100
#define MAX_PARTSIZE		100
#define MAX_PARTNUM			20

#define MB					1024*1024
#define TRUE				1
#define FALSE				0
#define NULL				0

#ifdef WIN32
#include <windows.h>
#include <Winsock.h>
#pragma comment(lib,"Ws2_32.lib") 
#pragma message("Automatically linking with Ws2_32.lib") 

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

#define _stricmp	strcasecmp

#define _gethostname	gethostname	
typedef 	unsigned int	HANDLE		;
typedef 	int				BOOL		;
typedef 	const char*     LPCSTR		;
typedef 	unsigned char	BYTE		;
typedef 	unsigned int	RGBQUAD		;
typedef 	unsigned short	WORD		;
typedef 	unsigned int	DWORD		;
typedef 	unsigned int	UINT		;
typedef 	unsigned long	LONGLONG	;

#endif

#ifndef _strlwr
#define _strlwr
inline char* strlwr( char *str ){
	char *orig = str; char d = 'a'-'A';
	for( ;*str!='\0';str++ ){ if ( *str>='A' && *str<='Z' ) *str = *str+d; }
	return orig;
}
#endif
#ifndef _strupr
#define _strupr
inline char* strupr( char *str ){
	char *orig = str; char d = 'a'-'A';
	for( ;*str!='\0';str++ ){ if ( *str>='a' && *str<='z' ) *str = *str-d; }
	return orig;
}
#endif

inline void Add_PathSym(char* lpStrPath){//在路径后加"/"
#ifdef WIN32
	strcat(lpStrPath,"\\");
#else
	strcat(lpStrPath,"/");
#endif
}
#ifndef _tagDivNam
#define _tagDivNam
typedef struct tagDivNam{
	char strPart[MAX_PARTNUM*MAX_PARTSIZE];
	int partNum;
	char divSym;
}DivNam;
#else
#pragma message("APGSFile.hpp, Warning: DivNam alread define, be sure it was define as: struct tagDivNam{  }. ") 
#endif

inline void SplitName(DivNam* dN,const char* lpstrNam,char s){
	char strNam[MAX_FILEPATH];strcpy(strNam,lpstrNam);
	memset(dN,0,sizeof(DivNam));
	if(*strNam == '\0') return;
	dN->divSym = s;
	int partNum = 0;
	char* pPart = dN->strPart;
	char* pS = strNam;
	char* pOld = strNam;int l=0;
	
	while(*pS){
		if(*pS!=s) l++;
		else{
			if(!l) continue;
			memcpy(pPart,pOld,l*sizeof(char));
			pPart = pPart+MAX_PARTSIZE;
			partNum++;
			l=0;pOld = pS+1;if(*pOld=='\0') break;
		}
		pS++;
	}
	memcpy(pPart,pOld,l*sizeof(char));
	partNum++;
	dN->partNum = partNum;
}
inline void GetPartNam(char* pStr,DivNam* dN,int i){
	if(i<0) i=0;else if(i>dN->partNum) i = dN->partNum;
	strcpy(pStr,dN->strPart+i*MAX_PARTSIZE);
}
class CSrchFl
{
public:
	CSrchFl() {
		InitDat();
	}
	virtual ~CSrchFl(){
		ReleaseDat();
	}
	CSrchFl(char* lpOrigPN){
		Open_KNL(lpOrigPN);
	}
	int InitSchExt(const char* pStrExt,bool bIgCaps = true);
	int GetSchExtNum() {return m_nSchExtNum;}
	inline bool GetSchFullName(int i,char* lpFullName) {if(!m_nSchExtNum) {*lpFullName='\0';return false;}
	if(i<0) i =0; else if(i>m_nSchExtNum) i = m_nSchExtNum;
	strcpy(lpFullName,m_pStrFile+*(m_pSchExtID+i)*MAX_FILEPATH);return true;}
	inline bool GetSchName(int i,char* lpSchName);
	inline bool GetSchFullPath(int i,char* lpSchName);
	int GetFileNum(){return m_nFileNum;}//获取原始路径下的文件个数
	int GetDirNum(){return m_nDirNum;}//获取原始路径下的文件夹个数
	int GetTotalNum(){return (m_nFileNum+m_nDirNum);}//获取原始路径下的文件总个数//包含文件和文件夹
	void GetOrignDir(char* lpstrDir){strcpy(lpstrDir,m_strOrignPath);}//获取原始路径
	void GetOrignDirNam(char* lpstrDir){strcpy(lpstrDir,m_strOrignNam);}//获取原始路径的名字
	void GetSrchExt(char* lpstrExt){strcpy(lpstrExt,m_strSrchExt);}
	inline bool GetFileName(int i,char* lpStrFileName){if(!m_nFileNum) {*lpStrFileName='\0';return false;}//获取第i个文件的名字
	if(i<0) i =0; else if(i>m_nFileNum) i = m_nFileNum;strcpy(lpStrFileName,m_pStrFile+i*MAX_FILEPATH);return true;}
	inline bool GetDirName(int i,char* lpStrDirName){if(!m_nDirNum) {*lpStrDirName='\0';return false;}//获取第i个文件夹的名字
	if(i<0) i =0; else if(i>m_nDirNum) i = m_nDirNum;strcpy(lpStrDirName,m_pStrDir+i*MAX_FILEPATH);return true;}
	inline bool GetFilePath(int i,char* lpStrFP);//获取第i个文件路径
	inline bool GetDirPath(int i,char* lpStrDP);//获取第i个文件夹路径
	bool Open(const char* lpOrigPN){//lpOrigPN文件路径//获取该文件夹路径下的文件和文件夹路径
		ReleaseDat();
		return Open_KNL(lpOrigPN);
	}
protected:
	bool InitDat(){//初始化变量成员
		m_nFileNum = m_nDirNum = m_nSchExtNum=0;memset(m_strOrignPath,0,MAX_FILEPATH);
		memset(m_strOrignNam,0,MAX_FILEPATH);memset(m_strSrchExt,0,MAX_EXT);
		m_pStrFile = m_pStrDir = m_pStrExt = NULL;
		 m_pSchExtID = NULL;
		 return true;
	}
	bool ReleaseDat(){//释放成员指针所分配的空间
		if(m_pStrFile) delete[] m_pStrFile;m_pStrFile = NULL;
		if(m_pStrDir) delete[] m_pStrDir;m_pStrDir = NULL;
		if(m_pStrExt) delete[] m_pStrExt;m_pStrExt = NULL;
		if (m_pSchExtID) delete[] m_pSchExtID;m_pSchExtID = NULL;
		return true;
	}
	bool Init();//获取输入路径下的文件数量和文件夹数量//并为其分配空间
	bool load();//获取输入路径下的文件名和文件夹名
	bool Open_KNL(const char* lpOrigPN){//lpOrigPN文件路径//获取该文件夹路径下的文件和文件夹路径
		InitDat();//初始化为0
		if(*lpOrigPN == '\0') return false;//路径为空跳出
		strcpy(m_strOrignPath,lpOrigPN);//m_strOrignPath存储输入的路径
		char* pS = m_strOrignPath + strlen(m_strOrignPath)-1;//pS指向m_strOrignPath的末尾
		if(*pS == '\\'||*pS == '/') *pS = '\0';//???

		pS	= strrchr(m_strOrignPath,'/');if(!pS)  pS = strrchr(m_strOrignPath,'\\');//pS指向m_strOrignPath的末尾
		if(*(pS+1)=='\0') return false;
		strcpy(m_strOrignNam,pS+1);
		if(Init())//找出文件夹个数//文件个数（根目录下）
			return load();//找出文件夹的名字//文件名字
		return false;
	}
protected:
	char m_strOrignPath[MAX_FILEPATH];
	char m_strOrignNam[MAX_FILEPATH];
	char m_strSrchExt[MAX_EXT];
	char* m_pStrFile;
	char* m_pStrDir;
	char* m_pStrExt;
	int m_nFileNum;
	int m_nDirNum;

	int m_nSchExtNum;
	int* m_pSchExtID;
};
inline bool CSrchFl::GetDirPath(int i,char* lpStrDP){
	char strNam[MAX_FILEPATH];
	if(!GetDirName(i,strNam)){
		*lpStrDP = '\0';return false;
	}
	strcpy(lpStrDP,m_strOrignPath);
	Add_PathSym(lpStrDP);//在路径后加"/"
	strcat(lpStrDP,strNam);
	return true;
}
inline bool CSrchFl::GetFilePath(int i,char* lpStrFP){
	char strNam[MAX_FILEPATH];
	if(!GetFileName(i,strNam)){
		*lpStrFP = '\0';return false;
	}
	strcpy(lpStrFP,m_strOrignPath);
	Add_PathSym(lpStrFP);
	strcat(lpStrFP,strNam);
	return true;
}

bool CSrchFl::Init()
{
	if(*m_strOrignPath == '\0') return false;
	int fileNum=0,dirNum=0;
#ifdef WIN32
	char strPath[MAX_FILEPATH];strcpy(strPath,m_strOrignPath);
	strcat( strPath,"\\*.*" );
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(strPath,&fd);
	if ( hFind!=INVALID_HANDLE_VALUE ){ 
		while( FindNextFile(hFind, &fd) ){
			if(!strcmp(fd.cFileName,"..")||!strcmp(fd.cFileName,".")) continue;
			if ( (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY )
			{
				dirNum++;
			}
			else fileNum++;
		}
	}
	::FindClose(hFind);	
#else
	DIR* pDir = NULL;
	struct dirent* pDirent = NULL;
	pDir = opendir(m_strOrignPath);
	if (pDir == NULL)
	{
		printf ("Invalid File Handle");
		return false;
	}
	else
	{
		for(pDirent=readdir(pDir);pDirent!=NULL;pDirent=readdir(pDir))
		{
			if(strcmp(pDirent->d_name,".") == 0||strcmp(pDirent->d_name,"..") == 0) continue;
			if (pDirent->d_type == 4)
			{
				dirNum++;
			}
			else fileNum++;
		}
	}
	closedir(pDir);
#endif
	m_nDirNum = dirNum;m_nFileNum = fileNum;
	if(fileNum) {
		m_pStrFile = new char[fileNum*MAX_FILEPATH];memset(m_pStrFile,0,sizeof(char)*fileNum*MAX_FILEPATH);
		m_pStrExt = new char[fileNum*MAX_EXT];memset(m_pStrExt,0,sizeof(char)*fileNum*MAX_EXT);
		m_pSchExtID = new int[fileNum];memset(m_pSchExtID,0,sizeof(int)*fileNum);
	}
	if(dirNum) {m_pStrDir = new char[dirNum*MAX_FILEPATH];memset(m_pStrDir,0,sizeof(char)*dirNum*MAX_FILEPATH);}
	return ((fileNum+dirNum>0)?true:false);
}
bool CSrchFl::load(){
	char* pDirPN,*pFilePN,*pExt,*pS;
	pDirPN = m_pStrDir;pFilePN = m_pStrFile;pExt = m_pStrExt;
#ifdef WIN32
	char strPath[MAX_FILEPATH];strcpy(strPath,m_strOrignPath);
	strcat( strPath,"\\*.*" );
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(strPath,&fd);//	int cnt = 0;
	if ( hFind!=INVALID_HANDLE_VALUE ){ 
		while( FindNextFile(hFind, &fd) ){
			if(!strcmp(fd.cFileName,"..")||!strcmp(fd.cFileName,".")) continue;
			if ( (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY )
			{
// 				strcpy(pDirPN+cnt*MAX_FILEPATH,fd.cFileName);
// 				cnt++;
				strcpy(pDirPN,fd.cFileName);
				pDirPN = pDirPN+MAX_FILEPATH;
			}
			else {
				strcpy(pFilePN,fd.cFileName);
				pS = strrchr(pFilePN,'.');
				if(pS) strcpy(pExt,pS+1);

				pFilePN = pFilePN+MAX_FILEPATH;
				pExt = pExt+MAX_EXT;
			}
		}
	}
	::FindClose(hFind);	
#else
	DIR* pDir = NULL;
	struct dirent* pDirent = NULL;
	pDir = opendir(m_strOrignPath);
	if (pDir == NULL)
	{
		printf ("Invalid File Handle");
		return false;
	}
	else
	{
		for(pDirent=readdir(pDir);pDirent!=NULL;pDirent=readdir(pDir))
		{
			if(strcmp(pDirent->d_name,".") == 0||strcmp(pDirent->d_name,"..") == 0) continue;
			if (pDirent->d_type == 4)
			{
				strcpy(pDirPN,pDirent->d_name);
				pDirPN = pDirPN+MAX_FILEPATH;
			}
			else {
				strcpy(pFilePN,pDirent->d_name);
				pS = strrchr(pFilePN,'.');if(pS) strcpy(pExt,pS+1);
				else *pExt = '\0';
				pFilePN = pFilePN+MAX_FILEPATH;
				pExt = pExt+MAX_EXT;
			}
		}
	}
	closedir(pDir);
#endif
	return true;
}

int CSrchFl::InitSchExt(const char* pStrExt,bool bIgCaps)
{
	char strExt[MAX_EXT];
	strcpy(strExt,pStrExt);
	if(*strExt == '\0'||m_nFileNum == 0) return 0;
	if(!strcmp(strExt,m_strSrchExt)) return m_nSchExtNum;
	char* pS;pS = m_pStrExt;
	int* pId;pId = m_pSchExtID;
	int num = 0;
	if(!bIgCaps){
		for (int i=0;i<m_nFileNum;i++,pS=pS+MAX_EXT)
		{
			if(!strcmp(strExt,pS)){ 
				*pId = i;pId++;
				num++;
			}
		}
	}
	else{
		for (int i=0;i<m_nFileNum;i++,pS=pS+MAX_EXT)
		{
			if (!_stricmp(strExt, pS)){
				*pId = i;pId++;
				num++;
			}
		}
	}
	m_nSchExtNum = num;
	return num;
}
inline bool CSrchFl::GetSchName(int i,char* lpSchName)
{
	char strNam[MAX_FILEPATH];
	if(!GetSchFullName(i,strNam)){
		*lpSchName = '\0';
		return false;
	}
	char* pS = strrchr(strNam,'.');*pS = '\0';
	strcpy(lpSchName,strNam);
	return true;
}
inline bool CSrchFl::GetSchFullPath(int i,char* lpSchName)
{
	char strNam[MAX_FILEPATH];
	if(!GetSchFullName(i,strNam)){
		*lpSchName = '\0';
		return false;
	}
	strcpy(lpSchName,m_strOrignPath);
	Add_PathSym(lpSchName);
	strcat(lpSchName,strNam);
	return true;
}
#endif // SearchFile_hpp__LX_2013_1_29
