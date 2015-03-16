
#include "stdafx.h"
#include "MipXml.h"
#include "Markup.h"
#include "ZY3FileName.h"

IMPLEMENT_DYNFILELOAD(CMIPXml,CPrjXml)

// LPCSTR CMIPXml::m_strLvl[] = {"Unknow","STRIP","01a","EPI","DSM","DEM","GEC","JJZ","GTC","DOM","DOG","MSC"};
// 
inline bool CMIPXml::CompleteImageInfo(ImageInfo* pInfo)
{
	bool bNoError = true;
	CMIPFile* pFile;	pFile = (CMIPFile*)CMIPFile::identify(pInfo->path);
	if( pFile ){
		if( pFile->TinyLoad(pInfo->path,pInfo) != ERR_NONE) bNoError = false;	delete pFile;
	}
	else { if( CSASMACFile::TinyLoad(pInfo->path,pInfo) != ERR_NONE) bNoError = false;	}
	  
	return bNoError;									
}
// 
// bool CMIPXml::GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj /* = true */)
// {
// 	*retPath = 0;
// 	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
// 	char* pT = strrchr(strPath,'.');	if(!pT) return false;
// 
// 	if( lvl == IL_SC || lvl == IL_EPI ) {
// 		if( bAdj ) strcpy(pT,"_rpc_adj.txt");
// 		else strcpy(pT,"_rpc.txt");
// 	}else strcpy(pT,".tfw");
// 
// 	strcpy(retPath,strPath);
// 
// 	if( IsExist(strPath) ) return true;
// 	else return false;
// }
// 
// bool CMIPXml::GetThumbPath(LPCSTR strImgPath,char* retPath)
// {
// 	*retPath = 0;
// 	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
// 
// 	if( !strrcnct(strPath,'.',"_ico.jpg") ) return false;
// 	
// 	strcpy(retPath,strPath);
// 	
// 	if( IsExist(strPath) ) return true;
// 	else return false;
// }
// 
// bool CMIPXml::GetBrowsePath(LPCSTR strImgPath,char* retPath)
// {
// 	*retPath = 0;
// 	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
// 	
// 	if( !strrcnct(strPath,'.',"_pre.jpg") ) return false;
// 	
// 	strcpy(retPath,strPath);
// 	
// 	if( IsExist(strPath) ) return true;
// 	else return false;
// 
// }
// 
// bool CMIPXml::GetMetaPath(LPCSTR strImgPath,char* retPath)
// {
// 	*retPath = 0;
// 	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
// 	
// 	if( !strrcnct(strPath,'.',"-meta.xml") ) return false;
// 	
// 	strcpy(retPath,strPath);
// 	
// 	if( IsExist(strPath) ) return true;
// 	else return false;
// 
// }
// 
// bool CMIPXml::RenameImage(char* strPath,IMG_LVL lvl)
// {
// 	char* pN = strrchr(strPath,'/');	if(!pN) pN = strrchr(strPath,'\\');	if(!pN) pN = strPath-1;	pN++;
// 	memcpy(pN+4,m_strLvl[lvl],3);
// 
// 	return true;
// }

inline bool CMIPXml::GenProdImageInfo(LPIMAGEINF pProd,const ImageInfo* pImg,IMG_LVL lvl,LPCSTR strOutPath)
{
	char strPath[MAX_PATHNAME];	strcpy(strPath,strOutPath);	char* pS = strPath+strlen(strPath)-1;
	memcpy( pProd,pImg,sizeof(ImageInfo) );
	char* pN = strrpath(pProd->path);	strcpy(pS,pN);

	CZY3SceneFile* pFile;	pFile = (CZY3SceneFile*)CMIPFile::identify(strPath);
	if( pFile ){
		pFile->RenameImage(strPath,lvl);	delete pFile;
	}
	else { CSASMACFile file; file.RenameImage(strPath,lvl);	}
	char strName[256];	strcpy(strName,pS);
	pS = strrpath(strPath); pN = strrchr(pS,'.');	strcpy(pN,strName);//建一层文件夹
	strcpy(pProd->path,strPath);
	pProd->lvl = lvl;
	
	return true;
}

