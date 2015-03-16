// XmlPrj.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

// XmlPrj.cpp : Defines the entry point for the DLL application.
//
/*----------------------------------------------------------------------+
|		XmlPrjDoc														|
|       Author:     DuanYanSong  2013/10/10   		            |
|            Ver 1.0													|
|       Copyright (c)2013, WHU RSGIS DPGrid Group                   |
|	         All rights reserved.                                       |
|		ysduan@whu.edu.cn; ysduan@sohu.com              				|
+----------------------------------------------------------------------*/

#include "stdafx.h"
#include "XmlPrj.h"
#include "LxXML.h"
#include "ZY3FileName.h"

#ifndef STATIC_LIB
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
#endif

/////////////////////////////////////////////////////////////////
/// class CArray_SCENEINF
/////////////////////////////////////////////////////////////////
int		CArray_SCENEINF::GetID(ImgInfo* imgInf) {
	if (imgInf->sSceneID[0] == '\0') { char* pS = strrchr(imgInf->sImgPN, '\\'); if (!pS) pS = strrchr(imgInf->sImgPN, '/'); if (!pS) pS = imgInf->sImgPN; printf("Error:UnKnow Image Name Type:%s!\n", pS + 1); return -1; }
	char sID[256];	sprintf(sID, "%s%s", imgInf->sOrbitID, imgInf->sSceneID);
	int i, pos_Buf = -1;
	for (i = 0; i < m_size; i++){ if (!strcmp(sID, (m_pBuf + i)->sID)) pos_Buf = i; }
	return pos_Buf;
}
void	CArray_SCENEINF::AddImgInfo(ImgInfo* imgInf){
	if (imgInf->imgAttrib == IT_MSC || imgInf->imgAttrib == IT_UNKNOW) return;
	int pos_Buf = GetID(imgInf);
	if (pos_Buf == -1) {
		SCENEINF uint; memset(&uint, 0, sizeof(SCENEINF));
		uint.sceneId = atoi(imgInf->sSceneID);
		strcpy(uint.sOrbitID, imgInf->sOrbitID);
		sprintf(uint.sID, "%s%s", imgInf->sOrbitID, imgInf->sSceneID);
		Append(uint); pos_Buf = m_size - 1;
	}
	SCENEINF* pBuf = m_pBuf + pos_Buf;
	switch (imgInf->imgAttrib)
	{
	case IT_FWD:
		strcpy(pBuf->strNameImgF, imgInf->sImgPN);	pBuf->imgFInf = imgInf;
		break;
	case IT_NAD:
		strcpy(pBuf->strNameImgN, imgInf->sImgPN);	pBuf->imgNInf = imgInf;
		break;
	case IT_BWD:
		strcpy(pBuf->strNameImgB, imgInf->sImgPN);	pBuf->imgBInf = imgInf;
		break;
	case IT_MUX:
		strcpy(pBuf->strNameImgC, imgInf->sImgPN);	pBuf->imgCInf = imgInf;
		break;
	default:
		pBuf->prodInf[imgInf->imgAttrib] = imgInf;
	}
};
void CArray_SCENEINF::SaveImgMeta(int idx, Img_Type imgType){

}
/////////////////////////////////////////////////////////////////
/// class CXmlPrj
/////////////////////////////////////////////////////////////////
CXmlPrj::CXmlPrj()
{
	memset(&m_tHdr, 0, sizeof(TskHdr));
	//	memset(&m_tCont,0,sizeof(TskCont)); 

	m_inImg = m_outImg = NULL;
	m_inNum = m_outNum = 0;

	memset(&m_projectionInfo, 0, sizeof(ProjectionInfo));
	m_projectionInfo.nEarthModelID = 3;
	m_projectionInfo.nMapProjID = 2;
	m_projectionInfo.fPrjScale = 1;
	m_projectionInfo.nPrjHemi = 1;
	m_projectionInfo.fPrjOriLat = 0;
	m_projectionInfo.fPrjCenMeri = 114;
	m_projectionInfo.fPrjFalseNor = 0;
	m_projectionInfo.fPrjFalseEas = 500000;
	m_projectionInfo.nElevationID = 0;

	m_scList.Reset();
	*m_strPrjPN = 0;
	*m_sGCDPN = 0;
	*m_sBPCPN = 0;
	*m_sGCPPN = 0;
	*m_sAdjReportPN = 0;
	*m_sMchDir = 0;
}

