// SpGeoCvt.h : main header file for the SPGEOCVT DLL
/*----------------------------------------------------------------------+
|		SpGeoCvt											 		    |
|       Author:     DuanYanSong  2008/08/13								|
|            Ver 1.0													|
|       Copyright (c) 2008, Supresoft Corporation                       |
|	         All rights reserved.                                       |
|       http://www.supresoft.com.cn                                     |
|		ysduan@supresoft.com.cn;ysduan@sohu.com                         |
+----------------------------------------------------------------------*/
#ifndef SPGEOCVT_H_DUANYANSONG_2008_08_13_15_28_40156
#define SPGEOCVT_H_DUANYANSONG_2008_08_13_15_28_40156

#include "math.h"

#ifndef SPGEOCVT_LIB

#define SPGEOCVT_LIB  __declspec(dllimport)
#define SPGEOCVT_IMP
#ifdef _DEBUG_SPGEOCVT
#pragma comment(lib,"SpGeoCvtD.lib") 
#pragma message("Automatically linking with SpGeoCvtD.lib") 
#else
#pragma comment(lib,"SpGeoCvt.lib") 
#pragma message("Automatically linking with SpGeoCvt.lib") 
#endif

#else

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "resource.h"		// main symbols

#endif

#define SPGC_PI					3.1415926535897932384626433832795e0
#define SPGC_R2D				57.295779513082320876798154814105e0
#define SPGC_D2R				0.017453292519943295769236907684886e0

enum ELPS_TYPE
{
    ET_WGS84        = 0,
    ET_BEIJING54    = 1,
    ET_XIAN80       = 2,
    ET_CN2000       = 3,
    ET_CUSTOM       = 4,    
};

#define SEMIMAJOR_WGS84	        6378137.0	
#define SEMIMINOR_WGS84		    6356752.31424517929

#define SEMIMAJOR_BEIJING54	    6378245.0	
#define SEMIMINOR_BEIJING54		6356863.018773047267

#define SEMIMAJOR_XIAN80	    6378140.0	
#define SEMIMINOR_XIAN80		6356755.2881575287

#define SEMIMAJOR_CN2000	    6378137.0	
#define SEMIMINOR_CN2000		6356752.3141

enum PROJECTION_TYPE
{
    GEODETIC_LONLAT     = 0,   //经纬度
	TM_PROJECTION       = 1,   //横轴墨卡托投影	
    GAUSS_PROJECTION    = 2,   //高斯－克吕格投影	
	UTM_PROJECTION      = 3,   //通用横轴墨卡托投影
    LAMBERT_PROJECTION  = 4,   //兰波特等角投影
    GEODETIC_CENTRXYZ   = 5,   //地心坐标    
};

enum ELEVATION_TYPE
{
    GEODETIC_HEI    =  0,
    GEOID_DATA      =  1,
};

#define MakeLambert2Scale(Std_Par1,Std_Par2)  ( floor(Std_Par1*100000.0)*1000+Std_Par2 )
#define GetLambert1(Scale)  ( floor(Scale/100000.0)/1000.0  )
#define GetLambert2(Scale)  ( Scale-floor(Scale/1000)*1000 )


inline double DFM2D(double dfm )
{
    double degree = int(dfm);
    double minute = int((dfm - degree) * 100);
    double second = ((dfm -degree)* 100 - minute) * 100;
    if (second >= 60) { minute++; second = ((dfm -degree)* 100 - minute) * 100; }
    return  degree + minute / 60 + second / 3600;
}

inline double D2DFM(double dfm,double *dd=NULL,double *ff=NULL,double *mm=NULL )
{
    double degree = int(dfm);
    double minute = int((dfm - degree) * 60);
    double second = (dfm - degree - minute/60) * 3600;
    if (second >= 60) { minute++; second -= 60; }    
    if (dd) *dd = degree; if(ff) *ff = minute; if(mm) *mm = second;
    return degree + minute / 100 + second / 10000;
}


class SPGEOCVT_LIB CSpGeoCvt
{
public:
    CSpGeoCvt();
    virtual ~CSpGeoCvt();

    BOOL   Set_Cvt_Par_ByGUI();
    
    void   Set_Cvt_Par( int elpsType,
                        int projType,
                        double a,double b,
                        double Origin_Latitude,  // rad
                        double Central_Meridian, // rad
                        double False_Easting ,
                        double False_Northing,
                        double Scale_Factor,
                        int eleType,
                        LPCSTR lpstrHeiAdj=NULL );
    void   Get_Cvt_Par( int *elpsType,
                        int *projType,
                        double *a,double *b,
                        double *Origin_Latitude , // rad
                        double *Central_Meridian, // rad
                        double *False_Easting ,
                        double *False_Northing,
                        double *Scale_Factor,
                        int *eleType,
                        char *lpstrHeiAdj=NULL );

