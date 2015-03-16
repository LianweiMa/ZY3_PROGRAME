#ifndef _WHU_WRITE_1_50000_DEM_XML_MLW
#define _WHU_WRITE_1_50000_DEM_XML_MLW
#include <stdio.h>
#include<string.h>
#include "Markup.h"
#include <time.h>
#include "typedef.h"
#include "SpGeoCvt.h"

bool write_1_50000DEMXML(const char* lpstrFilePath,char*MapNumber,char*GridSize,int nGridRows,int nGridCols,
						 double xmin,double ymin,double xmax,double ymax,CSpGeoCvt& geoCvt,
						 char*str_1_50000DsmXmlFileName);
//////////////////////////////////////////////////////////////////////////
//parameter//25个参数
//[in]MapNumber//图号////if NULL,set "unknown"
//[in]ImgSize//数据量大小//单位 MB，保留1位小数//if NULL,set "unknown"
//[in]GridSize:格网尺寸//if NULL,set "unknown"
//[in]GridRows格网行数//if NULL,set "unknown"
//[in]GridCols格网列数//if NULL,set "unknown"
//[in]Interpolation//if NULL,set "unknown"
//[in]CornerLongRange图廓角点经度范围//以度分秒格式填写//例如：1201500-1203000//if NULL,set "unknown"
//[in]CornerLatRange图廓角点纬度范围//以度分秒格式填写//if NULL,set "unknown"
//[in]SouthWestOrd//SouthWestAbs//西南图廓角点XY坐标//if NULL,set "unknown"
//[in]NorthWestOrd//NorthWestAbs//NorthEastOrd//NorthEastAbs//SouthEastOrd//SouthEastAbs//同上//if NULL,set "unknown"
//[in]LongerRadius//椭球长半径//if NULL,set "unknown"
//[in]OblatusRatio//椭球扁率//if NULL,set "unknown"
//[in]GeodeticDatum//所采用大地基准//if NULL,set "unknown"
//[in]MapProjection//地图投影名称//if NULL,set "unknown"
//[in]CentralMederian//中央子午线//if NULL,set "unknown"
//[in]ZoneDivisionMode//分带方式//if NULL,set "unknown"
//[in]GaussKrugerZoneNo//高斯-克吕格投影带号//if NULL,set "unknown"
//[in]HeightSystem//高程系统名//if NULL,set "unknown"
//[in]HeightDatum//高程基准//if NULL,set "unknown"
//[in]DataSourceType//主要数据源类型//if NULL,set "unknown"
//[out]str_1_50000DemXmlFileName:输出路径//if NULL,set "unknown"
//////////////////////////////////////////////////////////////////////////
bool
	write_1_50000DEMXML(char*MapNumber,char*ImgSize,char*GridSize,char*GridRows,char*GridCols,
	char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,
	char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,
	char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,char*MapProjection,char*CentralMederian,
	char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,char*HeightDatum,char*str_1_50000DemXmlFileName);
#endif