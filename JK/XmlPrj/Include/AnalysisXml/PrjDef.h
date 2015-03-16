////PrjDef.h
/********************************************************************
	PrjDef
	created:	2013/09/26
	author:		LX 
	purpose:	This file is for PrjDef function
*********************************************************************/
#if !defined PrjDef_h__LX_2013_9_26
#define PrjDef_h__LX_2013_9_26

#include "LObject.h"
#include "LxString.hpp"
#include <string.h>
#include <errno.h>


#define ORIBIT_INVALID	"-1"

enum TASK
{
	TSK_UNKNOW		=	0x0000,
	TSK_ATMCH	=	0x0001,
	TSK_ATADJ	=	0x0002,
	TSK_DEMMCH	=	0x0004,
	TSK_DEMFLT	=	0x0008,
	TSK_IMGEPI	=	0x0010,
	TSK_IMGREC	=	0x0020,
	TSK_IMGFUS	=	0x0040,
	TSK_IMGDOG	=	0x0080,
	TSK_IMGMSC	=	0x0100,
	TSK_DSM_MAP	=	0x0200,
	TSK_DEM_MAP	=	0x0400,
	TSK_IMG_MAP	=	0x0800,
	TSK_ATEDIT	=	0x1000,
	TSK_DEMEDIT	=	0x2000,
	TSK_DEMQC	=	0x4000,
	TSK_IMGQC	=	0x8000,
	TSK_IMGEDIT	=	0x10000,
	TSK_END		=	0x20000,
};

enum TASK_ID
{
	TTID_UNKNOW		=	0,
	TTID_ATMCH	,
	TTID_ATADJ	,
	TTID_DEMMCH	,
	TTID_DEMFLT	,
	TTID_IMGEPI	,
	TTID_IMGREC	,
	TTID_IMGFUS	,
	TTID_IMGDOG	,
	TTID_IMGMSC	,
	TTID_DSM_MAP,
	TTID_DEM_MAP,
	TTID_IMG_MAP,
	TTID_ATEDIT	,
	TTID_DEMEDT	,
	TTID_DEMQC	,
	TTID_IMGQC	,
	TTID_IMGEDT	,
	TTID_END,
};

static LPCSTR g_strTaskName[] = {"uno","AtMch","AtAdj","DemMch","DemFlt","ImgEpi","ImgRec","ImgFus","ImgDog","ImgMsc",
								"DsmMap","DemMap","ImgMap","AtEdit","DemEdit","DemQC","ImgQC","ImgEdit","end"};

#define ADD_BINARY(a,b)	{ if( !(a&b) ) a = a|b;	}//添加任务的

enum CAM_TYPE
{
	CT_UNKNOW = 0,
	CT_FWD,
	CT_NAD,
	CT_BWD,
	CT_MUX,
	CT_END,
};

enum IMG_LVL
{
	IL_UNKNOW = 0,
	IL_STRIP,
	IL_SC,
	IL_EPI,
	IL_DSM,
	IL_DEM,
	IL_CJZ,
	IL_JJZ,
	IL_DOM,
	IL_FUS,
	IL_DOG,
	IL_MSC,
	IL_END,
};

#ifndef _PRJHEADER
#define _PRJHEADER
typedef struct tagPrjHdr{
	char		jobName[100];//订单中TaskID的值
	char		jobID[100];//订单中JobID的值
	LPCSTR		vesion;
}PrjHdr;
#endif

#ifndef __MESSAGE_PORT
#define __MESSAGE_PORT
typedef struct tagMessagePort{
	char			adr[128];
	unsigned int	port;
}MessagePort;
#endif

#ifndef _IMAGEINFO
#define _IMAGEINFO
struct tagImageInfo;
typedef	struct tagPIMAGEINFO{
	struct tagImageInfo**	p;
	unsigned int			id;
}PIMAGEINFO;
typedef struct tagImageInfo{
	int			nID;
	char		path[MAX_PATHNAME];
	CAM_TYPE	cam;
	IMG_LVL		lvl;
	int			nBlockID;			//-1 for not choose
	char		sceneID[128];
	char		orbitID[128];

	PIMAGEINFO	pNext;
//	struct tagImageInfo*	pNext;
}ImageInfo,*LPIMAGEINF;
#endif

#ifndef _IMGEXINFO
#define _IMGEXINFO
typedef struct tagImgExInfo{
	char		strFileDir[MAX_PATHNAME];
	char		strAdjRpcNam[MAX_PATHNAME];
	char		strOrgRpcNam[MAX_PATHNAME];
	char		strTumbNam[MAX_PATHNAME];
	char		strBrowseNam[MAX_PATHNAME];
	char		strMetaNam[MAX_PATHNAME];

// 	char		strAdjRpcNam[MAX_FILENAME];
// 	char		strOrgRpcNam[MAX_FILENAME];
// 	char		strTumbNam[MAX_FILENAME];
// 	char		strBrowseNam[MAX_FILENAME];
// 	char		strMetaNam[MAX_FILENAME];

	double		fLat_LT;			//左上角纬度
	double		fLong_LT;			//左上角经度
	double		fLat_RB;			//右下角纬度
	double		fLong_RB;			//右下角经度
}ImgExInfo,*LPIMGEXINFO;
#endif

#ifndef _PROJECTIONINFO
#define _PROJECTIONINFO
typedef struct tagProjectionInfo{
	int		nEarthModelID;	//WGS84、CGCS2000等
	int		nMapProjID;		//Gauss 、UTM
	double	fPrjScale;		
	int		nPrjHemi;		
	double	fPrjOriLat;
	double	fPrjCenMeri;
	double	fPrjFalseNor;
	double	fPrjFalseEas;
	int		nZoneType;
	int		nZoneNo;
	int		nElevationID;
	double	fBur7Par[7];
}ProjectionInfo;
#endif

#ifndef _PRODINFO
#define _PRODINFO
typedef struct tagProdInfo{
	char	strGcdPath[MAX_PATHNAME];//ProdGcd
	char	strTempletPath[MAX_PATHNAME];
	//////////////////////////////////////////////////////////////////////////Lxy140829
	char    strGcpGcdPath[MAX_PATHNAME];
	char    strPar7GcdPath[MAX_PATHNAME];
	float   fDomPANGSD;
	float   fDomMUXGSD;
	/************************************************************************/
	float	fDemGSD;
	float	fDomGSD;
	float	fAverHeight;
}ProdInfo;
#endif

#ifndef _SCENEINFO
#define _SCENEINFO
typedef struct tagSceneInfo{
	unsigned long sceneId;
	char	sID[256];// [OrbitID+SceneID]
	char	sOrbitID[128];
    double	centE,centN;
	
    char	strNameImgF[512];
    char	strNameImgN[512];
    char	strNameImgB[512];
    char	strNameImgC[512];
		
	PIMAGEINFO	imgList;
//	ImageInfo* imgList;
}SceneInfo,*LPSCENEINFO;
#endif

class  CArray_ImageInfo
{
public:
	CArray_ImageInfo( ImageInfo* pBuf=NULL,int size=0 ){	m_maxSize = m_size = size; m_pBuf = NULL;if ( m_size ) m_pBuf = new ImageInfo[m_size]; if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(ImageInfo)*m_size ); };
	virtual ~CArray_ImageInfo(){ if (m_pBuf) delete []m_pBuf; };
	
	inline	ImageInfo*	GetData(int &size){ size=m_size; return m_pBuf; };
	inline	int			Append( ImageInfo uint ){ if ( m_size >= m_maxSize ){  m_maxSize += 30; ImageInfo* pOld = m_pBuf; m_pBuf	 = new ImageInfo[m_maxSize]; memset( m_pBuf,0,sizeof(ImageInfo)*m_maxSize ); memcpy( m_pBuf,pOld,sizeof(ImageInfo)*m_size );	delete []pOld; } m_pBuf[m_size]=uint;m_size++;return (m_size-1); };
	inline	void		SetSize(int size ){ if (size<m_maxSize) m_size=size; else Reset(NULL,size);  };
    inline	void		Reset( ImageInfo* pBuf=NULL,int size=0 ){ if (m_pBuf) delete []m_pBuf; m_pBuf = NULL; m_maxSize	= size; m_size = 0; if ( m_maxSize ){ m_pBuf = new ImageInfo[m_maxSize]; memset( m_pBuf,0,sizeof(ImageInfo)*m_maxSize ); } if ( pBuf ) { m_size = size;	memcpy( m_pBuf,pBuf,sizeof(ImageInfo)*m_size ); } }
    inline  ImageInfo&   operator[](int idx){ return m_pBuf[idx];   };
	inline	ImageInfo	GetAt(int idx){ if(idx>=m_size) { printf("Warning:Ask for too much image!\n"); return m_pBuf[m_size-1]; } return m_pBuf[idx];		   };
	inline	int			GetSize(){ return m_size;				   };

	inline	ImageInfo**	GetBufAdr() { return &m_pBuf;	}
	inline	int			GetImgNum( IMG_LVL lvl,CAM_TYPE cam=CT_END,LPCSTR oribit=NULL,LPCSTR scene=NULL){
		int i;	ImageInfo* pInfo=m_pBuf;	int num=0;
		for (i=0;i<m_size;i++,pInfo++){ 
			if( pInfo->lvl != lvl && lvl != IL_END ) continue;	//pInfo->lvl
			if( pInfo->cam != cam && cam != CT_END ) continue; //pInfo->cam
			if( oribit !=NULL && ( strcmp(oribit,pInfo->orbitID) != 0 ) ) continue;
			if( scene !=NULL && ( strcmp(scene,pInfo->sceneID) != 0 ) ) continue;
			num++;
		}
		return num;
	};
private:
	ImageInfo*		m_pBuf;
	int				m_size;
	int				m_maxSize;
};

class CArray_ImgExInfo
{ 
typedef ImgExInfo Array_Point;
#define NUM_APPEND	20	
public:
	CArray_ImgExInfo( Array_Point* pBuf=NULL,int size=0 ){	m_maxSize = m_size = size; m_pBuf = NULL;if ( m_size ) m_pBuf = new Array_Point[m_size];
	if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(Array_Point)*m_size ); };
	virtual ~CArray_ImgExInfo(){ if (m_pBuf) delete []m_pBuf; };
	
	inline	Array_Point*	GetData(){ return m_pBuf; };	
	inline	Array_Point*	GetData(int &size){ size=m_size; return m_pBuf; };
	inline	void	RemoveLast(){ if (m_size>0) m_size--; };
   	inline  void	RemoveAt(int idx){ if (idx<m_size){ for (int i=idx;i<m_size-1;i++) m_pBuf[i]=m_pBuf[i+1]; m_size--; } };
   	inline  int	    InsertAt(int idx,Array_Point uint){ Append(uint); for (int i=m_size-1;i>idx;i--) m_pBuf[i]=m_pBuf[i-1]; m_pBuf[idx]=uint; return idx; };
	inline	int	    Append( Array_Point uint ){ 
		if ( m_size >= m_maxSize ){  
			m_maxSize += NUM_APPEND; 
			Array_Point* pOld = m_pBuf; m_pBuf	 = new Array_Point[m_maxSize]; 
			//			memset( m_pBuf,0,sizeof(Array_Point)*m_maxSize );	
			memcpy( m_pBuf,pOld,sizeof(Array_Point)*m_size ); delete []pOld; 
		} 
		m_pBuf[m_size]=uint;m_size++;
		return (m_size-1); 
	};
	inline	void	SetSize(int size ){ if (size<=m_maxSize) m_size=size; else Reset(NULL,size);  };
    inline	void	Reset( Array_Point* pBuf=NULL,int size=0 ){ 
		if (m_pBuf) delete []m_pBuf; m_pBuf = NULL; 
		m_maxSize = size;m_size = 0; 
		if ( m_maxSize ){ 
			m_pBuf = new Array_Point[m_maxSize]; 
			//			memset( m_pBuf,0,sizeof(Array_Point)*m_maxSize ); 
		} 
		if ( pBuf ){ m_size = size;	memcpy( m_pBuf,pBuf,sizeof(Array_Point)*m_size ); }
	}
    inline  Array_Point&  operator[](int idx){ return m_pBuf[idx];   };
	inline	Array_Point	GetAt(int idx){ return m_pBuf[idx];		   };
	inline	int		GetSize(){ return m_size;				   };
