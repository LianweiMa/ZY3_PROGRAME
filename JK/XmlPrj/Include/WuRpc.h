// WuRpc.h: interface for the CRpc class.
//
//////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------+
|		WuRpc										 					|
|       Author:     DuanYanSong  2011/05/30								|
|            Ver 1.0													|
|       Copyright (c)2011, WHU RSGIS DPGrid Group						|
|	         All rights reserved.                                       |
|		ysduan@whu.edu.cn; ysduan@sohu.com              				|
+----------------------------------------------------------------------*/
#ifndef WURPC_H_DUANYANSONG_2011_05_30_18_49_234567890543567865
#define WURPC_H_DUANYANSONG_2011_05_30_18_49_234567890543567865

#ifndef WURPC_LIB
#define WURPC_LIB 

#ifndef _CONSOLE
#pragma comment( linker,"/NODEFAULTLIB:LIBC.lib")
#endif
#pragma comment(lib,"WuRpc.lib") 
#pragma message("Automatically linking with static lib: WuRpc.lib") 

#endif

// definition of the RPC parameters of satellite imagery
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
#else
#pragma message("SpRpc.h, Warning: RpcPara alread define, be sure it was define as: struct tagRpcPara{ double rpc[90]; }. \
               \nSpRpc.h, Warning: RpcPara alread define, be sure it was define as: struct tagRpcPara{ double rpc[90]; }.") 
#endif
		
#define M_LOWORD(l)           ((unsigned short)(l))
#define M_HIWORD(l)           ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
     
#ifndef _DXDY
#define _DXDY
typedef struct tagDXDY
{
    short dx,dy; // zoom 1000
}DXDY;
#else
#pragma message("WuRpc.h, Warning: DXDY alread define, be sure it was define as: struct tagDXDY{ short dx,dy;; }. \
               \nWuRpc.h, 警告:类型 DXDY 已经定义过,请确保其定义为: struct tagDXDY{ short dx,dy;; }.") 
#endif
        
#ifndef _ORIRPC_GCP
#define _ORIRPC_GCP
typedef struct tagORIRPC_GCP
{
    double lat,lon,alt; // lat,lon 's unit: degree 
    float px,py;        // unit: pixel and up left in image.
    
}ORIRPC_GCP;
#else
#pragma message("WuRpc.h, Warning: ORIRPC_GCP alread define, be sure it was define as: struct tagORIRPC_GCP{ double gx,gy,gz;float ix,iy; }. \
               \nWuRpc.h, 警告:类型 ORIRPC_GCP 已经定义过,请确保其定义为: struct tagORIRPC_GCP{ double gx,gy,gz;float ix,iy; }.") 
#endif

               
class CWuRpc  
{
public:
	CWuRpc();
	virtual ~CWuRpc();
	
	//!RPC
	RpcPara	m_rpcPar;
	double	m_aop6[6];

    int*    m_pDxyGrid; // It saved value is: dx= (realx-rpcx)*1000 , dy=(realy-rpcy)*1000;
    int     m_gridC,m_gridR,m_gridDx,m_gridDy;
    int*    NewGrid(int gridC,int gridR,int dx,int dy){ 
                m_gridC  = gridC; m_gridR  = gridR;
                m_gridDx = dx;    m_gridDy = dy;
                if (m_pDxyGrid) delete m_pDxyGrid; 
                m_pDxyGrid = new int[m_gridC*m_gridR +8]; 
                memset( m_pDxyGrid,0,sizeof(int)*m_gridC*m_gridR );
                return m_pDxyGrid;
            };
    void    GetDxy( double px,double py,double *dx,double *dy ){
                float   x0 = (float)( px/m_gridDx );
                float   y0 = (float)( py/m_gridDy );
                int     lbGridx = int(x0);	
                int     lbGridy = int(y0);	
                float   dx0 = (x0 - lbGridx);
                float   dy0 = (y0 - lbGridy);
                *dx = *dy = 0;
                if( lbGridx>=0 && lbGridx< m_gridC-1 && 
                    lbGridy>=0 && lbGridy< m_gridR-1 ){

                    float z00,z01,z10,z11;
                    int lbOffset = lbGridy * m_gridC + lbGridx;
                    int ltOffset = lbOffset + m_gridC;
                    DXDY* pDxy = (DXDY*)m_pDxyGrid;

                    z00 = pDxy[lbOffset  ].dx/1000.f;
                    z01 = pDxy[lbOffset+1].dx/1000.f;
                    z10 = pDxy[ltOffset  ].dx/1000.f;
                    z11 = pDxy[ltOffset+1].dx/1000.f;
                    z00 += dx0*(z01-z00);
                    z10 += dx0*(z11-z10);        
                    *dx = float( z00 + dy0*(z10 - z00) );

                    z00 = pDxy[lbOffset  ].dy/1000.f;
                    z01 = pDxy[lbOffset+1].dy/1000.f;
                    z10 = pDxy[ltOffset  ].dy/1000.f;
                    z11 = pDxy[ltOffset+1].dy/1000.f;
                    z00 += dx0*(z01-z00);
                    z10 += dx0*(z11-z10);        
                    *dy = float( z00 + dy0*(z10 - z00) );
                }
            };

