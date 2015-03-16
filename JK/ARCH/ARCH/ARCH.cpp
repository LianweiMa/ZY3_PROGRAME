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
//创建文件夹 创建元数据

#define MIP

//#include "SearchFile.hpp"
//remove
void Usage(){
	printf( 
        "%s", 
        "Usage: Arch [-l task] [-i ProjectPath] \n"
        "Opt:\n"
        " -l: AtMch TieMch AtAdj ImgEpi DemQMch DemFlt ImgRec ImgFus ImgMxz DemMap DomMap Order \n"
		" -i: the path of Project File.\n"
        );
	exit( ERR_EXE_NOTARGV );
}

#define WGS84A		6378137.0		    /* Semi-major axis of ellipsoid in meters  WGS84 */
#define WGS84B		6356752.31424517929	/* Semi-minor axis of ellipsoid            WGS84 */
#define PI          3.1415926535
#define EMIN_CHINA	73.55
#define EMAX_CHINA	135.08
#define NMIN_CHINA	20.5
#define NMAX_CHINA	53.55

inline int GetSiblingNode( CMarkup* pNode,const char* pszNode, char* pszText = NULL ){
	if(!pNode) return RET_ERR;
	
	MCD_STR str;
	if( pNode->FindElem(pszNode) ) 
	{
		if(pszText) { str = pNode->GetData(); strcpy( pszText, str.c_str()); }
		return RET_OK;
	}else
	{
		pNode->ResetMainPos();//
		if( pNode->FindElem(pszNode) ) {
			if(pszText) { str = pNode->GetData(); strcpy( pszText, str.c_str()); }
			return RET_OK;
		}else{
			printf("<%s> is not exist!\n",pszNode);
			return RET_ERR;
		}
	}
	return RET_ERR;
}

inline bool CarryFile(const char* lpSrc,const char* lpDst){
	if( !IsExist(lpSrc) ) { printf("%s is not Exist!\n",lpSrc); return false;}
	if( IsExist(lpDst) ) { if( filecmp_mtime(lpSrc,lpDst)>0 )	{	remove(lpDst);	rename(lpSrc,lpDst);	} }
	else rename(lpSrc,lpDst);
	return true;
}

bool	CarryImgFile(const char* lpSrc,ImgInfo* lpDst,bool bShp=true){
	if( !IsExist(lpSrc) || !lpDst ) return false;
	char strSrcPath[512],strDstPath[512],strF[512],strFSrc[512],c;
	strcpy(strF,lpSrc);	char* pF = strrpath(strF);
	
	strcpy(strSrcPath,lpSrc);		
	strcpy(strDstPath,lpDst->sImgPN);	
	char* pS = strrpath(strDstPath);	 c = *pS;	*pS=0;	CreateDir(strDstPath);	*pS = c;//创建目标路径
	pS = strrchr(strSrcPath,'.');	char* pT = strSrcPath + strlen(strSrcPath);
	if( !CarryFile(strSrcPath,strDstPath) ) { LogPrint(ERR_NONE,"Fail to Create %s!",strDstPath); return false; }//1 carry影像//找不到影像主体直接退出
	LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);

	strcpy(pT,".RPC");	
	if(!IsExist(strSrcPath))	strcpy(pT,".rpc");	
	if(!IsExist(strSrcPath))	{ 
		strcpy(pS,".tfw"); 	
	}	//TransforTFW(strSrcPath);

	strcpy(strDstPath,lpDst->sAdjRPCPath);
	if( !CarryFile(strSrcPath,strDstPath) )  LogPrint(ERR_NONE,"Fail to Create %s!",strDstPath);
	else LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);	

	strcpy(pS,"_PRE.JPG");
	if(!IsExist(strSrcPath))	
	{
		strcpy(pS,"_pre.jpg");
		if (!IsExist(strSrcPath)){
			strcpy(pF,strrpath(lpDst->sBrowsePN));
			strcpy(strFSrc,strF);
		}else strcpy(strFSrc,strSrcPath);	
	}else strcpy(strFSrc,strSrcPath);	

	strcpy(strDstPath,lpDst->sBrowsePN);
	if( !CarryFile(strFSrc,strDstPath) ) LogPrint(ERR_NONE,"Fail to Create %s!",strDstPath);
	else LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);

	strcpy(pS,"_ICO.JPG");
	if(!IsExist(strSrcPath))	
	{
		strcpy(pS,"_ico.jpg");
		if (!IsExist(strSrcPath)){
			strcpy(pF,strrpath(lpDst->sThumbPN));
			strcpy(strFSrc,strF);
		}else strcpy(strFSrc,strSrcPath);	
	}else strcpy(strFSrc,strSrcPath);	

	strcpy(strDstPath,lpDst->sThumbPN);
	if( !CarryFile(strFSrc,strDstPath) ) LogPrint(ERR_NONE,"Fail to Create  %s!",strDstPath);
	else LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);

	strcpy(pS,".xml");
	if(!IsExist(strSrcPath))	strcpy(pS,".XML");	

	strcpy(strDstPath,lpDst->sMetaPN);
	if( !CarryFile(strSrcPath,strDstPath) ) LogPrint(ERR_NONE,"Fail to Create  %s!",strDstPath);
	else LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);

	strcpy(pS,"_range.txt");
	if(!IsExist(strSrcPath))	strcpy(pS,"_range.txt");	
	
	strcpy(strDstPath,lpDst->sImgPN);	strcpy(strrchr(strDstPath,'.'),"_range.txt");
	CarryFile(strSrcPath,strDstPath);

	if(!bShp) return true;

	char strGeoRange[512],strFileName[256];	
	strcpy(strGeoRange,lpDst->sImgPN); Dos2Unix(strGeoRange);
	char* pPath = strrchr(strGeoRange,'/');
	strcpy(strFileName,pPath+1);	strcpy(pPath,"/GeoRange/");	CreateDir(strGeoRange);	strcat(strGeoRange,strFileName);
	pT = strrchr(strGeoRange,'.');

	strcpy(pS,".shp");
	if(!IsExist(strSrcPath))	strcpy(pS,".SHP");	
	strcpy(pT,".shp");	
//	CarryFile(strSrcPath,strGeoRange);
	CopyFile(strSrcPath,strGeoRange,TRUE);	

	strcpy(pS,".dbf");
	if(!IsExist(strSrcPath))	strcpy(pS,".DBF");	
	strcpy(pT,".dbf");
//	CarryFile(strSrcPath,strGeoRange);
	CopyFile(strSrcPath,strGeoRange,TRUE);	

	strcpy(pS,".shx");
	if(!IsExist(strSrcPath))	strcpy(pS,".SHX");	
	strcpy(pT,".shx");
