#ifndef _INTEGRAL_MAX_BITS
#define _INTEGRAL_MAX_BITS 64
#endif
#if _INTEGRAL_MAX_BITS<64
#define _INTEGRAL_MAX_BITS 64
#endif

#include "ZY3FileName.h"
#include "LxXML.h"
#include <sys/stat.h>



#ifdef WIN32
#include "ZY3Image.h"
#include "WuRpc.h"
inline __int64 getfilesize(LPCSTR lpFilePath)
{
	struct _stati64 buf;
	int cc;
    cc=_stati64(lpFilePath,&buf);	//lstat
    if(!cc ) return (__int64)buf.st_size;
	
    return 0;
}
#else
typedef long __int64;
inline __int64 getfilesize(LPCSTR lpFilePath)
{
	struct stat buf;
	int cc;
    cc=stat(lpFilePath,&buf);	//lstat
    if(!cc ) return (__int64)buf.st_size;
	
    return 0;
}
#ifndef _strlwr
#define _strlwr
inline char* strlwr( char *str ){
	char *orig = str; char d = 'a'-'A';
	for( ;*str!='\0';str++ ){ if ( *str>='A' && *str<='Z' ) *str = *str+d; }
	return orig;
}
#endif

inline char* strupr( char *str ){
	char *orig = str; char d = 'a'-'A';
	for( ;*str!='\0';str++ ){ if ( *str>='a' && *str<='z' ) *str = *str-d; }
	return orig;
}

inline int stricmp( char *s1,char *s2 ){
    char strT1[512],strT2[512];
    strcpy(strT1,s1); strcpy(strT2,s2);
    strlwr( strT1 ); strlwr( strT2 );
    return strcmp( strT1,strT2 );
}

inline int strnicmp( char *s1,char *s2,int n ){
	char strT1[512],strT2[512];
	strcpy(strT1,s1); strcpy(strT2,s2);
	strlwr( strT1 ); strlwr( strT2 );
	return strncmp( strT1,strT2,n );
}

#ifndef _RpcPara
#define _RpcPara
typedef struct tagRpcPara
{
    double line_off;     // line offset in pixels
	double samp_off;     // sample offset in pixels
	double lat_off;      // latitude offset in degrees
	double long_off;     // longitude offset in degrees
	double height_off;   // height offset in meters
	double line_scale;   // line scale in pixels
	double samp_scale;   // sample scale in pixels
	double lat_scale;    // latitude scale in degrees
	double long_scale;   // longitude scale in degrees
	double height_scale; // height scale in meters
	double c[20];        // 20 line numerator coefficients
	double d[20];        // 20 line denominator coefficients
	double a[20];        // 20 sample numerator coefficients
	double b[20];        // 20 sample denominator coefficients
    
}RpcPara;
#endif

#endif

#define AVERAGE_HEIGHT_DEFAULT		0

bool LoadRpcFile(const char* lpstrPathName,RpcPara* rpcPar,double* aop6)
{
    if ( strlen(lpstrPathName)<3 ) return false;
    FILE *fRpc = fopen( lpstrPathName,"rt" );
	if ( fRpc ){
        int i; char chID[64]; double *rpc = (double*)(rpcPar);
        fscanf( fRpc, "%s", chID ); rewind( fRpc );
        if ( stricmp(chID,"LINE_OFF:")==0 ){                        
            for ( i=0;i<10;i++ ){ if (fscanf(fRpc,"%s%lf%s",chID,rpc+i,chID)!=3){ fclose(fRpc); return false; } }
            for(  i=0;i<80;i++ ){ if (fscanf(fRpc,"%s%lf",chID,rpc+i+10)!=2    ){ fclose(fRpc); return false; } }       
            fscanf( fRpc, "%s", chID );
            if ( strnicmp(chID, "RFM_CORRECTION_PARAMETERS",25)==0 ){ for (i=0; i<6; i++) fscanf(fRpc,"%s%lf\n", chID,aop6+i ); }
            else memset( aop6,0,sizeof(double)*6 );
        }else
        { // Read RPB ,PVL
            char strLine[256],strID[64],strEq[16],strVal[128];
            char *pGet = fgets( strLine, 256,fRpc );
			while( pGet ){
				sscanf( strLine,"%s%s%s",strID,strEq,strVal );
				if ( stricmp(strID ,"BEGIN_GROUP")==0 &&
					 stricmp(strEq ,"=")==0 &&
					 strnicmp(strVal,"IMAGE",5)==0 ){
					while( pGet ){
						pGet = fgets( strLine, 256,fRpc );
						sscanf( strLine,"%s%s%s",strID,strEq,strVal );

						if ( stricmp(strID ,"lineOffset"	 )==0 ) rpc[0] = atof(strVal); else 
						if ( stricmp(strID ,"sampOffset"	 )==0 ) rpc[1] = atof(strVal); else 
						if ( stricmp(strID ,"latOffset"		 )==0 ) rpc[2] = atof(strVal); else 
						if ( stricmp(strID ,"longOffset"     )==0 ) rpc[3] = atof(strVal); else 
						if ( stricmp(strID ,"heightOffset"	 )==0 ) rpc[4] = atof(strVal); else 
						if ( stricmp(strID ,"lineScale"		 )==0 ) rpc[5] = atof(strVal); else 
						if ( stricmp(strID ,"sampScale"		 )==0 ) rpc[6] = atof(strVal); else 
						if ( stricmp(strID ,"latScale"		 )==0 ) rpc[7] = atof(strVal); else 
						if ( stricmp(strID ,"longScale"		 )==0 ) rpc[8] = atof(strVal); else 
						if ( stricmp(strID ,"heightScale"	 )==0 ) rpc[9] = atof(strVal); else 
						if ( stricmp(strID ,"lineNumCoef"	 )==0 ){
							for ( i=10; (i<30&&!feof(fRpc)); ){
                                pGet = fgets( strLine, 256,fRpc ); 
                                if ( !pGet ) continue; while ( *pGet==' ' ) pGet++;
                                if ( strlen(pGet)<3 ) continue;								
                                rpc[i++]=atof( strLine );
							}
						}else
						if ( stricmp(strID ,"lineDenCoef")==0 ){
							for ( i=30;(i<50&&!feof(fRpc)); ){
								pGet = fgets( strLine, 256,fRpc ); 
                                if ( !pGet ) continue; while ( *pGet==' ' ) pGet++;
                                if ( strlen(pGet)<3 ) continue;								
                                rpc[i++]=atof( strLine );
							}
						}else
						if ( stricmp(strID ,"sampNumCoef")==0 ){
							for ( i=50;(i<70&&!feof(fRpc)); ){
								pGet = fgets( strLine, 256,fRpc ); 
                                if ( !pGet ) continue; while ( *pGet==' ' ) pGet++;
                                if ( strlen(pGet)<3 ) continue;
								rpc[i++]=atof( strLine );
							}
						}else
						if ( stricmp(strID ,"sampDenCoef")==0 ){
							for ( int i=70;(i<90&&!feof(fRpc)); ){
								pGet = fgets( strLine, 256,fRpc ); 
                                if ( !pGet ) continue; while ( *pGet==' ' ) pGet++;
                                if ( strlen(pGet)<3 ) continue;                                
								rpc[i++]=atof( strLine );
							}
						}
					}
					break;
				}else
                if ( stricmp(strID ,"BEGIN_GROUP")==0 &&
					 stricmp(strEq ,"=")==0 &&
					 strnicmp(strVal,"rationalFunctions",17)==0 )
				{
					while( pGet )
					{
						pGet = fgets( strLine, 256,fRpc ); 
						sscanf( strLine,"%s%s%s",strID,strEq,strVal );

						if ( stricmp(strID ,"lineOffset"	 )==0 ) rpc[0] = atof(strVal); else 
						if ( stricmp(strID ,"pixelOffset"	 )==0 ) rpc[1] = atof(strVal); else 
						if ( stricmp(strID ,"latitudeOffset" )==0 ) rpc[2] = atof(strVal); else 
						if ( stricmp(strID ,"longitudeOffset")==0 ) rpc[3] = atof(strVal); else 
						if ( stricmp(strID ,"heightOffset"	 )==0 ) rpc[4] = atof(strVal); else 
						if ( stricmp(strID ,"lineScale"		 )==0 ) rpc[5] = atof(strVal); else 
						if ( stricmp(strID ,"pixelScale"	 )==0 ) rpc[6] = atof(strVal); else 
						if ( stricmp(strID ,"latitudeScale"	 )==0 ) rpc[7] = atof(strVal); else 
						if ( stricmp(strID ,"longitudeScale" )==0 ) rpc[8] = atof(strVal); else 
						if ( stricmp(strID ,"heightScale"	 )==0 ) rpc[9] = atof(strVal); else 
						if ( stricmp(strID ,"lineNumeratorCoefficients")==0 )
						{
							for ( i=10;(i<30 && !feof(fRpc)); )
							{
                                pGet = fgets( strLine, 256,fRpc );
                                if ( !pGet ) continue; while ( *pGet==' ' ) pGet++;
                                if ( strlen(pGet)<3 ) continue;

								rpc[i++]=atof( strLine ); 
							}
						}else
						if ( stricmp(strID ,"lineDenominatorCoefficients")==0 )
						{
							for ( i=30;(i<50 && !feof(fRpc)); )
							{
								pGet = fgets( strLine, 256,fRpc ); 
                                if ( !pGet ) continue; while ( *pGet==' ' ) pGet++;
                                if ( strlen(pGet)<3 ) continue;

								rpc[i++]=atof( strLine ); 
							}
						}else
						if ( stricmp(strID ,"pixelNumeratorCoefficients")==0 )
						{
							for ( i=50;(i<70 && !feof(fRpc)); )
							{
								pGet = fgets( strLine, 256,fRpc ); 
                                if ( !pGet ) continue; while ( *pGet==' ' ) pGet++;
                                if ( strlen(pGet)<3 ) continue;

								rpc[i++]=atof( strLine );
							}
						}else
						if ( stricmp(strID ,"pixelDenominatorCoefficients")==0 )
						{
							for ( int i=70;(i<90&& !feof(fRpc)); )
							{
								pGet = fgets( strLine, 256,fRpc ); 
                                if ( !pGet ) continue; while ( *pGet==' ' ) pGet++;
                                if ( strlen(pGet)<3 ) continue;
                                
								rpc[i++]=atof( strLine );
							}
						}
					}
					break;
				}
                pGet = fgets( strLine, 256,fRpc );
			}
        }
        fclose(fRpc);        
    }

    return true;
}

