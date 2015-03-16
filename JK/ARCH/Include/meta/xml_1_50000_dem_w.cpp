#include "xml_1_50000_dem_w.h"
#include <io.h>
#include <direct.h>
#include <sys/stat.h>

inline void  degree_decimal2dms(double decimal,int* degree,int* minute,int* second)
{
	*degree = int(decimal);
	decimal = decimal-*degree;	decimal = decimal*60;
	*minute = int(decimal);
	decimal = decimal-*minute;	decimal = decimal*60;
	*second = int(decimal);
}

bool write_1_50000DEMXML(const char* lpstrFilePath,char*MapNumber,char*GridSize,int nGridRows,int nGridCols,
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
	char ZoneNo[10]="empty";
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
		if(Projectid==2) sprintf(ZoneNo,"%d",int((ProjectCenMeri+3+180)/6));	
		
	}
	return write_1_50000DEMXML(MapNumber,size,GridSize,GridRows,GridCols,CornerLongRange,CornerLatRange,SouthWestOrd,SouthWestAbs,NorthWestOrd,NorthWestAbs,
		NorthEastOrd,NorthEastAbs,SouthEastOrd,SouthEastAbs,"6378137","1/298.257222101",(char*)(lpstrGeodeticDatum[Ellipsoid]),(char*)(lpstrMapProjection[Projectid]),
		CentralMederian,"6度带",ZoneNo,"大地高","1985中国高程基准",str_1_50000DsmXmlFileName);
}


