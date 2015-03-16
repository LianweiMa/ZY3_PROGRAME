#include "xml_dsm_w.h"
#include <direct.h>
#include<io.h>

#include "typedef.h"
#include "cpl_conv.h"
#include "gdal_priv.h"
#include "SpGeoCvt.h"

inline int getfilesize(LPCSTR lpFilePath)
{
	struct stat buf;
	int cc;
    cc=stat(lpFilePath,&buf);	//lstat
    if(!cc ) return buf.st_size;
	
    return 0;
}

inline void  degree_decimal2dms(double decimal,int* degree,int* minute,int* second)
{
	*degree = int(decimal);
	decimal = decimal-*degree;	decimal = decimal*60;
	*minute = int(decimal);
	decimal = decimal-*minute;	decimal = decimal*60;
	*second = int(decimal);
}
inline void Cvt2BLH(double x,double y,double z,double* lon,double* lat,double* h,CSpGeoCvt* geocvt)
{
	int Ellipsoid;           //椭球类型
	int Projectid;           //投影类型  
    double EllipsoA;         //椭球长半轴
    double EllipsoB;	     //椭球短半轴      
    double ProjectOriLat;    //中心纬度(弧度)
    double ProjectCenMeri;   //中心经度(弧度)
    double ProjectFalseEas;  //东偏移
   	double ProjectFalseNor;  //北偏移
    double ProjectScale;     //中心经线比例尺
   	int eleType;
	
	geocvt->Get_Cvt_Par(&Ellipsoid, &Projectid, &EllipsoA, &EllipsoB, &ProjectOriLat, &ProjectCenMeri, 
		&ProjectFalseEas, &ProjectFalseNor, &ProjectScale, &eleType );

	if (Projectid==GEODETIC_LONLAT){
		*lon = x;
		*lat = y;
		*h = z;
	}else if (Projectid==GEODETIC_CENTRXYZ){
		geocvt->Cvt_CXYZ2LBH(x,y,z,lon,lat,h);
	}else{
		geocvt->Cvt_Prj2LBH(x,y,z,lon,lat,h);
	}
}
bool
writeDSMXml(const char* lpstrNameImgF,const char* lpstrNameImgN,const char* lpstrNameImgB,
			const char* lpstrDemPath,const char* lpstrGcdPath,const char* lpstrXmlPath)
{
	int nCols,nRows,nBits,nBands;
	double fTransfer[6];
	double nDataSize = getfilesize(lpstrDemPath)/1024.0/1024;
	char strGrid[20]="unknown",strRows[10]="unknown",strCols[10]="unknown",strDataSize[20]="unknown";
	char CornerLongRange[50]="unknown",CornerLatRange[50]="unknown",SouthWestOrd[20]="unknown",SouthWestAbs[20]="unknown",NorthWestOrd[20]="unknown";
	char NorthWestAbs[20]="unknown",NorthEastOrd[20]="unknown",NorthEastAbs[20]="unknown",SouthEastOrd[20]="unknown",SouthEastAbs[20]="unknown";
	char LongerRadius[20]="6378137",OblatusRatio[20]="1/298.257222101";
	char GeodeticDatum[20] = "WGS1984"; char MapProjection[20] = "UTM";
	char CentralMederian[10] = "117",ZoneDivisionMode[10] = "6",GaussKrugerZoneNo[10] = "unknown";
	char HeightSystem[20] = "正常高",HeightDatum[50] = "1985国家高程基准";

	GDALAllRegister(); 
	GDALDataset *Img = (GDALDataset*)GDALOpen(lpstrDemPath,GA_ReadOnly);
	if(Img == NULL)
	{
		printf("Failed to open images.\n") ;
		return false;
	}
	
	//获取影像信息
	nCols = Img->GetRasterXSize(); 
	nRows = Img->GetRasterYSize();
	nBands= Img->GetRasterCount();		
	GDALDataType dataType = Img->GetRasterBand(1)->GetRasterDataType();
	nBits = GDALGetDataTypeSize(dataType);
	Img->GetGeoTransform(fTransfer);
	
	GDALClose(Img);
	
	CSpGeoCvt geocvt;
	if ( geocvt.Load4File(lpstrGcdPath) ){
		printf("TrasferParameter:\n[");	
		for(int i=0; i<6; i++) printf("%lf\t",fTransfer[i]);
		printf("]\n");
		int degree,minute,second;	double lon,lat,h;
		Cvt2BLH(fTransfer[0],fTransfer[3]+(nRows-1)*fTransfer[5],500,&lon,&lat,&h,&geocvt);
		degree_decimal2dms(lon,&degree,&minute,&second);
		sprintf(CornerLongRange,"%03d%02d%02d",degree,minute,second);
		degree_decimal2dms(lat,&degree,&minute,&second);
		sprintf(CornerLatRange,"%03d%02d%02d",degree,minute,second);
		Cvt2BLH(fTransfer[0]+(nCols-1)*fTransfer[1],fTransfer[3],500,&lon,&lat,&h,&geocvt);
		degree_decimal2dms(lon,&degree,&minute,&second);
		sprintf(CornerLongRange+strlen(CornerLongRange),"-%03d%02d%02d",degree,minute,second);
		degree_decimal2dms(lat,&degree,&minute,&second);
		sprintf(CornerLatRange+strlen(CornerLatRange),"-%03d%02d%02d",degree,minute,second);
		
		sprintf(NorthWestAbs,"%.2lf",fTransfer[0]);	sprintf(NorthWestOrd,"%.2lf",fTransfer[3]);
		sprintf(SouthEastAbs,"%.2lf",fTransfer[0]+(nCols-1)*fTransfer[1]);	sprintf(SouthEastOrd,"%.2lf",fTransfer[3]+(nRows-1)*fTransfer[5]);
		strcpy(NorthEastAbs,SouthEastAbs);	strcpy(NorthEastOrd,NorthWestOrd);
		strcpy(SouthWestAbs,NorthWestAbs);	strcpy(SouthWestOrd,SouthEastOrd);

		int Ellipsoid;           //椭球类型
		int Projectid;           //投影类型  
		double EllipsoA;         //椭球长半轴
		double EllipsoB;	     //椭球短半轴      
		double ProjectOriLat;    //中心纬度(弧度)
		double ProjectCenMeri;   //中心经度(弧度)
		double ProjectFalseEas;  //东偏移
		double ProjectFalseNor;  //北偏移
		double ProjectScale;     //中心经线比例尺
		int eleType;
		
		geocvt.Get_Cvt_Par(&Ellipsoid, &Projectid, &EllipsoA, &EllipsoB, &ProjectOriLat, &ProjectCenMeri, 
		&ProjectFalseEas, &ProjectFalseNor, &ProjectScale, &eleType );

		const char* lpstrEllipsoidName[] = {"WGS1984","BEIJING54","XIAN80","CGCS2000","CUSTUM"};
		const char* lpstrProjectionName[] = {"LonLat","TM","Gauss","UTM","Lambert","CentrXYZ"};
		strcpy(GeodeticDatum,lpstrEllipsoidName[Ellipsoid]);
		strcpy(MapProjection,lpstrProjectionName[Projectid]);
		sprintf(CentralMederian,"%d",int(ProjectCenMeri*SPGC_R2D));
		
	}
	
	sprintf(strGrid,"%d,%d",int(fabs(fTransfer[1])),int(fabs(fTransfer[5])));
	sprintf(strRows,"%d",nRows);	sprintf(strCols,"%d",nCols);
	sprintf(strDataSize,"%.1lf",nDataSize);

	return writeDSMXml(lpstrNameImgF,lpstrNameImgN,lpstrNameImgB,strGrid,strRows,strCols,strDataSize,CornerLongRange,CornerLatRange,
		SouthWestOrd,SouthWestAbs,NorthWestOrd,NorthWestAbs,NorthEastOrd,NorthEastAbs,SouthEastOrd,SouthEastAbs,
		LongerRadius,OblatusRatio,GeodeticDatum,MapProjection,CentralMederian,ZoneDivisionMode,GaussKrugerZoneNo,
		HeightSystem,HeightDatum,"Nearest_Neighbor",lpstrXmlPath);

}