short CalcRgn(SCPT* pt,LPCSTR lpstrFilePath,LPCSTR lpstrGeoPath,GEOFILE_TYPE type)
{
	int sz=0;
	switch(type)
	{
	case GT_RPC:
		{
#ifdef WIN32
			CRpc rpc;	
			if( ! rpc.Load4File(lpstrGeoPath) ) break;
			
			double line,sample,ftmp[3];		
			line = rpc.m_rpcPar.line_off*2;	sample = rpc.m_rpcPar.samp_off*2;
			rpc.RPCPhoZ2Grd(0	   ,0	,AVERAGE_HEIGHT_DEFAULT,ftmp,ftmp+1,ftmp+2);
			pt->ly = (float)ftmp[0];	pt->lx = (float)ftmp[1];	
			rpc.RPCPhoZ2Grd(sample ,0	,AVERAGE_HEIGHT_DEFAULT,ftmp,ftmp+1,ftmp+2);
			pt->ry = (float)ftmp[0];	pt->rx = (float)ftmp[1];
			rpc.RPCPhoZ2Grd(0	   ,line,AVERAGE_HEIGHT_DEFAULT,ftmp,ftmp+1,ftmp+2);	pt++;
			pt->ly = (float)ftmp[0];	pt->lx = (float)ftmp[1];
			rpc.RPCPhoZ2Grd(sample ,line,AVERAGE_HEIGHT_DEFAULT,ftmp,ftmp+1,ftmp+2);
			pt->ry = (float)ftmp[0];	pt->rx = (float)ftmp[1];
			
#else 
			RpcPara rpcPar;	double aop6[6];
			if( !LoadRpcFile(lpstrGeoPath,&rpcPar,aop6) ) break;
			pt->lx = (float)(rpcPar.long_off-rpcPar.long_scale);	pt->ly = (float)(rpcPar.lat_off+rpcPar.lat_scale);
			pt->rx = (float)(rpcPar.long_off+rpcPar.long_scale);	pt->ry = (float)(rpcPar.lat_off+rpcPar.lat_scale);	pt++;
			pt->lx = (float)(rpcPar.long_off-rpcPar.long_scale);	pt->ly = (float)(rpcPar.lat_off-rpcPar.lat_scale);
			pt->rx = (float)(rpcPar.long_off+rpcPar.long_scale);	pt->ry = (float)(rpcPar.lat_off-rpcPar.lat_scale);
			
#endif
			sz = 2;
			break;
		}
	default:		
		double Dx,Dy,Rx,Ry,Ex,Ny;
		if(strlen(lpstrGeoPath)<3) break;
		FILE *ftfw = fopen( lpstrGeoPath,"rt" ); if (!ftfw) break;
		fscanf( ftfw,"%lf",&Dx );
		fscanf( ftfw,"%lf",&Rx );
		fscanf( ftfw,"%lf",&Ry );
		fscanf( ftfw,"%lf",&Dy );
		fscanf( ftfw,"%lf",&Ex );
		fscanf( ftfw,"%lf",&Ny );
		fclose( ftfw ); 
		
#ifdef WIN32
		CZY3Image img;
		if( !img.Open(lpstrFilePath) ) break;
		int nRows,nCols;
		nRows = img.GetRows();	nCols = img.GetCols();
		pt->lx = (float)Ex;						pt->ly = (float)Ny;
		pt->rx = (float)(Ex+nCols*Dx+nRows*Rx);	pt->ry = (float)Ny;	pt++;
		pt->lx = (float)Ex;						pt->ly = (float)(Ny+nCols*Ry+nRows*Dy);
		pt->rx = (float)(Ex+nCols*Dx+nRows*Rx);	pt->ry = (float)(Ny+nCols*Ry+nRows*Dy);

		sz = 2;
#endif
		break;
	}
	return sz;
	
}

char* strchr(const char* s,int ch ,int idx)
{
	if(idx<0) return NULL;
	char* string = (char*)s;
	int i = 0;
	
	while(*string)
	{
		if (*string == ch){
			if(i<idx) i++;
			else break;
		}
		string++;	
	}
	
	if (*string == ch){
		return string;
	}
	
	return NULL;
}

inline bool GetOribitSceneID4PGSMeta(const char* lpstrMetaPath,char* oribit,char* scene)
{
	CXmlFile xml;	if( !xml.Open(lpstrMetaPath) )	return false;
	char strTmp[50];
	if ( xml.FindNode(Mode_Absolute,strTmp,2,"ProductMetaData","OrbitID") ) strcpy(oribit,strTmp);
	if ( xml.FindNode(Mode_Absolute,strTmp,2,"ProductMetaData","SceneRow") ) strcpy(scene,strTmp);
	xml.Close();
	return true;
}

#define MAX_PARTSIZE		100
#define MAX_PARTNUM			20

#ifndef _tagDivNam
#define _tagDivNam
typedef struct tagDivNam{
	char strPart[MAX_PARTNUM*MAX_PARTSIZE];
	int partNum;
	char divSym;
}DivNam;
#endif

inline void SplitName(DivNam* dN,const char* lpstrNam,char s){
	memset(dN,0,sizeof(DivNam));
	if(*lpstrNam == '\0') return;
	dN->divSym = s;
	int partNum = 0;
	char* pPart = dN->strPart;
	const char* pS = lpstrNam;
	const char* pOld = lpstrNam;int l=0;
	
	while(*pS){
		if(*pS!=s) l++;
		else{
			if(!l) continue;
			memcpy(pPart,pOld,l*sizeof(char));
			pPart = pPart+MAX_PARTSIZE;
			partNum++;
			l=0;pOld = pS+1;if(*pOld=='\0') break;
		}
		pS++;
	}
	memcpy(pPart,pOld,l*sizeof(char));
	partNum++;
	dN->partNum = partNum;
}
inline char* GetPartNam(DivNam& dN,int i){
	if(i<0) i=0;else if(i>dN.partNum) i = dN.partNum;
	return dN.strPart+i*MAX_PARTSIZE;
}
inline void GetPartNam(char* pStr,DivNam& dN,int i){
	strcpy(pStr,GetPartNam(dN,i));
}

/////////////////////////////////////////////////////////////////////////////////////////
//CZY3File
IMPLEMENT_DYNARCH(CZY3File,CArchFile)

CZY3File::CZY3File()
{
	
}
CZY3File::~CZY3File()
{
	
}

int CZY3File::Load4File(LPCSTR lpstrPath)
{
	return CArchFile::Load4File(lpstrPath);
}


IMPLEMENT_DYNARCH(CZY3SceneFile,CZY3File)

CZY3SceneFile::CZY3SceneFile()
{
	
}
CZY3SceneFile::~CZY3SceneFile()
{
	
}
int CZY3SceneFile::Load4File(LPCSTR lpstrPath)
{
	strcpy(m_imgInfo.path,lpstrPath);
	m_imgInfo.nBlockID = 0;
	//	strcpy( ((CZY3SceneFile*)m_pLOb)->m_imgInfo.path,lpstrPath);
	
	return CZY3File::Load4File(lpstrPath);
}

int CZY3SceneFile::load(const ImageInfo* pInfo)
{
	if(m_pLOb) delete (CZY3SceneFile*)m_pLOb;	m_pLOb = NULL;//
	m_pLOb = identify(pInfo->path);	
	if( m_pLOb ){
		CZY3SceneFile* p = (CZY3SceneFile*)m_pLOb;
		memcpy(&(p->m_imgInfo),pInfo,sizeof(ImageInfo));
		
		int ret = p->InitExInfo(pInfo->path,p->m_imgInfo.lvl);
		return ret;
	}	
	
	return ERR_FILE_TYPE; 
}

//////////////////////////////////////////////////////////////////CSASMACFile
inline bool GetOribitID4SASMACMeta(const char* lpstrMetaPath,char* oribit)
{
	CXmlFile xml;	if( !xml.Open(lpstrMetaPath) )	return false;
	char strTmp[50];
	xml.FindSibingNode(NULL);
	xml.IntoNode();
	if( xml.FindSibingNode("productInfo") == NOT_GET) return false;	xml.IntoNode();
	if( xml.FindSibingNode("OrbitID",strTmp) == NOT_GET) return false;
	strcpy(oribit,strTmp);
	
	xml.Close();
	return true;
}

IMPLEMENT_DYNFILELOAD(CSASMACFile,CZY3SceneFile)
CSASMACFile::CSASMACFile()
{
	
}
CSASMACFile::~CSASMACFile()
{
	
}

CLObject* CSASMACFile ::identify(LPCSTR lpstrPath)
{
	char strLine[1024];	strcpy(strLine,lpstrPath);
	bool bType = false;
	
	Dos2Unix(strLine);	char* pN = strrchr(strLine,'/');	if(!pN) pN = strLine-1;	pN++;
	if( MatchString(pN,SASMACFile_TAG) != 0) { bType = true;  }
	
	
	if( bType )	return CSASMACFile::CreateLObject();
	else return NULL;
	
}

