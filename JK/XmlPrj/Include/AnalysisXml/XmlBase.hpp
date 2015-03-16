//XmlBase.hpp
/********************************************************************
	XmlBase
	created:	2015/01/24
	author:		LX_whu 
	purpose:	This file is for XmlBase function
*********************************************************************/
#if !defined XmlBase_hpp__LX_whu_2015_1_24
#define XmlBase_hpp__LX_whu_2015_1_24

#include <stdarg.h>
#include "Markup.h"
#include "PrjLog.hpp"

typedef		CMarkup		CXml;

#define INTO_NODE(xml)	xml.IntoElem();
#define OUT_NODE(xml)	xml.OutOfElem();

#define NOT_GET			0x00
#define FORWARD_GET		0x01
#define BACKWARD_GET	0x02

enum PathMode{
	Mode_Absolute,
	Mode_Relative
};

class CXmlBase 
{
public:
	CXmlBase(){}
	virtual ~CXmlBase(){}
	bool	Open(LPCSTR	lpstrPathName){
		Close();
		if (!IsExist(lpstrPathName)) { return false; }
		bool bXml = m_xml.Load(lpstrPathName);

		if (!bXml)	{ printf("%s", GetXmlError()); }

		return bXml;
	}
	void	Close(){
		m_xml.ResetPos();
		m_xml.SetDoc(NULL);
	}
	bool	FindNode(PathMode mode, char* szText, const int fmt, ...){
		if (szText) *szText = 0;//清空字符串
		if (mode == Mode_Absolute) m_xml.ResetPos();//返回到root节点

		int num = fmt, argno = 0;
		char* a;
		va_list ap;
		va_start(ap, fmt);

		a = va_arg(ap, char *);
		if (FindSibingNode(a, NULL, false) == NOT_GET) return false;

		INTO_NODE(m_xml)
			argno++;
		while (a != 0 && argno < num)
		{
			a = va_arg(ap, char *);
			if (!m_xml.FindElem(a)) return false;
			INTO_NODE(m_xml)
				argno++;
		}
		OUT_NODE(m_xml);

		va_end(ap);

		if (szText) { strcpy(szText, m_xml.GetData().c_str()); }

		return true;
	}
	int		FindSibingNode(LPCSTR lpstrName, char* strText = NULL, bool bNextNode = true){
		if (strText) *strText = 0;
		if (!m_xml.GetElemContent().c_str()) return NOT_GET;//调这一步有什么用？？？
		MCD_STR str;

		if (!bNextNode && !strcmp(lpstrName, m_xml.GetTagName().c_str())) {
			if (strText) { str = m_xml.GetData(); strcpy(strText, str.c_str()); }
			return FORWARD_GET;
		}

		if (m_xml.FindElem(lpstrName))
		{
			if (strText) { str = m_xml.GetData(); strcpy(strText, str.c_str()); }
			return FORWARD_GET;
		}
		else
		{
			m_xml.ResetMainPos();//
			if (m_xml.FindElem(lpstrName)) {
				if (strText) { str = m_xml.GetData(); strcpy(strText, str.c_str()); }
				return BACKWARD_GET;
			}
			else{
				return NOT_GET;
			}
		}
		return NOT_GET;
	}
	int		FindChildNode(LPCSTR lpstrName, char* strText = NULL, bool bNextNode = true){
		if (strText) *strText = 0;
		if (!m_xml.GetElemContent().c_str()) return NOT_GET;

		MCD_STR str;
		if (!bNextNode && !strcmp(lpstrName, m_xml.GetChildTagName().c_str())) {
			if (strText) { str = m_xml.GetChildData(); strcpy(strText, str.c_str()); }
			return FORWARD_GET;
		}


		if (m_xml.FindChildElem(lpstrName))
		{
			if (strText) { str = m_xml.GetChildData(); strcpy(strText, str.c_str()); }
			return FORWARD_GET;
		}
		else
		{
			m_xml.ResetChildPos();
			if (m_xml.FindChildElem(lpstrName)) {
				if (strText) { str = m_xml.GetChildData(); strcpy(strText, str.c_str()); }
				return BACKWARD_GET;
			}
			else{
				return NOT_GET;
			}
		}
		return NOT_GET;
	}
	int		FindSiblingAttrib(LPCSTR lpstrName, LPCSTR lpstrAttrib, char* strText, bool bNextNode = true){
		int ret = FindSibingNode(lpstrName, NULL, bNextNode);
		if (ret == NOT_GET) return NOT_GET;

		GetAttrib(lpstrAttrib, strText);

		return ret;
	}
	void	IntoNode() { INTO_NODE(m_xml) };
	void	OutNode() { OUT_NODE(m_xml) };
	void	GetData(char* strText)  { strcpy(strText, m_xml.GetData().c_str()); }
	const char*	GetXmlError() const { return m_xml.GetError().c_str(); }

