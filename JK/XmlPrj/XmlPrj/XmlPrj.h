// XmlPrj.h : main header file for the XMLPRJ DLL
/*----------------------------------------------------------------------+
|		XmlPrj														|
|       Author:     DuanYanSong  2013/07/24   		            |
|            Ver 1.0													|
|       Copyright (c)2013, WHU RSGIS DPGrid Group                   |
|	         All rights reserved.                                       |
|		ysduan@whu.edu.cn; ysduan@sohu.com              				|
+----------------------------------------------------------------------*/
#ifndef XMLPRJ_H_DUANYANSONG_2013_07_24_22_33_53078
#define XMLPRJ_H_DUANYANSONG_2013_07_24_22_33_53078

#include "stdafx.h"

#ifndef XMLPRJ_LIB

#define XMLPRJ_LIB  __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib,"XmlPrjD.lib") 
#pragma message("Automatically linking with XmlPrjD.lib") 
#else
#pragma comment(lib,"XmlPrj.lib") 
#pragma message("Automatically linking with XmlPrj.lib") 
#endif

#else

#endif

#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <math.h>

//#define	 PRJ_TAG_V1		"MIP_PRJ_V1.0"
#define	 PRJ_TAG_V2		"MIP_PRJ_V2.0"

#define  RET_OK					0x1001
#define  RET_ERR				0x1002

/*
��ģ������Ŀ¼
��������������������������������������������������������������������������������
����		    ����Ŀ¼			���������ļ�
��������������������������������������������������������������������������������
��Ŀ¼		    (Prj)/				-
���п��ƿ���	(Prj)/				-

����(����ƥ��)	(Prj)/AT/ATMCH		/ATMCH.TSK,ATMCH.OVF
����(����ƽ��)	(Prj)/AT/ATADJ
����(�����༭)	(Prj)/AT/ATADJ
����(�����ʼ�)	(Prj)/AT/ATADJ
����(�����ɹ�)	(Prj)/AT/PRODUCT

DEM(��ϸƥ��)	(Prj)/DEM/DEMMCH    /DEMMCH1.TSK,DEMMCH2.TSK,DEMMCH3.TSK,DEMMCH.OVF
DEM(����ƥ��)	(Prj)/DEM/DEMMCH    /DEMMCH.TSK,DEMMCH.OVF
DEM(DEM���� )	(Prj)/DEM/DEMEDT
DEM(DEM�ʼ� )	(Prj)/DEM/QCHK
DEM(DEM�ɹ�)	(Prj)/DEM/PRODUCT

DOM(��������)   (Prj)/DOM/EPI       /EPI.TSK,EPI.OVF
DOM(Ӱ�����)   (Prj)/DOM/REC       /REC.TSK,REC.OVF
DOM(Ӱ���ں�)   (Prj)/DOM/FUS       /FUS.TSK,FUS.OVF
DOM(Ӱ����Ƕ)   (Prj)/DOM/MZX       /MZX.TSK,MZX.OVF
DOM(Ӱ���ʼ�)   (Prj)/DOM/QCHK
*/

#define ATMCH_TSK_PRJ		"/AT/ATMCH.TSK"
#define ATMCH_TSK_OVF		"/AT/ATMCH.OVF"

#define DEMQMCH_TSK_PRJ 	"/DEM/DEMMCH/DEMMCH.TSK"
#define DEMMCH_TSK1_PRJ 	"/DEM/DEMMCH/DEMMCH1.TSK"
#define DEMMCH_TSK2_PRJ 	"/DEM/DEMMCH/DEMMCH2.TSK"
#define DEMMCH_TSK3_PRJ 	"/DEM/DEMMCH/DEMMCH3.TSK"
#define DEMMCH_TSK_OVF 	    "/DEM/DEMMCH/DEMMCH.OVF"

#define DOMEPI_TSK_PRJ		"/DOM/EPI/EPI.TSK"
#define DOMEPI_TSK_OVF		"/DOM/EPI/EPI.OVF"
#define DOMREC_TSK_PRJ		"/DOM/REC/REC.TSK"
#define DOMREC_TSK_OVF		"/DOM/REC/REC.OVF"
#define DOMFUS_TSK_PRJ		"/DOM/FUS/FUS.TSK"
#define DOMFUS_TSK_OVF		"/DOM/FUS/FUS.OVF"
#define DOMMZX_TSK_PRJ		"/DOM/EPI/MZX.TSK"
#define DOMMZX_TSK_OVF		"/DOM/EPI/MZX.OVF"

#define RET_XML_CONTENT		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<task name=\"ImgProcess\" id=\"001\" orderid=\"001\">\n<result>success</result>\n<ret>1</ret>\n<message></message>\n</task>\n"


