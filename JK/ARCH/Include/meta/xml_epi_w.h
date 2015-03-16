#ifndef _WHU_WRITE_EPI_XML_MLW
#define _WHU_WRITE_EPI_XML_MLW
#include "Markup.h"
#include <time.h>
//////////////////////////////////////////////////////////////////////////
bool
writeEPIXml(const char* lpstrFImgPath,const char* lpstrNImgPath,const char* lpstrBImgPath,
			const char* lpstrFEpiPath,const char* lpstrNEpiPath,const char* lpstrBEpiPath,
			const char* lpstrXmlPath);
//parameter(24)
//[in]strImgFName:前视影像路径//1
//[in]strImgNName:下视影像路径//2
//[in]strImgBName:后视影像路径//3
//[in]CornerLongRange:图廓角点经度范围,以度分秒格式填写////if NULL,set "unknown"//7
//[in]CornerLatRange:图廓角点纬度范围,以度分秒格式填写////if NULL,set "unknown"//8
//[in]SouthWestOrd:西南角点X坐标,按标准分幅图廓角点坐标填写（不含外扩区）,保留两位小数//if NULL,set "unknown"//9
//[in]SouthWestAbs:西南角点Y坐标,按标准分幅图廓角点坐标填写（不含外扩区）,不加带号,保留两位小数//if NULL,set "unknown"//10
//[in]NorthWestOrd,NorthWestAbs,NorthEastOrd,NorthEastAbs,SouthEastOrd,SouthEastAbs同上//11-16
//[in]fn_meanParallax,nb_meanParallax,fb_meanParallax视差//if NULL,set "unknown"//4-6
//[in]EarthEllipsoid:椭球模型//if NULL,set "unknown"//17
//[in]GeodeticDatum:大地基准//if NULL,set "unknown"//18
//[in]AltitudeDatum:高程基准//if NULL,set "unknown"//19
//[in]AltitudeSystem:高程系统//if NULL,set "unknown"//20
//[in]MapProjection:投影//if NULL,set "unknown"//21
//[in]ZoneDivisionMode:分带方式//if NULL,set "unknown"//22
//[in]ZoneNo:投影带号//if NULL,set "unknown"//23
//[in]HeightMode:高程模型//if NULL,set "unknown"//24
//[in]ResamplingKernel:重采样方法//if NULL,set "unknown"//25
//[out]strEpiXmlFileName:输出的...epi.xml文件路径//26
bool
	writeEPIXml(const char*strImgFName,const char* strImgNName,const char* strImgBName ,char*CornerLongRange,char*CornerLatRange,
	char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,char*NorthWestAbs,char*NorthEastOrd,
	char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,char * fn_meanParallax,char * nb_meanParallax,
	char * fb_meanParallax,char *EarthEllipsoid,char *GeodeticDatum,char *AltitudeDatum,char*AltitudeSystem,
	char*MapProjection,char*ZoneDivisionMode,char*ZoneNo,char*HeightMode,char *ResamplingKernel,
	const char*strEpiXmlFileName);
#endif