CXmlPrj::~CXmlPrj()
{
	FreeTskInfo();
}

void CXmlPrj::FreeTskInfo(){
	if (m_inImg) delete[] m_inImg;	m_inImg = NULL;

	if (m_outImg)	delete[] m_outImg;	m_outImg = NULL;

}

int CXmlPrj::InitImgInfoByNam(ImgInfo* imginf){
	return RET_OK;
}

int CXmlPrj::SaveImgMeta(){
	return RET_OK;
}

inline void	ImgInfo4ImageInfo(ImgInfo* p1, const ImageInfo* p2, const ImgExInfo* p3)
{
	memset(p1, 0, sizeof(ImgInfo));

	sprintf(p1->sID, "%d", p2->nID);
	p1->nDatID = p2->nID;
	strcpy(p1->sImgPN, p2->path);

	switch (p2->lvl)
	{
	case IL_SC:
		if (p2->cam == CT_MUX) p1->imgAttrib = IT_MUX; else
		if (p2->cam == CT_FWD) p1->imgAttrib = IT_FWD; else
		if (p2->cam == CT_NAD) p1->imgAttrib = IT_NAD; else
		if (p2->cam == CT_BWD) p1->imgAttrib = IT_BWD;
		break;
	case IL_DSM:
		p1->imgAttrib = IT_DSM;
		break;
	case IL_DEM:
		p1->imgAttrib = IT_DEM;
		break;
	case IL_EPI:
		if (p2->cam == CT_FWD) p1->imgAttrib = IT_EPIF; else
		if (p2->cam == CT_NAD) p1->imgAttrib = IT_EPIN; else
		if (p2->cam == CT_BWD) p1->imgAttrib = IT_EPIB;
		break;
	case IL_CJZ:
		if (p2->cam == CT_MUX)	p1->imgAttrib = IT_XTJZC;
		else	p1->imgAttrib = IT_XTJZN;
		break;
	case IL_JJZ:
		if (p2->cam == CT_MUX)	p1->imgAttrib = IT_JJZC;
		else	p1->imgAttrib = IT_JJZN;
		break;
	case IL_DOM:
		if (p2->cam == CT_MUX)	p1->imgAttrib = IT_DOMC;
		else	p1->imgAttrib = IT_DOMN;
		break;
	case IL_FUS:
		p1->imgAttrib = IT_FUS;
		break;
	case IL_MSC:
		p1->imgAttrib = IT_MSC;
		break;
	default:
		break;
	}
	strcpy(p1->sSceneID, p2->sceneID);
	strcpy(p1->sOrbitID, p2->orbitID);
	// 	memcpy(p1->sSceneID,p2->sceneID,7);
	// 	memcpy(p1->sOrbitID,p2->orbitID,9);

	if (!p3){
		CZY3SceneFile	file;
		if (file.load(p2) != ERR_NONE)	return;
		CZY3SceneFile* pFile = (CZY3SceneFile*)(file.GetInterObject());

		pFile->GetOriFilePath(p1->sOrgRPCPath, false);
		pFile->GetOriFilePath(p1->sAdjRPCPath);
		pFile->GetThumbPath(p1->sThumbPN);
		pFile->GetBrowsePath(p1->sBrowsePN);
		pFile->GetMetaPath(p1->sMetaPN);
	}
	else{
		char strDir[512] = "";	if (p3->strFileDir[0])	{ strcpy(strDir, p3->strFileDir); VERF_SLASH(strDir);	strcat(strDir, "/"); }
		if (p3->strOrgRpcNam[0])	{ sprintf(p1->sOrgRPCPath, "%s%s", strDir, p3->strOrgRpcNam); }
		else { strcpy(p1->sOrgRPCPath, p1->sImgPN);	strcpy(strrchr(p1->sOrgRPCPath, '.'), "_rpc.txt"); }
		if (p3->strAdjRpcNam[0])	{ sprintf(p1->sAdjRPCPath, "%s%s", strDir, p3->strAdjRpcNam); }
		else { strcpy(p1->sAdjRPCPath, p1->sImgPN);	strcpy(strrchr(p1->sAdjRPCPath, '.'), ".tfw"); }
		if (p3->strTumbNam[0])	{ sprintf(p1->sThumbPN, "%s%s", strDir, p3->strTumbNam); }
		else { strcpy(p1->sThumbPN, p1->sImgPN);	strcpy(strrchr(p1->sThumbPN, '.'), "_ico.jpg"); }
		if (p3->strBrowseNam[0])	{ sprintf(p1->sBrowsePN, "%s%s", strDir, p3->strBrowseNam); }
		else { strcpy(p1->sBrowsePN, p1->sImgPN);	strcpy(strrchr(p1->sBrowsePN, '.'), "_pre.jpg"); }
		if (p3->strMetaNam[0])	{ sprintf(p1->sMetaPN, "%s%s", strDir, p3->strMetaNam); }
		else { strcpy(p1->sMetaPN, p1->sImgPN);	strcpy(strrchr(p1->sMetaPN, '.'), ".xml"); }
	}

}

