////MMipXml.h
/********************************************************************
MMipXml
created:	2015/03/09
author:		MLW
purpose:	This file is for MMipXml function
*********************************************************************/
#if !defined MMipXml_h__MLW_2015_3_9
#define MMipXml_h__MLW_2015_3_9

#include "LxXML.h"
#ifdef WIN32
#include "ogr_geometry.h"
#else
#include <ogr_geometry.h>
#endif // WIN32


#include "GeoFile.hpp"
#define MMIP_XML_ONLY	"<ProductiveTaskItemList>"

typedef struct tagGEOREFF
{
	int elpsType ;//��������
	int projType ;//ͶӰ����
	double a , b;//���򳤶̰뾶
	double Origin_Latitude ;//��ʼγ��
	double Central_Meridian;//���뾭��
	double False_Easting ;//��ƫ
	double False_Northing;//��ƫ
	double Scale_Factor;//ͶӰ����
	int eleType ;
	/*struct tagGEOREFF()
	{
		elpsType = ET_CN2000;//��������
		projType = GAUSS_PROJECTION;//ͶӰ����
		a = SEMIMAJOR_CN2000; b = SEMIMINOR_CN2000;//���򳤶̰뾶
		Origin_Latitude = 0.0;//��ʼγ��
		Central_Meridian = 117 ;//���뾭��
		False_Easting = 500000.0;//��ƫ
		False_Northing = 0.0;//��ƫ
		Scale_Factor = 1.0;//ͶӰ����
		eleType = 0;//��
	};*/
}GeoRef;
inline bool SetEllipsoidMode(char *strVal, GeoRef &geoRef, char*strDescType)
{
	if (!strcmp(strDescType, "Standard"))
	{
		if (!strcmp(strVal, "WGS84")) {
			geoRef.elpsType = ET_WGS84; return true;
		}
		else if (!strcmp(strVal, "CGCS2000")) {
			geoRef.elpsType = ET_CN2000; return true;
		}
		else if (!strcmp(strVal, "BeiJing54")) {
			geoRef.elpsType = ET_BEIJING54; return true;
		}
		else if (!strcmp(strVal, "XiAn80")) {
			geoRef.elpsType = ET_XIAN80; return true;
		}
		else return false;
	}
	else if (!strcmp(strDescType, "WKT"))
	{
		if (!strcmp(strVal, "GCS_WGS_1984")) {
			geoRef.elpsType = ET_WGS84; return true;
		}
		else if (!strcmp(strVal, "GCS_CGCS_2000")) {
			geoRef.elpsType = ET_CN2000; return true;
		}
		else if (!strcmp(strVal, "GCS_Beijing_1954")) {
			geoRef.elpsType = ET_BEIJING54; return true;
		}
		else if (!strcmp(strVal, "GCS_Xian_1980")) {
			geoRef.elpsType = ET_XIAN80; return true;
		}
		else return false;
	}
	else return false;	
}
inline bool SetProjectionMode(char *strVal, GeoRef &geoRef, char*strDescType)
{
	if (!strcmp(strDescType, "Standard"))
	{
		if (!strcmp(strVal, "Gauss")) {
			geoRef.projType = GAUSS_PROJECTION; return true;
		}
		else if (!strcmp(strVal, "UTM")) {
			geoRef.projType = UTM_PROJECTION; return true;
		}
		else if (!strcmp(strVal, "Albert")) {
			geoRef.projType = LAMBERT_PROJECTION; return true;
		}
		else return false;
	}
	else if (!strcmp(strDescType, "WKT"))
	{
		if (!strcmp(strVal, "Gauss_Kruger")) {
			geoRef.projType = GAUSS_PROJECTION; return true;
		}
		else if (!strcmp(strVal, "Transverse_Mercator")) {
			geoRef.projType = UTM_PROJECTION; return true;
		}
		else if (!strcmp(strVal, "Lambert")) {
			geoRef.projType = LAMBERT_PROJECTION; return true;
		}
		else return false;
	}
	else return false;
}
/*
int SetImageLevl(char *imgLvl, IMG_LVL lvl)
{
	return 1;
}*/
class CMMIPXml : public CPrjXml
{
	DECLARE_DYNFILELOAD(CMMIPXml)
public:
	static	bool WriteStatusXml(LPCSTR lpstrJobXmlPath, LPCSTR lpstrRetXmlPath = NULL);
	static	bool GetStatusPath(LPCSTR lpstrJobXmlPath, LPCSTR lpstrDefPath, char* strStatusPath);
	int		Save(LPCSTR lpstrPath);
	int		AnalyzeMipPar(LPCSTR lpstrPath = NULL);

	int AnalysisImport();
	int	AnalysisHead();
	int AnalysisExtraPar(LPCSTR lpstrGcd);
	int GenGeographicInfo(bool IsProdGeoSys);//ֻҪ�ҵ�<GeographicReference>�ֶ���XML��main positionλ��<GeographicReference>�ֶμ���
	//true:���ɲ�Ʒ����ϵ//false:������������ϵ

protected:
	static bool CompleteImageInfo(ImageInfo* pInfo);
	int AnalysisExport();

	static bool GenProdImageInfo(LPIMAGEINF pProd, const ImageInfo* pImg, IMG_LVL lvl, LPCSTR strOutPath);
public:
	char	m_strStatusPath[MAX_PATHNAME];
	char	m_strCompletePath[MAX_PATHNAME];
	char	m_strWorkDir[MAX_PATHNAME];
	char		m_taskItemID[100];//������TaskItemID��ֵ
	char m_strPathSetting[MAX_PATHNAME];//�洢FlowControl.ini�е�PathSetting
	bool m_IsMap;//true:�ַ�//false:���ַ�
	//char m_strAdjReportPN[MAX_PATHNAME];
};


#endif // MMipXml_h__MLW_2015_3_9