//ZY3FileName.h
/********************************************************************
	ZY3FileName
	created:	2013/10/09
	author:		LX 
	purpose:	This file is for ZY3FileName function
*********************************************************************/
#if !defined ZY3FileName_h__LX_2013_10_9
#define ZY3FileName_h__LX_2013_10_9

#include "PrjDef.h"


class CZY3File : public CArchFile
{
	DECLARE_DYNARCH(CZY3File)
public:
	CZY3File();
	virtual ~CZY3File();
};

#ifndef _SCPT
#define _SCPT
//!二维点数据定义（float型）
typedef struct tagSCPT
{
	float lx,ly,rx,ry;
}SCPT;
#endif

enum GEOFILE_TYPE{
	GT_UNKNOW,
	GT_ORI,
	GT_RPC,
	GT_TFW,
	GT_GEO,
	GT_END,
};

short CalcRgn(SCPT* pt,LPCSTR lpstrFilePath,LPCSTR lpstrGeoPath,GEOFILE_TYPE type);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
class CZY3SceneFile : public CZY3File
{
	DECLARE_DYNARCH(CZY3SceneFile)
public:
	CZY3SceneFile() ;
	virtual ~CZY3SceneFile();
	virtual int		TinyLoad(LPCSTR	lpstrPath) { return ERR_UNKNOW;	};
	virtual int		InitExInfo(LPCSTR	lpstrPath,IMG_LVL lvl) { return ERR_UNKNOW;	};
	virtual	bool	RenameImage(char* strPath,IMG_LVL lvl) { return false;	}
	virtual bool	GenerateMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath) { 
		if(IsExist(lpstrOrgMeta)) { CopyFile(lpstrOrgMeta,lpMetaPath,FALSE); return true; }
		return false;	
	};

	bool GenerateMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta){ char strMeta[512];	GetMetaPath(strMeta);	return GenerateMetaFile(lpOrderXml,lpstrOrgMeta,strMeta);	}

	virtual int load(const ImageInfo* pInfo);

	const ImageInfo* GetImgInfo()	const { return &m_imgInfo;	}
	short	 CalcSceneRgn(SCPT* pt)
	{
		char strFile[MAX_PATHNAME],strGeoFile[MAX_PATHNAME];
		strFile[0] = strGeoFile[0] = 0;
		GEOFILE_TYPE type = GT_UNKNOW;
		switch( m_imgInfo.lvl ) 
		{
		case IL_SC:
		case IL_EPI:
			{
				strcpy(strFile,m_imgInfo.path);
				GetOriFilePath(strGeoFile,false);
				type = GT_RPC;
				break;
			}
		case IL_DSM:
		case IL_DEM:
		default:
			strcpy(strFile,m_imgInfo.path);
			strcpy(strGeoFile,strFile);	strcpy(strrchr(strGeoFile,'.'),".tfw");
			type = GT_TFW;
			break;
		}
		return ::CalcRgn(pt,strFile,strGeoFile,type);
	}
	bool GetOriFilePath(char* retPath,bool bAdj = true)
	{
		char strPath[MAX_PATHNAME];	
		if(bAdj)	sprintf(strPath,"%s%s",m_imgExInfo.strFileDir,m_imgExInfo.strAdjRpcNam);
		else	sprintf(strPath,"%s%s",m_imgExInfo.strFileDir,m_imgExInfo.strOrgRpcNam);

		strcpy(retPath,strPath);
		if( IsExist(strPath) ) return true;	else return false;
	}
	bool GetThumbPath(char* retPath)
	{
		char strPath[MAX_PATHNAME];	
		sprintf(strPath,"%s%s",m_imgExInfo.strFileDir,m_imgExInfo.strTumbNam);
		
		strcpy(retPath,strPath);
		if( IsExist(strPath) ) return true;	else return false;
	}
	bool GetBrowsePath(char* retPath)
	{
		char strPath[MAX_PATHNAME];	
		sprintf(strPath,"%s%s",m_imgExInfo.strFileDir,m_imgExInfo.strBrowseNam);
		
		strcpy(retPath,strPath);
		if( IsExist(strPath) ) return true;	else return false;
	}
	bool GetMetaPath(char* retPath)
	{
		char strPath[MAX_PATHNAME];	
		sprintf(strPath,"%s%s",m_imgExInfo.strFileDir,m_imgExInfo.strMetaNam);
		
		strcpy(retPath,strPath);
		if( IsExist(strPath) ) return true;	else return false;
	}
