#include "xml_1_50000_dsm_w.h"
#include <direct.h>
#include <io.h>
#include <sys/stat.h>

inline void  degree_decimal2dms(double decimal,int* degree,int* minute,int* second)
{
	*degree = int(decimal);
	decimal = decimal-*degree;	decimal = decimal*60;
	*minute = int(decimal);
	decimal = decimal-*minute;	decimal = decimal*60;
	*second = int(decimal);
}

bool write_1_50000DSMXML(const char* lpstrFilePath,char*MapNumber,char*GridSize,int nGridRows,int nGridCols,
						 double xmin,double ymin,double xmax,double ymax,CSpGeoCvt& geoCvt,
						 char*str_1_50000DsmXmlFileName)
{
	struct stat buf;
	int cc;
    cc=stat(lpstrFilePath,&buf);	//lstat
    if(cc) return false;
	char size[10]; sprintf(size,"%.1lf",(buf.st_size/1024.0/1024.0));

	char GridRows[10],GridCols[10];
	sprintf(GridCols,"%d",nGridCols);	sprintf(GridRows,"%d",nGridRows);

	char CornerLongRange[50]=	"unknown";
	char CornerLatRange[50]	=	"unknown";
	{
		double lon_min,lon_max,lat_min,lat_max,tz;
		geoCvt.Cvt_Prj2LBH(xmin,ymin,0,&lon_min,&lat_min,&tz);
		geoCvt.Cvt_Prj2LBH(xmax,ymax,0,&lon_max,&lat_max,&tz);
		lon_min *= SPGC_R2D;	lon_max *= SPGC_R2D;
		lat_min *= SPGC_R2D;	lat_max *= SPGC_R2D;

		int degree,minute,second;
		degree_decimal2dms(lon_min,&degree,&minute,&second);
		sprintf(CornerLongRange,"%03d%02d%02d",degree,minute,second);
		degree_decimal2dms(lat_min,&degree,&minute,&second);
		sprintf(CornerLatRange,"%03d%02d%02d",degree,minute,second);
		degree_decimal2dms(lon_max,&degree,&minute,&second);
		sprintf(CornerLongRange+strlen(CornerLongRange),"-%03d%02d%02d",degree,minute,second);
		degree_decimal2dms(lat_max,&degree,&minute,&second);
		sprintf(CornerLatRange+strlen(CornerLatRange),"-%03d%02d%02d",degree,minute,second);
		printf("LatRange:%s\nLongRange:%s\n",CornerLatRange,CornerLongRange);
	}
	char SouthWestOrd[20]="unknown";	sprintf(SouthWestOrd,"%.2lf",ymin);
	char SouthWestAbs[20]="unknown";	sprintf(SouthWestAbs,"%.2lf",xmin);
	char NorthWestOrd[20]="unknown";	sprintf(NorthWestOrd,"%.2lf",ymax);
	char NorthWestAbs[20]="unknown";	sprintf(NorthWestAbs,"%.2lf",xmin);
	char NorthEastOrd[20]="unknown";	sprintf(NorthEastOrd,"%.2lf",ymax);
	char NorthEastAbs[20]="unknown";	sprintf(NorthEastAbs,"%.2lf",xmax);
	char SouthEastOrd[20]="unknown";	sprintf(SouthEastOrd,"%.2lf",ymin);
	char SouthEastAbs[20]="unknown";	sprintf(SouthEastAbs,"%.2lf",xmax);

	const char* lpstrGeodeticDatum[] = {"WGS1984","BEIJING54","XIAN80","CGCS2000"};
	const char* lpstrMapProjection[] = {"empty","TM 投影","高斯-克吕格投影","UTM 投影","兰伯特投影"};
	int Ellipsoid;           //椭球类型	
	int Projectid;           //投影类型 
	char CentralMederian[10];
	char ZoneNo[10];
	{
		
		double EllipsoA;         //椭球长半轴
		double EllipsoB;	     //椭球短半轴      
		double ProjectOriLat;    //中心纬度(弧度)
		double ProjectCenMeri;   //中心经度(弧度)
		double ProjectFalseEas;  //东偏移
		double ProjectFalseNor;  //北偏移
		double ProjectScale;     //中心经线比例尺
		int eleType;
		
		geoCvt.Get_Cvt_Par(&Ellipsoid, &Projectid, &EllipsoA, &EllipsoB, &ProjectOriLat, &ProjectCenMeri, 
			&ProjectFalseEas, &ProjectFalseNor, &ProjectScale, &eleType );
		ProjectCenMeri *= SPGC_R2D;
		sprintf(CentralMederian,"%d",int(ProjectCenMeri));
		sprintf(ZoneNo,"%d",int((ProjectCenMeri+3+180)/6));	
		
	}
	return write_1_50000DSMXML(MapNumber,size,GridSize,GridRows,GridCols,CornerLongRange,CornerLatRange,SouthWestOrd,SouthWestAbs,NorthWestOrd,NorthWestAbs,
		NorthEastOrd,NorthEastAbs,SouthEastOrd,SouthEastAbs,"6378137","1/298.257222101",(char*)(lpstrGeodeticDatum[Ellipsoid]),(char*)(lpstrMapProjection[Projectid]),
		CentralMederian,"6度带",Projectid==2?ZoneNo:"empty","大地高","1985中国高程基准",str_1_50000DsmXmlFileName);
}

