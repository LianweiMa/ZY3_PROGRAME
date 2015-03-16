#include "xml_dem_w.h"
#include <direct.h>
#include<io.h>
bool
	writeDEMXml(const char*strDsmXmlFileName,char*strDemXmlFileName)
{
	if (0==access(strDsmXmlFileName,0)) 
	{
		char strTmp[512];//temp string
		CMarkup xml_dsm;//read sc
		if (xml_dsm.Load(strDsmXmlFileName))
		{
			printf("\tload dsm_xml success\n");
			CMarkup xml_dem;//write dem
			xml_dem.SetDoc("<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
			xml_dem.AddElem("digital_elevation_model_metadata");
			xml_dem.IntoElem();
			{
				xml_dem.AddElem("ProductInfo");//productInfo
				xml_dem.IntoElem();
				{	
					strcpy(strTmp,strDemXmlFileName);
					char *pSDemXmlFileName1,*pSDemXmlFileName2,*pSDemXmlFileName;
					pSDemXmlFileName1 = strrchr(strTmp,'\\');		
					pSDemXmlFileName2 = strrchr(strTmp,'/');
					if(pSDemXmlFileName1&&pSDemXmlFileName2) 
					{
						if(pSDemXmlFileName1>pSDemXmlFileName2) pSDemXmlFileName = pSDemXmlFileName1+1;
						else pSDemXmlFileName = pSDemXmlFileName2+1;}
					else 
					{
						if(pSDemXmlFileName1)pSDemXmlFileName=pSDemXmlFileName1+1;
						else if(pSDemXmlFileName2) pSDemXmlFileName=pSDemXmlFileName2+1;else pSDemXmlFileName=strDemXmlFileName;}
					xml_dem.AddElem("MetaDataFileName",pSDemXmlFileName);//1
					xml_dem.AddElem("ProductName","整景数字高程模型");//2
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.ResetPos();xml_dsm.FindElem();
					xml_dsm.IntoElem();
					{
						if (xml_dsm.FindElem("ProductInfo"))
						{
							xml_dsm.IntoElem();
							{
								if (xml_dsm.FindElem("ProductCode"))xml_dem.AddElem("ProductCode",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("ProductCode","empty");//3
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("MapName"))xml_dem.AddElem("MapName",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("MapName","empty");//4
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("MapNumber"))xml_dem.AddElem("MapNumber",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("MapNumber","empty");//5
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("Scaledenominator"))xml_dem.AddElem("Scaledenominator",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("Scaledenominator","empty");//6
							}
							xml_dsm.OutOfElem();
						}
						else
						{
							xml_dem.AddElem("ProductCode","empty");//3
							xml_dem.AddElem("MapName","empty");//4
							xml_dem.AddElem("MapNumber","empty");//5
							xml_dem.AddElem("Scaledenominator","empty");//6
						}	
					}xml_dsm.OutOfElem();
					//////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////
					//add system time
					char *pS;time_t lt = time(0); 
					struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
					sprintf(strTmp, "%d",st.tm_year+1900);pS = strTmp + strlen(strTmp);
					if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
					pS = strTmp + strlen(strTmp);
					if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
					pS = strTmp + strlen(strTmp);
					xml_dem.AddElem("ProduceDate",strTmp);//7
					//////////////////////////////////////////////////////////////////////////
					xml_dem.AddElem("ProductUpdate","empty");//8
					xml_dem.AddElem("ProductVersion","empty");//9
					xml_dem.AddElem("Owner","国家测绘地理信息局");//10
					xml_dem.AddElem("Producer","国家测绘地理信息局卫星测绘应用中心");//11
					xml_dem.AddElem("Publisher","国家测绘地理信息局");//12
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.ResetPos();xml_dsm.FindElem();
					xml_dsm.IntoElem();
					{
						if (xml_dsm.FindElem("ProductInfo"))
						{
							xml_dsm.IntoElem();
							if (xml_dsm.FindElem("ImgSize"))xml_dem.AddElem("ImgSize",xml_dsm.GetData().c_str());
							else xml_dem.AddElem("ImgSize","unknown");//13
							xml_dsm.OutOfElem();
						}
						else
							xml_dem.AddElem("ImgSize","unknown");//13
					}xml_dsm.OutOfElem();
					//////////////////////////////////////////////////////////////////////////
					xml_dem.AddElem("DataFormat","Tif");//14
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.ResetPos();xml_dsm.FindElem();
					xml_dsm.IntoElem();
					{
						if (xml_dsm.FindElem("ProductInfo"))
						{
							xml_dsm.IntoElem();
							if (xml_dsm.FindElem("GridSize"))xml_dem.AddElem("GridSize",xml_dsm.GetData().c_str());
							else xml_dem.AddElem("GridSize","unknown");//15
							xml_dsm.OutOfElem();
						}
						else
							xml_dem.AddElem("GridSize","unknown");//15
					}xml_dsm.OutOfElem();
					//////////////////////////////////////////////////////////////////////////
					xml_dem.AddElem("GridSizeUnit","米");//16
					xml_dem.AddElem("GridArrange","由西到东、由北向南");//17
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.ResetPos();xml_dsm.FindElem();
					xml_dsm.IntoElem();
					{
						if (xml_dsm.FindElem("ProductInfo"))
						{
							xml_dsm.IntoElem();
							{
								if (xml_dsm.FindElem("GridRows"))xml_dem.AddElem("GridRows",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("GridRows","unknown");//18
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("GridCols"))xml_dem.AddElem("GridCols",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("GridCols","unknown");//19
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("PFloatNum"))xml_dem.AddElem("PFloatNum",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("PFloatNum","unknown");//20
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("HFloatNum"))xml_dem.AddElem("HFloatNum",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("HFloatNum","unknown");//21
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("CornerLongRange"))xml_dem.AddElem("CornerLongRange",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("CornerLongRange","unknown");//22
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("CornerLatRange"))xml_dem.AddElem("CornerLatRange",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("CornerLatRange","unknown");//23
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("SouthWestOrd"))xml_dem.AddElem("SouthWestOrd",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("SouthWestOrd","unknown");//24
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("SouthWestAbs"))xml_dem.AddElem("SouthWestAbs",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("SouthWestAbs","unknown");//25
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("NorthWestOrd"))xml_dem.AddElem("NorthWestOrd",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("NorthWestOrd","unknown");//26
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("NorthWestAbs"))xml_dem.AddElem("NorthWestAbs",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("NorthWestAbs","unknown");//27
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("NorthEastOrd"))xml_dem.AddElem("NorthEastOrd",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("NorthEastOrd","unknown");//28
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("NorthEastAbs"))xml_dem.AddElem("NorthEastAbs",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("NorthEastAbs","unknown");//29
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("SouthEastOrd"))xml_dem.AddElem("SouthEastOrd",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("SouthEastOrd","unknown");//30
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("SouthEastAbs"))xml_dem.AddElem("SouthEastAbs",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("SouthEastAbs","unknown");//31
								xml_dsm.ResetMainPos();
							}
							xml_dsm.OutOfElem();
						}
						else
						{
							xml_dem.AddElem("GridRows","unknown");//18
							xml_dem.AddElem("GridCols","unknown");//19
							xml_dem.AddElem("PFloatNum","unknown");//20
							xml_dem.AddElem("HFloatNum","unknown");//21
							xml_dem.AddElem("CornerLongRange","unknown");//22
							xml_dem.AddElem("CornerLatRange","unknown");//23
							xml_dem.AddElem("SouthWestOrd","unknown");//24
							xml_dem.AddElem("SouthWestAbs","unknown");//25
							xml_dem.AddElem("NorthWestOrd","unknown");//26
							xml_dem.AddElem("NorthWestAbs","unknown");//27
							xml_dem.AddElem("NorthEastOrd","unknown");//28
							xml_dem.AddElem("NorthEastAbs","unknown");//29
							xml_dem.AddElem("SouthEastOrd","unknown");//30
							xml_dem.AddElem("SouthEastAbs","unknown");//31
						}
					}xml_dsm.OutOfElem();
					//////////////////////////////////////////////////////////////////////////
					xml_dem.AddElem("ConfidentialLevel","秘密");//32
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.ResetPos();xml_dsm.FindElem();
					xml_dsm.IntoElem();
					{
						if (xml_dsm.FindElem("ProductInfo"))
						{
							xml_dsm.IntoElem();
							{
								if (xml_dsm.FindElem("LongerRadius"))xml_dem.AddElem("LongerRadius",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("LongerRadius","unknown");//33
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("OblatusRatio"))xml_dem.AddElem("OblatusRatio",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("OblatusRatio","unknown");//34
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("GeodeticDatum"))xml_dem.AddElem("GeodeticDatum",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("GeodeticDatum","unknown");//35
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("MapProjection"))xml_dem.AddElem("MapProjection",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("MapProjection","unknown");//36
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("CentralMederian"))xml_dem.AddElem("CentralMederian",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("CentralMederian","unknown");//37
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("ZoneDivisionMode"))xml_dem.AddElem("ZoneDivisionMode",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("ZoneDivisionMode","unknown");//38
								xml_dsm.ResetMainPos();
								if (xml_dsm.FindElem("GaussKrugerZoneNo"))xml_dem.AddElem("GaussKrugerZoneNo",xml_dsm.GetData().c_str());
								else xml_dem.AddElem("GaussKrugerZoneNo","unknown");//39
								xml_dsm.ResetMainPos();
							}
							xml_dsm.OutOfElem();
						}
						else
						{
							xml_dem.AddElem("LongerRadius","unknown");//33
							xml_dem.AddElem("OblatusRatio","unknown");//34
							xml_dem.AddElem("GeodeticDatum","unknown");//35
							xml_dem.AddElem("MapProjection","unknown");//36
							xml_dem.AddElem("CentralMederian","unknown");//37
							xml_dem.AddElem("ZoneDivisionMode","unknown");//38
							xml_dem.AddElem("GaussKrugerZoneNo","unknown");//39
						}
					}xml_dsm.OutOfElem();
					//////////////////////////////////////////////////////////////////////////
					xml_dem.AddElem("CoordinationUnit","米");//40
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.ResetPos();xml_dsm.FindElem();
					xml_dsm.IntoElem();
					{
						if (xml_dsm.FindElem("ProductInfo"))
						{
							xml_dsm.IntoElem();
							if (xml_dsm.FindElem("HeightSystem"))xml_dem.AddElem("HeightSystem",xml_dsm.GetData().c_str());
							else xml_dem.AddElem("HeightSystem","unknown");//41
							xml_dsm.ResetMainPos();
							if (xml_dsm.FindElem("HeightDatum"))xml_dem.AddElem("HeightDatum",xml_dsm.GetData().c_str());
							else xml_dem.AddElem("HeightDatum","unknown");//42
							xml_dsm.ResetMainPos();
							xml_dsm.OutOfElem();
						}
						else
						{
							xml_dem.AddElem("HeightSystem","unknown");//41
							xml_dem.AddElem("HeightDatum","unknown");//42
						}
					}xml_dsm.OutOfElem();
					//////////////////////////////////////////////////////////////////////////
					xml_dem.AddElem("VerticalRMS","empty");//43
					xml_dem.AddElem("Integrity","empty");//44
					xml_dem.AddElem("EditDegree","empty");//45
					xml_dem.AddElem("EditExplanation","empty");//46
					xml_dem.AddElem("LoopholeArea","empty");//47
					xml_dem.AddElem("LoopholeReason","empty");//48
					xml_dem.AddElem("LoopholeDescription","empty");//49
					xml_dem.AddElem("LoopholeRepaired","empty");//50
					xml_dem.AddElem("LoopholeRepairedDataType","empty");//51
					xml_dem.AddElem("LoopholeRepairedDataName","empty");//52
					xml_dem.AddElem("LoopholeRepairedMethod","empty");//53
					xml_dem.AddElem("LoopholeRepairedWidth","empty");//54
					xml_dem.AddElem("Conclusion","empty");//55
					xml_dem.AddElem("QualityEvaluateDepartment","empty");//56
					xml_dem.AddElem("DataQualityEvaluateDate","empty");//57
					xml_dem.AddElem("DataQuality","empty");//58
				}
				xml_dem.OutOfElem();
				xml_dem.AddElem("dataSourceInfo");//dataSourceInfo
				xml_dem.IntoElem();
				{
					xml_dem.AddElem("DataSourceType","资源三号01星#整景数字表面模型");//59					
					xml_dem.AddElem("DataSource","整景数字表面模型");//60
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.ResetPos();xml_dsm.FindElem();
					xml_dsm.IntoElem();
					{
						if (xml_dsm.FindElem("ProductInfo"))
						{
							xml_dsm.IntoElem();
							if (xml_dsm.FindElem("ProduceDate"))xml_dem.AddElem("DataSourceDate",xml_dsm.GetData().c_str());
							else xml_dem.AddElem("DataSourceDate","unknown");//61
							xml_dsm.OutOfElem();
						}
						else
							xml_dem.AddElem("DataSourceDate","unknown");//61
					}xml_dsm.OutOfElem();
					//////////////////////////////////////////////////////////////////////////
					xml_dem.AddElem("DataSourceResolution","2.1-3.5");//62
					xml_dem.AddElem("DataSourceColor","全色");//63
					xml_dem.AddElem("DataSourceQuality","empty");//64
					xml_dem.AddElem("DataSourceAccuracy","empty");//65
					xml_dem.AddElem("DataSourceElevation","empty");//66
					xml_dem.AddElem("DataSourceRemark","empty");//67
					xml_dem.AddElem("AssistantDataORNot","empty");//68
					xml_dem.AddElem("AssistantDataType","empty");//69
					xml_dem.AddElem("AssistantData","empty");//70
					xml_dem.AddElem("AssistantDataDate","empty");//71
					xml_dem.AddElem("AssistantDataResolution","empty");//72
					xml_dem.AddElem("AssistantDataAccuracy","empty");//73
					xml_dem.AddElem("AssistantDataElevation","empty");//74
					xml_dem.AddElem("AssistantDataRemark","empty");//75
				}
				xml_dem.OutOfElem();//dataSourceInfo//end
				xml_dem.AddElem("processInfo");//processInfo
				xml_dem.IntoElem();
				{
					xml_dem.AddElem("ControlSource","empty");//76
					xml_dem.AddElem("SateOriXRMS","empty");//77
					xml_dem.AddElem("SateOriYRMS","empty");//78
					xml_dem.AddElem("SateOriZRMS","empty");//79
					xml_dem.AddElem("ATProducerName","empty");//80
					xml_dem.AddElem("ATCheckerName","empty");//81
					xml_dem.AddElem("ManufactureType","empty");//82
					//////////////////////////////////////////////////////////////////////////
					xml_dsm.ResetPos();xml_dsm.FindElem();
					xml_dsm.IntoElem();
					{
						if (xml_dsm.FindElem("dataSourceInfo"))
						{
							xml_dsm.IntoElem();
							if (xml_dsm.FindElem("Interpolation"))xml_dem.AddElem("Interpolation",xml_dsm.GetData().c_str());
							else xml_dem.AddElem("Interpolation","unknown");//83
							xml_dsm.OutOfElem();
						}
						else
							xml_dem.AddElem("Interpolation","unknown");//83
					}xml_dsm.OutOfElem();
					//////////////////////////////////////////////////////////////////////////
					xml_dem.AddElem("OrthoRectifySoftware","DPGRID.SAT");//84
					xml_dem.AddElem("OrthoRectifyQulity","empty");//85
					xml_dem.AddElem("OrthoRectifyName","empty");//86
					xml_dem.AddElem("OrthoCheckName","empty");//87
					xml_dem.AddElem("ExceptError","empty");//88
					xml_dem.AddElem("WatersEditExplanation","empty");//89
					xml_dem.AddElem("RiversEditExplanation","empty");//90
					xml_dem.AddElem("ValleyEditExplanation","empty");//91
					xml_dem.AddElem("OtherEditExplanation","empty");//92
					xml_dem.AddElem("GeometricAccuracyEvaluatedMethod","empty");//93
					xml_dem.AddElem("CheckPointNum","empty");//94
					xml_dem.AddElem("CheckPointSource","empty");//95
					xml_dem.AddElem("CheckRMS","empty");//96
					xml_dem.AddElem("CheckMaxErr","empty");//97
					xml_dem.AddElem("ConclusionInstitute","empty");//98
					xml_dem.AddElem("InstituteCheckUnit","empty");//99
					xml_dem.AddElem("InstituteCheckName","empty");//100
					xml_dem.AddElem("InstituteCheckDate","empty");//101
					xml_dem.AddElem("BureauCheckName","empty");//102
					xml_dem.AddElem("BureauCheckUnit","empty");//103
					xml_dem.AddElem("ConclusionBureau","empty");//104
					xml_dem.AddElem("BureauCheckDate","empty");//105
				}
				xml_dem.OutOfElem();//processInfo//end
				xml_dem.AddElem("distributionInfo");//distributionInfo
				xml_dem.IntoElem();
				{
					xml_dem.AddElem("ProductPrice","empty");//106
					xml_dem.AddElem("Productmedia","empty");//107
					xml_dem.AddElem("Productformat","empty");//108
					xml_dem.AddElem("DistributorPhone","empty");//109
					xml_dem.AddElem("DistributorFaxPhone","empty");//110
					xml_dem.AddElem("DistributorAdministratityArea","empty");//111
					xml_dem.AddElem("DistributorAddress","empty");//112
					xml_dem.AddElem("DistributorPostCode","empty");//113
					xml_dem.AddElem("DistributorName","empty");//114
					xml_dem.AddElem("DistributorEMail","empty");//115
					xml_dem.AddElem("DistributorURL","empty");}//116	
				xml_dem.OutOfElem();}//distributionInfo//end	
			xml_dem.OutOfElem();
			//////////////////////////////////////////////////////////////////////////
			//access path
			strcpy(strTmp,strDemXmlFileName);
			char *pS1,*pS2,*pS;
			pS1 = strrchr(strTmp,'\\');		pS2 = strrchr(strTmp,'/');
			if(pS1||pS2) {if(pS1)pS=pS1;else if(pS2)pS=pS2;}
			else {if(pS1>pS2) pS = pS2; else pS = pS1;}*pS = 0;
			//////////////////////////////////////////////////////////////////////////
			if(!access(strTmp,0)){
				xml_dem.Save(strDemXmlFileName);
				printf("\tSave dem_xml success:%s\n",strDemXmlFileName);}
			else{
				_mkdir(strTmp);xml_dem.Save(strDemXmlFileName);
				printf("\tSave dem_xml success:%s\n",strDemXmlFileName);}			
			return true;}
		else{printf("\timage file does not exist!!!\n");return false;}}
	else{printf("\tload dsm_xml error!!!\n");return false;}
}