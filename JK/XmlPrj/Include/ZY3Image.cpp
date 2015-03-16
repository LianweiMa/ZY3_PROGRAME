// ZY3Image.cpp: implementation of the CZY3Image class.
//
/*----------------------------------------------------------------------+
|		ZY3Image											 			|
|       Author:     DuanYanSong  2011/05/30								|
|            Ver 1.0													|
|       Copyright (c)2011, WHU RSGIS DPGrid Group						|
|	         All rights reserved.                                       |
|		ysduan@whu.edu.cn; ysduan@sohu.com              				|
+----------------------------------------------------------------------*/
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "time.h"
#include "ZY3Image.h"

#define	TIFFTAG_SUBFILETYPE		254	/* subfile data descriptor */
#define	TIFFTAG_IMAGEWIDTH		256	/* image width in pixels */
#define	TIFFTAG_IMAGELENGTH		257	/* image height in pixels */
#define	TIFFTAG_BITSPERSAMPLE	258	/* bits per channel (sample) */
#define	TIFFTAG_COMPRESSION		259	/* data compression technique */
#define	TIFFTAG_PHOTOMETRIC		262	/* photometric interpretation */
#define	    PHOTOMETRIC_MINISBLACK	1	/* min value is black */
#define	    PHOTOMETRIC_RGB		    2	/* RGB color model */

#define	TIFFTAG_STRIPOFFSETS	273	/* offsets to data strips */
#define	TIFFTAG_ORIENTATION		274	/* +image orientation */
#define	    ORIENTATION_TOPLEFT	1	/* row 0 top, col 0 lhs */
#define	    ORIENTATION_BOTLEFT	4	/* row 0 bottom, col 0 lhs */
#define	TIFFTAG_SAMPLESPERPIXEL	277	/* samples per pixel */

#define	TIFFTAG_ROWSPERSTRIP	278	/* rows per strip of data */
#define	TIFFTAG_STRIPBYTECOUNTS	279	/* bytes counts for strips */

#define	TIFFTAG_SOFTWARE		305	/* name & release */
#define	TIFFTAG_DATETIME		306	/* creation date and time */
#define	TIFFTAG_ARTIST			315	/* creator of image */
#define	TIFFTAG_COLORMAP		320	/* RGB map for pallette image */

// geoTif    
#define TIFFTAG_GEOPIXELSCALE        33550    // double xyzScale[3]
#define TIFFTAG_GEOTIEPOINTS         33922    // double tiePts[][6]
#define TIFFTAG_GEOTRANSMATRIX       34264    // double matrix[16]
#define TIFFTAG_GEOKEYDIRECTORY      34735    // 
#define TIFFTAG_GEODOUBLEPARAMS      34736
#define TIFFTAG_GEOASCIIPARAMS       34737


typedef	enum {
	TIFF_SHORT	    = 3,	/* 16-bit unsigned integer */
	TIFF_LONG	    = 4,	/* 32-bit unsigned integer */

    TIFF_DOUBLE = 0x0C,
	TIFF_LONG8	= 16,
	TIFF_SLONG8	= 17,
	TIFF_IFD8	= 18,
}TIFDATATYPE;

#ifndef _LODWORD_HIDWORD
#define _LODWORD_HIDWORD
#define LODWORD(l)           ((DWORD)(l))
#define HIDWORD(l)           ((DWORD)(((LONGLONG)(l) >> 32) & 0xFFFFFFFF))
#endif

#ifdef WIN32

static BOOL IsExist(LPCSTR lpstrPathName){
    WIN32_FIND_DATA fd; HANDLE hFind=INVALID_HANDLE_VALUE;
    hFind = ::FindFirstFile(lpstrPathName,&fd);
    if ( hFind==INVALID_HANDLE_VALUE ) return FALSE;
    ::FindClose(hFind); return TRUE;
}
static BOOL CreateDir(LPCTSTR szPath)
{
    WIN32_FIND_DATA fd; HANDLE hFind = ::FindFirstFile(szPath,&fd);
    if ( hFind!=INVALID_HANDLE_VALUE ){ ::FindClose(hFind); ::CreateDirectory(szPath,NULL); return TRUE; }

    char strPath[512]; strcpy( strPath,szPath );
	char *pSplit = strrchr( strPath,'\\' );
	if ( !pSplit ) return TRUE; else *pSplit = 0; 
    if ( !CreateDir(strPath) ) return FALSE;
    return ::CreateDirectory(szPath,NULL);
}

static HANDLE CreateFileE(LPCSTR lpstrPathName,UINT dwDesiredAccess ){
	if ( dwDesiredAccess==GENERIC_READ ) 
		 return ::CreateFile( lpstrPathName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL );
	else return ::CreateFile( lpstrPathName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL );
}

#else

#define __USE_LARGEFILE64

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef union _LARGE_INTEGER {
    struct { DWORD LowPart; unsigned int HighPart; };
    struct { DWORD LowPart; unsigned int HighPart; }u;
    LONGLONG QuadPart;
}LARGE_INTEGER;

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define FILE_BEGIN						 SEEK_SET	
#define FILE_CURRENT					 SEEK_CUR
#define FILE_END						 SEEK_END
#define INVALID_HANDLE_VALUE			 (HANDLE)-1

inline char* strlwr( char *str ){
	char *orig = str; char d = 'a'-'A';
	for( ;*str!='\0';str++ ){ if ( *str>='A' && *str<='Z' ) *str = *str+d; }
	return orig;
}
static BOOL IsExist(LPCSTR lpstrPathName){return ( (access(lpstrPathName,0)==0)?TRUE:FALSE ); }
static BOOL CreateDir(LPCSTR szPath){
	char strPath[512],*pSplit; 
	if ( IsExist(szPath) ) return TRUE;
	strcpy( strPath,szPath ); pSplit = strrchr( strPath,'\\' );
	if ( !pSplit ) pSplit = strrchr( strPath,'/' );

	if ( !pSplit ) return TRUE; else *pSplit = 0; 
    if ( !CreateDir(strPath) ) return FALSE;
    return mkdir( szPath,S_IRWXU );
}
inline static void CloseHandle(int hFile){ close(hFile); }
inline static int WriteFile( int hFile,void *pBuf,unsigned int bufSize,void *opSz,int *lpOverlapped ){ 
	return write( hFile,pBuf,bufSize ); }
inline static int ReadFile( int hFile,void *pBuf,unsigned int bufSize,void *opSz,unsigned int *lpOverlapped ){ 
	return read( hFile,pBuf,bufSize ); }
inline static unsigned int SetFilePointer( int hFile,DWORD lDistanceToMove,DWORD *lpDistanceToMoveHigh,int dwMoveMethod ){
	LONGLONG ret,dis=0; if ( lpDistanceToMoveHigh ) dis = *lpDistanceToMoveHigh;
	dis = (dis<<32)|lDistanceToMove;
	ret = lseek64( hFile,dis,dwMoveMethod );
	return (unsigned int)ret;
}
inline static HANDLE CreateFileE(LPCSTR lpstrPathName,UINT dwDesiredAccess ){
	if ( dwDesiredAccess==GENERIC_READ ) return open64( lpstrPathName,O_RDWR );
	return open64( lpstrPathName,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR );
}
inline static void DeleteFile(LPCSTR lpstrPathName){ remove(lpstrPathName); }
#endif


inline static void  WriteTifTag( HANDLE hFile,WORD tagId,WORD tagType,DWORD tagSize,DWORD tagVal )
{
    DWORD rwByte;
    WriteFile( hFile,&tagId     ,2,&rwByte,NULL );
    WriteFile( hFile,&tagType   ,2,&rwByte,NULL );
    WriteFile( hFile,&tagSize   ,4,&rwByte,NULL );
    WriteFile( hFile,&tagVal    ,4,&rwByte,NULL );
}

inline static void  WriteBTifTag( HANDLE hFile,WORD tagId,WORD tagType,LONGLONG tagSize,LONGLONG tagVal )
{
    DWORD rwByte;
    WriteFile( hFile,&tagId     ,2,&rwByte,NULL );
    WriteFile( hFile,&tagType   ,2,&rwByte,NULL );
    WriteFile( hFile,&tagSize   ,8,&rwByte,NULL );
    WriteFile( hFile,&tagVal    ,8,&rwByte,NULL );
}

inline static void  ReadTifTag( HANDLE hFile,WORD &tagId,WORD &tagType,DWORD &tagSize,DWORD &tagVal )
{
    DWORD rwByte=0;
    ReadFile( hFile,&tagId     ,2,&rwByte,NULL );
    ReadFile( hFile,&tagType   ,2,&rwByte,NULL );
    ReadFile( hFile,&tagSize   ,4,&rwByte,NULL );
	ReadFile( hFile,&tagVal    ,4,&rwByte,NULL );	
}

inline static void  ReadBTifTag( HANDLE hFile,WORD &tagId,WORD &tagType,LONGLONG &tagSize,LONGLONG &tagVal )
{
    DWORD rwByte=0;
    ReadFile( hFile,&tagId     ,2,&rwByte,NULL );
    ReadFile( hFile,&tagType   ,2,&rwByte,NULL );
    ReadFile( hFile,&tagSize   ,8,&rwByte,NULL );
	ReadFile( hFile,&tagVal    ,8,&rwByte,NULL );	
}

inline static DWORD  SeekTif( HANDLE hFile, LONGLONG distance, DWORD MoveMethod )
{
    LARGE_INTEGER li; li.QuadPart = distance;
    return SetFilePointer( hFile,li.LowPart,&li.HighPart,MoveMethod );    
}

//////////////////////////////////////////////////////////////////////
BOOL CMemImage::Open( int cols,int rows,int pxbs,char *strPath )
{
    m_nCols = cols; m_nRows = rows; m_nPxbs = pxbs;
    sprintf( m_strPN,"%s\\%d_%d_%d_%d.raw",strPath,m_nCols,m_nRows,m_nPxbs,time(NULL) );
    m_fFile  = CreateFileE( m_strPN,GENERIC_WRITE ); 
    if ( !m_fFile || m_fFile==INVALID_HANDLE_VALUE ){ m_fFile=NULL; return FALSE; }
    return TRUE;
}

BOOL CMemImage::Read( BYTE *pBuf,int rowIdx )
{
    if ( !m_fFile ) return FALSE;
    LONGLONG offset = (LONGLONG)rowIdx*(LONGLONG)m_nCols*m_nPxbs;
    DWORD rw=0; SeekTif( m_fFile,offset,FILE_BEGIN );
    return ::ReadFile( m_fFile,pBuf,m_nCols*m_nPxbs,&rw,NULL );
}

BOOL CMemImage::Write( BYTE *pBuf,int rowIdx )
{
    if ( !m_fFile ) return FALSE;
    LONGLONG offset = (LONGLONG)rowIdx*(LONGLONG)m_nCols*m_nPxbs;
    DWORD rw=0; SeekTif( m_fFile,offset,FILE_BEGIN );
    return ::WriteFile( m_fFile,pBuf,m_nCols*m_nPxbs,&rw,NULL );
}