	//!
	/**
	@param	lpstrPathName	LPCSTR lpstrPathName:
    @return	TRUE,FALSE;
	*/
	bool Load4File(const char* lpstrPathName);
	
	//!
	/**
	@param	lpstrPathName	LPCSTR lpstrPathName:
    @return	TRUE,FALSE;
	*/
	bool Save2File(const char* lpstrPathName,double *sx=NULL,double *sy=NULL,double *mx=NULL,double *my=NULL,bool bResolveAop6=false,int iCs=0,int iRs=0 );
    bool ResolveAop6( const char* lpstrPathNameN,int cols=-1,int rows=-1 );
    bool ResolveRot(const char* lpstrPathName,int cols,int rows,int rot=270); // rot : 90,180,270

	//!
	/**
	@param	samp	double* samp, 
	@param	line	double* line,
	@param	lat 	double* lat,
	@param	lon 	double* lon, 
	@param	alt		double* alt
	@param	num	    int num , 
	*/
	void Calc_rpc_78( double* samp, double* line, double* lat, double* lon, double* alt, int num,
                      double *sx=NULL,double *sy=NULL,double *mx=NULL,double *my=NULL);

	//! 
	/**
	@param	lat	double lat,
	@param	lon	double lon,
	@param	alt	double alt,
	@param	*px	double *px,
	@param	*py	double *py,
	*/
	void RPCGrd2Pho(double lat, double lon, double alt,double *px,double *py);

	//!
	/**
	@param	lat	double lat, 
	@param	lon	double lon, 
	@param	alt	double alt, 
	@param	*px	double *px, 
	@param	*py	double *py, 
	*/
	void RPCPhoZ2Grd(double px,double py,double gz,double *lat, double *lon, double *alt);

    // for RPC pxy(Top_Bottom) to ScanXy(Left_Bottom)
    void RPC_pxy_to_ixy(double px,double py,double *ix,double *iy ){ RPC_pxy_to_ixy(px,py,ix,iy,m_aop6); };
    void RPC_pxy_to_ixy(double px,double py,double *ix,double *iy,double imgRows ){ RPC_pxy_to_ixy(px,py,ix,iy,m_aop6,imgRows); };
    void RPC_ixy_to_pxy(double ix,double iy,double *px,double *py,double imgRows ){ RPC_ixy_to_pxy(ix,iy,px,py,m_aop6,imgRows); };
    void RPC_ixy_to_pxy(double ix,double iy,double *px,double *py ){ RPC_ixy_to_pxy(ix,iy,px,py,m_aop6); }

    // Note:
    // pxy coord is left top
    // ixy coord is left bottom
    static void RPC_pxy_to_ixy(double px,double py,double *ix,double *iy,double *ab6,double imgRows ){
        *ix = px + ab6[3] + ab6[4]*px + ab6[5]*py;
        *iy = py + ab6[0] + ab6[1]*px + ab6[2]*py;
        *iy = imgRows-1-(*iy);
    }
    static void RPC_pxy_to_ixy(double px,double py,double *ix,double *iy,double *ab6 ){
        *ix = px + ab6[3] + ab6[4]*px + ab6[5]*py;
        *iy = py + ab6[0] + ab6[1]*px + ab6[2]*py;
    }
    
    // Note:
    // pxy coord is left top
    // ixy coord is left bottom
    static void RPC_ixy_to_pxy(double ix,double iy,double *px,double *py,double *ab6,double imgRows ){
        double t = 1 + ab6[2] + ab6[4] + ab6[4]*ab6[2] - ab6[5]*ab6[1]; iy = imgRows-1-iy;
        *px = (ix - ab6[3] + ab6[2]*ix - ab6[2]*ab6[3] - ab6[5]*iy + ab6[5]*ab6[0])/t;
        *py = (iy - ab6[1]*ix + ab6[1]*ab6[3] - ab6[0] + ab6[4]*iy - ab6[4]*ab6[0])/t;
    }
    static void RPC_ixy_to_pxy(double ix,double iy,double *px,double *py,double *ab6 ){
        double t = 1 + ab6[2] + ab6[4] + ab6[4]*ab6[2] - ab6[5]*ab6[1];
        *px = (ix - ab6[3] + ab6[2]*ix - ab6[2]*ab6[3] - ab6[5]*iy + ab6[5]*ab6[0])/t;
        *py = (iy - ab6[1]*ix + ab6[1]*ab6[3] - ab6[0] + ab6[4]*iy - ab6[4]*ab6[0])/t;
    }
};

typedef class CWuRpc CRpc;

void RPCPho2Grd( CWuRpc *pRpcL,double pxl,double pyl,
                 CWuRpc *pRpcR,double pxr,double pyr,
                 double *lat, double *lon, double *alt );


void RPC_Orient( const ORIRPC_GCP *pList,int listSize,RpcPara RPC,double *ab6,int cols=-1,int rows=-1 );


#endif // !defined(WURPC_H_DUANYANSONG_2011_06_30_18_49_234567890543567865)