    BOOL   Load4File(LPCSTR lpstrPathName);
    BOOL   Save2File(LPCSTR lpstrPathName);
    LPCSTR GetFilePN(){ return m_strPathName; };

    int    Cvt_Prj2LBH(double gx,double gy,double gz,double *lon,double *lat,double *hei);
    int    Cvt_LBH2Prj(double lon,double lat,double hei,double *gx,double *gy,double *gz); 
    
    int    Cvt_LBH2CXYZ(double lon,double lat,double hei,double *cx,double *cy,double *cz);
    int    Cvt_CXYZ2LBH(double cx,double cy,double cz,double *lon,double *lat,double *hei);

    static void Cvt_TPC2LBH_WGS84( double Xt,double Yt,double Zt,
		                          double Initial_B,double Initial_H,
		                          double L0,double B0,double H0,
		                          double *L,double *B,double *H )
    {
		double temp_L = -10000.0;
		double temp_B = Initial_B;
		double temp_H = Initial_H;
		double WGS84_a = SEMIMAJOR_WGS84;
		double WGS84_b = SEMIMINOR_WGS84;
		double WGS84_e = sqrt((WGS84_a*WGS84_a - WGS84_b*WGS84_b)/(WGS84_a*WGS84_a));
		double N0      = WGS84_a/sqrt(1 - WGS84_e*WGS84_e*sin(B0)*sin(B0));
        double N,WGS84_A,WGS84_B,WGS84_C,WGS84_D,WGS84_E,tt;		
		do{
			*L = temp_L;    *B = temp_B;    *H = temp_H;			
			N = WGS84_a/sqrt(1 - WGS84_e*WGS84_e*sin(*B)*sin(*B));
			
			WGS84_A = (1 + H0/N0) * cos(B0) - sin(B0)/N0 * Yt + cos(B0)/N0 * Zt;
			WGS84_B = (1 + H0/N0) * sin(B0) + cos(B0)/N0 * Yt + sin(B0)/N0 * Zt;
			WGS84_C = sqrt(WGS84_A*WGS84_A + pow(Xt/N0,2));
			WGS84_D = WGS84_e * WGS84_e * (N0 * sin(B0) - N * sin(*B));
			WGS84_E = sqrt(1 + 2*WGS84_D*sin(*B)/(N + *H) + pow(WGS84_D/(N+ *H),2));
			
			temp_L = atan(Xt / (N0 * WGS84_A)) + L0;
			temp_B = atan(WGS84_B/WGS84_C - WGS84_D/(N0*WGS84_C));                    
            tt = (cos(*B)*sin(temp_L - L0));
            if ( tt!=0 ) temp_H = Xt/tt - N;            
		}while( fabs(*L - temp_L)>1.0e-9||fabs(*B - temp_B)>1.0e-9||fabs(*H - temp_H)>1.0e-9 );
		
		*L = temp_L;    *B = temp_B;    *H = temp_H;
	};
    static void Cvt_LBH2TPC_WGS84( double L,double B,double H,
								  double L0,double B0,double H0,
								  double *Xt,double *Yt,double *Zt )
    {
        double sinB  = sin(B);
        double cosB  = cos(B);
        double sinB0 = sin(B0);
        double cosB0 = cos(B0);
        double sinL  = sin(L);
        double cosL  = cos(L);
        
        double WGS84_a = SEMIMAJOR_WGS84;
        double WGS84_b = SEMIMINOR_WGS84;
        double WGS84_e = sqrt((WGS84_a*WGS84_a - WGS84_b*WGS84_b))/WGS84_a;
        double N  = WGS84_a/sqrt(1 - WGS84_e*WGS84_e*sinB*sinB);
        double N0 = WGS84_a/sqrt(1 - WGS84_e*WGS84_e*sinB0*sinB0);
        double D  = WGS84_e * WGS84_e * (N0 * sinB0 - N * sinB);
        double l  = L - L0;
        
        double sinl  = sin(l);
        double cosl  = cos(l);
        
        *Xt = (N + H) * cosB * sinl;
        *Yt = (N + H) * (sinB * cosB0 - sinB0 * cosB * cosl) + D * cosB0;
        *Zt = (N + H) * (sinB * sinB0 + cosB0 * cosB * cosl) - (N0 + H0) + D * sinB0;
        
    };
    
