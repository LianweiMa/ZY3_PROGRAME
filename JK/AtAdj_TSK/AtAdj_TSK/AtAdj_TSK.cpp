// AtAdj_TSK.cpp : Defines the entry point for the console application.
//

#include "XmlPrj.h"
#include "MipAdjTsk.hpp"
#include "LxString.hpp"

using namespace Wanyi; 

void Usage(){
	printf( 
        "%s", 
        "Usage: AtAdj_TSK xmlPath [DemPath]\n"
        "\n"
        " xmlPath: the full path of xml file.\n"
        " DemPath: the full path of DemSource file.\n");
	
    exit( 1 );
}

int main(int argc, char* argv[])
{
	if(argc<2){
		Usage();
	}
	else{
		char strExe[512];  strcpy(strExe,argv[0]);	Dos2Unix(strExe);
		char strXml[512];  strcpy(strXml,argv[1]);
		char strLog[512];	char* pS;	char* pT;
		const char* lpDemPath = NULL;
		strcpy(strLog,strXml);
		pS = strrchr(strLog,'.');	strcpy(pS,"_adj.log");

		if(argc>2){
			lpDemPath = argv[2];
		}
		
		OpenLog(strLog);
		LogPrint(ERR_NONE,"Running:%s %s %s",strExe,strXml,strLog);
		
		char	strWorkDir[512],strXmlNam[128],strTsk[512];	strcpy(strWorkDir,strXml);	Dos2Unix(strWorkDir);
		pS = strrchr(strWorkDir,'/');	strcpy(strXmlNam,pS);	
		strcpy(pS,"/AT/ADJ/TSK");		strcpy(strTsk,strWorkDir);
		CreateDir(strTsk);

		///////////////////////////定义解析xml的实体
		CXmlPrj xml;	
		if( xml.LoadPrj(strXml) != RET_OK )
		{
			LogPrint(ERR_FILE_OPEN,"Can't Open %s!",strXml);
			return -1;
		}
		LogPrint(ERR_NONE,"Open %s Successfully!",strXml);

		char strF[512],strImgSzPath[512],strKnlConfig[512];
		if( !xml.m_sMchDir[0] ) return -1;

		strcpy(strImgSzPath,xml.m_sMchDir);
		pT = AddEndSlash(strImgSzPath);	strcpy(pT,strXmlNam);	strcpy(strrchr(pT,'.'),".sz");

		int ellipse_gcp = ControlInfo::WGS84;
		int heisys_gcp	= ControlInfo::NORMAL;
		int source_dem	= ControlInfo::SRTM;
		int heisys_dem	= ControlInfo::NORMAL;
		enum SR_Tpye{ SR_NO=0,SR_PAN,SR_MUX}  resectiontype;
		resectiontype = SR_NO;

		int model_pan = 1;
		enum { SAT_ZY3,SAT_JB,SAT_WFV} model_sat = SAT_ZY3;
		{
			char strTmp[200];
			strcpy(strF,argv[0]);	strcpy(strrchr(strF,'.'),".ini");	LogPrint(0,"\nConf= %s",strF);
			::GetPrivateProfileString("GcpInfo","Enum_Ellipsoid","{ WGS84 = 0,	LOCAL = 1 }",strTmp,200,strF);		LogPrint(0,"%s",strTmp);
			::GetPrivateProfileString("GcpInfo","Ellipsoid","0",strTmp,200,strF);		ellipse_gcp = atoi(strTmp);	LogPrint(0,"Ellipsoid= %d",ellipse_gcp);
			::GetPrivateProfileString("GcpInfo","Enum_HeightSystem","{ GEODETIC = 0, NORMAL = 1 }",strTmp,200,strF);		LogPrint(0,"%s",strTmp);
			::GetPrivateProfileString("GcpInfo","HeightSystem","0",strTmp,200,strF);	heisys_gcp = atoi(strTmp);	LogPrint(0,"HeightSystem= %d",heisys_gcp);
			
//			::GetPrivateProfileString("DemInfo","Enum_Source","{ SRTM  = 0, OTHER = 1 }",strTmp,200,strF);			LogPrint(0,"%s",strTmp);
//			::GetPrivateProfileString("DemInfo","Source","0",strTmp,200,strF);		source_dem = atoi(strTmp);		LogPrint(0,"Source= %d",source_dem);
			if(lpDemPath) source_dem = ControlInfo::OTHER;
			::GetPrivateProfileString("DemInfo","Enum_HeightSystem","{ GEODETIC = 0, NORMAL = 1 }",strTmp,200,strF);		LogPrint(0,"%s",strTmp);
			::GetPrivateProfileString("DemInfo","HeightSystem","0",strTmp,200,strF);	heisys_dem = atoi(strTmp);	LogPrint(0,"HeightSystem= %d",heisys_dem);
		
			::GetPrivateProfileString("ModelInfo","Enum","{ UNDO = 0, 6Pars = 1 8Pars = 2 ReSolve = 3}",strTmp,200,strF);		LogPrint(0,"%s",strTmp);
			::GetPrivateProfileString("ModelInfo","Pan","1",strTmp,200,strF);	model_pan = atoi(strTmp);	LogPrint(0,"Model_Pan= %d",model_pan);
			::GetPrivateProfileString("ModelInfo","Sat","zy3",strTmp,200,strF);	
			if( !strcmp(strTmp,"jb") || !strcmp(strTmp,"JB") ) model_sat = SAT_JB;
			else if( !strcmp(strTmp,"wfv") || !strcmp(strTmp,"WFV") ) model_sat = SAT_WFV;
			LogPrint(0,"Model_SAT= %s",model_sat==SAT_JB?"JB":(model_sat==SAT_WFV?"WFV":"ZY3"));

			::GetPrivateProfileString("PrjInfo","SpaceResection","0",strTmp,200,strF);	
			if( strTmp[0] == 'p' || strTmp[0] == 'P' ) resectiontype = SR_PAN;
			else if( strTmp[0] == 'm' || strTmp[0] == 'M' ) resectiontype = SR_MUX;
			else resectiontype = SR_NO;

			const char* strSRTitle[] = {"no","Pan","Mux"};
			LogPrint(0,"SpaceResection = %s\n",strSRTitle[resectiontype]);

		}
		
		char strOutPath[512];	strcpy(strOutPath,xml.m_sMchDir);	pT = AddEndSlash(strOutPath);	
		strcat(strTsk,"/");
		MipAdjTskWriter nadTaskWriter;
		//然后我们开始组织多光谱的工程，
		//只有一点需要注意：控制点需要换成纯下视平差得到的物方点文件，
		//因为多光谱是做的后方交会！！！切记
		MipAdjTskWriter muxTskWriter;

		nadTaskWriter.SetTaskInfo(
			strTsk,	//工程目录
			"adj_nad",															//工程名称
			true);																	//是否将纠正参数归算到RPC参数中
		
		//设置平差统计结果的输出位置
		strcpy(pT+1,"PAN");	CreateDir(strOutPath);	nadTaskWriter.pi.projPath = strOutPath;
		char strMuxGcp[512];	sprintf(strMuxGcp,"%s/GroundCoordinates.txt",strOutPath);
		LogPrint(0,"Set Pan Task Info:[%s,adj_nad,%s]",strTsk,xml.m_sMchDir);

		muxTskWriter.SetTaskInfo(
			strTsk,
			"adj_mux",		//注意跟上面不一样
			true);				//注意！，此处必须为true，多光谱8个参数，必须吃到RPC里面去！！！
		
		strcpy(pT+1,"MUX");	CreateDir(strOutPath);	muxTskWriter.pi.projPath = strOutPath;	//注意跟上面不一样
		LogPrint(0,"Set MUX Task Info:[%s,adj_mux]",strTsk,strTsk);

		strcpy(strF,xml.m_sGCPPN);	strcpy(strrchr(strF,'_'),".icp");
		if( !IsExist(strF) ) { FILE* fp = fopen(strF,"w");	if(fp) fclose(fp);	}
		{
			char strLocalGcd[512],strWgs84Gcd[512],strPar7Gcd[512];
			strcpy(strWgs84Gcd,strExe);		strcpy(strrchr(strWgs84Gcd,'/'),"/wgs84.gcd");
			strcpy(pS,"/GcpGeoSys.gcd");	strcpy(strLocalGcd,strWorkDir);
			strcpy(pS,"/Geocentric7.gcd");	strcpy(strPar7Gcd,strWorkDir);


			nadTaskWriter.ci.SetGcpInfo(
				xml.m_sGCPPN,		//此处加入控制点文件的绝对路径
				strF,		//检查点文件的绝对路径
				(ControlInfo::GCPTYPE)ellipse_gcp,	//此处只能填WGS84和LOCAL，这两个枚举变量，如果是LOCAL则需在下面给三个gcd文件，如果是WGS84，则不需要
				(ControlInfo::HEITYPE)heisys_gcp,//上述控制点高程值所属的系统，此处只能填NORMAL和GEODETIC两个枚举变量，NORMAL表示正常高，GEODETIC表示大地高
				strLocalGcd,		//三个GCD，只有控制点坐标是LOCAL时，才需要加入
				strWgs84Gcd,		//
				strPar7Gcd,			//
				"");				//用户自定义的EGM文件，不用给，给了软件也不用
			LogPrint(0,"Set Pan Gcp Info:[%s,%s]",xml.m_sGCPPN,strF);

			nadTaskWriter.ci.SetDemInfo(
				(ControlInfo::DEMTYPE)source_dem,	//此处只能填SRTM和OTHER，这两个枚举变量，如果是SRTM则默认使用内置的SRTM，如果是OTHER，则使用给入的.DEM或者.BEM格式的dem文件
				//如果使用用户指定的DEM，则需要给出对应坐标系的三个GCD文件
				(ControlInfo::HEITYPE)heisys_dem,//DEM的高程系统，此处只能填NORMAL和GEODETIC两个枚举变量，NORMAL表示正常高，GEODETIC表示大地高
				lpDemPath?lpDemPath:""	   ,	//如果上面给的是OTHER，则输入文件绝对路径
				strLocalGcd,	//DEM自身坐标系的的三个GCD文件
				strWgs84Gcd,	//
				strPar7Gcd,		//
				""					//用户自定义的EGM文件，不用给，给了软件也不用
			);
			LogPrint(0,"Set Pan Dem Info:[%s]",lpDemPath?lpDemPath:"");

			muxTskWriter.ci.SetGcpInfo(
				strMuxGcp,		//注意，加入的控制点是通过下视影像的平差得到的，
				//其相对路径为$(nadproj)\GroundCoordinates.txt
				strF,		
				ControlInfo::WGS84,																						//其坐标系为WGS84
				ControlInfo::GEODETIC,																					//其高程系为大地高
				"",																										//下面的gcd文件都不给！当然给了也没用
				"",		
				"",			
				"");				
			LogPrint(0,"Set MUX Gcp Info:[%s,%s]",strMuxGcp,strF);
				
			muxTskWriter.ci.SetDemInfo(
				(ControlInfo::DEMTYPE)source_dem,					//这些都照旧
				(ControlInfo::HEITYPE)heisys_dem,	
				"",							//这些都不用给
				"",							//		
				"",		
				"",
				""
			);
			LogPrint(0,"Set MUX Dem Info:[ ]");
		}
		
		int i, num_pan =0 ,num_mux = 0,num = xml.m_inNum;
		ImgInfo* pImgInf=xml.m_inImg;	
		for ( i=0; i<num ; i++,pImgInf++){
			if( pImgInf->imgAttrib == IT_MUX ) num_mux++;
			else if( pImgInf->imgAttrib==IT_BWD || pImgInf->imgAttrib==IT_FWD || pImgInf->imgAttrib==IT_NAD ) num_pan++;
		}
		LogPrint(0,"numofpan:%d\tmunofmux:%d",num_pan,num_mux);

		if( resectiontype==SR_NO  ) { nadTaskWriter.img.mdl.resize(num_pan+num_mux); muxTskWriter.img.mdl.resize(num_mux); }else
		if( resectiontype==SR_PAN ) { nadTaskWriter.img.mdl.resize(num_pan); muxTskWriter.img.mdl.resize(num_pan+num_mux); }else
		if( resectiontype==SR_MUX ) { nadTaskWriter.img.mdl.resize(num_pan); muxTskWriter.img.mdl.resize(num_mux); }
			

		nadTaskWriter.img.SetImgPointCoord(ImgTskList::LEFTDOWN);			//只要是熊博匹配的点，其坐标系都是leftdown的，需要根据影像高转化为leftup，
																		//如果是手动量的像点，坐标系是左上角的，就设为LEFTUP
		muxTskWriter.img.SetImgPointCoord(ImgTskList::LEFTDOWN);	

		char strIXYPath[512];	
		if( model_sat == SAT_WFV )	{ strcpy(strIXYPath,strXml);	strcpy(strrpath(strIXYPath),"/AT/ATMCH/Orientation/DOMMatch/");	pS = strIXYPath+strlen(strIXYPath)-1;	}
		else {	strcpy(strIXYPath,xml.m_sMchDir);	pS = AddEndSlash(strIXYPath); }

		FILE* fsz = fopen(strImgSzPath,"r");	int nCols = 0;
		char strLine[512];
		if( fsz ) fgets(strLine,512,fsz);

		////////////////////////////////////////////////////////////////获取输入影像的信息 pImgInf->nID即为熊博中影像id号
		pImgInf=xml.m_inImg;	int muxCnt=0,panCnt=0;
		for ( i = 0; i < num; i++,pImgInf++){		//输入每一张影像的信息，此处假设是100张
			if( !(pImgInf->imgAttrib==IT_MUX||pImgInf->imgAttrib==IT_BWD||pImgInf->imgAttrib==IT_FWD||pImgInf->imgAttrib==IT_NAD) ) continue;

			if( fsz ) {	fgets(strLine,512,fsz); sscanf(strLine,"%d",&nCols);	}
			pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');	if(!pT)	pT = pImgInf->sImgPN-1;	pT++;
			strcpy(pS+1,pT);		
			if(model_sat==SAT_WFV) sprintf(strrchr(pS,'.'),"_%d.ixy",pImgInf->nDatID);	else strcpy(strrchr(pS,'.'),".ixy");

			Unix2Dos(pImgInf->sOrgRPCPath);	Unix2Dos(strIXYPath);	Unix2Dos(pImgInf->sAdjRPCPath);
			if ( resectiontype != SR_NO && pImgInf->imgAttrib == IT_MUX )
			{
				muxTskWriter.img.mdl[muxCnt].InitForZy3Mux(
				2,							//2代表8参数的非线性模型，适合资源三号多光谱，如果是其他的卫星模型不明确的，就设为1，1表示6参数仿射模型
				nCols,						//从熊博的.sz文件里面获取
				pImgInf->sOrgRPCPath,	//原始的rpb文件，也可以是rpc
				strIXYPath,				//熊博匹配的ixy文件
				pImgInf->sAdjRPCPath,	//平差结果的rpc文件，也可以是rpb
				0,					//影像获取的时间，如果不好获得，就给0，没关系
				9100						//影像宽，可以通过判断，然后给定值，如下视永远是24530，当然不给也行。
				);
				muxCnt++;
			}
			else
			{
				if ( model_sat == SAT_JB ){
					nadTaskWriter.img.mdl[panCnt].InitForJb(
						model_pan,
						nCols,						//影像的高度，从熊博的.sz文件里面获取
						pImgInf->sOrgRPCPath,				//原始的rpb文件，也可以是rpc
						strIXYPath,				//熊博匹配的ixy文件
						pImgInf->sAdjRPCPath,	//平差结果的rpc文件，也可以是rpb
						0,					//影像获取的时间，如果不好获得，就给0，没关系
						24530						//影像宽，可以通过判断，然后给定值，如下视永远是24530，当然不给也行。
						);
				}else
				{
					nadTaskWriter.img.mdl[panCnt].InitForZy3Tlc(
						nCols,						//影像的高度，从熊博的.sz文件里面获取
						pImgInf->sOrgRPCPath,				//原始的rpb文件，也可以是rpc
						strIXYPath,				//熊博匹配的ixy文件
						pImgInf->sAdjRPCPath,	//平差结果的rpc文件，也可以是rpb
						0,					//影像获取的时间，如果不好获得，就给0，没关系
						24530						//影像宽，可以通过判断，然后给定值，如下视永远是24530，当然不给也行。
						);
				}
				panCnt++;
				if (resectiontype==SR_PAN){
					muxTskWriter.img.mdl[muxCnt].InitForZy3Mux(
						2,							//2代表8参数的非线性模型，适合资源三号多光谱，如果是其他的卫星模型不明确的，就设为1，1表示6参数仿射模型
						nCols,						//从熊博的.sz文件里面获取
						pImgInf->sOrgRPCPath,	//原始的rpb文件，也可以是rpc
						strIXYPath,				//熊博匹配的ixy文件
						pImgInf->sAdjRPCPath,	//平差结果的rpc文件，也可以是rpb
						0,					//影像获取的时间，如果不好获得，就给0，没关系
						24530						//影像宽，可以通过判断，然后给定值，如下视永远是24530，当然不给也行。
						);
					muxCnt++;
				}
			}
			LogPrint(0,"\n[%d]ImgInfo:\ncols:%d\norgrpc:%s\nixy:%s\nadjrpc:%s",i,nCols,pImgInf->sOrgRPCPath,strIXYPath,pImgInf->sAdjRPCPath);
		}
		if( fsz ) fclose(fsz);

		//最后，终于可以写文件了
		nadTaskWriter.Write();
		muxTskWriter.Write();
		LogPrint(0,"Task File is written!");

		char strKNLExe[512];	strcpy(strKNLExe,strExe);	
		pS = strrchr(strKNLExe,'/');	if(!pS) pS = strrchr(strKNLExe,'\\');
		strcpy(pS,"/AtAdj_KNL.exe ");
		
		LogPrint(ERR_NONE,"Total Num of Task : 1 ");

		pS = strrchr(strTsk,'/');	*pS = 0;	pS = strrchr(strTsk,'/');	strcpy(pS,"/ATADJ.TSK");

		FILE* fp = fopen(strTsk,"wt");	if(!fp) { LogPrint(ERR_NONE,"Fail to Create %s !",strTsk); return -1; }

		fprintf(fp,"1\n");	
		strcpy(strKnlConfig,strExe);
		strcpy(pS,"/TSK/adj_nad.tsk");	strcpy(strrchr(strKnlConfig,'/'),"/ControlPara.config");
		fprintf(fp,"%s \"%s\" \"%s\" ",strKNLExe,strTsk,strKnlConfig);

		if(resectiontype!=SR_NO) {
			strcpy(pS,"/TSK/adj_mux.tsk");	strcpy(strrchr(strKnlConfig,'/'),"/ControlPara-MUX.config");
			fprintf(fp," \"%s\" \"%s\" ",strTsk,strKnlConfig);
		}
		fprintf(fp,"\n");

		fclose(fp);
	}
	return 0;
}