//	CarryFile(strSrcPath,strGeoRange);
	CopyFile(strSrcPath,strGeoRange,TRUE);	

	strcpy(pS,".prj");
	if(!IsExist(strSrcPath))	strcpy(pS,".PRJ");	
	strcpy(pT,".prj");
//	CarryFile(strSrcPath,strGeoRange);
	CopyFile(strSrcPath,strGeoRange,TRUE);	

	return true;
}

inline void cpmeta(ImgInfo* pSrc,ImgInfo* pTarget,LPCSTR lpXml)
{
	if(!pSrc||!pTarget) return;
	CZY3SceneFile* pFile;
	pFile = (CZY3SceneFile*)LxLoadFile(GET_LOBINFO(CZY3SceneFile),pTarget->sImgPN);
	if(pFile){
		char strPath[512];	//strcpy(strPath,pSrc->sMetaPN);	strcpy(strrchr(strPath,'.'),"_tmp_mip.xml");
		pFile->GetMetaPath(strPath);	LogPrint(ERR_NONE,"Write Meta File:%s\n",strPath);
		pFile->GenerateMetaFile(lpXml,pSrc->sMetaPN,strPath);
		delete pFile;
// 		char strMeta[512];	pFile->GetMetaPath(strMeta);
// 		CarryFile(strPath,strMeta);
	}
}


void WriteRange(const char* lpstrImgPath)
{
	char strPath[512];	strcpy(strPath,lpstrImgPath);	strcpy(strrchr(strPath,'.'),"_range.txt");
	FILE* fp = fopen(strPath,"w");	if(!fp) return;
	fprintf(fp,"391.350159 2403.851563\n2390.621826 2005.420166\n1977.960815 2.591011 \n0.033614 411.694672");
	fclose(fp);

}