/********************************************************************
MIPPrj
created:	2013/03/23
author:		LX
purpose:	This file is for Read XML format MIPPrj function
*********************************************************************/

//---------------------------Define XML Structure
enum XML_Type { XX_MIP, XX_DQDA };

enum TASK_TYPE
{
	TT_UNKNOW = 0,
	TT_ATMCH,
	TT_ATADJ,
	TT_AT,
	TT_DSM,
	TT_DEM,
	TT_EPI,
	TT_JJZ,
	TT_DOM,
	TT_FUS,
	TT_MSC,
	TT_END,
};

enum Img_Type{ IT_UNKNOW = 0, IT_DSM, IT_DEM, IT_EPIN, IT_XTJZN, IT_XTJZC, IT_JJZN, IT_JJZC, IT_DOMN, IT_DOMC, IT_FUS, IT_MSC, IT_EPIF, IT_EPIB, IT_FWD, IT_BWD, IT_NAD, IT_MUX };

#ifndef _TASK_HEADER
#define _TASK_HEADER
typedef struct tagTskHdr{
	TASK_TYPE tskType;
	char	sTskID[45];
	char	sSaveDir[512];
}TskHdr;
#endif
//Ӱ��������Ϣ-mlw
#ifndef _IMGINFO
#define _IMGINFO
typedef struct tagImgInfo{
	char	sID[20];//sSceneID+sOrbitID
	int		nDatID;				//��ÿ���������ݷ���һ��ID,ID��1��ʼ
	char	sImgPN[512];//Ӱ��·��-mlw
	Img_Type	imgAttrib;//Ӱ�񼶱�-mlw
	char	sOrgRPCPath[512];	//ԭʼRPC·��-mlw	
	char	sAdjRPCPath[512];	//if sAttrib is DOM/JJZ/FUS/MSC,this stores tfw
	char	sThumbPN[512];//Ĵָͼ·��-mlw
	char	sBrowsePN[512];		//���ͼ·��-mlw
	char	sMetaPN[512]; //Ԫ����·��-mlw
#ifdef PRJ_TAG_V1
	char	sSceneID[8];
	char	sOrbitID[10];
#else
	char	sSceneID[128];//path-row-mlw
	char	sOrbitID[128];		//���Ȧ��
#endif
	double	fGsd;//�ֱ���-mlw
	double	fAvrH;//ƽ���߳�-mlw
	double	fLat_LT;			//���Ͻ�γ��
	double	fLong_LT;			//���ϽǾ���
	double	fLat_RB;			//���½�γ��
	double	fLong_RB;			//���½Ǿ���
}ImgInfo;
#endif
//ͶӰ��Ϣ-mlw
#ifndef _PROJECTIONINFO
#define _PROJECTIONINFO
typedef struct tagProjectionInfo{
	int		nEarthModelID;	//WGS84��CGCS2000��
	int		nMapProjID;		//Gauss ��UTM
	double	fPrjScale;
	int		nPrjHemi;
	double	fPrjOriLat;
	double	fPrjCenMeri;
	double	fPrjFalseNor;
	double	fPrjFalseEas;
	int		nZoneType;
	int		nZoneNo;
	int		nElevationID;//?????-mlw
	double	fBur7Par[7];
}ProjectionInfo;
#endif
//����Ϣ-mlw
#ifndef _SCENEINF
#define _SCENEINF
typedef struct tagSCENEINF
{
	unsigned long sceneId;
#ifdef PRJ_TAG_V1
	char	sID[20];// [OrbitID+SceneID]
	char	sOrbitID[10];
#else
	char	sID[256];// [OrbitID+SceneID]
	char	sOrbitID[128];
#endif

	double	centE, centN;

	char	strNameImgF[512];//../ZY3_01a_hsnfavp_001166_20121209_111837_0008_SASMAC_CHN_sec_rel_001_1212135661.tif
	char	strNameImgN[512];//../ZY3_01a_hsnnavp_001166_20121209_111906_0007_SASMAC_CHN_sec_rel_001_1212135835.tif
	char	strNameImgB[512];//../ZY3_01a_hsnbavp_001166_20121209_111935_0008_SASMAC_CHN_sec_rel_001_1212135755.tif
	char	strNameImgC[512];//../ZY3_01a_hsnfavm_001166_20121209_111859_0007_SASMAC_CHN_sec_rel_004_1212135919.tif

	ImgInfo* imgFInf;
	ImgInfo* imgNInf;
	ImgInfo* imgBInf;
	ImgInfo* imgCInf;

	ImgInfo* prodInf[15];


}SCENEINF;
#else
#pragma message("ZY3Prj.hpp, Warning: SCENEINF alread define, be sure it was define as: struct tagSCENEINF{ char strPathName[512];int startLines,endLines;double lbLon,lbLat,rbLon,rbLat,ltLon,ltLat,rtLon,rtLat; }. \
\nZY3Prj.hpp, ����:���� SCENEINF �Ѿ������,��ȷ���䶨��Ϊ: struct tagSCENEINF{ char strPathName[512];int startLines,endLines;double lbLon,lbLat,rbLon,rbLat,ltLon,ltLat,rtLon,rtLat; }.")
#endif

