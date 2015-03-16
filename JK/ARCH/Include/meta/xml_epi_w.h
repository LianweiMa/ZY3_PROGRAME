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
//[in]strImgFName:ǰ��Ӱ��·��//1
//[in]strImgNName:����Ӱ��·��//2
//[in]strImgBName:����Ӱ��·��//3
//[in]CornerLongRange:ͼ���ǵ㾭�ȷ�Χ,�Զȷ����ʽ��д////if NULL,set "unknown"//7
//[in]CornerLatRange:ͼ���ǵ�γ�ȷ�Χ,�Զȷ����ʽ��д////if NULL,set "unknown"//8
//[in]SouthWestOrd:���Ͻǵ�X����,����׼�ַ�ͼ���ǵ�������д��������������,������λС��//if NULL,set "unknown"//9
//[in]SouthWestAbs:���Ͻǵ�Y����,����׼�ַ�ͼ���ǵ�������д��������������,���Ӵ���,������λС��//if NULL,set "unknown"//10
//[in]NorthWestOrd,NorthWestAbs,NorthEastOrd,NorthEastAbs,SouthEastOrd,SouthEastAbsͬ��//11-16
//[in]fn_meanParallax,nb_meanParallax,fb_meanParallax�Ӳ�//if NULL,set "unknown"//4-6
//[in]EarthEllipsoid:����ģ��//if NULL,set "unknown"//17
//[in]GeodeticDatum:��ػ�׼//if NULL,set "unknown"//18
//[in]AltitudeDatum:�̻߳�׼//if NULL,set "unknown"//19
//[in]AltitudeSystem:�߳�ϵͳ//if NULL,set "unknown"//20
//[in]MapProjection:ͶӰ//if NULL,set "unknown"//21
//[in]ZoneDivisionMode:�ִ���ʽ//if NULL,set "unknown"//22
//[in]ZoneNo:ͶӰ����//if NULL,set "unknown"//23
//[in]HeightMode:�߳�ģ��//if NULL,set "unknown"//24
//[in]ResamplingKernel:�ز�������//if NULL,set "unknown"//25
//[out]strEpiXmlFileName:�����...epi.xml�ļ�·��//26
bool
	writeEPIXml(const char*strImgFName,const char* strImgNName,const char* strImgBName ,char*CornerLongRange,char*CornerLatRange,
	char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,char*NorthWestAbs,char*NorthEastOrd,
	char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,char * fn_meanParallax,char * nb_meanParallax,
	char * fb_meanParallax,char *EarthEllipsoid,char *GeodeticDatum,char *AltitudeDatum,char*AltitudeSystem,
	char*MapProjection,char*ZoneDivisionMode,char*ZoneNo,char*HeightMode,char *ResamplingKernel,
	const char*strEpiXmlFileName);
#endif