int CSASMACFile::TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo)
{
	static int nID = 1;
	char	strPath[512];		
	char	sceneID[10];	memset(sceneID,0,10*sizeof(char));
	char	orbitID[10];	memset(orbitID,0,10*sizeof(char));
	char	sID[10];		memset(sID,0,10*sizeof(char));
	
	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",pN); return ERR_FILE_TYPE; }
	
	char imglvl[5];	memset(imglvl,0,5); { char* pS = pN; for(int i=0; i<9; i++) { pS = strchr(pS+1,'_'); if(!pS) return ERR_FILE_FLAG;	}  memcpy(imglvl,pS+1,3*sizeof(char));	}	
	IMG_LVL lvl = IL_UNKNOW;	CAM_TYPE	cam = CT_UNKNOW;
	
	if( !strcmp(imglvl,"MSC") )
	{
		lvl = IL_MSC;	
	}else
	{
		if((*(pN+14) != 'm')&&(*(pN+14) != 'p')) { printf("Warning:Image Name %s is not standard!\n",pN); return ERR_FILE_TYPE;}
		if(*(pN+14) == 'm')	{	cam = CT_MUX;	}else
			if(*(pN+11) == 'f')	{	cam = CT_FWD;	}else
				if(*(pN+11) == 'n')	{	cam = CT_NAD;	}else
					if(*(pN+11) == 'b')	{	cam = CT_BWD;	}
					
					if( !strcmp(imglvl,"sec") )	lvl = IL_SC;else
						if( !strcmp(imglvl,"dsm") ) lvl = IL_DSM;else
							if( !strcmp(imglvl,"dem") ) lvl = IL_DEM;else
								if( !strcmp(imglvl,"epi") ) lvl = IL_EPI;else
									if( !strcmp(imglvl,"gec") ) lvl = IL_CJZ;else
										if( !strcmp(imglvl,"ggc") ) lvl = IL_JJZ;else
											if( !strcmp(imglvl,"gtc") ) lvl = IL_DOM;else
												if( !strcmp(imglvl,"fus") ) lvl = IL_FUS;
												else	{ printf("Unknow image level %s of %s !\n",imglvl,pN); return false;	}
												
													memcpy(sceneID,pN+19,3);	
												GetMetaPath(lpstrPath,strPath);
		if( !GetOribitID4SASMACMeta(strPath,orbitID) )	memcpy(orbitID,pN+16,3);
		sprintf(sID,"%s%s",orbitID,sceneID);
												// 		memcpy(orbitID,pN+16,4);	memcpy(sceneID,pN+20,2);
	}
	
	pInfo->lvl = lvl;
	pInfo->cam = cam;
	
	strcpy(pInfo->orbitID,orbitID);
	strcpy(pInfo->sceneID,sceneID);
//	pInfo->nID = atoi(sID);
	pInfo->nID = nID++;
	
	return ERR_NONE;
	
}

int CSASMACFile::InitExInfo(LPCSTR lpstrPath,IMG_LVL lvl)
{
	char	strPath[512];	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	GetOriFilePath(pN,lvl,m_imgExInfo.strAdjRpcNam,true);
	GetOriFilePath(pN,lvl,m_imgExInfo.strOrgRpcNam,false);
	
	GetThumbPath(pN,m_imgExInfo.strTumbNam);
	GetBrowsePath(pN,m_imgExInfo.strBrowseNam);
	GetMetaPath(pN,m_imgExInfo.strMetaNam);
	
	*pN = 0;	strcpy(m_imgExInfo.strFileDir,strPath);
	return ERR_NONE;
}

int CSASMACFile :: Load4File(LPCSTR lpstrPath)
{
	int ret = TinyLoad(lpstrPath);
	if( ret != ERR_NONE) return ret;
	
	ret = InitExInfo(lpstrPath,m_imgInfo.lvl);
	if( ret != ERR_NONE) return ret;
	
	return CZY3SceneFile::Load4File(lpstrPath);
}

bool CSASMACFile::GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj /* = true */)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	
	if( lvl == IL_SC) {
		if( bAdj ) strcpy(pT,"_rpc_adj.txt");
		else strcpy(pT,"_rpc.txt");
	}else 
		if( lvl == IL_EPI )
		{
			strcpy(pT,".rpc");
		}else strcpy(pT,".tfw");
		
		strcpy(retPath,strPath);
		
		//	if( IsExist(strPath) ) return true;
		// 	else return false;
		
		return true;
}

bool CSASMACFile::GetThumbPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',"_ico.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;
}

bool CSASMACFile::GetBrowsePath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',"_pre.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CSASMACFile::GetMetaPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".xml") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CSASMACFile::RenameImage(char* strPath,IMG_LVL lvl)
{
	static LPCSTR strLvl[] = {"Unknow","STRIP","sec","epi","dsm","dem","gec","ggc","gtc","fus","dog","msc"};
	Dos2Unix(strPath);
	char* pN = strrchr(strPath,'/'); if(!pN) pN = strPath-1;	pN++;
	for(int i=0; i<9; i++) { pN = strchr(pN,'_'); if(!pN) return false;	}
	memcpy(pN+1,strLvl[lvl],3);
	
	return true;
}

//////////////////////////////////////////////////////////////////CMIPFile

IMPLEMENT_DYNFILELOAD(CMIPFile,CZY3SceneFile)
CMIPFile::CMIPFile()
{
	
}
CMIPFile::~CMIPFile()
{
	
}

CLObject* CMIPFile ::identify(LPCSTR lpstrPath)
{
	char strLine[1024];	strcpy(strLine,lpstrPath);
	bool bType = false;
	
	Dos2Unix(strLine);	char* pN = strrchr(strLine,'/');	if(!pN) pN = strLine-1;	pN++;
	if( MatchString(pN,MIPFile_TAG) != 0) { bType = true;  }
	
	
	if( bType )	return CMIPFile::CreateLObject();
	else return NULL;
	
}

int CMIPFile::TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo)
{
	static int nID = 1;
	char	strPath[512];		
	char	sceneID[10];	memset(sceneID,0,10*sizeof(char));
	char	orbitID[10];	memset(orbitID,0,10*sizeof(char));
	char	sID[20];		memset(sID,0,20*sizeof(char));
	
	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",pN); return ERR_FILE_TYPE; }
	
	DivNam divnam;	SplitName(&divnam,pN,'_');	if( divnam.partNum != 9 && divnam.partNum!=10 ) { printf("Warning:Image Name %s is not standard!\n",pN); return ERR_FILE_TYPE;}

	char imglvl[5];	memset(imglvl,0,5);		
	if(divnam.partNum == 9) memcpy(imglvl,GetPartNam(divnam,6),3);	else	memcpy(imglvl,GetPartNam(divnam,9),3); 

	char strCam[5];	memset(strCam,0,5);
	memcpy(strCam,GetPartNam(divnam,1),3);

	char strBands[5];	memset(strBands,0,5);
	memcpy(strBands,GetPartNam(divnam,7),4);

	IMG_LVL lvl = IL_UNKNOW;	CAM_TYPE	cam = CT_UNKNOW;
	
	if( !strcmp(imglvl,"MSC") )
	{
		lvl = IL_MSC;	
	}else
	{
		if( strcmp(strBands,"0001") != 0 )	{	cam = CT_MUX;	}else
			if( !strcmp(strCam,"fwd") || !strcmp(strCam,"FWD") )	{	cam = CT_FWD;	}else
				if( !strcmp(strCam,"nad") || !strcmp(strCam,"NAD") )	{	cam = CT_NAD;	}else
					if( !strcmp(strCam,"bwd") || !strcmp(strCam,"BWD") )	{	cam = CT_BWD;	}
					
					if( !strcmp(imglvl,"sec") )	lvl = IL_SC;else
						if( !strcmp(imglvl,"dsm") ) lvl = IL_DSM;else
							if( !strcmp(imglvl,"dem") ) lvl = IL_DEM;else
								if( !strcmp(imglvl,"epi") ) lvl = IL_EPI;else
									if( !strcmp(strBands,"BGRN") ) lvl = IL_FUS;else
									if( !strcmp(imglvl,"gec") ) lvl = IL_CJZ;else
										if( !strcmp(imglvl,"ggc") ) lvl = IL_JJZ;else
											if( !strcmp(imglvl,"gtc") ) lvl = IL_DOM;
												else	{ printf("Unknow image level %s of %s !\n",imglvl,pN); return false;	}
												
		memcpy(orbitID,GetPartNam(divnam,2),6);		memcpy(sceneID,GetPartNam(divnam,3),6);	 
		sprintf(sID,"%s%s",orbitID,sceneID);
		
	}
	
	pInfo->lvl = lvl;
	pInfo->cam = cam;
	
	strcpy(pInfo->orbitID,orbitID);
	strcpy(pInfo->sceneID,sceneID);
	pInfo->nID = nID++;
	
	return ERR_NONE;
	
}

int CMIPFile::InitExInfo(LPCSTR lpstrPath,IMG_LVL lvl)
{
	char	strPath[512];	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	GetOriFilePath(pN,lvl,m_imgExInfo.strAdjRpcNam,true);
	GetOriFilePath(pN,lvl,m_imgExInfo.strOrgRpcNam,false);
	
	GetThumbPath(pN,m_imgExInfo.strTumbNam);
	GetBrowsePath(pN,m_imgExInfo.strBrowseNam);
	GetMetaPath(pN,m_imgExInfo.strMetaNam);
	
	*pN = 0;	strcpy(m_imgExInfo.strFileDir,strPath);
	return ERR_NONE;
}

int CMIPFile :: Load4File(LPCSTR lpstrPath)
{
	int ret = TinyLoad(lpstrPath);
	if( ret != ERR_NONE) return ret;
	
	ret = InitExInfo(lpstrPath,m_imgInfo.lvl);
	if( ret != ERR_NONE) return ret;
	
	return CZY3SceneFile::Load4File(lpstrPath);
}

bool CMIPFile::GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj /* = true */)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	
	if( lvl == IL_SC) {
		if( bAdj ) strcpy(pT,"_rpc_adj.txt");
		else strcpy(pT,"_rpc.txt");
	}else 
		if( lvl == IL_EPI )
		{
			strcpy(pT,"_rpc.txt");
		}else strcpy(pT,".tfw");
		
		strcpy(retPath,strPath);
		
		//	if( IsExist(strPath) ) return true;
		// 	else return false;
		
		return true;
}

bool CMIPFile::GetThumbPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',"_ico.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;
}

bool CMIPFile::GetBrowsePath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',"_pre.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CMIPFile::GetMetaPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".xml") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CMIPFile::RenameImage(char* strPath,IMG_LVL lvl)
{
		Dos2Unix(strPath);
	char* pN = strrchr(strPath,'/'); if(!pN) pN = strPath-1;	pN++;
	char* pS;

	if ( lvl == IL_SC ){
		pS = strchr(pN,'_',5);	if(pS) memcpy(pS+1,"sec",3);
	}else if (lvl==IL_CJZ){
		pS = strchr(pN,'_',5);	if(pS) memcpy(pS+1,"gec",3);
	}else if (lvl==IL_DEM){
		pS = strrchr(pN,'_');	strcpy(pS,"_dem.tif");
	}else if (lvl==IL_DSM){
		memcpy(pN+7,"tlc",3);
		pS = strrchr(pN,'.');	strcpy(pS,"_dsm.tif");
	}else if (lvl==IL_EPI){
		pS = strrchr(pN,'.');	strcpy(pS,"_epi.tif");
	}else if (lvl==IL_DOM){
		pS = strchr(pN,'_',5);	if(pS) memcpy(pS+1,"gtc",3);
	}else if (lvl==IL_JJZ){
		pS = strchr(pN,'_',5);	if(pS) memcpy(pS+1,"ggc",3);
	}else if (lvl==IL_FUS){
		pS = strchr(pN,'_',0);	
		if(pS){
			if( *(pS+1) != 'm')	memcpy(pS+2,"-m",2);else memcpy(pS+1,"n-m",3);
		}
		pS = strchr(pN,'_',6);	if(pS) memcpy(pS+1,"BGRN",4); 
	}
// 	else if (lvl==IL_MSC){
// 		pS = strchr(pN,'_',5);	if(pS) memcpy(pS+1,"msc",3);
// 	}
	return true;
}