CLObject* CMIPXml ::identify(LPCSTR lpstrPath)
{
	char strLine[1024];
	FILE* fp = fopen(lpstrPath,"r");		if(!fp) return NULL; 

	bool	bType = false;

	for (int i=0; i<30;	i++){
		fgets(strLine,1024,fp);

		if( MatchString(strLine,MIP_XML_ONLY) != 0) { bType = true; break; }
	}
	fclose(fp);
	
	if( bType )	return CMIPXml::CreateLObject();
	else return NULL;
	
}

int CMIPXml :: Load4File(LPCSTR lpstrPath)
{
	printf("Analysis Mip xml ... \n");

	if( !CPrjXml::Open(lpstrPath) ) return ERR_FILE_OPEN;
	
	int ret;

	ret = AnalysisHead();
	if( ret != ERR_NONE )  return ret;

	ret = AnalysisImport();
	if( ret != ERR_NONE )  return ret;

	ret = AnalysisExport();
	if( ret != ERR_NONE )  return ret;

	char strTmp[512];	strcpy(strTmp,lpstrPath);
	char* pS = strrchr(strTmp,'/');	if(!pS) pS = strrchr(strTmp,'\\');
	strcpy(pS,"/ProdGeoSys.gcd");

	ret = AnalysisExtraPar(strTmp);//在该函数调用之前，需要先解析出ProdGeoSys.gcd，但匹配平差任务不需要这个文件
	if( ret != ERR_NONE )  return ret;

	AnalyzeMipPar();
	
	UpdataTskList();
	printf("Finish Analysis Mip xml.\n");

	return CPrjXml::Load4File(lpstrPath);
}

int CMIPXml::AnalysisHead()
{
	char strVal[512];
	if( FindNode(Mode_Absolute,NULL,2,"root","head") )
	{

	}
	bool bExtentMap = false;
	if ( FindNode(Mode_Absolute,NULL,4,"root","content","ProductList","Product") ){
		if( FindChildNode("ExtentType",strVal) != NOT_GET )	 bExtentMap = (atoi(strVal)==50)?true:false ;//此时的bExtentMap存储的是分幅标志
	}
	if (  FindNode(Mode_Absolute,NULL,2,"root","content") ){
		IntoNode();
		FindSibingNode("TaskID",strVal);	strncpy(m_hdr.jobName,strVal,sizeof(m_hdr.jobName)-1 );
		FindSibingNode("MJMJobType",strVal);
		int tskType;
		if(!strcmp(strVal,"PCJS"))		tskType = TSK_ATMCH|TSK_ATADJ;else
		if(!strcmp(strVal,"DSM-PD"))	{
			if(bExtentMap) { tskType = TSK_DEMMCH|TSK_DSM_MAP; }
			else tskType = TSK_DEMMCH; 
		}
		else	if(!strcmp(strVal,"DEM-PD"))	{ 
			if(bExtentMap) { tskType = TSK_DEMFLT|TSK_DEM_MAP;}
			else tskType = TSK_DEMFLT; 
		}
		else	if(!strcmp(strVal,"EPI-PD"))	tskType = TSK_IMGEPI;else
		if(!strcmp(strVal,"JZ"))		tskType = TSK_IMGREC;else
		if(!strcmp(strVal,"RH"))		tskType = TSK_IMGFUS;else
		if(!strcmp(strVal,"XQ"))	{
			if(bExtentMap) { tskType = TSK_IMGMSC|TSK_IMG_MAP; }
			else tskType = TSK_IMGMSC; 
		}else
		if(!strcmp(strVal,"PCBJ"))		tskType = TSK_ATADJ;else
		{ ErrorPrint("Error: Unknown Job Type :%s!",strVal);	return ERR_FILE_READ;	}
								
		m_tskTypeCur = tskType;	m_tskTypeList |= tskType;
	}else	{ ErrorPrint("Error: Can't find the node: <content> !");	return ERR_FILE_READ; }

	return ERR_NONE;
}

