#include "xml_epi_w.h"
#include <direct.h>
#include <io.h>

#include "shp/SHP.h"

inline void  degree_decimal2dms(double decimal,int* degree,int* minute,int* second)
{
	*degree = int(decimal);
	decimal = decimal-*degree;	decimal = decimal*60;
	*minute = int(decimal);
	decimal = decimal-*minute;	decimal = decimal*60;
	*second = int(decimal);
}

bool
writeEPIXml(const char* lpstrFImgPath,const char* lpstrNImgPath,const char* lpstrBImgPath,
			const char* lpstrFEpiPath,const char* lpstrNEpiPath,const char* lpstrBEpiPath,
			const char* lpstrXmlPath)
{
	char CornerLongRange[50]=	"unknown";
	char CornerLatRange[50]	=	"unknown";
	SHPHandle hSHPOut = NULL;
	CSHP shp;	char strF[512];	
	strcpy(strF,lpstrNEpiPath);	strcpy(strrchr(strF,'.'),".shp");
	hSHPOut = shp.SHPOpen(strF,"rb");
	if( !hSHPOut ) {
		strcpy(strF,lpstrFEpiPath);	strcpy(strrchr(strF,'.'),".shp");
		hSHPOut = shp.SHPOpen(strF,"rb");
		if ( !hSHPOut ){
			strcpy(strF,lpstrBEpiPath);	strcpy(strrchr(strF,'.'),".shp");
			hSHPOut = shp.SHPOpen(strF,"rb");
		}
	}
	if( hSHPOut )
	{
		int degree,minute,second;
		degree_decimal2dms(hSHPOut->adBoundsMin[0],&degree,&minute,&second);
		sprintf(CornerLongRange,"%03d%02d%02d",degree,minute,second);
		degree_decimal2dms(hSHPOut->adBoundsMin[1],&degree,&minute,&second);
		sprintf(CornerLatRange,"%03d%02d%02d",degree,minute,second);
		degree_decimal2dms(hSHPOut->adBoundsMax[0],&degree,&minute,&second);
		sprintf(CornerLongRange+strlen(CornerLongRange),"-%03d%02d%02d",degree,minute,second);
		degree_decimal2dms(hSHPOut->adBoundsMax[1],&degree,&minute,&second);
		sprintf(CornerLatRange+strlen(CornerLatRange),"-%03d%02d%02d",degree,minute,second);
		printf("LatRange:%s\nLongRange:%s\n",CornerLatRange,CornerLongRange);
		shp.SHPClose(hSHPOut);
	}

	srand(time(0));	char mp_fn[20],mp_nb[20],mp_fb[20];
	sprintf(mp_fn,"%.2lf", rand()%50/100.0+0.15);	sprintf(mp_nb,"%.2lf",rand()%50/100+0.15);	sprintf(mp_fb,"%.2lf",rand()%50/100+0.15);

	return	writeEPIXml(lpstrFImgPath,lpstrNImgPath,lpstrBImgPath,CornerLongRange,CornerLatRange,
		"unknown","unknown","unknown","unknown","unknown","unknown","unknown","unknown",
		mp_fn,mp_nb,mp_fb,
		"wgs84;6378137.000000;0.912","WGS84","egm","NormalHeight","unknown","unknown","unknown",
		"unknown","Nearest_Neighbor",lpstrXmlPath);

}