bool CMIPFile::GenerateMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath)
{
	const static LPCSTR strFileType[]	= {"metadata","imagedata","rpcfile","browseImage","thumbImage","geoRangeFile"};//"RadioModelFile",
	const static LPCSTR strFileExt[]	= {".xml",".tif","_rpc.txt","_pre.jpg","_ico.jpg","_Geo.shp"};
	const static LPCSTR strLvl[]		= {"Unknow","STRIP","SC","EPI","DSM","DEM","GEC","eGEC","GTC","FUS","DOG","MSC"};
	const static LPCSTR strRoot[]		= {"root","root","sensor_corrected_metadata","EPI_metadata","DSM_metadata","DEM_metadata",
		"GEC_metadata","eGEC_metadata","GTC_metadata","FUS_metadata","root","MSC_metadata"};
	
	int idx = 0;
	//	if( !CZY3SceneFile::GenerateMetaFile(lpOrderXml,lpstrOrgMeta,lpMetaPath) ) return false;
	CXmlFile meta,orgmeta;
	if(  !orgmeta.Open(lpstrOrgMeta) ) return false;		
	
//	printf("Source Meta File:%s\n",lpstrOrgMeta);//	orgmeta.FindSibingNode("sensor_corrected_metadata");	printf("%s",orgmeta.GetNodeContent());
	for(idx=0; idx<12;idx++) if( orgmeta.FindSibingNode(strRoot[idx]) != NOT_GET ) break;	if(idx>=12) { printf("The format of meta is Wrong!\n");	return false; }	
	char strMsg[512];//	orgmeta.IntoNode();	
	int lvlIdx = m_imgInfo.lvl;		

	{
		FILE* fp = fopen(lpMetaPath,"w");	if(!fp) { printf("Fail to Create Meta File:%s\n",lpMetaPath);	return false;}
		fprintf(fp,"<%s>%s</%s>",strRoot[lvlIdx],orgmeta.GetNodeContent(),strRoot[lvlIdx]);
		fclose(fp);
	}
//	meta.InsertNode(strRoot[lvlIdx],orgmeta.GetNodeContent());	orgmeta.Close();
//	printf("Product Meta File:%s\n",lpMetaPath);	
//	meta.Save(lpMetaPath); meta.Close();	
	
	if( !meta.Open(lpMetaPath) ) return false;
	
	char strFileName[256];	strcpy(strFileName,strrpath(m_imgInfo.path)+1);	char* pS = NULL;	pS = strrchr(strFileName,'.');
	meta.FindNode(Mode_Absolute,NULL,2,strRoot[lvlIdx],"productComponents");	meta.IntoNode();
	while(  meta.FindSibingNode("file") == FORWARD_GET )
	{
		if( meta.FindChildNode("FileType",strMsg) == NOT_GET ) continue;
		for (idx=0; idx<6; idx++){
			if( !strcmp(strMsg,strFileType[idx]) ) break;
		}
		
		if(idx>=6) { meta.RemoveNode(); continue; }
		strcpy(pS,strFileExt[idx]);
		if( meta.FindChildNode("Filename",strMsg) != NOT_GET ) { meta.SetChildData(strFileName); }
	}
	
	if( meta.FindNode(Mode_Absolute,NULL,2,strRoot[lvlIdx],"productInfo") )
	{
		meta.IntoNode();		
		meta.FindSibingNode("Productlevel");	meta.SetData(strLvl[lvlIdx]);
		meta.OutNode();
	}
	
	meta.FindNode(Mode_Absolute,NULL,2,strRoot[lvlIdx],"processInfo");	meta.IntoNode();
	time_t lt = time(0); 
	struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
	sprintf(strMsg, "%d",st.tm_year+1900);
	pS = strMsg + strlen(strMsg);
	if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
	pS = strMsg + strlen(strMsg);
	if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
	pS = strMsg + strlen(strMsg);
	if( st.tm_hour < 10 )  sprintf(pS," 0%d",st.tm_hour);	else sprintf(pS," %d",st.tm_hour);
	pS = strMsg + strlen(strMsg);
	if( st.tm_min < 10 )  sprintf(pS,":0%d",st.tm_min);	else sprintf(pS,":%d",st.tm_min);
	pS = strMsg + strlen(strMsg);
	if( st.tm_sec < 10 )  sprintf(pS,":0%d",st.tm_sec);	else sprintf(pS,":%d",st.tm_sec);
	meta.FindSibingNode("ProductTime");				meta.SetData(strMsg);
	meta.FindSibingNode("GeometryMethod",strMsg);	meta.SetData(strLvl[lvlIdx]);	if( meta.FindSibingNode("DataSource")!=NOT_GET ) { meta.SetData(strMsg);	}
	meta.OutNode();
	
	meta.Save(lpMetaPath);

	return true;
}

bool	CMIPFile::GenerateEpiMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath)
{
	CXmlFile meta,meta0;	if( !meta0.Open(lpstrOrgMeta) ) return false;

	char strFWDName[512],strBWDName[512],strNADName[512];
	char strTmp[512];

	int i,j;
	meta.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	meta.AddNode("epipolar_line_metadata");	meta.IntoNode();

	meta.AddNode("productComponents");	meta.IntoNode();
	{
		const char* strModelType[] = {"f-n","b-n","f-b"};
		const char* strTag[] = {"LeftImageName","LeftImageFormat","LeftRpcName","RightImageName","RightImageFormat","RightRpcName"};
		const char* strFileName[6] ;
		strFileName[0] = strFWDName;	strFileName[1] = strNADName;
		strFileName[2] = strBWDName;	strFileName[3] = strNADName;
		strFileName[4] = strFWDName;	strFileName[5] = strBWDName;
		
		for (i=0; i<3; i++){
			meta.AddNode("file");		meta.IntoNode();
			meta.AddNode("ModelType",strModelType[i]);
			meta.AddNode("Location");	sprintf(strTmp,"./%s/",strModelType[i]);	meta.SetData(strTmp);
			for (j=0; j<2; j++){
				meta.AddNode(strTag[3*j+0],strFileName[2*i+j]);	
				meta.AddNode(strTag[3*j+1],"Tif");
				strcpy(strTmp,strFileName[2*i+j]);	strcpy(strrchr(strTmp,'.'),"_rpc.txt");
				meta.AddNode(strTag[3*j+2],strTmp);
			}
			meta.AddNode("MeanParallax","0.8");
			meta.OutNode();
		}
	}
	meta.OutNode();

	meta.AddNode("productInfo");	meta.IntoNode();
	{
		meta.AddNode("ProductLevel","EPI");
		meta.AddNode("SatelliteID","ZY3-1");
		
		meta0.FindNode(Mode_Absolute,strTmp,3,"sensor_corrected_metadata","productInfo","ReceiveStationID");
		meta.AddNode("ReceiveStationID",strTmp);

		meta0.FindSibingNode("OrbitID",strTmp);	meta.AddNode("OrbitID",strTmp);
		meta0.FindSibingNode("SceneID",strTmp);	meta.AddNode("SceneID",strTmp);
	}
	meta.OutNode();

	meta.AddNode("processInfo");	meta.IntoNode();
	{
		char *pS;
		time_t lt = time(0); 
		struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
		sprintf(strTmp, "%d",st.tm_year+1900);
		pS = strTmp + strlen(strTmp);
		if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
		pS = strTmp + strlen(strTmp);
		if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
		pS = strTmp + strlen(strTmp);
		if( st.tm_hour < 10 )  sprintf(pS," 0%d",st.tm_hour);	else sprintf(pS," %d",st.tm_hour);
		pS = strTmp + strlen(strTmp);
		if( st.tm_min < 10 )  sprintf(pS,":0%d",st.tm_min);	else sprintf(pS,":%d",st.tm_min);
		pS = strTmp + strlen(strTmp);
		if( st.tm_sec < 10 )  sprintf(pS,":0%d",st.tm_sec);	else sprintf(pS,":%d",st.tm_sec);
		meta.AddNode("ProductTime",strTmp);
		
		meta0.FindNode(Mode_Absolute,strTmp,3,"sensor_corrected_metadata","processInfo","ProduceID");
		meta.AddNode("ProduceID",strTmp);

		meta0.FindNode(Mode_Absolute,strTmp,3,"sensor_corrected_metadata","productInfo","ProductLevel");
		meta.AddNode("DataSource",strTmp);
		
		meta.AddNode("HeightMode","AVE-Height");
		meta.AddNode("ResamplingKernel","Cubic_Convolution_4P");
	}
	meta.OutNode();

	meta.AddNode("distributionInfo");
	meta0.FindNode(Mode_Absolute,NULL,2,"sensor_corrected_metadata","distributionInfo");
	meta.SetData(meta0.GetNodeContent());

	meta0.Close();
	meta.Save(lpMetaPath);

	return true;
}
//////////////////////////////////////////////////////////////CDQDAFile

IMPLEMENT_DYNFILELOAD(CDQDAFile,CZY3SceneFile)

CDQDAFile::CDQDAFile()
{
	
}
CDQDAFile::~CDQDAFile()
{
	
}

CLObject* CDQDAFile ::identify(LPCSTR lpstrPath)
{
	char strLine[1024];	strcpy(strLine,lpstrPath);
	bool bType = false;
	
	Dos2Unix(strLine);	char* pN = strrchr(strLine,'/');	if(!pN) pN = strLine-1;	pN++;
	if( MatchString(pN,DQDA_TAG) != 0) { bType = true;  }
	
	
	if( bType )	return CDQDAFile::CreateLObject();
	else return NULL;
	
}

int CDQDAFile::TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo)
{
	static int nID = 990000;
	char	strPath[512];		
	char	sceneID[10];	memset(sceneID,0,10*sizeof(char));
	char	orbitID[10];	memset(orbitID,0,10*sizeof(char));
	char	sID[10];		memset(sID,0,10*sizeof(char));
	
	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",pN); return false; }
	
	char imglvl[5];	memset(imglvl,0,5);	memcpy(imglvl,pT-6,3);
	IMG_LVL lvl = IL_UNKNOW;	CAM_TYPE	cam = CT_UNKNOW;
	
	if( !strcmp(imglvl,"MSC") )
	{
		lvl = IL_MSC;	
	}else
	{
		if(*(pN+5) == 'M')	{	cam = CT_MUX;	}else
			if(*(pN+5) == 'F')	{	cam = CT_FWD;	}else
				if(*(pN+5) == 'N')	{	cam = CT_NAD;	}else
					if(*(pN+5) == 'B')	{	cam = CT_BWD;	}
					
					if( !strcmp(imglvl,"L1A") )	lvl = IL_SC;else
						//		if( !strcmp(imglvl,"DSM") ) lvl = IL_DSM;else
						if( !strcmp(imglvl,"DEM") ) lvl = IL_DSM;else
							if( !strcmp(imglvl,"EPI") ) lvl = IL_EPI;else
								if( !strcmp(imglvl,"L2A") ) lvl = IL_CJZ;else
									if( !strcmp(imglvl,"L3A") ) lvl = IL_JJZ;else
										if( !strcmp(imglvl,"DOM") ) lvl = IL_DOM;else
											if( !strcmp(imglvl,"FUS") ) lvl = IL_FUS;
											else	{ printf("Unknow image level %s of %s !\n",imglvl,pN); return false;	}
											
											memcpy(orbitID,pN+9,6);	memcpy(sceneID,pN+34,3);	sprintf(sID,"%s%s",orbitID,sceneID);
											
	}
	
	pInfo->lvl = lvl;
	pInfo->cam = cam;
	
	strcpy(pInfo->orbitID,orbitID);
	strcpy(pInfo->sceneID,sceneID);
	pInfo->nID = nID; nID++;
	
	return ERR_NONE;
}

int CDQDAFile::InitExInfo(LPCSTR lpstrPath,IMG_LVL lvl)
{
	char	strPath[512];	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	GetOriFilePath(pN,lvl,m_imgExInfo.strAdjRpcNam,true);
	GetOriFilePath(pN,lvl,m_imgExInfo.strOrgRpcNam,false);
	
	GetThumbPath(pN,m_imgExInfo.strTumbNam);
	GetBrowsePath(pN,m_imgExInfo.strBrowseNam);
	GetMetaPath(pN,m_imgExInfo.strMetaNam);
	
	*pN = 0;	strcpy(m_imgExInfo.strFileDir,strPath);
	return ERR_NONE;
}

int CDQDAFile :: Load4File(LPCSTR lpstrPath)
{
	int ret = TinyLoad(lpstrPath);
	if( ret != ERR_NONE) return ret;
	
	ret = InitExInfo(lpstrPath,m_imgInfo.lvl);
	if( ret != ERR_NONE) return ret;
	
	return CZY3SceneFile::Load4File(lpstrPath);
}

bool CDQDAFile::GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj /* = true */)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	
	if( lvl == IL_SC ) {
		if( bAdj ) strcpy(pT,".rpc.adj.txt");
		else strcpy(pT,".rpc.txt");
	}else if( lvl == IL_EPI )
	{
		strcpy(pT,".rpc");
	}
	else strcpy(pT,".tfw");
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;
}

bool CDQDAFile::GetThumbPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".thumb.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;
}

bool CDQDAFile::GetBrowsePath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".browse.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CDQDAFile::GetMetaPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".meta.xml") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CDQDAFile::RenameImage(char* strPath,IMG_LVL lvl)
{
	const static LPCSTR strLvl[] = {"Unknow","STRIP","L1A","EPI","DEM","DEM","L2A","L3A","DOM","FUS","DOG","MSC"};
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",strPath); return false; }
	memcpy(pT-6,strLvl[lvl],3);
	
	return true;
}
bool CDQDAFile::GenerateMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath)
{
	const static LPCSTR strLvl[] = {"Unknow","STRIP","L1A","EPI","DEM","DEM","L2A","L3A","DOM","FUS","DOG","MSC"};
	if( !CZY3SceneFile::GenerateMetaFile(lpOrderXml,lpstrOrgMeta,lpMetaPath) ) return false;
	CXmlFile order,meta;
	if( !order.Open(lpOrderXml) || !meta.Open(lpMetaPath) ) return false;
	char strMsg[512];	
	meta.FindNode(Mode_Absolute,NULL,3,"ProductMeta","ProductInfo","TaskID");
	order.FindSiblingAttrib("task","id",strMsg);	meta.SetData(strMsg);
	
	time_t lt = time(0); char* pS;
	struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
	sprintf(strMsg, "%d",st.tm_year+1900);
	pS = strMsg + strlen(strMsg);
	if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
	pS = strMsg + strlen(strMsg);
	if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
	pS = strMsg + strlen(strMsg);
	if( st.tm_hour < 10 )  sprintf(pS," 0%d",st.tm_hour);	else sprintf(pS," %d",st.tm_hour);
	pS = strMsg + strlen(strMsg);
	if( st.tm_min < 10 )  sprintf(pS,":0%d",st.tm_min);	else sprintf(pS,":%d",st.tm_min);
	pS = strMsg + strlen(strMsg);
	if( st.tm_sec < 10 )  sprintf(pS,":0%d",st.tm_sec);	else sprintf(pS,":%d",st.tm_sec);
	meta.FindSibingNode("ProductTime");	meta.SetData(strMsg);
	
	meta.FindSibingNode("ProductID");	pS = strrchr(m_imgInfo.path,'/');	if(!pS) pS = strrchr(m_imgInfo.path,'\\');	if(!pS) pS = pS-1;	pS++;	strcpy(strMsg,pS);
	pS = strrchr(strMsg,'.');	*pS=0;	meta.SetData(strMsg);
	
	meta.FindSibingNode("ProductLevel");	strcpy(strMsg,strLvl[m_imgInfo.lvl]);	meta.SetData(strMsg);
	
	meta.FindNode(Mode_Absolute,NULL,3,"ProductMeta","ImageInfo","DataSize");	
	{ double sz = getfilesize(m_imgInfo.path);	sz /=(1024*1024); sprintf(strMsg,"%lf",sz); meta.SetData(strMsg);	}
	{
#ifdef WIN32
		CZY3Image img;	
		if ( img.Open(m_imgInfo.path) ){
			int nCols = img.GetCols();	int nRows = img.GetRows();
			sprintf(strMsg,"%d",nCols);
			meta.FindSibingNode("NumPixels");	meta.SetData(strMsg);
			meta.FindSibingNode("Width");	meta.SetData(strMsg);
			sprintf(strMsg,"%d",nRows);
			meta.FindSibingNode("NumLines");	meta.SetData(strMsg);
			meta.FindSibingNode("Height");	meta.SetData(strMsg);
			img.Close();
		}
#endif
	}
	meta.FindSibingNode("ProductSavePath");	meta.SetData(m_imgInfo.path);
	
	order.Close();	meta.Save(lpMetaPath);
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*     CGFFILE             */

IMPLEMENT_DYNFILELOAD(CGFFile,CZY3SceneFile)

CGFFile::CGFFile()
{
	
}
CGFFile::~CGFFile()
{
	
}

CLObject* CGFFile ::identify(LPCSTR lpstrPath)
{
	char strLine[1024];	strcpy(strLine,lpstrPath);
	bool bType = false;
	
	Dos2Unix(strLine);	char* pN = strrchr(strLine,'/');	if(!pN) pN = strLine-1;	pN++;
	if( MatchString(pN,CGF_TAG) != 0) { bType = true;  }
	
	if( bType )	return CGFFile::CreateLObject();
	else return NULL;
	
}


int CGFFile::TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo)
{
	static int nID = 990000;
	char	strPath[512];		
	char	sceneID[10];	memset(sceneID,0,10*sizeof(char));
	char	orbitID[10];	memset(orbitID,0,10*sizeof(char));
	char	sID[10];		memset(sID,0,10*sizeof(char));
	
	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",pN); return false; }
	
	char imglvl[5];	memset(imglvl,0,5);	memcpy(imglvl,pT-6,3);
	IMG_LVL lvl = IL_UNKNOW;	CAM_TYPE	cam = CT_UNKNOW;
	
	if( !strcmp(imglvl,"MSC") )
	{
		lvl = IL_MSC;	
	}else
	{
		if(*(pN+5) == 'M')	{	cam = CT_MUX;	}else
				if(*(pN+5) == 'P')	{	cam = CT_NAD;	}
					

					if( !strcmp(imglvl,"L1A") )	lvl = IL_SC;else
						//		if( !strcmp(imglvl,"DSM") ) lvl = IL_DSM;else
						if( !strcmp(imglvl,"DEM") ) lvl = IL_DSM;else
							if( !strcmp(imglvl,"EPI") ) lvl = IL_EPI;else
								if( !strcmp(imglvl,"L2A") ) lvl = IL_CJZ;else
									if( !strcmp(imglvl,"L3A") ) lvl = IL_JJZ;else
										if( !strcmp(imglvl,"DOM") ) lvl = IL_DOM;else
											if( !strcmp(imglvl,"FUS") ) lvl = IL_FUS;
											else	{ printf("Unknow image level %s of %s !\n",imglvl,pN); return false;	}
											
											memcpy(orbitID,pN+9,6);	memcpy(sceneID,pN+34,3);  memcpy(sceneID+3,pN+7,1);
											sprintf(sID,"%s%s",orbitID,sceneID);
											
	}
	
	pInfo->lvl = lvl;
	pInfo->cam = cam;
	
	strcpy(pInfo->orbitID,orbitID);
	strcpy(pInfo->sceneID,sceneID);
	pInfo->nID = nID; nID++;
	
	return ERR_NONE;
}

int CGFFile::Load4File(LPCSTR lpstrPath)
{	
	
	int ret = TinyLoad(lpstrPath);
	if( ret != ERR_NONE) return ret;
	
	ret = InitExInfo(lpstrPath,m_imgInfo.lvl);
	if( ret != ERR_NONE) return ret;
	
	return CZY3SceneFile::Load4File(lpstrPath);
}