class CProdXml:public CMarkup
{
public:
	enum MODERET { modeSuccess=0x0000,modeFail=0x0001,modeHalf=0x0001 };//modeHalf=0x0002
public:
	CProdXml(){ m_pXml = NULL; m_nInToalNum = 0; m_tskType = 0; Reset(); }
	~CProdXml() {  }
	bool	Open(const char* lpstr,CPrjXml* pXml)	{ 
// 			if(!pPrj) return false;
// 			memcpy(&m_tHdr,&(pPrj->m_tHdr),sizeof(TskHdr));
// 			memcpy(&m_projectionInfo,&(pPrj->m_projectionInfo),sizeof(ProjectionInfo));
// 			m_prjXMLAttrib = pPrj->m_prjXMLAttrib;

			CMarkup xml;	
			{
				char strxmlPath[512];	strcpy(strxmlPath,lpstr);
				if( !xml.Load(strxmlPath) ) 
				{
					m_nSucess = modeFail;
					strcpy(m_strMsg,xml.GetError().c_str());
					LogPrint(ERR_FILE_OPEN,"Error:%s\t%s",lpstr,m_strMsg);
				}else {
					pXml = (CPrjXml*)LxLoadFile(GET_LOBINFO(CPrjXml),strxmlPath);
					if( !pXml )
					{
						m_nSucess = modeFail;
						sprintf(m_strMsg,"Error:Fail to Analysis the Xml File:%s!",strxmlPath);
						LogPrint(ERR_FILE_OPEN,"%s",m_strMsg);
					}
					m_pXml = pXml;
				}
			}

			SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"); 
			GetSiblingNode(&xml,"root");		AddElem("root");	
			xml.IntoElem();				IntoElem();	
			GetSiblingNode(&xml,"head");		AddElem("head");
			xml.IntoElem();				IntoElem();
			GetSiblingNode(&xml,"MessageType");		AddElem("MessageType",xml.GetData());
			GetSiblingNode(&xml,"MessageID");			AddElem("MessageID",xml.GetData());
			GetSiblingNode(&xml,"OriginatorAddress");	AddElem("OriginatorAddress",xml.GetData());
			GetSiblingNode(&xml,"RecipientAddress");	AddElem("RecipientAddress",xml.GetData());

			SYSTEMTIME	st; GetLocalTime( &st ); char strMsg[256]; 
			sprintf(strMsg, "%d-%02d-%02dT%02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond );
			GetSiblingNode(&xml,"CreationTime");		AddElem("CreationTime",strMsg);
			xml.OutOfElem();			OutOfElem();

			GetSiblingNode(&xml,"content");	AddElem("content");		
			xml.IntoElem();				IntoElem();
			GetSiblingNode(&xml,"TaskID");		AddElem("TaskID",xml.GetData());
			GetSiblingNode(&xml,"JobID");		AddElem("JobID",xml.GetData());
			//GetSiblingNode(&xml,"JobOperator");		AddElem("JobOperator",xml.GetData());
			GetSiblingNode(&xml, "ProductiveTaskItemNum");      AddElem("FinishedTaskItemNum", xml.GetData());
			GetSiblingNode(&xml, "ProductiveTaskItemList");
			xml.IntoElem();//ProductiveTaskItemList
			//AddElem("JobReportTime",strMsg);
			AddElem("TaskItemFinishedStateList"); 
			IntoElem();
			{
				AddElem("TaskItemFinishedState");
				IntoElem();
				{
					GetSiblingNode(&xml, "ProductiveTaskItemInfo");
					xml.IntoElem();//ProductiveTaskItemInfo
					{
						GetSiblingNode(&xml, "TaskItemID");     AddElem("TaskItemID", xml.GetData());
						GetSiblingNode(&xml, "TaskItemType");  AddElem("TaskItemType", xml.GetData());
					}
					AddElem("FinishedStatus", "EntireCompleted"); 
					AddElem("FinishedStatusDisc");
					AddElem("ProductInfoList");
					IntoElem();
					{
						if (m_pXml)
						{
							int tskType =m_tskType = m_pXml->m_tskTypeCur;
							if ((tskType&TSK_ATMCH) || (tskType&TSK_ATADJ)) { 
								AddElem("Type", "RPC"); AddElem("Level");  AddElem("OrganizationMode","Scene"); 
							}
							else if (tskType&TSK_DEMMCH) { 
								AddElem("Type", "Terrain"); AddElem("Level","DSM");  AddElem("OrganizationMode", "Scene");
							}
							else if (tskType&TSK_DEMFLT) strcpy(strMsg, "DEM-PD");
							else if (tskType&TSK_IMGEPI) strcpy(strMsg, "EPI-PD");
							else if (tskType&TSK_IMGREC) {
								AddElem("Type", "Image"); AddElem("Level", "GTC");  AddElem("OrganizationMode", "Scene");
							}
							else if (tskType&TSK_IMGFUS) {
								AddElem("Type", "Image"); AddElem("Level", "Other ");  AddElem("OrganizationMode", "Scene");
							}
							else if (tskType&TSK_IMGMSC) {
								AddElem("Type", "Image"); AddElem("Level", "Other ");  AddElem("OrganizationMode", "50000Map");
							}
							else {  AddElem("Type", "Unknown"); AddElem("Level", "Unknown ");  AddElem("OrganizationMode", "Unknown"); }
						}
					}
					//OutOfElem();
				}
				//OutOfElem();
			}
			//OutOfElem();
			
			
			
/*
			AddElem("ProductLevel",strMsg);
			AddElem("ProductType","BGRN");
			AddElem("ProductID","123");
			GetSiblingNode(&xml,"ProductOutPath");		AddElem("OutputPath",xml.GetData());

			AddElem("InnerQResult","2");
			AddElem("InnerQFileName");

			AddElem("OutFileList");
			IntoElem();*/

			GetSiblingNode(&xml, "OutCompleteFileName");
			MCD_STR str;
			str = xml.GetData();
			xml.OutOfElem();//ProductiveTaskItemInfo
			xml.OutOfElem();//ProductiveTaskItemList
			

			if(m_pXml)
			{
				char inifilepath[512];	char strOutPath[512];
				if( GetPrivateProfilePath(inifilepath,CONFIG_FILE) ) {
					GetPrivateProfileString("PathSetting","linuxpath","",strOutPath,256,inifilepath); VERF_SLASH(strOutPath);	m_nHeadLen = strlen(strOutPath)+1;
					if( !IsExist(strOutPath) )  { GetPrivateProfileString("PathSetting","windowspath","",strOutPath,256,inifilepath); VERF_SLASH(strOutPath);	m_nHeadLen = strlen(strOutPath)+1; }
					if( !IsExist(strOutPath) )  { strcpy(strOutPath,""); m_nHeadLen = 0; }	else strcat(strOutPath,"/"); 
				}
				LogPrint(0,"Len_Path_Head = %d ",m_nHeadLen);
				sprintf(m_strFilePath,"%s%s",strOutPath,str.c_str());//获取完成单的完整路径
//				strcpy(m_strFilePath,str.c_str());
			}
			FILE* fp = NULL;
			if(m_strFilePath[0]!=0) fp = fopen(m_strFilePath,"w");
			if(!fp) {//如果找不到这个文件//就重新命名
				strcpy(m_strFilePath,lpstr);
				strcpy(strrchr(m_strFilePath,'.'),"_complete.xml");
			}else { fclose(fp); DeleteFile(m_strFilePath);}
		
		if(m_nSucess!=modeSuccess) { Close(); return false; }

		return true;
		
	}
	void	Write(ImgInfo* pImg){
		AddElem("ProductInfo");
		IntoElem();
		
		if ((m_tskType&TSK_ATMCH) || (m_tskType&TSK_ATADJ)) {
			AddElem("ProductType", "CALIB_RPC");
			AddElem("Path", pImg->sImgPN);
		}
		else if (m_tskType&TSK_DEMMCH) {
			AddElem("ProductType", "DSM_SCENE");
			AddElem("Path", pImg->sImgPN);

		}
		else if (m_tskType&TSK_DEMFLT);
		else if (m_tskType&TSK_IMGEPI) ;
		else if (m_tskType&TSK_IMGREC) {
			AddElem("ProductType", "GTC");
			AddElem("Path", pImg->sImgPN);

		}
		else if (m_tskType&TSK_IMGFUS) {
			AddElem("ProductType", "FUS");
			AddElem("Path", pImg->sImgPN);

		}
		else if (m_tskType&TSK_IMGMSC) {
			AddElem("ProductType", "MSC");
			AddElem("Path", pImg->sImgPN);
		}
		
		OutOfElem();
		m_nImgNum++;	
	}
	void	Write(const char* str){
		static int id = 0;
		AddElem("OutFile");
		IntoElem();
		AddElem("DataID",id++);
		
		char strVal[512];	strcpy(strVal,str);
		if( strrchr(strVal,'.') ) *strrpath(strVal) = 0;
		
		AddElem("OutFileName",strVal+m_nHeadLen);
		AddElem("OutFileSize",500);
		AddElem("IfArchive","0");
		AddElem("OutFileType","0");
		// 		AddElem("ZoneType",m_projectionInfo.nZoneType);
		// 		AddElem("ZoneNo",m_projectionInfo.nZoneNo);
		OutOfElem();
		m_nImgNum++;	
	}
	void	Close(){
		if(*m_strFilePath) {
			ResetPos();
			FindElem("root");	IntoElem();
			FindElem("content");IntoElem();
			FindElem("TaskItemFinishedStateList");	IntoElem();
			FindElem("TaskItemFinishedState"); IntoElem();
			if (m_nImgNum<=0)
			{
				FindElem("FinishedStatus"); SetData("NotComplete");
				FindElem("FinishedStatusDisc"); SetData("NotComplete");
			}
			else if (m_nImgNum<m_nInToalNum)
			{
				FindElem("FinishedStatus"); SetData("PartiallyCompleted"); 
				FindElem("FinishedStatusDisc"); SetData("PartiallyCompleted");
			}
			else{ 
				FindElem("FinishedStatus"); SetData("EntireCompleted"); 
				FindElem("FinishedStatusDisc"); SetData("EntireCompleted");
			}

			FindElem("ProductInfoList"); 
			InsertElem("TemporaryResult"); IntoElem();
			InsertElem("IsExportTemporaryData","FALSE"); 
			InsertElem("TemporaryDataPath");

			Save(m_strFilePath);
			LogPrint(ERR_NONE,"Save Complete XML:%s!",m_strFilePath);
			Reset();
		}
	}
	void SetRetMsg(MODERET mode,LPCSTR strMsg)
	{
		m_nSucess = mode;	
		if( strlen(m_strMsg)>2048-512 ) return;
		strcat(m_strMsg,strMsg);

	}
// public:
// 	enum OPENFLAGS { modeModify= 0x0000,modeCreate = 0x1000 };
protected:
	char			m_strFilePath[512];
	MODERET			m_nSucess;
	char			m_strMsg[2048];
	int				m_nImgNum;
	int			m_nInToalNum;
	CPrjXml*		m_pXml;
	int				m_nHeadLen;
	int m_tskType;
//	OPENFLAGS		m_mode;
	//TskHdr			m_tHdr;
// 	ImgInfo*		m_ImgInf;
// 	int				m_nImgNum;
// 	CArray_SCENEINF m_scList;
// 	ProjectionInfo	m_projectionInfo;
// 	XML_Type		m_prjXMLAttrib;
private:
	void Reset()
	{
		if( m_pXml ) delete m_pXml; m_pXml = NULL;
		m_nSucess = modeSuccess;
		m_strMsg[0] = 0;
		m_nImgNum=0;
		m_nHeadLen = 0;
	}
};

void MoveMap(const char* oldDir,const char* newDir,CProdXml& prodXml)
{
	CSrchFl findFile;
	if( !findFile.Open(oldDir) ) return;
	
	int	dir_num = findFile.GetDirNum();	
	
	int i;	char strOldPath[512],strNewPath[512],strName[128];
	strcpy(strNewPath,newDir);	char* pT = AddEndSlash(strNewPath)+1;
	for (i=0; i<dir_num; i++){
		findFile.GetDirPath(i,strOldPath);	findFile.GetDirName(i,strName);
		strcpy(pT,strName);
		rename(strOldPath,strNewPath);	prodXml.Write(strNewPath);
	}
	
}


void CarryEpiImg(const char* lpstrInterior,const char* lpstrExternal,const char* lpstrSc)
{
	char strSrcPath[512],strDstPath[512];

	strcpy(strSrcPath,lpstrInterior);	strcpy(strDstPath,lpstrExternal);
	if( !IsExist(strSrcPath) ) { LogPrint(ERR_NONE,"Fail to Create %s!",strDstPath); return ; }
	CopyFile(strSrcPath,strDstPath,TRUE);
	LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);

