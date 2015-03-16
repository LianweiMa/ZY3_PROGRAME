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
///[in]strImgFName:ǰ��Ӱ��·��//1
//[in]strImgNName:����Ӱ��·��//2
//[in]strImgBName:����Ӱ��·��//3
//[in]GridSize:�����ߴ�//if NULL,set "unknown"//4
//[in]GridRows:��������//if NULL,set "unknown"//5
//[in]GridCols:��������//if NULL,set "unknown"//6
//[in]ImgSize:Ӱ����������С,��λ MB,��������//if NULL,set "unknown"//7
//[in]CornerLongRange:ͼ���ǵ㾭�ȷ�Χ,�Զȷ����ʽ��д////if NULL,set "unknown"//8
//[in]CornerLatRange:ͼ���ǵ�γ�ȷ�Χ,�Զȷ����ʽ��д////if NULL,set "unknown"//9
//[in]SouthWestOrd:���Ͻǵ�X����,����׼�ַ�ͼ���ǵ�������д��������������,������λС��//if NULL,set "unknown"//10
//[in]SouthWestAbs:���Ͻǵ�Y����,����׼�ַ�ͼ���ǵ�������д��������������,���Ӵ���,������λС��//if NULL,set "unknown"//11
//[in]NorthWestOrd,NorthWestAbs,NorthEastOrd,NorthEastAbs,SouthEastOrd,SouthEastAbsͬ��//17
//[in]LongerRadius:���򳤰뾶////if NULL,set "unknown"//18
//[in]OblatusRatio:�������//if NULL,set "unknown"//19
//[in]GeodeticDatum:�����ô�ػ�׼//if NULL,set "unknown"//20
//[in]MapProjection:��ͼͶӰ//if NULL,set "unknown"//21
//[in]CentralMederian:����������//if NULL,set "unknown"//22
//[in]ZoneDivisionMode:�ִ���ʽ//if NULL,set "unknown"//23
//[in]GaussKrugerZoneNo:��˹-������ͶӰ����//if NULL,set "unknown"//24
//[in]HeightSystem:�߳�ϵͳ��//if NULL,set "unknown"//25
//[in]HeightDatum:�̻߳�׼//if NULL,set "unknown"//26
//[in]Interpolation:�߳��ڲ巽ʽ//if NULL,set "unknown"//27
//[out]strDsmXmlFileName:�����dsm��Ԫ�����ļ�,����·��//28
//////////////////////////////////////////////////////////////////////////
bool
	writeDSMXml(const char*strImgFName,const char* strImgNName,const char* strImgBName ,char*GridSize,char*GridRows,
	char*GridCols,char*ImgSize,char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,
	char*SouthWestAbs,char*NorthWestOrd,char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,
	char*SouthEastOrd,char*SouthEastAbs,char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,
	char*MapProjection,char*CentralMederian,char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,
	char*HeightDatum,char*Interpolation,const char*strDsmXmlFileName);
#endif