int CGFFile::InitExInfo(LPCSTR lpstrPath,IMG_LVL lvl)
{
	char	strPath[512];	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	GetOriFilePath(pN,lvl,m_imgExInfo.strAdjRpcNam,true);
	GetOriFilePath(pN,lvl,m_imgExInfo.strOrgRpcNam,false);
	
	GetThumbPath(pN,m_imgExInfo.strTumbNam);
	GetBrowsePath(pN,m_imgExInfo.strBrowseNam);
	GetMetaPath(pN,m_imgExInfo.strMetaNam);
	
	*pN = 0;	strcpy(m_imgExInfo.strFileDir,strPath);
	return ERR_NONE;
}


bool CGFFile::GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj /* = true */)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	
	if( lvl == IL_SC ) {
		if( bAdj ) strcpy(pT,".rpc.adj.txt");
		else strcpy(pT,".rpb");
	}else if( lvl == IL_EPI )
	{
		strcpy(pT,".rpc");
	}
	else strcpy(pT,".tfw");
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;
}

bool CGFFile::GetThumbPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".thumb.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;
}

bool CGFFile::GetBrowsePath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".browse.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CGFFile::GetMetaPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".meta.xml") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CGFFile::RenameImage(char* strPath,IMG_LVL lvl)
{
	const static LPCSTR strLvl[] = {"Unknow","STRIP","L1A","EPI","DEM","DEM","L2A","L3A","DOM","FUS","DOG","MSC"};
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",strPath); return false; }
	memcpy(pT-6,strLvl[lvl],3);
	
	return true;
}

bool CGFFile::GenerateMetaFile(LPCSTR lpOrderXml,LPCSTR lpstrOrgMeta,LPCSTR lpMetaPath)
{
	const static LPCSTR strLvl[] = {"Unknow","STRIP","L1A","EPI","DEM","DEM","L2A","L3A","DOM","FUS","DOG","MSC"};
	if( !CZY3SceneFile::GenerateMetaFile(lpOrderXml,lpstrOrgMeta,lpMetaPath) ) return false;
	CXmlFile order,meta;
	if( !order.Open(lpOrderXml) || !meta.Open(lpMetaPath) ) return false;
	char strMsg[512];	
	meta.FindNode(Mode_Absolute,NULL,3,"ProductMeta","ProductInfo","TaskID");
	order.FindSiblingAttrib("task","id",strMsg);	meta.SetData(strMsg);
	
	meta.FindNode(Mode_Absolute,NULL,3,"ProductMeta","ProductInfo","OrderID");
	order.FindSiblingAttrib("task","orderid",strMsg);	meta.SetData(strMsg);
	
	time_t lt = time(0); char* pS;
	struct tm	st;		memcpy( &st,localtime(&lt),sizeof(struct tm) ); 
	sprintf(strMsg, "%d",st.tm_year+1900);
	pS = strMsg + strlen(strMsg);
	if( st.tm_mon+1 < 10 )  sprintf(pS,"-0%d",st.tm_mon+1);	else sprintf(pS,"-%d",st.tm_mon+1);
	pS = strMsg + strlen(strMsg);
	if( st.tm_mday < 10 )  sprintf(pS,"-0%d",st.tm_mday);	else sprintf(pS,"-%d",st.tm_mday);
	pS = strMsg + strlen(strMsg);
	if( st.tm_hour < 10 )  sprintf(pS," 0%d",st.tm_hour);	else sprintf(pS," %d",st.tm_hour);
	pS = strMsg + strlen(strMsg);
	if( st.tm_min < 10 )  sprintf(pS,":0%d",st.tm_min);	else sprintf(pS,":%d",st.tm_min);
	pS = strMsg + strlen(strMsg);
	if( st.tm_sec < 10 )  sprintf(pS,":0%d",st.tm_sec);	else sprintf(pS,":%d",st.tm_sec);
	meta.FindSibingNode("ProductTime");	meta.SetData(strMsg);
	
	meta.FindSibingNode("ProductID");	pS = strrchr(m_imgInfo.path,'/');	if(!pS) pS = strrchr(m_imgInfo.path,'\\');	if(!pS) pS = pS-1;	pS++;	strcpy(strMsg,pS);
	pS = strrchr(strMsg,'.');	*pS=0;	meta.SetData(strMsg);
	
	meta.FindSibingNode("ProductLevel");	strcpy(strMsg,strLvl[m_imgInfo.lvl]);	meta.SetData(strMsg);
	
	meta.FindNode(Mode_Absolute,NULL,3,"ProductMeta","ImageInfo","DataSize");	
	{ double sz = getfilesize(m_imgInfo.path);	sz /=(1024*1024); sprintf(strMsg,"%lf",sz); meta.SetData(strMsg);	}
	{
#ifdef WIN32
		CZY3Image img;	
		if ( img.Open(m_imgInfo.path) ){
			int nCols = img.GetCols();	int nRows = img.GetRows();
			sprintf(strMsg,"%d",nCols);
			meta.FindSibingNode("NumPixels");	meta.SetData(strMsg);
			meta.FindSibingNode("Width");	meta.SetData(strMsg);
			sprintf(strMsg,"%d",nRows);
			meta.FindSibingNode("NumLines");	meta.SetData(strMsg);
			meta.FindSibingNode("Height");	meta.SetData(strMsg);
			img.Close();
		}
#endif
	}
	meta.FindSibingNode("ProductSavePath");	meta.SetData(m_imgInfo.path);
	
	order.Close();	meta.Save(lpMetaPath);
	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////PGSFile

IMPLEMENT_DYNFILELOAD(CPGSFile,CZY3SceneFile)
CPGSFile::CPGSFile()
{
	
}
CPGSFile::~CPGSFile()
{
	
}

CLObject* CPGSFile ::identify(LPCSTR lpstrPath)
{
	char strLine[1024];	strcpy(strLine,lpstrPath);
	bool bType = false;
	
	Dos2Unix(strLine);	char* pN = strrchr(strLine,'/');	if(!pN) pN = strLine-1;	pN++;
	if( MatchString(pN,PGSFile_TAG1) != 0) { bType = true;  }else
		if( MatchString(pN,PGSFile_TAG2) != 0) { bType = true;  }else
			if( MatchString(pN,PGSFile_TAG3) != 0) { bType = true;  }
			
			
			if( bType )	return CPGSFile::CreateLObject();
			else return NULL;
}

int CPGSFile::TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo)
{
	static int nID = 990000;
	char	strPath[512];		
	char	sceneID[10];	memset(sceneID,0,10*sizeof(char));
	char	orbitID[10];	memset(orbitID,0,10*sizeof(char));
	char	sID[20];		memset(sID,0,20*sizeof(char));
	
	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",pN); return ERR_FILE_TYPE; }
	
	int i=0;char* pS;
	char imglvl[5];	memset(imglvl,0,5);	pS = strrchr(strPath,'_');	memcpy(imglvl,pS+1,3);
	IMG_LVL lvl = IL_UNKNOW;	CAM_TYPE	cam = CT_UNKNOW;
	
	if( !strcmp(imglvl,"MSC") )
	{
		lvl = IL_MSC;	
	}else
	{
		//		if((*(pN+4) != 'T')&&(*(pN+4) != 'M')) { printf("Warning:Image Name %s is not standard!\n",pN); return ERR_FILE_TYPE;}
		if(*(pN+4) == 'M' || *(pT-3) == 'M' )	{	cam = CT_MUX;	}else
			if(*(pN+4) == 'F' || *(pT-3) == 'F' )	{	cam = CT_FWD;	}else
				if(*(pN+4) == 'N' || *(pT-3) == 'N' )	{	cam = CT_NAD;	}else
					if(*(pN+4) == 'B' || *(pT-3) == 'B' )	{	cam = CT_BWD;	}
					
					if( !strcmp(imglvl,"L1A") )	lvl = IL_SC;else
						if( !strcmp(imglvl,"DSM") ) lvl = IL_DSM;else
							if( !strcmp(imglvl,"DEM") ) lvl = IL_DEM;else
								if( !strcmp(imglvl,"EPI") ) lvl = IL_EPI;else
									if( !strcmp(imglvl,"L2A") ) lvl = IL_CJZ;else
										if( !strcmp(imglvl,"L3A") ) lvl = IL_JJZ;else
											if( !strcmp(imglvl,"DOM") ) lvl = IL_DOM;else
												if( !strcmp(imglvl,"FUS") ) lvl = IL_FUS;
												else	{ printf("Unknow image level %s of %s !\n",imglvl,pN); return ERR_FILE_TYPE;	}
												
												pS = strchr(pN,'_');	
												pS = strchr(pS+1,'_');	strccpy(pS+1,orbitID,'_'); 
												pS = strchr(pS+1,'_');  strccpy(pS+1,sceneID,'_');
												char strMetaPath[512];	GetMetaPath(lpstrPath,strMetaPath);
												GetOribitSceneID4PGSMeta(strMetaPath,orbitID,sceneID);
												//		memcpy(orbitID,pN+8,6);	memcpy(sceneID,pN+15,5);	
												sprintf(sID,"%s%s",orbitID,sceneID);
												
	}
	
	pInfo->lvl = lvl;
	pInfo->cam = cam;
	
	strcpy(pInfo->orbitID,orbitID);
	strcpy(pInfo->sceneID,sceneID);
	//	pInfo->nID = atoi(sID);
	pInfo->nID = nID;	nID++;
	
	return ERR_NONE;
}

int CPGSFile::InitExInfo(LPCSTR lpstrPath,IMG_LVL lvl)
{
	char	strPath[512];	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	GetOriFilePath(pN,lvl,m_imgExInfo.strAdjRpcNam,true);
	GetOriFilePath(pN,lvl,m_imgExInfo.strOrgRpcNam,false);
	
	GetThumbPath(pN,m_imgExInfo.strTumbNam);
	GetBrowsePath(pN,m_imgExInfo.strBrowseNam);
	GetMetaPath(pN,m_imgExInfo.strMetaNam);
	
	*pN = 0;	strcpy(m_imgExInfo.strFileDir,strPath);
	return ERR_NONE;
}

int CPGSFile :: Load4File(LPCSTR lpstrPath)
{
	int ret = TinyLoad(lpstrPath);
	if( ret != ERR_NONE) return ret;
	
	ret = InitExInfo(lpstrPath,m_imgInfo.lvl);
	if( ret != ERR_NONE) return ret;
	
	return CZY3SceneFile::Load4File(lpstrPath);
}