	char* pS1 = strrchr(strSrcPath,'.');	char* pT1 = strSrcPath + strlen(strSrcPath);
	char* pS2 = strrchr(strDstPath,'.');	char* pT2 = strDstPath + strlen(strDstPath);

	strcpy(pT1,".RPC");	strcpy(pS2,"_rpc.txt");
	if(!IsExist(strSrcPath))	strcpy(pT1,".rpc");	
	if( !IsExist(strSrcPath) )  LogPrint(ERR_NONE,"Fail to Create %s!",strDstPath);	
	else LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);	
	CopyFile(strSrcPath,strDstPath,TRUE);

	strcpy(pS1,".JPG");		strcpy(pS2,"_pre.jpg");
	if(!IsExist(strSrcPath))	strcpy(pS1,".jpg");		
	if( !IsExist(strSrcPath) ) LogPrint(ERR_NONE,"Fail to Create %s!",strDstPath);
	else LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);
	CopyFile(strSrcPath,strDstPath,TRUE);
	
	strcpy(pS1,".THUMB.JPG");	strcpy(pS2,"_ico.jpg");
	if(!IsExist(strSrcPath))	strcpy(pS1,".THUMB.jpg");	
	if( !IsExist(strSrcPath) ) LogPrint(ERR_NONE,"Fail to Create  %s!",strDstPath);
	else LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);
	CopyFile(strSrcPath,strDstPath,TRUE);

	strcpy(pS1,"_range.txt");	strcpy(pS2,"_range.txt");
	if(!IsExist(strSrcPath))	strcpy(pS1,"_RANGE.TXT");	
	if( !IsExist(strSrcPath) ) LogPrint(ERR_NONE,"Fail to Create  %s!",strDstPath);
	else LogPrint(ERR_NONE,"Save %s Successfully",strDstPath);
	CopyFile(strSrcPath,strDstPath,TRUE);

	char strFileName[256];	
	strcpy(strDstPath,lpstrExternal); Dos2Unix(strDstPath);
	char* pPath = strrchr(strDstPath,'/');
	strcpy(strFileName,pPath+1);	strcpy(pPath,"/GeoRange/");	CreateDir(strDstPath);	strcat(strDstPath,strFileName);
	pS2 = strrchr(strDstPath,'.');

	strcpy(strSrcPath,lpstrInterior);	 Dos2Unix(strSrcPath);	pS1 = strrchr(strSrcPath,'.');

	strcpy(pS1,".shp");
	if(!IsExist(strSrcPath))	strcpy(pS1,".SHP");	
	strcpy(pS2,".shp");	
	CopyFile(strSrcPath,strDstPath,TRUE);
	
	strcpy(pS1,".dbf");
	if(!IsExist(strSrcPath))	strcpy(pS1,".DBF");	
	strcpy(pS2,".dbf");
	CopyFile(strSrcPath,strDstPath,TRUE);
	
	strcpy(pS1,".shx");
	if(!IsExist(strSrcPath))	strcpy(pS1,".SHX");	
	strcpy(pS2,".shx");
	CopyFile(strSrcPath,strDstPath,TRUE);
	
	strcpy(pS1,".prj");
	if(!IsExist(strSrcPath))	strcpy(pS1,".PRJ");	
	strcpy(pS2,".prj");
	CopyFile(strSrcPath,strDstPath,TRUE);
}

