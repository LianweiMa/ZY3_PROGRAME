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
//parameter//25������
//[in]MapNumber//ͼ��////if NULL,set "unknown"//1
//[in]ImgSize//��������С//��λ MB������1λС��//if NULL,set "unknown"//2
//[in]GridSize:�����ߴ�//if NULL,set "unknown"//3
//[in]GridRows��������//if NULL,set "unknown"//4
//[in]GridCols��������//if NULL,set "unknown"//5
//[in]CornerLongRangeͼ���ǵ㾭�ȷ�Χ//�Զȷ����ʽ��д//���磺1201500-1203000//if NULL,set "unknown"//6
//[in]CornerLatRangeͼ���ǵ�γ�ȷ�Χ//�Զȷ����ʽ��д//if NULL,set "unknown"//7
//[in]SouthWestOrd//SouthWestAbs//����ͼ���ǵ�XY����//if NULL,set "unknown"//8//9
//[in]NorthWestOrd//NorthWestAbs//NorthEastOrd//NorthEastAbs//SouthEastOrd//SouthEastAbs//ͬ��//if NULL,set "unknown"//10//11//12//13//14//15
//[in]LongerRadius//���򳤰뾶//if NULL,set "unknown"//16
//[in]OblatusRatio//�������//if NULL,set "unknown"//17
//[in]GeodeticDatum//�����ô�ػ�׼//if NULL,set "unknown"//18
//[in]MapProjection//��ͼͶӰ����//if NULL,set "unknown"//19
//[in]CentralMederian//����������//if NULL,set "unknown"//20
//[in]ZoneDivisionMode//�ִ���ʽ//if NULL,set "unknown"//21
//[in]GaussKrugerZoneNo//��˹-������ͶӰ����//if NULL,set "unknown"//22
//[in]HeightSystem//�߳�ϵͳ��//if NULL,set "unknown"//23
//[in]HeightDatum//�̻߳�׼//if NULL,set "unknown"//24
//[out]str_1_50000DsmXmlFileName:���·��//if NULL,set "unknown"//25
//////////////////////////////////////////////////////////////////////////
bool
	write_1_50000DSMXML(char*MapNumber,char*ImgSize,char*GridSize,char*GridRows,char*GridCols,
	char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,
	char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,
	char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,char*MapProjection,char*CentralMederian,
	char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,char*HeightDatum,char*str_1_50000DsmXmlFileName);
#endif