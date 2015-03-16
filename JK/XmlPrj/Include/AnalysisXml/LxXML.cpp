#include <stdio.h>
#include <string.h>
#include "LxXML.h"
#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////////////////////
//CXmlFile
IMPLEMENT_DYNARCH(CXmlFile,CArchFile)


int CXmlFile::Load4File(LPCSTR lpstrPath)
{
	return CArchFile::Load4File(lpstrPath);
}

IMPLEMENT_DYNARCH(CPrjXml,CXmlFile)

CPrjXml::CPrjXml()
{
	memset(&m_hdr,0,sizeof(PrjHdr));
	memset(&m_prodInfo,0,sizeof(ProdInfo));
	m_nBlockID = 0;
	m_tskTypeList = m_tskTypeCur = 0;

	memset(m_strProcessSpace,0,sizeof(m_strProcessSpace));
	memset(m_strProductSpace,0,sizeof(m_strProductSpace));

	memset(&m_msgPort,0,sizeof(MessagePort));
}

CPrjXml::~CPrjXml()
{
	Reset();	
}

void CPrjXml::Reset()
{
	memset(&m_hdr,0,sizeof(PrjHdr));
	memset(&m_prodInfo,0,sizeof(ProdInfo));
	m_scList.Reset(NULL,0);
	m_import.Reset(NULL,0);	m_importEx.Reset(NULL,0);
	m_export.Reset(NULL,0);	m_exportEx.Reset(NULL,0);
}

bool CPrjXml::Open(LPCSTR lpstrPathName)
{
	Reset();
	if( !CXmlFile::Open(lpstrPathName) ) return false;
	return true;
}

int CPrjXml::Load4File(LPCSTR lpstrPath)
{
	char strF[512];	strcpy(strF,lpstrPath);	Dos2Unix(strF);
	*strrchr(strF,'/') = 0;

	if( !m_strProcessSpace[0] ) strcpy(m_strProcessSpace,strF);
	if( !m_strProductSpace[0] ) strcpy(m_strProductSpace,strF);

	return CXmlFile::Load4File(lpstrPath);
}

void CPrjXml::SetTskList(int tskType)
{
	if( !(tskType&TSK_END) ) m_tskTypeList = tskType;
	else{
		int nTmp=0;
		for (int i=1; i<TTID_END;i++){
			nTmp += 1;
			nTmp <<= 1;
		}
		m_tskTypeList = nTmp;
	}
}

int CPrjXml::Save(LPCSTR lpstrPath)
{
	LogPrint(0,"No Code to Save xml File!");

	return ERR_NONE;
}

int CPrjXml::CreateSymbolicLink4Import(int lengLimit /* = 256 */)
{
	char strConf[512];	::GetPrivateProfilePath(strConf,CONFIG_FILE);
	char strPathCap[100];
	::GetPrivateProfileString("PathSetting","linuxpath","",strPathCap,100,strConf);
	if ( !IsDir(strPathCap) ) {
		::GetPrivateProfileString("PathSetting","windowspath","",strPathCap,100,strConf);
		if( !IsDir(strPathCap) ) strcpy(strPathCap,"");
	}

	char strLink[512];	strcpy(strLink,strPathCap);		AddEndSlash(strLink);	
	
	srand(time(0));	
	int i,sz;
	ImageInfo* pInfo = m_import.GetData(sz);
	
	if (sz>0 && strlen(strLink)<1 && pInfo->path[1] == ':'){
		memcpy(strLink,pInfo->path,3*sizeof(char));
	}
	char* pLnk = strLink+strlen(strLink);

	int nLinkNum = 0;
	for (i=0; i<sz; i++,pInfo++){
		if ( strlen(pInfo->path)<lengLimit ) continue;
		int mkey=rand();
		sprintf(pLnk,"%d",mkey);
		while(1) { if( IsExist(strLink) ) strcat(strLink,"a");else break; }
		char* pS = strrpath(pInfo->path);	char c = *pS;	*pS = 0;
//		if( !::CreateLink(pInfo->path,strLink) ) continue;	*pS = c;
		::CreateLink(pInfo->path,strLink);	*pS = c;
		sprintf(pInfo->path,"%s%s",strLink,pS);
		nLinkNum++;
	}

	return nLinkNum;
	
}

int CPrjXml::ReleaseSymbolicLink4Import()
{
	int sz,i;
	ImageInfo* pInfo = m_import.GetData(sz);
	char strImagePath[512];

	int nLinkNum = 0;
	for (i=0; i<sz; i++,pInfo++){
		strcpy(strImagePath,pInfo->path);	char* pS = strrpath(strImagePath);
		if(!pS) continue;	char c = *pS;	*pS = 0;
		char strPath[512];	memset(strPath,0,512);
		if( readlink(strImagePath,strPath,512)<0 ) continue;
		RemoveLink(strImagePath);
		*pS = c;	sprintf(pInfo->path,"%s%s",strPath,pS);
		nLinkNum++;
	}

	return nLinkNum;
}
/////////////////////////////////////////////////////////////////
#define _NODEVAL_LENGTH	20

#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )




