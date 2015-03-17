// ARCH.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XmlPrj.h"
#include "LxXML.h"
#include "PrjLog.hpp"
#include "ZY3FileName.h"
#include "./meta/xml_epi_w.h"
#include "./meta/xml_dsm_w.h"
#include "./meta/xml_dem_w.h"
#include "SearchFile.hpp"
//Crete dir Create meta

#define MIP

void Usage(){
	printf(
		"%s",
		"Usage: Arch [-l task] [-i ProjectPath] \n"
		"Opt:\n"
		" -l: AtMch TieMch AtAdj ImgEpi DemQMch DemFlt ImgRec ImgFus ImgMxz DemMap DomMap Order \n"
		" -i: the path of Project File.\n"
		);
	exit(ERR_EXE_NOTARGV);
}

#define WGS84A		6378137.0		    /* Semi-major axis of ellipsoid in meters  WGS84 */
#define WGS84B		6356752.31424517929	/* Semi-minor axis of ellipsoid            WGS84 */
#define PI          3.1415926535
#define EMIN_CHINA	73.55
#define EMAX_CHINA	135.08
#define NMIN_CHINA	20.5
#define NMAX_CHINA	53.55

inline int GetSiblingNode(CMarkup* pNode, const char* pszNode, char* pszText = NULL){
	if (!pNode) return RET_ERR;

	MCD_STR str;
	if (pNode->FindElem(pszNode))
	{
		if (pszText) { str = pNode->GetData(); strcpy(pszText, str.c_str()); }
		return RET_OK;
	}
	else
	{
		pNode->ResetMainPos();//
		if (pNode->FindElem(pszNode)) {
			if (pszText) { str = pNode->GetData(); strcpy(pszText, str.c_str()); }
			return RET_OK;
		}
		else{
			printf("<%s> is not exist!\n", pszNode);
			return RET_ERR;
		}
	}
	return RET_ERR;
}

inline bool CarryFile(const char* lpSrc, const char* lpDst){
	if (!IsExist(lpSrc)) { printf("%s is not Exist!\n", lpSrc); return false; }
	if (IsExist(lpDst)) { if (filecmp_mtime(lpSrc, lpDst) > 0)	{ remove(lpDst);	rename(lpSrc, lpDst); } }
	else rename(lpSrc, lpDst);
	return true;
}

bool	CarryImgFile(const char* lpSrc, ImgInfo* lpDst, bool bShp = true){
	if (!IsExist(lpSrc) || !lpDst) return false;
	char strSrcPath[512], strDstPath[512], strF[512], strFSrc[512], c;
	strcpy(strF, lpSrc);	char* pF = strrpath(strF);

	strcpy(strSrcPath, lpSrc);
	strcpy(strDstPath, lpDst->sImgPN);
	char* pS = strrpath(strDstPath);	 c = *pS;	*pS = 0;	CreateDir(strDstPath);	*pS = c;
	pS = strrchr(strSrcPath, '.');	char* pT = strSrcPath + strlen(strSrcPath);
	if (!CarryFile(strSrcPath, strDstPath)) { LogPrint(ERR_NONE, "Fail to Create %s!", strDstPath); return false; }//1 carry image//if not found, return false
	LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);

	strcpy(pT, ".RPC");
	if (!IsExist(strSrcPath))	strcpy(pT, ".rpc");
	if (!IsExist(strSrcPath))	{
		strcpy(pS, ".tfw");
	}	//TransforTFW(strSrcPath);

	strcpy(strDstPath, lpDst->sAdjRPCPath);
	if (!CarryFile(strSrcPath, strDstPath))  LogPrint(ERR_NONE, "Fail to Create %s!", strDstPath);
	else LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);

	strcpy(pS, "_PRE.JPG");
	if (!IsExist(strSrcPath))
	{
		strcpy(pS, "_pre.jpg");
		if (!IsExist(strSrcPath)){
			strcpy(pF, strrpath(lpDst->sBrowsePN));
			strcpy(strFSrc, strF);
		}
		else strcpy(strFSrc, strSrcPath);
	}
	else strcpy(strFSrc, strSrcPath);

	strcpy(strDstPath, lpDst->sBrowsePN);
	if (!CarryFile(strFSrc, strDstPath)) LogPrint(ERR_NONE, "Fail to Create %s!", strDstPath);
	else LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);

	strcpy(pS, "_ICO.JPG");
	if (!IsExist(strSrcPath))
	{
		strcpy(pS, "_ico.jpg");
		if (!IsExist(strSrcPath)){
			strcpy(pF, strrpath(lpDst->sThumbPN));
			strcpy(strFSrc, strF);
		}
		else strcpy(strFSrc, strSrcPath);
	}
	else strcpy(strFSrc, strSrcPath);

	strcpy(strDstPath, lpDst->sThumbPN);
	if (!CarryFile(strFSrc, strDstPath)) LogPrint(ERR_NONE, "Fail to Create  %s!", strDstPath);
	else LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);

	strcpy(pS, ".xml");
	if (!IsExist(strSrcPath))	strcpy(pS, ".XML");

	strcpy(strDstPath, lpDst->sMetaPN);
	if (!CarryFile(strSrcPath, strDstPath)) LogPrint(ERR_NONE, "Fail to Create  %s!", strDstPath);
	else LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);

	strcpy(pS, "_range.txt");
	if (!IsExist(strSrcPath))	strcpy(pS, "_range.txt");

	strcpy(strDstPath, lpDst->sImgPN);	strcpy(strrchr(strDstPath, '.'), "_range.txt");
	CarryFile(strSrcPath, strDstPath);

	if (!bShp) return true;

	char strGeoRange[512], strFileName[256];
	strcpy(strGeoRange, lpDst->sImgPN); Dos2Unix(strGeoRange);
	char* pPath = strrchr(strGeoRange, '/');
	strcpy(strFileName, pPath + 1);	strcpy(pPath, "/GeoRange/");	CreateDir(strGeoRange);	strcat(strGeoRange, strFileName);
	pT = strrchr(strGeoRange, '.');

	strcpy(pS, ".shp");
	if (!IsExist(strSrcPath))	strcpy(pS, ".SHP");
	strcpy(pT, ".shp");
	//	CarryFile(strSrcPath,strGeoRange);
	CopyFile(strSrcPath, strGeoRange, TRUE);

	strcpy(pS, ".dbf");
	if (!IsExist(strSrcPath))	strcpy(pS, ".DBF");
	strcpy(pT, ".dbf");
	//	CarryFile(strSrcPath,strGeoRange);
	CopyFile(strSrcPath, strGeoRange, TRUE);

	strcpy(pS, ".shx");
	if (!IsExist(strSrcPath))	strcpy(pS, ".SHX");
	strcpy(pT, ".shx");
	//	CarryFile(strSrcPath,strGeoRange);
	CopyFile(strSrcPath, strGeoRange, TRUE);

	strcpy(pS, ".prj");
	if (!IsExist(strSrcPath))	strcpy(pS, ".PRJ");
	strcpy(pT, ".prj");
	//	CarryFile(strSrcPath,strGeoRange);
	CopyFile(strSrcPath, strGeoRange, TRUE);

	return true;
}

inline void cpmeta(ImgInfo* pSrc, ImgInfo* pTarget, LPCSTR lpXml)
{
	if (!pSrc || !pTarget) return;
	CZY3SceneFile* pFile;
	pFile = (CZY3SceneFile*)LxLoadFile(GET_LOBINFO(CZY3SceneFile), pTarget->sImgPN);
	if (pFile){
		char strPath[512];
		pFile->GetMetaPath(strPath);	LogPrint(ERR_NONE, "Write Meta File:%s\n", strPath);
		pFile->GenerateMetaFile(lpXml, pSrc->sMetaPN, strPath);
		delete pFile;
	}
}

void WriteRange(const char* lpstrImgPath)
{
	char strPath[512];	strcpy(strPath, lpstrImgPath);	strcpy(strrchr(strPath, '.'), "_range.txt");
	FILE* fp = fopen(strPath, "w");	if (!fp) return;
	fprintf(fp, "391.350159 2403.851563\n2390.621826 2005.420166\n1977.960815 2.591011 \n0.033614 411.694672");
	fclose(fp);

}

class CProdXml :public CMarkup
{
public:
	enum MODERET { modeSuccess = 0x0000, modeFail = 0x0001, modeHalf = 0x0001 };//modeHalf=0x0002
public:
	CProdXml(){ m_pXml = NULL; m_nInToalNum = 0; Reset(); }
	~CProdXml() {  }
	bool	Open(const char* lpstr, CPrjXml* pXml)	{
		CMarkup xml;
		{
			char strxmlPath[512];	strcpy(strxmlPath, lpstr);
			if (!xml.Load(strxmlPath))
			{
				m_nSucess = modeFail;
				strcpy(m_strMsg, xml.GetError().c_str());
				LogPrint(ERR_FILE_OPEN, "Error:%s\t%s", lpstr, m_strMsg);
			}
			else {
				pXml = (CPrjXml*)LxLoadFile(GET_LOBINFO(CPrjXml), strxmlPath);
				if (!pXml)
				{
					m_nSucess = modeFail;
					sprintf(m_strMsg, "Error:Fail to Analysis the Xml File:%s!", strxmlPath);
					LogPrint(ERR_FILE_OPEN, "%s", m_strMsg);
				}
				m_pXml = pXml;
			}
		}
/*		                                                             Start---- Write : "complete xml" frame                                                                             */
		SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
		/*
					<root>
					<head>
					<MessageType>MessageType< / MessageType>
					<MessageID>MessageID< / MessageID>
					<OriginatorAddress>OriginatorAddress< / OriginatorAddress>
					<RecipientAddress>MIP< / RecipientAddress>
					<CreationTime>2015 - 03 - 16T22:17 : 12 < / CreationTime >
					< / head>
					</root>
*/
		GetSiblingNode(&xml, "root");		AddElem("root");
		xml.IntoElem(); /*xml: into <root> Node*/	IntoElem();//into <root> Node
		GetSiblingNode(&xml, "head");		AddElem("head");
		xml.IntoElem();	 /*xml: into <head> Node*/	IntoElem();//into <head> Node
		GetSiblingNode(&xml, "MessageType");		AddElem("MessageType", xml.GetData());
		GetSiblingNode(&xml, "MessageID");			AddElem("MessageID", xml.GetData());
		GetSiblingNode(&xml, "OriginatorAddress");	AddElem("OriginatorAddress", xml.GetData());
		GetSiblingNode(&xml, "RecipientAddress");	AddElem("RecipientAddress", xml.GetData());
		SYSTEMTIME	st; GetLocalTime(&st); char strMsg[256];
		sprintf(strMsg, "%d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		GetSiblingNode(&xml, "CreationTime");		AddElem("CreationTime", strMsg);
		xml.OutOfElem();/*xml: out <head> Node.*/			OutOfElem();//out <head> Node.
		/*
				<content>
				<TaskID>T150127006001< / TaskID>
				<JobID>J15012700065< / JobID>
				<FinishedTaskItemNum>1 < / FinishedTaskItemNum >
				<TaskItemFinishedStateList>
				<TaskItemFinishedState>
				<TaskItemID>J15012700065< / TaskItemID>
				<TaskItemType>BlockAdjustment< / TaskItemType>
				<FinishedStatus>NotComplete< / FinishedStatus>
				<FinishedStatusDisc>NotComplete< / FinishedStatusDisc>
				<ProductInfoList>
				<Type>RPC< / Type>
				<Level / >
				<OrganizationMode>Scene< / OrganizationMode>
				<ProductInfo/>
				< / ProductInfoList>
				<TemporaryResult>
				<TemporaryDataPath / >
				<IsExportTemporaryData>FALSE< / IsExportTemporaryData>
				< / TemporaryResult>
				< / TaskItemFinishedState>
				< / TaskItemFinishedStateList>
				< / content>
				*/
		GetSiblingNode(&xml, "content");	AddElem("content");
		xml.IntoElem();	 /*xml: into <content> Node*/			IntoElem();//into <content> Node
		GetSiblingNode(&xml, "TaskID");		AddElem("TaskID", xml.GetData());
		GetSiblingNode(&xml, "JobID");		AddElem("JobID", xml.GetData());
		GetSiblingNode(&xml, "ProductiveTaskItemNum");      AddElem("FinishedTaskItemNum", xml.GetData());
		GetSiblingNode(&xml, "ProductiveTaskItemList");     AddElem("TaskItemFinishedStateList");
		xml.IntoElem();  /*xml: into <ProductiveTaskItemList> Node*/     IntoElem();//into <TaskItemFinishedStateList> Node
		{
			GetSiblingNode(&xml, "ProductiveTaskItemInfo");   AddElem("TaskItemFinishedState");
			xml.IntoElem(); /*xml: into <ProductiveTaskItemInfo> Node*/  IntoElem();//into <TaskItemFinishedState> Node
			{
				GetSiblingNode(&xml, "TaskItemID");   AddElem("TaskItemID", xml.GetData());
				GetSiblingNode(&xml, "TaskItemType");  AddElem("TaskItemType", xml.GetData());

				AddElem("FinishedStatus", "Other");
				AddElem("FinishedStatusDisc", "Other");
				AddElem("ProductInfoList"); IntoElem();//into <ProductInfo> Node
				AddElem("Type", "Other");
				AddElem("Level", "Other");
				AddElem("OrganizationMode", "Other");
/*
				AddElem("ProductInfo"); IntoElem();//into <ProductInfoList> Node
				AddElem("ProductType", "Other");
				AddElem("Path", "Other");
				OutOfElem();//out <ProductInfo> Node.*/
				OutOfElem();//out <ProductInfoList> Node.
				AddElem("TemporaryResult"); IntoElem();//into <TemporaryResult> Node;
				AddElem("TemporaryDataPath", "Other");
				AddElem("IsExportTemporaryData", "Other");
				OutOfElem();//out <TemporaryResult> Node.
			}
/*		                                                             Start----- Get : "complete xml" file name                                                                             */
			GetSiblingNode(&xml, "OutCompleteFileName");
			MCD_STR str;
			str = xml.GetData();
/*		                                                             Start-----get absotue file path                                                                             */			
			char inifilepath[512];	char strOutPath[512];
			if (GetPrivateProfilePath(inifilepath, CONFIG_FILE)) {
				GetPrivateProfileString("PathSetting", "linuxpath", "", strOutPath, 256, inifilepath); VERF_SLASH(strOutPath);	m_nHeadLen = strlen(strOutPath) + 1;
				if (!IsExist(strOutPath))  { GetPrivateProfileString("PathSetting", "windowspath", "", strOutPath, 256, inifilepath); VERF_SLASH(strOutPath);	m_nHeadLen = strlen(strOutPath) + 1; }
				if (!IsExist(strOutPath))  { strcpy(strOutPath, ""); m_nHeadLen = 0; }
				else strcat(strOutPath, "/");
				LogPrint(0, "Len_Path_Head = %d ", m_nHeadLen);
				sprintf(m_strFilePath, "%s%s", strOutPath, str.c_str());// Get : "complete xml" file name  
			}
/*		                                                             End-----get absotue file path                                                                             */
/*		                                                             End-----Get : "complete xml" file name                                                                             */
			xml.OutOfElem(); /*xml: outo <ProductiveTaskItemInfo> Node*/  OutOfElem();//out <TaskItemFinishedState> Node
		}
		xml.OutOfElem();  /*xml: out <ProductiveTaskItemList> Node*/     OutOfElem();//out <TaskItemFinishedStateList> Node
		xml.OutOfElem();	 /*xml: out <content> Node*/			OutOfElem();//out <content> Node
		xml.OutOfElem(); /*xml: out <root> Node*/ 	OutOfElem();//out <root> Node
/*		                                                              End----Write: "complete xml" frame                                                                             */
/*		                                                             Start----- if not found  "complete xml" file name ,rename "complete xml".                                                                             */
		FILE* fp = NULL;
		if (m_strFilePath[0] != 0) fp = fopen(m_strFilePath, "w");
		if (!fp) {
			strcpy(m_strFilePath, lpstr);
			strcpy(strrchr(m_strFilePath, '.'), "_complete.xml");
		}
		else { fclose(fp); DeleteFile(m_strFilePath); }
/*		                                                             End----- if not found  "complete xml" file name ,rename "complete xml".                                                                             */
		if (m_nSucess != modeSuccess) { Close(); return false; }
		return true;
	}
	
	/*void	Write(ImgInfo* pImg){
		char strDataType[MAX_FILEPATH], strDataLevl[MAX_FILEPATH], strOrganizationMode[MAX_FILEPATH];
		char strProdcutType[MAX_FILEPATH], strProductPath[MAX_FILEPATH];
		if ((m_pXml->m_tskTypeCur&TSK_ATMCH) || (m_pXml->m_tskTypeCur&TSK_ATADJ)) {
			strcpy(strDataType, "RPC");
			strcpy(strDataLevl, "Other");
			strcpy(strOrganizationMode, "Scene");
			strcpy(strProdcutType, "CALIB_RPC");
			strcpy(strProductPath, pImg->sImgPN);
		}
		else if (m_pXml->m_tskTypeCur&TSK_DEMMCH) {
			strcpy(strDataType, "Terrain");
			strcpy(strDataLevl, "DSM");
			strcpy(strOrganizationMode, "Scene");
			strcpy(strProdcutType, "DSM_SCENE");
			strcpy(strProductPath, pImg->sImgPN);
		}
		else if (m_pXml->m_tskTypeCur&TSK_DEMFLT);
		else if (m_pXml->m_tskTypeCur&TSK_IMGEPI);
		else if (m_pXml->m_tskTypeCur&TSK_IMGREC) {
			strcpy(strDataType, "Image");
			strcpy(strDataLevl, "GTC");
			strcpy(strOrganizationMode, "Scene");
			strcpy(strProdcutType, "GTC_SCENE");
			strcpy(strProductPath, pImg->sImgPN);
		}
		else if (m_pXml->m_tskTypeCur&TSK_IMGFUS) {
			strcpy(strDataType, "Image");
			strcpy(strDataLevl, "FUS");
			strcpy(strOrganizationMode, "Scene");
			strcpy(strProdcutType, "OTHER");
			strcpy(strProductPath, pImg->sImgPN);
		}
		else if (m_pXml->m_tskTypeCur&TSK_IMGMSC) {
			strcpy(strDataType, "Image");
			strcpy(strDataLevl, "MSC");
			strcpy(strOrganizationMode, "Other");
			strcpy(strProdcutType, "MOSIC_FILE");
			strcpy(strProductPath, pImg->sImgPN);
		}
		else
		{
			strcpy(strDataType, "Other");
			strcpy(strDataLevl, "Other");
			strcpy(strOrganizationMode, "Other");
			strcpy(strProdcutType, "Other");
			strcpy(strProductPath, "");
		}
/ *		                                                             Start----- Write: product info.                                                                             * /
		ResetPos();
		FindElem("root");	IntoElem();//into <root> Node
		FindElem("content"); IntoElem();//into <content> Node
		FindElem("TaskItemFinishedStateList");	IntoElem();//into <TaskItemFinishedStateList> Node
		FindElem("TaskItemFinishedState"); IntoElem();//into <TaskItemFinishedState> Node
		FindElem("ProductInfoList"); IntoElem();//into <ProductInfoList> Node
		FindElem("Type"); SetData(strDataType);
		FindElem("Level"); SetData(strDataLevl);
		FindElem("OrganizationMode"); SetData(strOrganizationMode);
		AddElem("ProductInfo");IntoElem();//into <ProductInfo> Node
		AddElem("ProductType",strProdcutType);
		AddElem("Path", strProductPath);
		OutOfElem();//out <ProductInfo> Node
		OutOfElem();//out <ProductInfoList> Node
		OutOfElem();//out <TaskItemFinishedState> Node
		OutOfElem();//out <TaskItemFinishedStateList> Node
		OutOfElem();//out <content> Node
		OutOfElem();//out <root> Node
/ *		                                                             End----- Write: product info.                                                                             * /
		m_nImgNum++;
	}*/
	void	Write(char* str,char*lpProdType=NULL){
		char strDataType[MAX_FILEPATH], strDataLevl[MAX_FILEPATH], strOrganizationMode[MAX_FILEPATH];
		char strProdcutType[MAX_FILEPATH], strProductPath[MAX_FILEPATH];
		if ((m_pXml->m_tskTypeCur&TSK_ATMCH) || (m_pXml->m_tskTypeCur&TSK_ATADJ)) {
			strcpy(strDataType, "RPC");
			strcpy(strDataLevl, "Other");
			strcpy(strOrganizationMode, "Scene");
			if (lpProdType)strcpy(strProdcutType, lpProdType);
			else strcpy(strProdcutType, "CALIB_RPC");
			strcpy(strProductPath, str);
		}
		else if (m_pXml->m_tskTypeCur&TSK_DEMMCH) {
			strcpy(strDataType, "Terrain");
			strcpy(strDataLevl, "DSM");
			strcpy(strOrganizationMode, "Scene");
			strcpy(strProdcutType, "DSM_SCENE");
			strcpy(strProductPath, str);
		}
		else if (m_pXml->m_tskTypeCur&TSK_DEMFLT);
		else if (m_pXml->m_tskTypeCur&TSK_IMGEPI);
		else if (m_pXml->m_tskTypeCur&TSK_IMGREC) {
			strcpy(strDataType, "Image");
			strcpy(strDataLevl, "GTC");
			strcpy(strOrganizationMode, "Scene");
			strcpy(strProdcutType, "GTC_SCENE");
			strcpy(strProductPath, str);
		}
		else if (m_pXml->m_tskTypeCur&TSK_IMGFUS) {
			strcpy(strDataType, "Image");
			strcpy(strDataLevl, "FUS");
			strcpy(strOrganizationMode, "Scene");
			strcpy(strProdcutType, "OTHER");
			strcpy(strProductPath, str);
		}
		else if (m_pXml->m_tskTypeCur&TSK_IMGMSC) {
			strcpy(strDataType, "Image");
			strcpy(strDataLevl, "MSC");
			strcpy(strOrganizationMode, "Other");
			strcpy(strProdcutType, "MOSIC_FILE");
			strcpy(strProductPath, str);
		}
		else
		{
			strcpy(strDataType, "Other");
			strcpy(strDataLevl, "Other");
			strcpy(strOrganizationMode, "Other");
			strcpy(strProdcutType, "Other");
			strcpy(strProductPath, "");
		}
		/*		                                                             Start----- Write: product info.                                                                             */
		ResetPos();
		FindElem("root");	IntoElem();//into <root> Node
		FindElem("content"); IntoElem();//into <content> Node
		FindElem("TaskItemFinishedStateList");	IntoElem();//into <TaskItemFinishedStateList> Node
		FindElem("TaskItemFinishedState"); IntoElem();//into <TaskItemFinishedState> Node
		FindElem("ProductInfoList"); IntoElem();//into <ProductInfoList> Node
		FindElem("Type"); SetData(strDataType);
		FindElem("Level"); SetData(strDataLevl);
		FindElem("OrganizationMode"); SetData(strOrganizationMode);
		AddElem("ProductInfo"); IntoElem();//into <ProductInfo> Node
		AddElem("ProductType", strProdcutType);
		AddElem("Path", strProductPath);
		OutOfElem();//out <ProductInfo> Node
		OutOfElem();//out <ProductInfoList> Node
		OutOfElem();//out <TaskItemFinishedState> Node
		OutOfElem();//out <TaskItemFinishedStateList> Node
		OutOfElem();//out <content> Node
		OutOfElem();//out <root> Node
		/*		                                                             End----- Write: product info.                                                                             */
		m_nImgNum++;
	}
	void	Close(){
			ResetPos();
			FindElem("root");	IntoElem();//into <root> Node
			FindElem("content"); IntoElem();//into <content> Node
			FindElem("TaskItemFinishedStateList");	IntoElem();//into <TaskItemFinishedStateList> Node
			FindElem("TaskItemFinishedState"); IntoElem();//into <TaskItemFinishedState> Node
			if (m_nImgNum <= 0)
			{
				FindElem("FinishedStatus"); SetData("NotComplete");
				FindElem("FinishedStatusDisc"); SetData("NotComplete");
			}
			else if (m_nImgNum < m_nInToalNum)
			{
				FindElem("FinishedStatus"); SetData("PartiallyCompleted");
				FindElem("FinishedStatusDisc"); SetData("PartiallyCompleted");
			}
			else{
				FindElem("FinishedStatus"); SetData("EntireCompleted");
				FindElem("FinishedStatusDisc"); SetData("EntireCompleted");
			}
			OutOfElem();//out <TaskItemFinishedState> Node
			OutOfElem();//out <TaskItemFinishedStateList> Node
			OutOfElem();//out <content> Node
			OutOfElem();//out <root> Node
			if (Save(m_strFilePath))LogPrint(ERR_NONE, "Save Complete XML:%s!", m_strFilePath);
			else
			{
				m_nSucess = modeFail;
				sprintf(m_strMsg, "Error:Fail to Save the Xml File:%s!", m_strFilePath);
				LogPrint(ERR_FILE_OPEN, "%s", m_strMsg);
			}		
			Reset();//delete
	}
	void SetRetMsg(MODERET mode, LPCSTR strMsg)
	{
		m_nSucess = mode;
		if (strlen(m_strMsg) > 2048 - 512) return;
		strcat(m_strMsg, strMsg);
	}

protected:
	char			m_strFilePath[512];
	MODERET			m_nSucess;
	char			m_strMsg[2048];
	int				m_nImgNum;
	int			m_nInToalNum;
	CPrjXml*		m_pXml;
	int				m_nHeadLen;

private:
	void Reset()
	{
		if (m_pXml) delete m_pXml; m_pXml = NULL;
		m_nSucess = modeSuccess;
		m_strMsg[0] = 0;
		m_nImgNum = 0;
		m_nHeadLen = 0;
	}
};

void MoveMap(const char* oldDir, const char* newDir, CProdXml& prodXml)
{
	CSrchFl findFile;
	if (!findFile.Open(oldDir)) return;

	int	dir_num = findFile.GetDirNum();

	int i;	char strOldPath[512], strNewPath[512], strName[128];
	strcpy(strNewPath, newDir);	char* pT = AddEndSlash(strNewPath) + 1;
	for (i = 0; i < dir_num; i++){
		findFile.GetDirPath(i, strOldPath);	findFile.GetDirName(i, strName);
		strcpy(pT, strName);
		rename(strOldPath, strNewPath);	prodXml.Write(strNewPath);
	}

}

void CarryEpiImg(const char* lpstrInterior, const char* lpstrExternal, const char* lpstrSc)
{
	char strSrcPath[512], strDstPath[512];

	strcpy(strSrcPath, lpstrInterior);	strcpy(strDstPath, lpstrExternal);
	if (!IsExist(strSrcPath)) { LogPrint(ERR_NONE, "Fail to Create %s!", strDstPath); return; }
	CopyFile(strSrcPath, strDstPath, TRUE);
	LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);

	char* pS1 = strrchr(strSrcPath, '.');	char* pT1 = strSrcPath + strlen(strSrcPath);
	char* pS2 = strrchr(strDstPath, '.');	char* pT2 = strDstPath + strlen(strDstPath);

	strcpy(pT1, ".RPC");	strcpy(pS2, "_rpc.txt");
	if (!IsExist(strSrcPath))	strcpy(pT1, ".rpc");
	if (!IsExist(strSrcPath))  LogPrint(ERR_NONE, "Fail to Create %s!", strDstPath);
	else LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);
	CopyFile(strSrcPath, strDstPath, TRUE);

	strcpy(pS1, ".JPG");		strcpy(pS2, "_pre.jpg");
	if (!IsExist(strSrcPath))	strcpy(pS1, ".jpg");
	if (!IsExist(strSrcPath)) LogPrint(ERR_NONE, "Fail to Create %s!", strDstPath);
	else LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);
	CopyFile(strSrcPath, strDstPath, TRUE);

	strcpy(pS1, ".THUMB.JPG");	strcpy(pS2, "_ico.jpg");
	if (!IsExist(strSrcPath))	strcpy(pS1, ".THUMB.jpg");
	if (!IsExist(strSrcPath)) LogPrint(ERR_NONE, "Fail to Create  %s!", strDstPath);
	else LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);
	CopyFile(strSrcPath, strDstPath, TRUE);

	strcpy(pS1, "_range.txt");	strcpy(pS2, "_range.txt");
	if (!IsExist(strSrcPath))	strcpy(pS1, "_RANGE.TXT");
	if (!IsExist(strSrcPath)) LogPrint(ERR_NONE, "Fail to Create  %s!", strDstPath);
	else LogPrint(ERR_NONE, "Save %s Successfully", strDstPath);
	CopyFile(strSrcPath, strDstPath, TRUE);

	char strFileName[256];
	strcpy(strDstPath, lpstrExternal); Dos2Unix(strDstPath);
	char* pPath = strrchr(strDstPath, '/');
	strcpy(strFileName, pPath + 1);	strcpy(pPath, "/GeoRange/");	CreateDir(strDstPath);	strcat(strDstPath, strFileName);
	pS2 = strrchr(strDstPath, '.');

	strcpy(strSrcPath, lpstrInterior);	 Dos2Unix(strSrcPath);	pS1 = strrchr(strSrcPath, '.');

	strcpy(pS1, ".shp");
	if (!IsExist(strSrcPath))	strcpy(pS1, ".SHP");
	strcpy(pS2, ".shp");
	CopyFile(strSrcPath, strDstPath, TRUE);

	strcpy(pS1, ".dbf");
	if (!IsExist(strSrcPath))	strcpy(pS1, ".DBF");
	strcpy(pS2, ".dbf");
	CopyFile(strSrcPath, strDstPath, TRUE);

	strcpy(pS1, ".shx");
	if (!IsExist(strSrcPath))	strcpy(pS1, ".SHX");
	strcpy(pS2, ".shx");
	CopyFile(strSrcPath, strDstPath, TRUE);

	strcpy(pS1, ".prj");
	if (!IsExist(strSrcPath))	strcpy(pS1, ".PRJ");
	strcpy(pS2, ".prj");
	CopyFile(strSrcPath, strDstPath, TRUE);
}