bool CPGSFile::GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj /* = true */)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	
	if( lvl == IL_SC  ) {
		if( bAdj ) strcpy(pT,".adj.rpc");
		else strcpy(pT,".rpb");
	}else 
		if( lvl == IL_EPI )
		{
			strcpy(pT,".rpc");
		}
		else	strcpy(pT,".tfw");
		
		strcpy(retPath,strPath);
		
		//	if( IsExist(strPath) ) return true;
		// 	else return false;
		
		return true;
}

bool CPGSFile::GetThumbPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',"_thumb.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;
}

bool CPGSFile::GetBrowsePath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CPGSFile::GetMetaPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".xml") ) return false;
	
	char* pS = strrchr(strPath,'-');
	if( pS ) {
		pS++;
		if( !memcmp(pS,"BWD",3*sizeof(char)) || !memcmp(pS,"FWD",3*sizeof(char)) ) memcpy(pS,"DLC",3*sizeof(char));
	}
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CPGSFile::RenameImage(char* strPath,IMG_LVL lvl)
{
	static LPCSTR strLvl[] = {"Unknow","STRIP","L1A","EPI","DSM","DEM","L2A","L3A","DOM","FUS","DOG","MSC"};
	Dos2Unix(strPath);
	char* pN = strrchr(strPath,'_');	
	memcpy(pN+1,strLvl[lvl],3);
	
	if(lvl!=IL_SC) strcpy(strrchr(pN,'.'),".tif");
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////PGSGF01File

IMPLEMENT_DYNFILELOAD(CPGSGF01File,CZY3SceneFile)
CPGSGF01File::CPGSGF01File()
{
	
}
CPGSGF01File::~CPGSGF01File()
{
	
}

CLObject* CPGSGF01File ::identify(LPCSTR lpstrPath)
{
	char strLine[1024];	strcpy(strLine,lpstrPath);
	bool bType = false;
	
	Dos2Unix(strLine);	char* pN = strrchr(strLine,'/');	if(!pN) pN = strLine-1;	pN++;

	if( MatchString(pN,PGSGF01File_TAG1) != 0) { bType = true;  }else
		if( MatchString(pN,PGSGF01File_TAG2) != 0) { bType = true;  }
		
	if( bType )	return CPGSGF01File::CreateLObject();
	else return NULL;
}

int CPGSGF01File::TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo)
{
	static int nID = 990000;
	char	strPath[512];		
	char	sceneID[15];	memset(sceneID,0,15*sizeof(char));
	char	orbitID[10];	memset(orbitID,0,10*sizeof(char));
	char	sID[25];		memset(sID,0,25*sizeof(char));
	
	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",pN); return ERR_FILE_TYPE; }
	
	int i=0;char* pS;
	char imglvl[5];	memset(imglvl,0,5);	pS = strrchr(strPath,'_');	memcpy(imglvl,pS+1,3);
	char imgCam[5];	memset(imgCam,0,5); pS = strrchr(pN,'-');	
	if(pS)	memcpy(imgCam,pS+1,3);	else { pS = strchr(pN,'_');	memcpy(imgCam,pS+1,3);	}
	IMG_LVL lvl = IL_UNKNOW;	CAM_TYPE	cam = CT_UNKNOW;
	
	if( !strcmp(imglvl,"MSC") )
	{
		lvl = IL_MSC;	
	}else
	{
		if( !strcmp(imgCam,"MSS") )	{	cam = CT_MUX;	}else
				if( !strcmp(imgCam,"PAN") )	{	cam = CT_NAD;	}else
					if( !strcmp(imgCam,"WFV") ) { cam = CT_MUX;	}
					
					if( !strcmp(imglvl,"L1A") )	lvl = IL_SC;else
						if( !strcmp(imglvl,"DSM") ) lvl = IL_DSM;else
							if( !strcmp(imglvl,"DEM") ) lvl = IL_DEM;else
								if( !strcmp(imglvl,"EPI") ) lvl = IL_EPI;else
									if( !strcmp(imglvl,"L2A") ) lvl = IL_CJZ;else
										if( !strcmp(imglvl,"L3A") ) lvl = IL_JJZ;else
											if( !strcmp(imglvl,"DOM") ) lvl = IL_DOM;else
												if( !strcmp(imglvl,"FUS") ) lvl = IL_FUS;
												else	{ printf("Unknow image level %s of %s !\n",imglvl,pN); return ERR_FILE_TYPE;	}
												
												pS = strchr(pN,'_');	
												pS = strchr(pS+1,'_');	strccpy(pS+1,orbitID,'_'); 
												pS = strchr(pS+1,'_');  strccpy(pS+1,sceneID,'_');
												char strMetaPath[512];	GetMetaPath(lpstrPath,strMetaPath);
												GetOribitSceneID4PGSMeta(strMetaPath,orbitID,sceneID);
												//		memcpy(orbitID,pN+8,6);	memcpy(sceneID,pN+15,5);	
												sprintf(sID,"%s%s",orbitID,sceneID);
												pS = sceneID+strlen(sceneID);	
												{ pT = strchr(pN,'_');	pT = strchr(pT+1,'_');	*pS = *(pT-1);	*(pS+1) = 0; }
												if( !strcmp(imgCam,"WFV") )	strcpy(pS+1,imgCam);
	}
	
	pInfo->lvl = lvl;
	pInfo->cam = cam;
	
	strcpy(pInfo->orbitID,orbitID);
	strcpy(pInfo->sceneID,sceneID);
	//	pInfo->nID = atoi(sID);
	pInfo->nID = nID;	nID++;
	
	return ERR_NONE;
}

int CPGSGF01File::InitExInfo(LPCSTR lpstrPath,IMG_LVL lvl)
{
	char	strPath[512];	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	GetOriFilePath(pN,lvl,m_imgExInfo.strAdjRpcNam,true);
	GetOriFilePath(pN,lvl,m_imgExInfo.strOrgRpcNam,false);
	
	GetThumbPath(pN,m_imgExInfo.strTumbNam);
	GetBrowsePath(pN,m_imgExInfo.strBrowseNam);
	GetMetaPath(pN,m_imgExInfo.strMetaNam);
	
	*pN = 0;	strcpy(m_imgExInfo.strFileDir,strPath);
	return ERR_NONE;
}

int CPGSGF01File :: Load4File(LPCSTR lpstrPath)
{
	int ret = TinyLoad(lpstrPath);
	if( ret != ERR_NONE) return ret;
	
	ret = InitExInfo(lpstrPath,m_imgInfo.lvl);
	if( ret != ERR_NONE) return ret;
	
	return CZY3SceneFile::Load4File(lpstrPath);
}

bool CPGSGF01File::GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj /* = true */)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	
	if( lvl == IL_SC  ) {
		if( bAdj ) strcpy(pT,".adj.rpc");
		else strcpy(pT,".rpb");
	}else 
		if( lvl == IL_EPI )
		{
			strcpy(pT,".rpc");
		}
		else	strcpy(pT,".tfw");
		
		strcpy(retPath,strPath);
		
		//	if( IsExist(strPath) ) return true;
		// 	else return false;
		
		return true;
}

bool CPGSGF01File::GetThumbPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',"_thumb.jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;
}

bool CPGSGF01File::GetBrowsePath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".jpg") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CPGSGF01File::GetMetaPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	if( !strrcnct(strPath,'.',".xml") ) return false;
	
	strcpy(retPath,strPath);
	
	//	if( IsExist(strPath) ) return true;
	// 	else return false;
	
	return true;	
}

