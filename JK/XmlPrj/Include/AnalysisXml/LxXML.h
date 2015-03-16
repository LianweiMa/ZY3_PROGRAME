////LxXML.h
/********************************************************************
	LxXML
	created:	2013/06/27
	author:		LX 
	purpose:	This file is for LxXML function
*********************************************************************/
#if !defined LxXML_h__LX_2013_6_27
#define LxXML_h__LX_2013_6_27

#include "XmlBase.hpp"
#include "PrjDef.h"

#define MIN_DEMGSD		15
#define MIN_DOMGSD		0.5 	

class CXmlFile	:	public	CArchFile,public CXmlBase
{
	DECLARE_DYNARCH(CXmlFile)
public:
	void operator=(const CXmlFile& object) { m_xml = object.m_xml;	*((CArchFile*)this) = *((CArchFile*)&object); }
};

#define MAX_LINK_PATH	256

class CPrjXml	:	public	CXmlFile
{
	DECLARE_DYNARCH(CPrjXml);
public:
	CPrjXml();
	virtual	~CPrjXml();
	void	Reset();
	bool	Open(LPCSTR lpstrPathName);
	virtual	int		Save(LPCSTR lpstrPath);
	LPCSTR GetVersion() const { return m_hdr.vesion;	}
	const  char*	GetGcdPath()	const { return m_prodInfo.strGcdPath;	}

	void	UpdataTskList() { m_tskTypeList = m_tskTypeCur;	}
	void	SetTskList(int tskType);	// TSK_END represent all
	void	SetCurTsk(TASK tskType)	{ m_tskTypeCur = tskType;	}
	void	AddTsk2List(TASK tskType) { if(tskType<TSK_ATMCH || tskType>=TSK_END) return;	if(!(m_tskTypeList&tskType)) m_tskTypeList = m_tskTypeList|tskType; }
	void	RmTsk4List(TASK tskType)	{if(tskType<TSK_ATMCH || tskType>=TSK_END) return;	if( (m_tskTypeList&tskType) ) m_tskTypeList = m_tskTypeList|tskType; }

	int CreateSymbolicLink4Import(int lengLimit = MAX_LINK_PATH );
	int ReleaseSymbolicLink4Import();

	void operator=( const CPrjXml& object ) {
		memcpy(&m_hdr,&(object.m_hdr),sizeof(PrjHdr));		memcpy(&(m_prodInfo),&(object.m_prodInfo),sizeof(ProdInfo));

		int num;	ImageInfo* pInfo; ImgExInfo* pInfoEx;	SceneInfo* psc;
		pInfo = ((class CArray_ImageInfo*)&(object.m_import) )->GetData(num);
		m_import.Reset(pInfo,num);	
		pInfoEx= ((class CArray_ImgExInfo*)&(object.m_importEx) )->GetData(num);
		m_importEx.Reset(pInfoEx,num);	

		pInfo = ((class CArray_ImageInfo*)&(object.m_export) )->GetData(num);
//		pInfo = object.m_export.GetData(num);
		m_export.Reset(pInfo,num);
		pInfoEx= ((class CArray_ImgExInfo*)&(object.m_exportEx) )->GetData(num);
		m_exportEx.Reset(pInfoEx,num);

		psc = ((class CArray_SceneInfo*)&(object.m_scList) )->GetData(num);
//		psc = object.m_scList.GetData(num);
		m_scList.Reset(psc,num);

		m_tskTypeCur = object.m_tskTypeCur;	m_tskTypeList = object.m_tskTypeList;	m_nBlockID = object.m_nBlockID;

		*((CXmlFile*)this)= *((CXmlFile*)&object);	
	}
public:
	virtual int	AnalysisHead()	{ ErrorPrint("No Code to Analysis Xml Head!");	return ERR_UNKNOW; };
	virtual int AnalysisImport() { ErrorPrint("No Code to Analysis Xml Import!");	return ERR_UNKNOW; };
	virtual int AnalysisExport() { ErrorPrint("No Code to Analysis Xml Export!");	return ERR_UNKNOW; };
	virtual int AnalysisExtraPar() { ErrorPrint("No Code to Analysis Xml Extra parameter!");	return ERR_UNKNOW; };

public:
	int			m_tskTypeCur;
	int			m_tskTypeList;
	PrjHdr		m_hdr;
	CArray_ImageInfo	m_import;
	CArray_ImgExInfo	m_importEx;
	CArray_ImageInfo	m_export;
	CArray_ImgExInfo	m_exportEx;
	ProdInfo	m_prodInfo;
	CArray_SceneInfo	m_scList;
	int			m_nBlockID;		

	char		m_strProductSpace[512];
	char		m_strProcessSpace[512];
	MessagePort	m_msgPort;
};



#endif // LxXML_h__LX_2013_6_27