int CXmlPrj::LoadPrj(const char* sXmlPN){

	m_scList.Reset(NULL, 0);
	if (m_inImg) delete[] m_inImg;	m_inImg = NULL;
	if (m_outImg) delete[] m_outImg; m_outImg = NULL;

	CPrjXml xml;
	if (xml.load(sXmlPN) != ERR_NONE) return RET_ERR;
	CPrjXml* pXml = (CPrjXml*)(xml.GetInterObject());

	char strPath[512];	strcpy(strPath, sXmlPN);	Dos2Unix(strPath);
	char* pS = strrchr(strPath, '/');
	char strNam[100];	strcpy(strNam, pS);
	char* pN = strrchr(strNam, '.');

	int tskType = pXml->m_tskTypeCur;	int i;
	if (tskType&TSK_ATMCH || tskType&TSK_ATADJ)
	{
		m_tHdr.tskType = TT_AT;
		LPIMAGEINF pInfo = pXml->m_import.GetData(m_inNum);	if (m_inNum < 1) { printf("Error:No File for AT!\n");	return RET_ERR; }
		int numEx = 0;	LPIMGEXINFO pInfoEx = pXml->m_importEx.GetData(numEx);

		m_inImg = new ImgInfo[m_inNum];	memset(m_inImg, 0, sizeof(ImgInfo)*m_inNum);

		for (i = 0; i < m_inNum; i++, pInfo++){
			ImgInfo4ImageInfo(m_inImg + i, pInfo, numEx == 0 ? NULL : pInfoEx);// delete image which is not SC
			if (i < numEx)	pInfoEx++; else pInfoEx = NULL;
		}
		m_scList.AddImgInfoList(m_inImg, m_inNum);

		strcpy(pN, "_Save.gcp");	sprintf(pS, "/AT/ATMCH/%s", strNam);	strcpy(m_sGCPPN, strPath);
		strcpy(pN, "_Tie.bpc");	sprintf(pS, "/AT/ATMCH/%s", strNam);	strcpy(m_sBPCPN, strPath);
		strcpy(pS, "/Product/AT/");	strcpy(m_sMchDir, strPath);
		sprintf(m_sAdjReportPN, pXml->m_strProductSpace,"Calibration/OptimizedResultData/MultiResolution/calibResult.txt");

	}
	else
	if (tskType&TSK_IMGEPI)
	{
		m_tHdr.tskType = TT_EPI;
		LPIMAGEINF pInfo = pXml->m_import.GetData(m_inNum);	if (m_inNum < 3) { printf("Error:No enough File for EPI!\n");	return RET_ERR; }
		m_inImg = new ImgInfo[m_inNum];	memset(m_inImg, 0, sizeof(ImgInfo)*m_inNum);
		int numEx = 0;	LPIMGEXINFO pInfoEx = pXml->m_importEx.GetData(numEx);

		for (i = 0; i < m_inNum; i++, pInfo++){
			ImgInfo4ImageInfo(m_inImg + i, pInfo, numEx == 0 ? NULL : pInfoEx);
			if (i < numEx)	pInfoEx++; else pInfoEx = NULL;
		}
		m_scList.AddImgInfoList(m_inImg, m_inNum);

		pInfo = pXml->m_export.GetData(m_outNum);				if (m_outNum < 3) { printf("Error:No enough File for EPI!\n");	return RET_ERR; }
		m_outImg = new ImgInfo[m_outNum];	memset(m_outImg, 0, sizeof(ImgInfo)*m_outNum);
		pInfoEx = pXml->m_exportEx.GetData(numEx);

		for (i = 0; i < m_outNum; i++, pInfo++){
			ImgInfo4ImageInfo(m_outImg + i, pInfo, numEx == 0 ? NULL : pInfoEx);
			if (i < numEx)	pInfoEx++; else pInfoEx = NULL;
			(m_outImg + i)->fGsd = pXml->m_prodInfo.fDomGSD;
		}
		m_scList.AddImgInfoList(m_outImg, m_outNum);
	}
	else
	if (tskType&TSK_DEMMCH)
	{
		m_tHdr.tskType = TT_DSM;
		LPIMAGEINF pInfo = pXml->m_import.GetData(m_inNum);	if (m_inNum < 3) { printf("Error:No enough File for DSM!\n");	return RET_ERR; }
		m_inImg = new ImgInfo[m_inNum];	memset(m_inImg, 0, sizeof(ImgInfo)*m_inNum);
		int numEx = 0;	LPIMGEXINFO pInfoEx = pXml->m_importEx.GetData(numEx);

		for (i = 0; i < m_inNum; i++, pInfo++){
			ImgInfo4ImageInfo(m_inImg + i, pInfo, numEx == 0 ? NULL : pInfoEx);
			if (i < numEx)	pInfoEx++; else pInfoEx = NULL;
		}
		m_scList.AddImgInfoList(m_inImg, m_inNum);

		pInfo = pXml->m_export.GetData(m_outNum);				if (m_outNum < 1) { printf("Error:No enough File for DSM!\n");	return RET_ERR; }
		m_outImg = new ImgInfo[m_outNum];	memset(m_outImg, 0, sizeof(ImgInfo)*m_outNum);
		pInfoEx = pXml->m_exportEx.GetData(numEx);

		for (i = 0; i < m_outNum; i++, pInfo++){
			ImgInfo4ImageInfo(m_outImg + i, pInfo, numEx == 0 ? NULL : pInfoEx);
			if (i < numEx)	pInfoEx++; else pInfoEx = NULL;
			(m_outImg + i)->fGsd = pXml->m_prodInfo.fDemGSD;
		}
		m_scList.AddImgInfoList(m_outImg, m_outNum);
	}
	else
	if (tskType&TSK_DEMFLT)
	{
		int num1, num2, ci, co;
		m_tHdr.tskType = TT_DEM;

		m_inNum = pXml->m_import.GetImgNum(IL_DSM) + pXml->m_export.GetImgNum(IL_DSM);	if (m_inNum < 1) { printf("Error:No enough DSM File for DEM!\n");	return RET_ERR; }
		m_inImg = new ImgInfo[m_inNum];		memset(m_inImg, 0, sizeof(ImgInfo)*m_inNum);

		m_outNum = pXml->m_import.GetImgNum(IL_DEM) + pXml->m_export.GetImgNum(IL_DEM);	if (m_outNum < 1) { printf("Error:No enough DSM File for DEM!\n");	return RET_ERR; }
		m_outImg = new ImgInfo[m_outNum];	memset(m_outImg, 0, sizeof(ImgInfo)*m_outNum);
		int numEx_in = 0;		LPIMGEXINFO pInfoEx_in = pXml->m_importEx.GetData(numEx_in);
		int numEx_out = 0;	LPIMGEXINFO pInfoEx_out = pXml->m_exportEx.GetData(numEx_out);

		LPIMAGEINF pInfo = pXml->m_import.GetData(num1);
		ci = 0;	co = 0;
		for (i = 0; i < num1; i++, pInfo++){
			if (pInfo->lvl == IL_DSM) {
				ImgInfo4ImageInfo(m_inImg + ci, pInfo, numEx_in == 0 ? NULL : pInfoEx_in); ci++;
			}
			else	if (pInfo->lvl == IL_DEM) {
				ImgInfo4ImageInfo(m_outImg + co, pInfo, numEx_in == 0 ? NULL : pInfoEx_in);	(m_outImg + co)->fGsd = pXml->m_prodInfo.fDemGSD;	co++;
			}
			if (ci < numEx_in)	pInfoEx_in++; else pInfoEx_in = NULL;
		}
		pInfo = pXml->m_export.GetData(num2);
		for (i = 0; i < num2; i++, pInfo++){
			if (pInfo->lvl == IL_DSM) {
				ImgInfo4ImageInfo(m_inImg + ci, pInfo, numEx_out == 0 ? NULL : pInfoEx_out); ci++;
			}
			else	if (pInfo->lvl == IL_DEM) { ImgInfo4ImageInfo(m_outImg + co, pInfo, numEx_out == 0 ? NULL : pInfoEx_out);	(m_outImg + co)->fGsd = pXml->m_prodInfo.fDemGSD;	co++; }
			if (co < numEx_out)	pInfoEx_out++; else pInfoEx_out = NULL;
		}
		m_scList.AddImgInfoList(m_inImg, m_inNum);

		m_scList.AddImgInfoList(m_outImg, m_outNum);
	}
	else
	if (tskType&TSK_IMGREC)////////////////////////////
	{
		m_tHdr.tskType = TT_DOM;

		LPIMAGEINF pInfo = pXml->m_import.GetData(m_inNum);
		int numEx = 0;	LPIMGEXINFO pInfoEx = pXml->m_importEx.GetData(numEx);

		if (m_inNum > 0){
			m_inImg = new ImgInfo[m_inNum];	memset(m_inImg, 0, sizeof(ImgInfo)*m_inNum);

			for (i = 0; i < m_inNum; i++, pInfo++){
				ImgInfo4ImageInfo(m_inImg + i, pInfo, numEx == 0 ? NULL : pInfoEx);
				if (i < numEx)	pInfoEx++; else pInfoEx = NULL;
				(m_inImg + i)->fGsd = pXml->m_prodInfo.fDomGSD;
				(m_inImg + i)->fAvrH = pXml->m_prodInfo.fAverHeight;
			}
			m_scList.AddImgInfoList(m_inImg, m_inNum);
		}

		pInfo = pXml->m_export.GetData(m_outNum);	if (m_outNum < 1) { printf("Error:No enough  File for REC!\n");	return RET_ERR; }
		m_outImg = new ImgInfo[m_outNum];	memset(m_outImg, 0, sizeof(ImgInfo)*m_outNum);
		pInfoEx = pXml->m_exportEx.GetData(numEx);

		for (i = 0; i < m_outNum; i++, pInfo++){
			ImgInfo4ImageInfo(m_outImg + i, pInfo, numEx == 0 ? NULL : pInfoEx);
			if (i < numEx)	pInfoEx++; else pInfoEx = NULL;
			(m_outImg + i)->fGsd = pXml->m_prodInfo.fDomGSD;
			(m_outImg + i)->fAvrH = pXml->m_prodInfo.fAverHeight;
		}
		m_scList.AddImgInfoList(m_outImg, m_outNum);
	}
	else
	if (tskType&TSK_IMGFUS)
	{
		m_tHdr.tskType = TT_FUS;

		LPIMAGEINF pInfo = pXml->m_import.GetData(m_inNum);
		m_inNum -= pXml->m_import.GetImgNum(IL_SC);
		int numEx = 0;	LPIMGEXINFO pInfoEx = pXml->m_importEx.GetData(numEx);//////////////////////////////////

		int cnt = 0;
		if (m_inNum > 0){
			m_inImg = new ImgInfo[m_inNum];	memset(m_inImg, 0, sizeof(ImgInfo)*m_inNum);

			for (i = 0; i < m_inNum; i++, pInfo++){
				if (pInfo->lvl == IL_SC) continue;
				ImgInfo4ImageInfo(m_inImg + cnt, pInfo, numEx == 0 ? NULL : pInfoEx);
				if (cnt < numEx)	pInfoEx++; else pInfoEx = NULL;
				(m_inImg + cnt)->fGsd = pXml->m_prodInfo.fDomGSD;
				cnt++;
			}
			m_scList.AddImgInfoList(m_inImg, m_inNum);
		}

		pInfo = pXml->m_export.GetData(m_outNum);	if (m_inNum + m_outNum < 3) { printf("Error:No enough  File for FUS!\n");	return RET_ERR; }
		m_outImg = new ImgInfo[m_outNum];	memset(m_outImg, 0, sizeof(ImgInfo)*m_outNum);
		pInfoEx = pXml->m_exportEx.GetData(numEx);

		for (i = 0; i < m_outNum; i++, pInfo++){
			ImgInfo4ImageInfo(m_outImg + i, pInfo, numEx == 0 ? NULL : pInfoEx);
			if (i < numEx)	pInfoEx++; else pInfoEx = NULL;
			(m_outImg + i)->fGsd = pXml->m_prodInfo.fDomGSD;
		}
		m_scList.AddImgInfoList(m_outImg, m_outNum);
	}
	else
	if (tskType&TSK_IMGMSC)
	{
		m_tHdr.tskType = TT_MSC;

		LPIMAGEINF pInfo1, pInfo2;	int num1, num2;
		pInfo1 = pXml->m_import.GetData(num1);	pInfo2 = pXml->m_export.GetData(num2);
		m_inNum = num1 + num2 - pXml->m_import.GetImgNum(IL_SC) - 1;	if (m_inNum < 2) { printf("Error:No enough  File for MSC!\n");	return RET_ERR; }
		m_inImg = new ImgInfo[m_inNum];		m_outNum = 1;	memset(m_inImg, 0, sizeof(ImgInfo)*m_inNum);
		m_outImg = new ImgInfo[m_outNum];	memset(m_outImg, 0, sizeof(ImgInfo)*m_outNum);

		int numEx_in = 0;		LPIMGEXINFO pInfoEx_in = pXml->m_importEx.GetData(numEx_in);
		int numEx_out = 0;	LPIMGEXINFO pInfoEx_out = pXml->m_exportEx.GetData(numEx_out);

		int cnt = 0;

		for (i = 0; i < num1; i++, pInfo1++){
			if (pInfo1->lvl == IL_SC) continue;
			ImgInfo4ImageInfo(m_inImg + cnt, pInfo1, pInfoEx_in);	cnt++;
			if (cnt < numEx_in)	pInfoEx_in++; else pInfoEx_in = NULL;
		}
		for (; i < num2 + num1; i++, pInfo2++){
			if (pInfo2->lvl == IL_MSC) {
				ImgInfo4ImageInfo(m_outImg, pInfo2, pInfoEx_out);
				continue;
			}
			ImgInfo4ImageInfo(m_inImg + cnt, pInfo2, pInfoEx_out);	cnt++;
			if (cnt < numEx_out)	pInfoEx_out++; else pInfoEx_out = NULL;
		}
		m_scList.AddImgInfoList(m_inImg, m_inNum);

		strcpy(m_sMchDir, pXml->m_prodInfo.strTempletPath);

	}

	strcpy(m_tHdr.sTskID, pXml->m_hdr.jobID[0] ? pXml->m_hdr.jobID : pXml->m_hdr.jobName);
	strcpy(m_sGCDPN, pXml->m_prodInfo.strGcdPath);
	strcpy(m_strPrjPN, sXmlPN);
	m_prjXMLAttrib = XX_MIP;
	return RET_OK;
}