void CMemImage::Close()
{
    if (m_fFile) CloseHandle( m_fFile ); 	
	m_fFile = NULL; m_nCols = m_nRows = m_nPxbs = 0;
    DeleteFile(m_strPN);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZY3Image::CZY3Image()
{
	m_nRows = m_nCols = m_nBands = m_nBits = 0;	
    m_openFlags = 0; 
    memset( m_strPathName,0,256 );
	
	m_fTiff = 0;
	m_Offset= 0;
	m_pBufRow	= NULL;
	m_pStripOff = NULL;

    memset( m_bndOffX,0,sizeof(m_bndOffX) );
    memset( m_bndOffY,0,sizeof(m_bndOffY) );

    m_x0 = m_y0 = 0; m_dx = m_dy = 1;    
    m_Ellipsoid     = Ellipse_WGS_84;
    m_GeodeticDatum = Datum_WGS84;
    m_CSType        = PCS_WGS84_UTM_zone_1N; 
    m_Projection    = Proj_UTM_zone_1N;
    m_RasterType    = 1;
    m_ModelType     = 1;
}

CZY3Image::~CZY3Image()
{
	Close();	
}

void CZY3Image::Close()
{
	if (m_fTiff&&m_openFlags==modeCreate ){
		SeekTif( m_fTiff,m_Offset+((LONGLONG)(m_nRows)*(LONGLONG)(m_nCols))*(LONGLONG)(m_nBands)*(m_nBits/8)-4,FILE_BEGIN );
		DWORD dw=0; ::WriteFile( m_fTiff,&dw,4,&dw,NULL );
	}
	if (m_fTiff) CloseHandle( m_fTiff ); 	
	m_fTiff = NULL; m_Offset= 0;
	if ( m_pBufRow   ) delete []m_pBufRow;   m_pBufRow  = NULL;
	if ( m_pStripOff ) delete []m_pStripOff; m_pStripOff= NULL;

    m_nRows = m_nCols = m_nBands = m_nBits = 0;	
    m_nBands = 1;
    m_openFlags = 0;
    memset( m_strPathName,0,256 );
    memset( m_bndOffX,0,sizeof(m_bndOffX) );
    memset( m_bndOffY,0,sizeof(m_bndOffY) );
}

BOOL CZY3Image::Open( LPCSTR lpstrPathName,UINT flag )
{
	Close(); m_openFlags = flag; 
	strcpy( m_strPathName,lpstrPathName );
	if ( m_openFlags==modeRead )
    {
        // Read GeoInfo
        double padfMatrix[16]; memset( padfMatrix,0X00,sizeof(padfMatrix) );
        struct KeyEntry {
            unsigned short ent_key;        // GeoKey ID            
            unsigned short ent_location;   // TIFF Tag ID or 0     
            unsigned short ent_count;      // GeoKey value count   
            unsigned short ent_val_offset; // value or tag offset  
        }geoKey[8]; memset( geoKey,0,sizeof(geoKey) );

        m_fTiff = CreateFileE( m_strPathName,GENERIC_READ ); if ( !m_fTiff || m_fTiff==INVALID_HANDLE_VALUE ) return FALSE;
		
		DWORD rwByte; WORD wflag; DWORD lflag; LONGLONG llflag,offset; 
		wflag=0x4949; ReadFile( m_fTiff,&wflag,2,&rwByte,NULL ); // 'II'
		if ( wflag != 0x4949 ) return FALSE;
		wflag=0x002A; ReadFile( m_fTiff,&wflag,2,&rwByte,NULL ); // 42
		
		if ( wflag == 0x002A ){ // Is Classic Tiff
			lflag=0x0008; ReadFile( m_fTiff,&lflag,4,&rwByte,NULL ); // Offset
			offset = lflag; SeekTif( m_fTiff,offset,FILE_BEGIN );
			WORD tagSum = 0x000C; ReadFile( m_fTiff,&tagSum,2,&rwByte,NULL ); // TagSum
			
			WORD tagId; WORD tagType; DWORD tagSize,tagVal; 
			for( WORD i=0;i<tagSum;i++ ){
				SeekTif( m_fTiff,offset+2+i*12,FILE_BEGIN ); 
                tagId=0; tagType=0; tagSize=0; tagVal = 0;
				ReadTifTag( m_fTiff,tagId,tagType,tagSize,tagVal );
				switch( tagId ){
					case TIFFTAG_IMAGEWIDTH:
						m_nCols = tagVal;
						break;
					case TIFFTAG_IMAGELENGTH:
						m_nRows = tagVal;
						break;
					case TIFFTAG_BITSPERSAMPLE:
                        if ( tagSize>1 ){
                            SeekTif( m_fTiff,tagVal,FILE_BEGIN ); tagVal = 0;
                            ReadFile( m_fTiff,&tagVal,sizeof(WORD),&rwByte,NULL );
                        }
						if ( tagVal==8 || tagVal==16 )  m_nBits = tagVal;
                        else m_nBits = 8;
						break;
					case TIFFTAG_SAMPLESPERPIXEL:
						m_nBands = tagVal;
						break;
                    case TIFFTAG_ROWSPERSTRIP:
                        break;
					case TIFFTAG_STRIPOFFSETS:
						m_Offset = tagVal;
						if ( tagSize>1 ){
							m_pStripOff = new LONGLONG[tagSize];
							SeekTif( m_fTiff,tagVal,FILE_BEGIN );
							ReadFile( m_fTiff,m_pStripOff,tagSize*sizeof(DWORD),&rwByte,NULL );
							LONGLONG *pD=m_pStripOff+tagSize-1; DWORD v,*pS=(DWORD*)m_pStripOff; pS += tagSize-1;
							for( v=0;v<tagSize;v++,pD--,pS-- ) *pD = *pS;
						}
						break;	
                    case TIFFTAG_GEOTRANSMATRIX:
                        SeekTif( m_fTiff,tagVal,FILE_BEGIN ); 
                        ReadFile( m_fTiff,padfMatrix,sizeof(padfMatrix),&rwByte,NULL ); 
                        break;
                    case TIFFTAG_GEOKEYDIRECTORY:
                        SeekTif( m_fTiff,tagVal,FILE_BEGIN ); 
                        ReadFile( m_fTiff,geoKey,sizeof(geoKey),&rwByte,NULL ); 
                        break;
				}
			}	
		}else
		if ( wflag == 0x002B ){ // Is Big Tiff
			lflag=0x0008; ReadFile( m_fTiff,&lflag,4,&rwByte,NULL ); // Bytesize of offsets
			if ( lflag!=0x0008 ){ Close(); return FALSE; }
			llflag=0x0010; ReadFile( m_fTiff,&llflag,8,&rwByte,NULL ); // Offset
			offset = llflag; SeekTif( m_fTiff,offset,FILE_BEGIN );
			LONGLONG tagSum = 0x000C; ReadFile( m_fTiff,&tagSum,8,&rwByte,NULL ); // TagSum
			
			WORD tagId; WORD tagType; LONGLONG tagSize; LONGLONG tagVal;
			for( WORD i=0;i<tagSum;i++ ){
				SeekTif( m_fTiff,offset+8+i*20,FILE_BEGIN );
				ReadBTifTag( m_fTiff,tagId,tagType,tagSize,tagVal );
				switch( tagId ){
					case TIFFTAG_IMAGEWIDTH:
						m_nCols = (int)tagVal;
						break;
					case TIFFTAG_IMAGELENGTH:
						m_nRows = (int)tagVal;
						break;
					case TIFFTAG_BITSPERSAMPLE:
                        if ( (short)tagVal==8 || (short)tagVal==16 )  m_nBits = (short)(tagVal);
                        else m_nBits = 8;                        
                        //m_nSampleBytes = (int)((tagVal+7)/8);
						break;
					case TIFFTAG_SAMPLESPERPIXEL:
						m_nBands = (int)tagVal;
						break;
					case TIFFTAG_STRIPOFFSETS:
						m_Offset = tagVal;
						if ( tagSize>1 ){
							m_pStripOff = new LONGLONG[ (int)tagSize];
							SeekTif( m_fTiff,m_Offset,FILE_BEGIN );
							ReadFile( m_fTiff,m_pStripOff,(int)tagSize*sizeof(LONGLONG),&rwByte,NULL );
						}
						break;
					case TIFFTAG_STRIPBYTECOUNTS:
						tagVal = tagVal;						
						break;

                    case TIFFTAG_GEOTRANSMATRIX:
                        SeekTif( m_fTiff,tagVal,FILE_BEGIN ); 
                        ReadFile( m_fTiff,padfMatrix,sizeof(padfMatrix),&rwByte,NULL ); 
                        break;
                    case TIFFTAG_GEOKEYDIRECTORY:
                        SeekTif( m_fTiff,tagVal,FILE_BEGIN ); 
                        ReadFile( m_fTiff,geoKey,sizeof(geoKey),&rwByte,NULL ); 
                        break;
				}
			}			
		}
        m_ModelType     = geoKey[1].ent_val_offset;
        m_RasterType    = geoKey[2].ent_val_offset;
        m_Projection    = geoKey[3].ent_val_offset;
        m_CSType        = geoKey[4].ent_val_offset;
        m_GeodeticDatum = geoKey[5].ent_val_offset;
        m_Ellipsoid     = geoKey[6].ent_val_offset;
        m_dx = padfMatrix[0];
        m_dy = -padfMatrix[5];
        m_x0 = padfMatrix[3]+m_dx/2;
        m_y0 = padfMatrix[7]-m_dy/2 - m_dy*(m_nRows-1);

        if (m_nBands==0) m_nBands = 1; // Find this bug by: KangYiFei ,Append this code for some tif which not created by ZY3 Tif .
        if (m_nBits==0 ) m_nBits  = 8;
		m_pBufRow = new BYTE[ m_nCols*m_nBands*m_nBits/8 + 128];
    }else
    if ( flag==modeCreate )
    {
        m_fTiff  = CreateFileE( m_strPathName,GENERIC_WRITE ); if ( !m_fTiff ) return FALSE;
		m_Offset = 1024; m_nBits = 16;

		// Init with 0
		BYTE tt[1024]; memset(tt,0,1024);
		SeekTif( m_fTiff,0,FILE_BEGIN ); DWORD rwByte;
		WriteFile( m_fTiff,tt,1024,&rwByte,NULL );
        
    }else
        return FALSE;
	
    return TRUE;
}

BOOL CZY3Image::Read ( BYTE *pBuf,int row )
{	
	row = m_nRows-1-row;
	if ( row>=0||row<m_nRows ){
        int i,c,row0,cols0; for( i=0;i<m_nBands;i++ ){ if ( m_bndOffX[i]||m_bndOffY[i] ) break; }
        if ( i==m_nBands ){
            LONGLONG offset = m_pStripOff?m_pStripOff[row]:m_Offset+(LONGLONG)row*(LONGLONG)m_nCols*(LONGLONG)m_nBands*(m_nBits/8);
		    DWORD rw=0; SeekTif( m_fTiff,offset,FILE_BEGIN );
            return ::ReadFile( m_fTiff,pBuf,m_nCols*m_nBands*(m_nBits/8),&rw,NULL );
        }else{
            row0 = row; memset( pBuf,0,m_nCols*m_nBands*(m_nBits/8) );
            for( i=0;i<m_nBands;i++ ){
                row = row0+m_bndOffY[i]; if ( row<0 || row>=m_nRows ) continue;
                LONGLONG offset = m_pStripOff?m_pStripOff[row]:m_Offset+(LONGLONG)row*(LONGLONG)m_nCols*(LONGLONG)m_nBands*(m_nBits/8);
		        DWORD rw=0; SeekTif( m_fTiff,offset,FILE_BEGIN );
                if ( ::ReadFile( m_fTiff,m_pBufRow,m_nCols*m_nBands*(m_nBits/8),&rw,NULL ) ){
                    if ( (m_nBits/8)==2 ){
                        WORD *pD = (WORD*)(pBuf + i*2),*pS = (WORD*)(m_pBufRow + i*2);
                        if ( m_bndOffX[i]>0 ){ pS += m_bndOffX[i]*m_nBands; cols0 = m_nCols-m_bndOffX[i]; }
                        else{ pD += m_bndOffX[i]*m_nBands; cols0 = m_nCols+m_bndOffX[i]; }
                        for ( c=0;c<cols0;c++,pD+=m_nBands,pS+=m_nBands ) *pD = *pS;
                    }else{
                        BYTE *pD = pBuf + i,*pS = m_pBufRow + i;
                        if ( m_bndOffX[i]>0 ){ pS += m_bndOffX[i]*m_nBands; cols0 = m_nCols-m_bndOffX[i]; }
                        else{ pD += m_bndOffX[i]*m_nBands; cols0 = m_nCols+m_bndOffX[i]; }
                        for ( c=0;c<cols0;c++,pD+=m_nBands,pS+=m_nBands ) *pD = *pS;
                    }
                }
            }        
        }
        return TRUE;
	}else
        memset( pBuf,0,m_nCols*m_nBands*(m_nBits/8) );
	return FALSE;
}

BOOL CZY3Image::Read2W( WORD *pBuf,int row )
{
    row = m_nRows-1-row;
	if ( row>=0||row<m_nRows ){
        LONGLONG offset = m_pStripOff?m_pStripOff[row]:m_Offset+(LONGLONG)row*(LONGLONG)m_nCols*(LONGLONG)m_nBands*(m_nBits/8);
		
        DWORD rw=0; SeekTif( m_fTiff,offset,FILE_BEGIN );
		if ( (m_nBits/8)==sizeof(WORD) ) return ::ReadFile( m_fTiff,pBuf,m_nCols*m_nBands*(m_nBits/8),&rw,NULL );
		else{
			if ( !::ReadFile( m_fTiff,m_pBufRow,m_nCols*m_nBands*(m_nBits/8),&rw,NULL ) ) return FALSE;
			BYTE *pS = m_pBufRow; WORD *pD = pBuf; int c;
			for ( c=0;c<m_nCols*m_nBands;c++,pS++,pD++ ) *pD = *pS;
			return TRUE;
		}	 
	}
	return FALSE;
}
// CRgn
BOOL CZY3Image::Write ( BYTE *pBuf,int rowIdx,int rows )
{	
	int row = m_nRows-rows-rowIdx;
	if ( row>=0||row<m_nRows ){
		DWORD rw=0; SeekTif( m_fTiff,m_Offset+(LONGLONG)row*(LONGLONG)m_nCols*(LONGLONG)m_nBands*(m_nBits/8),FILE_BEGIN );
        
        if ( rows<=1) ::WriteFile( m_fTiff,pBuf,m_nCols*m_nBands*(m_nBits/8),&rw,NULL );
        else{
            int i,rZ = m_nCols*m_nBands*(m_nBits/8);
            BYTE *pR = new BYTE[ rZ +8];
            for( i=0;i<rows/2;i++ ){
                memcpy( pR,pBuf+rZ*i,rZ );
                memcpy( pBuf+rZ*i,pBuf+rZ*(rows-1-i),rZ );
                memcpy( pBuf+rZ*(rows-1-i),pR,rZ);
            }
            ::WriteFile( m_fTiff,pBuf,rZ*rows,&rw,NULL );
            for( i=0;i<rows/2;i++ ){
                memcpy( pR,pBuf+rZ*i,rZ );
                memcpy( pBuf+rZ*i,pBuf+rZ*(rows-1-i),rZ );
                memcpy( pBuf+rZ*(rows-1-i),pR,rZ);
            }
            delete[] pR;
        }
	}
	return FALSE;
}

void CZY3Image::UpdateImgBuf()
{   
	LONGLONG datSz = (LONGLONG)m_nRows*(LONGLONG)m_nCols*(LONGLONG)m_nBands*(m_nBits/8);
	if ( m_openFlags==modeCreate && datSz>0 ){
		
		int cols = m_nCols,rows = m_nRows,bands = m_nBands,bits = m_nBits;

		HANDLE hFile    = m_fTiff;
		DWORD rwByte; WORD wflag;DWORD lflag;LONGLONG llflag; 

		// Write Header
		SeekTif( hFile,0,FILE_BEGIN );  
		if ( datSz>0X10000000 ){ // Big TIF
			
			wflag =0x4949; WriteFile( hFile,&wflag ,2,&rwByte,NULL ); // 'II'
			wflag =0x002B; WriteFile( hFile,&wflag ,2,&rwByte,NULL ); // 43
			lflag =0x0008; WriteFile( hFile,&lflag ,4,&rwByte,NULL ); // Bytesize of offsets
			llflag=0x0010; WriteFile( hFile,&llflag,8,&rwByte,NULL ); // Offset
			llflag=0x000E; WriteFile( hFile,&llflag,8,&rwByte,NULL ); // TagSum

			llflag = datSz;
			LONGLONG datOff = 1024; // 1024*( (8+8+14*20)/1024 +1	);
			WriteBTifTag( hFile,TIFFTAG_SUBFILETYPE    ,TIFF_SHORT,1,                  1 );
			WriteBTifTag( hFile,TIFFTAG_COMPRESSION    ,TIFF_SHORT,1,                  1 );
			WriteBTifTag( hFile,TIFFTAG_IMAGEWIDTH     ,TIFF_LONG ,1,               cols );
			WriteBTifTag( hFile,TIFFTAG_IMAGELENGTH    ,TIFF_LONG ,1,               rows );
			WriteBTifTag( hFile,TIFFTAG_BITSPERSAMPLE  ,TIFF_SHORT,1,               bits );                 
			WriteBTifTag( hFile,TIFFTAG_SAMPLESPERPIXEL,TIFF_SHORT,1,              bands );
			WriteBTifTag( hFile,TIFFTAG_PHOTOMETRIC    ,TIFF_SHORT,1,  (bands==1)?PHOTOMETRIC_MINISBLACK:PHOTOMETRIC_RGB );
			WriteBTifTag( hFile,TIFFTAG_ORIENTATION    ,TIFF_SHORT,1, ORIENTATION_TOPLEFT);
			WriteBTifTag(hFile, TIFFTAG_ROWSPERSTRIP, TIFF_LONG, 1, 0xffffffffffffffff);
			WriteBTifTag( hFile,TIFFTAG_STRIPOFFSETS   ,TIFF_LONG ,1,               1024 );
			WriteBTifTag( hFile,TIFFTAG_STRIPBYTECOUNTS,TIFF_LONG ,1,             llflag );
            
            lflag = 8+2+14*20;
            WriteTifTag( hFile,TIFFTAG_GEOTRANSMATRIX ,TIFF_DOUBLE, 16,lflag );
            lflag = 8+2+14*20+16*sizeof(double);
            WriteTifTag( hFile,TIFFTAG_GEOKEYDIRECTORY,TIFF_SHORT ,4*8,lflag );
            WriteBTifTag( hFile,0,0,0,0 );

		}else{
			wflag=0x4949; WriteFile( hFile,&wflag,2,&rwByte,NULL ); // 'II'
			wflag=0x002A; WriteFile( hFile,&wflag,2,&rwByte,NULL ); // 42
			lflag=0x0008; WriteFile( hFile,&lflag,4,&rwByte,NULL ); // Offset
			wflag=0x000E; WriteFile( hFile,&wflag,2,&rwByte,NULL ); // TagSum

			lflag = (DWORD)datSz;
			DWORD datOff = 1024*( (8+2+14*12)/1024 +1	);
			WriteTifTag( hFile,TIFFTAG_SUBFILETYPE    ,TIFF_SHORT,1,                  1 );
			WriteTifTag( hFile,TIFFTAG_COMPRESSION    ,TIFF_SHORT,1,                  1 );
			WriteTifTag( hFile,TIFFTAG_IMAGEWIDTH     ,TIFF_LONG ,1,               cols );
			WriteTifTag( hFile,TIFFTAG_IMAGELENGTH    ,TIFF_LONG ,1,               rows );
			WriteTifTag( hFile,TIFFTAG_BITSPERSAMPLE  ,TIFF_SHORT,1,               bits );                  
			WriteTifTag( hFile,TIFFTAG_SAMPLESPERPIXEL,TIFF_SHORT,1,              bands );
			WriteTifTag( hFile,TIFFTAG_PHOTOMETRIC    ,TIFF_SHORT,1,  (bands==1)?PHOTOMETRIC_MINISBLACK:PHOTOMETRIC_RGB );    
			WriteTifTag( hFile,TIFFTAG_ORIENTATION    ,TIFF_SHORT,1, ORIENTATION_TOPLEFT);
			WriteTifTag( hFile,TIFFTAG_ROWSPERSTRIP   ,TIFF_LONG ,1,         0xffffffff );
			WriteTifTag( hFile,TIFFTAG_STRIPOFFSETS   ,TIFF_LONG ,1,             datOff );
			WriteTifTag( hFile,TIFFTAG_STRIPBYTECOUNTS,TIFF_LONG ,1,              lflag );
            lflag = 8+2+14*12;
            WriteTifTag( hFile,TIFFTAG_GEOTRANSMATRIX ,TIFF_DOUBLE, 16,lflag );
            lflag = 8+2+14*12+16*sizeof(double);
            WriteTifTag( hFile,TIFFTAG_GEOKEYDIRECTORY,TIFF_SHORT ,4*8,lflag );
            WriteTifTag( hFile,0,0,0,0 );                        
		}
        // Write GeoInfo
        double padfMatrix[16]; memset( padfMatrix,0X00,sizeof(padfMatrix) );
        struct KeyEntry {
            unsigned short ent_key;        // GeoKey ID            
            unsigned short ent_location;   // TIFF Tag ID or 0     
            unsigned short ent_count;      // GeoKey value count   
            unsigned short ent_val_offset; // value or tag offset  
        }geoKey[8]; memset( geoKey,0,sizeof(geoKey) );
        geoKey[0].ent_key			= 1;
        geoKey[0].ent_location		= 1;
        geoKey[0].ent_count			= 0;
        geoKey[0].ent_val_offset	= 6; // number of keys 
        
        geoKey[1].ent_key			= GTModelTypeGeoKey ;
        geoKey[1].ent_location		= 0;
        geoKey[1].ent_count			= 1;
        geoKey[1].ent_val_offset	= m_ModelType;
        
        geoKey[2].ent_key			= GTRasterTypeGeoKey;
        geoKey[2].ent_location		= 0;
        geoKey[2].ent_count			= 1;
        geoKey[2].ent_val_offset	= m_RasterType;
        
        geoKey[3].ent_key			= ProjectionGeoKey;
        geoKey[3].ent_location		= 0;
        geoKey[3].ent_count			= 1;
        geoKey[3].ent_val_offset	= m_Projection;				
        
        geoKey[4].ent_key			= ProjectedCSTypeGeoKey;
        geoKey[4].ent_location		= 0;
        geoKey[4].ent_count			= 1;
        geoKey[4].ent_val_offset	= m_CSType;				
        
        geoKey[5].ent_key			= GeogGeodeticDatumGeoKey;
        geoKey[5].ent_location		= 0;
        geoKey[5].ent_count			= 1;
        geoKey[5].ent_val_offset	= m_GeodeticDatum;	
        
        geoKey[6].ent_key			= GeogEllipsoidGeoKey;
        geoKey[6].ent_location		= 0;
        geoKey[6].ent_count			= 1;
        geoKey[6].ent_val_offset	= m_Ellipsoid;
        
        double Dx,Dy,Rx,Ry,Ex,Ny;
        Dx =  m_dx;
        Rx =  0; 
        Ry =  0; 
        Dy = -m_dy;
        Ex = m_x0 ;
        Ny = m_y0 - Dy*(m_nRows-1);
        padfMatrix[0]  = Dx; padfMatrix[1]  = Rx;  padfMatrix[2]  = 0; padfMatrix[3]  = Ex-Dx/2;  // GEOTRANSMATRIX to TFW
        padfMatrix[4]  = Ry; padfMatrix[5]  = Dy;  padfMatrix[6]  = 0; padfMatrix[7]  = Ny-Dy/2;  // GEOTRANSMATRIX to TFW
        padfMatrix[8]  = 0 ; padfMatrix[9]  =  0;  padfMatrix[10] = 1; padfMatrix[11] =  0;                                                              
        padfMatrix[12] = 0 ; padfMatrix[12] =  0;  padfMatrix[13] = 0; padfMatrix[15] =  1;
        
        WriteFile( hFile,padfMatrix,sizeof(padfMatrix),&rwByte,NULL ); 
        WriteFile( hFile,geoKey,sizeof(geoKey),&rwByte,NULL );
	}
}
