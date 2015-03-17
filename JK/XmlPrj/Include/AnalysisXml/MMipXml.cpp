#include "stdafx.h"
#include "MMipXml.h"
#include "Markup.h"
#include "ZY3FileName.h"
#include "file_operation.hpp"

int CMMIPXml::GenGeographicInfo(bool IsProdGeoSys)//只要找到<GeographicReference>字段且XML的main position位于<GeographicReference>字段即可
{
	GeoRef geoRef; char strDescType[512], strGeoVal[512];
	geoRef.elpsType = ET_CN2000;//椭球类型
	geoRef.projType = GAUSS_PROJECTION;//投影类型
	geoRef.a = SEMIMAJOR_CN2000; geoRef.b = SEMIMINOR_CN2000;//椭球长短半径
	geoRef.Origin_Latitude = 0.0;//初始纬度
	geoRef.Central_Meridian = 117;//中央经线
	geoRef.False_Easting = 500000.0;//东偏
	geoRef.False_Northing = 0.0;//北偏
	geoRef.Scale_Factor = 1.0;//投影比例
	geoRef.eleType = 0;//？
	if (FindChildNode("DescType", strDescType) == NOT_GET){ ErrorPrint("Error: Can't find the node: <strDescType> !");	return ERR_FILE_READ; }//获取地理信息表达标准
	if (!strcmp(strDescType, "Standard")) {
		if (FindChildNode("GeodeticDatum", strGeoVal) != NOT_GET)
		{
			/////////////////椭球类型/////////////////////////
			SetEllipsoidMode(strGeoVal, geoRef, strDescType);
			/////////////////椭球长短半径/////////////////////
			CGeoFile::GetEplsDefPar(geoRef.elpsType, &geoRef.a, &geoRef.b);
			////////////////投影类型//////////////////////
			if (FindChildNode("Projection", strGeoVal) != NOT_GET)
			{
				SetProjectionMode(strGeoVal, geoRef, strDescType);
				//////////////////投影比例///////////////////////////
				if (FindChildNode("ProjectScale", strGeoVal) != NOT_GET) geoRef.Scale_Factor = atof(strGeoVal);
				CGeoFile::GetProjDefPar(geoRef.projType, &geoRef.Scale_Factor);
				//////////////////中央经线/////////////////////
				int nZoneType, nZoneNo;
				if (FindChildNode("ZoneType", strGeoVal) != NOT_GET)	nZoneType = atoi(strGeoVal);
				if (FindChildNode("ZoneNo", strGeoVal) != NOT_GET)	nZoneNo = atoi(strGeoVal);
				if (nZoneType == 3) geoRef.Central_Meridian = (nZoneType*nZoneNo);
				else if (nZoneType == 6) geoRef.Central_Meridian = (nZoneType*nZoneNo - 3);
				///////////////东偏///////////////////////
				if (FindChildNode("ProjectFalseEas", strGeoVal) != NOT_GET) geoRef.False_Easting = atof(strGeoVal);
				///////////////北偏///////////////////////
				if (FindChildNode("ProjectFalseNor", strGeoVal) != NOT_GET) geoRef.False_Northing = atof(strGeoVal);
			}
			else{ ErrorPrint("Error: Can't find the node: <Projection> !");	return ERR_FILE_READ; }
		}
		else{ ErrorPrint("Error: Can't find the node: <GeodeticDatum> !");	return ERR_FILE_READ; }
	}
	else if (!strcmp(strDescType, "WKT")) {
		char *pStrWKT = new char[MAX_PATHNAME * 2];
		if (FindChildNode("WKT", pStrWKT) == NOT_GET){ ErrorPrint("Error: Can't find the node: <WKT> !");	return ERR_FILE_READ; }
		OGRSpatialReference geographicReference;
		if (strGeoVal) geographicReference.importFromWkt((char**)&pStrWKT);//引入WKT
		/////////////////椭球类型/////////////////////////
		strcpy(strGeoVal, geographicReference.GetAttrValue("GEOGCS"));
		SetEllipsoidMode(strGeoVal, geoRef, strDescType);
		/////////////////椭球长短半径/////////////////////
		CGeoFile::GetEplsDefPar(geoRef.elpsType, &geoRef.a, &geoRef.b);
		////////////////投影类型//////////////////////
		strcpy(strGeoVal, geographicReference.GetAttrValue("PROJECTION"));
		SetProjectionMode(strGeoVal, geoRef, strDescType);
		//////////////////投影比例///////////////////////////
		geoRef.Scale_Factor = geographicReference.GetProjParm(SRS_PP_SCALE_FACTOR);
		CGeoFile::GetProjDefPar(geoRef.projType, &geoRef.Scale_Factor);
		//////////////////中央经线/////////////////////
		geoRef.Central_Meridian = geographicReference.GetProjParm(SRS_PP_CENTRAL_MERIDIAN);
		///////////////东偏///////////////////////
		geoRef.False_Easting = geographicReference.GetProjParm(SRS_PP_FALSE_EASTING);
		///////////////北偏///////////////////////
		geoRef.False_Northing = geographicReference.GetProjParm(SRS_PP_FALSE_NORTHING);
		/////////////////Origin_Latitude/////////////////////
		geoRef.Origin_Latitude = geographicReference.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN);
		//delete[] pStrWKT;
		pStrWKT = NULL;
	}
	////////////////保存Geocentric.gcd、Geocentric7.gcd////////////////////////
	if (IsProdGeoSys)
	{
		char strProdGeoSysFile[MAX_PATHNAME];
		CreateDir(m_strProcessSpace);
		sprintf(strProdGeoSysFile, "%s%s", m_strProcessSpace, "ProdGeoSys.gcd");
		CGeoFile geofile;
		geofile.Set_Cvt_Par(geoRef.elpsType, geoRef.projType, geoRef.a, geoRef.b, geoRef.Origin_Latitude, geoRef.Central_Meridian, geoRef.False_Easting, geoRef.False_Northing, geoRef.Scale_Factor, geoRef.eleType);
		
		
		printf("strProdGeoSysFile: %s\n", strProdGeoSysFile);
		if (!geofile.Save2File(strProdGeoSysFile)) { return ERR_FILE_OPEN; }
		strcpy(m_prodInfo.strGcdPath, strProdGeoSysFile);
	}
	else{
		char strGCPDATAPath[MAX_PATHNAME], strGcpGcdFile[MAX_PATHNAME], strPar7CvtFile[MAX_PATHNAME], strDomlistFile[MAX_PATHNAME];
		printf("m_strProcessSpace: %s\n", m_strProcessSpace);
		sprintf(strGCPDATAPath, "%s%s", m_strProcessSpace, "AT/GCPDATA/");
		CreateDir(strGCPDATAPath);
		sprintf(strGcpGcdFile, "%s%s", strGCPDATAPath, "Geocentric.gcd");
		sprintf(strPar7CvtFile, "%s%s", strGCPDATAPath, "Geocentric7.gcd");
		sprintf(strDomlistFile, "%s%s", strGCPDATAPath, "DomList.txt");
		FILE *fp = fopen(strDomlistFile, "w");
		if (fp != NULL)fprintf(fp, "%s", strGCPDATAPath);
		else{ ErrorPrint("Error: File open error :%s!", strDomlistFile);	return ERR_FILE_READ; }
		fclose(fp);
		CGeoFile geofile;
		geofile.Set_Cvt_Par(geoRef.elpsType, geoRef.projType, geoRef.a, geoRef.b, geoRef.Origin_Latitude, geoRef.Central_Meridian, geoRef.False_Easting, geoRef.False_Northing, geoRef.Scale_Factor, geoRef.eleType);
		if (!geofile.Save2File(strGcpGcdFile)) { return ERR_FILE_OPEN; }
		double dx = 0.0, dy = 0.0, dz = 0.0, rx = 0.0, ry = 0.0, rz = 0.0, scale = 0.0;
		if (!CGeoFile::Save7Para2File(strPar7CvtFile, dx, dy, dz, rx, ry, rz, scale)) { return ERR_FILE_OPEN; }
	}
	return 1;
}