void CarryEpiFile(const char* lpstrNImgPath,const char* lpstrFImgPath,const char* lpstrBImgPath,
				  const char* lpstrNEpiPath,const char* lpstrFEpiPath,const char* lpstrBEpiPath,
				  ImgInfo* lpNImg,ImgInfo* lpFImg,ImgInfo* lpBImg,CProdXml& prodXml)
{
	char strMsg[1024];
	if( !IsExist(lpstrNEpiPath) ){ 
		LogPrint(ERR_EPI_EPIIMG,"Fail to Create Epipolar Image:%s !",strrpath(lpstrNEpiPath)+1); 
		sprintf(strMsg,"lost product: %s\n",strrpath(lpstrNEpiPath)+1);
		prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
		return; 
	}
	if( !IsExist(lpstrFEpiPath) ){ 
		LogPrint(ERR_EPI_EPIIMG,"Fail to Create Epipolar Image:%s !",strrpath(lpstrFEpiPath)+1); 
		sprintf(strMsg,"lost product: %s\n",strrpath(lpstrFEpiPath)+1);
		prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
		return; 
	}
	if( !IsExist(lpstrBEpiPath) ){ 
		LogPrint(ERR_EPI_EPIIMG,"Fail to Create Epipolar Image:%s !",strrpath(lpstrBEpiPath)+1); 
		sprintf(strMsg,"lost product: %s\n",strrpath(lpstrBEpiPath)+1);
		prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
		return; 
	}
	char* pT = NULL;

	char strN[512];
	char strArchFileName[128];	
	pT = strrpath(lpNImg->sImgPN);	strcpy(strArchFileName,pT+1);	*strrchr(strArchFileName,'.') = 0;	memcpy(strArchFileName+7,"tlc",3);
	strcpy(strN,lpNImg->sImgPN);	*strrpath(strN) = 0;	pT = strrpath(strN);	strcpy(pT+1,strArchFileName);	CreateDir(strN);	pT = strN+strlen(strN);
	*pT = '/';	pT++;
	strcpy(pT,strArchFileName);	strcat(pT,".xml");	

	
	char strTmp[512];
	{
		strcpy(strTmp,lpstrNEpiPath);	strcpy(strrchr(strTmp,'.'),".xml");

		writeEPIXml(lpstrFImgPath,lpstrNImgPath,lpstrBImgPath,
			lpstrFEpiPath,lpstrNEpiPath,lpstrBEpiPath,strTmp);
		CarryFile(strTmp,strN);
		prodXml.Write(strN);
	}
	
	
	char *pT2,*pT3;	
	strcpy(pT,"f-n/");	CreateDir(strN);	pT2 = strN+strlen(strN);
	pT3 = strrpath(lpNImg->sImgPN);	strcpy(pT2,pT3+1);	CarryEpiImg(lpstrNEpiPath,strN,lpstrNImgPath);//	prodXml.Write(strN);
	pT3 = strrpath(lpFImg->sImgPN);	strcpy(pT2,pT3+1);	CarryEpiImg(lpstrFEpiPath,strN,lpstrFImgPath);//	prodXml.Write(strN);

	strcpy(pT,"f-b/");	CreateDir(strN);	pT2 = strN+strlen(strN);
	pT3 = strrpath(lpBImg->sImgPN);	strcpy(pT2,pT3+1);	CarryEpiImg(lpstrBEpiPath,strN,lpstrBImgPath);//	prodXml.Write(strN);
	pT3 = strrpath(lpFImg->sImgPN);	strcpy(pT2,pT3+1);	CarryEpiImg(lpstrFEpiPath,strN,lpstrFImgPath);//	prodXml.Write(strN);

	strcpy(pT,"n-b/");	CreateDir(strN);	pT2 = strN+strlen(strN);
	pT3 = strrpath(lpNImg->sImgPN);	strcpy(pT2,pT3+1);	CarryEpiImg(lpstrNEpiPath,strN,lpstrNImgPath);//	prodXml.Write(strN);
	pT3 = strrpath(lpBImg->sImgPN);	strcpy(pT2,pT3+1);	CarryEpiImg(lpstrBEpiPath,strN,lpstrBImgPath);//	prodXml.Write(strN);

	remove(lpstrFEpiPath);	remove(lpstrNEpiPath);	remove(lpstrBEpiPath);
}

