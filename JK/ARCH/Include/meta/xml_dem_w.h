#ifndef _WHU_WRITE_DEM_XML_MLW
#define _WHU_WRITE_DEM_XML_MLW
#include "Markup.h"
#include <time.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////////
//parameter(2)
//[in]strDsmXmlFileName: 输入的dsm的元数据文件,包含路经
//[out]strDemXmlFileName:输出的dem的元数据文件,包含路经
//////////////////////////////////////////////////////////////////////////
bool
	writeDEMXml(const char*strDsmXmlFileName,char*strDemXmlFileName);
#endif