int CXmlPrj::SaveProdGcdFile(const char* sProdGcdPN){
	CopyFile(m_sGCDPN, sProdGcdPN, FALSE);
	return RET_OK;
}


int CXmlPrj::Check()
{
	return RET_OK;
}

void CXmlPrj::SaveRetXml(const char* lpExe, const char* lpXml/* =NULL */, bool bRunOk/* =true */, const char* lpMsg/* =NULL */)
{
	char strPath[512], strXml[512];

	if (!lpXml) { if (!*m_strPrjPN) return;  	strcpy(strXml, m_strPrjPN);	strcpy(strrchr(strXml, '.'), ".result.xml"); }
	else strcpy(strXml, lpXml);

	strcpy(strPath, lpExe);
	char* pS = strrchr(strPath, '/');	if (!pS) pS = strrchr(strPath, '\\');
	strcpy(pS, "/RetXml.dat");
	if (!IsExist(strPath)) { FILE* fp = fopen(strPath, "w"); if (!fp) fp = fopen(strXml, "w");	fputs(RET_XML_CONTENT, fp); fclose(fp);	if (!IsExist(strPath))	return; }

	CXmlFile xml;
	xml.Open(strPath);

	xml.FindSibingNode("task");
	xml.SetAttrib("id", m_tHdr.sTskID);


	xml.FindChildNode("result");
	if (bRunOk){
		xml.SetChildData("success");
		xml.FindChildNode("ret");
		xml.SetChildData("1");
	}
	else{
		xml.SetChildData("fail");
		xml.FindChildNode("ret");
		xml.SetChildData("0");

		if (lpMsg)	{
			xml.FindChildNode("message");
			xml.SetChildData(lpMsg);
		}
	}

	xml.Save(strXml);
}

