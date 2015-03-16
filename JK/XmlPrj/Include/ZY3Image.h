// ZY3Image.h: interface for the CZY3Image class.
//
//////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------+
|		ZY3Image											 			|
|       Author:     DuanYanSong  2011/05/30								|
|            Ver 1.0													|
|       Copyright (c)2011, WHU RSGIS DPGrid Group						|
|	         All rights reserved.                                       |
|		ysduan@whu.edu.cn; ysduan@sohu.com              				|
+----------------------------------------------------------------------*/

#ifndef ZY3IMAGE_H_DUANYANSONG_2011_05_30_18_25_435678908765432
#define ZY3IMAGE_H_DUANYANSONG_2011_05_30_18_25_435678908765432
#include "typedef.h"
// Geo Tiff Value
#define   GTModelTypeGeoKey           1024 
typedef enum {
	ModelTypeProjected  = 1,  /* Projection Coordinate System */
	ModelTypeGeographic = 2,  /* Geographic latitude-longitude System */
	ModelTypeGeocentric = 3,   /* Geocentric (X,Y,Z) Coordinate System */
	ModelProjected  = ModelTypeProjected,   /* alias */
	ModelGeographic = ModelTypeGeographic,  /* alias */
	ModelGeocentric = ModelTypeGeocentric   /* alias */
}modeltype_t;

#define   GTRasterTypeGeoKey          1025
#define         RasterPixelIsArea        1
#define         RasterPixelIsPoint       2
#define   GTCitationGeoKey            1026


#define GeographicTypeGeoKey			2048
typedef enum {
	GCS_AGD84				= 4203,
	GCS_Beijing_1954		= 4214,
	GCS_OS_SN80				= 4279,
	GCS_Tokyo				= 4301,
	GCS_WGS_84				= 4326,
	GCSE_GRS1980			= 4019,
	GCSE_Krassowsky1940		= 4024,
	GCSE_WGS84				= 4030,
	GCSE_Sphere				= 4035,
}geographic_t;
#define GeogGeodeticDatumGeoKey			2050
typedef enum {
	Datum_WGS84				= 6326,
	Datum_Tokyo				= 6301,
	Datum_OS_SN_1980		= 6279,
	Datum_Beijing_1954		= 6214,	
	DatumE_Krassowsky1960	= 6024,	
	DatumE_GRS1980			= 6019,	
}geodeticdatum_t;
#define GeogPrimeMeridianGeoKey			2051
typedef enum {
	PM_Greenwich = 	8901,
	PM_Lisbon	 = 	8902,
	PM_Paris	 = 	8903,
	PM_Bogota	 = 	8904,
	PM_Madrid	 = 	8905,
	PM_Rome		 = 	8906,
	PM_Bern		 = 	8907,
	PM_Jakarta	 = 	8908,
	PM_Ferro	 = 	8909,
	PM_Brussels	 = 	8910,
	PM_Stockholm = 	8911,
}primemeridian_t;
#define GeogLinearUnitsGeoKey			2052
#define GeogLinearUnitSizeGeoKey		2053
#define GeogAngularUnitsGeoKey			2054
#define GeogAngularUnitSizeGeoKey		2055
#define GeogSemiMajorAxisGeoKey			2057
#define GeogSemiMinorAxisGeoKey			2058
#define GeogPrimeMeridianLongGeoKey		2059
#define GeogEllipsoidGeoKey				2056
typedef enum {
	Ellipse_Sphere				= 7035,
	Ellipse_WGS_84				= 7030,
	Ellipse_Krassowsky_1940		= 7024,
	Ellipse_International_1967  = 7023,
	Ellipse_GRS_1980			= 7019,
}ellipsoid_t;

#define ProjectedCSTypeGeoKey			3072
typedef enum {
	PCS_Hjorsey_1955_Lambert  = 3053,
	PCS_ISN93_Lambert_1993	  = 3057,

	PCS_Beijing_Gauss_zone_13 = 21413,
	PCS_Beijing_Gauss_zone_14 = 21414,
	// ...
	PCS_Beijing_Gauss_zone_23 = 21423,
	
	PCS_Beijing_Gauss_13N	  =	21473,
	PCS_Beijing_Gauss_14N	  =	21474,
	// ...
	PCS_Beijing_Gauss_23N	  = 21483,
	
	PCS_WGS84_UTM_zone_1N     = 32601,
	PCS_WGS84_UTM_zone_2N     = 32602,
	// ...
	PCS_WGS84_UTM_zone_60N	  =	32660,

	PCS_WGS84_UTM_zone_1S	  =	32701,
	PCS_WGS84_UTM_zone_2S	  =	32702,
	// ...
	PCS_WGS84_UTM_zone_60S	  =	32760,

}pcstype_t;

