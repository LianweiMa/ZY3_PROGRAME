#ifndef _WHU_WRITE_DEM_XML_MLW
#define _WHU_WRITE_DEM_XML_MLW
#include "Markup.h"
#include <time.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////////
//parameter(2)
//[in]strDsmXmlFileName: �����dsm��Ԫ�����ļ�,����·��
//[out]strDemXmlFileName:�����dem��Ԫ�����ļ�,����·��
//////////////////////////////////////////////////////////////////////////
bool
	writeDEMXml(const char*strDsmXmlFileName,char*strDemXmlFileName);
#endif