private:
	Array_Point*	m_pBuf;
	int		m_size;
	int		m_maxSize;
	
};

class  CArray_SceneInfo
{
public:
	CArray_SceneInfo( SceneInfo* pBuf=NULL,int size=0 ){	m_maxSize = m_size = size; m_pBuf = NULL;if ( m_size ) m_pBuf = new SceneInfo[m_size]; if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(SceneInfo)*m_size ); };
	virtual ~CArray_SceneInfo(){ if (m_pBuf) delete []m_pBuf; };
	
	inline	SceneInfo*	GetData(int &size){ size=m_size; return m_pBuf; };
	inline	int			Append( SceneInfo uint ){ if ( m_size >= m_maxSize ){  m_maxSize += 30; SceneInfo* pOld = m_pBuf; m_pBuf	 = new SceneInfo[m_maxSize]; memset( m_pBuf,0,sizeof(SceneInfo)*m_maxSize ); memcpy( m_pBuf,pOld,sizeof(SceneInfo)*m_size );	delete []pOld; } m_pBuf[m_size]=uint;m_size++;return (m_size-1); };
	inline	void		SetSize(int size ){ if (size<m_maxSize) m_size=size; else Reset(NULL,size);  };
    inline	void		Reset( SceneInfo* pBuf=NULL,int size=0 ){ if (m_pBuf) delete []m_pBuf; m_pBuf = NULL; m_maxSize	= size; m_size = 0; if ( m_maxSize ){ m_pBuf = new SceneInfo[m_maxSize]; memset( m_pBuf,0,sizeof(SceneInfo)*m_maxSize ); } if ( pBuf ) { m_size = size;	memcpy( m_pBuf,pBuf,sizeof(SceneInfo)*m_size ); } }
    inline  SceneInfo&   operator[](int idx){ return m_pBuf[idx];   };
	inline	SceneInfo	GetAt(int idx){ if(idx>=m_size) { printf("Warning:Ask for too much Scene!\n"); return m_pBuf[m_size-1]; } return m_pBuf[idx];		   };
	inline	int			GetSize(){ return m_size;				   };
	
	inline	SceneInfo**	GetBufAdr( ) { return &m_pBuf;	}
	int					GetID(ImageInfo* imgInf) {
				if( imgInf->sceneID[0] == '\0' ) { char* pS = strrchr(imgInf->path,'/'); if(!pS) pS = strrchr(imgInf->path,'\\'); if(!pS) pS =imgInf->path-1; printf("Error:UnKnow Image Name Type:%s!\n",pS+1); return -1; }
				char sID[256];	sprintf(sID,"%s%s",imgInf->orbitID,imgInf->sceneID);
				int i,pos_Buf = -1;
				for (i=0;i<m_size;i++){ if(!strcmp(sID,(m_pBuf+i)->sID)) { pos_Buf = i; break; } }
				return pos_Buf;
				};
	inline	void		AppendImageArray(CArray_ImageInfo& imgArray) {
		int i=0;int num = imgArray.GetSize();	ImageInfo** pImgInfo=imgArray.GetBufAdr();
		for (;i<num;i++){ AppendImage(pImgInfo,i); } 
	}
	inline	void		AppendImageList(ImageInfo** imgBase,int st,int num) {
		int i=0;	
		for (;i<num;i++){ AppendImage(imgBase,st+i); } 
	}
	inline	bool		AppendImage(ImageInfo** imgBase,int id){
			ImageInfo* imgInf = *imgBase+id;
			if(imgInf->lvl == IL_MSC||imgInf->lvl == IL_UNKNOW) return false;
			int pos_Buf = GetID(imgInf);
			if( pos_Buf == -1) { //pos_Buf == -1说明还没有存储过这一景，此景是新的一景，是可以存储的
				SceneInfo uint; memset(&uint,0,sizeof(SceneInfo)); 
				uint.sceneId = atoi(imgInf->sceneID); 
				strcpy(uint.sOrbitID,imgInf->orbitID);
				sprintf(uint.sID,"%s%s",imgInf->orbitID,imgInf->sceneID);
				Append(uint); pos_Buf = m_size-1; //存储//仅只存储了sceneID，orbitID，sID这三项信息//下面的代码是补充信息的，若是外部dem则仅此三项信息
			} //pos_Buf != -1说明这一景已经存在过，须忽略该景

			//补充景信息
			SceneInfo* pBuf = m_pBuf+pos_Buf;//获取已经存在的（或刚刚添加的）景信息

			if( GetImage(pos_Buf,imgInf->lvl,imgInf->cam) ) { printf("Ignore Repeat in Scene:%s.\n",pBuf->sID);	return false; }
			
			if ( imgInf->lvl == IL_SC ){
				switch (imgInf->cam)
				{
				case CT_FWD:
					strcpy(pBuf->strNameImgF,imgInf->path);	
					break;
				case CT_NAD:
					strcpy(pBuf->strNameImgN,imgInf->path);	
					break;
				case CT_BWD:
					strcpy(pBuf->strNameImgB,imgInf->path);	
					break;
				case CT_MUX:
					strcpy(pBuf->strNameImgC,imgInf->path);	
					break;
				}
			}
			
			imgInf->pNext = pBuf->imgList;//??????
//			pBuf->imgList = imgInf;
			pBuf->imgList.p = imgBase;	pBuf->imgList.id = id;

			return true;
	};
	inline	ImageInfo*	GetImage(int idx,IMG_LVL lvl,CAM_TYPE cam = CT_END){
		SceneInfo* pBuf = m_pBuf+idx;	ImageInfo* pImgInf = pBuf->imgList.p?*(pBuf->imgList.p)+pBuf->imgList.id:NULL;
		while (pImgInf){ 
			if( pImgInf->lvl == lvl)
			{
				if( cam == CT_END || pImgInf->cam == cam )
					return pImgInf;
			}
			pImgInf = pImgInf->pNext.p?*(pImgInf->pNext.p)+pImgInf->pNext.id:NULL;
		}
		return NULL;
	}