int main(int argc, char* argv[])
{
	const char* lpPrjPath = NULL;
	const char* lpTask = NULL;
	int i;
	for (i=0; i<argc-1; i++){
		if ( !strcmp(argv[i],"-l") ){
//			if( argv[++i][0] == '-') { printf("Error:Please Enter Task List!\n"); break;  } 
			while( i+1<argc && argv[i+1][0] != '-') { i++; }	if( argv[i][0] == '-') { printf("Error:Please Enter Task List!\n"); break;  } 
			lpTask = argv[i];
		}else
			if ( !strcmp(argv[i],"-i") ){
				if( argv[++i][0] == '-') { printf("Error:Please Enter Exec Parameter List!\n"); break;  } 
				lpPrjPath = argv[i];
			}
	}
	
	if ( !lpTask || !lpPrjPath){
		Usage();
	}

	char strExe[512];	
	if(argv[0][1]==':')		strcpy(strExe,argv[0]+2);	else strcpy(strExe,argv[0]);	Dos2Unix(strExe);

	TASK tskType;
	if (!strcmp(lpTask, "AtMch") && !strcmp(lpTask, "TieMch"))		tskType = TSK_ATMCH; else
	if(!strcmp(lpTask,"AtAdj"))		tskType = TSK_ATADJ;else
	if(!strcmp(lpTask,"ImgEpi"))		tskType = TSK_IMGEPI;else
	if(!strcmp(lpTask,"DemQMch"))		tskType = TSK_DEMMCH;else
	if(!strcmp(lpTask,"DemFlt"))		tskType = TSK_DEMFLT;else
	if(!strcmp(lpTask,"ImgRec"))		tskType = TSK_IMGREC;else
	if(!strcmp(lpTask,"ImgFus"))		tskType = TSK_IMGFUS;else
	if(!strcmp(lpTask,"ImgMxz"))		tskType = TSK_IMGMSC;else
	if(!strcmp(lpTask,"DemMap"))		tskType = TSK_DSM_MAP;else
	if(!strcmp(lpTask,"DomMap"))		tskType = TSK_IMG_MAP;else
	if(!strcmp(lpTask,"Order"))		tskType = TSK_END;else
	Usage();

	char strXml[512];		strcpy(strXml,lpPrjPath);

	char strLog[512];	char* pS;
	strcpy(strLog,strXml);
	pS = strrchr(strLog,'.');	strcpy(pS,"_ARCH.log");
	
	
	OpenLog(strLog);
	LogPrint(ERR_NONE,"Running:%s %s %s",strExe,lpTask,strXml);

	CProdXml prodXml;	CPrjXml* pXml=NULL;
	if( !prodXml.Open(strXml,pXml) ) { LogPrint(ERR_FILE_OPEN,"Error:Fail to Load File:%s!",strXml); return ERR_FILE_OPEN; }
	LogPrint(ERR_NONE,"File:%s is loaded.",strXml);

	CXmlPrj xmlPrj;	char strMsg[256];
	if( xmlPrj.LoadPrj(strXml) != RET_OK )
	{
		LogPrint(ERR_FILE_OPEN,"Can't Open %s!",strXml);
		sprintf(strMsg,"there is something wrong in %s\n",strXml);
		prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
		prodXml.Close();
		return -1;
	}
	LogPrint(ERR_NONE,"Open %s Successfully",strXml);

	char strWorkSpace[512];	strcpy(strWorkSpace,strXml);
	pS = strrchr(strWorkSpace,'/');	if(!pS) pS = strrchr(strWorkSpace,'\\');	*(pS) = 0;

	char strFileNam[215],strImgNam[215];	strcpy(strFileNam,pS+1);
	char* pN = strrchr(strFileNam,'.');

	char strTmp[512];	strcpy(strTmp,strWorkSpace);
	pS = strTmp+strlen(strTmp);

	char strGcdPath[512];	strcpy(strGcdPath,strXml);	Dos2Unix(strGcdPath);	strcpy(strrchr(strGcdPath,'/')+1,"ProdGeoSys.gcd");
	char strSetGeoCmd[1024];	strcpy(strSetGeoCmd,strExe);	
	strcpy(strrchr(strSetGeoCmd,'/')+1,"../RunPE ");	strcat(strSetGeoCmd,strExe);	strcpy(strrchr(strSetGeoCmd,'/')+1,"SetGeo.exe ");
	strcat(strSetGeoCmd,"-tfw -gcd ");	strcat(strSetGeoCmd,strGcdPath);
	char* pSetGeoCmd = strSetGeoCmd+strlen(strSetGeoCmd);

	char* pT;		

	
	
	switch (tskType)
	{
	case TSK_ATMCH:
		{
			char strATWork[512],*pT2;
			strcpy(pS,"/Product/AT/");
			strcpy(strATWork,strTmp);		pT2=AddEndSlash(strATWork);

			strcpy(pN,".sz");
			sprintf(pS,"/AT/ATMCH/%s",strFileNam);
			if( IsExist(strTmp) ) 
			{ strcpy(pT2+1,strFileNam); CopyFile(strTmp,strATWork,FALSE); LogPrint(ERR_NONE,"Save %s Successfully",strATWork); }

			strcpy(pN,"_Save.gcp");
			sprintf(pS,"/AT/ATMCH/%s",strFileNam);
			if( IsExist(strTmp) ) { CopyFile(strTmp,xmlPrj.m_sGCPPN,FALSE); LogPrint(ERR_NONE,"Save %s Successfully",xmlPrj.m_sGCPPN); }

			strcpy(pN,"_Tie.bpc");
			sprintf(pS,"/AT/ATMCH/%s",strFileNam);
			if( IsExist(strTmp) ) { CopyFile(strTmp,xmlPrj.m_sBPCPN,FALSE);	LogPrint(ERR_NONE,"Save %s Successfully",xmlPrj.m_sBPCPN); }

			ImgInfo* pImgInf;	
			for (i=0,pImgInf=xmlPrj.m_inImg;i<xmlPrj.m_inNum;i++,pImgInf++){
				pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
				strcpy(strImgNam,pT);	strcpy(strrchr(strImgNam,'.'),".ixy");
				sprintf(pS,"/AT/ATMCH/Work%s",strImgNam);	strcpy(pT2,strImgNam);
				if( IsExist(strTmp) ) { CopyFile(strTmp,strATWork,FALSE);	LogPrint(ERR_NONE,"Save %s Successfully",strATWork);  }
				else
				{
					sprintf(strMsg,"fail to save %s\n",strATWork);
					prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
				}
			}
// 				xmlPrj.m_tHdr.tskType = TT_ATADJ;
// 				xmlPrj.SaveTskHdr2Bin(xmlPrj.m_tHdr);

		}	
		break;
	case TSK_ATADJ:
		{
			char strRPCSrc[512],strRPCDst[512];
			strcpy(pS,"/AT/Product");	CreateDir(strTmp);//创建RPC归档目录	
			strcpy(strRPCDst,strTmp);	pN = strRPCDst+strlen(strRPCDst);//赋值给strRPCDst/../AT/Product//pN指向strRPCDst
			//strcpy(pS,"/AT/ATADJ/Images");	pS = strTmp+strlen(strTmp);//pS
			//归档平差报告
			char strAdjReportPN[MAX_FILEPATH];
			sprintf(strAdjReportPN, "%s%s", strWorkSpace, "/Product/AT/PAN/report/AdjustmentReport.txt");
			CopyFile(strAdjReportPN, xmlPrj.m_sAdjReportPN, FALSE);
			ImgInfo* pImgInf;	
			for (i=0,pImgInf=xmlPrj.m_inImg;i<xmlPrj.m_inNum;i++,pImgInf++){
				strRPCSrc[0] = 0;//清空strRPCSrc值
				pT = strrpath(pImgInf->sImgPN);//pT指向SC影像名
			/*pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
				strcpy(pS,pT);	strcpy(pS+strlen(pS),".aop");
				if( IsExist(strTmp) ) strcpy(strRPCSrc,strTmp);
				else{
					strcpy(strRPCSrc,pImgInf->sImgPN);
					strcpy(strRPCSrc+strlen(strRPCSrc),".aop");
					if(!IsExist(strRPCSrc)) strRPCSrc[0] = 0;
				}
				if(strRPCSrc[0] != 0) {
					if(filecmp_mtime(strRPCSrc,pImgInf->sAdjRPCPath)>0) CopyFile(strRPCSrc,pImgInf->sAdjRPCPath,FALSE);
					LogPrint(ERR_NONE,"Save %s Successfully",pImgInf->sAdjRPCPath);
				}*/

				strcpy(strRPCSrc,pImgInf->sAdjRPCPath);
				if(IsExist(strRPCSrc)){
					strcpy(pN,pT);	strcpy(pN+strlen(pN),".rpc");
					CopyFile(strRPCSrc,strRPCDst,FALSE);
					LogPrint(ERR_NONE,"Save %s Successfully",strRPCDst);
					prodXml.Write(pImgInf);
				}
			}
// 				xmlPrj.m_tHdr.tskType = TT_EPI;
// 				xmlPrj.SaveTskHdr2Bin(xmlPrj.m_tHdr);
		}
		break;
	case TSK_IMGEPI:
		{
			strcpy(pS,"/DOM/EPI/Product");	pS = strTmp+strlen(strTmp);
			int	nSceneNum;
			SCENEINF* pScene = xmlPrj.m_scList.GetData(nSceneNum);
			LogPrint(ERR_NONE,"Scence Num:%d",nSceneNum );

			char strF[512],strN[512],strB[512];
			for (i=0;i<nSceneNum;i++,pScene++){
				pT = strrpath(pScene->strNameImgF);
				if(!pT) { LogPrint(ERR_EPI_EPIIMG,"Warning:Scence %s May Lost FWD Image !",pScene->sID ); continue; }
				strcpy(pS,pT);	strcpy(strF,strTmp);
				pT = strrpath(pScene->strNameImgN);
				if(!pT) { LogPrint(ERR_EPI_EPIIMG,"Warning:Scence %s May Lost NAD Image !",pScene->sID ); continue; }
				strcpy(pS,pT);	strcpy(strN,strTmp);
				pT = strrpath(pScene->strNameImgB);
				if(!pT) { LogPrint(ERR_EPI_EPIIMG,"Warning:Scence %s May Lost BWD Image !",pScene->sID ); continue; }
				strcpy(pS,pT);	strcpy(strB,strTmp);
				CarryEpiFile(pScene->strNameImgN,pScene->strNameImgF,pScene->strNameImgB,strN,strF,strB,pScene->prodInf[IT_EPIN],pScene->prodInf[IT_EPIF],pScene->prodInf[IT_EPIB],prodXml);
			}
		}
		break;
	case TSK_DEMMCH:
		{
			strcpy(pS,"/DEM/Product");	pS = strTmp+strlen(strTmp);
			int	nSceneNum;
			SCENEINF* pScene = xmlPrj.m_scList.GetData(nSceneNum);
			
			char strMeta[512];
			for (i=0;i<nSceneNum;i++,pScene++){
				if(!pScene->prodInf[IT_DSM])  continue;
				sprintf(pS,"/%s",strrpath(pScene->prodInf[IT_DSM]->sImgPN)+1);	strcpy(strrchr(strTmp,'.'),".TIF");
				strcpy(strMeta,strTmp);	strcpy(strrchr(strMeta,'.'),".xml");
				
				writeDSMXml(pScene->strNameImgF,pScene->strNameImgN,pScene->strNameImgB,
				strTmp,strGcdPath,strMeta);

				if(!CarryImgFile(strTmp,pScene->prodInf[IT_DSM],false)) {
					if(pScene->prodInf[IT_DSM]) 
					{
						LogPrint(ERR_DEM_FLAG,"Fail to Create DSM :%s !",pScene->prodInf[IT_DSM]->sImgPN);
						sprintf(strMsg,"lost product: %s\n",pScene->prodInf[IT_DSM]->sImgPN);
					}else sprintf(strMsg,"lost DSM product in scene:%s\n",pScene->sID);
					prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					continue;
				}
				prodXml.Write(pScene->prodInf[IT_DSM]);		//WriteRange(pScene->prodInf[IT_DSM]->sImgPN);
//				cpmeta(pScene->imgNInf,pScene->prodInf[IT_DSM],strXml); 
// 				sprintf(pSetGeoCmd," -in %s -out %s",pScene->prodInf[IT_DSM]->sImgPN,pScene->prodInf[IT_DSM]->sImgPN);
// 				printf("Call:%s\n",strSetGeoCmd);	system(strSetGeoCmd);

			}
// 				xmlPrj.m_tHdr.tskType = TT_DEM;
// 				xmlPrj.SaveTskHdr2Bin(xmlPrj.m_tHdr);
		}
		break;
	case TSK_DEMFLT:
		{
			strcpy(pS,"/DEM/Product");	pS = strTmp+strlen(strTmp);
			int	nSceneNum;
			SCENEINF* pScene = xmlPrj.m_scList.GetData(nSceneNum);
			
			char strMeta[512];
			for (i=0;i<nSceneNum;i++,pScene++){
				if(!pScene->prodInf[IT_DEM])  continue;
				sprintf(pS,"/%s",strrpath(pScene->prodInf[IT_DEM]->sImgPN)+1);	
				strcpy(strMeta,strTmp);	strcpy(strrchr(strMeta,'.'),".xml");
				
				writeDEMXml(pScene->prodInf[IT_DSM]->sMetaPN,strMeta);
				
				if(!CarryImgFile(strTmp,pScene->prodInf[IT_DEM],false)) {
					if(pScene->prodInf[IT_DEM]) 
					{
						LogPrint(ERR_DEM_FLAG,"Fail to Create DEM :%s !",pScene->prodInf[IT_DEM]->sImgPN);
						sprintf(strMsg,"lost product: %s\n",pScene->prodInf[IT_DEM]->sImgPN);
					}else sprintf(strMsg,"lost DEM product in scene:%s\n",pScene->sID);
					prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					continue;
				}
				prodXml.Write(pScene->prodInf[IT_DEM]);
			}
			
// 				xmlPrj.m_tHdr.tskType = TT_DOM;
// 				xmlPrj.SaveTskHdr2Bin(xmlPrj.m_tHdr);
		}
		break;
	case TSK_IMGREC:
		{
			strcpy(pS,"/DOM/Product");	pS = strTmp+strlen(strTmp);
			
			int	nSceneNum;
			SCENEINF* pScene = xmlPrj.m_scList.GetData(nSceneNum);	ImgInfo* pImgInf;
			LogPrint(ERR_NONE,"Scence Num:%d",nSceneNum );
			
			for (i=0;i<nSceneNum;i++,pScene++){
				
				if ( pScene->prodInf[IT_DOMN] ){
					pImgInf = pScene->prodInf[IT_DOMN];
					pT = strrpath(pImgInf->sImgPN);
					strcpy(pS,pT);
					if(!CarryImgFile(strTmp,pImgInf)) {
						LogPrint(ERR_DEM_FLAG,"Fail to Create Rectified Image :%s !",pImgInf->sImgPN);
						sprintf(strMsg,"lost product: %s\n",pImgInf->sImgPN);
						prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					}else{
						prodXml.Write(pImgInf);
#ifndef MIP
						cpmeta(xmlPrj.m_inImg,pImgInf,strXml);
#endif
					}				
				}
				else if ( pScene->prodInf[IT_JJZN] ){
					pImgInf = pScene->prodInf[IT_JJZN];
					pT = strrpath(pImgInf->sImgPN);
//					pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
					strcpy(pS,pT);
					if(!CarryImgFile(strTmp,pImgInf)) {
						LogPrint(ERR_DEM_FLAG,"Fail to Create Rectified Image :%s !",pImgInf->sImgPN);
						sprintf(strMsg,"lost product: %s\n",pImgInf->sImgPN);
						prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					}else{
						prodXml.Write(pImgInf);
#ifndef MIP
						cpmeta(xmlPrj.m_inImg,pImgInf,strXml);
#endif

					}				
				}
				else if ( pScene->prodInf[IT_XTJZN] ){
					pImgInf = pScene->prodInf[IT_XTJZN];
					pT = strrpath(pImgInf->sImgPN);
//					pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
					strcpy(pS,pT);
					if(!CarryImgFile(strTmp,pImgInf)) {
						LogPrint(ERR_DEM_FLAG,"Fail to Create Rectified Image :%s !",pImgInf->sImgPN);
						sprintf(strMsg,"lost product: %s\n",pImgInf->sImgPN);
						prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					}else{
						prodXml.Write(pImgInf);
#ifndef MIP
						cpmeta(xmlPrj.m_inImg,pImgInf,strXml);
#endif

					}				
				}
				if ( pScene->prodInf[IT_DOMC] ){
					pImgInf = pScene->prodInf[IT_DOMC];
					pT = strrpath(pImgInf->sImgPN);
//					pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
					strcpy(pS,pT);
					if(!CarryImgFile(strTmp,pImgInf)) {
						LogPrint(ERR_DEM_FLAG,"Fail to Create Rectified Image :%s !",pImgInf->sImgPN);
						sprintf(strMsg,"lost product: %s\n",pImgInf->sImgPN);
						prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					}else{
						prodXml.Write(pImgInf);
#ifndef MIP
						cpmeta(xmlPrj.m_inImg,pImgInf,strXml);
#endif

					}				
				}
				else if ( pScene->prodInf[IT_JJZC] ){
					pImgInf = pScene->prodInf[IT_JJZC];
					pT = strrpath(pImgInf->sImgPN);
//					pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
					strcpy(pS,pT);
					if(!CarryImgFile(strTmp,pImgInf)) {
						LogPrint(ERR_DEM_FLAG,"Fail to Create Rectified Image :%s !",pImgInf->sImgPN);
						sprintf(strMsg,"lost product: %s\n",pImgInf->sImgPN);
						prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					}else{
						prodXml.Write(pImgInf);
#ifndef MIP
						cpmeta(xmlPrj.m_inImg,pImgInf,strXml);
#endif

					}				
				}
				else if ( pScene->prodInf[IT_XTJZC] ){
					pImgInf = pScene->prodInf[IT_XTJZC];
					pT = strrpath(pImgInf->sImgPN);
//					pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
					strcpy(pS,pT);
					if(!CarryImgFile(strTmp,pImgInf)) {
						LogPrint(ERR_DEM_FLAG,"Fail to Create Rectified Image :%s !",pImgInf->sImgPN);
						sprintf(strMsg,"lost product: %s\n",pImgInf->sImgPN);
						prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					}else{
						prodXml.Write(pImgInf);
#ifndef MIP
						cpmeta(xmlPrj.m_inImg,pImgInf,strXml);
#endif

					}				
				}
			}

// 				ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
// 				char metapath[512];	strcpy(metapath,xmlPrj.m_inImg->sMetaPN);
// 				for (i=0; i<num; i++,pImgInf++){
// 					pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
// 					strcpy(pS,pT);
// 					if(!CarryImgFile(strTmp,pImgInf)) {
// 						LogPrint(ERR_DEM_FLAG,"Fail to Create Rectified Image :%s !",pImgInf->sImgPN);
// 						sprintf(strMsg,"lost product: %s\n",pImgInf->sImgPN);
// 						prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
// 						continue;
// 					}
// 					prodXml.Write(pImgInf);
// 					cpmeta(xmlPrj.m_inImg,pImgInf,strXml); 
// 
// 				}
// 				xmlPrj.m_tHdr.tskType = TT_FUS;
// 				xmlPrj.SaveTskHdr2Bin(xmlPrj.m_tHdr);
		}
		break;
	case TSK_IMGFUS:
		{
			strcpy(pS,"/DOM/Product");	pS = strTmp+strlen(strTmp);

			ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
			for (i=0; i<num; i++,pImgInf++){
				pT = strrpath(pImgInf->sImgPN);
//				pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');
				strcpy(pS,pT);
				if(!CarryImgFile(strTmp,pImgInf)) {
					LogPrint(ERR_DEM_FLAG,"Fail to Create Rectified Image :%s !",pImgInf->sImgPN);
					sprintf(strMsg,"lost product: %s\n",pImgInf->sImgPN);
					prodXml.SetRetMsg(CProdXml::modeHalf,strMsg);
					continue;
				}
				prodXml.Write(pImgInf); 
#ifndef MIP
				cpmeta(xmlPrj.m_inImg,pImgInf,strXml);
#endif

			}
// 				xmlPrj.m_tHdr.tskType = TT_MSC;
// 				xmlPrj.SaveTskHdr2Bin(xmlPrj.m_tHdr);
		}
		break;
	case TSK_IMGMSC:
		{
			strcpy(pS,"/DOM/Product");	pS = strTmp+strlen(strTmp);

			ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
			char bbi[512];
			for (i=0; i<num; i++,pImgInf++){
				strcpy(bbi,pImgInf->sImgPN);
				pT = strrpath(bbi);
				strcpy(pS,pT);
				if(!CarryFile(strTmp,bbi))
				{ sprintf(strMsg,"lost product: %s\n",bbi);	prodXml.SetRetMsg(CProdXml::modeHalf,strMsg); continue; }
//				strcpy(strrchr(strTmp,'.'),".tfw");	strcpy(strrchr(bbi,'.'),".tfw");

				strcpy(strTmp+strlen(strTmp),".geo");	strcpy(bbi+strlen(bbi),".geo");
				if(!CarryFile(strTmp,bbi)){ sprintf(strMsg,"lost product: %s\n",bbi);	prodXml.SetRetMsg(CProdXml::modeHalf,strMsg); continue; }

				strcpy(strrchr(strTmp,'.'),".idx");	strcpy(strrchr(bbi,'.'),".idx");
				if(!CarryFile(strTmp,bbi)){ sprintf(strMsg,"lost product: %s\n",bbi);	prodXml.SetRetMsg(CProdXml::modeHalf,strMsg); continue; }

				strcpy(strrchr(strTmp,'.'),".inf");	strcpy(strrchr(bbi,'.'),".inf");
				if(!CarryFile(strTmp,bbi)){ sprintf(strMsg,"lost product: %s\n",bbi);	prodXml.SetRetMsg(CProdXml::modeHalf,strMsg); continue; }

				prodXml.Write(pImgInf); 
#ifndef MIP
				cpmeta(xmlPrj.m_inImg,pImgInf,strXml);
#endif

			}
// 				xmlPrj.m_tHdr.tskType = TT_ATMCH;
// 				xmlPrj.SaveTskHdr2Bin(xmlPrj.m_tHdr);
		}
		break;
	case  TSK_IMG_MAP:{
			strcpy(pS,"/DOM/Product");	pS = strTmp+strlen(strTmp);
			
			ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
			char bbi[512];
			for (i=0; i<num; i++,pImgInf++){
				strcpy(bbi,pImgInf->sImgPN);
				pT = strrpath(bbi);				
				*pT = 0;
				strcpy(pS,"/MAP");
				MoveMap(strTmp,bbi,prodXml);
			}
					  }
			break;
	case  TSK_DSM_MAP:{
		strcpy(pS,"/DEM/Product");	pS = strTmp+strlen(strTmp);
		
		ImgInfo* pImgInf = xmlPrj.m_outImg;	int num = xmlPrj.m_outNum;
		char strMapPath[512];

		strcpy(strMapPath,pImgInf->sImgPN);
		pT = strrpath(strMapPath);	*pT = 0;	
		pT = strrpath(strMapPath);	*pT = 0;
		
		MoveMap(strTmp,strMapPath,prodXml);
		
		}
			break;
	}
	if( strcmp(lpTask,"AtMch")!=0 )	prodXml.Close();

	LogPrint(ERR_NONE,"ARCH %s %s Over!",lpTask,strXml); 
	
	return 0;
}