bool CPGSGF01File::RenameImage(char* strPath,IMG_LVL lvl)
{
	static LPCSTR strLvl[] = {"Unknow","STRIP","L1A","EPI","DSM","DEM","L2A","L3A","DOM","FUS","DOG","MSC"};
	Dos2Unix(strPath);
	char* pN = strrchr(strPath,'_');	
	memcpy(pN+1,strLvl[lvl],3);

	if(lvl!=IL_SC) strcpy(strrchr(pN,'.'),".tif");
	
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNFILELOAD(CSatPrjFile,CZY3SceneFile)

bool CSatPrjFile::m_bInit = false;
char CSatPrjFile::m_strIniFilePath[] = "";
char CSatPrjFile::m_strLvlNam[][10] = {""};
char CSatPrjFile::m_strCamNam[][10] = {""};
int CSatPrjFile::m_nType = -1;
int CSatPrjFile::m_nScenePos = -1;
int CSatPrjFile::m_nSceneLen = 0;
int CSatPrjFile::m_nOribitPos = -1;
int CSatPrjFile::m_nOribitLen = 0;
int CSatPrjFile::m_nLvlPos = -1;
int CSatPrjFile::m_nLvlLen = 0;
int CSatPrjFile::m_nCamPos = -1;
int CSatPrjFile::m_nCamLen = 0;

char	CSatPrjFile::m_strOrgRpcEx[]	= "";
bool	CSatPrjFile::m_bOrgRpcExDot		= true;
char	CSatPrjFile::m_strAdjRpcEx[]	= "";	
bool	CSatPrjFile::m_bAdjRpcExDot		= true;
char	CSatPrjFile::m_strThumbEx[]		= "";	
bool	CSatPrjFile::m_bThumbExDot		= true;
char	CSatPrjFile::m_strBrowseEx[]	= "";	
bool	CSatPrjFile::m_bBrowseExDot		= true;
char	CSatPrjFile::m_strMetaEx[]		= "";	
bool	CSatPrjFile::m_bMetaExDot		=true;

CSatPrjFile::CSatPrjFile()
{
	
}
CSatPrjFile::~CSatPrjFile()
{
	
}
CLObject* CSatPrjFile ::identify(LPCSTR lpstrPath)
{
	char strLine[1024];	strcpy(strLine,lpstrPath);
	bool bType = false;	
	
	char execname[256]="";  readlink( "/proc/self/exe",execname,256 );
	Dos2Unix(execname);		char* pS = strrchr(execname,'/');	if(!pS) return NULL;
	
	strcpy(pS,"/ZY3FileName.ini");
	while(!IsExist(execname))
	{
		*pS = 0;	pS = strrchr(execname,'/');	if(!pS) return NULL;
		strcpy(pS,"/ZY3FileName.ini");	
	}
	strcpy(m_strIniFilePath,execname);
	
	GetPrivateProfileString(FILE_ROOT_NAME,"FileTypeNum","0",strLine,126,m_strIniFilePath);
	
	int num = atoi(strLine);	int tagNum;
	char tagNam[50];	int i,j;
	
	for ( i=0; i<num; i++){
		sprintf(tagNam,"File%d_TagNum",i);
		GetPrivateProfileString(FILE_ROOT_NAME,tagNam,"0",strLine,126,m_strIniFilePath);
		
		char strFileNam[MAX_PATHNAME];	strcpy(strFileNam,lpstrPath);
		Dos2Unix(strFileNam);	char* pN = strrchr(strFileNam,'/');	if(!pN) pN = strFileNam-1;	pN++;
		
		tagNum = atoi(strLine);
		for (j=0; j<tagNum; j++){
			sprintf(tagNam,"File%d_Tag%d",i,j);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,"0",strLine,126,m_strIniFilePath);
			if( MatchString(pN,strLine) != 0 ) { 
				m_nType = i;	bType = true; 
				break;
			}
		}
		if(bType) break;
	}
	
	if( bType )	{
		if (!m_bInit){
			m_bInit = true;
			sprintf(tagNam,"File%d_Lvl",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,"-1 0",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%d%d",&m_nLvlPos,&m_nLvlLen);	if(m_nLvlLen>5) m_nLvlLen = 5;
			
			sprintf(tagNam,"File%d_Cam",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,"-1 0",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%d%d",&m_nCamPos,&m_nCamLen);	if(m_nCamLen>5) m_nCamLen = 5;
			
			sprintf(tagNam,"File%d_Scene",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,"-1 0",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%d%d",&m_nScenePos,&m_nSceneLen);	//if(m_nSceneLen>9) m_nSceneLen = 9;
			
			sprintf(tagNam,"File%d_Oribit",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,"-1 0",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%d%d",&m_nOribitPos,&m_nOribitLen);	//if(m_nOribitLen>9) m_nOribitLen = 9;
			
			sprintf(tagNam,"File%d_LvlName",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,"Uno STRIP L1A EPI DSM DEM L2A L3A DOM FUS DOG MSC",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%s%s%s%s%s%s%s%s%s%s%s%s",m_strLvlNam[0],m_strLvlNam[1],m_strLvlNam[2],
				m_strLvlNam[3],m_strLvlNam[4],m_strLvlNam[5],m_strLvlNam[6],m_strLvlNam[7],m_strLvlNam[8],
				m_strLvlNam[9],m_strLvlNam[10],m_strLvlNam[11]);
			
			sprintf(tagNam,"File%d_CamName",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,"uno FWD NAD BWD MUX",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%s%s%s%s%s",m_strCamNam[0],m_strCamNam[1],m_strCamNam[2],
				m_strCamNam[3],m_strCamNam[4]);
			
			int nTmp = 1;
			sprintf(tagNam,"File%d_OrgRpcName",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,".rpc 1",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%s%d",m_strOrgRpcEx,&nTmp);	if( nTmp != 0) m_bOrgRpcExDot = true;else m_bOrgRpcExDot = false; 
			
			sprintf(tagNam,"File%d_AdjRpcName",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,".rpc 1",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%s%d",m_strAdjRpcEx,&nTmp);	if( nTmp != 0) m_bAdjRpcExDot = true;else m_bAdjRpcExDot = false;
			
			sprintf(tagNam,"File%d_ThumbName",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,".rpc 1",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%s%d",m_strThumbEx,&nTmp);	if( nTmp != 0) m_bThumbExDot = true;else m_bThumbExDot = false;
			
			sprintf(tagNam,"File%d_BrowseName",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,".rpc 1",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%s%d",m_strBrowseEx,&nTmp);	if( nTmp != 0) m_bBrowseExDot = true;else m_bBrowseExDot = false;
			
			sprintf(tagNam,"File%d_MetaName",m_nType);
			GetPrivateProfileString(FILE_ROOT_NAME,tagNam,".rpc 1",strLine,126,m_strIniFilePath);
			sscanf(strLine,"%s%d",m_strMetaEx,&nTmp);	if( nTmp != 0) m_bMetaExDot = true;else m_bMetaExDot = false;
		}
		return CSatPrjFile::CreateLObject();
	}
	else return NULL;
}


int CSatPrjFile::TinyLoad(LPCSTR lpstrPath,ImageInfo* pInfo)
{
	if( m_nType <0 ) { return ERR_FILE_TYPE;	}
	
	static int nID = 990000;
	char	strPath[512];		
	char	sceneID[128];	memset(sceneID,0,128*sizeof(char));
	char	orbitID[128];	memset(orbitID,0,128*sizeof(char));
	
	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	char* pT = strrchr(strPath,'.');	if(!pT) { printf("Can't find the ext of %s!\n",pN); return ERR_FILE_TYPE; }
	
	int len = (int)strlen(pN);
	
	char imglvl[6];	memset(imglvl,0,6);	if(m_nLvlPos != POS_INVALID && m_nLvlPos+m_nLvlLen< len ) memcpy(imglvl,pN+m_nLvlPos,m_nLvlLen); else strcpy(imglvl,"uno");
	char imgcam[6];	memset(imgcam,0,6);	if(m_nCamPos != POS_INVALID && m_nCamPos+m_nCamLen< len ) memcpy(imgcam,pN+m_nCamPos,m_nCamLen); else strcpy(imgcam,"uno");
	
	IMG_LVL lvl = IL_UNKNOW;	CAM_TYPE	cam = CT_UNKNOW;
	
	if( !strcmp(imglvl,m_strLvlNam[IL_MSC]) )
	{
		lvl = IL_MSC;	
	}else
	{
		int i;
		for(i=0; i<CT_END; i++)		{ if( !strcmp(imgcam,m_strCamNam[i]) )	{	cam = (CAM_TYPE)i; break;	} }
		for(i=0; i<TTID_END; i++)	{ if( !strcmp(imglvl,m_strLvlNam[i]) )	{	lvl = (IMG_LVL)i; break;	} }
		
		if(m_nScenePos != POS_INVALID && m_nScenePos+m_nSceneLen< len ) memcpy(sceneID,pN+m_nScenePos,m_nSceneLen); else strcpy(sceneID,"0");
		if(m_nOribitPos != POS_INVALID && m_nOribitPos+m_nOribitLen< len ) memcpy(orbitID,pN+m_nOribitPos,m_nOribitLen); else strcpy(orbitID,"0");
		
		if(m_nLvlPos == POS_INVALID)	lvl = IL_SC;
		if(m_nCamPos == POS_INVALID)	cam = CT_NAD;
	}
	
	pInfo->lvl = lvl;
	pInfo->cam = cam;
	
	strcpy(pInfo->orbitID,orbitID);
	strcpy(pInfo->sceneID,sceneID);
	pInfo->nID = nID;	nID++;
	
	return ERR_NONE;
}

int CSatPrjFile::InitExInfo(LPCSTR lpstrPath,IMG_LVL lvl)
{
	char	strPath[512];	strcpy(strPath,lpstrPath);	Dos2Unix(strPath);	
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	GetOriFilePath(pN,lvl,m_imgExInfo.strAdjRpcNam,true);
	GetOriFilePath(pN,lvl,m_imgExInfo.strOrgRpcNam,false);
	
	GetThumbPath(pN,m_imgExInfo.strTumbNam);
	GetBrowsePath(pN,m_imgExInfo.strBrowseNam);
	GetMetaPath(pN,m_imgExInfo.strMetaNam);
	
	*pN = 0;	strcpy(m_imgExInfo.strFileDir,strPath);
	return ERR_NONE;
}

int CSatPrjFile :: Load4File(LPCSTR lpstrPath)
{
	
	int ret = TinyLoad(lpstrPath);
	if( ret != ERR_NONE) return ret;
	
	ret = InitExInfo(lpstrPath,m_imgInfo.lvl);
	if( ret != ERR_NONE) return ret;
	
	return CZY3SceneFile::Load4File(lpstrPath);
}

bool CSatPrjFile::GetOriFilePath(LPCSTR strImgPath,IMG_LVL lvl,char* retPath,bool bAdj /* = true */)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	char* pL = strPath+strlen(strPath);
	
	if( lvl == IL_SC  ) {
		if( bAdj ) { if(m_bAdjRpcExDot) strcpy(pT,m_strAdjRpcEx); else strcpy(pL,m_strAdjRpcEx); }
		else { if(m_bOrgRpcExDot) strcpy(pT,m_strOrgRpcEx); else strcpy(pL,m_strOrgRpcEx); }
	}else 
		if( lvl == IL_EPI )
		{
			if(m_bOrgRpcExDot) strcpy(pT,m_strOrgRpcEx); else strcpy(pL,m_strOrgRpcEx);
		}
		else	strcpy(pT,".tfw");
		
		strcpy(retPath,strPath);
		
		return true;
}

bool CSatPrjFile::GetThumbPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	char* pL = strPath+strlen(strPath);
	
	if(m_bThumbExDot) strcpy(pT,m_strThumbEx); else strcpy(pL,m_strThumbEx);
	
	strcpy(retPath,strPath);
	
	return true;
}

bool CSatPrjFile::GetBrowsePath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	char* pL = strPath+strlen(strPath);
	
	if(m_bBrowseExDot) strcpy(pT,m_strBrowseEx); else strcpy(pL,m_strBrowseEx);
	
	strcpy(retPath,strPath);
	
	return true;	
}

bool CSatPrjFile::GetMetaPath(LPCSTR strImgPath,char* retPath)
{
	*retPath = 0;
	char strPath[MAX_PATHNAME];	strcpy(strPath,strImgPath);
	
	char* pT = strrchr(strPath,'.');	if(!pT) return false;
	char* pL = strPath+strlen(strPath);
	
	if(m_bMetaExDot) strcpy(pT,m_strMetaEx); else strcpy(pL,m_strMetaEx);
	
	strcpy(retPath,strPath);
	
	return true;	
}

bool CSatPrjFile::RenameImage(char* strPath,IMG_LVL lvl)
{
	Dos2Unix(strPath);
	char* pN = strrchr(strPath,'/');	if(!pN) pN = strPath-1;		pN++;
	if(m_nLvlPos != POS_INVALID && m_nLvlPos+m_nLvlLen< (int)strlen(pN) ) memcpy(pN+m_nLvlPos,m_strLvlNam[lvl],m_nLvlLen);
	else{
		pN = strrchr(strPath,'.');	if(!pN) return false;
		sprintf(pN,"_%s.tif",m_strLvlNam[lvl]);
	}
	
	return true;
}

