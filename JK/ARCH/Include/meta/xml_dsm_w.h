#ifndef _WHU_WRITE_DSM_XML_MLW
#define _WHU_WRITE_DSM_XML_MLW
#include "Markup.h"
#include <time.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////////
bool
writeDSMXml(const char* lpstrNameImgF,const char* lpstrNameImgN,const char* lpstrNameImgB,
			const char* lpstrDemPath,const char* lpstrGcdPath,const char* lpstrXmlPath);
//parameter(28)
///[in]strImgFName:前视影像路径//1
//[in]strImgNName:下视影像路径//2
//[in]strImgBName:后视影像路径//3
//[in]GridSize:格网尺寸//if NULL,set "unknown"//4
//[in]GridRows:网格行数//if NULL,set "unknown"//5
//[in]GridCols:网格列数//if NULL,set "unknown"//6
//[in]ImgSize:影像数据量大小,单位 MB,保留整数//if NULL,set "unknown"//7
//[in]CornerLongRange:图廓角点经度范围,以度分秒格式填写////if NULL,set "unknown"//8
//[in]CornerLatRange:图廓角点纬度范围,以度分秒格式填写////if NULL,set "unknown"//9
//[in]SouthWestOrd:西南角点X坐标,按标准分幅图廓角点坐标填写（不含外扩区）,保留两位小数//if NULL,set "unknown"//10
//[in]SouthWestAbs:西南角点Y坐标,按标准分幅图廓角点坐标填写（不含外扩区）,不加带号,保留两位小数//if NULL,set "unknown"//11
//[in]NorthWestOrd,NorthWestAbs,NorthEastOrd,NorthEastAbs,SouthEastOrd,SouthEastAbs同上//17
//[in]LongerRadius:椭球长半径////if NULL,set "unknown"//18
//[in]OblatusRatio:椭球扁率//if NULL,set "unknown"//19
//[in]GeodeticDatum:所采用大地基准//if NULL,set "unknown"//20
//[in]MapProjection:地图投影//if NULL,set "unknown"//21
//[in]CentralMederian:中央子午线//if NULL,set "unknown"//22
//[in]ZoneDivisionMode:分带方式//if NULL,set "unknown"//23
//[in]GaussKrugerZoneNo:高斯-克吕格投影带号//if NULL,set "unknown"//24
//[in]HeightSystem:高程系统名//if NULL,set "unknown"//25
//[in]HeightDatum:高程基准//if NULL,set "unknown"//26
//[in]Interpolation:高程内插方式//if NULL,set "unknown"//27
//[out]strDsmXmlFileName:输出的dsm的元数据文件,包含路经//28
//////////////////////////////////////////////////////////////////////////
bool
	writeDSMXml(const char*strImgFName,const char* strImgNName,const char* strImgBName ,char*GridSize,char*GridRows,
	char*GridCols,char*ImgSize,char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,
	char*SouthWestAbs,char*NorthWestOrd,char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,
	char*SouthEastOrd,char*SouthEastAbs,char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,
	char*MapProjection,char*CentralMederian,char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,
	char*HeightDatum,char*Interpolation,const char*strDsmXmlFileName);
#endif