protected:
	ImageInfo m_imgInfo;
	ImgExInfo m_imgExInfo;
};

#define SASMACFile_TAG	"SASMAC"
class CSASMACFile : public CZY3SceneFile
{
	DECLARE_DYNFILELOAD(CSASMACFile)
public:
	CSASMACFile();
	virtual ~CSASMACFile();
	virtual int	TinyLoad(LPCSTR lpstrPath) { return TinyLoad(lpstrPath,&m_imgInfo); }
	virtual int	InitExInfo(LPCSTR	lpstrPath,IMG_LVL lvl);

	static  int TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo);
	virtual	bool RenameImage(char* strPath,IMG_LVL lvl);
//	static bool RenameImage(char* strPath,IMG_LVL lvl);
private:
	static bool GetOriFilePath(LPCSTR strPath,IMG_LVL lvl,char* retPath,bool bAdj = true); 
	static bool GetThumbPath(LPCSTR strImgPath,char* retPath);
	static bool GetBrowsePath(LPCSTR strImgPath,char* retPath);
	static bool GetMetaPath(LPCSTR strPath,char* retPath);
};

#define MIPFile_TAG	"zy301a"
class CMIPFile : public CZY3SceneFile
{
	DECLARE_DYNFILELOAD(CMIPFile)
public:
	CMIPFile();
	virtual ~CMIPFile();
	virtual int	TinyLoad(LPCSTR lpstrPath) { return TinyLoad(lpstrPath,&m_imgInfo); }
	virtual int	InitExInfo(LPCSTR	lpstrPath,IMG_LVL lvl);
	
	static  int TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo);
	virtual	bool RenameImage(char* strPath,IMG_LVL lvl);
	virtual bool GenerateMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath);
	//	static bool RenameImage(char* strPath,IMG_LVL lvl);
private:
	bool	GenerateEpiMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath);
	static bool GetOriFilePath(LPCSTR strPath,IMG_LVL lvl,char* retPath,bool bAdj = true); 
	static bool GetThumbPath(LPCSTR strImgPath,char* retPath);
	static bool GetBrowsePath(LPCSTR strImgPath,char* retPath);
	static bool GetMetaPath(LPCSTR strPath,char* retPath);
};

#define PGSFile_TAG1	"ZY3_TLC_"
#define PGSFile_TAG2	"ZY3_MUX_"
#define PGSFile_TAG3	"ZY3_NAD_"

class CPGSFile : public CZY3SceneFile
{
	DECLARE_DYNFILELOAD(CPGSFile)
public:
	CPGSFile();
	virtual	~CPGSFile();
	virtual int	TinyLoad(LPCSTR lpstrPath) { return TinyLoad(lpstrPath,&m_imgInfo); }
	virtual int	InitExInfo(LPCSTR	lpstrPath,IMG_LVL lvl);
	
	static  int TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo);
	virtual	bool RenameImage(char* strPath,IMG_LVL lvl);
	//	static bool RenameImage(char* strPath,IMG_LVL lvl);
private:
	static bool GetOriFilePath(LPCSTR strPath,IMG_LVL lvl,char* retPath,bool bAdj = true); 
	static bool GetThumbPath(LPCSTR strImgPath,char* retPath);
	static bool GetBrowsePath(LPCSTR strImgPath,char* retPath);
	static bool GetMetaPath(LPCSTR strPath,char* retPath);
};

#define PGSGF01File_TAG1	"GF1_PMS"
#define PGSGF01File_TAG2	"GF1_WFV"
class CPGSGF01File : public CZY3SceneFile
{
	DECLARE_DYNFILELOAD(CPGSGF01File)
public:
	CPGSGF01File();
	virtual	~CPGSGF01File();
	virtual int	TinyLoad(LPCSTR lpstrPath) { return TinyLoad(lpstrPath,&m_imgInfo); }
	virtual int	InitExInfo(LPCSTR	lpstrPath,IMG_LVL lvl);
	
	static  int TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo);
	virtual	bool RenameImage(char* strPath,IMG_LVL lvl);
	//	static bool RenameImage(char* strPath,IMG_LVL lvl);