    // Some Default Value Seting
    static void GetEplsDefPar(int eplsIdx,double *a,double *b,double *e=NULL)
    {
        switch( eplsIdx )
        {
            case ET_WGS84:
                *a = SEMIMAJOR_WGS84;
                *b = SEMIMINOR_WGS84;
                if(e) *e = ( *a - *b ) / *a;            
                break;
            case ET_BEIJING54:
		        *a = SEMIMAJOR_BEIJING54;
		        *b = SEMIMINOR_BEIJING54;
		        if(e)*e = ( *a - *b ) / *a;
                break;
            case ET_XIAN80:
		        *a = SEMIMAJOR_XIAN80;
		        *b = SEMIMINOR_XIAN80;
		        if(e)*e = ( *a - *b ) / *a;
                break;
            case ET_CN2000:
		        *a = SEMIMAJOR_CN2000;
		        *b = SEMIMINOR_CN2000;
		        if(e)*e = ( *a - *b ) / *a;            
                break;
        }
    };
    static void GetProjDefPar(int projIdx,double *scale)
    {
        switch(projIdx)
        {
            case GAUSS_PROJECTION: 
                *scale        = 1;      
                break;
            case UTM_PROJECTION:   
                *scale         = 0.9996; 
                break;
        }
    };
    static void GetUTMDefPar(int zone,char hemiSphere,double *Central_Meridian,double *False_Northing  )
    {
        *Central_Meridian = (6*zone-3)*3.14159265358979323/180.0;
        *False_Northing   = (hemiSphere=='S')?10000000:0;
    };

protected:
    int    m_ElpsType ;
    int    m_ProjType ;
    int    m_eleType  ;
    char   m_strHeiAdj[256];
    char   m_strPathName[256];

    float* m_pGeoId;
private:
    void*  m_pGeoTrans;
};

#ifndef _DPT3D
#define _DPT3D
//!三维点数据定义（double型）
typedef struct tagDPT3D
{
	double x,y,z;
}DPT3D;
#else
#pragma  message("SpGeoCvt.h, Warning: DPT3D alread define,be sure it was define as: struct tagDPT3D{ double x,y,z; }. \
                \nSpGeoCvt.h, 警告:类型 DPT3D 已经定义过,请确保其定义为: struct tagDPT3D{ double x,y,z; }") 
#endif

class SPGEOCVT_LIB CSpPar7Cvt
{
public:
    CSpPar7Cvt(){ m_l2wDx=m_l2wDy=m_l2wDz=0; m_l2wScale=0; 
        memset(m_l2wM,0,sizeof(m_l2wM)); m_l2wM[0] = m_l2wM[4] = m_l2wM[8] = 1.0;
        memset(m_w2lM,0,sizeof(m_w2lM)); m_w2lM[0] = m_w2lM[4] = m_w2lM[8] = 1.0;
        memset(m_strPathName,0,sizeof(m_strPathName));
    };
    virtual ~CSpPar7Cvt(){};
    BOOL   Set_Cvt_Par_ByGUI();    
    BOOL   Set_Cvt_Par_ByGCP( DPT3D *pSrc,DPT3D *pDes,int ptSum,BOOL bDelCross=FALSE );
    void   Set_Cvt_Par( double dx,double dy,double dz,
                        double rx,double ry,double rz,
                        double scale){
                m_l2wDx=dx; m_l2wDy=dy; m_l2wDz=dz; 
                m_l2wM[0] = 1.0 ; m_w2lM[0] = 1.0 ;
                m_l2wM[1] =  rz ; m_w2lM[1] = -rz ;
                m_l2wM[2] = -ry ; m_w2lM[2] =  ry ;
                m_l2wM[3] = -rz ; m_w2lM[3] =  rz ;
                m_l2wM[4] = 1.0 ; m_w2lM[4] = 1.0 ;
                m_l2wM[5] =  rx ; m_w2lM[5] = -rx ;
                m_l2wM[6] =  ry ; m_w2lM[6] = -ry ;
                m_l2wM[7] = -rx ; m_w2lM[7] =  rx ;
                m_l2wM[8] = 1.0 ; m_w2lM[8] = 1.0 ;
                m_l2wScale=scale;
            };
    void    Get_Cvt_Par( double *dx,double *dy,double *dz,
                        double *rx,double *ry,double *rz,
                        double *scale){
                *dx=m_l2wDx;   *dy=m_l2wDy; *dz=m_l2wDz;
                *rx=m_l2wM[5]; *ry=m_l2wM[6]; *rz=m_l2wM[1];
                *scale=m_l2wScale;
            };
    BOOL    Load4File(LPCSTR lpstrPathName);
    BOOL    Save2File(LPCSTR lpstrPathName);
    LPCSTR  GetFilePN(){ return m_strPathName; };
    void    Cvt_WCXYZ2LCXYZ(double wcx,double wcy,double wcz,double *lcx,double *lcy,double *lcz){
                double tgtxyz[3],srcxyz[3]={ wcx,wcy,wcz };
                MatrixMult( m_w2lM,srcxyz,tgtxyz,3,3,1 );        
                *lcx = tgtxyz[0]*( 1.0 - m_l2wScale ) - m_l2wDx;
                *lcy = tgtxyz[1]*( 1.0 - m_l2wScale ) - m_l2wDy;
                *lcz = tgtxyz[2]*( 1.0 - m_l2wScale ) - m_l2wDz; 
            };
    void    Cvt_LCXYZ2WCXYZ(double lcx,double lcy,double lcz,double *wcx,double *wcy,double *wcz){
                double tgtxyz[3],srcxyz[3]={ lcx,lcy,lcz };
                MatrixMult( m_l2wM,srcxyz,tgtxyz,3,3,1 );        
                *wcx = tgtxyz[0]*( 1.0 + m_l2wScale ) + m_l2wDx;
                *wcy = tgtxyz[1]*( 1.0 + m_l2wScale ) + m_l2wDy;
                *wcz = tgtxyz[2]*( 1.0 + m_l2wScale ) + m_l2wDz; 
            };