IMPLEMENT_DYNFILELOAD(CMMIPXml, CPrjXml)

inline bool CMMIPXml::CompleteImageInfo(ImageInfo* pInfo)
{
	bool bNoError = true;
	CMIPFile* pFile;	pFile = (CMIPFile*)CMIPFile::identify(pInfo->path);
	if (pFile){
		if (pFile->TinyLoad(pInfo->path, pInfo) != ERR_NONE) bNoError = false;	delete pFile;
	}
	else { if (CSASMACFile::TinyLoad(pInfo->path, pInfo) != ERR_NONE) bNoError = false; }

	return bNoError;
}

inline bool CMMIPXml::GenProdImageInfo(LPIMAGEINF pProd, const ImageInfo* pImg, IMG_LVL lvl, LPCSTR strOutPath)
{
	char strPath[MAX_PATHNAME];
	switch (lvl)
	{
	case IL_UNKNOW:
		break;
	case IL_STRIP:
		break;
	case IL_SC:
		break;
	case IL_EPI:
		break;
	case IL_DSM:
		sprintf(strPath, "%s%s", strOutPath, "DSM/PrimaryDSMResult/");
		break;
	case IL_DEM:	
		break;
	case IL_CJZ:
		break;
	case IL_JJZ:
		break;
	case IL_DOM:
		switch (pImg->cam)
		{
		case CT_NAD:
			sprintf(strPath, "%s%s", strOutPath, "ImageOperation/PrimaryGTCResult/PAN/");
			break;
		case CT_MUX:
			sprintf(strPath, "%s%s", strOutPath, "ImageOperation/PrimaryGTCResult/MUX/");
			break;
		}
		break;
	case IL_FUS:
		sprintf(strPath, "%s%s", strOutPath, "ImageOperation/FusionResult/");
		break;
	case IL_DOG:
		break;
	case IL_MSC:
		sprintf(strPath, "%s%s", strOutPath, "ImageOperation/MosaicResult/");
		break;
	case IL_END:
		break;
	default:
		strcpy(strPath, strOutPath);
		break;
	}
	char* pS = strPath + strlen(strPath) - 1;
	memcpy(pProd, pImg, sizeof(ImageInfo));
	char* pN = strrpath(pProd->path);	strcpy(pS, pN);

	CZY3SceneFile* pFile;	pFile = (CZY3SceneFile*)CMIPFile::identify(strPath);
	if (pFile){
		pFile->RenameImage(strPath, lvl);	delete pFile;
	}
	else { CSASMACFile file; file.RenameImage(strPath, lvl); }
	char strName[256];	strcpy(strName, pS);
	pS = strrpath(strPath); pN = strrchr(pS, '.');	strcpy(pN, strName);//建一层文件夹
	strcpy(pProd->path, strPath);
	pProd->lvl = lvl;

	return true;
}

CLObject* CMMIPXml::identify(LPCSTR lpstrPath)
{
	char strLine[1024];
	FILE* fp = fopen(lpstrPath, "r");		if (!fp) return NULL;

	bool	bType = false;

	for (int i = 0; i < 30; i++){
		fgets(strLine, 1024, fp);

		if (MatchString(strLine, MMIP_XML_ONLY) != 0) { bType = true; break; }
	}
	fclose(fp);

	if (bType)	return CMMIPXml::CreateLObject();
	else return NULL;

}

int CMMIPXml::Load4File(LPCSTR lpstrPath)
{
	printf("\nAnalysis MMip xml ... \n");

	if (!CPrjXml::Open(lpstrPath)) return ERR_FILE_OPEN;

	int ret;

	ret = AnalysisHead();
	if (ret != ERR_NONE)  return ret;

	char strTmp[512];	strcpy(strTmp, lpstrPath);
	char* pS = strrchr(strTmp, '/');	if (!pS) pS = strrchr(strTmp, '\\');
	strcpy(pS, "/ProdGeoSys.gcd");
	ret = AnalysisExtraPar(strTmp);
	if (ret != ERR_NONE)  return ret;

	ret = AnalysisImport();
	if (ret != ERR_NONE)  return ret;

	ret = AnalysisExport();
	if (ret != ERR_NONE)  return ret;

	AnalyzeMipPar();

	UpdataTskList();
	printf("\nFinish Analysis MMip xml.\n");

	return CPrjXml::Load4File(lpstrPath);
}