private:
	static bool GetOriFilePath(LPCSTR strPath,IMG_LVL lvl,char* retPath,bool bAdj = true); 
	static bool GetThumbPath(LPCSTR strImgPath,char* retPath);
	static bool GetBrowsePath(LPCSTR strImgPath,char* retPath);
	static bool GetMetaPath(LPCSTR strPath,char* retPath);
};

#define DQDA_TAG	"ZY3A_"
class CDQDAFile : public CZY3SceneFile
{
	DECLARE_DYNFILELOAD(CDQDAFile)
public:
	CDQDAFile();
	virtual	~CDQDAFile();
	virtual int	TinyLoad(LPCSTR lpstrPath) { return TinyLoad(lpstrPath,&m_imgInfo); }
	virtual int	InitExInfo(LPCSTR	lpstrPath,IMG_LVL lvl);
	
	static  int TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo);
	virtual	bool RenameImage(char* strPath,IMG_LVL lvl);
	virtual bool GenerateMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath);
	//	static bool RenameImage(char* strPath,IMG_LVL lvl);
private:
	static bool GetOriFilePath(LPCSTR strPath,IMG_LVL lvl,char* retPath,bool bAdj = true); 
	static bool GetThumbPath(LPCSTR strImgPath,char* retPath);
	static bool GetBrowsePath(LPCSTR strImgPath,char* retPath);
	static bool GetMetaPath(LPCSTR strPath,char* retPath);
};



#define CGF_TAG	"GF0"//1
class CGFFile : public CZY3SceneFile
{
	DECLARE_DYNFILELOAD(CGFFile)
public:
	CGFFile();
	virtual	~CGFFile();
	virtual int	TinyLoad(LPCSTR lpstrPath) { return TinyLoad(lpstrPath,&m_imgInfo); }
	static  int TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo);
	virtual int	InitExInfo(LPCSTR	lpstrPath,IMG_LVL lvl);

	virtual	bool RenameImage(char* strPath,IMG_LVL lvl);
	virtual bool GenerateMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath);
private:
	static bool GetOriFilePath(LPCSTR strPath,IMG_LVL lvl,char* retPath,bool bAdj = true); 
	static bool GetThumbPath(LPCSTR strImgPath,char* retPath);
	static bool GetBrowsePath(LPCSTR strImgPath,char* retPath);
	static bool GetMetaPath(LPCSTR strPath,char* retPath);
};







#define POS_INVALID	-1
#define FILE_ROOT_NAME	"FileNameDef"

class CSatPrjFile : CZY3SceneFile
{
	DECLARE_DYNFILELOAD(CSatPrjFile)
public:
	CSatPrjFile();
	virtual	~CSatPrjFile();
	virtual int	TinyLoad(LPCSTR lpstrPath) { return TinyLoad(lpstrPath,&m_imgInfo); }
	virtual int	InitExInfo(LPCSTR	lpstrPath,IMG_LVL lvl);
	
	static  int TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo);
	virtual	bool RenameImage(char* strPath,IMG_LVL lvl);
	//	static bool RenameImage(char* strPath,IMG_LVL lvl);
private:
	static bool GetOriFilePath(LPCSTR strPath,IMG_LVL lvl,char* retPath,bool bAdj = true); 
	static bool GetThumbPath(LPCSTR strImgPath,char* retPath);
	static bool GetBrowsePath(LPCSTR strImgPath,char* retPath);
	static bool GetMetaPath(LPCSTR strPath,char* retPath);
private:
	static  bool	m_bInit;
	static	char	m_strIniFilePath[MAX_PATHNAME];
	static	int		m_nType;
	static	char	m_strLvlNam[TTID_END][10];
	static	char	m_strCamNam[CT_END][10];
	static	int		m_nLvlPos;
	static	int		m_nLvlLen;
	static	int		m_nCamPos;
	static	int		m_nCamLen;
	static	int		m_nScenePos;
	static	int		m_nSceneLen;
	static	int		m_nOribitPos;
	static	int		m_nOribitLen;

	static	char	m_strOrgRpcEx[15];
	static	bool	m_bOrgRpcExDot;
	static	char	m_strAdjRpcEx[15];
	static	bool	m_bAdjRpcExDot;
	static	char	m_strThumbEx[15];
	static	bool	m_bThumbExDot;
	static	char	m_strBrowseEx[15];
	static	bool	m_bBrowseExDot;
	static	char	m_strMetaEx[15];
	static	bool	m_bMetaExDot;

};
#endif // ZY3FileName_h__LX_2013_10_9