void CarryEpiFile(const char* lpstrNImgPath, const char* lpstrFImgPath, const char* lpstrBImgPath,
	const char* lpstrNEpiPath, const char* lpstrFEpiPath, const char* lpstrBEpiPath,
	ImgInfo* lpNImg, ImgInfo* lpFImg, ImgInfo* lpBImg, CProdXml& prodXml)
{
	char strMsg[1024];
	if (!IsExist(lpstrNEpiPath)){
		LogPrint(ERR_EPI_EPIIMG, "Fail to Create Epipolar Image:%s !", strrpath(lpstrNEpiPath) + 1);
		sprintf(strMsg, "lost product: %s\n", strrpath(lpstrNEpiPath) + 1);
		prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
		return;
	}
	if (!IsExist(lpstrFEpiPath)){
		LogPrint(ERR_EPI_EPIIMG, "Fail to Create Epipolar Image:%s !", strrpath(lpstrFEpiPath) + 1);
		sprintf(strMsg, "lost product: %s\n", strrpath(lpstrFEpiPath) + 1);
		prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
		return;
	}
	if (!IsExist(lpstrBEpiPath)){
		LogPrint(ERR_EPI_EPIIMG, "Fail to Create Epipolar Image:%s !", strrpath(lpstrBEpiPath) + 1);
		sprintf(strMsg, "lost product: %s\n", strrpath(lpstrBEpiPath) + 1);
		prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
		return;
	}
	char* pT = NULL;

	char strN[512];
	char strArchFileName[128];
	pT = strrpath(lpNImg->sImgPN);	strcpy(strArchFileName, pT + 1);	*strrchr(strArchFileName, '.') = 0;	memcpy(strArchFileName + 7, "tlc", 3);
	strcpy(strN, lpNImg->sImgPN);	*strrpath(strN) = 0;	pT = strrpath(strN);	strcpy(pT + 1, strArchFileName);	CreateDir(strN);	pT = strN + strlen(strN);
	*pT = '/';	pT++;
	strcpy(pT, strArchFileName);	strcat(pT, ".xml");


	char strTmp[512];
	{
		strcpy(strTmp, lpstrNEpiPath);	strcpy(strrchr(strTmp, '.'), ".xml");

		writeEPIXml(lpstrFImgPath, lpstrNImgPath, lpstrBImgPath,
			lpstrFEpiPath, lpstrNEpiPath, lpstrBEpiPath, strTmp);
		CarryFile(strTmp, strN);
		prodXml.Write(strN);
	}


	char *pT2, *pT3;
	strcpy(pT, "f-n/");	CreateDir(strN);	pT2 = strN + strlen(strN);
	pT3 = strrpath(lpNImg->sImgPN);	strcpy(pT2, pT3 + 1);	CarryEpiImg(lpstrNEpiPath, strN, lpstrNImgPath);//	prodXml.Write(strN);
	pT3 = strrpath(lpFImg->sImgPN);	strcpy(pT2, pT3 + 1);	CarryEpiImg(lpstrFEpiPath, strN, lpstrFImgPath);//	prodXml.Write(strN);

	strcpy(pT, "f-b/");	CreateDir(strN);	pT2 = strN + strlen(strN);
	pT3 = strrpath(lpBImg->sImgPN);	strcpy(pT2, pT3 + 1);	CarryEpiImg(lpstrBEpiPath, strN, lpstrBImgPath);//	prodXml.Write(strN);
	pT3 = strrpath(lpFImg->sImgPN);	strcpy(pT2, pT3 + 1);	CarryEpiImg(lpstrFEpiPath, strN, lpstrFImgPath);//	prodXml.Write(strN);

	strcpy(pT, "n-b/");	CreateDir(strN);	pT2 = strN + strlen(strN);
	pT3 = strrpath(lpNImg->sImgPN);	strcpy(pT2, pT3 + 1);	CarryEpiImg(lpstrNEpiPath, strN, lpstrNImgPath);//	prodXml.Write(strN);
	pT3 = strrpath(lpBImg->sImgPN);	strcpy(pT2, pT3 + 1);	CarryEpiImg(lpstrBEpiPath, strN, lpstrBImgPath);//	prodXml.Write(strN);

	remove(lpstrFEpiPath);	remove(lpstrNEpiPath);	remove(lpstrBEpiPath);
}