int CMIPXml::AnalysisImport()
{
	char strVal[512];

	if (  FindNode(Mode_Absolute,strVal,3,"root","content","InputCount") && ( FindSibingNode("InputFileList") != NOT_GET ) ){
		IntoNode();

		int num_import = atoi(strVal);
		m_import.SetSize( num_import );//设置m_improt的大小
		m_scList.Reset();

		ImageInfo info;	memset(&info,0,sizeof(ImageInfo));

		for (int i=0; i<num_import; i++){

			if( FindSibingNode("InputFile") != FORWARD_GET ) break;//FOEWARD_GET是什么//向前寻找到该元素

			FindChildNode("DataID",strVal);	
			FindChildNode("InputFileName",info.path);	CompleteImageInfo(&info);//根据影像命名规范,补充影像的其他信息,如相机类型、轨道号、景号、影像级别等
			//如果ino.path中的命名无法解析，则提示Warning:Image Name %s is not standard!
//			info.nID = atoi(strVal);
			//////////////////////////////////////////////////////////////////////////
			//读取xml里有关影像信息的字段 
			FindChildNode("InputFileType",strVal);

/*
			switch (switch_on)
			{
			case TSK_IMGREC:
			case TSK_DEMMCH:
			default:
				break;
			}*/
			if( !strcmp(strVal,"SC") ) {
				info.lvl = IL_SC;
			}else 
			if( !strcmp(strVal,"DSM") ) {	
				info.lvl = IL_DSM;
			}else 
			if( !strcmp(strVal,"DEM") ) {	
				info.lvl = IL_DEM;
			}else
			if( !strcmp(strVal,"GTC") ) {	
				info.lvl = IL_DOM;
			}else
			if( !strcmp(strVal,"DOM") ) {	
				info.lvl = IL_FUS;
			}

			if(info.lvl != IL_UNKNOW ) m_import.Append(info);//如果不能识别其影像级别，则不解析//外部dem还是可以解析的
		}
	}else	{ ErrorPrint("%s",GetXmlError() );	return ERR_FILE_READ; }
	//////////////////////////////////////////////////////////////////////////
	if( m_import.GetSize() == 0 )  { ErrorPrint("There is no import data in xml!"); return ERR_FILE_READ; }

	//////////////////////////////////////////////////////////////////////////
	//AppendImageArray函数作用：将m_import里的所有影像均组织到相应的景中
	m_scList.AppendImageArray(m_import);
	//////////////////////////////////////////////////////////////////////////
	return ERR_NONE;
}