bool
	writeDSMXml(const char*strImgFName,const char* strImgNName,const char* strImgBName ,char*GridSize,char*GridRows,
	char*GridCols,char*ImgSize,char*CornerLongRange,char*CornerLatRange,char*SouthWestOrd,
	char*SouthWestAbs,char*NorthWestOrd,char*NorthWestAbs,char*NorthEastOrd,char*NorthEastAbs,
	char*SouthEastOrd,char*SouthEastAbs,char*LongerRadius,char*OblatusRatio,char*GeodeticDatum,
	char*MapProjection,char*CentralMederian,char*ZoneDivisionMode,char*GaussKrugerZoneNo,char*HeightSystem,
	char*HeightDatum,char*Interpolation,const char*strDsmXmlFileName)
{
	if (0==access(strImgFName,0)&&0==access(strImgNName,0)&&0==access(strImgBName,0)) 
	{
		char strTmp[512];//temp string
		char str_FXmlPath[512],str_NXmlPath[512],str_BXmlPath[512];	
		strcpy(strTmp,strImgFName);	strcpy(strrchr(strTmp,'.'),".xml");strcpy(str_FXmlPath,strTmp);	//get *.xml
		strcpy(strTmp,strImgNName);	strcpy(strrchr(strTmp,'.'),".xml");strcpy(str_NXmlPath,strTmp);	//get *.xml
		strcpy(strTmp,strImgBName);	strcpy(strrchr(strTmp,'.'),".xml");strcpy(str_BXmlPath,strTmp);	//get *.xml
		CMarkup xml_fwd_sc,xml_nad_sc,xml_bwd_sc;//read sc
		if (xml_fwd_sc.Load(str_FXmlPath)&&xml_nad_sc.Load(str_NXmlPath)&&xml_bwd_sc.Load(str_BXmlPath))
		{
			printf("\tload sc_xml success\n");
			CMarkup xml_dsm;//write dsm
			xml_dsm.SetDoc("<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
			xml_dsm.AddElem("digital_surface_model_metadata");
			xml_dsm.IntoElem();
			{
				xml_dsm.AddElem("ProductInfo");//productInfo
				xml_dsm.IntoElem();
				{	
					strcpy(strTmp,strDsmXmlFileName);
					char *pSDsmXmlFileName1,*pSDsmXmlFileName2,*pSDsmXmlFileName;
					pSDsmXmlFileName1 = strrchr(strTmp,'\\');		
					pSDsmXmlFileName2 = strrchr(strTmp,'/');
					if(pSDsmXmlFileName1&&pSDsmXmlFileName2) 
						{
							if(pSDsmXmlFileName1>pSDsmXmlFileName2) pSDsmXmlFileName = pSDsmXmlFileName1+1;
							else pSDsmXmlFileName = pSDsmXmlFileName2+1;}
					else 
					{
						if(pSDsmXmlFileName1)pSDsmXmlFileName=pSDsmXmlFileName1+1;
						else if(pSDsmXmlFileName2) pSDsmXmlFileName=pSDsmXmlFileName2+1;else pSDsmXmlFileName=(char*)strDsmXmlFileName;}
					xml_dsm.AddElem("MetaDataFileName",pSDsmXmlFileName);//1
					xml_dsm.AddElem("ProductName","整景数字表面模型");//2
					xml_dsm.AddElem("ProductCode","empty");//3
					xml_dsm.AddElem("MapName","empty");//4
					xml_dsm.AddElem("MapNumber","empty");//5
					xml_dsm.AddElem("Scaledenominator","empty");//6
					//////////////////////////////////////////////////////////////////////////
					//add system time
					char *pS;time_t lt = time(0); 
					struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
					sprintf(strTmp, "%d",st.tm_year+1900);pS = strTmp + strlen(strTmp);
					if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
					pS = strTmp + strlen(strTmp);
					if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
					pS = strTmp + strlen(strTmp);
					xml_dsm.AddElem("ProduceDate",strTmp);//7
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.AddElem("ProductUpdate","empty");//8
					xml_dsm.AddElem("ProductVersion","empty");//9
					xml_dsm.AddElem("Owner","国家测绘地理信息局");//10

					xml_dsm.AddElem("Producer","国家测绘地理信息局卫星测绘应用中心");//11
					xml_dsm.AddElem("Publisher","国家测绘地理信息局");//12
					if(NULL==ImgSize) xml_dsm.AddElem("ImgSize","unknown");
					else xml_dsm.AddElem("ImgSize",ImgSize);//13
					xml_dsm.AddElem("DataFormat","Tif");//14
					if(NULL==GridSize) xml_dsm.AddElem("GridSize","unknown");
					else xml_dsm.AddElem("GridSize",GridSize);//15
					xml_dsm.AddElem("GridSizeUnit","米");//16
					xml_dsm.AddElem("GridArrange","由西到东、由北向南");//17
					if(NULL==GridRows) xml_dsm.AddElem("GridRows","unknown");
					else xml_dsm.AddElem("GridRows",GridRows);//18
					if(NULL==GridCols) xml_dsm.AddElem("GridCols","unknown");
					else xml_dsm.AddElem("GridCols",GridCols);//19
					xml_dsm.AddElem("PFloatNum","6");//20
					xml_dsm.AddElem("HFloatNum","6");//21

					if(NULL==CornerLongRange) xml_dsm.AddElem("CornerLongRange","unknown");
					else xml_dsm.AddElem("CornerLongRange",CornerLongRange);//22
					if(NULL==CornerLatRange) xml_dsm.AddElem("CornerLatRange","unknown");
					else xml_dsm.AddElem("CornerLatRange",CornerLatRange);//23

					if(NULL==SouthWestOrd) xml_dsm.AddElem("SouthWestOrd","unknown");
					else xml_dsm.AddElem("SouthWestOrd",SouthWestOrd);//24
					if(NULL==SouthWestAbs) xml_dsm.AddElem("SouthWestAbs","unknown");
					else xml_dsm.AddElem("SouthWestAbs",SouthWestAbs);//25
					if(NULL==NorthWestOrd)xml_dsm.AddElem("NorthWestOrd","unknown");
					else xml_dsm.AddElem("NorthWestOrd",NorthWestOrd);//26
					if(NULL==NorthWestAbs)xml_dsm.AddElem("NorthWestAbs","unknown");
					else xml_dsm.AddElem("NorthWestAbs",NorthWestAbs);//27
					if(NULL==NorthEastOrd)xml_dsm.AddElem("NorthEastOrd","unknown");
					else xml_dsm.AddElem("NorthEastOrd",NorthEastOrd);//28
					if(NULL==NorthEastAbs)xml_dsm.AddElem("NorthEastAbs","unknown");
					else xml_dsm.AddElem("NorthEastAbs",NorthEastAbs);//29
					if(NULL==SouthEastOrd)xml_dsm.AddElem("SouthEastOrd","unknown");
					else xml_dsm.AddElem("SouthEastOrd",SouthEastOrd);//30
					if(NULL==SouthEastAbs)xml_dsm.AddElem("SouthEastAbs","unknown");
					else xml_dsm.AddElem("SouthEastAbs",SouthEastAbs);//31
					xml_dsm.AddElem("ConfidentialLevel","秘密");//32

					if(NULL==LongerRadius)xml_dsm.AddElem("LongerRadius","unknown");
					else xml_dsm.AddElem("LongerRadius",LongerRadius);//33
					if(NULL==OblatusRatio)xml_dsm.AddElem("OblatusRatio","unknown");
					else xml_dsm.AddElem("OblatusRatio",OblatusRatio);//34
					if(NULL==GeodeticDatum)xml_dsm.AddElem("GeodeticDatum","unknown");
					else xml_dsm.AddElem("GeodeticDatum",GeodeticDatum);//35
					if(NULL==MapProjection)xml_dsm.AddElem("MapProjection","unknown");
					else xml_dsm.AddElem("MapProjection",MapProjection);//36

					if(NULL==CentralMederian)xml_dsm.AddElem("CentralMederian","unknown");
					else xml_dsm.AddElem("CentralMederian",CentralMederian);//37
					if(NULL==ZoneDivisionMode)xml_dsm.AddElem("ZoneDivisionMode","unknown");
					else xml_dsm.AddElem("ZoneDivisionMode",ZoneDivisionMode);//38
					if(NULL==GaussKrugerZoneNo)xml_dsm.AddElem("GaussKrugerZoneNo","unknown");
					else xml_dsm.AddElem("GaussKrugerZoneNo",GaussKrugerZoneNo);//39
					xml_dsm.AddElem("CoordinationUnit","米");//40
					if(NULL==HeightSystem)xml_dsm.AddElem("HeightSystem","unknown");
					else xml_dsm.AddElem("HeightSystem",HeightSystem);//41
					if(NULL==HeightDatum)xml_dsm.AddElem("HeightDatum","unknown");
					else xml_dsm.AddElem("HeightDatum",HeightDatum);//42
					xml_dsm.AddElem("VerticalRMS","empty");//43
					xml_dsm.AddElem("Integrity","empty");//44
					xml_dsm.AddElem("EditDegree","empty");//45
					xml_dsm.AddElem("EditExplanation","empty");//46
					xml_dsm.AddElem("LoopholeArea","empty");//47
					xml_dsm.AddElem("LoopholeReason","empty");//48
					xml_dsm.AddElem("LoopholeDescription","empty");//49
					xml_dsm.AddElem("LoopholeRepaired","empty");//50
					xml_dsm.AddElem("LoopholeRepairedDataType","empty");//51
					xml_dsm.AddElem("LoopholeRepairedDataName","empty");//52
					xml_dsm.AddElem("LoopholeRepairedMethod","empty");//53
					xml_dsm.AddElem("LoopholeRepairedWidth","empty");//54
					xml_dsm.AddElem("Conclusion","empty");//55
					xml_dsm.AddElem("QualityEvaluateDepartment","empty");//56
					xml_dsm.AddElem("DataQualityEvaluateDate","empty");//57
					xml_dsm.AddElem("DataQuality","empty");//58
				}
				xml_dsm.OutOfElem();
				xml_dsm.AddElem("dataSourceInfo");//dataSourceInfo
				xml_dsm.IntoElem();
				{
					xml_dsm.AddElem("DataSourceType","资源三号01星#三线阵传感器校正影像");//59
					//////////////////////////////////////////////////////////////////////////
					char strF[512],strB[512],strN[512];
					const char *pS1,*pS2,*pSS;
					//////////////////////////////////////////////////////////////////////////
					strcpy(strTmp,strImgFName);
					pS1 = strrchr(strTmp,'\\');		pS2 = strrchr(strTmp,'/');
					if(pS1||pS2) {if(pS1)pSS=pS1;else if(pS2)pSS=pS2;else pSS=NULL;}else {if(pS1>pS2) pSS = pS2; else pSS = pS1;}
					strcpy(strF,pSS+1);
					//////////////////////////////////////////////////////////////////////////
					strcpy(strTmp,strImgBName);
					pS1 = strrchr(strTmp,'\\');		pS2 = strrchr(strTmp,'/');
					if(pS1||pS2) {if(pS1)pSS=pS1;else if(pS2)pSS=pS2;else pSS=NULL;}else {if(pS1>pS2) pSS = pS2; else pSS = pS1;}
					strcpy(strB,pSS+1);
					//////////////////////////////////////////////////////////////////////////
					strcpy(strTmp,strImgNName);
					pS1 = strrchr(strTmp,'\\');		pS2 = strrchr(strTmp,'/');
					if(pS1||pS2) {if(pS1)pSS=pS1;else if(pS2)pSS=pS2;else pSS=NULL;}else {if(pS1>pS2) pSS = pS2; else pSS = pS1;}
					strcpy(strN,pSS+1);
					sprintf(strTmp,"像对1:%s#%s;像对2:%s#%s;像对3:%s#%s",strF,strN,strN,strB,strF,strB);
					xml_dsm.AddElem("DataSource",strTmp);//60
					//////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////
					char*strDateMax,*strDateMin,strFDate[512],strBDate[512],strNDate[512];
					xml_fwd_sc.ResetPos();xml_fwd_sc.FindElem();xml_fwd_sc.IntoElem();
					xml_bwd_sc.ResetPos();xml_bwd_sc.FindElem();xml_bwd_sc.IntoElem();
					xml_nad_sc.ResetPos();xml_nad_sc.FindElem();xml_nad_sc.IntoElem();
					if (xml_fwd_sc.FindElem("processInfo")&&xml_bwd_sc.FindElem("processInfo")&&xml_nad_sc.FindElem("processInfo"))
					{
						xml_fwd_sc.IntoElem();xml_bwd_sc.IntoElem();xml_nad_sc.IntoElem();
						{
							if (xml_fwd_sc.FindElem("ProductTime")&&xml_nad_sc.FindElem("ProductTime")&&xml_bwd_sc.FindElem("ProductTime"))
							{
								strcpy(strFDate,xml_fwd_sc.GetData().c_str());strcpy(strBDate,xml_bwd_sc.GetData().c_str());strcpy(strNDate,xml_nad_sc.GetData().c_str());
								*strrchr(strFDate,' ')=0;*strrchr(strBDate,' ')=0;*strrchr(strNDate,' ')=0;
								if(strcmp(strFDate,strNDate)){strDateMax=strFDate;strDateMin=strNDate;}
								else{strDateMax=strNDate;strDateMin=strFDate;}
								if(strcmp(strBDate,strDateMax)) strDateMax=strBDate;
								if (strcmp(strDateMin,strBDate)) strDateMin=strBDate;
								sprintf(strTmp,"%s~%s",strDateMin,strDateMax);
								xml_dsm.AddElem("DataSourceDate",strTmp);//61
							}else{xml_dsm.AddElem("DataSourceDate","unknown");printf("\tget ProductTime from sc_xml: unknown!!!\n");}
						}
						xml_fwd_sc.OutOfElem();xml_bwd_sc.OutOfElem();xml_nad_sc.OutOfElem();
					}else{xml_dsm.AddElem("DataSourceDate","unknown");printf("\tget ProductTime from sc_xml: unknown!!!\n");}
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.AddElem("DataSourceResolution","2.1-3.5");//62
					xml_dsm.AddElem("DataSourceColor","PAN");//63
					xml_dsm.AddElem("DataSourceQuality","empty");//64
					xml_dsm.AddElem("DataSourceAccuracy","empty");//65
					xml_dsm.AddElem("DataSourceElevation","empty");//66
					xml_dsm.AddElem("DataSourceRemark","empty");//67
					xml_dsm.AddElem("AssistantDataORNot","empty");//68
					xml_dsm.AddElem("AssistantDataType","empty");//69
					xml_dsm.AddElem("AssistantData","empty");//70
					xml_dsm.AddElem("AssistantDataDate","empty");//71
					xml_dsm.AddElem("AssistantDataResolution","empty");//72
					xml_dsm.AddElem("AssistantDataAccuracy","empty");//73
					xml_dsm.AddElem("AssistantDataElevation","empty");//74
					xml_dsm.AddElem("AssistantDataRemark","empty");//75
				}
				xml_dsm.OutOfElem();//dataSourceInfo//end
				xml_dsm.AddElem("processInfo");//processInfo
				xml_dsm.IntoElem();
				{
					xml_dsm.AddElem("ControlSource","empty");//76
					xml_dsm.AddElem("SateOriXRMS","empty");//77
					xml_dsm.AddElem("SateOriYRMS","empty");//78
					xml_dsm.AddElem("SateOriZRMS","empty");//79
					xml_dsm.AddElem("ATProducerName","empty");//80
					xml_dsm.AddElem("ATCheckerName","empty");//81
					xml_dsm.AddElem("ManufactureType","empty");//82

					if(NULL==Interpolation)xml_dsm.AddElem("Interpolation","unknown");
					else xml_dsm.AddElem("Interpolation",Interpolation);//83
					xml_dsm.AddElem("OrthoRectifySoftware","DPGRID.SAT");//84
					xml_dsm.AddElem("OrthoRectifyQulity","empty");//85
					xml_dsm.AddElem("OrthoRectifyName","empty");//86
					xml_dsm.AddElem("OrthoCheckName","empty");//87
					xml_dsm.AddElem("ExceptError","empty");//88
					xml_dsm.AddElem("WatersEditExplanation","empty");//89
					xml_dsm.AddElem("RiversEditExplanation","empty");//90
					xml_dsm.AddElem("ValleyEditExplanation","empty");//91
					xml_dsm.AddElem("OtherEditExplanation","empty");//92
					xml_dsm.AddElem("GeometricAccuracyEvaluatedMethod","empty");//93
					xml_dsm.AddElem("CheckPointNum","empty");//94
					xml_dsm.AddElem("CheckPointSource","empty");//95
					xml_dsm.AddElem("CheckRMS","empty");//96
					xml_dsm.AddElem("CheckMaxErr","empty");//97
					xml_dsm.AddElem("ConclusionInstitute","empty");//98
					xml_dsm.AddElem("InstituteCheckUnit","empty");//99
					xml_dsm.AddElem("InstituteCheckName","empty");//100
					xml_dsm.AddElem("InstituteCheckDate","empty");//101
					xml_dsm.AddElem("BureauCheckName","empty");//102
					xml_dsm.AddElem("BureauCheckUnit","empty");//103
					xml_dsm.AddElem("ConclusionBureau","empty");//104
					xml_dsm.AddElem("BureauCheckDate","empty");//105
				}
				xml_dsm.OutOfElem();//processInfo//end
				xml_dsm.AddElem("distributionInfo");//distributionInfo
				xml_dsm.IntoElem();
				{
					xml_dsm.AddElem("ProductPrice","empty");//106
					xml_dsm.AddElem("Productmedia","empty");//107
					xml_dsm.AddElem("Productformat","empty");//108
					xml_dsm.AddElem("DistributorPhone","empty");//109
					xml_dsm.AddElem("DistributorFaxPhone","empty");//110
					xml_dsm.AddElem("DistributorAdministratityArea","empty");//111
					xml_dsm.AddElem("DistributorAddress","empty");//112
					xml_dsm.AddElem("DistributorPostCode","empty");//113
					xml_dsm.AddElem("DistributorName","empty");//114
					xml_dsm.AddElem("DistributorEMail","empty");//115
					xml_dsm.AddElem("DistributorURL","empty");}//116	
				xml_dsm.OutOfElem();}//distributionInfo//end	
			xml_dsm.OutOfElem();
			//////////////////////////////////////////////////////////////////////////
			//access path
			strcpy(strTmp,strDsmXmlFileName);
			char *pS1,*pS2,*pS;
			pS1 = strrchr(strTmp,'\\');		pS2 = strrchr(strTmp,'/');
			if(pS1||pS2) {if(pS1)pS=pS1;else if(pS2)pS=pS2;}
			else {if(pS1>pS2) pS = pS2; else pS = pS1;}*pS = 0;
			//////////////////////////////////////////////////////////////////////////
			if(!access(strTmp,0)){
				xml_dsm.Save(strDsmXmlFileName);
				printf("\tSave xml_dsm success:%s\n",strDsmXmlFileName);}
			else{
				_mkdir(strTmp);xml_dsm.Save(strDsmXmlFileName);
				printf("\tSave xml_dsm success:%s\n",strDsmXmlFileName);}			
			return true;}
		else{printf("\timage file does not exist!!!\n");return false;}}
	else{printf("\tload sc_xml error!!!\n");return false;}
}