int main(int argc, char* argv[])
{
	const char* lpPrjPath = NULL;
	const char* lpTask = NULL;
	int i;
	for (i = 0; i < argc - 1; i++){
		if (!strcmp(argv[i], "-l")){
			while (i + 1 < argc && argv[i + 1][0] != '-') { i++; }	if (argv[i][0] == '-') { printf("Error:Please Enter Task List!\n"); break; }
			lpTask = argv[i];
		}
		else
		if (!strcmp(argv[i], "-i")){
			if (argv[++i][0] == '-') { printf("Error:Please Enter Exec Parameter List!\n"); break; }
			lpPrjPath = argv[i];
		}
	}

	if (!lpTask || !lpPrjPath){
		Usage();
	}

	char strExe[512];
	if (argv[0][1] == ':')		strcpy(strExe, argv[0] + 2);	else strcpy(strExe, argv[0]);	Dos2Unix(strExe);

	TASK tskType;
	if (!strcmp(lpTask, "AtMch") && !strcmp(lpTask, "TieMch"))		tskType = TSK_ATMCH; else
	if (!strcmp(lpTask, "AtAdj"))		tskType = TSK_ATADJ; else
	if (!strcmp(lpTask, "ImgEpi"))		tskType = TSK_IMGEPI; else
	if (!strcmp(lpTask, "DemQMch"))		tskType = TSK_DEMMCH; else
	if (!strcmp(lpTask, "DemFlt"))		tskType = TSK_DEMFLT; else
	if (!strcmp(lpTask, "ImgRec"))		tskType = TSK_IMGREC; else
	if (!strcmp(lpTask, "ImgFus"))		tskType = TSK_IMGFUS; else
	if (!strcmp(lpTask, "ImgMxz"))		tskType = TSK_IMGMSC; else
	if (!strcmp(lpTask, "DemMap"))		tskType = TSK_DSM_MAP; else
	if (!strcmp(lpTask, "DomMap"))		tskType = TSK_IMG_MAP; else
	if (!strcmp(lpTask, "Order"))		tskType = TSK_END; else
		Usage();

	char strXml[512];		strcpy(strXml, lpPrjPath);

	char strLog[512];	char* pS;
	strcpy(strLog, strXml);
	pS = strrchr(strLog, '.');	strcpy(pS, "_ARCH.log");


	OpenLog(strLog);
	LogPrint(ERR_NONE, "Running:%s %s %s", strExe, lpTask, strXml);

	CProdXml prodXml;	CPrjXml* pXml = NULL;
	if (!prodXml.Open(strXml, pXml)) { LogPrint(ERR_FILE_OPEN, "Error:Fail to Load File:%s!", strXml); return ERR_FILE_OPEN; }
	LogPrint(ERR_NONE, "File:%s is loaded.", strXml);

	CXmlPrj xmlPrj;	char strMsg[256];
	if (xmlPrj.LoadPrj(strXml) != RET_OK)
	{
		LogPrint(ERR_FILE_OPEN, "Can't Open %s!", strXml);
		sprintf(strMsg, "there is something wrong in %s\n", strXml);
		prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
		prodXml.Close();
		return -1;
	}
	LogPrint(ERR_NONE, "Open %s Successfully", strXml);

	char strWorkSpace[512];	strcpy(strWorkSpace, strXml);
	pS = strrchr(strWorkSpace, '/');	if (!pS) pS = strrchr(strWorkSpace, '\\');	*(pS) = 0;

	char strFileNam[215], strImgNam[215];	strcpy(strFileNam, pS + 1);
	char* pN = strrchr(strFileNam, '.');

	char strTmp[512];	strcpy(strTmp, strWorkSpace);
	pS = strTmp + strlen(strTmp);

	char strGcdPath[512];	strcpy(strGcdPath, strXml);	Dos2Unix(strGcdPath);	strcpy(strrchr(strGcdPath, '/') + 1, "ProdGeoSys.gcd");
	char strSetGeoCmd[1024];	strcpy(strSetGeoCmd, strExe);
	strcpy(strrchr(strSetGeoCmd, '/') + 1, "../RunPE ");	strcat(strSetGeoCmd, strExe);	strcpy(strrchr(strSetGeoCmd, '/') + 1, "SetGeo.exe ");
	strcat(strSetGeoCmd, "-tfw -gcd ");	strcat(strSetGeoCmd, strGcdPath);
	char* pSetGeoCmd = strSetGeoCmd + strlen(strSetGeoCmd);

	char* pT;
		
	switch (tskType)
	{
	case TSK_ATMCH:
	{
					  char strATWork[512], *pT2;
					  strcpy(pS, "/Product/AT/");
					  strcpy(strATWork, strTmp);		pT2 = AddEndSlash(strATWork);

					  strcpy(pN, ".sz");
					  sprintf(pS, "/AT/ATMCH/%s", strFileNam);
					  if (IsExist(strTmp))
					  {
						  strcpy(pT2 + 1, strFileNam); CopyFile(strTmp, strATWork, FALSE); LogPrint(ERR_NONE, "Save %s Successfully", strATWork);
					  }

					  strcpy(pN, "_Save.gcp");
					  sprintf(pS, "/AT/ATMCH/%s", strFileNam);
					  if (IsExist(strTmp)) { CopyFile(strTmp, xmlPrj.m_sGCPPN, FALSE); LogPrint(ERR_NONE, "Save %s Successfully", xmlPrj.m_sGCPPN); }

					  strcpy(pN, "_Tie.bpc");
					  sprintf(pS, "/AT/ATMCH/%s", strFileNam);
					  if (IsExist(strTmp)) { CopyFile(strTmp, xmlPrj.m_sBPCPN, FALSE);	LogPrint(ERR_NONE, "Save %s Successfully", xmlPrj.m_sBPCPN); }

					  ImgInfo* pImgInf;
					  for (i = 0, pImgInf = xmlPrj.m_inImg; i < xmlPrj.m_inNum; i++, pImgInf++){
						  pT = strrchr(pImgInf->sImgPN, '/');	if (!pT) pT = strrchr(pImgInf->sImgPN, '\\');
						  strcpy(strImgNam, pT);	strcpy(strrchr(strImgNam, '.'), ".ixy");
						  sprintf(pS, "/AT/ATMCH/Work%s", strImgNam);	strcpy(pT2, strImgNam);
						  if (IsExist(strTmp)) { CopyFile(strTmp, strATWork, FALSE);	LogPrint(ERR_NONE, "Save %s Successfully", strATWork); }
						  else
						  {
							  sprintf(strMsg, "fail to save %s\n", strATWork);
							  prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
						  }
					  }
	}
		break;
	case TSK_ATADJ:
	{
					  //Archive ¡°report¡± file
					  char strReportmentFilePath[512];
					  strcpy(strReportmentFilePath, xmlPrj.m_sAdjReportPN); *(strrpath(strReportmentFilePath)) = '\0';
					  CreateDir(strReportmentFilePath);//Create ¡°report¡± file folder
					  char strAdjReportPN[MAX_FILEPATH];
					  sprintf(strAdjReportPN, "%s%s", strWorkSpace, "/Product/AT/PAN/report/AdjustmentReport.txt");
					  if (IsExist(strAdjReportPN))
					  {
						 CopyFile(strAdjReportPN, xmlPrj.m_sAdjReportPN, FALSE);
						  LogPrint(ERR_NONE, "Save %s Successfully", xmlPrj.m_sAdjReportPN);
						  prodXml.Write(xmlPrj.m_sAdjReportPN,"CALIB_REPORT");
					  }
					  //Archive ¡°RPC¡± file
					  for (int i = 0; i < xmlPrj.m_inNum;i++)
					  {
						  if (IsExist((xmlPrj.m_inImg+i)->sAdjRPCPath))
						  {
							  LogPrint(ERR_NONE, "Save %s Successfully", (xmlPrj.m_inImg + i)->sAdjRPCPath);
							  prodXml.Write((xmlPrj.m_inImg + i)->sAdjRPCPath);
						  }
					  }		  	 	
	}
		break;
	case TSK_IMGEPI:
	{
					   strcpy(pS, "/DOM/EPI/Product");	pS = strTmp + strlen(strTmp);
					   int	nSceneNum;
					   SCENEINF* pScene = xmlPrj.m_scList.GetData(nSceneNum);
					   LogPrint(ERR_NONE, "Scence Num:%d", nSceneNum);

					   char strF[512], strN[512], strB[512];
					   for (i = 0; i < nSceneNum; i++, pScene++){
						   pT = strrpath(pScene->strNameImgF);
						   if (!pT) { LogPrint(ERR_EPI_EPIIMG, "Warning:Scence %s May Lost FWD Image !", pScene->sID); continue; }
						   strcpy(pS, pT);	strcpy(strF, strTmp);
						   pT = strrpath(pScene->strNameImgN);
						   if (!pT) { LogPrint(ERR_EPI_EPIIMG, "Warning:Scence %s May Lost NAD Image !", pScene->sID); continue; }
						   strcpy(pS, pT);	strcpy(strN, strTmp);
						   pT = strrpath(pScene->strNameImgB);
						   if (!pT) { LogPrint(ERR_EPI_EPIIMG, "Warning:Scence %s May Lost BWD Image !", pScene->sID); continue; }
						   strcpy(pS, pT);	strcpy(strB, strTmp);
						   CarryEpiFile(pScene->strNameImgN, pScene->strNameImgF, pScene->strNameImgB, strN, strF, strB, pScene->prodInf[IT_EPIN], pScene->prodInf[IT_EPIF], pScene->prodInf[IT_EPIB], prodXml);
					   }
	}
		break;
	case TSK_DEMMCH:
	{
					   strcpy(pS, "/DEM/Product");	pS = strTmp + strlen(strTmp);
					   int	nSceneNum;
					   SCENEINF* pScene = xmlPrj.m_scList.GetData(nSceneNum);

					   char strMeta[512];
					   for (i = 0; i < nSceneNum; i++, pScene++){
						   if (!pScene->prodInf[IT_DSM])  continue;
						   sprintf(pS, "/%s", strrpath(pScene->prodInf[IT_DSM]->sImgPN) + 1);	strcpy(strrchr(strTmp, '.'), ".TIF");
						   strcpy(strMeta, strTmp);	strcpy(strrchr(strMeta, '.'), ".xml");

						   writeDSMXml(pScene->strNameImgF, pScene->strNameImgN, pScene->strNameImgB,
							   strTmp, strGcdPath, strMeta);

						   if (!CarryImgFile(strTmp, pScene->prodInf[IT_DSM], false)) {
							   if (pScene->prodInf[IT_DSM])
							   {
								   LogPrint(ERR_DEM_FLAG, "Fail to Create DSM :%s !", pScene->prodInf[IT_DSM]->sImgPN);
								   sprintf(strMsg, "lost product: %s\n", pScene->prodInf[IT_DSM]->sImgPN);
							   }
							   else sprintf(strMsg, "lost DSM product in scene:%s\n", pScene->sID);
							   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   continue;
						   }
						   prodXml.Write(pScene->prodInf[IT_DSM]->sImgPN);
					   }
	}
		break;
	case TSK_DEMFLT:
	{
					   strcpy(pS, "/DEM/Product");	pS = strTmp + strlen(strTmp);
					   int	nSceneNum;
					   SCENEINF* pScene = xmlPrj.m_scList.GetData(nSceneNum);

					   char strMeta[512];
					   for (i = 0; i < nSceneNum; i++, pScene++){
						   if (!pScene->prodInf[IT_DEM])  continue;
						   sprintf(pS, "/%s", strrpath(pScene->prodInf[IT_DEM]->sImgPN) + 1);
						   strcpy(strMeta, strTmp);	strcpy(strrchr(strMeta, '.'), ".xml");

						   writeDEMXml(pScene->prodInf[IT_DSM]->sMetaPN, strMeta);

						   if (!CarryImgFile(strTmp, pScene->prodInf[IT_DEM], false)) {
							   if (pScene->prodInf[IT_DEM])
							   {
								   LogPrint(ERR_DEM_FLAG, "Fail to Create DEM :%s !", pScene->prodInf[IT_DEM]->sImgPN);
								   sprintf(strMsg, "lost product: %s\n", pScene->prodInf[IT_DEM]->sImgPN);
							   }
							   else sprintf(strMsg, "lost DEM product in scene:%s\n", pScene->sID);
							   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   continue;
						   }
						   prodXml.Write(pScene->prodInf[IT_DEM]->sImgPN);
					   }
	}
		break;
	case TSK_IMGREC:
	{
					   strcpy(pS, "/DOM/Product");	pS = strTmp + strlen(strTmp);

					   int	nSceneNum;
					   SCENEINF* pScene = xmlPrj.m_scList.GetData(nSceneNum);	ImgInfo* pImgInf;
					   LogPrint(ERR_NONE, "Scence Num:%d", nSceneNum);

					   for (i = 0; i < nSceneNum; i++, pScene++){

						   if (pScene->prodInf[IT_DOMN]){
							   pImgInf = pScene->prodInf[IT_DOMN];
							   pT = strrpath(pImgInf->sImgPN);
							   strcpy(pS, pT);
							   if (!CarryImgFile(strTmp, pImgInf)) {
								   LogPrint(ERR_DEM_FLAG, "Fail to Create Rectified Image :%s !", pImgInf->sImgPN);
								   sprintf(strMsg, "lost product: %s\n", pImgInf->sImgPN);
								   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   }
							   else{
								   prodXml.Write(pImgInf->sImgPN);
#ifndef MIP
								   cpmeta(xmlPrj.m_inImg, pImgInf, strXml);
#endif
							   }				
						   }
						   else if (pScene->prodInf[IT_JJZN]){
							   pImgInf = pScene->prodInf[IT_JJZN];
							   pT = strrpath(pImgInf->sImgPN);
							   strcpy(pS, pT);
							   if (!CarryImgFile(strTmp, pImgInf)) {
								   LogPrint(ERR_DEM_FLAG, "Fail to Create Rectified Image :%s !", pImgInf->sImgPN);
								   sprintf(strMsg, "lost product: %s\n", pImgInf->sImgPN);
								   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   }
							   else{
								   prodXml.Write(pImgInf->sImgPN);
#ifndef MIP
								   cpmeta(xmlPrj.m_inImg, pImgInf, strXml);
#endif
							   }
						   }
						   else if (pScene->prodInf[IT_XTJZN]){
							   pImgInf = pScene->prodInf[IT_XTJZN];
							   pT = strrpath(pImgInf->sImgPN);
							   strcpy(pS, pT);
							   if (!CarryImgFile(strTmp, pImgInf)) {
								   LogPrint(ERR_DEM_FLAG, "Fail to Create Rectified Image :%s !", pImgInf->sImgPN);
								   sprintf(strMsg, "lost product: %s\n", pImgInf->sImgPN);
								   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   }
							   else{
								   prodXml.Write(pImgInf->sImgPN);
#ifndef MIP
								   cpmeta(xmlPrj.m_inImg, pImgInf, strXml);
#endif
							   }
						   }
						   if (pScene->prodInf[IT_DOMC]){
							   pImgInf = pScene->prodInf[IT_DOMC];
							   pT = strrpath(pImgInf->sImgPN);
							   strcpy(pS, pT);
							   if (!CarryImgFile(strTmp, pImgInf)) {
								   LogPrint(ERR_DEM_FLAG, "Fail to Create Rectified Image :%s !", pImgInf->sImgPN);
								   sprintf(strMsg, "lost product: %s\n", pImgInf->sImgPN);
								   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   }
							   else{
								   prodXml.Write(pImgInf->sImgPN);
#ifndef MIP
								   cpmeta(xmlPrj.m_inImg, pImgInf, strXml);
#endif
							   }
						   }
						   else if (pScene->prodInf[IT_JJZC]){
							   pImgInf = pScene->prodInf[IT_JJZC];
							   pT = strrpath(pImgInf->sImgPN);
							   strcpy(pS, pT);
							   if (!CarryImgFile(strTmp, pImgInf)) {
								   LogPrint(ERR_DEM_FLAG, "Fail to Create Rectified Image :%s !", pImgInf->sImgPN);
								   sprintf(strMsg, "lost product: %s\n", pImgInf->sImgPN);
								   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   }
							   else{
								   prodXml.Write(pImgInf->sImgPN);
#ifndef MIP
								   cpmeta(xmlPrj.m_inImg, pImgInf, strXml);
#endif
							   }
						   }
						   else if (pScene->prodInf[IT_XTJZC]){
							   pImgInf = pScene->prodInf[IT_XTJZC];
							   pT = strrpath(pImgInf->sImgPN);
							   strcpy(pS, pT);
							   if (!CarryImgFile(strTmp, pImgInf)) {
								   LogPrint(ERR_DEM_FLAG, "Fail to Create Rectified Image :%s !", pImgInf->sImgPN);
								   sprintf(strMsg, "lost product: %s\n", pImgInf->sImgPN);
								   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   }
							   else{
								   prodXml.Write(pImgInf->sImgPN);
#ifndef MIP
								   cpmeta(xmlPrj.m_inImg, pImgInf, strXml);
#endif
							   }
						   }
					   }
	}
		break;
	case TSK_IMGFUS:
	{
					   strcpy(pS, "/DOM/Product");	pS = strTmp + strlen(strTmp);

					   ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
					   for (i = 0; i < num; i++, pImgInf++){
						   pT = strrpath(pImgInf->sImgPN);
						   strcpy(pS, pT);
						   if (!CarryImgFile(strTmp, pImgInf)) {
							   LogPrint(ERR_DEM_FLAG, "Fail to Create Rectified Image :%s !", pImgInf->sImgPN);
							   sprintf(strMsg, "lost product: %s\n", pImgInf->sImgPN);
							   prodXml.SetRetMsg(CProdXml::modeHalf, strMsg);
							   continue;
						   }
						   prodXml.Write(pImgInf->sImgPN);
#ifndef MIP
						   cpmeta(xmlPrj.m_inImg, pImgInf, strXml);
#endif
					   }
	}
		break;
	case TSK_IMGMSC:
	{
					   strcpy(pS, "/DOM/Product");	pS = strTmp + strlen(strTmp);

					   ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
					   char bbi[512];
					   for (i = 0; i < num; i++, pImgInf++){
						   strcpy(bbi, pImgInf->sImgPN);
						   pT = strrpath(bbi);
						   strcpy(pS, pT);
						   if (!CarryFile(strTmp, bbi))
						   {
							   sprintf(strMsg, "lost product: %s\n", bbi);	prodXml.SetRetMsg(CProdXml::modeHalf, strMsg); continue;
						   }

						   strcpy(strTmp + strlen(strTmp), ".geo");	strcpy(bbi + strlen(bbi), ".geo");
						   if (!CarryFile(strTmp, bbi)){ sprintf(strMsg, "lost product: %s\n", bbi);	prodXml.SetRetMsg(CProdXml::modeHalf, strMsg); continue; }

						   strcpy(strrchr(strTmp, '.'), ".idx");	strcpy(strrchr(bbi, '.'), ".idx");
						   if (!CarryFile(strTmp, bbi)){ sprintf(strMsg, "lost product: %s\n", bbi);	prodXml.SetRetMsg(CProdXml::modeHalf, strMsg); continue; }

						   strcpy(strrchr(strTmp, '.'), ".inf");	strcpy(strrchr(bbi, '.'), ".inf");
						   if (!CarryFile(strTmp, bbi)){ sprintf(strMsg, "lost product: %s\n", bbi);	prodXml.SetRetMsg(CProdXml::modeHalf, strMsg); continue; }

						   prodXml.Write(pImgInf->sImgPN);
#ifndef MIP
						   cpmeta(xmlPrj.m_inImg, pImgInf, strXml);
#endif
					   }
	}
		break;
	case  TSK_IMG_MAP:{
						  strcpy(pS, "/DOM/Product");	pS = strTmp + strlen(strTmp);

						  ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
						  char bbi[512];
						  for (i = 0; i < num; i++, pImgInf++){
							  strcpy(bbi, pImgInf->sImgPN);
							  pT = strrpath(bbi);
							  *pT = 0;
							  strcpy(pS, "/MAP");
							  MoveMap(strTmp, bbi, prodXml);
						  }
	}
		break;
	case  TSK_DSM_MAP:{
						  strcpy(pS, "/DEM/Product");	pS = strTmp + strlen(strTmp);

						  ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
						  char strMapPath[512];

						  strcpy(strMapPath, pImgInf->sImgPN);
						  pT = strrpath(strMapPath);	*pT = 0;
						  pT = strrpath(strMapPath);	*pT = 0;

						  MoveMap(strTmp, strMapPath, prodXml);

	}
		break;
	}
	if (strcmp(lpTask, "AtMch") != 0)	prodXml.Close();

	LogPrint(ERR_NONE, "ARCH %s %s Over!", lpTask, strXml);

	return 0;
}