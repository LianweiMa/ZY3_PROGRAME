#ifndef _WHU_WRITE_1_50000_DSM_XML_MLW
#define _WHU_WRITE_1_50000_DSM_XML_MLW
#include <stdio.h>
#include<string.h>
#include "Markup.h"
#include <time.h>

#include "typedef.h"
#include "SpGeoCvt.h"

bool write_1_50000DSMXML(const char* lpstrFilePath,char*MapNumber,char*GridSize,int nGridRows,int nGridCols,
						 double xmin,double ymin,double xmax,double ymax,CSpGeoCvt& geoCvt,
						 char*str_1_50000DsmXmlFileName);
//////////////////////////////////////////////////////////////////////////
//parameter//25个参数
//[in]MapNumber//图号////if NULL,set "unknown"//1
//[in]ImgSize//数据量大小//单位 MB，保留1位小数//if NULL,set "unknown"//2
//[in]GridSize:格网尺寸//if NULL,set "unknown"//3
//[in]GridRows格网行数//if NULL,set "unknown"//4
//[in]GridCols格网列数//if NULL,set "unknown"//5
//[in]CornerLongRange图廓角点经度范围//以度分秒格式填写//例如：1201500-1203000//if NULL,set "unknown"//6
//[in]CornerLatRange图廓角点纬度范围//以度分秒格式填写//if NULL,set "unknown"//7
//[in]SouthWestOrd//SouthWestAbs//西南图廓角点XY坐标//if NULL,set "unknown"//8//9
//[in]NorthWestOrd//NorthWestAbs//NorthEastOrd//NorthEastAbs//SouthEastOrd//SouthEastAbs//同上//if NULL,set "unknown"//10//11//12//13//14//15
//[in]LongerRadius//椭球长半径//if NULL,set "unknown"//16
//[in]OblatusRatio//椭球扁率//if NULL,set "unknown"//17
//[in]GeodeticDatum//所采用大地基准//if NULL,set "unknown"//18
//[in]MapProjection//地图投影名称//if NULL,set "unknown"//19
//[in]CentralMederian//中央子午线//if NULL,set "unknown"//20
//[in]ZoneDivisionMode//分带方式//if NULL,set "unknown"//21
//[in]GaussKrugerZoneNo//高斯-克吕格投影带号//if NULL,set "unknown"//22
//[in]HeightSystem//高程系统名//if NULL,set "unknown"//23
//[in]HeightDatum//高程基准//if NULL,set "unknown"//24
//[out]str_1_50000DsmXmlFileName:输出路径//if NULL,set "unknown"//25
//////////////////////////////////////////////////////////////////////////
bool
	write_1_50000DSMXML(char*MapNumber,char*ImgSize,char*GridSize,char*GridRows,char*GridCols,
	char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,
	char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,
	char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,char*MapProjection,char*CentralMederian,
	char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,char*HeightDatum,char*str_1_50000DsmXmlFileName);
#endif