    static void MatrixMult( double *pa,double *pb,double *pc,int m,int n,int l ){
	            double p; int i,j,u,v,t=0;
	            for ( i=0; i<m; i++ ){ for ( v=0; v<l; v++ ){ p=0; for (j=0,u=0; j<n; j++,u++){ p+=pa[i*n+j]*pb[u*l+v]; } pc[t++]=p;  } }
            };
protected:
    double m_l2wDx,m_l2wDy,m_l2wDz;
    double m_l2wM[9],m_w2lM[9];
    double m_l2wScale;
    char   m_strPathName[256];
};


// 注：本模块转换的基础公式：大地高(84) ＝ 正常高 ＋ 高程异常值 <==> 正常高 = 大地高(84) - 高程异常值
class SPGEOCVT_LIB CSpAltCvt
{
public:
    CSpAltCvt(){ m_pDz = NULL; m_Lon0=m_Lat0=m_dLon=m_dLat=0;m_nLon=m_nLat=0;m_strPathName[0]=0; };
    virtual ~CSpAltCvt(); // { if (m_pDz) delete m_pDz; m_pDz=NULL; }

    BOOL    Set_Cvt_Par_ByGUI( CSpGeoCvt *pGeo=NULL,CSpGeoCvt *pAlt=NULL,CSpPar7Cvt *pPar7=NULL );    
    void    Set_Cvt_Par( double lon0,double lat0,double dlon,double dlat,int nlon,int nlat,float *pDz );
    void    Get_Cvt_Par( double *lon0,double *lat0,double *dlon,double *dlat,int *nlon,int *nlat,float **pDz ){
                *lon0 = m_Lon0; *lat0 = m_Lat0; *dlon = m_dLon; *dlat = m_dLat; 
                *nlon = m_nLon; *nlat = m_nLat;  
                *pDz = m_pDz;
            };
    BOOL    Load4File(LPCSTR lpstrPathName);
    BOOL    Save2File(LPCSTR lpstrPathName);
    LPCSTR  GetFilePN(){ return m_strPathName; };
    float   Get_Alt2Geo(double lon,double lat){ // 特别注意，此处 Alt2Geo 名称不正确，这个返回的是“高程异常值”，获取正常高时需要减去这个返回值。
                if ( m_dLat<=0||m_dLon<=0||m_nLon<=0||m_nLat<=0 ) return 0;
				if ( lon<SPGC_PI && lon>-SPGC_PI && lat<SPGC_PI/2 && lat>-SPGC_PI/2 ){
					lon = lon*SPGC_R2D; lat = lat*SPGC_R2D;
				}
                int row = int( (lat-m_Lat0)/m_dLat );
                int col = int( (lon-m_Lon0)/m_dLon );	
                if ( col<0||row<0||col>=m_nLon-1 || row>=m_nLat-1 ) return 0;
                float dz=0,*p = m_pDz + (row*m_nLon+col);
                if ( *p<9998 && *(p+1)<9998 && *(p+m_nLon)<9998 && *(p+m_nLon+1)<9998 ){
                    double dx = (lon-( col*m_dLon+m_Lon0 ))/m_dLat;
                    double dy = (lat-( row*m_dLat+m_Lat0 ))/m_dLon;
                    dz  = float( (1-dx)*(1-dy)*(*p)+dx*(1-dy)*(*(p+1))+(1-dx)*dy*p[m_nLon]+dx*dy*p[m_nLon+1] );
                }
                return dz;
           };
    float  Get_Geo2Alt(double lon,double lat){ return -1*Get_Alt2Geo(lon,lat); };
    void   Cvt_Alt2Geo(double lon,double lat,double alt,double *hei){ *hei=alt+Get_Alt2Geo(lon,lat); };
    void   Cvt_Geo2Alt(double lon,double lat,double hei,double *alt){ *alt=hei+Get_Geo2Alt(lon,lat); };    
protected:
    double m_Lon0,m_Lat0,m_dLon,m_dLat;
    int    m_nLon,m_nLat;
    float* m_pDz;
    char   m_strPathName[256];
};

#endif