void CXmlPrj::SaveTskHdr2Bin(TskHdr& thdr)
{
	int		tsk;
	TASK_TYPE	tsk_ = thdr.tskType;

	if (tsk_ == TT_AT) tsk = TSK_ATMCH | TSK_ATADJ; else
	if (tsk_ == TT_ATMCH) tsk = TSK_ATMCH; else
	if (tsk_ == TT_ATADJ) tsk = TSK_ATADJ; else
	if (tsk_ == TT_EPI) tsk = TSK_IMGEPI; else
	if (tsk_ == TT_DSM) tsk = TSK_DEMMCH; else
	if (tsk_ == TT_DEM) tsk = TSK_DEMFLT; else
	if (tsk_ == TT_DOM) tsk = TSK_IMGREC; else
	if (tsk_ == TT_FUS) tsk = TSK_IMGFUS; else
	if (tsk_ == TT_MSC) tsk = TSK_IMGMSC;

	char strPath[512];	strcpy(strPath, m_strPrjPN);
	char* pS = strPath + strlen(strPath);	strcpy(pS, ".tsk");

	CXmlFile xml;
	if (!xml.Open(strPath))	{ printf("Fail to Open TskXml:%s\n%s!", strPath, xml.GetXmlError());	return; }

	char strVal[512];
	if (!xml.FindNode(Mode_Absolute, strVal, 2, "SatPrjXml", "TskCur"))
	{
		printf("Error: Can't find the node: <TskCur> !");	return;
	}

	sprintf(strVal, "%d", tsk);
	xml.SetData(strVal);

	xml.Save(strPath);
}
