////MipXml.h
/********************************************************************
	MipXml
	created:	2013/09/26
	author:		LX 
	purpose:	This file is for MipXml function
*********************************************************************/
#if !defined MipXml_h__LX_2013_9_26
#define MipXml_h__LX_2013_9_26

#include "LxXML.h"

#define MIP_XML_ONLY	"<MJMJobType>"

class CMIPXml : public CPrjXml
{
	DECLARE_DYNFILELOAD(CMIPXml)
public:
	static	bool WriteStatusXml(LPCSTR lpstrJobXmlPath,LPCSTR lpstrRetXmlPath=NULL);
	static	bool GetStatusPath(LPCSTR lpstrJobXmlPath,LPCSTR lpstrDefPath,char* strStatusPath);
	int		Save(LPCSTR lpstrPath);
	int		AnalyzeMipPar(LPCSTR lpstrPath=NULL);

	int AnalysisImport();
	int	AnalysisHead();
	int AnalysisExtraPar(LPCSTR lpstrGcd);

// 	static bool GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj = true);	// retPath:  sc is RPC,else is tfw; if file isn't exist ,return false,else return true   
// 	static bool GetThumbPath(LPCSTR strImgPath,char* retPath);
// 	static bool GetBrowsePath(LPCSTR strImgPath,char* retPath);
// 	static bool GetMetaPath(LPCSTR strImgPath,char* retPath);
// 	static bool RenameImage(char* strPath,IMG_LVL lvl);
protected:
	static bool CompleteImageInfo(ImageInfo* pInfo);
	int AnalysisExport();

	static bool GenProdImageInfo(LPIMAGEINF pProd,const ImageInfo* pImg,IMG_LVL lvl,LPCSTR strOutPath);
public:
	char	m_strStatusPath[MAX_PATHNAME];
	char	m_strCompletePath[MAX_PATHNAME];
	char	m_strWorkDir[MAX_PATHNAME];
	ProjectionInfo m_projectionInfo;
};


#endif // MipXml_h__LX_2013_9_26