bool
	write_1_50000DSMXML(char*MapNumber,char*ImgSize,char*GridSize,char*GridRows,char*GridCols,
	char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,
	char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,
	char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,char*MapProjection,char*CentralMederian,
	char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,char*HeightDatum,char*str_1_50000DsmXmlFileName)
{
	CMarkup xml_1_50000_dsm;//write 1_50000_dsm
	char strTmp[512];//temp string
	xml_1_50000_dsm.SetDoc("<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
	xml_1_50000_dsm.AddElem("digital_surface_model_metadata");
	xml_1_50000_dsm.IntoElem();
	{
		xml_1_50000_dsm.AddElem("ProductInfo");
		xml_1_50000_dsm.IntoElem();
		{
			char *pS1,*pS2,*pS;
			pS1 = strrchr(str_1_50000DsmXmlFileName,'\\');		pS2 = strrchr(str_1_50000DsmXmlFileName,'/');
			if(pS1||pS2) {if(pS1)pS=pS1;else pS=pS2;}
			else {if(pS1>pS2) pS = pS2; else pS = pS1;}
			xml_1_50000_dsm.AddElem("MetaDataFileName",pS+1);//1
			xml_1_50000_dsm.AddElem("ProductName","1:50000数字表面模型");//2
			xml_1_50000_dsm.AddElem("ProductCode","empty");//3
			xml_1_50000_dsm.AddElem("MapName","empty");//4
			if(NULL==MapNumber)xml_1_50000_dsm.AddElem("MapNumber","unknown");
			else xml_1_50000_dsm.AddElem("MapNumber",MapNumber);//5
			xml_1_50000_dsm.AddElem("Scaledenominator","empty");//6
			//////////////////////////////////////////////////////////////////////////
			//add system time
			time_t lt = time(0); 
			struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
			sprintf(strTmp, "%d",st.tm_year+1900);pS = strTmp + strlen(strTmp);
			if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
			pS = strTmp + strlen(strTmp);
			if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
			pS = strTmp + strlen(strTmp);
			xml_1_50000_dsm.AddElem("ProduceDate",strTmp);//7
			//////////////////////////////////////////////////////////////////////////
			xml_1_50000_dsm.AddElem("ProductUppdate","empty");//8
			xml_1_50000_dsm.AddElem("ProductVersion","empty");//9
			xml_1_50000_dsm.AddElem("Owner","国家测绘地理信息局");//10
			xml_1_50000_dsm.AddElem("Producer","国家测绘地理信息局卫星测绘应用中心");//11
			xml_1_50000_dsm.AddElem("Publisher","国家测绘地理信息局");//12
			if(NULL==ImgSize) xml_1_50000_dsm.AddElem("ImgSize","unknown");
			else xml_1_50000_dsm.AddElem("ImgSize",ImgSize);//13
			xml_1_50000_dsm.AddElem("DataFormat","Tif");//14
			if(NULL==GridSize) xml_1_50000_dsm.AddElem("GridSize","unknown");//15
			else xml_1_50000_dsm.AddElem("GridSize",GridSize);
			xml_1_50000_dsm.AddElem("GridSizeUnit","米");//16
			xml_1_50000_dsm.AddElem("GridArrange","由西到东、由北向南");//17
			if(NULL==GridRows) xml_1_50000_dsm.AddElem("GridRows","unknown");//18
			else xml_1_50000_dsm.AddElem("GridRows",GridRows);
			if(NULL==GridCols) xml_1_50000_dsm.AddElem("GridCols","unknown");
			else xml_1_50000_dsm.AddElem("GridCols",GridCols);//9
			xml_1_50000_dsm.AddElem("PDecimalDigits","empty");//20
			xml_1_50000_dsm.AddElem("HDecimalDigits","empty");//21
			if(NULL==CornerLongRange) xml_1_50000_dsm.AddElem("CornerLongRange","unknown");
			else xml_1_50000_dsm.AddElem("CornerLongRange",CornerLongRange);//22
			if(NULL==CornerLatRange) xml_1_50000_dsm.AddElem("CornerLatRange","unknown");
			else xml_1_50000_dsm.AddElem("CornerLatRange",CornerLatRange);//23
			if(NULL==SouthWestOrd) xml_1_50000_dsm.AddElem("SouthWestOrd","unknown");
			else xml_1_50000_dsm.AddElem("SouthWestOrd",SouthWestOrd);//24
			if(NULL==SouthWestAbs) xml_1_50000_dsm.AddElem("SouthWestAbs","unknown");
			else xml_1_50000_dsm.AddElem("SouthWestAbs",SouthWestAbs);//25
			if(NULL==NorthWestOrd)xml_1_50000_dsm.AddElem("NorthWestOrd","unknown");
			else xml_1_50000_dsm.AddElem("NorthWestOrd",NorthWestOrd);//26
			if(NULL==NorthWestAbs)xml_1_50000_dsm.AddElem("NorthWestAbs","unknown");
			else xml_1_50000_dsm.AddElem("NorthWestAbs",NorthWestAbs);//27
			if(NULL==NorthEastOrd)xml_1_50000_dsm.AddElem("NorthEastOrd","unknown");
			else xml_1_50000_dsm.AddElem("NorthEastOrd",NorthEastOrd);//28
			if(NULL==NorthEastAbs)xml_1_50000_dsm.AddElem("NorthEastAbs","unknown");
			else xml_1_50000_dsm.AddElem("NorthEastAbs",NorthEastAbs);//29
			if(NULL==SouthEastOrd)xml_1_50000_dsm.AddElem("SouthEastOrd","unknown");
			else xml_1_50000_dsm.AddElem("SouthEastOrd",SouthEastOrd);//30
			if(NULL==SouthEastAbs)xml_1_50000_dsm.AddElem("SouthEastAbs","unknown");
			else xml_1_50000_dsm.AddElem("SouthEastAbs",SouthEastAbs);//31
			xml_1_50000_dsm.AddElem("ConfidentialLevel","秘密");//32			
			if(NULL==LongerRadius)xml_1_50000_dsm.AddElem("LongerRadius","unknown");		
			else xml_1_50000_dsm.AddElem("LongerRadius",LongerRadius);//33
			if(NULL==OblatusRatio)xml_1_50000_dsm.AddElem("OblatusRatio","unknown");
			else xml_1_50000_dsm.AddElem("OblatusRatio",OblatusRatio);//34
			if(NULL==GeodeticDatum)xml_1_50000_dsm.AddElem("GeodeticDatum","unknown");
			else xml_1_50000_dsm.AddElem("GeodeticDatum",GeodeticDatum);//35
			if(NULL==MapProjection)xml_1_50000_dsm.AddElem("MapProjection","unknown");
			else xml_1_50000_dsm.AddElem("MapProjection",MapProjection);//36
			if(NULL==CentralMederian)xml_1_50000_dsm.AddElem("CentralMederian","unknown");
			else xml_1_50000_dsm.AddElem("CentralMederian",CentralMederian);//37
			if(NULL==ZoneDivisionMode)xml_1_50000_dsm.AddElem("ZoneDivisionMode","unknown");
			else xml_1_50000_dsm.AddElem("ZoneDivisionMode",ZoneDivisionMode);//38
			if(NULL==GaussKrugerZoneNo)xml_1_50000_dsm.AddElem("GaussKrugerZoneNo","unknown");
			else xml_1_50000_dsm.AddElem("GaussKrugerZoneNo",GaussKrugerZoneNo);//39
			xml_1_50000_dsm.AddElem("CoordinationUnit","米");//40
			if(NULL==HeightSystem)xml_1_50000_dsm.AddElem("HeightSystem","unknown");
			else xml_1_50000_dsm.AddElem("HeightSystem",HeightSystem);//41
			if(NULL==HeightDatum)xml_1_50000_dsm.AddElem("HeightDatum","unknown");
			else xml_1_50000_dsm.AddElem("HeightDatum",HeightDatum);//42
			xml_1_50000_dsm.AddElem("WestMosaic","empty");//43
			xml_1_50000_dsm.AddElem("NorthMosaic","empty");//44
			xml_1_50000_dsm.AddElem("EastMosaic","empty");//45
			xml_1_50000_dsm.AddElem("SouthMosaic","empty");//46
			xml_1_50000_dsm.AddElem("NorthWestMapName","empty");//47
			xml_1_50000_dsm.AddElem("NorthMapName","empty");//48
			xml_1_50000_dsm.AddElem("NorthEastMapName","empty");//49
			xml_1_50000_dsm.AddElem("WestMapName","empty");//50
			xml_1_50000_dsm.AddElem("EastMapName","empty");//51
			xml_1_50000_dsm.AddElem("SouthWestMapName","empty");//52
			xml_1_50000_dsm.AddElem("SouthMapName","empty");//53
			xml_1_50000_dsm.AddElem("SouthWestMapName","empty");//54
			xml_1_50000_dsm.AddElem("VerticalRMS","empty");//55
			xml_1_50000_dsm.AddElem("Integrity","empty");//56
			xml_1_50000_dsm.AddElem("MosicQuality","empty");//57
			xml_1_50000_dsm.AddElem("EditDegree","empty");//58
			xml_1_50000_dsm.AddElem("EditExplanation","empty");//59
			xml_1_50000_dsm.AddElem("RepairArea","empty");//60
			xml_1_50000_dsm.AddElem("RepairReason","empty");//61
			xml_1_50000_dsm.AddElem("RepairDescription","empty");//62
			xml_1_50000_dsm.AddElem("IsRepaired","empty");//63
			xml_1_50000_dsm.AddElem("RepairedDataType","empty");//64
			xml_1_50000_dsm.AddElem("RepairDataName","empty");//65
			xml_1_50000_dsm.AddElem("RepairMethod","empty");//66
			xml_1_50000_dsm.AddElem("RepairedWidth","empty");//67
			xml_1_50000_dsm.AddElem("Conclusion","empty");//68
			xml_1_50000_dsm.AddElem("QualityEvalDepartment","empty");//69
			xml_1_50000_dsm.AddElem("QualityEvalDate","empty");//70
			xml_1_50000_dsm.AddElem("DataQuality","empty");//71
		}
		xml_1_50000_dsm.OutOfElem();
		xml_1_50000_dsm.AddElem("dataSourceInfo");
		xml_1_50000_dsm.IntoElem();
		{
			xml_1_50000_dsm.AddElem("DataSourceType","unknown");//72
			xml_1_50000_dsm.AddElem("DataSource","unknown");//73
			xml_1_50000_dsm.AddElem("DataSourceDate","unknown");//74
			xml_1_50000_dsm.AddElem("DataSourceGSD","unknown");//75
			xml_1_50000_dsm.AddElem("DataSourceColor","unknown");//76
			xml_1_50000_dsm.AddElem("DataSourceQuality","empty");//77
			xml_1_50000_dsm.AddElem("DataSourceAccuracy","empty");//78
			xml_1_50000_dsm.AddElem("DataSourceEPrecision","empty");//79
			xml_1_50000_dsm.AddElem("DataSourceRemark","empty");//80
			xml_1_50000_dsm.AddElem("AssistantDataORNot","empty");//81
			xml_1_50000_dsm.AddElem("AssistantDataType","empty");//82
			xml_1_50000_dsm.AddElem("AssistantData","empty");//83
			xml_1_50000_dsm.AddElem("AssistantDataDate","empty");//84
			xml_1_50000_dsm.AddElem("AssistantDataGSD","empty");//85
			xml_1_50000_dsm.AddElem("AssistantDataAccuracy","empty");//86
			xml_1_50000_dsm.AddElem("AssistantDataElevation","empty");//87
			xml_1_50000_dsm.AddElem("AssistantDataRemark","empty");//88
		}
		xml_1_50000_dsm.OutOfElem();
		xml_1_50000_dsm.AddElem("processInfo");
		xml_1_50000_dsm.IntoElem();
		{
			xml_1_50000_dsm.AddElem("ControlSource","empty");//89
			xml_1_50000_dsm.AddElem("SateOriXRMS","empty");//90
			xml_1_50000_dsm.AddElem("SateOriYRMS","empty");//91
			xml_1_50000_dsm.AddElem("SateOriZRMS","empty");//92
			xml_1_50000_dsm.AddElem("ATProducerName","empty");//93
			xml_1_50000_dsm.AddElem("ATCheckerName","empty");//94
			xml_1_50000_dsm.AddElem("ManufactureType","empty");//95
			xml_1_50000_dsm.AddElem("Interpolation","empty");//96
			xml_1_50000_dsm.AddElem("OrthoRectifySoftware","empty");//97
			xml_1_50000_dsm.AddElem("OrthoRectifyQulity","empty");//98
			xml_1_50000_dsm.AddElem("OrthoRectifyName","empty");//99
			xml_1_50000_dsm.AddElem("OrthoCheckName","empty");//100
			xml_1_50000_dsm.AddElem("ExceptError","empty");//101
			xml_1_50000_dsm.AddElem("WatersEditExplanation","empty");//102	
			xml_1_50000_dsm.AddElem("RiversEditExplanation","empty");//103
			xml_1_50000_dsm.AddElem("ValleyEditExplanation","empty");//104
			xml_1_50000_dsm.AddElem("OtherEditExplanation","empty");//105
			xml_1_50000_dsm.AddElem("WestMosaicMaxError","empty");//106	
			xml_1_50000_dsm.AddElem("NorthMosaicMaxError","empty");//107
			xml_1_50000_dsm.AddElem("EastMosaicMaxError","empty");//108
			xml_1_50000_dsm.AddElem("SouthMosaicMaxError","empty");//109
			xml_1_50000_dsm.AddElem("NorthEastMosaicMaxError","empty");//110	
			xml_1_50000_dsm.AddElem("NorthWestMosaicMaxError","empty");//111	
			xml_1_50000_dsm.AddElem("SouthEastMosaicMaxError","empty");//112
			xml_1_50000_dsm.AddElem("SouthWestMosaicMaxError","empty");//113
			xml_1_50000_dsm.AddElem("MosicProducerName","empty");//114
			xml_1_50000_dsm.AddElem("MosicCheckerName","empty");//115
			xml_1_50000_dsm.AddElem("GeometryAccuracy","empty");//116
			xml_1_50000_dsm.AddElem("CheckPointNum","empty");//117
			xml_1_50000_dsm.AddElem("CheckPointSource","empty");//118
			xml_1_50000_dsm.AddElem("CheckRMS","empty");//119
			xml_1_50000_dsm.AddElem("CheckMaxErr","empty");//120
			xml_1_50000_dsm.AddElem("ConclusionInstitute","empty");//121
			xml_1_50000_dsm.AddElem("InstituteCheckUnit","empty");//122
			xml_1_50000_dsm.AddElem("InstituteCheckName","empty");//123
			xml_1_50000_dsm.AddElem("InstituteCheckDate","empty");//124
			xml_1_50000_dsm.AddElem("BureauCheckName","empty");//125
			xml_1_50000_dsm.AddElem("BureauCheckUnit","empty");//126
			xml_1_50000_dsm.AddElem("ConclusionBureau","empty");//127
			xml_1_50000_dsm.AddElem("BureauCheckDate","empty");//128
		}
		xml_1_50000_dsm.OutOfElem();
		xml_1_50000_dsm.AddElem("distributionInfo");
		xml_1_50000_dsm.IntoElem();
		{
			xml_1_50000_dsm.AddElem("ProductPrice","empty");//129
			xml_1_50000_dsm.AddElem("Productmedia","empty");//130
			xml_1_50000_dsm.AddElem("Productformat","empty");//131
			xml_1_50000_dsm.AddElem("DistributorPhone","empty");//132
			xml_1_50000_dsm.AddElem("DistributorFaxPhone","empty");//133
			xml_1_50000_dsm.AddElem("DistributorAdministratityArea","empty");//134
			xml_1_50000_dsm.AddElem("DistributorAddress","empty");//135
			xml_1_50000_dsm.AddElem("DistributorPostCode","empty");//136
			xml_1_50000_dsm.AddElem("DistributorName","empty");//137
			xml_1_50000_dsm.AddElem("DistributorEMail","empty");//138
			xml_1_50000_dsm.AddElem("DistributorURL","empty");//139
		}
		xml_1_50000_dsm.OutOfElem();
	}
	xml_1_50000_dsm.OutOfElem();
	strcpy(strTmp,str_1_50000DsmXmlFileName);
	{
		char *pS1,*pS2,*pS;
		pS1 = strrchr(strTmp,'\\');		pS2 = strrchr(strTmp,'/');
		if(pS1||pS2) {if(pS1)pS=pS1;else pS=pS2;}
		else {if(pS1>pS2) pS = pS2; else pS = pS1;}
		*pS = 0;
	}
	if(!access(strTmp,0)){
		xml_1_50000_dsm.Save(str_1_50000DsmXmlFileName);
		printf("\tSave xml_1_50000_dsm success:%s\n",str_1_50000DsmXmlFileName);}
	else{
		_mkdir(strTmp);xml_1_50000_dsm.Save(str_1_50000DsmXmlFileName);
		printf("\tSave xml_1_50000_dsm success:%s\n",str_1_50000DsmXmlFileName);}				
	return true;
}