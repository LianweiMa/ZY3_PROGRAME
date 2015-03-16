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
//parameter//25������
//[in]MapNumber//ͼ��////if NULL,set "unknown"
//[in]ImgSize//��������С//��λ MB������1λС��//if NULL,set "unknown"
//[in]GridSize:�����ߴ�//if NULL,set "unknown"
//[in]GridRows��������//if NULL,set "unknown"
//[in]GridCols��������//if NULL,set "unknown"
//[in]Interpolation//if NULL,set "unknown"
//[in]CornerLongRangeͼ���ǵ㾭�ȷ�Χ//�Զȷ����ʽ��д//���磺1201500-1203000//if NULL,set "unknown"
//[in]CornerLatRangeͼ���ǵ�γ�ȷ�Χ//�Զȷ����ʽ��д//if NULL,set "unknown"
//[in]SouthWestOrd//SouthWestAbs//����ͼ���ǵ�XY����//if NULL,set "unknown"
//[in]NorthWestOrd//NorthWestAbs//NorthEastOrd//NorthEastAbs//SouthEastOrd//SouthEastAbs//ͬ��//if NULL,set "unknown"
//[in]LongerRadius//���򳤰뾶//if NULL,set "unknown"
//[in]OblatusRatio//�������//if NULL,set "unknown"
//[in]GeodeticDatum//�����ô�ػ�׼//if NULL,set "unknown"
//[in]MapProjection//��ͼͶӰ����//if NULL,set "unknown"
//[in]CentralMederian//����������//if NULL,set "unknown"
//[in]ZoneDivisionMode//�ִ���ʽ//if NULL,set "unknown"
//[in]GaussKrugerZoneNo//��˹-������ͶӰ����//if NULL,set "unknown"
//[in]HeightSystem//�߳�ϵͳ��//if NULL,set "unknown"
//[in]HeightDatum//�̻߳�׼//if NULL,set "unknown"
//[in]DataSourceType//��Ҫ����Դ����//if NULL,set "unknown"
//[out]str_1_50000DemXmlFileName:���·��//if NULL,set "unknown"
//////////////////////////////////////////////////////////////////////////
bool
	write_1_50000DEMXML(char*MapNumber,char*ImgSize,char*GridSize,char*GridRows,char*GridCols,
	char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,
	char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,
	char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,char*MapProjection,char*CentralMederian,
	char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,char*HeightDatum,char*str_1_50000DemXmlFileName);
#endif