#define ProjectionGeoKey				3074
typedef enum {
	Proj_UTM_zone_1N  = 16001,
	Proj_UTM_zone_2N  = 16002,
	// ...
	Proj_UTM_zone_60N = 16060,

	Proj_UTM_zone_1S  = 16101,
	Proj_UTM_zone_2S  = 16102,
	// ...
	Proj_UTM_zone_60S = 16160,

	Proj_Gauss_Kruger_zone_0 = 16200,
	Proj_Gauss_Kruger_zone_1 = 16201,
	Proj_Gauss_Kruger_zone_2 = 16202,
	Proj_Gauss_Kruger_zone_3 = 16203,
	Proj_Gauss_Kruger_zone_4 = 16204,
	Proj_Gauss_Kruger_zone_5 = 16205,
    // ...
    Proj_Gauss_Kruger_zone_99 = 16299,

}projection_t;
#define ProjCoordTransGeoKey			3075
typedef enum {
	CT_TransverseMercator				= 1,
	CT_TransvMercator_Modified_Alaska	= 2, 
	CT_Mercator							= 7,
	CT_LambertConfConic_2SP				= 8,
	CT_LambertAzimEqualArea				= 10,
	CT_Stereographic					= 14,
	CT_Orthographic						= 21,
	CT_GaussKruger						= CT_TransverseMercator,
}coordtrans_t;





#ifdef WIN32

#include "Windows.h"

#else

#include <memory.h>
typedef unsigned int        HANDLE;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int        DWORD;
typedef unsigned int        UINT;
typedef const char*         LPCSTR;
//typedef unsigned long		LONGLONG;//-mlw

#endif

#define MB					1024*1024
#define TRUE				1
#define FALSE				0
#define NULL				0

#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))


class CMemImage
{
public:
    CMemImage(){m_fFile=NULL;m_nCols=m_nRows=m_nPxbs=0;};
    virtual ~CMemImage(){ Close(); };
    virtual BOOL Open( int cols,int rows,int pxbs,char *strPath="z:" );
    virtual BOOL Read( BYTE *pBuf,int rowIdx );
    virtual BOOL Write( BYTE *pBuf,int rowIdx );
    virtual void Close();
protected:
    int         m_nCols,m_nRows,m_nPxbs;
    HANDLE		m_fFile;
    char        m_strPN[512];
};


//!影像文件读写接口(Raw方式)
/**
影像文件读写接口(Raw方式)
此接口定义的是对影像文件的Raw方式访问，也就是说当影像文件使用此接口访问
获取的数据都是符合Raw方式，即：按行得到数据，并且影像象素占用字节为 1(灰度) 或者 3(RGB) 。
*/

class CZY3Image  
{
public:
	CZY3Image();
	virtual ~CZY3Image();

	//! 文件访问模式
	enum OPENFLAGS { modeRead= 0x0000,modeCreate = 0x1000 };

	//!打开影像
	/**
	@param	lpstrPathName	LPCSTR lpstrPathName，文件名
	@param	flag			OpenFlags flag,打开模式 @see OpenFlags
    @return	成功TRUE,失败 FALSE;
	*/
	virtual BOOL		Open( LPCSTR lpstrPathName,UINT flag=modeRead );
	//!关闭影像
	virtual void		Close();
    