int CMMIPXml::AnalysisHead()
{
	char strVal[512];
	if (FindNode(Mode_Absolute, NULL, 2, "root", "head"))
	{

	}
	bool bExtentMap = false;//此时的bExtentMap存储的是分幅标志
	if (FindNode(Mode_Absolute, NULL, 2, "root", "content")){
		IntoNode();
		FindSibingNode("TaskID", strVal);	strncpy(m_hdr.jobName, strVal, sizeof(m_hdr.jobName) - 1);//获取TaskID
		FindSibingNode("JobID", strVal);	strncpy(m_hdr.jobID, strVal, sizeof(m_hdr.jobID) - 1);//获取JobID
		if (FindNode(Mode_Absolute, NULL, 3, "root", "content", "ProductiveTaskItemList")){
			if (FindNode(Mode_Absolute, NULL, 4, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo")){
				IntoNode();
				FindSibingNode("TaskItemID", strVal); strncpy(m_taskItemID, strVal, sizeof(m_taskItemID)-1);//获取TaskItemID
				FindSibingNode("TaskItemType", strVal);//获取任务类型
				int tskType;

				if (!strcmp(strVal, "BlockAdjustment"))		tskType = TSK_ATMCH | TSK_ATADJ; else
				if (!strcmp(strVal, "DSMAutoGeneration"))	{
					if (FindNode(Mode_Absolute, NULL, 5, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "DSMAutoGenerationPara")){
						if (FindChildNode("DSMMapScale", strVal) != NOT_GET)	 bExtentMap = (strcmp(strVal, "No")) ? true : false;//获取是否分幅标志
					}
					else	{ ErrorPrint("Error: Can't find the node: <DSMAutoGenerationPara> !");	return ERR_FILE_READ; }
					if (bExtentMap) { tskType = TSK_DEMMCH | TSK_DSM_MAP; }
					else tskType = TSK_DEMMCH;
				}
				else
				if (!strcmp(strVal, "ImageOrtho"))		tskType = TSK_IMGREC; else
				if (!strcmp(strVal, "ImageFusion"))		tskType = TSK_IMGFUS; else
				if (!strcmp(strVal, "Mosaic"))	{
					if (FindNode(Mode_Absolute, NULL, 5, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "ImageMosicPara")){
						if (FindNode(Mode_Absolute, NULL, 6, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "ImageMosicPara", "ClipTile")){
							if (FindChildNode("MapScale", strVal) != NOT_GET)	 bExtentMap = (strcmp(strVal, "No")) ? true : false;//获取是否分幅标志
						}
						else	{ ErrorPrint("Error: Can't find the node: <ClipTile> !");	return ERR_FILE_READ; }
					}
					else	{ ErrorPrint("Error: Can't find the node: <ImageMosicPara> !");	return ERR_FILE_READ; }
					if (bExtentMap) { tskType = TSK_IMGMSC | TSK_IMG_MAP; }
					else tskType = TSK_IMGMSC;
				}
				else//if (!strcmp(strVal, "FreeNetAdjustment"))		tskType = TSK_ATMCH | TSK_ATADJ; else
				{
					ErrorPrint("Error: Unknown Job Type :%s!", strVal);	return ERR_FILE_READ;
				}
				m_tskTypeCur = tskType;	m_tskTypeList |= tskType;
			}
			else	{ ErrorPrint("Error: Can't find the node: <ProductiveTaskItemInfo> !");	return ERR_FILE_READ; }
		}
		else	{ ErrorPrint("Error: Can't find the node: <ProductiveTaskItemList> !");	return ERR_FILE_READ; }
	}
	else	{ ErrorPrint("Error: Can't find the node: <content> !");	return ERR_FILE_READ; }

	return ERR_NONE;
}

int CMMIPXml::AnalysisImport()
{
	char strDataType[512];

	if (FindNode(Mode_Absolute, NULL, 5, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "InputDataSourceList") != NOT_GET){
		IntoNode();
		ImageInfo info;	memset(&info, 0, sizeof(ImageInfo));
		while (FindSibingNode("InputDataSource") == FORWARD_GET){
			IntoNode();
			{//读取输入数据类型//有影像(Image)、高程(Terrain)、控制(GCPts)//Image//Terrain//GCPts	
				if (FindSibingNode("Type", strDataType) == NOT_GET){ ErrorPrint("Error: Can't find the node of Image: <Type> !");	return ERR_FILE_READ; }
				/*Image*/	if (!strcmp(strDataType, "Image")){
					////////////////////////////////解析影像级别//////////////////////////////////////////////////
					char strImgVal[512]; IMG_LVL img_lvl = IL_UNKNOW;
					if (FindSibingNode("Level", strImgVal) == NOT_GET){ ErrorPrint("Error: Can't find the node of Image: <Level> !");	return ERR_FILE_READ; }
					if (!strcmp(strImgVal, "SEC")) img_lvl = IL_SC;
					else if (!strcmp(strImgVal, "DSM"))img_lvl = IL_DSM;
					else if (!strcmp(strImgVal, "DEM"))img_lvl = IL_DEM;
					else if (!strcmp(strImgVal, "GTC"))img_lvl = IL_DOM;
					else if (!strcmp(strImgVal, "DOM"))img_lvl = IL_FUS;

					/////////////////////////////解析原始输入数据/////////////////////////
					if (FindSibingNode("OrganizationMode", strImgVal) == NOT_GET){ ErrorPrint("Error: Can't find the node of Image: <OrganizationMode> !");	return ERR_FILE_READ; }//解析影像组织模式//仅处理“景”模式
					if (!strcmp(strImgVal, "Scene")){
						if (FindSibingNode("FileInfoList") != NOT_GET){
							IntoNode();
							{
								while (FindSibingNode("FileInfo") == FORWARD_GET){
									if (FindChildNode("FullName", info.path) == NOT_GET)continue;
									CompleteImageInfo(&info); info.lvl = img_lvl;
									if (info.lvl != IL_UNKNOW) m_import.Append(info);//如果不能识别其影像级别，则不解析//外部dem还是可以解析
								}
							}
							OutNode();
						}
						else /////////////////////////////解析上一道工序生产的产品/////////////////////////
						if (FindSibingNode("DataSourceProductDirectory", strImgVal) != NOT_GET)
						{
							char strProdOutPath[512];
							sprintf(strProdOutPath, "%s%s", m_strPathSetting, strImgVal);
							STRINGVec FilenameVec;
							fSearchFile((char*)strProdOutPath, "*.*", FilenameVec);//寻找指定的文件
							for (unsigned int i = 0; i < FilenameVec.size(); i++)
							{
								strcpy(info.path, FilenameVec.at(i).str);
								CompleteImageInfo(&info); info.lvl = img_lvl;
								if (info.lvl != IL_UNKNOW) m_import.Append(info);//如果不能识别其影像级别，则不解析//外部dem还是可以解析的
							}
						}
					}
					else{ ErrorPrint("Error: Unknown image organizationMode!");	return ERR_FILE_READ; }
				}
				/*Terrain*/	else if (!strcmp(strDataType, "Terrain")){
					char strTerrainVal[MAX_PATHNAME]; IMG_LVL terrain_lvl = IL_UNKNOW;
					if (FindSibingNode("Level", strTerrainVal) == NOT_GET){ ErrorPrint("Error: Can't find the node of Terrain: <Level> !");	return ERR_FILE_READ; };//解析数据级别//
					if (!strcmp(strTerrainVal, "DSM")) terrain_lvl = IL_DSM;
					else if (!strcmp(strTerrainVal, "DEM")) terrain_lvl = IL_DEM;

					/////////////////////////////解析原始输入的数据/////////////////////////
					if (FindSibingNode("OrganizationMode", strTerrainVal) == NOT_GET){ ErrorPrint("Error: Can't find the node of Terrain: <OrganizationMode> !");	return ERR_FILE_READ; }//解析影像组织模式//仅处理“景”模式和整块DEM
					if (!strcmp(strTerrainVal, "Other")){//Other//认为是一整块大DEM
						if (FindSibingNode("FileInfoList") != NOT_GET){
							IntoNode();
							{
								while (FindSibingNode("FileInfo") == FORWARD_GET){
									if (FindChildNode("FullName", info.path) == NOT_GET)continue;
									CompleteImageInfo(&info);	info.lvl = terrain_lvl;
									strcpy(info.orbitID, ORIBIT_INVALID);//整块大DEM的Orbit设置为ORIBIT_INVALID
									if (info.lvl != IL_UNKNOW) m_import.Append(info);
								}
							}
							OutNode();
						}
						else	{ ErrorPrint("Error: Can't find the node of Terrain: <FileInfoList> !");	return ERR_FILE_READ; }
					}
					else if (!strcmp(strTerrainVal, "Scene")){
						if (FindSibingNode("FileInfoList", NULL) != NOT_GET){
							IntoNode();
							{
								while (FindSibingNode("FileInfo") == FORWARD_GET){
									if (FindChildNode("FullName", info.path) == NOT_GET)continue;
									CompleteImageInfo(&info);	info.lvl = terrain_lvl;
									if (info.lvl != IL_UNKNOW) m_import.Append(info);
								}
							}
							OutNode();
						}
						else /////////////////////////////解析上一道工序生产的产品/////////////////////////
						if (FindSibingNode("DataSourceProductDirectory", strTerrainVal) != NOT_GET)
						{
							char strInputProdPath[MAX_PATHNAME];
							sprintf(strInputProdPath, "%s%s", m_strPathSetting, strTerrainVal);
							STRINGVec FilenameVec;
							fSearchFile((char*)strInputProdPath, "*.*", FilenameVec);
							for (unsigned int i = 0; i < FilenameVec.size(); i++){
								strcpy(info.path, FilenameVec.at(i).str);
								CompleteImageInfo(&info); info.lvl = terrain_lvl;
								if (info.lvl != IL_UNKNOW) m_import.Append(info);
							}
						}
						else { ErrorPrint("Error: Can't find the node of Terrain: <FileInfoList/DataSourceProductDirectory> !");	return ERR_FILE_READ; }
					}
					else{ ErrorPrint("Error: Unknown Terrain organizationMode !");	return ERR_FILE_READ; }
				}//GCPts
				/*GCPts*/else if (!strcmp(strDataType, "GCPts")){
					char strGCPtsLvel[MAX_PATHNAME], strGCPtsVal[MAX_PATHNAME];///解析GCPts级别/
					if (FindSibingNode("Level", strGCPtsLvel) == NOT_GET) { ErrorPrint("Error: Can't find the node of GCPts: <Level> !");	return ERR_FILE_READ; }
					if (FindSibingNode("GeographicReference") == NOT_GET){ ErrorPrint("Error: Can't find the node of GCPts: <GeographicReference> !");	return ERR_FILE_READ; }
					GenGeographicInfo(false);//解析控制坐标系并将gcd文件生产在相应的文件夹下
					//////////Copy控制片///////////////////////////////////////////////////////////////
					char strGCImagePath[MAX_PATHNAME], strGCPDATAPath[MAX_PATHNAME];
					sprintf(strGCPDATAPath, "%s%s", m_strProcessSpace, "AT/GCPDATA/");

					if (!strcmp(strGCPtsLvel, "GCPLib")) { ErrorPrint("Error: Unknown GCPts Level !");	return ERR_FILE_READ; }
					else if (!strcmp(strGCPtsLvel, "GCPCoordinateFile")) { ErrorPrint("Error: Unknown GCPts Level !");	return ERR_FILE_READ; }
					else if (!strcmp(strGCPtsLvel, "GCImageTip")) {//Copy控制片
						if (FindSibingNode("FileInfoList") != NOT_GET){
							IntoNode();
							{
								while (FindSibingNode("DirectoryInfo") == FORWARD_GET){
									if (FindChildNode("DirName", strGCPtsVal) == NOT_GET) continue;
									sprintf(strGCImagePath, "%s%s", m_strPathSetting, strGCPtsVal);
									fCarryFile(strGCImagePath, strGCPDATAPath);
								}
							}
							OutNode();
						}
						else	{ ErrorPrint("Error: Can't find the node: <FileInfoList> !");	return ERR_FILE_READ; }
					}
					else if (!strcmp(strGCPtsLvel, "GCImage")) {	//Copy控制片
						if (FindSibingNode("FileInfoList") != NOT_GET){
							IntoNode();
							{
								while (FindSibingNode("FileInfo") == FORWARD_GET){
									if (FindChildNode("FullName", strGCPtsVal) == NOT_GET)continue;
									sprintf(strGCImagePath, "%s%s", m_strPathSetting, strGCPtsVal);
									char strDstFile[MAX_PATHNAME];
									char *pS = strrchr(strGCPtsVal, '/');
									sprintf(strDstFile, "%s%s", strGCPDATAPath, pS + 1);
									CopyFile(strGCImagePath, strGCPDATAPath,false);
								}
							}
							OutNode();
						}
						else	{ ErrorPrint("Error: Can't find the node of GCPts: <FileInfoList> !");	return ERR_FILE_READ; }
					}
					else{ ErrorPrint("Error: Unknown GCPts Level !");	return ERR_FILE_READ; }
					/////////////////////////////解析GCPts级别-END//////////////////////////////////////////////////////////////
				}
				/*RPC*/		else if (!strcmp(strDataType, "RPC"))
				{
					char strRPCVal[MAX_PATHNAME];
					if (FindSibingNode("DataSourceProductDirectory", strRPCVal) != NOT_GET)
					{
						ImgExInfo exInfo; memset(&exInfo, 0, sizeof(ImgExInfo));
						char strInputProdPath[MAX_PATHNAME];
						sprintf(strInputProdPath, "%s%s", m_strPathSetting, strRPCVal);
						STRINGVec FilenameVec;
						fSearchFile((char*)strInputProdPath, "*.*", FilenameVec);
						unsigned int rpcFileNum = FilenameVec.size();
						if (rpcFileNum!=0)
						{
							for (unsigned int i = 0; i < rpcFileNum; i++){
								strcpy(exInfo.strAdjRpcNam, FilenameVec.at(i).str);
								m_importEx.Append(exInfo);
							}
						}
						else { ErrorPrint("There is no adjRpc file in xml!"); return ERR_FILE_READ; }						
					}
					else { ErrorPrint("Error: Can't find the node of Terrain: <FileInfoList/DataSourceProductDirectory> !");	return ERR_FILE_READ; }
				}
				else{ ErrorPrint("Error: Unknown Data Type !");	return ERR_FILE_READ; }
			}
			OutNode();
		}//FindSibingNode("InputDataSource")//while结束
	}
	else	{ ErrorPrint("Error: Can't find the node: <InputDataSourceList> !");	return ERR_FILE_READ; }
	//AppendImageArray函数作用：将m_import里的所有影像均组织到相应的景中
	int importNum = m_import.GetSize();
	if (importNum == 0)  { ErrorPrint("There is no import data in xml!"); return ERR_FILE_READ; }
	
	//////////////////////////////////////////////////////////////////////////
	//将输入数据和输出数据组织成景模式
	m_scList.Reset();
	//将输入数据补充称绝对路径
	for (int i = 0; i < importNum; i++)
	{
		char strImageFile[MAX_PATHNAME];
		sprintf(strImageFile, "%s%s", m_strPathSetting, m_import[i].path);
		sprintf(m_import[i].path, "%s", strImageFile);
	}
	m_scList.AppendImageArray(m_import);

	
	return ERR_NONE;
}

int CMMIPXml::AnalysisExport()
{
	char strOutPath[MAX_PATHNAME];
	sprintf(strOutPath, "%s",m_strProductSpace);
	int i, num;	SceneInfo* scInf = m_scList.GetData(num);
	m_export.Reset(NULL, 100);

	ImageInfo info;	memset(&info, 0, sizeof(ImageInfo));
	LPIMAGEINF pInfo;
	//	int nExCurNum = 0;	int nExtLasNum = 0;
	int tskType = m_tskTypeCur;
	if (tskType&TSK_ATMCH){
	}
	if (tskType&TSK_ATADJ){
		int inputNum=m_import.GetSize();
		ImgExInfo exInfo;	memset(&exInfo, 0, sizeof(ImgExInfo));
		char strAdjRpcFile[MAX_PATHNAME];
		for (int i = 0; i < inputNum;i++)
		{
			char strTemp[MAX_PATHNAME];
			strcpy(strTemp, m_import.GetAt(i).path);
			
			char *pT = strrchr(strTemp, '.');sprintf(pT, "%s", "_rpc.txt");	
			sprintf(strAdjRpcFile, "%s%s%s", m_strProductSpace, "Calibration/OptimizedResultData/MultiResolution/", strTemp + strlen(m_strPathSetting));
			strcpy(exInfo.strAdjRpcNam, strAdjRpcFile);
			m_importEx.Append(exInfo);
		}
	}
	if (tskType&TSK_IMGEPI){
		LPIMAGEINF	pImgF, pImgN, pImgB;
		for (i = 0; i < num; i++){
			pImgF = m_scList.GetImage(i, IL_SC, CT_FWD);
			pImgN = m_scList.GetImage(i, IL_SC, CT_NAD);
			pImgB = m_scList.GetImage(i, IL_SC, CT_BWD);

			if (!pImgF || !pImgN || !pImgB) continue;

			GenProdImageInfo(&info, pImgF, IL_EPI, strOutPath);	m_export.Append(info);
			GenProdImageInfo(&info, pImgN, IL_EPI, strOutPath);	m_export.Append(info);
			GenProdImageInfo(&info, pImgB, IL_EPI, strOutPath);	m_export.Append(info);

		}
		// 		pInfo = m_export.GetData(nExCurNum);
		// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;	
	}
	if (tskType&TSK_DEMMCH){
		LPIMAGEINF	pImgF, pImgN, pImgB;
		for (i = 0; i < num; i++){
			pImgF = m_scList.GetImage(i, IL_SC, CT_FWD);
			pImgN = m_scList.GetImage(i, IL_SC, CT_NAD);
			pImgB = m_scList.GetImage(i, IL_SC, CT_BWD);

			if (!pImgF || !pImgN || !pImgB) continue;

			GenProdImageInfo(&info, pImgN, IL_DSM, strOutPath);	m_export.Append(info);
		}
		// 		pInfo = m_export.GetData(nExCurNum);
		// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}
	if (tskType&TSK_DEMFLT){
		LPIMAGEINF	pDSM;
		for (i = 0; i < num; i++){
			pDSM = m_scList.GetImage(i, IL_DSM);

			if (!pDSM) continue;
			GenProdImageInfo(&info, pDSM, IL_DEM, strOutPath);	m_export.Append(info);
		}
		// 		pInfo = m_export.GetData(nExCurNum);
		// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}
	if (tskType&TSK_IMGREC){
		LPIMAGEINF	pImgF, pImgN, pImgB, pImg, pImgC;
		LPIMAGEINF pDSM, pDEM, pTer;
		pImg = pTer = NULL;
		for (i = 0; i < num; i++){
			pImgF = m_scList.GetImage(i, IL_SC, CT_FWD);
			pImgN = m_scList.GetImage(i, IL_SC, CT_NAD);
			pImgB = m_scList.GetImage(i, IL_SC, CT_BWD);
			pImgC = m_scList.GetImage(i, IL_SC, CT_MUX);
			pDSM = m_scList.GetImage(i, IL_DSM);
			pDEM = m_scList.GetImage(i, IL_DEM);

			if (pImgN) pImg = pImgN; else if (pImgF) pImg = pImgF; else if (pImgB) pImg = pImgB;	else if (!pImgC) continue;
			if (pDSM) pTer = pDSM; else if (pDEM) pTer = pDEM;
			else if (m_import.GetImgNum(IL_DSM, CT_END, ORIBIT_INVALID) != 0) pTer = NULL; else if (m_import.GetImgNum(IL_DEM, CT_END, ORIBIT_INVALID) != 0) pTer = NULL; else	continue;

			if (pImg)	{ GenProdImageInfo(&info, pImg, IL_DOM, strOutPath);	m_export.Append(info); }
			if (pImgC)	{ GenProdImageInfo(&info, pImgC, IL_DOM, strOutPath);	m_export.Append(info); }
		}
		// 		pInfo = m_export.GetData(nExCurNum);
		// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}
	if (tskType&TSK_IMGFUS){
		LPIMAGEINF	pImg = NULL, pImgC = NULL;
		LPSCENEINFO pscene = NULL;
		for (i = 0; i < num; i++){
			pImg = NULL, pImgC = NULL;
			pscene = &(m_scList.GetAt(i));
			pInfo = pscene->imgList.p ? *(pscene->imgList.p) + (pscene->imgList.id) : NULL;
			while (pInfo){
				if (pInfo->cam == CT_MUX) {
					if (pInfo->lvl == IL_DOM) pImgC = pInfo; else
					if (pInfo->lvl == IL_JJZ && (!pImgC || (pImgC && pImgC->lvl < IL_JJZ))) pImgC = pInfo; else
					if (pInfo->lvl == IL_CJZ && !pImgC) pImgC = pInfo;
				}
				else
				if (pInfo->cam != CT_UNKNOW){
					if (pInfo->lvl == IL_DOM) pImg = pInfo; else
					if (pInfo->lvl == IL_JJZ && (!pImg || (pImg && pImg->lvl < IL_JJZ))) pImg = pInfo; else
					if (pInfo->lvl == IL_CJZ && !pImg) pImg = pInfo;
				}
				pInfo = pInfo->pNext.p ? *(pInfo->pNext.p) + (pInfo->pNext.id) : NULL;
			}

			if (!pImg || !pImgC) continue;

			GenProdImageInfo(&info, pImg, IL_FUS, strOutPath);	m_export.Append(info);
		}
		// 		pInfo = m_export.GetData(nExCurNum);
		// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}
	if (tskType&TSK_IMGMSC){
		info.lvl = IL_MSC;
		sprintf(info.path, "%sZY3_MSC_%s_MSC.bbi", strOutPath, m_hdr.jobName);	m_export.Append(info);
		// 		pInfo = m_export.GetData(nExCurNum);
		// 		m_scList.AppendImageList(pInfo+nExtLasNum,nExCurNum-nExtLasNum);	nExtLasNum = nExCurNum;
	}

	m_scList.AppendImageArray(m_export);//写到相应的景中
	//	if( m_export.GetSize() == 0 )  { ErrorPrint("There is no export information in xml!"); return ERR_FILE_READ; }
	return ERR_NONE;
}

int CMMIPXml::AnalysisExtraPar(LPCSTR lpstrGcd)
{
	char strOutPath[MAX_PATHNAME] = "";	char strTmp[MAX_PATHNAME];
	{
		char inifilepath[512];
		if (GetPrivateProfilePath(inifilepath, CONFIG_FILE)) {//首先找到FlowControl.ini文件
			GetPrivateProfileString("PathSetting", "linuxpath", "", strOutPath, 256, inifilepath); VERF_SLASH(strOutPath);//获取linuxpath
			if (!IsExist(strOutPath))  { GetPrivateProfileString("PathSetting", "windowspath", "", strOutPath, 256, inifilepath); VERF_SLASH(strOutPath); }//linuxpath不存在时,获取windowspath
			if (!IsExist(strOutPath))  strcpy(strOutPath, "");	else strcat(strOutPath, "/"); //若windowspath也不存在,则将strOutPath置空
		}
		strcpy(m_strPathSetting, strOutPath);//存储相对路径的帽子
		//printf("m_strPathSetting: %s\n", m_strPathSetting);
		strcpy(m_strProductSpace, strOutPath);	strcpy(m_strProcessSpace, strOutPath);

		if (!FindNode(Mode_Absolute, strTmp, 6, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "OutputData", "ProductPath"))
		{
			ErrorPrint("%s", GetXmlError());	return ERR_FILE_READ;
		}
		strcat(strOutPath, strTmp);	strcat(m_strProductSpace, strTmp);
		//strcpy(m_strProductSpace, strTmp);
		if (!FindNode(Mode_Absolute, strTmp, 6, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "OutputData", "ProcessOutPath"))
		{
			ErrorPrint("%s", GetXmlError());	return ERR_FILE_READ;
		}
		strcat(m_strProcessSpace, strTmp);
		//strcpy(m_strProcessSpace, strTmp);
		AddEndSlash(m_strProcessSpace);		AddEndSlash(m_strProductSpace);
	}

	////////////////////////////解析额外参数GSD/ProdGeoSys.gcd//MapType///////////////////////////////
	m_prodInfo.fDemGSD = 25;	m_prodInfo.fDomGSD = 2.5;	m_prodInfo.fAverHeight = 500;
	if (FindNode(Mode_Absolute, NULL, 4, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo") != NOT_GET){
		IntoNode();
		{
			char strProdGeoVal[MAX_PATHNAME];
			if (FindSibingNode("DSMAutoGenerationPara") != NOT_GET)
			{
				///////////////////////解析DSM产品坐标系/Map/GSD/////////////////////////////////////
				IntoNode();
				{
					if (FindSibingNode("GeographicReference") != NOT_GET)GenGeographicInfo(true);
					else{ ErrorPrint("Error: Can't find the node of DSMAutoGenerationPara: <GeographicReference> !");	return ERR_FILE_READ; }
					if (FindSibingNode("DSMMapScale", strProdGeoVal) != NOT_GET)
					{
						if (!strcmp(strProdGeoVal, "No"))m_IsMap = false;
						else m_IsMap = true;
					}
					if (FindSibingNode("GSD", strProdGeoVal) != NOT_GET)m_prodInfo.fDemGSD = (float)atof(strProdGeoVal);
				}
				OutNode();
			}
			else if (FindSibingNode("ImageOrthoPara") != NOT_GET)
			{
				///////////////////////解析DOM产品坐标系/GSD/////////////////////////////////////
				IntoNode();
				{
					if (FindSibingNode("GeographicReference") != NOT_GET)GenGeographicInfo(true);
					else{ ErrorPrint("Error: Can't find the node of ImageOrthoPara: <GeographicReference> !");	return ERR_FILE_READ; }
					if (FindSibingNode("GSD", strProdGeoVal) != NOT_GET)m_prodInfo.fDomGSD = (float)atof(strProdGeoVal);
				}
				OutNode();

			}
			else if (FindSibingNode("ImageFusionPara") != NOT_GET)
			{
			}
			else if (FindSibingNode("ImageMosicPara") != NOT_GET)
			{
				IntoNode();
				{
					if (FindSibingNode("ClipTile") != NOT_GET)
					{
						if (FindChildNode("MapScale", strProdGeoVal) == NOT_GET){ ErrorPrint("Error: Can't find the node of ImageMosicPara: <MapScale> !");	return ERR_FILE_READ; }
						if (!strcmp(strProdGeoVal, "No"))m_IsMap = false;
						else m_IsMap = true;
					}
				}
				OutNode();
			}
		}
		OutNode();
	}
	else	{ ErrorPrint("Error: Can't find the node: <ProductiveTaskItemInfo> !");	return ERR_FILE_READ; }
	if (m_prodInfo.fDemGSD < MIN_DEMGSD) m_prodInfo.fDemGSD = MIN_DEMGSD;
	if (m_prodInfo.fDomGSD < MIN_DOMGSD)  m_prodInfo.fDomGSD = MIN_DOMGSD;
	return ERR_NONE;
}

int CMMIPXml::AnalyzeMipPar(LPCSTR lpstrPath)
{
	if (lpstrPath) if (!Open(lpstrPath)) return ERR_FILE_READ;
	FindNode(Mode_Absolute, m_strWorkDir, 6, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "OutputData", "ProductPath");
	FindNode(Mode_Absolute, m_strStatusPath, 5, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "OutStatusFileName");
	FindNode(Mode_Absolute, m_strCompletePath, 5, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "OutCompleteFileName");

	//////////////////////////////////////////////////////////////////////////
	int importNum = m_import.GetSize();
	int improtExNum = m_importEx.GetSize();
	if (importNum>improtExNum)
	{
		ImgExInfo exInfo; memset(&exInfo, 0, sizeof(ImgExInfo));
		for (int i = 0; i < importNum - improtExNum;i++)
		{
			m_importEx.Append(exInfo);
		}
	}
	int exportNum = m_export.GetSize();
	int exportExNum = m_exportEx.GetSize();
	if (exportNum>exportExNum)
	{
		ImgExInfo exInfo; memset(&exInfo, 0, sizeof(ImgExInfo));
		for (int i = 0; i < exportNum - exportExNum;i++)
		{
			m_exportEx.Append(exInfo);
		}
	}

	return ERR_NONE;
}

bool CMMIPXml::GetStatusPath(LPCSTR lpstrJobXmlPath, LPCSTR lpstrDefPath, char* strStatusPath)
{
	char strDef[512], strRelPath[256];	strcpy(strDef, lpstrDefPath);
	CXmlFile jobXml;	jobXml.Open(lpstrJobXmlPath);
	if (!jobXml.FindNode(Mode_Absolute, strRelPath, 5, "root", "content", "ProductiveTaskItemList", "ProductiveTaskItemInfo", "OutStatusFileName")){
		strcpy(strStatusPath, strDef);	return false;
	}

	char inifilepath[512];
	if (GetPrivateProfilePath(inifilepath, CONFIG_FILE)) {
		GetPrivateProfileString("PathSetting", "linuxpath", "", strDef, 256, inifilepath); VERF_SLASH(strDef);
		if (!IsExist(strDef))  { GetPrivateProfileString("PathSetting", "windowspath", "", strDef, 256, inifilepath); VERF_SLASH(strDef); }
		if (!IsExist(strDef))  strcpy(strDef, "");	else strcat(strDef, "/");
	}
	strcat(strDef, strRelPath);
	strcpy(strStatusPath, strDef);
	return true;

}

inline void GetStdCurTime(char* strTime)
{
	time_t lt = time(0); char* pS;
	struct tm	st;		memcpy(&st, localtime(&lt), sizeof(struct tm));
	sprintf(strTime, "%d", st.tm_year + 1900);
	pS = strTime + strlen(strTime);
	if (st.tm_mon + 1 < 10)  sprintf(pS, "-0%d", st.tm_mon + 1);	else sprintf(pS, "-%d", st.tm_mon + 1);
	pS = strTime + strlen(strTime);
	if (st.tm_mday < 10)  sprintf(pS, "-0%d", st.tm_mday);	else sprintf(pS, "-%d", st.tm_mday);
	pS = strTime + strlen(strTime);
	if (st.tm_hour < 10)  sprintf(pS, "T0%d", st.tm_hour);	else sprintf(pS, "T%d", st.tm_hour);
	pS = strTime + strlen(strTime);
	if (st.tm_min < 10)  sprintf(pS, ":0%d", st.tm_min);	else sprintf(pS, ":%d", st.tm_min);
	pS = strTime + strlen(strTime);
	if (st.tm_sec < 10)  sprintf(pS, ":0%d", st.tm_sec);	else sprintf(pS, ":%d", st.tm_sec);
}

bool CMMIPXml::WriteStatusXml(LPCSTR lpstrJobXmlPath, LPCSTR lpstrRetXmlPath/* =NULL */)
{
	CXmlFile jobxml, retxml;
	if (!jobxml.Open(lpstrJobXmlPath)) { printf("Error: Fail to Open job xml:%s!\n", lpstrJobXmlPath); return false; }

	retxml.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");

	jobxml.FindSibingNode("root");				retxml.AddNode("root");
	jobxml.IntoNode();							retxml.IntoNode();

	char  strVal[512];
	jobxml.FindSibingNode("head");				retxml.AddNode("head");
	jobxml.IntoNode();							retxml.IntoNode();
	jobxml.FindSibingNode("MessageType");		jobxml.GetData(strVal);	retxml.AddNode("MessageType", strVal);
	jobxml.FindSibingNode("MessageID");			jobxml.GetData(strVal);	retxml.AddNode("MessageID", strVal);
	jobxml.FindSibingNode("OriginatorAddress");	jobxml.GetData(strVal);	retxml.AddNode("OriginatorAddress", strVal);
	jobxml.FindSibingNode("RecipientAddress");	jobxml.GetData(strVal);	retxml.AddNode("RecipientAddress", strVal);

	char strMsg[256];	GetStdCurTime(strMsg);
	jobxml.FindSibingNode("CreationTime");		retxml.AddNode("CreationTime", strMsg);
	jobxml.OutNode();							retxml.OutNode();

	jobxml.FindSibingNode("content");			retxml.AddNode("content");
	jobxml.IntoNode();							retxml.IntoNode();
	jobxml.FindSibingNode("TaskID");			jobxml.GetData(strVal);	retxml.AddNode("TaskID", strVal);/////////////////////////////////////////////////////////////
	jobxml.FindSibingNode("JobID");				jobxml.GetData(strVal);	retxml.AddNode("JobID", strVal);/////////////////////////////////////////////////////////////
	jobxml.FindSibingNode("ProductiveTaskItemList");	jobxml.IntoNode(); jobxml.FindChildNode("TaskItemType",strVal); jobxml.OutNode();
	retxml.AddNode("JobType", strVal);/////////////////////////////////////////////////////////////
	retxml.AddNode("JobState", 1);//2
	retxml.AddNode("JobLog", "......");//100%

	retxml.AddNode("JobTime", strMsg);

	char retPath[512];	retPath[0] = 0;
	if (lpstrRetXmlPath){
		strcpy(retPath, lpstrRetXmlPath);
	}
	else
	{
		char inifilepath[512];
		if (GetPrivateProfilePath(inifilepath, CONFIG_FILE)) {
			GetPrivateProfileString("PathSetting", "linuxpath", "", retPath, 256, inifilepath); VERF_SLASH(retPath);
			if (!IsExist(retPath))  { GetPrivateProfileString("PathSetting", "windowspath", "", retPath, 256, inifilepath); VERF_SLASH(retPath); }
			if (!IsExist(retPath))  strcpy(retPath, "");	else strcat(retPath, "/");
		}
		char* pS = retPath + strlen(retPath);
		jobxml.IntoNode();
		jobxml.FindChildNode("OutStatusFileName", pS);
		jobxml.OutNode();
	}

	if (retxml.Save(retPath))	printf("Write Status XML File: %s!\n", retPath);
	else printf("Fail to Write Status XML File: %s!\n", retPath);

	jobxml.Close();

	return true;
}

int CMMIPXml::Save(LPCSTR lpstrPath)
{
	if (!Open(lpstrPath))
		SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");

	CompleteNode("root");	IntoNode();
	CompleteNode("head");	IntoNode();
	CompleteNode("RecipientAddress", "MIP");
	OutNode();

	CompleteNode("content");	IntoNode();
	CompleteNode("TaskID", m_hdr.jobName);
	{
		char strTskNam[20];	strcpy(strTskNam, "uno");
		if (m_tskTypeCur &	TSK_ATMCH) strcpy(strTskNam, "BlockAdjustment"); else
		if (m_tskTypeCur &	TSK_ATADJ) strcpy(strTskNam, "BlockAdjustment"); else
		if (m_tskTypeCur & TSK_DEMMCH) strcpy(strTskNam, "DSMAutoGeneration"); else
		if (m_tskTypeCur & TSK_DEMFLT) strcpy(strTskNam, "DEM-PD"); else
		if (m_tskTypeCur & TSK_IMGEPI) strcpy(strTskNam, "EPI-PD"); else
		if (m_tskTypeCur & TSK_IMGREC) strcpy(strTskNam, "ImageOrtho"); else
		if (m_tskTypeCur & TSK_IMGFUS) strcpy(strTskNam, "ImageFusion"); else
		if (m_tskTypeCur & TSK_IMGMSC) strcpy(strTskNam, "Mosaic"); else
		if (m_tskTypeCur & TSK_ATEDIT) strcpy(strTskNam, "PCBJ");

		CompleteNode("MJMJobType", strTskNam);
	}

	CompleteNode("ProductCount", "0");
	CompleteNode("ProductList");	IntoNode();
	CompleteNode("Product");	IntoNode();
	OutNode();	OutNode();

	CompleteNode("ProductOutPath", m_strWorkDir);
	CompleteNode("OutJobStatusFileName", m_strStatusPath);
	CompleteNode("OutJobCompleteFileName", m_strCompletePath);

	char strVal[20];
	{
		sprintf(strVal, "%lf", m_prodInfo.fAverHeight);
		CompleteNode("ProdAverHeight", strVal);
		sprintf(strVal, "%lf", m_prodInfo.fDemGSD);
		CompleteNode("ProdDemGsd", strVal);
		sprintf(strVal, "%lf", m_prodInfo.fDomGSD);
		CompleteNode("ProdDomGsd", strVal);
	}

	int sz = 0;
	ImageInfo* pInfo = m_import.GetData(sz);

	RemoveNode("InputFileList");
	CompleteNode("InputCount", sz);
	CompleteNode("InputFileList", NULL, false);
	IntoNode();
	for (int i = 0; i < sz; i++, pInfo++){
		AddNode("InputFile");	IntoNode();
		AddNode("DataID", pInfo->nID);
		AddNode("InputFileName", pInfo->path);
		OutNode();
	}
	OutNode();
	CXmlFile::Save(lpstrPath);
	return ERR_NONE;
}

#ifdef FLOW_PRJ_MIP
//Print Status File for PP to analyze
void PrintLog(const char *pPN, int code, const char *pMsg)
{
	static	time_t	lastTime = 0;	if (lastTime == 0) time(&lastTime);
	time_t curTime;	time(&curTime);
	if (code == 0){
		if (curTime - lastTime < 30) return;
		lastTime = curTime;
	}
	CXmlFile xml;
	xml.Open(pPN);

	char strTime[256];	GetStdCurTime(strTime);

	xml.CompleteNode("root");
	xml.IntoNode();

	xml.CompleteNode("head");
	xml.IntoNode();
	xml.CompleteNode("CreationTime", strTime);
	xml.OutNode();

	xml.CompleteNode("content");
	xml.IntoNode();
	xml.CompleteNode("JobState", code == 2 ? 2 : 1);
	xml.CompleteNode("JobLog", pMsg);
	xml.CompleteNode("JobTime", strTime);

	xml.Save(pPN);
}
#endif