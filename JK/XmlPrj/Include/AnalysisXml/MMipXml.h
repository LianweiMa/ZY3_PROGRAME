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
#include "GeoInfoTrans.hpp"

#define MMIP_XML_ONLY	"<ProductiveTaskItemList>"

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
	
	//////////////////////////////////////////////////////////////////////////mlw add-2015.03.09
	int GenGeographicInfo(bool IsProdGeoSys);////true:ProdGeoSys.gcd//false:GcpGeoSys.gcd
	/************************************************************************/
protected:
	static bool CompleteImageInfo(ImageInfo* pInfo);
	int AnalysisExport();

	static bool GenProdImageInfo(LPIMAGEINF pProd, const ImageInfo* pImg, IMG_LVL lvl, LPCSTR strOutPath);
public:
	char	m_strStatusPath[MAX_PATHNAME];
	char	m_strCompletePath[MAX_PATHNAME];
	char	m_strWorkDir[MAX_PATHNAME];
	char		m_taskItemID[100];
	char m_strPathSetting[MAX_PATHNAME];//the head of absolute path setting
	bool m_IsMap;//true:Map//false:no map
};


#endif // MMipXml_h__MLW_2015_3_9