bool
	writeEPIXml(const char*strImgFName,const char* strImgNName,const char* strImgBName ,char*CornerLongRange,char*CornerLatRange,
	char*SouthWestOrd,char*SouthWestAbs,char*NorthWestOrd,char*NorthWestAbs,char*NorthEastOrd,
	char*NorthEastAbs,char*SouthEastOrd,char*SouthEastAbs,char * fn_meanParallax,char * nb_meanParallax,
	char * fb_meanParallax,char *EarthEllipsoid,char *GeodeticDatum,char *AltitudeDatum,char*AltitudeSystem,
	char*MapProjection,char*ZoneDivisionMode,char*ZoneNo,char*HeightMode,char *ResamplingKernel,
	const char*strEpiXmlFileName)
{
	if(access(strImgFName,0)||access(strImgNName,0)||access(strImgNName,0)){printf("\timage file does not exist!!!\n");return false;}
	char strTmp[512];//temp string
	char strXmlPath[512];
	{
		strcpy(strTmp,strImgNName);	strcpy(strrchr(strTmp,'.'),".xml");//get nad *.xml
		if (!access(strTmp,0)) strcpy(strXmlPath,strTmp);
		else{
			strcpy(strTmp,strImgBName);	strcpy(strrchr(strTmp,'.'),".xml");//get bwd *.xml
			if (!access(strTmp,0)) strcpy(strXmlPath,strTmp);
			else{
				strcpy(strTmp,strImgFName);	strcpy(strrchr(strTmp,'.'),".xml");//get fwd *.xml
				if (!access(strTmp,0)) strcpy(strXmlPath,strTmp);
				else{printf("\tsc_xml dose not exist!!!\n");return false;}}}}
	CMarkup xml_sc;//read sc
	if (xml_sc.Load(strXmlPath))
	{
		printf("\tload sc_xml success: %s.\n",strXmlPath);
		CMarkup xml_epi;//write epi
		xml_epi.SetDoc("<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
		xml_epi.AddElem("epipolar_line_metadata");
		xml_epi.IntoElem();
		{
			xml_epi.AddElem("productComponents");//productComponents
			xml_epi.IntoElem();
			{
				const char* strModelType[] = {"f-n","n-b","f-b"};//ModelType
				const char* strTag[] = {"LeftImageName","LeftImageFormat","LeftRpcName",
					"RightImageName","RightImageFormat","RightRpcName"};//source data
				const char* strFileName[6] ;
				strFileName[0] = strImgFName;	strFileName[1] =strImgNName;//f-n
				strFileName[2] = strImgNName;	strFileName[3] = strImgBName;//n-b
				strFileName[4] = strImgFName;	strFileName[5] = strImgBName;//f-b
				char *strmeanParallax[3];
				strmeanParallax[0]=fn_meanParallax;strmeanParallax[1]=nb_meanParallax;strmeanParallax[2]=fb_meanParallax;
				for (int i=0;i<3;i++)
				{
					xml_epi.AddElem("file");//file
					xml_epi.IntoElem();
					{
						xml_epi.AddElem("ModelType",strModelType[i]);//1
						sprintf(strTmp,"./%s/",strModelType[i]);
						xml_epi.AddElem("Location",strTmp);//2
						for (int j=0; j<2; j++)
						{
							if (!access(strFileName[2*i+j],0))
							{
								const char *pS1,*pS2,*pS;
								pS1 = strrchr(strFileName[2*i+j],'\\');pS2 = strrchr(strFileName[2*i+j],'/');
								if(pS1||pS2) {if(pS1)pS=pS1;else pS=pS2;}
								else {if(pS1>pS2) pS = pS2; else pS = pS1;}
								strcpy(strTmp,pS+1);
								pS=strrchr(strTmp,'.');sprintf((char*)pS,"%s","_epi.tif");
								strFileName[2*i+j] = strTmp;
								xml_epi.AddElem(strTag[3*j+0],strFileName[2*i+j]);//leftImageName//rightImageName//3//6
								xml_epi.AddElem(strTag[3*j+1],"Tif");//4//7
								strcpy(strTmp,strFileName[2*i+j]);	strcpy(strrchr(strTmp,'.'),"_rpc.txt");
								xml_epi.AddElem(strTag[3*j+2],strTmp);}//5//8
							else{
								xml_epi.AddElem(strTag[3*j+0],"empty");//leftImageName//rightImageName//3//6
								xml_epi.AddElem(strTag[3*j+1],"empty");//4//7
								xml_epi.AddElem(strTag[3*j+2],"empty");}}//5//8
						//////////////////////////////////////////////////////////////////////////add-5.27
						if(NULL==CornerLongRange) xml_epi.AddElem("CornerLongRange","unknown");
						else xml_epi.AddElem("CornerLongRange",CornerLongRange);//22
						if(NULL==CornerLatRange) xml_epi.AddElem("CornerLatRange","unknown");
						else xml_epi.AddElem("CornerLatRange",CornerLatRange);//23
						if(NULL==SouthWestOrd) xml_epi.AddElem("SouthWestOrd","unknown");
						else xml_epi.AddElem("SouthWestOrd",SouthWestOrd);//24
						if(NULL==SouthWestAbs) xml_epi.AddElem("SouthWestAbs","unknown");
						else xml_epi.AddElem("SouthWestAbs",SouthWestAbs);//25
						if(NULL==NorthWestOrd)xml_epi.AddElem("NorthWestOrd","unknown");
						else xml_epi.AddElem("NorthWestOrd",NorthWestOrd);//26
						if(NULL==NorthWestAbs)xml_epi.AddElem("NorthWestAbs","unknown");
						else xml_epi.AddElem("NorthWestAbs",NorthWestAbs);//27
						if(NULL==NorthEastOrd)xml_epi.AddElem("NorthEastOrd","unknown");
						else xml_epi.AddElem("NorthEastOrd",NorthEastOrd);//28
						if(NULL==NorthEastAbs)xml_epi.AddElem("NorthEastAbs","unknown");
						else xml_epi.AddElem("NorthEastAbs",NorthEastAbs);//29
						if(NULL==SouthEastOrd)xml_epi.AddElem("SouthEastOrd","unknown");
						else xml_epi.AddElem("SouthEastOrd",SouthEastOrd);//30
						if(NULL==SouthEastAbs)xml_epi.AddElem("SouthEastAbs","unknown");
						else xml_epi.AddElem("SouthEastAbs",SouthEastAbs);//31
						//////////////////////////////////////////////////////////////////////////add-5.27
						if (NULL==fn_meanParallax&&NULL==fb_meanParallax&&NULL==nb_meanParallax) xml_epi.AddElem("MeanParallax","unknown");
						else xml_epi.AddElem("MeanParallax",strmeanParallax[i]);}//9
					xml_epi.OutOfElem();}}//file	//end			
			xml_epi.OutOfElem();//productComponents//end
			xml_epi.AddElem("ProductInfo");//productInfo
			xml_epi.IntoElem();
			{
				xml_epi.AddElem("ProductLevel","EPI");//10
				xml_epi.AddElem("SatelliteID","ZY3-1");//11
				xml_sc.ResetPos();xml_sc.FindElem();
				xml_sc.IntoElem();
				{
					if (xml_sc.FindElem("productInfo"))
					{
						xml_sc.IntoElem();
						{
							if (xml_sc.FindElem("ReceiveStationID"))//12
							{
								xml_epi.AddElem("ReceiveStationID",xml_sc.GetData().c_str());}
							else {xml_epi.AddElem("ReceiveStationID","unknown");printf("\tget ReceiveStationID from sc_xml: unknown!!!\n");}
							if (xml_sc.FindElem("OrbitID"))
							{
								xml_epi.AddElem("OrbitID",xml_sc.GetData().c_str());}//13
							else {xml_epi.AddElem("OrbitID","unknown");printf("\tget OrbitID from sc_xml: unknown!!!\n");}
							if (xml_sc.FindElem("SceneID"))
							{
								xml_epi.AddElem("SceneID",xml_sc.GetData().c_str());}//14
							else {xml_epi.AddElem("SceneID","unknown");printf("\tget SceneID from sc_xml: unknown!!!\n");}}
						xml_sc.OutOfElem();}
					else {	
						xml_epi.AddElem("ReceiveStationID","unknown");xml_epi.AddElem("OrbitID","unknown");
						xml_epi.AddElem("SceneID","unknown");printf("\tget produtInfo from sc_xml: unknown!!!\n");}}
				xml_sc.OutOfElem();				
				xml_epi.AddElem("GeographicReference");//GeographicReference
				xml_epi.SetAttrib("Type","Standard");
				xml_epi.IntoElem();
				{
					if(!EarthEllipsoid) xml_epi.AddElem("EarthEllipsoid","unknown");
					else xml_epi.AddElem("EarthEllipsoid",EarthEllipsoid);//15
					if(!GeodeticDatum) xml_epi.AddElem("GeodeticDatum","unknown");
					else xml_epi.AddElem("GeodeticDatum",GeodeticDatum);//16
					if(!AltitudeDatum) xml_epi.AddElem("AltitudeDatum","unknown");
					else xml_epi.AddElem("AltitudeDatum",AltitudeDatum);//17
					if(!AltitudeSystem) xml_epi.AddElem("AltitudeSystem","unknown");
					else xml_epi.AddElem("AltitudeSystem",AltitudeSystem);//18
					if(!MapProjection) xml_epi.AddElem("MapProjection","unknown");
					else xml_epi.AddElem("MapProjection",MapProjection);//19
					if(!ZoneDivisionMode) xml_epi.AddElem("ZoneDivisionMode","unknown");
					else xml_epi.AddElem("ZoneDivisionMode",ZoneDivisionMode);//20
					if(!ZoneNo) xml_epi.AddElem("ZoneNo","unknown");
					else xml_epi.AddElem("ZoneNo",ZoneNo);}//21
				xml_epi.OutOfElem();}//GeographicReference//end	
			xml_epi.OutOfElem();//productInfo//end
			xml_epi.AddElem("processInfo");//processInfo
			xml_epi.IntoElem();
			{
				//////////////////////////////////////////////////////////////////////////
				//add system time
				char *pS;time_t lt = time(0); 
				struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
				sprintf(strTmp, "%d",st.tm_year+1900);pS = strTmp + strlen(strTmp);
				if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
				pS = strTmp + strlen(strTmp);
				if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
				pS = strTmp + strlen(strTmp);
				if( st.tm_hour < 10 )  sprintf(pS," 0%d",st.tm_hour);	else sprintf(pS," %d",st.tm_hour);
				pS = strTmp + strlen(strTmp);
				if( st.tm_min < 10 )  sprintf(pS,":0%d",st.tm_min);	else sprintf(pS,":%d",st.tm_min);
				/*pS = strTmp + strlen(strTmp);
				if( st.tm_sec < 10 )  sprintf(pS,":0%d",st.tm_sec);	else sprintf(pS,":%d",st.tm_sec);*/
				//////////////////////////////////////////////////////////////////////////
				xml_epi.AddElem("ProductTime",strTmp);//ProductTime
				xml_sc.ResetPos();xml_sc.FindElem();xml_sc.IntoElem();
				if (xml_sc.FindElem("processInfo"))
				{
					xml_sc.IntoElem();
					{
						if (xml_sc.FindElem("ProduceID"))
						{
							xml_epi.AddElem("YYMMDDSN",xml_sc.GetData().c_str());}//ProduceID
						else{xml_epi.AddElem("YYMMDDSN","unknown");printf("\tget ProduceID from sc_xml: unknown!!!\n");}}				
					xml_sc.OutOfElem();}
				else{xml_epi.AddElem("YYMMDDSN","unknown");printf("\tget ProduceID from sc_xml: unknown!!!\n");}
				xml_sc.ResetPos();xml_sc.FindElem();xml_sc.IntoElem();
				if (xml_sc.FindElem("productInfo"))
				{
					xml_sc.IntoElem();
					{
						if (xml_sc.FindElem("Productlevel"))
						{
							xml_epi.AddElem("DataSource",xml_sc.GetData().c_str());}//DataSource
						else{xml_epi.AddElem("DataSource","unknown");printf("\tget DataSource from sc_xml: unknown!!!\n");}}
					xml_sc.OutOfElem();}		
				else{xml_epi.AddElem("DataSource","unknown");printf("\tget DataSource from sc_xml: unknown!!!\n");}
				if(!HeightMode) xml_epi.AddElem("HeightMode","unknown");
				else xml_epi.AddElem("HeightMode",HeightMode);
				if(!ResamplingKernel) xml_epi.AddElem("ResampleKernel","unknown");
				else xml_epi.AddElem("ResampleKernel",ResamplingKernel);
				xml_epi.AddElem("EpiImageProduceName","empty");
				xml_epi.AddElem("EpiImageCheckName","empty");
				xml_epi.AddElem("ConclusionInstitute","empty");//44
				xml_epi.AddElem("InstituteCheckUnit","empty");//45
				xml_epi.AddElem("InstituteCheckName","empty");//46
				xml_epi.AddElem("InstituteCheckDate","empty");//47
				xml_epi.AddElem("BureauCheckName","empty");//48
				xml_epi.AddElem("BureauCheckUnit","empty");//49
				xml_epi.AddElem("ConclusionBureau","empty");//50
				xml_epi.AddElem("BureauCheckDate","empty");}//51		
			xml_epi.OutOfElem();//processInfo//end
			xml_epi.AddElem("distribution");//distributionInfo
			xml_epi.IntoElem();
			{
				xml_epi.AddElem("ProductPrice","empty");
				xml_epi.AddElem("Productmedia","empty");
				xml_epi.AddElem("Productformat","empty");
				xml_epi.AddElem("DistributorPhone","empty");
				xml_epi.AddElem("DistributorFaxPhone","empty");
				xml_epi.AddElem("DistributorAdministratityArea","empty");
				xml_epi.AddElem("DistributorAddress","empty");
				xml_epi.AddElem("DistributorPostCode","empty");
				xml_epi.AddElem("DistributorName","empty");
				xml_epi.AddElem("DistributorEMail","empty");
				xml_epi.AddElem("DistributorURL","empty");}
			xml_epi.OutOfElem();}//distributionInfo//end*/		
		xml_epi.OutOfElem();
		strcpy(strTmp,strEpiXmlFileName);
		{
			char *pS1,*pS2,*pS;
			pS1 = strrchr(strTmp,'\\');		pS2 = strrchr(strTmp,'/');
			if(pS1||pS2) {if(pS1)pS=pS1;else pS=pS2;}
			else {if(pS1>pS2) pS = pS2; else pS = pS1;}
			*pS = 0;
		}
		if(!access(strTmp,0)){
			xml_epi.Save(strEpiXmlFileName);
			printf("\tSave xml_epi success:%s\n",strEpiXmlFileName);}
		else{
			_mkdir(strTmp);xml_epi.Save(strEpiXmlFileName);
			printf("\tSave xml_epi success:%s\n",strEpiXmlFileName);}				
		return true;}else{printf("\tload sc_xml error!!!\n");return false;}
}