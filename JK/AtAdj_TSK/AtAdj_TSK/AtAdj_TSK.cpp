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

		///////////////////////////�������xml��ʵ��
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
		//Ȼ�����ǿ�ʼ��֯����׵Ĺ��̣�
		//ֻ��һ����Ҫע�⣺���Ƶ���Ҫ���ɴ�����ƽ��õ����﷽���ļ���
		//��Ϊ����������ĺ󷽽��ᣡ�����м�
		MipAdjTskWriter muxTskWriter;

		nadTaskWriter.SetTaskInfo(
			strTsk,	//����Ŀ¼
			"adj_nad",															//��������
			true);																	//�Ƿ񽫾����������㵽RPC������
		
		//����ƽ��ͳ�ƽ�������λ��
		strcpy(pT+1,"PAN");	CreateDir(strOutPath);	nadTaskWriter.pi.projPath = strOutPath;
		char strMuxGcp[512];	sprintf(strMuxGcp,"%s/GroundCoordinates.txt",strOutPath);
		LogPrint(0,"Set Pan Task Info:[%s,adj_nad,%s]",strTsk,xml.m_sMchDir);

		muxTskWriter.SetTaskInfo(
			strTsk,
			"adj_mux",		//ע������治һ��
			true);				//ע�⣡���˴�����Ϊtrue�������8������������Ե�RPC����ȥ������
		
		strcpy(pT+1,"MUX");	CreateDir(strOutPath);	muxTskWriter.pi.projPath = strOutPath;	//ע������治һ��
		LogPrint(0,"Set MUX Task Info:[%s,adj_mux]",strTsk,strTsk);

		strcpy(strF,xml.m_sGCPPN);	strcpy(strrchr(strF,'_'),".icp");
		if( !IsExist(strF) ) { FILE* fp = fopen(strF,"w");	if(fp) fclose(fp);	}
		{
			char strLocalGcd[512],strWgs84Gcd[512],strPar7Gcd[512];
			strcpy(strWgs84Gcd,strExe);		strcpy(strrchr(strWgs84Gcd,'/'),"/wgs84.gcd");
			strcpy(pS,"/GcpGeoSys.gcd");	strcpy(strLocalGcd,strWorkDir);
			strcpy(pS,"/Geocentric7.gcd");	strcpy(strPar7Gcd,strWorkDir);


			nadTaskWriter.ci.SetGcpInfo(
				xml.m_sGCPPN,		//�˴�������Ƶ��ļ��ľ���·��
				strF,		//�����ļ��ľ���·��
				(ControlInfo::GCPTYPE)ellipse_gcp,	//�˴�ֻ����WGS84��LOCAL��������ö�ٱ����������LOCAL���������������gcd�ļ��������WGS84������Ҫ
				(ControlInfo::HEITYPE)heisys_gcp,//�������Ƶ�߳�ֵ������ϵͳ���˴�ֻ����NORMAL��GEODETIC����ö�ٱ�����NORMAL��ʾ�����ߣ�GEODETIC��ʾ��ظ�
				strLocalGcd,		//����GCD��ֻ�п��Ƶ�������LOCALʱ������Ҫ����
				strWgs84Gcd,		//
				strPar7Gcd,			//
				"");				//�û��Զ����EGM�ļ������ø����������Ҳ����
			LogPrint(0,"Set Pan Gcp Info:[%s,%s]",xml.m_sGCPPN,strF);

			nadTaskWriter.ci.SetDemInfo(
				(ControlInfo::DEMTYPE)source_dem,	//�˴�ֻ����SRTM��OTHER��������ö�ٱ����������SRTM��Ĭ��ʹ�����õ�SRTM�������OTHER����ʹ�ø����.DEM����.BEM��ʽ��dem�ļ�
				//���ʹ���û�ָ����DEM������Ҫ������Ӧ����ϵ������GCD�ļ�
				(ControlInfo::HEITYPE)heisys_dem,//DEM�ĸ߳�ϵͳ���˴�ֻ����NORMAL��GEODETIC����ö�ٱ�����NORMAL��ʾ�����ߣ�GEODETIC��ʾ��ظ�
				lpDemPath?lpDemPath:""	   ,	//������������OTHER���������ļ�����·��
				strLocalGcd,	//DEM��������ϵ�ĵ�����GCD�ļ�
				strWgs84Gcd,	//
				strPar7Gcd,		//
				""					//�û��Զ����EGM�ļ������ø����������Ҳ����
			);
			LogPrint(0,"Set Pan Dem Info:[%s]",lpDemPath?lpDemPath:"");

			muxTskWriter.ci.SetGcpInfo(
				strMuxGcp,		//ע�⣬����Ŀ��Ƶ���ͨ������Ӱ���ƽ��õ��ģ�
				//�����·��Ϊ$(nadproj)\GroundCoordinates.txt
				strF,		
				ControlInfo::WGS84,																						//������ϵΪWGS84
				ControlInfo::GEODETIC,																					//��߳�ϵΪ��ظ�
				"",																										//�����gcd�ļ�����������Ȼ����Ҳû��
				"",		
				"",			
				"");				
			LogPrint(0,"Set MUX Gcp Info:[%s,%s]",strMuxGcp,strF);
				
			muxTskWriter.ci.SetDemInfo(
				(ControlInfo::DEMTYPE)source_dem,					//��Щ���վ�
				(ControlInfo::HEITYPE)heisys_dem,	
				"",							//��Щ�����ø�
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
			

		nadTaskWriter.img.SetImgPointCoord(ImgTskList::LEFTDOWN);			//ֻҪ���ܲ�ƥ��ĵ㣬������ϵ����leftdown�ģ���Ҫ����Ӱ���ת��Ϊleftup��
																		//������ֶ�������㣬����ϵ�����Ͻǵģ�����ΪLEFTUP
		muxTskWriter.img.SetImgPointCoord(ImgTskList::LEFTDOWN);	

		char strIXYPath[512];	
		if( model_sat == SAT_WFV )	{ strcpy(strIXYPath,strXml);	strcpy(strrpath(strIXYPath),"/AT/ATMCH/Orientation/DOMMatch/");	pS = strIXYPath+strlen(strIXYPath)-1;	}
		else {	strcpy(strIXYPath,xml.m_sMchDir);	pS = AddEndSlash(strIXYPath); }

		FILE* fsz = fopen(strImgSzPath,"r");	int nCols = 0;
		char strLine[512];
		if( fsz ) fgets(strLine,512,fsz);

		////////////////////////////////////////////////////////////////��ȡ����Ӱ�����Ϣ pImgInf->nID��Ϊ�ܲ���Ӱ��id��
		pImgInf=xml.m_inImg;	int muxCnt=0,panCnt=0;
		for ( i = 0; i < num; i++,pImgInf++){		//����ÿһ��Ӱ�����Ϣ���˴�������100��
			if( !(pImgInf->imgAttrib==IT_MUX||pImgInf->imgAttrib==IT_BWD||pImgInf->imgAttrib==IT_FWD||pImgInf->imgAttrib==IT_NAD) ) continue;

			if( fsz ) {	fgets(strLine,512,fsz); sscanf(strLine,"%d",&nCols);	}
			pT = strrchr(pImgInf->sImgPN,'/');	if(!pT) pT = strrchr(pImgInf->sImgPN,'\\');	if(!pT)	pT = pImgInf->sImgPN-1;	pT++;
			strcpy(pS+1,pT);		
			if(model_sat==SAT_WFV) sprintf(strrchr(pS,'.'),"_%d.ixy",pImgInf->nDatID);	else strcpy(strrchr(pS,'.'),".ixy");

			Unix2Dos(pImgInf->sOrgRPCPath);	Unix2Dos(strIXYPath);	Unix2Dos(pImgInf->sAdjRPCPath);
			if ( resectiontype != SR_NO && pImgInf->imgAttrib == IT_MUX )
			{
				muxTskWriter.img.mdl[muxCnt].InitForZy3Mux(
				2,							//2����8�����ķ�����ģ�ͣ��ʺ���Դ���Ŷ���ף����������������ģ�Ͳ���ȷ�ģ�����Ϊ1��1��ʾ6��������ģ��
				nCols,						//���ܲ���.sz�ļ������ȡ
				pImgInf->sOrgRPCPath,	//ԭʼ��rpb�ļ���Ҳ������rpc
				strIXYPath,				//�ܲ�ƥ���ixy�ļ�
				pImgInf->sAdjRPCPath,	//ƽ������rpc�ļ���Ҳ������rpb
				0,					//Ӱ���ȡ��ʱ�䣬������û�ã��͸�0��û��ϵ
				9100						//Ӱ�������ͨ���жϣ�Ȼ�����ֵ����������Զ��24530����Ȼ����Ҳ�С�
				);
				muxCnt++;
			}
			else
			{
				if ( model_sat == SAT_JB ){
					nadTaskWriter.img.mdl[panCnt].InitForJb(
						model_pan,
						nCols,						//Ӱ��ĸ߶ȣ����ܲ���.sz�ļ������ȡ
						pImgInf->sOrgRPCPath,				//ԭʼ��rpb�ļ���Ҳ������rpc
						strIXYPath,				//�ܲ�ƥ���ixy�ļ�
						pImgInf->sAdjRPCPath,	//ƽ������rpc�ļ���Ҳ������rpb
						0,					//Ӱ���ȡ��ʱ�䣬������û�ã��͸�0��û��ϵ
						24530						//Ӱ�������ͨ���жϣ�Ȼ�����ֵ����������Զ��24530����Ȼ����Ҳ�С�
						);
				}else
				{
					nadTaskWriter.img.mdl[panCnt].InitForZy3Tlc(
						nCols,						//Ӱ��ĸ߶ȣ����ܲ���.sz�ļ������ȡ
						pImgInf->sOrgRPCPath,				//ԭʼ��rpb�ļ���Ҳ������rpc
						strIXYPath,				//�ܲ�ƥ���ixy�ļ�
						pImgInf->sAdjRPCPath,	//ƽ������rpc�ļ���Ҳ������rpb
						0,					//Ӱ���ȡ��ʱ�䣬������û�ã��͸�0��û��ϵ
						24530						//Ӱ�������ͨ���жϣ�Ȼ�����ֵ����������Զ��24530����Ȼ����Ҳ�С�
						);
				}
				panCnt++;
				if (resectiontype==SR_PAN){
					muxTskWriter.img.mdl[muxCnt].InitForZy3Mux(
						2,							//2����8�����ķ�����ģ�ͣ��ʺ���Դ���Ŷ���ף����������������ģ�Ͳ���ȷ�ģ�����Ϊ1��1��ʾ6��������ģ��
						nCols,						//���ܲ���.sz�ļ������ȡ
						pImgInf->sOrgRPCPath,	//ԭʼ��rpb�ļ���Ҳ������rpc
						strIXYPath,				//�ܲ�ƥ���ixy�ļ�
						pImgInf->sAdjRPCPath,	//ƽ������rpc�ļ���Ҳ������rpb
						0,					//Ӱ���ȡ��ʱ�䣬������û�ã��͸�0��û��ϵ
						24530						//Ӱ�������ͨ���жϣ�Ȼ�����ֵ����������Զ��24530����Ȼ����Ҳ�С�
						);
					muxCnt++;
				}
			}
			LogPrint(0,"\n[%d]ImgInfo:\ncols:%d\norgrpc:%s\nixy:%s\nadjrpc:%s",i,nCols,pImgInf->sOrgRPCPath,strIXYPath,pImgInf->sAdjRPCPath);
		}
		if( fsz ) fclose(fsz);

		//������ڿ���д�ļ���
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