    virtual void        SetGeoInf(double x0,double y0,double dx,double dy,
                                  int Ellipsoid,int GeodeticDatum,int CSType,
                                  int Projection,int RasterType=RasterPixelIsArea,int ModelType=ModelTypeProjected ){
                            m_x0 = x0; m_y0 = y0; m_dx = dx; m_dy = dy;

                            m_Ellipsoid     = Ellipsoid;
                            m_GeodeticDatum = GeodeticDatum;
                            m_CSType        = CSType; 
                            m_Projection    = Projection;
                            m_RasterType    = RasterType;
                            m_ModelType     = ModelType; 
                            UpdateImgBuf();
                        };
    virtual void        GetGeoInf(double *x0,double *y0,double *dx,double *dy,
                            int *Ellipsoid,int *GeodeticDatum,int *CSType,
                            int *Projection,int *RasterType,int *ModelType ){
                            *x0 = m_x0; *y0 = m_y0; *dx = m_dx; *dy = m_dy;
        
                            *Ellipsoid     = m_Ellipsoid    ;
                            *GeodeticDatum = m_GeodeticDatum;
                            *CSType        = m_CSType       ; 
                            *Projection    = m_Projection   ;
                            *RasterType    = m_RasterType   ;
                            *ModelType     = m_ModelType    ;                                           
                        };
    //!读影像行
	/**
	*@param  pBuf    BYTE *pBuf ，读回的影像行地址，注意此空间必须大于等于影像行大小
	*@param  rowIdx  int rowIdx ，影像行号(Base 0)
	*@return 成功TRUE,失败 FALSE;
	*/
    virtual BOOL		Read  ( WORD *pBuf,int rowIdx ){ return Read( (BYTE *)pBuf,rowIdx ); };
    virtual BOOL		Read  ( BYTE *pBuf,int rowIdx );    
    virtual BOOL		Read2W( WORD *pBuf,int rowIdx );    
	virtual BOOL		Read2B( BYTE *pBuf,int rowIdx ){
		if ( m_nBits==8 ) return Read( pBuf,rowIdx );
        else{ 
            Read( m_pBufRow,rowIdx ); 
            BYTE *pBuf16 = m_pBufRow,*pBuf8 = pBuf; int i,cols=m_nCols*m_nBands;
            for( i=0;i<cols;i++,pBuf8++,pBuf16+=2 ) *pBuf8 = *(pBuf16+1)>255?255:*(pBuf16+1); 
        }
        return TRUE;
	}