int CMIPXml::AnalysisExport()
{
	char strOutPath[MAX_PATHNAME]="";	char strTmp[256];

	{
		char inifilepath[512];	
		if( GetPrivateProfilePath(inifilepath,CONFIG_FILE) ) {//首先找到FlowControl.ini文件
			GetPrivateProfileString("PathSetting","linuxpath","",strOutPath,256,inifilepath); VERF_SLASH(strOutPath);//获取linuxpath
			if( !IsExist(strOutPath) )  { GetPrivateProfileString("PathSetting","windowspath","",strOutPath,256,inifilepath); VERF_SLASH(strOutPath); }//linuxpath不存在时,获取windowspath
			if( !IsExist(strOutPath) )  strcpy(strOutPath,"");	else strcat(strOutPath,"/"); //若windowspath也不存在,则将strOutPath置空
		}
		strcpy(m_strProductSpace,strOutPath);	strcpy(m_strProcessSpace,strOutPath);

		if ( !FindNode(Mode_Absolute,strTmp,3,"root","content","ProductOutPath") )
		{ ErrorPrint("%s",GetXmlError() );	return ERR_FILE_READ; }
		strcat(strOutPath,strTmp);	strcat(m_strProductSpace,strTmp);
		if ( !FindNode(Mode_Absolute,strTmp,3,"root","content","ProcessOutPath") )
		{ ErrorPrint("%s",GetXmlError() );	return ERR_FILE_READ; }
		strcat(m_strProcessSpace,strTmp);
		AddEndSlash(m_strProcessSpace);		AddEndSlash(m_strProductSpace);
	}

	AddEndSlash(strOutPath);		

	int i,num;	SceneInfo* scInf = m_scList.GetData(num);
	m_export.Reset(NULL,100);

	ImageInfo info;	memset(&info,0,sizeof(ImageInfo));
	LPIMAGEINF pInfo;
//	int nExCurNum = 0;	int nExtLasNum = 0;
	int tskType = m_tskTypeCur;
	if (tskType&TSK_ATMCH){
	}
	if ( tskType&TSK_ATADJ ){
//		strcpy(m_importEx[i].strAdjRpcNam, "");
	}
	if ( tskType&TSK_IMGEPI ){
		LPIMAGEINF	pImgF,pImgN,pImgB;
		for (i=0;  i<num;  i++){
			pImgF = m_scList.GetImage(i,IL_SC,CT_FWD);
			pImgN = m_scList.GetImage(i,IL_SC,CT_NAD);
			pImgB = m_scList.GetImage(i,IL_SC,CT_BWD);

			if( !pImgF || !pImgN || !pImgB ) continue;
			
			GenProdImageInfo(&info,pImgF,IL_EPI,strOutPath);	m_export.Append(info);
			GenProdImageInfo(&info,pImgN,IL_EPI,strOutPath);	m_export.Append(info);
			GenProdImageInfo(&info,pImgB,IL_EPI,strOutPath);	m_export.Append(info);

		}
// 		pInfo = m_export.GetData(nExCurNum);
// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;	
	}
	if ( tskType&TSK_DEMMCH ){
		LPIMAGEINF	pImgF,pImgN,pImgB;
		for (i=0;  i<num;  i++){
			pImgF = m_scList.GetImage(i,IL_SC,CT_FWD);
			pImgN = m_scList.GetImage(i,IL_SC,CT_NAD);
			pImgB = m_scList.GetImage(i,IL_SC,CT_BWD);
			
			if( !pImgF || !pImgN || !pImgB ) continue;

			GenProdImageInfo(&info,pImgN,IL_DSM,strOutPath);	m_export.Append(info);
		}
// 		pInfo = m_export.GetData(nExCurNum);
// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}
	if ( tskType&TSK_DEMFLT ){
		LPIMAGEINF	pDSM;
		for (i=0;  i<num;  i++){
			pDSM = m_scList.GetImage(i,IL_DSM);
						
			if( !pDSM ) continue;
			GenProdImageInfo(&info,pDSM,IL_DEM,strOutPath);	m_export.Append(info);	
		}
// 		pInfo = m_export.GetData(nExCurNum);
// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}
	if ( tskType&TSK_IMGREC ){
		char strVal[50] = "eGEC";
		if ( FindNode(Mode_Absolute,NULL,4,"root","content","ProductList","Product") ){
			if( FindChildNode("ProductLevel",strVal) == NOT_GET )	strcpy(strVal,"eGEC"); //老订单中会有这个信息的
		}
		LPIMAGEINF	pImgF,pImgN,pImgB,pImg,pImgC;
		LPIMAGEINF pDSM,pDEM,pTer;
		pImg = pTer = NULL;
		for (i=0;  i<num;  i++){
			pImgF = m_scList.GetImage(i,IL_SC,CT_FWD);
			pImgN = m_scList.GetImage(i,IL_SC,CT_NAD);
			pImgB = m_scList.GetImage(i,IL_SC,CT_BWD);
			pImgC = m_scList.GetImage(i,IL_SC,CT_MUX);
			pDSM = m_scList.GetImage(i,IL_DSM);
			pDEM = m_scList.GetImage(i,IL_DEM);
			
			if( pImgN ) pImg = pImgN;else if( pImgF ) pImg = pImgF; else if( pImgB ) pImg = pImgB;	else if( !pImgC ) continue;

			if ( !strcmp(strVal,"GEC") ){
				if(pImg)	{	GenProdImageInfo(&info,pImg ,IL_CJZ,strOutPath);	m_export.Append(info);	}
				if(pImgC)	{	GenProdImageInfo(&info,pImgC,IL_CJZ,strOutPath);	m_export.Append(info);	}
			}else{
				if(pImg)	{	GenProdImageInfo(&info,pImg ,IL_JJZ,strOutPath);	m_export.Append(info);	}//写了一个info，仅只更新产品信息中的lvl和path，其他影像信息同原始的影像信息	
				if(pImgC)	{	GenProdImageInfo(&info,pImgC,IL_JJZ,strOutPath);	m_export.Append(info);	}
			}

			if( pDSM ) pTer = pDSM;else if( pDEM ) pTer = pDEM;else continue;
			
			if(pImg)	{	GenProdImageInfo(&info,pImg ,IL_DOM,strOutPath);	m_export.Append(info);	}
			if(pImgC)	{	GenProdImageInfo(&info,pImgC,IL_DOM,strOutPath);	m_export.Append(info);	}
		}
// 		pInfo = m_export.GetData(nExCurNum);
// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}
	if ( tskType&TSK_IMGFUS ){
		LPIMAGEINF	pImg = NULL,pImgC = NULL;
		LPSCENEINFO pscene=NULL;
		for (i=0;  i<num;  i++){
			pImg = NULL,pImgC = NULL;
			pscene = &( m_scList.GetAt(i) );
			pInfo = pscene->imgList.p?*(pscene->imgList.p)+(pscene->imgList.id):NULL;
			while (pInfo){
				if( pInfo->cam == CT_MUX ) {
					if( pInfo->lvl == IL_DOM) pImgC = pInfo;else
					if( pInfo->lvl == IL_JJZ && (!pImgC || (pImgC && pImgC->lvl<IL_JJZ)) ) pImgC = pInfo;else
					if( pInfo->lvl == IL_CJZ && !pImgC ) pImgC = pInfo;
				}else
				if (pInfo->cam != CT_UNKNOW ){
					if( pInfo->lvl == IL_DOM) pImg = pInfo;else
					if( pInfo->lvl == IL_JJZ && (!pImg || (pImg && pImg->lvl<IL_JJZ)) ) pImg = pInfo;else
					if( pInfo->lvl == IL_CJZ && !pImg ) pImg = pInfo;
				}
				pInfo = pInfo->pNext.p?*(pInfo->pNext.p)+(pInfo->pNext.id):NULL;
			}

			if( !pImg || !pImgC) continue;

			GenProdImageInfo(&info,pImg ,IL_FUS,strOutPath);	m_export.Append(info);
		}
// 		pInfo = m_export.GetData(nExCurNum);
// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}
	if ( tskType&TSK_IMGMSC ){
		info.lvl = IL_MSC;
		sprintf(info.path,"%sZY3_MSC_%s_MSC.bbi",strOutPath,m_hdr.jobName);	m_export.Append(info);
// 		pInfo = m_export.GetData(nExCurNum);
// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}

	m_scList.AppendImageArray(m_export);//写到相应的景中
//	if( m_export.GetSize() == 0 )  { ErrorPrint("There is no export information in xml!"); return ERR_FILE_READ; }
	return ERR_NONE;
}

int CMIPXml::AnalysisExtraPar(LPCSTR lpstrGcd)
{
	memset(&m_projectionInfo,0,sizeof(ProjectionInfo));
	m_projectionInfo.nEarthModelID	= 0;
	m_projectionInfo.nMapProjID		= 3;
	m_projectionInfo.fPrjScale		= 0.99960;
	m_projectionInfo.nPrjHemi			= 1;
	m_projectionInfo.fPrjOriLat		= 0;
	m_projectionInfo.fPrjCenMeri		= 117;
	m_projectionInfo.fPrjFalseNor		= 0;
	m_projectionInfo.fPrjFalseEas		= 500000;
	m_projectionInfo.nElevationID		= 0;

	FindNode(Mode_Absolute,m_prodInfo.strTempletPath,3,"root","content","DogTempletPath");
	

	m_prodInfo.fDemGSD = 25;	m_prodInfo.fDomGSD = 2.5;	m_prodInfo.fAverHeight = 500;

	char strVal[512];
	if( FindNode(Mode_Relative,NULL,2,"InputFileList","InputFile") )
	{		
		if( FindChildNode("ZoneType",strVal) != NOT_GET )	m_projectionInfo.nZoneType = atoi(strVal);
		if( FindChildNode("ZoneNo",strVal) != NOT_GET )		m_projectionInfo.nZoneNo = atoi(strVal);
		m_projectionInfo.fPrjCenMeri = m_projectionInfo.nZoneType*m_projectionInfo.nZoneNo;
	}
	if ( FindNode(Mode_Absolute,NULL,4,"root","content","ProductList","Product") ){
		if( FindChildNode("ProjectionParameters",strVal) != NOT_GET )	m_projectionInfo.fPrjScale = atof(strVal);
		if( FindChildNode("BursaSevenParameters",strVal) != NOT_GET )
		{ double* pBur = m_projectionInfo.fBur7Par; sscanf(strVal,"%lf%*c%lf%*c%lf%*c%lf%*c%lf%*c%lf%*c%lf",pBur,pBur+1,pBur+2,pBur+3,pBur+4,pBur+5,pBur+6); }
		
	}
	
	FindNode(Mode_Absolute,NULL,2,"root","content");
	if (FindChildNode("ProdAverHeight", strVal) != NOT_GET)	m_prodInfo.fAverHeight = (float)atof(strVal);
	if (FindChildNode("ProdDemGsd", strVal) != NOT_GET)		m_prodInfo.fDemGSD = (float)atof(strVal);
	if( FindChildNode("ProdDomGsd",strVal) != NOT_GET )		m_prodInfo.fDomGSD = (float)atof(strVal);
		
	if( m_prodInfo.fDemGSD < MIN_DEMGSD ) m_prodInfo.fDemGSD = MIN_DEMGSD;
	if( m_prodInfo.fDomGSD < MIN_DOMGSD )  m_prodInfo.fDomGSD = MIN_DOMGSD;

	if( !lpstrGcd || !IsExist(lpstrGcd) && !SaveProdGcdFile(lpstrGcd,m_projectionInfo) ) { return ERR_FILE_OPEN;	}//在这里会生产出一个ProdGeoSys.gcd文件

	strcpy(m_prodInfo.strGcdPath,lpstrGcd);

	return ERR_NONE;
}

int CMIPXml::AnalyzeMipPar(LPCSTR lpstrPath)
{
	if(lpstrPath) if( !Open(lpstrPath) ) return ERR_FILE_READ;
	FindNode(Mode_Absolute,m_strWorkDir,3,"root","content","ProductOutPath");
	FindSibingNode("OutJobStatusFileName",m_strStatusPath);
	FindSibingNode("OutJobCompleteFileName",m_strCompletePath);
	return ERR_NONE;
}

inline void GetStdCurTime(char* strTime)
{
	time_t lt = time(0); char* pS;
	struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
	sprintf(strTime, "%d",st.tm_year+1900);
	pS = strTime + strlen(strTime);
	if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
	pS = strTime + strlen(strTime);
	if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
	pS = strTime + strlen(strTime);
	if( st.tm_hour < 10 )  sprintf(pS,"T0%d",st.tm_hour);	else sprintf(pS,"T%d",st.tm_hour);
	pS = strTime + strlen(strTime);
	if( st.tm_min < 10 )  sprintf(pS,":0%d",st.tm_min);	else sprintf(pS,":%d",st.tm_min);
	pS = strTime + strlen(strTime);
	if( st.tm_sec < 10 )  sprintf(pS,":0%d",st.tm_sec);	else sprintf(pS,":%d",st.tm_sec);
}

bool CMIPXml::GetStatusPath(LPCSTR lpstrJobXmlPath,LPCSTR lpstrDefPath,char* strStatusPath)
{
	char strDef[512],strRelPath[256];	strcpy(strDef,lpstrDefPath);
	CXmlFile jobXml;	jobXml.Open(lpstrJobXmlPath);
	if ( !jobXml.FindNode(Mode_Absolute,strRelPath,3,"root","content","OutJobStatusFileName") ){
		strcpy(strStatusPath,strDef);	return false;
	}
	
	char inifilepath[512];	
	if( GetPrivateProfilePath(inifilepath,CONFIG_FILE) ) {
		GetPrivateProfileString("PathSetting","linuxpath","",strDef,256,inifilepath); VERF_SLASH(strDef);
		if( !IsExist(strDef) )  { GetPrivateProfileString("PathSetting","windowspath","",strDef,256,inifilepath); VERF_SLASH(strDef); }
		if( !IsExist(strDef) )  strcpy(strDef,"");	else strcat(strDef,"/"); 
	}
	strcat(strDef,strRelPath);
	strcpy(strStatusPath,strDef);
	return true;

}

bool CMIPXml::WriteStatusXml(LPCSTR lpstrJobXmlPath,LPCSTR lpstrRetXmlPath/* =NULL */)
{
	CXmlFile jobxml,retxml;
	if( !jobxml.Open(lpstrJobXmlPath) ) { printf("Error: Fail to Open job xml:%s!\n",lpstrJobXmlPath); return false;	}
	
	retxml.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"); 

	jobxml.FindSibingNode("root");				retxml.AddNode("root");
	jobxml.IntoNode();							retxml.IntoNode();
	
	char  strVal[512];
	jobxml.FindSibingNode("head");				retxml.AddNode("head");
	jobxml.IntoNode();							retxml.IntoNode();
	jobxml.FindSibingNode("MessageType");		jobxml.GetData(strVal);	retxml.AddNode("MessageType",strVal);
	jobxml.FindSibingNode("MessageID");			jobxml.GetData(strVal);	retxml.AddNode("MessageID",strVal);
	jobxml.FindSibingNode("OriginatorAddress");	jobxml.GetData(strVal);	retxml.AddNode("OriginatorAddress",strVal);
	jobxml.FindSibingNode("RecipientAddress");	jobxml.GetData(strVal);	retxml.AddNode("RecipientAddress",strVal);

	char strMsg[256];	GetStdCurTime(strMsg);	
	jobxml.FindSibingNode("CreationTime");		retxml.AddNode("CreationTime",strMsg);
	jobxml.OutNode();							retxml.OutNode();
	
	jobxml.FindSibingNode("content");			retxml.AddNode("content");
	jobxml.IntoNode();							retxml.IntoNode();
	jobxml.FindSibingNode("TaskID");			jobxml.GetData(strVal);	retxml.AddNode("TaskID",strVal);
	jobxml.FindSibingNode("JobID");				jobxml.GetData(strVal);	retxml.AddNode("JobID",strVal);
	jobxml.FindSibingNode("MJMJobType");		jobxml.GetData(strVal);	retxml.AddNode("JobType",strVal);
	retxml.AddNode("JobState",1);//2
	retxml.AddNode("JobLog","......");//100%
	
	retxml.AddNode("JobTime",strMsg);
	
	char retPath[512];	retPath[0] = 0;
	if (lpstrRetXmlPath){
		strcpy(retPath,lpstrRetXmlPath);
	}else
	{
		char inifilepath[512];	
		if( GetPrivateProfilePath(inifilepath,CONFIG_FILE) ) {
			GetPrivateProfileString("PathSetting","linuxpath","",retPath,256,inifilepath); VERF_SLASH(retPath);
			if( !IsExist(retPath) )  { GetPrivateProfileString("PathSetting","windowspath","",retPath,256,inifilepath); VERF_SLASH(retPath); }
			if( !IsExist(retPath) )  strcpy(retPath,"");	else strcat(retPath,"/"); 
		}
		char* pS = retPath+strlen(retPath);
		jobxml.FindSibingNode("OutJobStatusFileName");
		jobxml.GetData(pS);
	}
	
	if( retxml.Save(retPath) )	printf("Write Status XML File: %s!\n",retPath);
	else printf("Fail to Write Status XML File: %s!\n",retPath);
	
	jobxml.Close();

	return true;
}

int CMIPXml::Save(LPCSTR lpstrPath)
{
	if( !Open(lpstrPath) )
		SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");

	CompleteNode("root");	IntoNode();
	CompleteNode("head");	IntoNode();
	CompleteNode("RecipientAddress","MIP");
	OutNode();

	CompleteNode("content");	IntoNode();
	CompleteNode("TaskID",m_hdr.jobName);
	{
		char strTskNam[20];	strcpy(strTskNam,"uno");
		if( m_tskTypeCur &	TSK_ATMCH ) strcpy(strTskNam,"PCJS");else
		if( m_tskTypeCur &	TSK_ATADJ ) strcpy(strTskNam,"PCJS");else
		if( m_tskTypeCur & TSK_DEMMCH ) strcpy(strTskNam,"DSM-PD");else
		if( m_tskTypeCur & TSK_DEMFLT ) strcpy(strTskNam,"DEM-PD");else
		if( m_tskTypeCur & TSK_IMGEPI ) strcpy(strTskNam,"EPI-PD");else
		if( m_tskTypeCur & TSK_IMGREC ) strcpy(strTskNam,"JZ");else
		if( m_tskTypeCur & TSK_IMGFUS ) strcpy(strTskNam,"RH");else
		if( m_tskTypeCur & TSK_IMGMSC ) strcpy(strTskNam,"XQ");else
		if( m_tskTypeCur & TSK_ATEDIT ) strcpy(strTskNam,"PCBJ");

		CompleteNode("MJMJobType",strTskNam);
	}

	CompleteNode("ProductCount","0");
	CompleteNode("ProductList");	IntoNode();
	CompleteNode("Product");	IntoNode();
// 	{
// 		char strVal[256];	double* pBur = m_projectionInfo.fBur7Par;
// 		sprintf(strVal,"%lf,%lf,%lf,%lf,%lf,%lf,%lf",pBur,pBur+1,pBur+2,pBur+3,pBur+4,pBur+5,pBur+6);
// 		CompleteNode("BursaSevenParameters",strVal);
// 		sprintf(strVal,"%lf",m_projectionInfo.fPrjScale);
// 		CompleteNode("ProjectionParameters",strVal);
// 	}
	OutNode();	OutNode();

	CompleteNode("ProductOutPath",m_strWorkDir);
	CompleteNode("OutJobStatusFileName",m_strStatusPath);
	CompleteNode("OutJobCompleteFileName",m_strCompletePath);

	char strVal[20];
	{	
		sprintf(strVal,"%lf",m_prodInfo.fAverHeight);
		CompleteNode("ProdAverHeight",strVal);
		sprintf(strVal,"%lf",m_prodInfo.fDemGSD);
		CompleteNode("ProdDemGsd",strVal);
		sprintf(strVal,"%lf",m_prodInfo.fDomGSD);
		CompleteNode("ProdDomGsd",strVal);
	}
	
	int sz=0;
	ImageInfo* pInfo = m_import.GetData(sz);
	
	RemoveNode("InputFileList");
	CompleteNode("InputCount",sz);
	CompleteNode("InputFileList",NULL,false);
	IntoNode();
	for (int i=0; i<sz; i++,pInfo++){
		AddNode("InputFile");	IntoNode();
		AddNode("DataID",pInfo->nID);
		AddNode("InputFileName",pInfo->path);
		OutNode();
	}
	OutNode();

// 	FindNode(Mode_Absolute,NULL,4,"root","content","InputFileList","InputFile"); 
// 	AddChildNode("ZoneType",m_projectionInfo.nZoneType);
// 	AddChildNode("ZoneNo",m_projectionInfo.nZoneNo);
	
	CXmlFile::Save(lpstrPath);
	return ERR_NONE;
}

/*
#ifdef FLOW_PRJ_MIP
//Print Status File for PP to analyze
void PrintLog( const char *pPN,int code,const char *pMsg )
{
	static	time_t	lastTime = 0;	if(lastTime==0) time(&lastTime);
	time_t curTime;	time(&curTime);
	if (code==0){
		if(curTime-lastTime<30) return;
		lastTime = curTime;
	}
	CXmlFile xml;
	xml.Open(pPN);
	
	char strTime[256];	GetStdCurTime(strTime);

	xml.CompleteNode("root");
	xml.IntoNode();

	xml.CompleteNode("head");
	xml.IntoNode();
	xml.CompleteNode("CreationTime",strTime);
	xml.OutNode();

	xml.CompleteNode("content");
	xml.IntoNode();
	xml.CompleteNode("JobState",code==2?2:1);
	xml.CompleteNode("JobLog",pMsg);
	xml.CompleteNode("JobTime",strTime);

	xml.Save(pPN);
}

#endif*/