	bool	RemoveNode(LPCSTR lpstrName = NULL) { if (!lpstrName) return m_xml.RemoveElem();	if (FindSibingNode(lpstrName) == NOT_GET)	return true; return m_xml.RemoveElem(); }
	bool	RemoveChildNode(LPCSTR lpstrName = NULL) { if (!lpstrName) return m_xml.RemoveChildElem();	if (FindChildNode(lpstrName) == NOT_GET)	return true; return m_xml.RemoveChildElem(); }
	bool	CompleteNode(LPCSTR lpstrName, LPCSTR lpstrContent = NULL, bool bInsert = true) {
		if (FindSibingNode(lpstrName) == NOT_GET) {
			if (bInsert)	return InsertNode(lpstrName, lpstrContent);
			else AddNode(lpstrName, lpstrContent);
		}
		if (lpstrContent) return SetData(lpstrContent);	 return true;
	}
	bool	CompleteNode(LPCSTR lpstrName, int nVal) {
		char strVal[256];	sprintf(strVal, "%d", nVal);
		return CompleteNode(lpstrName, strVal);
	}
	bool	CompleteNode(LPCSTR lpstrName, double fVal) {
		char strVal[256];	sprintf(strVal, "%lf", fVal);
		return CompleteNode(lpstrName, strVal);
	}

	bool	AddSubDoc(LPCSTR lpstrName) { return m_xml.AddSubDoc(lpstrName); }
	bool	AddNode(LPCSTR lpstrName) { return m_xml.AddElem(lpstrName); }
	bool	AddNode(LPCSTR lpstrName, const char *fmt, ...){
		char strMsg[2048]; va_list ap; va_start(ap, fmt); vsprintf(strMsg, fmt, ap); va_end(ap);
		return m_xml.AddElem(lpstrName, strMsg);
	}
	bool	AddChildNode(LPCSTR lpstrName, LPCSTR lpstrContent = NULL) { return m_xml.AddChildElem(lpstrName, lpstrContent); };
	bool	AddNode(LPCSTR lpstrName, int nVal) { return m_xml.AddElem(lpstrName, nVal); }
	bool	AddChildNode(LPCSTR lpstrName, int nVal) { return m_xml.AddChildElem(lpstrName, nVal); };
	//	bool	InsertNode(LPCSTR lpstrName,LPCSTR lpstrContent=NULL ) { return m_xml.InsertElem(lpstrName,lpstrContent);	} 
	bool	InsertNode(LPCSTR lpstrName) { return m_xml.InsertElem(lpstrName); }
	bool	InsertChildNode(LPCSTR lpstrName, LPCSTR lpstrContent = NULL) { return m_xml.InsertChildElem(lpstrName, lpstrContent); };
	bool	InsertNode(LPCSTR lpstrName, int nVal) { return m_xml.InsertElem(lpstrName, nVal); }
	bool	InsertNode(LPCSTR lpstrName, const char *fmt, ...){
		char strMsg[2048]; va_list ap; va_start(ap, fmt); vsprintf(strMsg, fmt, ap); va_end(ap);
		return m_xml.InsertElem(lpstrName, strMsg);
	}
	bool	InsertChildNode(LPCSTR lpstrName, int nVal) { return m_xml.InsertChildElem(lpstrName, nVal); };

	bool	SetDoc(LPCSTR lpstrName) { return m_xml.SetDoc(lpstrName); }
	const char*	GetNodeContent() const	{
		static MCD_STR str;
		str = m_xml.GetElemContent();
		return str.c_str();
	};
	bool	Save(LPCSTR lpstrPathName) { return m_xml.Save(lpstrPathName); };
	void	GetAttrib(LPCSTR lpstrName, char* strText) { if (strText) strcpy(strText, m_xml.GetAttrib(lpstrName).c_str()); }
	bool	SetAttrib(LPCSTR lpstrName, char* strText) { return m_xml.SetAttrib(lpstrName, strText); }
	bool	SetChildData(LPCSTR lpstrName) { return m_xml.SetChildData(lpstrName); }
	bool	SetData(LPCSTR lpstrName) { return m_xml.SetData(lpstrName); }
	bool	SetData(int nVal) { return m_xml.SetData(nVal); }

protected:
	CXml	m_xml;

};

#endif // XmlBase_hpp__LX_whu_2015_1_24