	//!写影像行
	/**
	*@param  pBuf    BYTE *pBuf ，写影像行地址，注意此空间必须大于等于影像行大小
	*@param  rowIdx  int rowIdx ，影像行号(Base 0)
	*@return 成功TRUE,失败 FALSE;
	*/
	virtual BOOL		Write  ( WORD *pBuf,int rowIdx ){ return Write( (BYTE*)pBuf,rowIdx,1 ); };
    virtual BOOL		Write  ( BYTE *pBuf,int rowIdx ){ return Write( (BYTE*)pBuf,rowIdx,1 ); } 
    virtual BOOL        Write  ( WORD *pBuf,int rowIdx,int rows ){ return Write( (BYTE*)pBuf,rowIdx,rows ); } 
    virtual BOOL        Write  ( BYTE *pBuf,int rowIdx,int rows );
    //!读影像块
	/**
	@param	pBuf		LPVOID pBuf	 ，影像数据存放内存块
    @param  sRow        int sRow ，影象区域起始行
    @param  sCol        int sCol ，影象区域起始列
    @param  rows        int rows ，影象区域行数
    @param  cols        int cols ，影象区域列数
	@return	成功TRUE,失败 FALSE;
	*/
    virtual BOOL        Read ( WORD* pBuf,int sRow,int sCol,int rows,int cols ){ return Read( (BYTE*)pBuf,sRow,sCol,rows,cols ); };
    virtual BOOL        Read ( BYTE* pBuf,int sRow,int sCol,int rows,int cols ){
                            if ( m_openFlags!=modeRead ) return FALSE;
							int be_row,be_col,ed_row,ed_col,col0=cols,bytes=m_nBits/8;
							be_row=sRow; ed_row = rows + be_row;
							be_col=sCol; ed_col = cols + be_col;
							memset( pBuf, 0, rows*cols*m_nBands*bytes );
							if ( be_row > m_nRows || be_col > m_nCols || ed_row < 0 || ed_col < 0 ) return FALSE;
							BYTE *pRowBuf = new BYTE[m_nCols*m_nBands*bytes +8];
							if ( be_row<0 || be_col<0 || ed_row>m_nRows || ed_col>m_nCols ){
								if (be_row < 0)       { pBuf -= (be_row*cols)*bytes*m_nBands; rows += be_row; be_row = 0; }
								if (be_col < 0)       { pBuf -= (be_col)*bytes*m_nBands;      cols += be_col; be_col = 0; }
								if (ed_row > m_nRows) { rows = m_nRows - be_row; }
								if (ed_col > m_nCols) { cols = m_nCols - be_col; }
							}
							for ( int r=0;r<rows; r++ ){
								Read( pRowBuf,be_row+r );
								memcpy( pBuf,pRowBuf+be_col*m_nBands*bytes,cols*m_nBands*bytes );
								pBuf  += col0*m_nBands*bytes;
							}
							delete pRowBuf;
							return TRUE;
						};    
    virtual BOOL        Read8( BYTE* pBuf,int sRow,int sCol,int rows,int cols,bool bFile=false ){
                            if ( m_openFlags!=modeRead ) return FALSE;
                            CMemImage *pImg = (CMemImage *)pBuf;
                            if (m_nBits==8){
                                if ( bFile ){
                                    BYTE *pRow = new BYTE[ cols +8];
                                    for ( int r=0;r<rows; r++ ){ 
                                        Read( pRow,sRow+r,sCol,1,cols ); 
                                        pImg->Write(pRow,r); }
                                    delete pRow; return TRUE;
                                }else
                                    return Read( pBuf,sRow,sCol,rows,cols );
                            }
                            
							int r,c,be_row,be_col,ed_row,ed_col,col0=cols,bytes=m_nBits/8,ret=1;
							be_row=sRow; ed_row = rows + be_row;
							be_col=sCol; ed_col = cols + be_col;
							memset( pBuf, 0, rows*cols*m_nBands );
							if ( be_row > m_nRows || be_col > m_nCols || ed_row < 0 || ed_col < 0 ) return FALSE;
							if ( be_row<0 || be_col<0 || ed_row>m_nRows || ed_col>m_nCols ){
								if (be_row < 0)       { pBuf -= (be_row*cols)*bytes*m_nBands; rows += be_row; be_row = 0; }
								if (be_col < 0)       { pBuf -= (be_col)*bytes*m_nBands;      cols += be_col; be_col = 0; }
								if (ed_row > m_nRows) { rows = m_nRows - be_row; }
								if (ed_col > m_nCols) { cols = m_nCols - be_col; }
							}
                            WORD *pRowBuf = new WORD[m_nCols*m_nBands +8];
                            BYTE *pD;WORD *pS; LONGLONG loSum,hiSum,sum=0,*pTab16 = new LONGLONG [65536];
                            memset( pTab16,0,sizeof(LONGLONG)*65536 );
                            for ( r=0;r<rows; r+=8 ){
                                Read( pRowBuf,be_row+r );
                                for( pS=pRowBuf+be_col*m_nBands,c=0;c<cols*m_nBands;c++,pS++ ){ sum++; pTab16[*pS]++; }
                            }
                            int i,minI,maxI,midI,minSum,maxSum;
                            minSum=int( sum*0.001f );  if (minSum<10) minSum = 10;
                            maxSum=int( sum*0.001f );  if (maxSum<10) maxSum = 10;                            
                            for ( sum=0,minI=0;minI<65535;minI++ ){ sum+=pTab16[minI]; if (sum>minSum) break; }
                            for ( sum=0,maxI=65535;maxI>0;maxI-- ){ sum+=pTab16[maxI]; if (sum>maxSum) break; }
                            
                            // for Sta. White rgn (Mybe Cloud)
                            float per=1; midI = (minI+maxI)/2; 
                            for ( loSum=0,i=minI;i<=midI;i++ ) loSum+=pTab16[i];
                            for ( hiSum=0,i=midI;i< maxI;i++ ) hiSum+=pTab16[i];                            
                            per = ( loSum*1.f/hiSum*1.f ); 
                            if ( per<0.05f || (maxI-minI)<192 || maxI<160 || minI>768 ) ret=10;
                            //printf("\nminI=%d ,maxI=%d, midI=%d loSum= %I64d ,hiSum= %I64d ,per= %f ,ret= %d\n",minI,maxI,midI,loSum,hiSum,per,ret );

                            memset( pTab16,0,sizeof(LONGLONG)*65536 );
                            for ( i=minI;i<maxI;i++ ) pTab16[i] = 1+int( (i-minI)*254.f/(maxI-minI) );
                            for( ;i<65535;i++ ) pTab16[i] = 255;
                            for ( r=0;r<rows; r++ ){
								Read( pRowBuf,be_row+r );
							    
                                if ( bFile ){
                                    for ( pS=pRowBuf+be_col*m_nBands,pD=(BYTE*)pRowBuf,c=0;c<cols*m_nBands;c++,pD++,pS++) *pD=BYTE(pTab16[*pS]);
                                    pImg->Write( (BYTE*)pRowBuf,r);
                                }else{
                                    for ( pS=pRowBuf+be_col*m_nBands,pD=pBuf,c=0;c<cols*m_nBands;c++,pD++,pS++) *pD=BYTE(pTab16[*pS]);
                                    pBuf  += col0*m_nBands;
                                }
							}
                            delete pTab16;
							delete pRowBuf;
                            return ret;
                        };