class XMLPRJ_LIB CArray_SCENEINF
{
public:
	CArray_SCENEINF(SCENEINF* pBuf = NULL, int size = 0){ m_maxSize = m_size = size; m_pBuf = NULL; if (m_size) m_pBuf = new SCENEINF[m_size]; if (pBuf) memcpy(m_pBuf, pBuf, sizeof(SCENEINF)*m_size); };
	virtual ~CArray_SCENEINF(){ if (m_pBuf) delete[]m_pBuf; };

	inline	SCENEINF*	GetData(int &size){ size = m_size; return m_pBuf; };
	inline	int			Append(SCENEINF uint){ if (m_size >= m_maxSize){ m_maxSize += 30; SCENEINF* pOld = m_pBuf; m_pBuf = new SCENEINF[m_maxSize]; memset(m_pBuf, 0, sizeof(SCENEINF)*m_maxSize); memcpy(m_pBuf, pOld, sizeof(SCENEINF)*m_size);	delete[]pOld; } m_pBuf[m_size] = uint; m_size++; return (m_size - 1); };
	inline	void		SetSize(int size){ if (size<m_maxSize) m_size = size; else Reset(NULL, size); };
	inline	void		Reset(SCENEINF* pBuf = NULL, int size = 0){ if (m_pBuf) delete[]m_pBuf; m_pBuf = NULL; m_maxSize = m_size = size; if (m_maxSize){ m_pBuf = new SCENEINF[m_maxSize]; memset(m_pBuf, 0, sizeof(SCENEINF)*m_maxSize); } if (pBuf) memcpy(m_pBuf, pBuf, sizeof(SCENEINF)*m_size); }
	inline  SCENEINF&   operator[](int idx){ return m_pBuf[idx]; };
	inline	SCENEINF	GetAt(int idx){ if (idx >= m_size) { printf("Warning:Ask for too much Scene!\n"); return m_pBuf[m_size - 1]; } return m_pBuf[idx]; };
	inline	int			GetSize(){ return m_size; };

	int					GetID(ImgInfo* imgInf);
	inline	void		AddImgInfoList(ImgInfo* imgInfo, int num) { int i = 0; ImgInfo* pImgInfo = imgInfo;	for (; i<num; i++, pImgInfo++){ AddImgInfo(pImgInfo); } }
	void				AddImgInfo(ImgInfo* imgInf);
	void				SaveImgMeta(int idx, Img_Type imgType);
private:
	SCENEINF*		m_pBuf;
	int				m_size;
	int				m_maxSize;
};

class XMLPRJ_LIB CXmlPrj
{
public:
	CXmlPrj();
	virtual ~CXmlPrj();

	int		LoadPrj(const char* sXmlPN);
	int		SaveProdGcdFile(const char* sProdGcdPN);
	int		SaveImgMeta();
	int		InitImgInfoByNam(ImgInfo* imginf);
	int		ReadImgMeta(const char* sImgMetaPN, ImgInfo* pInFile);
	void 	SaveRetXml(const char* lpExe, const char* lpXml = NULL, bool bRunOk = true, const char* lpMsg = NULL);
	void	SaveTskHdr2Bin(TskHdr& thdr);
	// 	void	RemoveXmlBin(const char* lpStrXml);

protected:
	void	FreeTskInfo();
	int		Check();
	// 	int		LoadXmlBin(const char* lpStr);
	// 	void	SaveXmlBin(const char* lpStr);

public:
	TskHdr			m_tHdr;
	ImgInfo*		m_inImg;
	int				m_inNum;
	ImgInfo*		m_outImg;
	int				m_outNum;
	ProjectionInfo	m_projectionInfo;
	char			m_sGCDPN[512];

	char			m_sBPCPN[512];
	char			m_sGCPPN[512];
	char			m_sAdjReportPN[512];
	char			m_sMchDir[512];//if task is msc ,this store the pattern plate

	char			m_strPrjPN[512];

	CArray_SCENEINF m_scList;
	XML_Type		m_prjXMLAttrib;
protected:

};

#endif