bool
	write_1_50000DEMXML(char*MapNumber,char*ImgSize,char*GridSize,char*GridRows,char*GridCols,
	char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,
	char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,
	char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,char*MapProjection,char*CentralMederian,
	char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,char*HeightDatum,char*str_1_50000DemXmlFileName)
{
	CMarkup xml_1_50000_dem;//write 1_50000_dem
	char strTmp[512];//temp string
	xml_1_50000_dem.SetDoc("<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
	xml_1_50000_dem.AddElem("digital_elevation_model_metadata");
	xml_1_50000_dem.IntoElem();
	{
		xml_1_50000_dem.AddElem("ProductInfo");
		xml_1_50000_dem.IntoElem();
		{
			char *pS1,*pS2,*pS;
			pS1 = strrchr(str_1_50000DemXmlFileName,'\\');		pS2 = strrchr(str_1_50000DemXmlFileName,'/');
			if(pS1||pS2) {if(pS1)pS=pS1;else pS=pS2;}
			else {if(pS1>pS2) pS = pS2; else pS = pS1;}
			xml_1_50000_dem.AddElem("MetaDataFileName",pS+1);//1
			xml_1_50000_dem.AddElem("ProductName","1:50000数字高程模型");//2
			xml_1_50000_dem.AddElem("ProductCode","empty");//3
			xml_1_50000_dem.AddElem("MapName","empty");//4
			if(NULL==MapNumber)xml_1_50000_dem.AddElem("MapNumber","unknown");
			else xml_1_50000_dem.AddElem("MapNumber",MapNumber);//5
			xml_1_50000_dem.AddElem("Scaledenominator","empty");//6
			//////////////////////////////////////////////////////////////////////////
			//add system time
			time_t lt = time(0); 
			struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
			sprintf(strTmp, "%d",st.tm_year+1900);pS = strTmp + strlen(strTmp);
			if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
			pS = strTmp + strlen(strTmp);
			if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
			pS = strTmp + strlen(strTmp);
			xml_1_50000_dem.AddElem("ProduceDate",strTmp);//7
			//////////////////////////////////////////////////////////////////////////
			xml_1_50000_dem.AddElem("ProductUppdate","empty");//8
			xml_1_50000_dem.AddElem("ProductVersion","empty");//9
			xml_1_50000_dem.AddElem("Owner","国家测绘地理信息局");//10
			xml_1_50000_dem.AddElem("Producer","国家测绘地理信息局卫星测绘应用中心");//11
			xml_1_50000_dem.AddElem("Publisher","国家测绘地理信息局");//12
			if(NULL==ImgSize) xml_1_50000_dem.AddElem("ImgSize","unknown");
			else xml_1_50000_dem.AddElem("ImgSize",ImgSize);//13
			xml_1_50000_dem.AddElem("DataFormat","Tif");//14
			if(NULL==GridSize) xml_1_50000_dem.AddElem("GridSize","unknown");//15
			else xml_1_50000_dem.AddElem("GridSize",GridSize);
			xml_1_50000_dem.AddElem("GridSizeUnit","米");//16
			xml_1_50000_dem.AddElem("GridArrange","由西到东、由北向南");//17
			if(NULL==GridRows) xml_1_50000_dem.AddElem("GridRows","unknown");//18
			else xml_1_50000_dem.AddElem("GridRows",GridRows);
			if(NULL==GridCols) xml_1_50000_dem.AddElem("GridCols","unknown");
			else xml_1_50000_dem.AddElem("GridCols",GridCols);//9
			xml_1_50000_dem.AddElem("PDecimalDigits","empty");//20
			xml_1_50000_dem.AddElem("HDecimalDigits","empty");//21
			if(NULL==CornerLongRange) xml_1_50000_dem.AddElem("CornerLongRange","unknown");
			else xml_1_50000_dem.AddElem("CornerLongRange",CornerLongRange);//22
			if(NULL==CornerLatRange) xml_1_50000_dem.AddElem("CornerLatRange","unknown");
			else xml_1_50000_dem.AddElem("CornerLatRange",CornerLatRange);//23
			if(NULL==SouthWestOrd) xml_1_50000_dem.AddElem("SouthWestOrd","unknown");
			else xml_1_50000_dem.AddElem("SouthWestOrd",SouthWestOrd);//24
			if(NULL==SouthWestAbs) xml_1_50000_dem.AddElem("SouthWestAbs","unknown");
			else xml_1_50000_dem.AddElem("SouthWestAbs",SouthWestAbs);//25
			if(NULL==NorthWestOrd)xml_1_50000_dem.AddElem("NorthWestOrd","unknown");
			else xml_1_50000_dem.AddElem("NorthWestOrd",NorthWestOrd);//26
			if(NULL==NorthWestAbs)xml_1_50000_dem.AddElem("NorthWestAbs","unknown");
			else xml_1_50000_dem.AddElem("NorthWestAbs",NorthWestAbs);//27
			if(NULL==NorthEastOrd)xml_1_50000_dem.AddElem("NorthEastOrd","unknown");
			else xml_1_50000_dem.AddElem("NorthEastOrd",NorthEastOrd);//28
			if(NULL==NorthEastAbs)xml_1_50000_dem.AddElem("NorthEastAbs","unknown");
			else xml_1_50000_dem.AddElem("NorthEastAbs",NorthEastAbs);//29
			if(NULL==SouthEastOrd)xml_1_50000_dem.AddElem("SouthEastOrd","unknown");
			else xml_1_50000_dem.AddElem("SouthEastOrd",SouthEastOrd);//30
			if(NULL==SouthEastAbs)xml_1_50000_dem.AddElem("SouthEastAbs","unknown");
			else xml_1_50000_dem.AddElem("SouthEastAbs",SouthEastAbs);//31
			xml_1_50000_dem.AddElem("ConfidentialLevel","秘密");//32			
			if(NULL==LongerRadius)xml_1_50000_dem.AddElem("LongerRadius","unknown");		
			else xml_1_50000_dem.AddElem("LongerRadius",LongerRadius);//33
			if(NULL==OblatusRatio)xml_1_50000_dem.AddElem("OblatusRatio","unknown");
			else xml_1_50000_dem.AddElem("OblatusRatio",OblatusRatio);//34
			if(NULL==GeodeticDatum)xml_1_50000_dem.AddElem("GeodeticDatum","unknown");
			else xml_1_50000_dem.AddElem("GeodeticDatum",GeodeticDatum);//35
			if(NULL==MapProjection)xml_1_50000_dem.AddElem("MapProjection","unknown");
			else xml_1_50000_dem.AddElem("MapProjection",MapProjection);//36
			if(NULL==CentralMederian)xml_1_50000_dem.AddElem("CentralMederian","unknown");
			else xml_1_50000_dem.AddElem("CentralMederian",CentralMederian);//37
			if(NULL==ZoneDivisionMode)xml_1_50000_dem.AddElem("ZoneDivisionMode","unknown");
			else xml_1_50000_dem.AddElem("ZoneDivisionMode",ZoneDivisionMode);//38
			if(NULL==GaussKrugerZoneNo)xml_1_50000_dem.AddElem("GaussKrugerZoneNo","unknown");
			else xml_1_50000_dem.AddElem("GaussKrugerZoneNo",GaussKrugerZoneNo);//39
			xml_1_50000_dem.AddElem("CoordinationUnit","米");//40
			if(NULL==HeightSystem)xml_1_50000_dem.AddElem("HeightSystem","unknown");
			else xml_1_50000_dem.AddElem("HeightSystem",HeightSystem);//41
			if(NULL==HeightDatum)xml_1_50000_dem.AddElem("HeightDatum","unknown");
			else xml_1_50000_dem.AddElem("HeightDatum",HeightDatum);//42
			xml_1_50000_dem.AddElem("WestMosaic","empty");//43
			xml_1_50000_dem.AddElem("NorthMosaic","empty");//44
			xml_1_50000_dem.AddElem("EastMosaic","empty");//45
			xml_1_50000_dem.AddElem("SouthMosaic","empty");//46
			xml_1_50000_dem.AddElem("NorthWestMapName","empty");//47
			xml_1_50000_dem.AddElem("NorthMapName","empty");//48
			xml_1_50000_dem.AddElem("NorthEastMapName","empty");//49
			xml_1_50000_dem.AddElem("WestMapName","empty");//50
			xml_1_50000_dem.AddElem("EastMapName","empty");//51
			xml_1_50000_dem.AddElem("SouthWestMapName","empty");//52
			xml_1_50000_dem.AddElem("SouthMapName","empty");//53
			xml_1_50000_dem.AddElem("SouthWestMapName","empty");//54
			xml_1_50000_dem.AddElem("VerticalRMS","empty");//55
			xml_1_50000_dem.AddElem("Integrity","empty");//56
			xml_1_50000_dem.AddElem("MosicQuality","empty");//57
			xml_1_50000_dem.AddElem("EditDegree","empty");//58
			xml_1_50000_dem.AddElem("EditExplanation","empty");//59
			xml_1_50000_dem.AddElem("RepairArea","empty");//60
			xml_1_50000_dem.AddElem("RepairReason","empty");//61
			xml_1_50000_dem.AddElem("RepairDescription","empty");//62
			xml_1_50000_dem.AddElem("IsRepaired","empty");//63
			xml_1_50000_dem.AddElem("RepairedDataType","empty");//64
			xml_1_50000_dem.AddElem("RepairDataName","empty");//65
			xml_1_50000_dem.AddElem("RepairMethod","empty");//66
			xml_1_50000_dem.AddElem("RepairedWidth","empty");//67
			xml_1_50000_dem.AddElem("Conclusion","empty");//68
			xml_1_50000_dem.AddElem("QualityEvalDepartment","empty");//69
			xml_1_50000_dem.AddElem("QualityEvalDate","empty");//70
			xml_1_50000_dem.AddElem("DataQuality","empty");//71
		}
		xml_1_50000_dem.OutOfElem();
		xml_1_50000_dem.AddElem("dataSourceInfo");
		xml_1_50000_dem.IntoElem();
		{
			xml_1_50000_dem.AddElem("DataSourceType","unknown");//72
			xml_1_50000_dem.AddElem("DataSource","unknown");//73
			xml_1_50000_dem.AddElem("DataSourceDate","unknown");//74
			xml_1_50000_dem.AddElem("DataSourceGSD","unknown");//75
			xml_1_50000_dem.AddElem("DataSourceColor","unknown");//76
			xml_1_50000_dem.AddElem("DataSourceQuality","empty");//77
			xml_1_50000_dem.AddElem("DataSourceAccuracy","empty");//78
			xml_1_50000_dem.AddElem("DataSourceEPrecision","empty");//79
			xml_1_50000_dem.AddElem("DataSourceRemark","empty");//80
			xml_1_50000_dem.AddElem("AssistantDataORNot","empty");//81
			xml_1_50000_dem.AddElem("AssistantDataType","empty");//82
			xml_1_50000_dem.AddElem("AssistantData","empty");//83
			xml_1_50000_dem.AddElem("AssistantDataDate","empty");//84
			xml_1_50000_dem.AddElem("AssistantDataGSD","empty");//85
			xml_1_50000_dem.AddElem("AssistantDataAccuracy","empty");//86
			xml_1_50000_dem.AddElem("AssistantDataElevation","empty");//87
			xml_1_50000_dem.AddElem("AssistantDataRemark","empty");//88
		}
		xml_1_50000_dem.OutOfElem();
		xml_1_50000_dem.AddElem("processInfo");
		xml_1_50000_dem.IntoElem();
		{
			xml_1_50000_dem.AddElem("ControlSource","empty");//89
			xml_1_50000_dem.AddElem("SateOriXRMS","empty");//90
			xml_1_50000_dem.AddElem("SateOriYRMS","empty");//91
			xml_1_50000_dem.AddElem("SateOriZRMS","empty");//92
			xml_1_50000_dem.AddElem("ATProducerName","empty");//93
			xml_1_50000_dem.AddElem("ATCheckerName","empty");//94
			xml_1_50000_dem.AddElem("ManufactureType","empty");//95
			xml_1_50000_dem.AddElem("Interpolation","empty");//96
			xml_1_50000_dem.AddElem("OrthoRectifySoftware","empty");//97
			xml_1_50000_dem.AddElem("OrthoRectifyQulity","empty");//98
			xml_1_50000_dem.AddElem("OrthoRectifyName","empty");//99
			xml_1_50000_dem.AddElem("OrthoCheckName","empty");//100
			xml_1_50000_dem.AddElem("ExceptError","empty");//101
			xml_1_50000_dem.AddElem("WatersEditExplanation","empty");//102	
			xml_1_50000_dem.AddElem("RiversEditExplanation","empty");//103
			xml_1_50000_dem.AddElem("ValleyEditExplanation","empty");//104
			xml_1_50000_dem.AddElem("OtherEditExplanation","empty");//105
			xml_1_50000_dem.AddElem("WestMosaicMaxError","empty");//106	
			xml_1_50000_dem.AddElem("NorthMosaicMaxError","empty");//107
			xml_1_50000_dem.AddElem("EastMosaicMaxError","empty");//108
			xml_1_50000_dem.AddElem("SouthMosaicMaxError","empty");//109
			xml_1_50000_dem.AddElem("NorthEastMosaicMaxError","empty");//110	
			xml_1_50000_dem.AddElem("NorthWestMosaicMaxError","empty");//111	
			xml_1_50000_dem.AddElem("SouthEastMosaicMaxError","empty");//112
			xml_1_50000_dem.AddElem("SouthWestMosaicMaxError","empty");//113
			xml_1_50000_dem.AddElem("MosicProducerName","empty");//114
			xml_1_50000_dem.AddElem("MosicCheckerName","empty");//115
			xml_1_50000_dem.AddElem("GeometryAccuracy","empty");//116
			xml_1_50000_dem.AddElem("CheckPointNum","empty");//117
			xml_1_50000_dem.AddElem("CheckPointSource","empty");//118
			xml_1_50000_dem.AddElem("CheckRMS","empty");//119
			xml_1_50000_dem.AddElem("CheckMaxErr","empty");//120
			xml_1_50000_dem.AddElem("ConclusionInstitute","empty");//121
			xml_1_50000_dem.AddElem("InstituteCheckUnit","empty");//122
			xml_1_50000_dem.AddElem("InstituteCheckName","empty");//123
			xml_1_50000_dem.AddElem("InstituteCheckDate","empty");//124
			xml_1_50000_dem.AddElem("BureauCheckName","empty");//125
			xml_1_50000_dem.AddElem("BureauCheckUnit","empty");//126
			xml_1_50000_dem.AddElem("ConclusionBureau","empty");//127
			xml_1_50000_dem.AddElem("BureauCheckDate","empty");//128
		}
		xml_1_50000_dem.OutOfElem();
		xml_1_50000_dem.AddElem("distributionInfo");
		xml_1_50000_dem.IntoElem();
		{
			xml_1_50000_dem.AddElem("ProductPrice","empty");//129
			xml_1_50000_dem.AddElem("Productmedia","empty");//130
			xml_1_50000_dem.AddElem("Productformat","empty");//131
			xml_1_50000_dem.AddElem("DistributorPhone","empty");//132
			xml_1_50000_dem.AddElem("DistributorFaxPhone","empty");//133
			xml_1_50000_dem.AddElem("DistributorAdministratityArea","empty");//134
			xml_1_50000_dem.AddElem("DistributorAddress","empty");//135
			xml_1_50000_dem.AddElem("DistributorPostCode","empty");//136
			xml_1_50000_dem.AddElem("DistributorName","empty");//137
			xml_1_50000_dem.AddElem("DistributorEMail","empty");//138
			xml_1_50000_dem.AddElem("DistributorURL","empty");//139
		}
		xml_1_50000_dem.OutOfElem();
	}
	xml_1_50000_dem.OutOfElem();
	strcpy(strTmp,str_1_50000DemXmlFileName);*strrchr(strTmp,'/')=0;
	if(0==access(strTmp,0)){
		xml_1_50000_dem.Save(str_1_50000DemXmlFileName);
		printf("\tSave xml_1_50000_dem success:%s\n",str_1_50000DemXmlFileName);
	}else{
		_mkdir(strTmp);
		xml_1_50000_dem.Save(str_1_50000DemXmlFileName);
		printf("\tSave xml_1_50000_dem success:%s\n",str_1_50000DemXmlFileName);
	}
	return true;
	strcpy(strTmp,str_1_50000DemXmlFileName);
	{
		char *pS1,*pS2,*pS;
		pS1 = strrchr(strTmp,'\\');		pS2 = strrchr(strTmp,'/');
		if(pS1||pS2) {if(pS1)pS=pS1;else pS=pS2;}
		else {if(pS1>pS2) pS = pS2; else pS = pS1;}
		*pS = 0;
	}
	if(!access(strTmp,0)){
		xml_1_50000_dem.Save(str_1_50000DemXmlFileName);
		printf("\tSave xml_1_50000_dem success:%s\n",str_1_50000DemXmlFileName);}
	else{
		_mkdir(strTmp);xml_1_50000_dem.Save(str_1_50000DemXmlFileName);
		printf("\tSave xml_1_50000_dem success:%s\n",str_1_50000DemXmlFileName);}				
	return true;
}