    //!设置 ZY3_MUX 的偏移
    /**
    @param	bandOffX    int *bandOffX , 影像各波段 列(X)偏移位置，左为 0
    @param  bandOffY    int *bandOffY , 影像各波段 行(Y)偏移位置，下为 0
    *@return TRUE,FALSE
    */
    virtual void        SetBandReadOffset( int *bandOffX,int *bandOffY ){ 
        memcpy(m_bndOffX,bandOffX,sizeof(int)*m_nBands); 
        memcpy(m_bndOffY,bandOffY,sizeof(int)*m_nBands); 
    };
    
    //!获取影像像素波段数
	/**
	*@return 影像波段数 1（一般为灰度） 或 3（RGB彩色）
	*/
	inline const  int GetBands(){ return m_nBands; };

	//!获取影像行数
	/**
	*@return 影像行数
	*/
	inline const  int GetRows(){ return m_nRows; };
	//!获取影像列数
	/**
	*@return 影像列数 
	*/
	inline const int GetCols(){ return m_nCols; };
    //!获取影像位深
	/**
	*@return 影像位深 8: BYTE ,16 : WORD
	*/
	inline const int GetBits(){ return m_nBits; };

	
	//!设定影像行数
	/**
    设定影像行数，只有新建影像时此函数才可以使用
	*@param  int nRows:影像行数
	*@return 无
	*/
	inline void SetRows(int nRows){ m_nRows=nRows; UpdateImgBuf(); };
	//!设定影像列数
	/**
    设定影像列数，只有新建影像时此函数才可以使用
	*@param  int nCols:影像列数
	*@return 无
	*/
	inline void SetCols(int nCols){ m_nCols=nCols; UpdateImgBuf(); }; 
	//!设定像素波段数
	/**
    设定波段数，只有新建影像时此函数才可以使用
	*@param  int nBands 波段数
	*@return 无
	*/
    inline void SetBands(int nBands){ m_nBands=nBands; UpdateImgBuf(); };
    //!获取影像位深
	/**
	*@return 影像位深 8: BYTE ,16 : WORD
	*/
	inline void SetBits(int nBits){ m_nBits=nBits; UpdateImgBuf(); };

    inline const char* GetPathName(){ return m_strPathName; };
    
protected:
    //!更新影像的Buffer
	/**
    设定影像属性后更新影像的Buffer,此函数是针对新建影像使用的。
	*/
    virtual void UpdateImgBuf();

protected:
    int 	        m_nRows,m_nCols,m_nBands,m_nBits;
    int             m_bndOffX[16],m_bndOffY[16];

    //!影像文件打开方式
    UINT            m_openFlags;
    
    //!影像文件路径
    char            m_strPathName[256];

    double  m_x0,m_y0,m_dx,m_dy;
    int     m_Ellipsoid,m_GeodeticDatum,m_CSType,m_Projection,m_RasterType,m_ModelType;


private:
	HANDLE			m_fTiff;
	LONGLONG		m_Offset;
	BYTE*			m_pBufRow;
	LONGLONG*   	m_pStripOff;

};


inline static BOOL    Save2Tif( const char *strImgName,BYTE *pImg,int cols,int rows,int bands )
{
    CZY3Image imgFile; if ( !imgFile.Open( strImgName,CZY3Image::modeCreate ) ) return FALSE;
	imgFile.SetCols(cols); imgFile.SetRows(rows); imgFile.SetBands(bands); imgFile.SetBits(8);
	BYTE *pRow=new BYTE[ cols*bands +8];
	for ( int i=0;i<rows;i++,pImg+=cols*bands ){ memcpy( pRow,pImg,cols*bands ); imgFile.Write( pRow,i );  }
	delete pRow; imgFile.Close();
	return TRUE;
}

inline static BOOL    Save2Tif( const char *strImgName,WORD *pImg,int cols,int rows,int bands )
{
	CZY3Image imgFile; if ( !imgFile.Open( strImgName,CZY3Image::modeCreate ) ) return FALSE;
	imgFile.SetCols(cols); imgFile.SetRows(rows); imgFile.SetBands(bands); imgFile.SetBits(16);
	WORD *pRow=new WORD[ cols*bands +8];
	for ( int i=0;i<rows;i++,pImg+=cols*bands ){ memcpy( pRow,pImg,cols*bands*sizeof(WORD) ); imgFile.Write( pRow,i );  }
	delete pRow; imgFile.Close();
	return TRUE;
}

#endif 