private:
	SceneInfo*		m_pBuf;
	int				m_size;
	int				m_maxSize;
};

inline bool SaveProdGcdFile(const char* lpstrPath,ProjectionInfo& projectionInfo ){
	//	ProdInf* pProdInf = m_tCont.prodInf;
	const char* title[7] = {"Dx=","Dy=","Dz=","Rx=","Ry=","Rz=","Scale="};
	
	FILE* fp = fopen(lpstrPath,"w");	if(!fp) { printf("%s\n",strerror(errno)); return false; }
	
	fprintf(fp,"[GeoCvtInf]\n");
	fprintf(fp,"Ellipsoid=%d\n",projectionInfo.nEarthModelID);
	fprintf(fp,"Projectid=%d\n",projectionInfo.nMapProjID);
	fprintf(fp,"EllipsoA=6378137.000000000\nEllipsoB=6356752.314245000\n");
	fprintf(fp,"ProjectScale=%lf\n",projectionInfo.fPrjScale);
	fprintf(fp,"ProjectHemi=%d\n",projectionInfo.nPrjHemi);
	fprintf(fp,"ProjectOriLat=%lf\n",projectionInfo.fPrjOriLat);
	fprintf(fp,"ProjectCenMeri=%lf\n",projectionInfo.fPrjCenMeri);
	fprintf(fp,"ProjectFalseNor=%lf\n",projectionInfo.fPrjFalseNor);
	fprintf(fp,"ProjectFalseEas=%lf\n",projectionInfo.fPrjFalseEas);
	fprintf(fp,"Elevationid=%d\n",projectionInfo.nElevationID);
	fprintf(fp,"ElevationHeiAdj=\n");
	fprintf(fp,"ProjectZoneNo=%d\n",projectionInfo.nZoneNo);
	
	
	fprintf(fp,"[Param7Inf]\n");
	
	int i;
	for (i=0;i<7;i++){
		fprintf(fp,"%s%e\n",title[i],projectionInfo.fBur7Par[i]);
	}
	
	fclose(fp);
	
	return true;
}

#endif // PrjDef_h__LX_2013_9_26