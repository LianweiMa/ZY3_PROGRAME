// SHP.cpp: implementation of the CSHP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SHP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSHP::CSHP()
{

}

CSHP::~CSHP()
{

}
/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// $Id: shpopen.c,v 1.39 2002/08/26 06:46:56 warmerda Exp $
//
// Project:  Shapelib
// Purpose:  Implementation of core Shapefile read/write functions.
// Author:   Frank Warmerdam, warmerdam@pobox.com
//
///////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;

#if UINT_MAX == 65535
typedef long	      int32;
#else
typedef int	      int32;
#endif

#ifndef FALSE
#  define FALSE		0
#  define TRUE		1
#endif

#define ByteCopy( a, b, c )	memcpy( b, a, c )
#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif

static int 	bBigEndian;


//////////////////////////////////////////////////////////////////////////
//                              SwapWord()                              //
//                                                                      //
//      Swap a 2, 4 or 8 byte word.                                     //
//////////////////////////////////////////////////////////////////////////

static void	SwapWord( int length, void * wordP )

{
    int		i;
    uchar	temp;

    for( i=0; i < length/2; i++ )
    {
	temp = ((uchar *) wordP)[i];
	((uchar *)wordP)[i] = ((uchar *) wordP)[length-i-1];
	((uchar *) wordP)[length-i-1] = temp;
    }
}
//////////////////////////////////////////////////////////////////////////
//                             SfRealloc()                              //
//                                                                      //
//      A realloc cover function that will access a NULL pointer as     //
//      a valid input.                                                  //
//////////////////////////////////////////////////////////////////////////

static void * SfRealloc( void * pMem, int nNewSize )

{
    if( pMem == NULL )
        return( (void *) malloc(nNewSize) );
    else
        return( (void *) realloc(pMem,nNewSize) );
}

//////////////////////////////////////////////////////////////////////////
//                          SHPWriteHeader()                            //
//                                                                      //
//      Write out a header for the .shp and .shx files as well as the	//
//	contents of the index (.shx) file.				//
//////////////////////////////////////////////////////////////////////////

static void SHPWriteHeader( SHPHandle psSHP )

{
    uchar     	abyHeader[100];
    int		i;
    int32	i32;
    double	dValue;
    int32	*panSHX;

// -------------------------------------------------------------------- //
//      Prepare header block for .shp file.                             //
// -------------------------------------------------------------------- //
    for( i = 0; i < 100; i++ )
      abyHeader[i] = 0;

    abyHeader[2] = 0x27;				// magic cookie //
    abyHeader[3] = 0x0a;

    i32 = psSHP->nFileSize/2;				// file size //
    ByteCopy( &i32, abyHeader+24, 4 );
    if( !bBigEndian ) SwapWord( 4, abyHeader+24 );
    
    i32 = 1000;						// version //
    ByteCopy( &i32, abyHeader+28, 4 );
    if( bBigEndian ) SwapWord( 4, abyHeader+28 );
    
    i32 = psSHP->nShapeType;				// shape type //
    ByteCopy( &i32, abyHeader+32, 4 );
    if( bBigEndian ) SwapWord( 4, abyHeader+32 );

    dValue = psSHP->adBoundsMin[0];			// set bounds //
    ByteCopy( &dValue, abyHeader+36, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+36 );

    dValue = psSHP->adBoundsMin[1];
    ByteCopy( &dValue, abyHeader+44, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+44 );

    dValue = psSHP->adBoundsMax[0];
    ByteCopy( &dValue, abyHeader+52, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+52 );

    dValue = psSHP->adBoundsMax[1];
    ByteCopy( &dValue, abyHeader+60, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+60 );

    dValue = psSHP->adBoundsMin[2];			// z //
    ByteCopy( &dValue, abyHeader+68, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+68 );

    dValue = psSHP->adBoundsMax[2];
    ByteCopy( &dValue, abyHeader+76, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+76 );

    dValue = psSHP->adBoundsMin[3];			// m //
    ByteCopy( &dValue, abyHeader+84, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+84 );

    dValue = psSHP->adBoundsMax[3];
    ByteCopy( &dValue, abyHeader+92, 8 );
    if( bBigEndian ) SwapWord( 8, abyHeader+92 );

// -------------------------------------------------------------------- //
//      Write .shp file header.                                         //
// -------------------------------------------------------------------- //
    fseek( psSHP->fpSHP, 0, 0 );
    fwrite( abyHeader, 100, 1, psSHP->fpSHP );

// -------------------------------------------------------------------- //
//      Prepare, and write .shx file header.                            //
// -------------------------------------------------------------------- //
    i32 = (psSHP->nRecords * 2 * sizeof(int32) + 100)/2;   // file size //
    ByteCopy( &i32, abyHeader+24, 4 );
    if( !bBigEndian ) SwapWord( 4, abyHeader+24 );
    
    fseek( psSHP->fpSHX, 0, 0 );
    fwrite( abyHeader, 100, 1, psSHP->fpSHX );

// -------------------------------------------------------------------- //
//      Write out the .shx contents.                                    //
// -------------------------------------------------------------------- //
    panSHX = (int32 *) malloc(sizeof(int32) * 2 * psSHP->nRecords);

    for( i = 0; i < psSHP->nRecords; i++ )
    {
	panSHX[i*2  ] = psSHP->panRecOffset[i]/2;
	panSHX[i*2+1] = psSHP->panRecSize[i]/2;
	if( !bBigEndian ) SwapWord( 4, panSHX+i*2 );
	if( !bBigEndian ) SwapWord( 4, panSHX+i*2+1 );
    }

    fwrite( panSHX, sizeof(int32) * 2, psSHP->nRecords, psSHP->fpSHX );

    free( panSHX );
}

//////////////////////////////////////////////////////////////////////////
//                              SHPOpen()                               //
//                                                                      //
//      Open the .shp and .shx files based on the basename of the       //
//      files or either file name.                                      //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	CSHP::SHPOpen()
//	  功  能:	打开shp和shx文件，并读取shp头信息和shx整个文件
//	  原  理:	
//	  输  入:	(const char * pszLayer,          //文件名,包含路径
//               const char * pszAccess)         //文件打开方式            
//	  输  出:	无
//	  返回值:   不成功:NULL;成功:SHPHandle psSHP //打开的文件指针
//    赋  值:   无
//	  注  意:   调用函数SwapWord()
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
SHPHandle SHPAPI_CALL
CSHP::SHPOpen( const char * pszLayer, const char * pszAccess )

{
    char		*pszFullname, *pszBasename;
    SHPHandle		psSHP;
    
    uchar		*pabyBuf;
    int			i;
    double		dValue;
    
// -------------------------------------------------------------------- //
//      Ensure the access string is one of the legal ones.  We          //
//      ensure the result string indicates binary to avoid common       //
//      problems on Windows.                                            //
// -------------------------------------------------------------------- //
    if( strcmp(pszAccess,"rb+") == 0 || strcmp(pszAccess,"r+b") == 0
        || strcmp(pszAccess,"r+") == 0 )
        pszAccess = "r+b";
    else
        pszAccess = "rb";
    
// -------------------------------------------------------------------- //
//	Establish the byte order on this machine.			//
// -------------------------------------------------------------------- //
    i = 1;
    if( *((uchar *) &i) == 1 )
        bBigEndian = FALSE;
    else
        bBigEndian = TRUE;

// -------------------------------------------------------------------- //
//	Initialize the info structure.					//
// -------------------------------------------------------------------- //
    psSHP = (SHPHandle) calloc(sizeof(SHPInfo),1);

    psSHP->bUpdated = FALSE;

// -------------------------------------------------------------------- //
//	Compute the base (layer) name.  If there is any extension	//
//	on the passed in filename we will strip it off.			//
// -------------------------------------------------------------------- //
    pszBasename = (char *) malloc(strlen(pszLayer)+5);
    strcpy( pszBasename, pszLayer );
    for( i = strlen(pszBasename)-1; 
	 i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
	       && pszBasename[i] != '\\';
	 i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

// -------------------------------------------------------------------- //
//	Open the .shp and .shx files.  Note that files pulled from	//
//	a PC to Unix with upper case filenames won't work!		//
// -------------------------------------------------------------------- //
    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf( pszFullname, "%s.shp", pszBasename );
    psSHP->fpSHP = fopen(pszFullname, pszAccess );
    if( psSHP->fpSHP == NULL )
    {
        sprintf( pszFullname, "%s.SHP", pszBasename );
        psSHP->fpSHP = fopen(pszFullname, pszAccess );
    }
    
    if( psSHP->fpSHP == NULL )
    {
        free( psSHP );
        free( pszBasename );
        free( pszFullname );
        return( NULL );
    }

    sprintf( pszFullname, "%s.shx", pszBasename );
    psSHP->fpSHX = fopen(pszFullname, pszAccess );
    if( psSHP->fpSHX == NULL )
    {
        sprintf( pszFullname, "%s.SHX", pszBasename );
        psSHP->fpSHX = fopen(pszFullname, pszAccess );
    }
    
    if( psSHP->fpSHX == NULL )
    {
        fclose( psSHP->fpSHP );
        free( psSHP );
        free( pszBasename );
        free( pszFullname );
        return( NULL );
    }

    free( pszFullname );
    free( pszBasename );

// -------------------------------------------------------------------- //
//  Read the file size from the SHP file.				//
// -------------------------------------------------------------------- //
    pabyBuf = (uchar *) malloc(100);
    fread( pabyBuf, 100, 1, psSHP->fpSHP );

    psSHP->nFileSize = (pabyBuf[24] * 256 * 256 * 256
			+ pabyBuf[25] * 256 * 256
			+ pabyBuf[26] * 256
			+ pabyBuf[27]) * 2;

// -------------------------------------------------------------------- //
//  Read SHX file Header info                                           //
// -------------------------------------------------------------------- //
    fread( pabyBuf, 100, 1, psSHP->fpSHX );

    if( pabyBuf[0] != 0 
        || pabyBuf[1] != 0 
        || pabyBuf[2] != 0x27 
        || (pabyBuf[3] != 0x0a && pabyBuf[3] != 0x0d) )
    {
	fclose( psSHP->fpSHP );
	fclose( psSHP->fpSHX );
	free( psSHP );

	return( NULL );
    }

    psSHP->nRecords = pabyBuf[27] + pabyBuf[26] * 256
      + pabyBuf[25] * 256 * 256 + pabyBuf[24] * 256 * 256 * 256;
    psSHP->nRecords = (psSHP->nRecords*2 - 100) / 8;

    psSHP->nShapeType = pabyBuf[32];

    if( psSHP->nRecords < 0 || psSHP->nRecords > 256000000 )
    {
        // this header appears to be corrupt.  Give up. //
	fclose( psSHP->fpSHP );
	fclose( psSHP->fpSHX );
	free( psSHP );

	return( NULL );
    }

// -------------------------------------------------------------------- //
//      Read the bounds.                                                //
// -------------------------------------------------------------------- //
    if( bBigEndian ) SwapWord( 8, pabyBuf+36 );
    memcpy( &dValue, pabyBuf+36, 8 );
    psSHP->adBoundsMin[0] = dValue;

    if( bBigEndian ) SwapWord( 8, pabyBuf+44 );
    memcpy( &dValue, pabyBuf+44, 8 );
    psSHP->adBoundsMin[1] = dValue;

    if( bBigEndian ) SwapWord( 8, pabyBuf+52 );
    memcpy( &dValue, pabyBuf+52, 8 );
    psSHP->adBoundsMax[0] = dValue;

    if( bBigEndian ) SwapWord( 8, pabyBuf+60 );
    memcpy( &dValue, pabyBuf+60, 8 );
    psSHP->adBoundsMax[1] = dValue;

    if( bBigEndian ) SwapWord( 8, pabyBuf+68 );		// z //
    memcpy( &dValue, pabyBuf+68, 8 );
    psSHP->adBoundsMin[2] = dValue;
    
    if( bBigEndian ) SwapWord( 8, pabyBuf+76 );
    memcpy( &dValue, pabyBuf+76, 8 );
    psSHP->adBoundsMax[2] = dValue;
    
    if( bBigEndian ) SwapWord( 8, pabyBuf+84 );		// z //
    memcpy( &dValue, pabyBuf+84, 8 );
    psSHP->adBoundsMin[3] = dValue;

    if( bBigEndian ) SwapWord( 8, pabyBuf+92 );
    memcpy( &dValue, pabyBuf+92, 8 );
    psSHP->adBoundsMax[3] = dValue;

    free( pabyBuf );

// -------------------------------------------------------------------- //
//	Read the .shx file to get the offsets to each record in 	//
//	the .shp file.							//
// -------------------------------------------------------------------- //
    psSHP->nMaxRecords = psSHP->nRecords;

    psSHP->panRecOffset =
        (int *) malloc(sizeof(int) * MAX(1,psSHP->nMaxRecords) );
    psSHP->panRecSize =
        (int *) malloc(sizeof(int) * MAX(1,psSHP->nMaxRecords) );

    pabyBuf = (uchar *) malloc(8 * MAX(1,psSHP->nRecords) );
    fread( pabyBuf, 8, psSHP->nRecords, psSHP->fpSHX );

    for( i = 0; i < psSHP->nRecords; i++ )
    {
	int32		nOffset, nLength;

	memcpy( &nOffset, pabyBuf + i * 8, 4 );
	if( !bBigEndian ) SwapWord( 4, &nOffset );

	memcpy( &nLength, pabyBuf + i * 8 + 4, 4 );
	if( !bBigEndian ) SwapWord( 4, &nLength );

	psSHP->panRecOffset[i] = nOffset*2;
	psSHP->panRecSize[i] = nLength*2;
    }
    free( pabyBuf );

    return( psSHP );
}
/////////////////////////////////////////////////////////////////////
//	  函数名:	CSHP::SHPOpenTxt()
//	  功  能:	打开shp和shx文件，并读取shp头信息和shx整个文件，Txt格式
//	  原  理:	
//	  输  入:	(const char * pszLayer,          //文件名,包含路径
//               const char * pszAccess)         //文件打开方式            
//	  输  出:	无
//	  返回值:   不成功:NULL;成功:SHPHandle psSHP //打开的文件指针
//    赋  值:   无
//	  注  意:   打开方式(DQY)修改为"a"或"a+",将信息添加到文件尾
//              只有需要添加时才调用,不要将信息添加到非目标文件尾
//              调用函数SwapWord()
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
SHPHandle SHPAPI_CALL
CSHP::SHPOpenTxt( const char * pszLayer, const char * pszAccess )

{
    char		*pszFullname, *pszBasename;
    SHPHandle		psSHP;
    
    int			i;
    
// -------------------------------------------------------------------- //
//      Ensure the access string is one of the legal ones.  We          //
//      ensure the result string indicates binary to avoid common       //
//      problems on Windows.                                            //
// -------------------------------------------------------------------- //
    if( strcmp(pszAccess,"r+b") == 0 || strcmp(pszAccess,"rb+") == 0
        || strcmp(pszAccess,"r+") == 0 )
        pszAccess = "a";
    else
        pszAccess = "a+";
    
// -------------------------------------------------------------------- //
//	Establish the byte order on this machine.			//
// -------------------------------------------------------------------- //
    i = 1;
    if( *((uchar *) &i) == 1 )
        bBigEndian = FALSE;
    else
        bBigEndian = TRUE;

// -------------------------------------------------------------------- //
//	Initialize the info structure.					//
// -------------------------------------------------------------------- //
    psSHP = (SHPHandle) calloc(sizeof(SHPInfo),1);

    psSHP->bUpdated = FALSE;

// -------------------------------------------------------------------- //
//	Compute the base (layer) name.  If there is any extension	//
//	on the passed in filename we will strip it off.			//
// -------------------------------------------------------------------- //
    pszBasename = (char *) malloc(strlen(pszLayer)+5);
    strcpy( pszBasename, pszLayer );
    for( i = strlen(pszBasename)-1; 
	 i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
	       && pszBasename[i] != '\\';
	 i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

// -------------------------------------------------------------------- //
//	Open the .shp and .shx files.  Note that files pulled from	//
//	a PC to Unix with upper case filenames won't work!		//
// -------------------------------------------------------------------- //
    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf( pszFullname, "%s.shp", pszBasename );
    psSHP->fpSHP = fopen(pszFullname, pszAccess );
    if( psSHP->fpSHP == NULL )
    {
        sprintf( pszFullname, "%s.SHP", pszBasename );
        psSHP->fpSHP = fopen(pszFullname, pszAccess );
    }
    
    if( psSHP->fpSHP == NULL )
    {
        free( psSHP );
        free( pszBasename );
        free( pszFullname );
        return( NULL );
    }

    sprintf( pszFullname, "%s.shx", pszBasename );
    psSHP->fpSHX = fopen(pszFullname, pszAccess );
    if( psSHP->fpSHX == NULL )
    {
        sprintf( pszFullname, "%s.SHX", pszBasename );
        psSHP->fpSHX = fopen(pszFullname, pszAccess );
    }
    
    if( psSHP->fpSHX == NULL )
    {
        fclose( psSHP->fpSHP );
        free( psSHP );
        free( pszBasename );
        free( pszFullname );
        return( NULL );
    }

    free( pszFullname );
    free( pszBasename );

    return( psSHP );
}
//////////////////////////////////////////////////////////////////////////
//                              SHPClose()                              //
//								                                       	//
//                  	Close the .shp and .shx files.					//
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	SHPClose()
//	  功  能:	关闭二进制文本
//	  原  理:	
//	  输  入:	( SHPHandle psSHP )         //要关闭的文件指针
//	  输  出:	无
//	  返回值:   无
//    赋  值:   无
//	  注  意:   更新为真时会调用SHPWriteHeader()函数,更新数据范围
//              调用实例
//    信  息:   DQY 注释 2006.11
////////////////////////////////////////////////////////////////////
void SHPAPI_CALL
CSHP::SHPClose(SHPHandle psSHP )

{
// -------------------------------------------------------------------- //
//	Update the header if we have modified anything.			//
// -------------------------------------------------------------------- //
    if( psSHP->bUpdated )
    {
	SHPWriteHeader( psSHP );
    }

// -------------------------------------------------------------------- //
//      Free all resources, and close files.                            //
// -------------------------------------------------------------------- //
    free( psSHP->panRecOffset );
    free( psSHP->panRecSize );

    fclose( psSHP->fpSHX );
    fclose( psSHP->fpSHP );

    if( psSHP->pabyRec != NULL )
    {
        free( psSHP->pabyRec );
    }
    
    free( psSHP );
}
//////////////////////////////////////////////////////////////////////////
//                           SHPCloseTxt()                              //
//								                                    	//
//	                  Close the .shp and .shx files.					//
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	SHPCloseTxt()
//	  功  能:	关闭文本文件
//	  原  理:	
//	  输  入:	( SHPHandle psSHP )         //要关闭的文件指针
//	  输  出:	无
//	  返回值:   无
//    赋  值:   无
//	  注  意:   更新文件头信息(地图范围数据)被注释
//              调用实例
//    信  息:   DQY 仿SHPClose()函数编写 2006.11
////////////////////////////////////////////////////////////////////
void SHPAPI_CALL
CSHP::SHPCloseTxt(SHPHandle psSHP )

{
// -------------------------------------------------------------------- //
//	Update the header if we have modified anything.			//
// -------------------------------------------------------------------- //

//     if( psSHP->bUpdated )
//     {
// 	SHPWriteHeader( psSHP );
//     }

// -------------------------------------------------------------------- //
//      Free all resources, and close files.                            //
// -------------------------------------------------------------------- //
    free( psSHP->panRecOffset );
    free( psSHP->panRecSize );

    fclose( psSHP->fpSHX );
    fclose( psSHP->fpSHP );

    if( psSHP->pabyRec != NULL )
    {
        free( psSHP->pabyRec );
    }
    
    free( psSHP );
}
//////////////////////////////////////////////////////////////////////////
//                             SHPGetInfo()                             //
//                                                                      //
//      Fetch general information about the shape file.                 //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	CSHP::SHPGetInfo()
//	  功  能:	获取文件基本信息:实体个数,类型,边界double Min[4]Max[4]
//	  原  理:	
//	  输  入:	(SHPHandle psSHP,                 //文件指针
//               int * pnEntities,                //实体个数
//               int * pnShapeType,               //实体类型
//               double * padfMinBound,           //边界最小值
//               double * padfMaxBound)           //边界最大值
//	  输  出:	无
//	  返回值:   无
//    赋  值:   四个   //实体个数,类型,边界double Min[4]Max[4]
//	  注  意:   
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
void SHPAPI_CALL
CSHP::SHPGetInfo(SHPHandle psSHP, int * pnEntities, int * pnShapeType,
           double * padfMinBound, double * padfMaxBound )

{
    int		i;
    
    if( pnEntities != NULL )
        *pnEntities = psSHP->nRecords;

    if( pnShapeType != NULL )
        *pnShapeType = psSHP->nShapeType;

    for( i = 0; i < 4; i++ )
    {
        if( padfMinBound != NULL )
            padfMinBound[i] = psSHP->adBoundsMin[i];
        if( padfMaxBound != NULL )
            padfMaxBound[i] = psSHP->adBoundsMax[i];
    }
}

//////////////////////////////////////////////////////////////////////////
//                             SHPCreate()                              //
//                                                                      //
//      Create a new shape file and return a handle to the open         //
//      shape file with read/write access.                              //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	SHPCreate()
//	  功  能:	创建SHP文件和SHX文件,并写入头信息
//	  原  理:	
//	  输  入:	(const char * pszLayer,          //要创建的文件名
//               int nShapeType)                 //文件中的实体类型         
//	  输  出:	无
//	  返回值:   SHPHandle   //创建好的文件指针
//    赋  值:   无   
//	  注  意:   创建好文件后关闭,再通过SHPOpen()打开
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
SHPHandle SHPAPI_CALL
CSHP::SHPCreate( const char * pszLayer, int nShapeType )

{
    char	*pszBasename, *pszFullname;
    int		i;
    FILE	*fpSHP, *fpSHX;
    uchar     	abyHeader[100];
    int32	i32;
    double	dValue;
    
// -------------------------------------------------------------------- //
//      Establish the byte order on this system.                        //
// -------------------------------------------------------------------- //
    i = 1;
    if( *((uchar *) &i) == 1 )
        bBigEndian = FALSE;
    else
        bBigEndian = TRUE;

// -------------------------------------------------------------------- //
//	Compute the base (layer) name.  If there is any extension	//
//	on the passed in filename we will strip it off.			//
// -------------------------------------------------------------------- //
   pszBasename = (char *) malloc(strlen(pszLayer)+5);
    strcpy( pszBasename, pszLayer );
    for( i = strlen(pszBasename)-1; 
	 i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
	       && pszBasename[i] != '\\';
	 i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

// -------------------------------------------------------------------- //
//      Open the two files so we can write their headers.               //
// -------------------------------------------------------------------- //
    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf( pszFullname, "%s.shp", pszBasename );
    fpSHP = fopen(pszFullname, "wb" );
    if( fpSHP == NULL )
        return( NULL );

    sprintf( pszFullname, "%s.shx", pszBasename );
    fpSHX = fopen(pszFullname, "wb" );
    if( fpSHX == NULL )
        return( NULL );

    free( pszFullname );
    free( pszBasename );

// -------------------------------------------------------------------- //
//      Prepare header block for .shp file.                             //
// -------------------------------------------------------------------- //
    for( i = 0; i < 100; i++ )
      abyHeader[i] = 0;

    abyHeader[2] = 0x27;				// magic cookie //
    abyHeader[3] = 0x0a;

    i32 = 50;						// file size //
    ByteCopy( &i32, abyHeader+24, 4 );
    if( !bBigEndian ) SwapWord( 4, abyHeader+24 );
    
    i32 = 1000;						// version //
    ByteCopy( &i32, abyHeader+28, 4 );
    if( bBigEndian ) SwapWord( 4, abyHeader+28 );
    
    i32 = nShapeType;					// shape type //
    ByteCopy( &i32, abyHeader+32, 4 );
    if( bBigEndian ) SwapWord( 4, abyHeader+32 );

    dValue = 0.0;					// set bounds //
    ByteCopy( &dValue, abyHeader+36, 8 );
    ByteCopy( &dValue, abyHeader+44, 8 );
    ByteCopy( &dValue, abyHeader+52, 8 );
    ByteCopy( &dValue, abyHeader+60, 8 );

// -------------------------------------------------------------------- //
//      Write .shp file header.                                         //
// -------------------------------------------------------------------- //
    fwrite( abyHeader, 100, 1, fpSHP );

// -------------------------------------------------------------------- //
//      Prepare, and write .shx file header.                            //
// -------------------------------------------------------------------- //
    i32 = 50;						// file size //
    ByteCopy( &i32, abyHeader+24, 4 );
    if( !bBigEndian ) SwapWord( 4, abyHeader+24 );
    
    fwrite( abyHeader, 100, 1, fpSHX );

// -------------------------------------------------------------------- //
//      Close the files, and then open them as regular existing files.  //
// -------------------------------------------------------------------- //
    fclose( fpSHP );
    fclose( fpSHX );

    return( SHPOpen( pszLayer, "r+b" ) );
}
//////////////////////////////////////////////////////////////////////////
//                             SHPCreate()                              //
//                                                                      //
//      Create a new shape file and return a handle to the open         //
//      shape file with read/write access.                              //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	SHPCreateHeaderTxt()
//	  功  能:	创建SHP文件和SHX文件并写入头信息,txt格式
//	  原  理:	
//	  输  入:	(SHPHandle psSHP,                //已打开的原文件指针
//               const char * pszLayer,          //要创建的文件名
//               int nShapeType)                 //文件中的实体类型         
//	  输  出:	无
//	  返回值:   SHPHandle   //创建好的文件指针
//    赋  值:   无   
//	  注  意:   psSHP!=NULL
//              创建好文件后关闭,再通过SHPOpenTxt()打开,打开方式为文件形式
//              调用实例
//    信  息:   DQY 模仿SHPCreate()编写 2006.11
////////////////////////////////////////////////////////////////////
SHPHandle SHPAPI_CALL
CSHP::SHPCreateHeaderTxt( SHPHandle psSHP,const char * pszLayer, int nShapeType )

{
	if (psSHP==NULL)
	{
		return NULL;
	}

    char	*pszBasename, *pszFullname;
    int		i;
    FILE	*fpSHP, *fpSHX;
    //uchar     	abyHeader[100];
    //int32	i32;
    //double	dValue;
    
// -------------------------------------------------------------------- //
//      Establish the byte order on this system.                        //
// -------------------------------------------------------------------- //
    i = 1;
    if( *((uchar *) &i) == 1 )
        bBigEndian = FALSE;
    else
        bBigEndian = TRUE;

// -------------------------------------------------------------------- //
//	Compute the base (layer) name.  If there is any extension	//
//	on the passed in filename we will strip it off.			//
// -------------------------------------------------------------------- //
    pszBasename = (char *) malloc(strlen(pszLayer)+5);
    strcpy( pszBasename, pszLayer );
    for( i = strlen(pszBasename)-1; 
	 i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
	       && pszBasename[i] != '\\';
	 i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

// -------------------------------------------------------------------- //
//      Open the two files so we can write their headers.               //
// -------------------------------------------------------------------- //
    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf( pszFullname, "%s.shp", pszBasename );
    fpSHP = fopen(pszFullname, "w" );
    if( fpSHP == NULL )
        return( NULL );

    sprintf( pszFullname, "%s.shx", pszBasename );
    fpSHX = fopen(pszFullname, "w" );
    if( fpSHX == NULL )
        return( NULL );

    free( pszFullname );
    free( pszBasename );

// -------------------------------------------------------------------- //
//      Prepare header block for .shp file.                             //
// -------------------------------------------------------------------- //
	
// -------------------------------------------------------------------- //
//      Write .shp file header.                                         //
// -------------------------------------------------------------------- //	fprintf(fpSHP,"SHP File Header:\n");
    fprintf(fpSHP,"File Length = %d Bytes\n",psSHP->nFileSize);    // file size //
	//fprintf(fpSHP,"Version = 1000\n",);                    // version //
	fprintf(fpSHP,"Shape Type = %d\n",psSHP->nShapeType);     // shape type //
	fprintf(fpSHP,"Bounding Box(Xmin,Ymin,Zmin,Mmin):\n");   //  bounds //
	fprintf(fpSHP,"%lf,%lf,%lf,%lf\n",psSHP->adBoundsMin[0],psSHP->adBoundsMin[1],
		                           psSHP->adBoundsMin[2],psSHP->adBoundsMin[3]);
	fprintf(fpSHP,"Bounding Box(Xmax,Ymax,Zmax,Mmax):\n");
	fprintf(fpSHP,"%lf,%lf,%lf,%lf\n",psSHP->adBoundsMax[0],psSHP->adBoundsMax[1],
		                           psSHP->adBoundsMax[2],psSHP->adBoundsMax[3]);
	fprintf(fpSHP,"Records:\n");
// -------------------------------------------------------------------- //
//      Prepare, and write .shx file header.                            //
// -------------------------------------------------------------------- //
	fprintf(fpSHX,"File Length = %d Bytes\n",8*psSHP->nRecords+100);  // file size //
	//fprintf(fpSHX,"Version = 1000\n",);                    // version //
	fprintf(fpSHX,"Shape Type = %d\n",psSHP->nShapeType);     // shape type //
	fprintf(fpSHX,"Bounding Box(Xmin,Ymin,Zmin,Mmin):\n");   //  bounds //
	fprintf(fpSHX,"%lf,%lf,%lf,%lf\n",psSHP->adBoundsMin[0],psSHP->adBoundsMin[1],
		                           psSHP->adBoundsMin[2],psSHP->adBoundsMin[3]);
	fprintf(fpSHX,"Bounding Box(Xmax,Ymax,Zmax,Mmax):\n");
	fprintf(fpSHX,"%lf,%lf,%lf,%lf\n",psSHP->adBoundsMax[0],psSHP->adBoundsMax[1],
		                           psSHP->adBoundsMax[2],psSHP->adBoundsMax[3]);
	fprintf(fpSHX,"Records:\n");    
// -------------------------------------------------------------------- //
//      Close the files, and then open them as regular existing files.  //
// -------------------------------------------------------------------- //
    fclose( fpSHP );
    fclose( fpSHX );

    return( SHPOpenTxt( pszLayer, "r+" ) );
}
//////////////////////////////////////////////////////////////////////////
//                           _SHPSetBounds()                            //
//                                                                      //
//      Compute a bounds rectangle for a shape, and set it into the     //
//      indicated location in the record.                               //
//////////////////////////////////////////////////////////////////////////

static void	_SHPSetBounds( uchar * pabyRec, SHPObject * psShape )

{
    ByteCopy( &(psShape->dfXMin), pabyRec +  0, 8 );
    ByteCopy( &(psShape->dfYMin), pabyRec +  8, 8 );
    ByteCopy( &(psShape->dfXMax), pabyRec + 16, 8 );
    ByteCopy( &(psShape->dfYMax), pabyRec + 24, 8 );

    if( bBigEndian )
    {
        SwapWord( 8, pabyRec + 0 );
        SwapWord( 8, pabyRec + 8 );
        SwapWord( 8, pabyRec + 16 );
        SwapWord( 8, pabyRec + 24 );
    }
}

//////////////////////////////////////////////////////////////////////////
//                         SHPComputeExtents()                          //
//                                                                      //
//      Recompute the extents of a shape.  Automatically done by        //
//      SHPCreateObject().                                              //
//////////////////////////////////////////////////////////////////////////

void SHPAPI_CALL
CSHP::SHPComputeExtents( SHPObject * psObject )

{
    int		i;
    
// -------------------------------------------------------------------- //
//      Build extents for this object.                                  //
// -------------------------------------------------------------------- //
    if( psObject->nVertices > 0 )
    {
        psObject->dfXMin = psObject->dfXMax = psObject->padfX[0];
        psObject->dfYMin = psObject->dfYMax = psObject->padfY[0];
        psObject->dfZMin = psObject->dfZMax = psObject->padfZ[0];
        psObject->dfMMin = psObject->dfMMax = psObject->padfM[0];
    }
    
    for( i = 0; i < psObject->nVertices; i++ )
    {
        psObject->dfXMin = MIN(psObject->dfXMin, psObject->padfX[i]);
        psObject->dfYMin = MIN(psObject->dfYMin, psObject->padfY[i]);
        psObject->dfZMin = MIN(psObject->dfZMin, psObject->padfZ[i]);
        psObject->dfMMin = MIN(psObject->dfMMin, psObject->padfM[i]);

        psObject->dfXMax = MAX(psObject->dfXMax, psObject->padfX[i]);
        psObject->dfYMax = MAX(psObject->dfYMax, psObject->padfY[i]);
        psObject->dfZMax = MAX(psObject->dfZMax, psObject->padfZ[i]);
        psObject->dfMMax = MAX(psObject->dfMMax, psObject->padfM[i]);
    }
}

//////////////////////////////////////////////////////////////////////////
//                          SHPCreateObject()                           //
//                                                                      //
//      Create a shape object.  It should be freed with                 //
//      SHPDestroyObject().                                             //
//////////////////////////////////////////////////////////////////////////

SHPObject SHPAPI_CALL1(*)
CSHP::SHPCreateObject( int nSHPType, int nShapeId, int nParts,
                 int * panPartStart, int * panPartType,
                 int nVertices, double * padfX, double * padfY,
                 double * padfZ, double * padfM )

{
    SHPObject	*psObject;
    int		i, bHasM, bHasZ;

    psObject = (SHPObject *) calloc(1,sizeof(SHPObject));
    psObject->nSHPType = nSHPType;
    psObject->nShapeId = nShapeId;

// -------------------------------------------------------------------- //
//	Establish whether this shape type has M, and Z values.		//
// -------------------------------------------------------------------- //
    if( nSHPType == SHPT_ARCM
        || nSHPType == SHPT_POINTM
        || nSHPType == SHPT_POLYGONM
        || nSHPType == SHPT_MULTIPOINTM )
    {
        bHasM = TRUE;
        bHasZ = FALSE;
    }
    else if( nSHPType == SHPT_ARCZ
             || nSHPType == SHPT_POINTZ
             || nSHPType == SHPT_POLYGONZ
             || nSHPType == SHPT_MULTIPOINTZ
             || nSHPType == SHPT_MULTIPATCH )
    {
        bHasM = TRUE;
        bHasZ = TRUE;
    }
    else
    {
        bHasM = FALSE;
        bHasZ = FALSE;
    }

// -------------------------------------------------------------------- //
//      Capture parts.  Note that part type is optional, and            //
//      defaults to ring.                                               //
// -------------------------------------------------------------------- //
    if( nSHPType == SHPT_ARC || nSHPType == SHPT_POLYGON
        || nSHPType == SHPT_ARCM || nSHPType == SHPT_POLYGONM
        || nSHPType == SHPT_ARCZ || nSHPType == SHPT_POLYGONZ
        || nSHPType == SHPT_MULTIPATCH )
    {
        psObject->nParts = MAX(1,nParts);

        psObject->panPartStart = (int *)
            malloc(sizeof(int) * psObject->nParts);
        psObject->panPartType = (int *)
            malloc(sizeof(int) * psObject->nParts);

        psObject->panPartStart[0] = 0;
        psObject->panPartType[0] = SHPP_RING;
        
        for( i = 0; i < nParts; i++ )
        {
            psObject->panPartStart[i] = panPartStart[i];
            if( panPartType != NULL )
                psObject->panPartType[i] = panPartType[i];
            else
                psObject->panPartType[i] = SHPP_RING;
        }
    }

// -------------------------------------------------------------------- //
//      Capture vertices.  Note that Z and M are optional, but X and    //
//      Y are not.                                                      //
// -------------------------------------------------------------------- //
    if( nVertices > 0 )
    {
        psObject->padfX = (double *) calloc(sizeof(double),nVertices);
        psObject->padfY = (double *) calloc(sizeof(double),nVertices);
        psObject->padfZ = (double *) calloc(sizeof(double),nVertices);
        psObject->padfM = (double *) calloc(sizeof(double),nVertices);

        assert( padfX != NULL );
        assert( padfY != NULL );
    
        for( i = 0; i < nVertices; i++ )
        {
            psObject->padfX[i] = padfX[i];
            psObject->padfY[i] = padfY[i];
            if( padfZ != NULL && bHasZ )
                psObject->padfZ[i] = padfZ[i];
            if( padfM != NULL && bHasM )
                psObject->padfM[i] = padfM[i];
        }
    }

// -------------------------------------------------------------------- //
//      Compute the extents.                                            //
// -------------------------------------------------------------------- //
    psObject->nVertices = nVertices;
    SHPComputeExtents( psObject );

    return( psObject );
}

//////////////////////////////////////////////////////////////////////////
//                       SHPCreateSimpleObject()                        //
//                                                                      //
//      Create a simple (common) shape object.  Destroy with            //
//      SHPDestroyObject().                                             //
//////////////////////////////////////////////////////////////////////////

SHPObject SHPAPI_CALL1(*)
CSHP::SHPCreateSimpleObject( int nSHPType, int nVertices,
                       double * padfX, double * padfY,
                       double * padfZ )

{
    return( SHPCreateObject( nSHPType, -1, 0, NULL, NULL,
                             nVertices, padfX, padfY, padfZ, NULL ) );
}
                                  
//////////////////////////////////////////////////////////////////////////
//                           SHPWriteObject()                           //
//                                                                      //
//      Write out the vertices of a new structure.  Note that it is     //
//      only possible to write vertices at the end of the file.         //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	SHPWriteObject()
//	  功  能:	写入单个实体信息
//	  原  理:	
//	  输  入:	(SHPHandle psSHP,          //原数据文件指针
//               int nShapeId,             //实体ID
//               SHPObject * psObject)     // 实体的数据指针        
//	  输  出:	无
//	  返回值:   int nShapeId           //实体ID,可能相对于传入值有所改动
//    赋  值:   无
//	  注  意:   
//              调用实例
//    信  息:   DQY 注释 2006.11
////////////////////////////////////////////////////////////////////
int SHPAPI_CALL
CSHP::SHPWriteObject(SHPHandle psSHP, int nShapeId, SHPObject * psObject )
		      
{
    int	       	nRecordOffset, i, nRecordSize;
    uchar	*pabyRec;
    int32	i32;

    psSHP->bUpdated = TRUE;

// -------------------------------------------------------------------- //
//      Ensure that shape object matches the type of the file it is     //
//      being written to.                                               //
// -------------------------------------------------------------------- //
    assert( psObject->nSHPType == psSHP->nShapeType 
            || psObject->nSHPType == SHPT_NULL );

// -------------------------------------------------------------------- //
//      Ensure that -1 is used for appends.  Either blow an             //
//      assertion, or if they are disabled, set the shapeid to -1       //
//      for appends.                                                    //
// -------------------------------------------------------------------- //
    assert( nShapeId == -1 
            || (nShapeId >= 0 && nShapeId < psSHP->nRecords) );

    if( nShapeId != -1 && nShapeId >= psSHP->nRecords )
        nShapeId = -1;

// -------------------------------------------------------------------- //
//      Add the new entity to the in memory index.                      //
// -------------------------------------------------------------------- //
    if( nShapeId == -1 && psSHP->nRecords+1 > psSHP->nMaxRecords )
    {
	psSHP->nMaxRecords =(int) ( psSHP->nMaxRecords * 1.3 + 100);

	psSHP->panRecOffset = (int *) 
            SfRealloc(psSHP->panRecOffset,sizeof(int) * psSHP->nMaxRecords );
	psSHP->panRecSize = (int *) 
            SfRealloc(psSHP->panRecSize,sizeof(int) * psSHP->nMaxRecords );
    }

// -------------------------------------------------------------------- //
//      Initialize record.                                              //
// -------------------------------------------------------------------- //
    pabyRec = (uchar *) malloc(psObject->nVertices * 4 * sizeof(double) 
			       + psObject->nParts * 8 + 128);
    
// -------------------------------------------------------------------- //
//  Extract vertices for a Polygon or Arc.				//
// -------------------------------------------------------------------- //
    if( psObject->nSHPType == SHPT_POLYGON
        || psObject->nSHPType == SHPT_POLYGONZ
        || psObject->nSHPType == SHPT_POLYGONM
        || psObject->nSHPType == SHPT_ARC 
        || psObject->nSHPType == SHPT_ARCZ
        || psObject->nSHPType == SHPT_ARCM
        || psObject->nSHPType == SHPT_MULTIPATCH )
    {
	int32		nPoints, nParts;
	int    		i;

	nPoints = psObject->nVertices;
	nParts = psObject->nParts;

	_SHPSetBounds( pabyRec + 12, psObject );

	if( bBigEndian ) SwapWord( 4, &nPoints );
	if( bBigEndian ) SwapWord( 4, &nParts );

	ByteCopy( &nPoints, pabyRec + 40 + 8, 4 );
	ByteCopy( &nParts, pabyRec + 36 + 8, 4 );

        nRecordSize = 52;

        //Write part start positions.//

	ByteCopy( psObject->panPartStart, pabyRec + 44 + 8,
                  4 * psObject->nParts );
	for( i = 0; i < psObject->nParts; i++ )
	{
	    if( bBigEndian ) SwapWord( 4, pabyRec + 44 + 8 + 4*i );
            nRecordSize += 4;
	}

        //Write multipatch part types if needed.//

        if( psObject->nSHPType == SHPT_MULTIPATCH )
        {
            memcpy( pabyRec + nRecordSize, psObject->panPartType,
                    4*psObject->nParts );
            for( i = 0; i < psObject->nParts; i++ )
            {
                if( bBigEndian ) SwapWord( 4, pabyRec + nRecordSize );
                nRecordSize += 4;
            }
        }

        // Write the (x,y) vertex values. //
	for( i = 0; i < psObject->nVertices; i++ )
	{
	    ByteCopy( psObject->padfX + i, pabyRec + nRecordSize, 8 );
	    ByteCopy( psObject->padfY + i, pabyRec + nRecordSize + 8, 8 );

	    if( bBigEndian )
                SwapWord( 8, pabyRec + nRecordSize );
            
	    if( bBigEndian )
                SwapWord( 8, pabyRec + nRecordSize + 8 );

            nRecordSize += 2 * 8;
	}

        // Write the Z coordinates (if any). //
        if( psObject->nSHPType == SHPT_POLYGONZ
            || psObject->nSHPType == SHPT_ARCZ
            || psObject->nSHPType == SHPT_MULTIPATCH )
        {
            ByteCopy( &(psObject->dfZMin), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
            
            ByteCopy( &(psObject->dfZMax), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;

            for( i = 0; i < psObject->nVertices; i++ )
            {
                ByteCopy( psObject->padfZ + i, pabyRec + nRecordSize, 8 );
                if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
                nRecordSize += 8;
            }
        }

        // Write the M values, if any. //
        if( psObject->nSHPType == SHPT_POLYGONM
            || psObject->nSHPType == SHPT_ARCM
#ifndef DISABLE_MULTIPATCH_MEASURE            
            || psObject->nSHPType == SHPT_MULTIPATCH
#endif            
            || psObject->nSHPType == SHPT_POLYGONZ
            || psObject->nSHPType == SHPT_ARCZ )
        {
            ByteCopy( &(psObject->dfMMin), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
            
            ByteCopy( &(psObject->dfMMax), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;

            for( i = 0; i < psObject->nVertices; i++ )
            {
                ByteCopy( psObject->padfM + i, pabyRec + nRecordSize, 8 );
                if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
                nRecordSize += 8;
            }
        }
    }

// -------------------------------------------------------------------- //
//  Extract vertices for a MultiPoint.					//
// -------------------------------------------------------------------- //
    else if( psObject->nSHPType == SHPT_MULTIPOINT
             || psObject->nSHPType == SHPT_MULTIPOINTZ
             || psObject->nSHPType == SHPT_MULTIPOINTM )
    {
	int32		nPoints;
	int    		i;

	nPoints = psObject->nVertices;

        _SHPSetBounds( pabyRec + 12, psObject );

	if( bBigEndian ) SwapWord( 4, &nPoints );
	ByteCopy( &nPoints, pabyRec + 44, 4 );
	
	for( i = 0; i < psObject->nVertices; i++ )
	{
	    ByteCopy( psObject->padfX + i, pabyRec + 48 + i*16, 8 );
	    ByteCopy( psObject->padfY + i, pabyRec + 48 + i*16 + 8, 8 );

	    if( bBigEndian ) SwapWord( 8, pabyRec + 48 + i*16 );
	    if( bBigEndian ) SwapWord( 8, pabyRec + 48 + i*16 + 8 );
	}

	nRecordSize = 48 + 16 * psObject->nVertices;

        if( psObject->nSHPType == SHPT_MULTIPOINTZ )
        {
            ByteCopy( &(psObject->dfZMin), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;

            ByteCopy( &(psObject->dfZMax), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
            
            for( i = 0; i < psObject->nVertices; i++ )
            {
                ByteCopy( psObject->padfZ + i, pabyRec + nRecordSize, 8 );
                if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
                nRecordSize += 8;
            }
        }

        if( psObject->nSHPType == SHPT_MULTIPOINTZ
            || psObject->nSHPType == SHPT_MULTIPOINTM )
        {
            ByteCopy( &(psObject->dfMMin), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;

            ByteCopy( &(psObject->dfMMax), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
            
            for( i = 0; i < psObject->nVertices; i++ )
            {
                ByteCopy( psObject->padfM + i, pabyRec + nRecordSize, 8 );
                if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
                nRecordSize += 8;
            }
        }
    }

// -------------------------------------------------------------------- //
//      Write point.							//
// -------------------------------------------------------------------- //
    else if( psObject->nSHPType == SHPT_POINT
             || psObject->nSHPType == SHPT_POINTZ
             || psObject->nSHPType == SHPT_POINTM )
    {
	ByteCopy( psObject->padfX, pabyRec + 12, 8 );
	ByteCopy( psObject->padfY, pabyRec + 20, 8 );

	if( bBigEndian ) SwapWord( 8, pabyRec + 12 );
	if( bBigEndian ) SwapWord( 8, pabyRec + 20 );

        nRecordSize = 28;
        
        if( psObject->nSHPType == SHPT_POINTZ )
        {
            ByteCopy( psObject->padfZ, pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
        }
        
        if( psObject->nSHPType == SHPT_POINTZ
            || psObject->nSHPType == SHPT_POINTM )
        {
            ByteCopy( psObject->padfM, pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
        }
    }

// -------------------------------------------------------------------- //
//      Not much to do for null geometries.                             //
// -------------------------------------------------------------------- //
    else if( psObject->nSHPType == SHPT_NULL )
    {
        nRecordSize = 12;
    }

    else
    {
        // unknown type //
        assert( FALSE );
    }

// -------------------------------------------------------------------- //
//      Establish where we are going to put this record. If we are      //
//      rewriting and existing record, and it will fit, then put it     //
//      back where the original came from.  Otherwise write at the end. //
// -------------------------------------------------------------------- //
    if( nShapeId == -1 || psSHP->panRecSize[nShapeId] < nRecordSize-8 )
    {
        if( nShapeId == -1 )
            nShapeId = psSHP->nRecords++;

        psSHP->panRecOffset[nShapeId] = nRecordOffset = psSHP->nFileSize;
        psSHP->panRecSize[nShapeId] = nRecordSize-8;
        psSHP->nFileSize += nRecordSize;
    }
    else
    {
        nRecordOffset = psSHP->panRecOffset[nShapeId];
    }
    
// -------------------------------------------------------------------- //
//      Set the shape type, record number, and record size.             //
// -------------------------------------------------------------------- //
    i32 = nShapeId+1;					// record # //
    if( !bBigEndian ) SwapWord( 4, &i32 );
    ByteCopy( &i32, pabyRec, 4 );

    i32 = (nRecordSize-8)/2;				// record size //
    if( !bBigEndian ) SwapWord( 4, &i32 );
    ByteCopy( &i32, pabyRec + 4, 4 );

    i32 = psObject->nSHPType;				// shape type //
    if( bBigEndian ) SwapWord( 4, &i32 );
    ByteCopy( &i32, pabyRec + 8, 4 );

// -------------------------------------------------------------------- //
//      Write out record.                                               //
// -------------------------------------------------------------------- //
    if( fseek( psSHP->fpSHP, nRecordOffset, 0 ) != 0
        || fwrite( pabyRec, nRecordSize, 1, psSHP->fpSHP ) < 1 )
    {
        printf( "Error in fseek() or fwrite().\n" );
        free( pabyRec );
        return -1;
    }
    
    free( pabyRec );

// -------------------------------------------------------------------- //
//	Expand file wide bounds based on this shape.			//
// -------------------------------------------------------------------- //
    if( psSHP->adBoundsMin[0] == 0.0
        && psSHP->adBoundsMax[0] == 0.0
        && psSHP->adBoundsMin[1] == 0.0
        && psSHP->adBoundsMax[1] == 0.0 
        && psObject->nSHPType != SHPT_NULL )
    {
        psSHP->adBoundsMin[0] = psSHP->adBoundsMax[0] = psObject->padfX[0];
        psSHP->adBoundsMin[1] = psSHP->adBoundsMax[1] = psObject->padfY[0];
        psSHP->adBoundsMin[2] = psSHP->adBoundsMax[2] = psObject->padfZ[0];
        psSHP->adBoundsMin[3] = psSHP->adBoundsMax[3] = psObject->padfM[0];
    }

    for( i = 0; i < psObject->nVertices; i++ )
    {
	psSHP->adBoundsMin[0] = MIN(psSHP->adBoundsMin[0],psObject->padfX[i]);
	psSHP->adBoundsMin[1] = MIN(psSHP->adBoundsMin[1],psObject->padfY[i]);
	psSHP->adBoundsMin[2] = MIN(psSHP->adBoundsMin[2],psObject->padfZ[i]);
	psSHP->adBoundsMin[3] = MIN(psSHP->adBoundsMin[3],psObject->padfM[i]);
	psSHP->adBoundsMax[0] = MAX(psSHP->adBoundsMax[0],psObject->padfX[i]);
	psSHP->adBoundsMax[1] = MAX(psSHP->adBoundsMax[1],psObject->padfY[i]);
	psSHP->adBoundsMax[2] = MAX(psSHP->adBoundsMax[2],psObject->padfZ[i]);
	psSHP->adBoundsMax[3] = MAX(psSHP->adBoundsMax[3],psObject->padfM[i]);
    }

    return( nShapeId  );
}
//////////////////////////////////////////////////////////////////////////
//                           SHPWriteObject()                           //
//                                                                      //
//      Write out the vertices of a new structure.  Note that it is     //
//      only possible to write vertices at the end of the file.         //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	SHPWriteObjectTxt()
//	  功  能:	写入单个实体信息,Txt格式
//	  原  理:	
//	  输  入:	(SHPHandle psSHP,          //原数据文件指针
//               SHPHandle psOutSHP,       //要写入的文件的指针
//               int nShapeId,             //实体ID
//               SHPObject * psObject)     // 实体的数据指针        
//	  输  出:	无
//	  返回值:   int nShapeId           //实体ID,可能相对于传入值有所改动
//    赋  值:   无
//	  注  意:   两个文件指针可以用一个,有待进一步改进
//              调用实例
//    信  息:   DQY 仿SHPWriteObject函数编写 2006.11
////////////////////////////////////////////////////////////////////
int SHPAPI_CALL
CSHP::SHPWriteObjectTxt(SHPHandle psSHP, SHPHandle psOutSHP,int nShapeId, SHPObject * psObject )
		      
{
    int	       	nRecordOffset, i, nRecordSize;
    uchar	*pabyRec;
    int32	i32;

    psSHP->bUpdated = TRUE;

// -------------------------------------------------------------------- //
//      Ensure that shape object matches the type of the file it is     //
//      being written to.                                               //
// -------------------------------------------------------------------- //
    assert( psObject->nSHPType == psSHP->nShapeType 
            || psObject->nSHPType == SHPT_NULL );

// -------------------------------------------------------------------- //
//      Ensure that -1 is used for appends.  Either blow an             //
//      assertion, or if they are disabled, set the shapeid to -1       //
//      for appends.                                                    //
// -------------------------------------------------------------------- //
    assert( nShapeId == -1 
            || (nShapeId >= 0 && nShapeId < psSHP->nRecords) );

    if( nShapeId != -1 && nShapeId >= psSHP->nRecords )
        nShapeId = -1;

// -------------------------------------------------------------------- //
//      Add the new entity to the in memory index.                      //
// -------------------------------------------------------------------- //
    if( nShapeId == -1 && psSHP->nRecords+1 > psSHP->nMaxRecords )
    {
	psSHP->nMaxRecords =(int) ( psSHP->nMaxRecords * 1.3 + 100);

	psSHP->panRecOffset = (int *) 
            SfRealloc(psSHP->panRecOffset,sizeof(int) * psSHP->nMaxRecords );
	psSHP->panRecSize = (int *) 
            SfRealloc(psSHP->panRecSize,sizeof(int) * psSHP->nMaxRecords );
    }
// -------------------------------------------------------------------- //
//      写入记录头信息                                                  //
// -------------------------------------------------------------------- //
	fprintf(psOutSHP->fpSHP,"Record Header:Record Number = %d,Cpmtent Length\n",
		//psObject->nShapeId);
		nShapeId);
	fprintf(psOutSHP->fpSHX,"Record Header:Record Number = %d,Cpmtent Length\n",
		//psObject->nShapeId);
		nShapeId);
// -------------------------------------------------------------------- //
//      Initialize record.                                              //
// -------------------------------------------------------------------- //
    pabyRec = (uchar *) malloc(psObject->nVertices * 4 * sizeof(double) 
			       + psObject->nParts * 8 + 128);
    
// -------------------------------------------------------------------- //
//  Extract vertices for a Polygon or Arc.				//
// -------------------------------------------------------------------- //
    if( psObject->nSHPType == SHPT_POLYGON
        || psObject->nSHPType == SHPT_POLYGONZ
        || psObject->nSHPType == SHPT_POLYGONM
        || psObject->nSHPType == SHPT_ARC 
        || psObject->nSHPType == SHPT_ARCZ
        || psObject->nSHPType == SHPT_ARCM
        || psObject->nSHPType == SHPT_MULTIPATCH )
    {
	int32		nPoints, nParts;
	int    		i;

	nPoints = psObject->nVertices;
	nParts = psObject->nParts;

	_SHPSetBounds( pabyRec + 12, psObject );

	if( bBigEndian ) SwapWord( 4, &nPoints );
	if( bBigEndian ) SwapWord( 4, &nParts );

	ByteCopy( &nPoints, pabyRec + 40 + 8, 4 );
	ByteCopy( &nParts, pabyRec + 36 + 8, 4 );

        nRecordSize = 52;

        //Write part start positions.//

	ByteCopy( psObject->panPartStart, pabyRec + 44 + 8,
                  4 * psObject->nParts );
	for( i = 0; i < psObject->nParts; i++ )
	{
	    if( bBigEndian ) SwapWord( 4, pabyRec + 44 + 8 + 4*i );
            nRecordSize += 4;
	}

        //Write multipatch part types if needed.//

        if( psObject->nSHPType == SHPT_MULTIPATCH )
        {
            memcpy( pabyRec + nRecordSize, psObject->panPartType,
                    4*psObject->nParts );
            for( i = 0; i < psObject->nParts; i++ )
            {
                if( bBigEndian ) SwapWord( 4, pabyRec + nRecordSize );
                nRecordSize += 4;
            }
        }

        // Write the (x,y) vertex values. //
	for( i = 0; i < psObject->nVertices; i++ )
	{
	    ByteCopy( psObject->padfX + i, pabyRec + nRecordSize, 8 );
	    ByteCopy( psObject->padfY + i, pabyRec + nRecordSize + 8, 8 );

	    if( bBigEndian )
                SwapWord( 8, pabyRec + nRecordSize );
            
	    if( bBigEndian )
                SwapWord( 8, pabyRec + nRecordSize + 8 );

            nRecordSize += 2 * 8;
	}

        // Write the Z coordinates (if any). //
        if( psObject->nSHPType == SHPT_POLYGONZ
            || psObject->nSHPType == SHPT_ARCZ
            || psObject->nSHPType == SHPT_MULTIPATCH )
        {
            ByteCopy( &(psObject->dfZMin), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
            
            ByteCopy( &(psObject->dfZMax), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;

            for( i = 0; i < psObject->nVertices; i++ )
            {
                ByteCopy( psObject->padfZ + i, pabyRec + nRecordSize, 8 );
                if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
                nRecordSize += 8;
            }
        }

        // Write the M values, if any. //
        if( psObject->nSHPType == SHPT_POLYGONM
            || psObject->nSHPType == SHPT_ARCM
#ifndef DISABLE_MULTIPATCH_MEASURE            
            || psObject->nSHPType == SHPT_MULTIPATCH
#endif            
            || psObject->nSHPType == SHPT_POLYGONZ
            || psObject->nSHPType == SHPT_ARCZ )
        {
            ByteCopy( &(psObject->dfMMin), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
            
            ByteCopy( &(psObject->dfMMax), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;

            for( i = 0; i < psObject->nVertices; i++ )
            {
                ByteCopy( psObject->padfM + i, pabyRec + nRecordSize, 8 );
                if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
                nRecordSize += 8;
            }
        }
    }

// -------------------------------------------------------------------- //
//  Extract vertices for a MultiPoint.					//
// -------------------------------------------------------------------- //
    else if( psObject->nSHPType == SHPT_MULTIPOINT
             || psObject->nSHPType == SHPT_MULTIPOINTZ
             || psObject->nSHPType == SHPT_MULTIPOINTM )
    {
	int32		nPoints;
	int    		i;

	nPoints = psObject->nVertices;

        _SHPSetBounds( pabyRec + 12, psObject );

	if( bBigEndian ) SwapWord( 4, &nPoints );
	ByteCopy( &nPoints, pabyRec + 44, 4 );
	
	for( i = 0; i < psObject->nVertices; i++ )
	{
	    ByteCopy( psObject->padfX + i, pabyRec + 48 + i*16, 8 );
	    ByteCopy( psObject->padfY + i, pabyRec + 48 + i*16 + 8, 8 );

	    if( bBigEndian ) SwapWord( 8, pabyRec + 48 + i*16 );
	    if( bBigEndian ) SwapWord( 8, pabyRec + 48 + i*16 + 8 );
	}

	nRecordSize = 48 + 16 * psObject->nVertices;

        if( psObject->nSHPType == SHPT_MULTIPOINTZ )
        {
            ByteCopy( &(psObject->dfZMin), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;

            ByteCopy( &(psObject->dfZMax), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
            
            for( i = 0; i < psObject->nVertices; i++ )
            {
                ByteCopy( psObject->padfZ + i, pabyRec + nRecordSize, 8 );
                if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
                nRecordSize += 8;
            }
        }

        if( psObject->nSHPType == SHPT_MULTIPOINTZ
            || psObject->nSHPType == SHPT_MULTIPOINTM )
        {
            ByteCopy( &(psObject->dfMMin), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;

            ByteCopy( &(psObject->dfMMax), pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
            
            for( i = 0; i < psObject->nVertices; i++ )
            {
                ByteCopy( psObject->padfM + i, pabyRec + nRecordSize, 8 );
                if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
                nRecordSize += 8;
            }
        }
    }

// -------------------------------------------------------------------- //
//      Write point.							//
// -------------------------------------------------------------------- //
    else if( psObject->nSHPType == SHPT_POINT
             || psObject->nSHPType == SHPT_POINTZ
             || psObject->nSHPType == SHPT_POINTM )
    {
	ByteCopy( psObject->padfX, pabyRec + 12, 8 );
	ByteCopy( psObject->padfY, pabyRec + 20, 8 );

	if( bBigEndian ) SwapWord( 8, pabyRec + 12 );
	if( bBigEndian ) SwapWord( 8, pabyRec + 20 );

        nRecordSize = 28;
        
        if( psObject->nSHPType == SHPT_POINTZ )
        {
            ByteCopy( psObject->padfZ, pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
        }
        
        if( psObject->nSHPType == SHPT_POINTZ
            || psObject->nSHPType == SHPT_POINTM )
        {
            ByteCopy( psObject->padfM, pabyRec + nRecordSize, 8 );
            if( bBigEndian ) SwapWord( 8, pabyRec + nRecordSize );
            nRecordSize += 8;
        }
    }

// -------------------------------------------------------------------- //
//      Not much to do for null geometries.                             //
// -------------------------------------------------------------------- //
    else if( psObject->nSHPType == SHPT_NULL )
    {
        nRecordSize = 12;
    }

    else
    {
        // unknown type //
        assert( FALSE );
    }

// -------------------------------------------------------------------- //
//      Establish where we are going to put this record. If we are      //
//      rewriting and existing record, and it will fit, then put it     //
//      back where the original came from.  Otherwise write at the end. //
// -------------------------------------------------------------------- //
    if( nShapeId == -1 || psSHP->panRecSize[nShapeId] < nRecordSize-8 )
    {
        if( nShapeId == -1 )
            nShapeId = psSHP->nRecords++;

        psSHP->panRecOffset[nShapeId] = nRecordOffset = psSHP->nFileSize;
        psSHP->panRecSize[nShapeId] = nRecordSize-8;
        psSHP->nFileSize += nRecordSize;
    }
    else
    {
        nRecordOffset = psSHP->panRecOffset[nShapeId];
    }
    
// -------------------------------------------------------------------- //
//      Set the shape type, record number, and record size.             //
// -------------------------------------------------------------------- //
    i32 = nShapeId+1;					// record # //
    if( !bBigEndian ) SwapWord( 4, &i32 );
    ByteCopy( &i32, pabyRec, 4 );

    i32 = (nRecordSize-8)/2;				// record size //
    if( !bBigEndian ) SwapWord( 4, &i32 );
    ByteCopy( &i32, pabyRec + 4, 4 );

    i32 = psObject->nSHPType;				// shape type //
    if( bBigEndian ) SwapWord( 4, &i32 );
    ByteCopy( &i32, pabyRec + 8, 4 );

// -------------------------------------------------------------------- //
//      写入记录信息    .                                               //
// -------------------------------------------------------------------- //
	fprintf(psOutSHP->fpSHP,"Entity ID= %d ,Shape Type = %d \n",
		psObject->nShapeId,psObject->nSHPType);
	fprintf(psOutSHP->fpSHP,"Entity Point Count = %d \n",psObject->nVertices);
	for( i = 0; i < psObject->nVertices; i++ )
    {
		fprintf(psOutSHP->fpSHP,"VerticeID = %d ,X = %lf ,Y= %lf ,Z= %lf ,M= %lf \n",
			i,psObject->padfX[i],psObject->padfY[i],psObject->padfZ[i],psObject->padfM[i]);
	}
// -------------------------------------------------------------------- //
//      Write out record.                                               //
// -------------------------------------------------------------------- //

//     if( fseek( psSHP->fpSHP, nRecordOffset, 0 ) != 0
//         || fwrite( pabyRec, nRecordSize, 1, psSHP->fpSHP ) < 1 )
//     {
//         printf( "Error in fseek() or fwrite().\n" );
//         free( pabyRec );
//         return -1;
//     }
    
    free( pabyRec );

// -------------------------------------------------------------------- //
//	Expand file wide bounds based on this shape.			//
// -------------------------------------------------------------------- //
    if( psSHP->adBoundsMin[0] == 0.0
        && psSHP->adBoundsMax[0] == 0.0
        && psSHP->adBoundsMin[1] == 0.0
        && psSHP->adBoundsMax[1] == 0.0 
        && psObject->nSHPType != SHPT_NULL )
    {
        psSHP->adBoundsMin[0] = psSHP->adBoundsMax[0] = psObject->padfX[0];
        psSHP->adBoundsMin[1] = psSHP->adBoundsMax[1] = psObject->padfY[0];
        psSHP->adBoundsMin[2] = psSHP->adBoundsMax[2] = psObject->padfZ[0];
        psSHP->adBoundsMin[3] = psSHP->adBoundsMax[3] = psObject->padfM[0];
    }

    for( i = 0; i < psObject->nVertices; i++ )
    {
	psSHP->adBoundsMin[0] = MIN(psSHP->adBoundsMin[0],psObject->padfX[i]);
	psSHP->adBoundsMin[1] = MIN(psSHP->adBoundsMin[1],psObject->padfY[i]);
	psSHP->adBoundsMin[2] = MIN(psSHP->adBoundsMin[2],psObject->padfZ[i]);
	psSHP->adBoundsMin[3] = MIN(psSHP->adBoundsMin[3],psObject->padfM[i]);
	psSHP->adBoundsMax[0] = MAX(psSHP->adBoundsMax[0],psObject->padfX[i]);
	psSHP->adBoundsMax[1] = MAX(psSHP->adBoundsMax[1],psObject->padfY[i]);
	psSHP->adBoundsMax[2] = MAX(psSHP->adBoundsMax[2],psObject->padfZ[i]);
	psSHP->adBoundsMax[3] = MAX(psSHP->adBoundsMax[3],psObject->padfM[i]);
    }

    return( nShapeId  );
}
//////////////////////////////////////////////////////////////////////////
//                          SHPReadObject()                             //
//                                                                      //
//      Read the vertices, parts, and other non-attribute information	//
//	for one shape.							//
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	CSHP::SHPReadObject()
//	  功  能:	读取单个实体
//	  原  理:	
//	  输  入:	(SHPHandle psSHP,            //文件指针
//               int hEntity)                //实体ID,从0开始
//	  输  出:	无
//	  返回值:   单个实体
//    赋  值:   无   //
//	  注  意:   
//				调用实例
//              for( i = 0; i < nEntities; i++ )
//              {
//                  SHPObject	*psShape;              
//              	psShape = SHPReadObject( hSHP, i );
//                  if( SHPRewindObject( hSHP, psShape ) )
//                     nInvalidCount++;
//                  SHPWriteObject( hSHPOut, -1, psShape );
 //                 SHPDestroyObject( psShape );
 //              }
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
SHPObject SHPAPI_CALL1(*)
CSHP::SHPReadObject( SHPHandle psSHP, int hEntity )

{
    SHPObject		*psShape;

// -------------------------------------------------------------------- //
//      Validate the record/entity number.                              //
// -------------------------------------------------------------------- //
    if( hEntity < 0 || hEntity >= psSHP->nRecords )
        return( NULL );

// -------------------------------------------------------------------- //
//      Ensure our record buffer is large enough.                       //
// -------------------------------------------------------------------- //
    if( psSHP->panRecSize[hEntity]+8 > psSHP->nBufSize )
    {
	psSHP->nBufSize = psSHP->panRecSize[hEntity]+8;
	psSHP->pabyRec = (uchar *) SfRealloc(psSHP->pabyRec,psSHP->nBufSize);
    }

// -------------------------------------------------------------------- //
//      Read the record.                                                //
// -------------------------------------------------------------------- //
    fseek( psSHP->fpSHP, psSHP->panRecOffset[hEntity], 0 );
    fread( psSHP->pabyRec, psSHP->panRecSize[hEntity]+8, 1, psSHP->fpSHP );

// -------------------------------------------------------------------- //
//	Allocate and minimally initialize the object.			//
// -------------------------------------------------------------------- //
    psShape = (SHPObject *) calloc(1,sizeof(SHPObject));
    psShape->nShapeId = hEntity;

    memcpy( &psShape->nSHPType, psSHP->pabyRec + 8, 4 );
    if( bBigEndian ) SwapWord( 4, &(psShape->nSHPType) );

// ==================================================================== //
//  Extract vertices for a Polygon or Arc.				//
// ==================================================================== //
    if( psShape->nSHPType == SHPT_POLYGON || psShape->nSHPType == SHPT_ARC
        || psShape->nSHPType == SHPT_POLYGONZ
        || psShape->nSHPType == SHPT_POLYGONM
        || psShape->nSHPType == SHPT_ARCZ
        || psShape->nSHPType == SHPT_ARCM
        || psShape->nSHPType == SHPT_MULTIPATCH )
    {
	int32		nPoints, nParts;
	int    		i, nOffset;

// -------------------------------------------------------------------- //
//	Get the X/Y bounds.						//
// -------------------------------------------------------------------- //
        memcpy( &(psShape->dfXMin), psSHP->pabyRec + 8 +  4, 8 );
        memcpy( &(psShape->dfYMin), psSHP->pabyRec + 8 + 12, 8 );
        memcpy( &(psShape->dfXMax), psSHP->pabyRec + 8 + 20, 8 );
        memcpy( &(psShape->dfYMax), psSHP->pabyRec + 8 + 28, 8 );

	if( bBigEndian ) SwapWord( 8, &(psShape->dfXMin) );
	if( bBigEndian ) SwapWord( 8, &(psShape->dfYMin) );
	if( bBigEndian ) SwapWord( 8, &(psShape->dfXMax) );
	if( bBigEndian ) SwapWord( 8, &(psShape->dfYMax) );

// -------------------------------------------------------------------- //
//      Extract part/point count, and build vertex and part arrays      //
//      to proper size.                                                 //
// -------------------------------------------------------------------- //
	memcpy( &nPoints, psSHP->pabyRec + 40 + 8, 4 );
	memcpy( &nParts, psSHP->pabyRec + 36 + 8, 4 );

	if( bBigEndian ) SwapWord( 4, &nPoints );
	if( bBigEndian ) SwapWord( 4, &nParts );

	psShape->nVertices = nPoints;
        psShape->padfX = (double *) calloc(nPoints,sizeof(double));
        psShape->padfY = (double *) calloc(nPoints,sizeof(double));
        psShape->padfZ = (double *) calloc(nPoints,sizeof(double));
        psShape->padfM = (double *) calloc(nPoints,sizeof(double));

	psShape->nParts = nParts;
        psShape->panPartStart = (int *) calloc(nParts,sizeof(int));
        psShape->panPartType = (int *) calloc(nParts,sizeof(int));

        for( i = 0; i < nParts; i++ )
            psShape->panPartType[i] = SHPP_RING;

// -------------------------------------------------------------------- //
//      Copy out the part array from the record.                        //
// -------------------------------------------------------------------- //
	memcpy( psShape->panPartStart, psSHP->pabyRec + 44 + 8, 4 * nParts );
	for( i = 0; i < nParts; i++ )
	{
	    if( bBigEndian ) SwapWord( 4, psShape->panPartStart+i );
	}

	nOffset = 44 + 8 + 4*nParts;

// -------------------------------------------------------------------- //
//      If this is a multipatch, we will also have parts types.         //
// -------------------------------------------------------------------- //
        if( psShape->nSHPType == SHPT_MULTIPATCH )
        {
            memcpy( psShape->panPartType, psSHP->pabyRec + nOffset, 4*nParts );
            for( i = 0; i < nParts; i++ )
            {
                if( bBigEndian ) SwapWord( 4, psShape->panPartType+i );
            }

            nOffset += 4*nParts;
        }
        
// -------------------------------------------------------------------- //
//      Copy out the vertices from the record.                          //
// -------------------------------------------------------------------- //
	for( i = 0; i < nPoints; i++ )
	{
	    memcpy(psShape->padfX + i,
		   psSHP->pabyRec + nOffset + i * 16,
		   8 );

	    memcpy(psShape->padfY + i,
		   psSHP->pabyRec + nOffset + i * 16 + 8,
		   8 );

	    if( bBigEndian ) SwapWord( 8, psShape->padfX + i );
	    if( bBigEndian ) SwapWord( 8, psShape->padfY + i );
	}

        nOffset += 16*nPoints;
        
// -------------------------------------------------------------------- //
//      If we have a Z coordinate, collect that now.                    //
// -------------------------------------------------------------------- //
        if( psShape->nSHPType == SHPT_POLYGONZ
            || psShape->nSHPType == SHPT_ARCZ
            || psShape->nSHPType == SHPT_MULTIPATCH )
        {
            memcpy( &(psShape->dfZMin), psSHP->pabyRec + nOffset, 8 );
            memcpy( &(psShape->dfZMax), psSHP->pabyRec + nOffset + 8, 8 );
            
            if( bBigEndian ) SwapWord( 8, &(psShape->dfZMin) );
            if( bBigEndian ) SwapWord( 8, &(psShape->dfZMax) );
            
            for( i = 0; i < nPoints; i++ )
            {
                memcpy( psShape->padfZ + i,
                        psSHP->pabyRec + nOffset + 16 + i*8, 8 );
                if( bBigEndian ) SwapWord( 8, psShape->padfZ + i );
            }

            nOffset += 16 + 8*nPoints;
        }

// -------------------------------------------------------------------- //
//      If we have a M measure value, then read it now.  We assume      //
//      that the measure can be present for any shape if the size is    //
//      big enough, but really it will only occur for the Z shapes      //
//      (options), and the M shapes.                                    //
// -------------------------------------------------------------------- //
        if( psSHP->panRecSize[hEntity]+8 >= nOffset + 16 + 8*nPoints )
        {
            memcpy( &(psShape->dfMMin), psSHP->pabyRec + nOffset, 8 );
            memcpy( &(psShape->dfMMax), psSHP->pabyRec + nOffset + 8, 8 );
            
            if( bBigEndian ) SwapWord( 8, &(psShape->dfMMin) );
            if( bBigEndian ) SwapWord( 8, &(psShape->dfMMax) );
            
            for( i = 0; i < nPoints; i++ )
            {
                memcpy( psShape->padfM + i,
                        psSHP->pabyRec + nOffset + 16 + i*8, 8 );
                if( bBigEndian ) SwapWord( 8, psShape->padfM + i );
            }
        }
        
    }

// ==================================================================== //
//  Extract vertices for a MultiPoint.					//
// ==================================================================== //
    else if( psShape->nSHPType == SHPT_MULTIPOINT
             || psShape->nSHPType == SHPT_MULTIPOINTM
             || psShape->nSHPType == SHPT_MULTIPOINTZ )
    {
	int32		nPoints;
	int    		i, nOffset;

	memcpy( &nPoints, psSHP->pabyRec + 44, 4 );
	if( bBigEndian ) SwapWord( 4, &nPoints );

	psShape->nVertices = nPoints;
        psShape->padfX = (double *) calloc(nPoints,sizeof(double));
        psShape->padfY = (double *) calloc(nPoints,sizeof(double));
        psShape->padfZ = (double *) calloc(nPoints,sizeof(double));
        psShape->padfM = (double *) calloc(nPoints,sizeof(double));

	for( i = 0; i < nPoints; i++ )
	{
	    memcpy(psShape->padfX+i, psSHP->pabyRec + 48 + 16 * i, 8 );
	    memcpy(psShape->padfY+i, psSHP->pabyRec + 48 + 16 * i + 8, 8 );

	    if( bBigEndian ) SwapWord( 8, psShape->padfX + i );
	    if( bBigEndian ) SwapWord( 8, psShape->padfY + i );
	}

        nOffset = 48 + 16*nPoints;
        
// -------------------------------------------------------------------- //
//	Get the X/Y bounds.						//
// -------------------------------------------------------------------- //
        memcpy( &(psShape->dfXMin), psSHP->pabyRec + 8 +  4, 8 );
        memcpy( &(psShape->dfYMin), psSHP->pabyRec + 8 + 12, 8 );
        memcpy( &(psShape->dfXMax), psSHP->pabyRec + 8 + 20, 8 );
        memcpy( &(psShape->dfYMax), psSHP->pabyRec + 8 + 28, 8 );

	if( bBigEndian ) SwapWord( 8, &(psShape->dfXMin) );
	if( bBigEndian ) SwapWord( 8, &(psShape->dfYMin) );
	if( bBigEndian ) SwapWord( 8, &(psShape->dfXMax) );
	if( bBigEndian ) SwapWord( 8, &(psShape->dfYMax) );

// -------------------------------------------------------------------- //
//      If we have a Z coordinate, collect that now.                    //
// -------------------------------------------------------------------- //
        if( psShape->nSHPType == SHPT_MULTIPOINTZ )
        {
            memcpy( &(psShape->dfZMin), psSHP->pabyRec + nOffset, 8 );
            memcpy( &(psShape->dfZMax), psSHP->pabyRec + nOffset + 8, 8 );
            
            if( bBigEndian ) SwapWord( 8, &(psShape->dfZMin) );
            if( bBigEndian ) SwapWord( 8, &(psShape->dfZMax) );
            
            for( i = 0; i < nPoints; i++ )
            {
                memcpy( psShape->padfZ + i,
                        psSHP->pabyRec + nOffset + 16 + i*8, 8 );
                if( bBigEndian ) SwapWord( 8, psShape->padfZ + i );
            }

            nOffset += 16 + 8*nPoints;
        }

// -------------------------------------------------------------------- //
//      If we have a M measure value, then read it now.  We assume      //
//      that the measure can be present for any shape if the size is    //
//      big enough, but really it will only occur for the Z shapes      //
//      (options), and the M shapes.                                    //
// -------------------------------------------------------------------- //
        if( psSHP->panRecSize[hEntity]+8 >= nOffset + 16 + 8*nPoints )
        {
            memcpy( &(psShape->dfMMin), psSHP->pabyRec + nOffset, 8 );
            memcpy( &(psShape->dfMMax), psSHP->pabyRec + nOffset + 8, 8 );
            
            if( bBigEndian ) SwapWord( 8, &(psShape->dfMMin) );
            if( bBigEndian ) SwapWord( 8, &(psShape->dfMMax) );
            
            for( i = 0; i < nPoints; i++ )
            {
                memcpy( psShape->padfM + i,
                        psSHP->pabyRec + nOffset + 16 + i*8, 8 );
                if( bBigEndian ) SwapWord( 8, psShape->padfM + i );
            }
        }
    }

// ==================================================================== //
//      Extract vertices for a point.                                   //
// ==================================================================== //
    else if( psShape->nSHPType == SHPT_POINT
             || psShape->nSHPType == SHPT_POINTM
             || psShape->nSHPType == SHPT_POINTZ )
    {
        int	nOffset;
        
	psShape->nVertices = 1;
        psShape->padfX = (double *) calloc(1,sizeof(double));
        psShape->padfY = (double *) calloc(1,sizeof(double));
        psShape->padfZ = (double *) calloc(1,sizeof(double));
        psShape->padfM = (double *) calloc(1,sizeof(double));

	memcpy( psShape->padfX, psSHP->pabyRec + 12, 8 );
	memcpy( psShape->padfY, psSHP->pabyRec + 20, 8 );

	if( bBigEndian ) SwapWord( 8, psShape->padfX );
	if( bBigEndian ) SwapWord( 8, psShape->padfY );

        nOffset = 20 + 8;
        
// -------------------------------------------------------------------- //
//      If we have a Z coordinate, collect that now.                    //
// -------------------------------------------------------------------- //
        if( psShape->nSHPType == SHPT_POINTZ )
        {
            memcpy( psShape->padfZ, psSHP->pabyRec + nOffset, 8 );
        
            if( bBigEndian ) SwapWord( 8, psShape->padfZ );
            
            nOffset += 8;
        }

// -------------------------------------------------------------------- //
//      If we have a M measure value, then read it now.  We assume      //
//      that the measure can be present for any shape if the size is    //
//      big enough, but really it will only occur for the Z shapes      //
//      (options), and the M shapes.                                    //
// -------------------------------------------------------------------- //
        if( psSHP->panRecSize[hEntity]+8 >= nOffset + 8 )
        {
            memcpy( psShape->padfM, psSHP->pabyRec + nOffset, 8 );
        
            if( bBigEndian ) SwapWord( 8, psShape->padfM );
        }

// -------------------------------------------------------------------- //
//      Since no extents are supplied in the record, we will apply      //
//      them from the single vertex.                                    //
// -------------------------------------------------------------------- //
        psShape->dfXMin = psShape->dfXMax = psShape->padfX[0];
        psShape->dfYMin = psShape->dfYMax = psShape->padfY[0];
        psShape->dfZMin = psShape->dfZMax = psShape->padfZ[0];
        psShape->dfMMin = psShape->dfMMax = psShape->padfM[0];
    }

    return( psShape );
}

//////////////////////////////////////////////////////////////////////////
//                            SHPTypeName()                             //
//////////////////////////////////////////////////////////////////////////

const char SHPAPI_CALL1(*)
CSHP::SHPTypeName( int nSHPType )

{
    switch( nSHPType )
    {
      case SHPT_NULL:
        return "NullShape";

      case SHPT_POINT:
        return "Point";

      case SHPT_ARC:
        return "Arc";

      case SHPT_POLYGON:
        return "Polygon";

      case SHPT_MULTIPOINT:
        return "MultiPoint";
        
      case SHPT_POINTZ:
        return "PointZ";

      case SHPT_ARCZ:
        return "ArcZ";

      case SHPT_POLYGONZ:
        return "PolygonZ";

      case SHPT_MULTIPOINTZ:
        return "MultiPointZ";
        
      case SHPT_POINTM:
        return "PointM";

      case SHPT_ARCM:
        return "ArcM";

      case SHPT_POLYGONM:
        return "PolygonM";

      case SHPT_MULTIPOINTM:
        return "MultiPointM";

      case SHPT_MULTIPATCH:
        return "MultiPatch";

      default:
        return "UnknownShapeType";
    }
}

//////////////////////////////////////////////////////////////////////////
//                          SHPPartTypeName()                           //
//////////////////////////////////////////////////////////////////////////

const char SHPAPI_CALL1(*)
CSHP::SHPPartTypeName( int nPartType )

{
    switch( nPartType )
    {
      case SHPP_TRISTRIP:
        return "TriangleStrip";
        
      case SHPP_TRIFAN:
        return "TriangleFan";

      case SHPP_OUTERRING:
        return "OuterRing";

      case SHPP_INNERRING:
        return "InnerRing";

      case SHPP_FIRSTRING:
        return "FirstRing";

      case SHPP_RING:
        return "Ring";

      default:
        return "UnknownPartType";
    }
}

//////////////////////////////////////////////////////////////////////////
//                          SHPDestroyObject()                          //
//////////////////////////////////////////////////////////////////////////

void SHPAPI_CALL
CSHP::SHPDestroyObject( SHPObject * psShape )

{
    if( psShape == NULL )
        return;
    
    if( psShape->padfX != NULL )
        free( psShape->padfX );
    if( psShape->padfY != NULL )
        free( psShape->padfY );
    if( psShape->padfZ != NULL )
        free( psShape->padfZ );
    if( psShape->padfM != NULL )
        free( psShape->padfM );

    if( psShape->panPartStart != NULL )
        free( psShape->panPartStart );
    if( psShape->panPartType != NULL )
        free( psShape->panPartType );

    free( psShape );
}

//////////////////////////////////////////////////////////////////////////
//                          SHPRewindObject()                           //
//                                                                      //
//      Reset the winding of polygon objects to adhere to the           //
//      specification.                                                  //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::SHPRewindObject( SHPHandle hSHP, SHPObject * psObject )

{
    int  iOpRing, bAltered = 0;

// -------------------------------------------------------------------- //
//      Do nothing if this is not a polygon object.                     //
// -------------------------------------------------------------------- //
    if( psObject->nSHPType != SHPT_POLYGON
        && psObject->nSHPType != SHPT_POLYGONZ
        && psObject->nSHPType != SHPT_POLYGONM )
        return 0;

// -------------------------------------------------------------------- //
//      Process each of the rings.                                      //
// -------------------------------------------------------------------- //
    for( iOpRing = 0; iOpRing < psObject->nParts; iOpRing++ )
    {
        int      bInner, iVert, nVertCount, nVertStart, iCheckRing;
        double   dfSum, dfTestX, dfTestY;

// -------------------------------------------------------------------- //
//      Determine if this ring is an inner ring or an outer ring        //
//      relative to all the other rings.  For now we assume the         //
//      first ring is outer and all others are inner, but eventually    //
//      we need to fix this to handle multiple island polygons and      //
//      unordered sets of rings.                                        //
// -------------------------------------------------------------------- //
        dfTestX = psObject->padfX[psObject->panPartStart[iOpRing]];
        dfTestY = psObject->padfY[psObject->panPartStart[iOpRing]];

        bInner = FALSE;
        for( iCheckRing = 0; iCheckRing < psObject->nParts; iCheckRing++ )
        {
            int iEdge;

            if( iCheckRing == iOpRing )
                continue;
            
            nVertStart = psObject->panPartStart[iCheckRing];

            if( iCheckRing == psObject->nParts-1 )
                nVertCount = psObject->nVertices 
                    - psObject->panPartStart[iCheckRing];
            else
                nVertCount = psObject->panPartStart[iCheckRing+1] 
                    - psObject->panPartStart[iCheckRing];

            for( iEdge = 0; iEdge < nVertCount; iEdge++ )
            {
                int iNext;

                if( iEdge < nVertCount-1 )
                    iNext = iEdge+1;
                else
                    iNext = 0;

                if( (psObject->padfY[iEdge+nVertStart] < dfTestY 
                     && psObject->padfY[iNext+nVertStart] >= dfTestY)
                    || (psObject->padfY[iNext+nVertStart] < dfTestY 
                        && psObject->padfY[iEdge+nVertStart] >= dfTestY) )
                {
                    if( psObject->padfX[iEdge+nVertStart] 
                        + (dfTestY - psObject->padfY[iEdge+nVertStart])
                           / (psObject->padfY[iNext+nVertStart]
                              - psObject->padfY[iEdge+nVertStart])
                           * (psObject->padfX[iNext+nVertStart]
                              - psObject->padfX[iEdge+nVertStart]) < dfTestX )
                        bInner = !bInner;
                }
            }
        }

// -------------------------------------------------------------------- //
//      Determine the current order of this ring so we will know if     //
//      it has to be reversed.                                          //
// -------------------------------------------------------------------- //
        nVertStart = psObject->panPartStart[iOpRing];

        if( iOpRing == psObject->nParts-1 )
            nVertCount = psObject->nVertices - psObject->panPartStart[iOpRing];
        else
            nVertCount = psObject->panPartStart[iOpRing+1] 
                - psObject->panPartStart[iOpRing];

        dfSum = 0.0;
        for( iVert = nVertStart; iVert < nVertStart+nVertCount-1; iVert++ )
        {
            dfSum += psObject->padfX[iVert] * psObject->padfY[iVert+1]
                - psObject->padfY[iVert] * psObject->padfX[iVert+1];
        }

        dfSum += psObject->padfX[iVert] * psObject->padfY[nVertStart]
               - psObject->padfY[iVert] * psObject->padfX[nVertStart];

// -------------------------------------------------------------------- //
//      Reverse if necessary.                                           //
// -------------------------------------------------------------------- //
        if( (dfSum < 0.0 && bInner) || (dfSum > 0.0 && !bInner) )
        {
            int   i;

            bAltered++;
            for( i = 0; i < nVertCount/2; i++ )
            {
                double dfSaved;

                // Swap X //
                dfSaved = psObject->padfX[nVertStart+i];
                psObject->padfX[nVertStart+i] = 
                    psObject->padfX[nVertStart+nVertCount-i-1];
                psObject->padfX[nVertStart+nVertCount-i-1] = dfSaved;

                // Swap Y //
                dfSaved = psObject->padfY[nVertStart+i];
                psObject->padfY[nVertStart+i] = 
                    psObject->padfY[nVertStart+nVertCount-i-1];
                psObject->padfY[nVertStart+nVertCount-i-1] = dfSaved;

                // Swap Z //
                if( psObject->padfZ )
                {
                    dfSaved = psObject->padfZ[nVertStart+i];
                    psObject->padfZ[nVertStart+i] = 
                        psObject->padfZ[nVertStart+nVertCount-i-1];
                    psObject->padfZ[nVertStart+nVertCount-i-1] = dfSaved;
                }

                // Swap M //
                if( psObject->padfM )
                {
                    dfSaved = psObject->padfM[nVertStart+i];
                    psObject->padfM[nVertStart+i] = 
                        psObject->padfM[nVertStart+nVertCount-i-1];
                    psObject->padfM[nVertStart+nVertCount-i-1] = dfSaved;
                }
            }
        }
    }

    return bAltered;
}

/////////////////////////////////////////////////////////////////////
//
// $Id: dbfopen.c,v 1.48 2003/03/10 14:51:27 warmerda Exp $
//
// Project:  Shapelib
// Purpose:  Implementation of .dbf access API documented in dbf_api.html.
// Author:   Frank Warmerdam, warmerdam@pobox.com
//
////////////////////////////////////////////////////////////////////
#include <ctype.h>

static int	nStringFieldLen = 0;
static char * pszStringField = NULL;

//////////////////////////////////////////////////////////////////////////
//                           DBFWriteHeader()                           //
//                                                                      //
//      This is called to write out the file header, and field          //
//      descriptions before writing any actual data records.  This      //
//      also computes all the DBFDataSet field offset/size/decimals     //
//      and so forth values.                                            //
//////////////////////////////////////////////////////////////////////////

static void DBFWriteHeader(DBFHandle psDBF)

{
    unsigned char	abyHeader[XBASE_FLDHDR_SZ];
    int		i;

    if( !psDBF->bNoHeader )
        return;

    psDBF->bNoHeader = FALSE;

// -------------------------------------------------------------------- //
//	Initialize the file header information.				//
// -------------------------------------------------------------------- //
    for( i = 0; i < XBASE_FLDHDR_SZ; i++ )
        abyHeader[i] = 0;

    abyHeader[0] = 0x03;		// memo field? - just copying 	//

    // date updated on close, record count preset at zero //

    abyHeader[8] = psDBF->nHeaderLength % 256;
    abyHeader[9] = psDBF->nHeaderLength / 256;
    
    abyHeader[10] = psDBF->nRecordLength % 256;
    abyHeader[11] = psDBF->nRecordLength / 256;

// -------------------------------------------------------------------- //
//      Write the initial 32 byte file header, and all the field        //
//      descriptions.                                     		//
// -------------------------------------------------------------------- //
    fseek( psDBF->fp, 0, 0 );
    fwrite( abyHeader, XBASE_FLDHDR_SZ, 1, psDBF->fp );
    fwrite( psDBF->pszHeader, XBASE_FLDHDR_SZ, psDBF->nFields, psDBF->fp );

// -------------------------------------------------------------------- //
//      Write out the newline character if there is room for it.        //
// -------------------------------------------------------------------- //
    if( psDBF->nHeaderLength > 32*psDBF->nFields + 32 )
    {
        char	cNewline;

        cNewline = 0x0d;
        fwrite( &cNewline, 1, 1, psDBF->fp );
    }
}

//////////////////////////////////////////////////////////////////////////
//                           DBFFlushRecord()                           //
//                                                                      //
//      Write out the current record if there is one.                   //
//////////////////////////////////////////////////////////////////////////

static void DBFFlushRecord( DBFHandle psDBF )

{
    int		nRecordOffset;

    if( psDBF->bCurrentRecordModified && psDBF->nCurrentRecord > -1 )
    {
	psDBF->bCurrentRecordModified = FALSE;

	nRecordOffset = psDBF->nRecordLength * psDBF->nCurrentRecord 
	                                             + psDBF->nHeaderLength;

	fseek( psDBF->fp, nRecordOffset, 0 );
	fwrite( psDBF->pszCurrentRecord, psDBF->nRecordLength, 1, psDBF->fp );
    }
}

//////////////////////////////////////////////////////////////////////////
//                              DBFOpen()                               //
//                                                                      //
//      Open a .dbf file.                                               //
//////////////////////////////////////////////////////////////////////////
   
/////////////////////////////////////////////////////////////////////
//	  函数名:	CSHP::DBFOpen()
//	  功  能:	打开dbf文件,读取文件头信息和记录的相关信息
//	  原  理:	
//	  输  入:	(const char * pszFilename,          //文件名,包含路径
//               const char * pszAccess)            //文件打开方式            
//	  输  出:	无
//	  返回值:   
//    赋  值:   无   //
//	  注  意:   
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
DBFHandle SHPAPI_CALL
CSHP::DBFOpen( const char * pszFilename, const char * pszAccess )

{
    DBFHandle		psDBF;
    unsigned char		*pabyBuf;
    int			nFields, nHeadLen, nRecLen, iField, i;
    char		*pszBasename, *pszFullname;

// -------------------------------------------------------------------- //
//      We only allow the access strings "rb" and "r+".                  //
// -------------------------------------------------------------------- //
    if( strcmp(pszAccess,"r") != 0 && strcmp(pszAccess,"r+") != 0 
        && strcmp(pszAccess,"rb") != 0 && strcmp(pszAccess,"rb+") != 0
        && strcmp(pszAccess,"r+b") != 0 )
        return( NULL );

    if( strcmp(pszAccess,"r") == 0 )
        pszAccess = "rb";
 
    if( strcmp(pszAccess,"r+") == 0 )
        pszAccess = "rb+";

// -------------------------------------------------------------------- //
//	Compute the base (layer) name.  If there is any extension	//
//	on the passed in filename we will strip it off.			//
// -------------------------------------------------------------------- //
    pszBasename = (char *) malloc(strlen(pszFilename)+5);
    strcpy( pszBasename, pszFilename );
    for( i = strlen(pszBasename)-1; 
	 i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
	       && pszBasename[i] != '\\';
	 i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf( pszFullname, "%s.dbf", pszBasename );
        
    psDBF = (DBFHandle) calloc( 1, sizeof(DBFInfo) );
    psDBF->fp = fopen( pszFullname, pszAccess );

    if( psDBF->fp == NULL )
    {
        sprintf( pszFullname, "%s.DBF", pszBasename );
        psDBF->fp = fopen(pszFullname, pszAccess );
    }
    
    free( pszBasename );
    free( pszFullname );
    
    if( psDBF->fp == NULL )
    {
        free( psDBF );
        return( NULL );
    }

    psDBF->bNoHeader = FALSE;
    psDBF->nCurrentRecord = -1;
    psDBF->bCurrentRecordModified = FALSE;

// -------------------------------------------------------------------- //
//  Read Table Header info                                              //
// -------------------------------------------------------------------- //
    pabyBuf = (unsigned char *) malloc(500);
    if( fread( pabyBuf, 32, 1, psDBF->fp ) != 1 )
    {
        fclose( psDBF->fp );
        free( pabyBuf );
        free( psDBF );
        return NULL;
    }

    psDBF->nRecords = 
     pabyBuf[4] + pabyBuf[5]*256 + pabyBuf[6]*256*256 + pabyBuf[7]*256*256*256;

    psDBF->nHeaderLength = nHeadLen = pabyBuf[8] + pabyBuf[9]*256;
    psDBF->nRecordLength = nRecLen = pabyBuf[10] + pabyBuf[11]*256;
    
    psDBF->nFields = nFields = (nHeadLen - 32) / 32;

    psDBF->pszCurrentRecord = (char *) malloc(nRecLen);

// -------------------------------------------------------------------- //
//  Read in Field Definitions                                           //
// -------------------------------------------------------------------- //
    
    pabyBuf = (unsigned char *) SfRealloc(pabyBuf,nHeadLen);
    psDBF->pszHeader = (char *) pabyBuf;

    fseek( psDBF->fp, 32, 0 );
    if( fread( pabyBuf, nHeadLen-32, 1, psDBF->fp ) != 1 )
    {
        fclose( psDBF->fp );
        free( pabyBuf );
        free( psDBF );
        return NULL;
    }

    psDBF->panFieldOffset = (int *) malloc(sizeof(int) * nFields);
    psDBF->panFieldSize = (int *) malloc(sizeof(int) * nFields);
    psDBF->panFieldDecimals = (int *) malloc(sizeof(int) * nFields);
    psDBF->pachFieldType = (char *) malloc(sizeof(char) * nFields);

    for( iField = 0; iField < nFields; iField++ )
    {
		unsigned char		*pabyFInfo;
		
		pabyFInfo = pabyBuf+iField*32;
		
		if( pabyFInfo[11] == 'N' || pabyFInfo[11] == 'F' )
		{
			psDBF->panFieldSize[iField] = pabyFInfo[16];
			psDBF->panFieldDecimals[iField] = pabyFInfo[17];
		}
		else
		{
			psDBF->panFieldSize[iField] = pabyFInfo[16] + pabyFInfo[17]*256;
			psDBF->panFieldDecimals[iField] = 0;
		}
		
		psDBF->pachFieldType[iField] = (char) pabyFInfo[11];
		if( iField == 0 )
		{
			psDBF->panFieldOffset[iField] = 1;
		}
		else
		{
			psDBF->panFieldOffset[iField] = 
			psDBF->panFieldOffset[iField-1] + psDBF->panFieldSize[iField-1];
		}
    }

    return( psDBF );
}

//////////////////////////////////////////////////////////////////////////
//                              DBFClose()                              //
//////////////////////////////////////////////////////////////////////////

void SHPAPI_CALL
CSHP::DBFClose(DBFHandle psDBF)
{
// -------------------------------------------------------------------- //
//      Write out header if not already written.                        //
// -------------------------------------------------------------------- //
    if( psDBF->bNoHeader )
        DBFWriteHeader( psDBF );

    DBFFlushRecord( psDBF );

// -------------------------------------------------------------------- //
//      Update last access date, and number of records if we have	//
//	write access.                					//
// -------------------------------------------------------------------- //
    if( psDBF->bUpdated )
    {
	unsigned char		abyFileHeader[32];

	fseek( psDBF->fp, 0, 0 );
	fread( abyFileHeader, 32, 1, psDBF->fp );

	abyFileHeader[1] = 95;			// YY //
	abyFileHeader[2] = 7;			// MM //
	abyFileHeader[3] = 26;			// DD //

	abyFileHeader[4] = psDBF->nRecords % 256;
	abyFileHeader[5] = (psDBF->nRecords/256) % 256;
	abyFileHeader[6] = (psDBF->nRecords/(256*256)) % 256;
	abyFileHeader[7] = (psDBF->nRecords/(256*256*256)) % 256;

	fseek( psDBF->fp, 0, 0 );
	fwrite( abyFileHeader, 32, 1, psDBF->fp );
    }

// -------------------------------------------------------------------- //
//      Close, and free resources.                                      //
// -------------------------------------------------------------------- //
    fclose( psDBF->fp );

    if( psDBF->panFieldOffset != NULL )
    {
        free( psDBF->panFieldOffset );
        free( psDBF->panFieldSize );
        free( psDBF->panFieldDecimals );
        free( psDBF->pachFieldType );
    }

    free( psDBF->pszHeader );
    free( psDBF->pszCurrentRecord );

    free( psDBF );

    if( pszStringField != NULL )
    {
        free( pszStringField );
        pszStringField = NULL;
        nStringFieldLen = 0;
    }
}

//////////////////////////////////////////////////////////////////////////
//                             DBFCreate()                              //
//                                                                      //
//      Create a new .dbf file.                                         //
//////////////////////////////////////////////////////////////////////////

DBFHandle SHPAPI_CALL
CSHP::DBFCreate( const char * pszFilename )

{
    DBFHandle	psDBF;
    FILE	*fp;
    char	*pszFullname, *pszBasename;
    int		i;

// -------------------------------------------------------------------- //
//	Compute the base (layer) name.  If there is any extension	//
//	on the passed in filename we will strip it off.			//
// -------------------------------------------------------------------- //
    pszBasename = (char *) malloc(strlen(pszFilename)+5);
    strcpy( pszBasename, pszFilename );
    for( i = strlen(pszBasename)-1; 
	 i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
	       && pszBasename[i] != '\\';
	 i-- ) {}

    if( pszBasename[i] == '.' )
        pszBasename[i] = '\0';

    pszFullname = (char *) malloc(strlen(pszBasename) + 5);
    sprintf( pszFullname, "%s.dbf", pszBasename );
    free( pszBasename );

// -------------------------------------------------------------------- //
//      Create the file.                                                //
// -------------------------------------------------------------------- //
    fp = fopen( pszFullname, "wb" );
    if( fp == NULL )
        return( NULL );

    fputc( 0, fp );
    fclose( fp );

    fp = fopen( pszFullname, "rb+" );
    if( fp == NULL )
        return( NULL );

    free( pszFullname );

// -------------------------------------------------------------------- //
//	Create the info structure.					//
// -------------------------------------------------------------------- //
    psDBF = (DBFHandle) malloc(sizeof(DBFInfo));

    psDBF->fp = fp;
    psDBF->nRecords = 0;
    psDBF->nFields = 0;
    psDBF->nRecordLength = 1;
    psDBF->nHeaderLength = 33;
    
    psDBF->panFieldOffset = NULL;
    psDBF->panFieldSize = NULL;
    psDBF->panFieldDecimals = NULL;
    psDBF->pachFieldType = NULL;
    psDBF->pszHeader = NULL;

    psDBF->nCurrentRecord = -1;
    psDBF->bCurrentRecordModified = FALSE;
    psDBF->pszCurrentRecord = NULL;

    psDBF->bNoHeader = TRUE;

    return( psDBF );
}

//////////////////////////////////////////////////////////////////////////
//                            DBFAddField()                             //
//                                                                      //
//      Add a field to a newly created .dbf file before any records     //
//      are written.                                                    //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFAddField(DBFHandle psDBF, const char * pszFieldName, 
            DBFFieldType eType, int nWidth, int nDecimals )

{
    char	*pszFInfo;
    int		i;

// -------------------------------------------------------------------- //
//      Do some checking to ensure we can add records to this file.     //
// -------------------------------------------------------------------- //
    if( psDBF->nRecords > 0 )
        return( -1 );

    if( !psDBF->bNoHeader )
        return( -1 );

    if( eType != FTDouble && nDecimals != 0 )
        return( -1 );

    if( nWidth < 1 )
        return -1;

// -------------------------------------------------------------------- //
//      SfRealloc all the arrays larger to hold the additional field      //
//      information.                                                    //
// -------------------------------------------------------------------- //
    psDBF->nFields++;

    psDBF->panFieldOffset = (int *) 
      SfRealloc( psDBF->panFieldOffset, sizeof(int) * psDBF->nFields );

    psDBF->panFieldSize = (int *) 
      SfRealloc( psDBF->panFieldSize, sizeof(int) * psDBF->nFields );

    psDBF->panFieldDecimals = (int *) 
      SfRealloc( psDBF->panFieldDecimals, sizeof(int) * psDBF->nFields );

    psDBF->pachFieldType = (char *) 
      SfRealloc( psDBF->pachFieldType, sizeof(char) * psDBF->nFields );

// -------------------------------------------------------------------- //
//      Assign the new field information fields.                        //
// -------------------------------------------------------------------- //
    psDBF->panFieldOffset[psDBF->nFields-1] = psDBF->nRecordLength;
    psDBF->nRecordLength += nWidth;
    psDBF->panFieldSize[psDBF->nFields-1] = nWidth;
    psDBF->panFieldDecimals[psDBF->nFields-1] = nDecimals;

    if( eType == FTLogical )
        psDBF->pachFieldType[psDBF->nFields-1] = 'L';
    else if( eType == FTString )
        psDBF->pachFieldType[psDBF->nFields-1] = 'C';
    else
        psDBF->pachFieldType[psDBF->nFields-1] = 'N';

// -------------------------------------------------------------------- //
//      Extend the required header information.                         //
// -------------------------------------------------------------------- //
    psDBF->nHeaderLength += 32;
    psDBF->bUpdated = FALSE;

    psDBF->pszHeader = (char *) SfRealloc(psDBF->pszHeader,psDBF->nFields*32);

    pszFInfo = psDBF->pszHeader + 32 * (psDBF->nFields-1);

    for( i = 0; i < 32; i++ )
        pszFInfo[i] = '\0';

    if( (int) strlen(pszFieldName) < 10 )
        strncpy( pszFInfo, pszFieldName, strlen(pszFieldName));
    else
        strncpy( pszFInfo, pszFieldName, 10);

    pszFInfo[11] = psDBF->pachFieldType[psDBF->nFields-1];

    if( eType == FTString )
    {
        pszFInfo[16] = nWidth % 256;
        pszFInfo[17] = nWidth / 256;
    }
    else
    {
        pszFInfo[16] = nWidth;
        pszFInfo[17] = nDecimals;
    }
    
// -------------------------------------------------------------------- //
//      Make the current record buffer appropriately larger.            //
// -------------------------------------------------------------------- //
    psDBF->pszCurrentRecord = (char *) SfRealloc(psDBF->pszCurrentRecord,
					       psDBF->nRecordLength);

    return( psDBF->nFields-1 );
}

//////////////////////////////////////////////////////////////////////////
//                          DBFReadAttribute()                          //
//                                                                      //
//      Read one of the attribute fields of a record.                   //
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//	  函数名:	static void *DBFReadAttribute()
//	  功  能:	读取某个实体某个字段的存储信息(属性信息)
//	  原  理:	
//	  输  入:	(DBFHandle psDBF,         //DBF文件指针
//               int hEntity,             //实体ID
//               int iField,              //字段ID
//               char chReqType )         //读取类型   
//	  输  出:	无
//	  返回值:   
//    赋  值:   无   //
//	  注  意:   
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
static void *DBFReadAttribute(DBFHandle psDBF, int hEntity, int iField,
                              char chReqType )

{
    int	       	nRecordOffset;
    unsigned char	*pabyRec;
    void	*pReturnField = NULL;

    static double dDoubleField;

// -------------------------------------------------------------------- //
//      Verify selection.                                               //
// -------------------------------------------------------------------- //
    if( hEntity < 0 || hEntity >= psDBF->nRecords )
        return( NULL );

    if( iField < 0 || iField >= psDBF->nFields )
        return( NULL );

// -------------------------------------------------------------------- //
//	Have we read the record?					//
// -------------------------------------------------------------------- //
    if( psDBF->nCurrentRecord != hEntity )
    {
	DBFFlushRecord( psDBF );

	nRecordOffset = psDBF->nRecordLength * hEntity + psDBF->nHeaderLength;

	if( fseek( psDBF->fp, nRecordOffset, 0 ) != 0 )
        {
            fprintf( stderr, "fseek(%d) failed on DBF file.\n",
                     nRecordOffset );
            return NULL;
        }

	if( fread( psDBF->pszCurrentRecord, psDBF->nRecordLength, 
                   1, psDBF->fp ) != 1 )
        {
            fprintf( stderr, "fread(%d) failed on DBF file.\n",
                     psDBF->nRecordLength );
            return NULL;
        }

	psDBF->nCurrentRecord = hEntity;
    }

    pabyRec = (unsigned char *) psDBF->pszCurrentRecord;

// -------------------------------------------------------------------- //
//	Ensure our field buffer is large enough to hold this buffer.	//
// -------------------------------------------------------------------- //
    if( psDBF->panFieldSize[iField]+1 > nStringFieldLen )
    {
	nStringFieldLen = psDBF->panFieldSize[iField]*2 + 10;
	pszStringField = (char *) SfRealloc(pszStringField,nStringFieldLen);
    }

// -------------------------------------------------------------------- //
//	Extract the requested field.					//
// -------------------------------------------------------------------- //
    strncpy( pszStringField, 
	     ((const char *) pabyRec) + psDBF->panFieldOffset[iField],
	     psDBF->panFieldSize[iField] );
    pszStringField[psDBF->panFieldSize[iField]] = '\0';

    pReturnField = pszStringField;

// -------------------------------------------------------------------- //
//      Decode the field.                                               //
// -------------------------------------------------------------------- //
    if( chReqType == 'N' )
    {
        dDoubleField = atof(pszStringField);

	pReturnField = &dDoubleField;
    }

// -------------------------------------------------------------------- //
//      Should we trim white space off the string attribute value?      //
// -------------------------------------------------------------------- //
#ifdef TRIM_DBF_WHITESPACE
    else
    {
        char	*pchSrc, *pchDst;

        pchDst = pchSrc = pszStringField;
        while( *pchSrc == ' ' )
            pchSrc++;

        while( *pchSrc != '\0' )
            *(pchDst++) = *(pchSrc++);
        *pchDst = '\0';

        while( pchDst != pszStringField && *(--pchDst) == ' ' )
            *pchDst = '\0';
    }
#endif
    
    return( pReturnField );
}

//////////////////////////////////////////////////////////////////////////
//                        DBFReadIntAttribute()                         //
//                                                                      //
//      Read an integer attribute.                                      //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	int SHPAPI_CALL CSHP::DBFReadIntegerAttribute()
//	  功  能:	读取某个记录某个字段的整型信息(整型属性信息)
//	  原  理:	
//	  输  入:	(DBFHandle psDBF,         //DBF文件指针
//               int iRecord,             //记录ID
//               int iField)              //字段ID
//	  输  出:	无
//	  返回值:   
//    赋  值:   无   //
//	  注  意:   调用函数DBFReadAttribute()
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
int SHPAPI_CALL
CSHP::DBFReadIntegerAttribute( DBFHandle psDBF, int iRecord, int iField )

{
    double	*pdValue;

    pdValue = (double *) DBFReadAttribute( psDBF, iRecord, iField, 'N' );

    if( pdValue == NULL )
        return 0;
    else
        return( (int) *pdValue );
}

//////////////////////////////////////////////////////////////////////////
//                        DBFReadDoubleAttribute()                      //
//                                                                      //
//      Read a double attribute.                                        //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	int SHPAPI_CALL CSHP::DBFReadDoubleAttribute()
//	  功  能:	读取某个记录某个字段的双精度信息(双精度属性信息)
//	  原  理:	
//	  输  入:	(DBFHandle psDBF,         //DBF文件指针
//               int iRecord,             //记录ID
//               int iField)              //字段ID
//	  输  出:	无
//	  返回值:   
//    赋  值:   无   //
//	  注  意:   调用函数DBFReadAttribute()
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
double SHPAPI_CALL
CSHP::DBFReadDoubleAttribute( DBFHandle psDBF, int iRecord, int iField )

{
    double	*pdValue;

    pdValue = (double *) DBFReadAttribute( psDBF, iRecord, iField, 'N' );

    if( pdValue == NULL )
        return 0.0;
    else
        return( *pdValue );
}

//////////////////////////////////////////////////////////////////////////
//                        DBFReadStringAttribute()                      //
//                                                                      //
//      Read a string attribute.                                        //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	int SHPAPI_CALL CSHP::DBFReadStringAttribute()
//	  功  能:	读取某个记录某个字段的字符串信息(字符串属性信息)
//	  原  理:	
//	  输  入:	(DBFHandle psDBF,         //DBF文件指针
//               int iRecord,             //记录ID
//               int iField)              //字段ID
//	  输  出:	无
//	  返回值:   
//    赋  值:   无   //
//	  注  意:   调用函数DBFReadAttribute()
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
const char SHPAPI_CALL1(*)
CSHP::DBFReadStringAttribute( DBFHandle psDBF, int iRecord, int iField )

{
    return( (const char *) DBFReadAttribute( psDBF, iRecord, iField, 'C' ) );
}

//////////////////////////////////////////////////////////////////////////
//                        DBFReadLogicalAttribute()                     //
//                                                                      //
//      Read a logical attribute.                                       //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	int SHPAPI_CALL CSHP::DBFReadLogicalAttribute()
//	  功  能:	读取某个记录某个字段的逻辑型信息(逻辑型属性信息)
//	  原  理:	
//	  输  入:	(DBFHandle psDBF,         //DBF文件指针
//               int iRecord,             //记录ID
//               int iField)              //字段ID
//	  输  出:	无
//	  返回值:   
//    赋  值:   无   //
//	  注  意:   调用函数DBFReadAttribute()
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
const char SHPAPI_CALL1(*)
CSHP::DBFReadLogicalAttribute( DBFHandle psDBF, int iRecord, int iField )

{
    return( (const char *) DBFReadAttribute( psDBF, iRecord, iField, 'L' ) );
}

//////////////////////////////////////////////////////////////////////////
//                         DBFIsAttributeNULL()                         //
//                                                                      //
//      Return TRUE if value for field is NULL.                         //
//                                                                      //
//      Contributed by Jim Matthews.                                    //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFIsAttributeNULL( DBFHandle psDBF, int iRecord, int iField )

{
    const char	*pszValue;

    pszValue = DBFReadStringAttribute( psDBF, iRecord, iField );

    switch(psDBF->pachFieldType[iField])
    {
      case 'N':
      case 'F':
        // NULL numeric fields have value "****************" //
        return pszValue[0] == '*';

      case 'D':
        // NULL date fields have value "00000000" //
        return strncmp(pszValue,"00000000",8) == 0;

      case 'L':
        // NULL boolean fields have value "?" // 
        return pszValue[0] == '?';

      default:
        // empty string fields are considered NULL //
        return strlen(pszValue) == 0;
    }
}

//////////////////////////////////////////////////////////////////////////
//                          DBFGetFieldCount()                          //
//                                                                      //
//      Return the number of fields in this table.                      //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	int CSHP::DBFGetFieldCount()
//	  功  能:	获取字段的总个数
//	  原  理:	
//	  输  入:	(DBFHandle psDBF)                 //文件指针
//	  输  出:	无
//	  返回值:   int 字段的总个数
//    赋  值:   无
//	  注  意:   
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
int SHPAPI_CALL
CSHP::DBFGetFieldCount( DBFHandle psDBF )

{
    return( psDBF->nFields );
}

//////////////////////////////////////////////////////////////////////////
//                         DBFGetRecordCount()                          //
//                                                                      //
//      Return the number of records in this table.                     //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	int CSHP::DBFGetRecordCount()
//	  功  能:	获取记录的总个数
//	  原  理:	
//	  输  入:	(DBFHandle psDBF)                 //文件指针
//	  输  出:	无
//	  返回值:   int 记录的总个数
//    赋  值:   无
//	  注  意:   
//              调用实例
//    信  息:   DQY注释 2006.11
////////////////////////////////////////////////////////////////////
int SHPAPI_CALL
CSHP::DBFGetRecordCount( DBFHandle psDBF )

{
    return( psDBF->nRecords );
}

//////////////////////////////////////////////////////////////////////////
//                          DBFGetFieldInfo()                           //
//                                                                      //
//      Return any requested information about the field.               //
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//	  函数名:	DBFFieldType CSHP::DBFGetFieldInfo()
//	  功  能:	获取字段类型
//	  原  理:	
//	  输  入:	(DBFHandle psDBF,                 //文件指针
//               int iField,                      //字段ID,从0开始
//               char * pszFieldName,             //字段名
//               int * pnWidth,                   //字段宽度
//               int * pnDecimals)                //小数位            
//	  输  出:	无
//	  返回值:   无
//    赋  值:   无   //
//	  注  意:   
//              调用实例
//    信  息:   DQY 2006.11
////////////////////////////////////////////////////////////////////
DBFFieldType SHPAPI_CALL
CSHP::DBFGetFieldInfo( DBFHandle psDBF, int iField, char * pszFieldName,
                 int * pnWidth, int * pnDecimals )

{
    if( iField < 0 || iField >= psDBF->nFields )
        return( FTInvalid );

    if( pnWidth != NULL )
        *pnWidth = psDBF->panFieldSize[iField];

    if( pnDecimals != NULL )
        *pnDecimals = psDBF->panFieldDecimals[iField];

    if( pszFieldName != NULL )
    {
		int	i;
		
		strncpy( pszFieldName, (char *) psDBF->pszHeader+iField*32, 11 );
		pszFieldName[11] = '\0';
		for( i = 10; i > 0 && pszFieldName[i] == ' '; i-- )
			pszFieldName[i] = '\0';
    }

    if ( psDBF->pachFieldType[iField] == 'L' )
		return( FTLogical);
	
    else if( psDBF->pachFieldType[iField] == 'N' 
		|| psDBF->pachFieldType[iField] == 'F'
		|| psDBF->pachFieldType[iField] == 'D' )
    {
		if( psDBF->panFieldDecimals[iField] > 0 )
			return( FTDouble );
		else
			return( FTInteger );
    }
    else
    {
		return( FTString );
    }
}

//////////////////////////////////////////////////////////////////////////
//                         DBFWriteAttribute()                          //
//									//
//	Write an attribute record to the file.				//
//////////////////////////////////////////////////////////////////////////

static int DBFWriteAttribute(DBFHandle psDBF, int hEntity, int iField,
			     void * pValue )

{
    int	       	nRecordOffset, i, j, nRetResult = TRUE;
    unsigned char	*pabyRec;
    char	szSField[400], szFormat[20];

// -------------------------------------------------------------------- //
//	Is this a valid record?						//
// -------------------------------------------------------------------- //
    if( hEntity < 0 || hEntity > psDBF->nRecords )
        return( FALSE );

    if( psDBF->bNoHeader )
        DBFWriteHeader(psDBF);

// -------------------------------------------------------------------- //
//      Is this a brand new record?                                     //
// -------------------------------------------------------------------- //
    if( hEntity == psDBF->nRecords )
    {
	DBFFlushRecord( psDBF );

	psDBF->nRecords++;
	for( i = 0; i < psDBF->nRecordLength; i++ )
	    psDBF->pszCurrentRecord[i] = ' ';

	psDBF->nCurrentRecord = hEntity;
    }

// -------------------------------------------------------------------- //
//      Is this an existing record, but different than the last one     //
//      we accessed?                                                    //
// -------------------------------------------------------------------- //
    if( psDBF->nCurrentRecord != hEntity )
    {
	DBFFlushRecord( psDBF );

	nRecordOffset = psDBF->nRecordLength * hEntity + psDBF->nHeaderLength;

	fseek( psDBF->fp, nRecordOffset, 0 );
	fread( psDBF->pszCurrentRecord, psDBF->nRecordLength, 1, psDBF->fp );

	psDBF->nCurrentRecord = hEntity;
    }

    pabyRec = (unsigned char *) psDBF->pszCurrentRecord;

    psDBF->bCurrentRecordModified = TRUE;
    psDBF->bUpdated = TRUE;

// -------------------------------------------------------------------- //
//      Translate NULL value to valid DBF file representation.          //
//                                                                      //
//      Contributed by Jim Matthews.                                    //
// -------------------------------------------------------------------- //
    if( pValue == NULL )
    {
        switch(psDBF->pachFieldType[iField])
        {
          case 'N':
          case 'F':
	    // NULL numeric fields have value "****************" //
            memset( (char *) (pabyRec+psDBF->panFieldOffset[iField]), '*', 
                    psDBF->panFieldSize[iField] );
            break;

          case 'D':
	    // NULL date fields have value "00000000" //
            memset( (char *) (pabyRec+psDBF->panFieldOffset[iField]), '0', 
                    psDBF->panFieldSize[iField] );
            break;

          case 'L':
	    // NULL boolean fields have value "?" // 
            memset( (char *) (pabyRec+psDBF->panFieldOffset[iField]), '?', 
                    psDBF->panFieldSize[iField] );
            break;

          default:
            // empty string fields are considered NULL //
            memset( (char *) (pabyRec+psDBF->panFieldOffset[iField]), '\0', 
                    psDBF->panFieldSize[iField] );
            break;
        }
        return TRUE;
    }

// -------------------------------------------------------------------- //
//      Assign all the record fields.                                   //
// -------------------------------------------------------------------- //
    switch( psDBF->pachFieldType[iField] )
    {
      case 'D':
      case 'N':
      case 'F':
	if( psDBF->panFieldDecimals[iField] == 0 )
	{
            int		nWidth = psDBF->panFieldSize[iField];

            if( sizeof(szSField)-2 < nWidth )
                nWidth = sizeof(szSField)-2;

	    sprintf( szFormat, "%%%dd", nWidth );
	    sprintf(szSField, szFormat, (int) *((double *) pValue) );
	    if( (int)strlen(szSField) > psDBF->panFieldSize[iField] )
            {
	        szSField[psDBF->panFieldSize[iField]] = '\0';
                nRetResult = FALSE;
            }

	    strncpy((char *) (pabyRec+psDBF->panFieldOffset[iField]),
		    szSField, strlen(szSField) );
	}
	else
	{
            int		nWidth = psDBF->panFieldSize[iField];

            if( sizeof(szSField)-2 < nWidth )
                nWidth = sizeof(szSField)-2;

	    sprintf( szFormat, "%%%d.%df", 
                     nWidth, psDBF->panFieldDecimals[iField] );
	    sprintf(szSField, szFormat, *((double *) pValue) );
	    if( (int) strlen(szSField) > psDBF->panFieldSize[iField] )
            {
	        szSField[psDBF->panFieldSize[iField]] = '\0';
                nRetResult = FALSE;
            }
	    strncpy((char *) (pabyRec+psDBF->panFieldOffset[iField]),
		    szSField, strlen(szSField) );
	}
	break;

      case 'L':
        if (psDBF->panFieldSize[iField] >= 1  && 
            (*(char*)pValue == 'F' || *(char*)pValue == 'T'))
            *(pabyRec+psDBF->panFieldOffset[iField]) = *(char*)pValue;
        break;

      default:
	if( (int) strlen((char *) pValue) > psDBF->panFieldSize[iField] )
        {
	    j = psDBF->panFieldSize[iField];
            nRetResult = FALSE;
        }
	else
        {
            memset( pabyRec+psDBF->panFieldOffset[iField], ' ',
                    psDBF->panFieldSize[iField] );
	    j = strlen((char *) pValue);
        }

	strncpy((char *) (pabyRec+psDBF->panFieldOffset[iField]),
		(char *) pValue, j );
	break;
    }

    return( nRetResult );
}

//////////////////////////////////////////////////////////////////////////
//                     DBFWriteAttributeDirectly()                      //
//                                                                      //
//      Write an attribute record to the file, but without any          //
//      reformatting based on type.  The provided buffer is written     //
//      as is to the field position in the record.                      //
//////////////////////////////////////////////////////////////////////////

int CSHP::DBFWriteAttributeDirectly(DBFHandle psDBF, int hEntity, int iField,
                              void * pValue )

{
    int	       	nRecordOffset, i, j;
    unsigned char	*pabyRec;

// -------------------------------------------------------------------- //
//	Is this a valid record?						//
// -------------------------------------------------------------------- //
    if( hEntity < 0 || hEntity > psDBF->nRecords )
        return( FALSE );

    if( psDBF->bNoHeader )
        DBFWriteHeader(psDBF);

// -------------------------------------------------------------------- //
//      Is this a brand new record?                                     //
// -------------------------------------------------------------------- //
    if( hEntity == psDBF->nRecords )
    {
	DBFFlushRecord( psDBF );

	psDBF->nRecords++;
	for( i = 0; i < psDBF->nRecordLength; i++ )
	    psDBF->pszCurrentRecord[i] = ' ';

	psDBF->nCurrentRecord = hEntity;
    }

// -------------------------------------------------------------------- //
//      Is this an existing record, but different than the last one     //
//      we accessed?                                                    //
// -------------------------------------------------------------------- //
    if( psDBF->nCurrentRecord != hEntity )
    {
	DBFFlushRecord( psDBF );

	nRecordOffset = psDBF->nRecordLength * hEntity + psDBF->nHeaderLength;

	fseek( psDBF->fp, nRecordOffset, 0 );
	fread( psDBF->pszCurrentRecord, psDBF->nRecordLength, 1, psDBF->fp );

	psDBF->nCurrentRecord = hEntity;
    }

    pabyRec = (unsigned char *) psDBF->pszCurrentRecord;

// -------------------------------------------------------------------- //
//      Assign all the record fields.                                   //
// -------------------------------------------------------------------- //
    if( (int)strlen((char *) pValue) > psDBF->panFieldSize[iField] )
        j = psDBF->panFieldSize[iField];
    else
    {
        memset( pabyRec+psDBF->panFieldOffset[iField], ' ',
                psDBF->panFieldSize[iField] );
        j = strlen((char *) pValue);
    }

    strncpy((char *) (pabyRec+psDBF->panFieldOffset[iField]),
            (char *) pValue, j );

    psDBF->bCurrentRecordModified = TRUE;
    psDBF->bUpdated = TRUE;

    return( TRUE );
}

//////////////////////////////////////////////////////////////////////////
//                      DBFWriteDoubleAttribute()                       //
//                                                                      //
//      Write a double attribute.                                       //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFWriteDoubleAttribute( DBFHandle psDBF, int iRecord, int iField,
                         double dValue )

{
    return( DBFWriteAttribute( psDBF, iRecord, iField, (void *) &dValue ) );
}

//////////////////////////////////////////////////////////////////////////
//                      DBFWriteIntegerAttribute()                      //
//                                                                      //
//      Write a integer attribute.                                      //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFWriteIntegerAttribute( DBFHandle psDBF, int iRecord, int iField,
                          int nValue )

{
    double	dValue = nValue;

    return( DBFWriteAttribute( psDBF, iRecord, iField, (void *) &dValue ) );
}

//////////////////////////////////////////////////////////////////////////
//                      DBFWriteStringAttribute()                       //
//                                                                      //
//      Write a string attribute.                                       //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFWriteStringAttribute( DBFHandle psDBF, int iRecord, int iField,
                         const char * pszValue )

{
    return( DBFWriteAttribute( psDBF, iRecord, iField, (void *) pszValue ) );
}

//////////////////////////////////////////////////////////////////////////
//                      DBFWriteNULLAttribute()                         //
//                                                                      //
//      Write a string attribute.                                       //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFWriteNULLAttribute( DBFHandle psDBF, int iRecord, int iField )

{
    return( DBFWriteAttribute( psDBF, iRecord, iField, NULL ) );
}

//////////////////////////////////////////////////////////////////////////
//                      DBFWriteLogicalAttribute()                      //
//                                                                      //
//      Write a logical attribute.                                      //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFWriteLogicalAttribute( DBFHandle psDBF, int iRecord, int iField,
		       const char lValue)

{
    return( DBFWriteAttribute( psDBF, iRecord, iField, (void *) (&lValue) ) );
}

//////////////////////////////////////////////////////////////////////////
//                         DBFWriteTuple()                              //
//									//
//	Write an attribute record to the file.				//
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFWriteTuple(DBFHandle psDBF, int hEntity, void * pRawTuple )

{
    int	       	nRecordOffset, i;
    unsigned char	*pabyRec;

// -------------------------------------------------------------------- //
//	Is this a valid record?						//
// -------------------------------------------------------------------- //
    if( hEntity < 0 || hEntity > psDBF->nRecords )
        return( FALSE );

    if( psDBF->bNoHeader )
        DBFWriteHeader(psDBF);

// -------------------------------------------------------------------- //
//      Is this a brand new record?                                     //
// -------------------------------------------------------------------- //
    if( hEntity == psDBF->nRecords )
    {
	DBFFlushRecord( psDBF );

	psDBF->nRecords++;
	for( i = 0; i < psDBF->nRecordLength; i++ )
	    psDBF->pszCurrentRecord[i] = ' ';

	psDBF->nCurrentRecord = hEntity;
    }

// -------------------------------------------------------------------- //
//      Is this an existing record, but different than the last one     //
//      we accessed?                                                    //
// -------------------------------------------------------------------- //
    if( psDBF->nCurrentRecord != hEntity )
    {
	DBFFlushRecord( psDBF );

	nRecordOffset = psDBF->nRecordLength * hEntity + psDBF->nHeaderLength;

	fseek( psDBF->fp, nRecordOffset, 0 );
	fread( psDBF->pszCurrentRecord, psDBF->nRecordLength, 1, psDBF->fp );

	psDBF->nCurrentRecord = hEntity;
    }

    pabyRec = (unsigned char *) psDBF->pszCurrentRecord;

    memcpy ( pabyRec, pRawTuple,  psDBF->nRecordLength );

    psDBF->bCurrentRecordModified = TRUE;
    psDBF->bUpdated = TRUE;

    return( TRUE );
}

//////////////////////////////////////////////////////////////////////////
//                          DBFReadTuple()                              //
//                                                                      //
//      Read one of the attribute fields of a record.                   //
//////////////////////////////////////////////////////////////////////////

const char SHPAPI_CALL1(*)
CSHP::DBFReadTuple(DBFHandle psDBF, int hEntity )

{
    int	       	nRecordOffset;
    unsigned char	*pabyRec;
    static char	*pReturnTuple = NULL;

    static int	nTupleLen = 0;

// -------------------------------------------------------------------- //
//	Have we read the record?					//
// -------------------------------------------------------------------- //
    if( hEntity < 0 || hEntity >= psDBF->nRecords )
        return( NULL );

    if( psDBF->nCurrentRecord != hEntity )
    {
	DBFFlushRecord( psDBF );

	nRecordOffset = psDBF->nRecordLength * hEntity + psDBF->nHeaderLength;

	fseek( psDBF->fp, nRecordOffset, 0 );
	fread( psDBF->pszCurrentRecord, psDBF->nRecordLength, 1, psDBF->fp );

	psDBF->nCurrentRecord = hEntity;
    }

    pabyRec = (unsigned char *) psDBF->pszCurrentRecord;

    if ( nTupleLen < psDBF->nRecordLength) {
      nTupleLen = psDBF->nRecordLength;
      pReturnTuple = (char *) SfRealloc(pReturnTuple, psDBF->nRecordLength);
    }
    
    memcpy ( pReturnTuple, pabyRec, psDBF->nRecordLength );
        
    return( pReturnTuple );
}

//////////////////////////////////////////////////////////////////////////
//                          DBFCloneEmpty()                              //
//                                                                      //
//      Read one of the attribute fields of a record.                   //
//////////////////////////////////////////////////////////////////////////

DBFHandle SHPAPI_CALL
CSHP::DBFCloneEmpty(DBFHandle psDBF, const char * pszFilename ) 
{
    DBFHandle	newDBF;

   newDBF = DBFCreate ( pszFilename );
   if ( newDBF == NULL ) return ( NULL ); 
   
   newDBF->pszHeader = (char *) malloc ( 32 * psDBF->nFields );
   memcpy ( newDBF->pszHeader, psDBF->pszHeader, 32 * psDBF->nFields );
   
   newDBF->nFields = psDBF->nFields;
   newDBF->nRecordLength = psDBF->nRecordLength;
   newDBF->nHeaderLength = 32 * (psDBF->nFields+1);
    
   newDBF->panFieldOffset = (int *) malloc ( sizeof(int) * psDBF->nFields ); 
   memcpy ( newDBF->panFieldOffset, psDBF->panFieldOffset, sizeof(int) * psDBF->nFields );
   newDBF->panFieldSize = (int *) malloc ( sizeof(int) * psDBF->nFields );
   memcpy ( newDBF->panFieldSize, psDBF->panFieldSize, sizeof(int) * psDBF->nFields );
   newDBF->panFieldDecimals = (int *) malloc ( sizeof(int) * psDBF->nFields );
   memcpy ( newDBF->panFieldDecimals, psDBF->panFieldDecimals, sizeof(int) * psDBF->nFields );
   newDBF->pachFieldType = (char *) malloc ( sizeof(int) * psDBF->nFields );
   memcpy ( newDBF->pachFieldType, psDBF->pachFieldType, sizeof(int) * psDBF->nFields );

   newDBF->bNoHeader = TRUE;
   newDBF->bUpdated = TRUE;
   
   DBFWriteHeader ( newDBF );
   DBFClose ( newDBF );
   
   newDBF = DBFOpen ( pszFilename, "rb+" );

   return ( newDBF );
}

//////////////////////////////////////////////////////////////////////////
//                       DBFGetNativeFieldType()                        //
//                                                                      //
//      Return the DBase field type for the specified field.            //
//                                                                      //
//      Value can be one of: 'C' (String), 'D' (Date), 'F' (Float),     //
//                           'N' (Numeric, with or without decimal),    //
//                           'L' (Logical),                             //
//                           'M' (Memo: 10 digits .DBT block ptr)       //
//////////////////////////////////////////////////////////////////////////

char SHPAPI_CALL
CSHP::DBFGetNativeFieldType( DBFHandle psDBF, int iField )

{
    if( iField >=0 && iField < psDBF->nFields )
        return psDBF->pachFieldType[iField];

    return  ' ';
}

//////////////////////////////////////////////////////////////////////////
//                            str_to_upper()                            //
//////////////////////////////////////////////////////////////////////////

static void str_to_upper (char *string)
{
    int len;
    short i = -1;

    len = strlen (string);

    while (++i < len)
        if (isalpha(string[i]) && islower(string[i]))
            string[i] = toupper ((int)string[i]);
}

//////////////////////////////////////////////////////////////////////////
//                          DBFGetFieldIndex()                          //
//                                                                      //
//      Get the index number for a field in a .dbf file.                //
//                                                                      //
//      Contributed by Jim Matthews.                                    //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::DBFGetFieldIndex(DBFHandle psDBF, const char *pszFieldName)

{
    char          name[12], name1[12], name2[12];
    int           i;

    strncpy(name1, pszFieldName,11);
    name1[11] = '\0';
    str_to_upper(name1);

    for( i = 0; i < DBFGetFieldCount(psDBF); i++ )
    {
        DBFGetFieldInfo( psDBF, i, name, NULL, NULL );
        strncpy(name2,name,11);
        str_to_upper(name2);

        if(!strncmp(name1,name2,10))
            return(i);
    }
    return(-1);
}
///////////////////////////////////////////////////////////////////////////////
// $Id: shptree.c,v 1.9 2003/01/28 15:53:41 warmerda Exp $
//
// Project:  Shapelib
// Purpose:  Implementation of quadtree building and searching functions.
// Author:   Frank Warmerdam, warmerdam@pobox.com
//
///////////////////////////////////////////////////////////////////////////////

// -------------------------------------------------------------------- //
//      If the following is 0.5, nodes will be split in half.  If it    //
//      is 0.6 then each subnode will contain 60% of the parent         //
//      node, with 20% representing overlap.  This can be help to       //
//      prevent small objects on a boundary from shifting too high      //
//      up the tree.                                                    //
// -------------------------------------------------------------------- //

/*#define SHP_SPLIT_RATIO	0.55

//////////////////////////////////////////////////////////////////////////
//                          SHPTreeNodeInit()                           //
//                                                                      //
//      Initialize a tree node.                                         //
//////////////////////////////////////////////////////////////////////////

static SHPTreeNode *SHPTreeNodeCreate( double * padfBoundsMin,
                                       double * padfBoundsMax )

{
    SHPTreeNode	*psTreeNode;

    psTreeNode = (SHPTreeNode *) malloc(sizeof(SHPTreeNode));

    psTreeNode->nShapeCount = 0;
    psTreeNode->panShapeIds = NULL;
    psTreeNode->papsShapeObj = NULL;

    psTreeNode->nSubNodes = 0;

    if( padfBoundsMin != NULL )
        memcpy( psTreeNode->adfBoundsMin, padfBoundsMin, sizeof(double) * 4 );

    if( padfBoundsMax != NULL )
        memcpy( psTreeNode->adfBoundsMax, padfBoundsMax, sizeof(double) * 4 );

    return psTreeNode;
}


//////////////////////////////////////////////////////////////////////////
//                           SHPCreateTree()                            //
//////////////////////////////////////////////////////////////////////////

SHPTree SHPAPI_CALL1(*)
CSHP::SHPCreateTree( SHPHandle hSHP, int nDimension, int nMaxDepth,
               double *padfBoundsMin, double *padfBoundsMax )

{
    SHPTree	*psTree;

    if( padfBoundsMin == NULL && hSHP == NULL )
        return NULL;

// -------------------------------------------------------------------- //
//      Allocate the tree object                                        //
// -------------------------------------------------------------------- //
    psTree = (SHPTree *) malloc(sizeof(SHPTree));

    psTree->hSHP = hSHP;
    psTree->nMaxDepth = nMaxDepth;
    psTree->nDimension = nDimension;

// -------------------------------------------------------------------- //
//      If no max depth was defined, try to select a reasonable one     //
//      that implies approximately 8 shapes per node.                   //
// -------------------------------------------------------------------- //
    if( psTree->nMaxDepth == 0 && hSHP != NULL )
    {
        int	nMaxNodeCount = 1;
        int	nShapeCount;

        SHPGetInfo( hSHP, &nShapeCount, NULL, NULL, NULL );
        while( nMaxNodeCount*4 < nShapeCount )
        {
            psTree->nMaxDepth += 1;
            nMaxNodeCount = nMaxNodeCount * 2;
        }
    }

// -------------------------------------------------------------------- //
//      Allocate the root node.                                         //
// -------------------------------------------------------------------- //
    psTree->psRoot = SHPTreeNodeCreate( padfBoundsMin, padfBoundsMax );

// -------------------------------------------------------------------- //
//      Assign the bounds to the root node.  If none are passed in,     //
//      use the bounds of the provided file otherwise the create        //
//      function will have already set the bounds.                      //
// -------------------------------------------------------------------- //
    if( padfBoundsMin == NULL )
    {
        SHPGetInfo( hSHP, NULL, NULL,
                    psTree->psRoot->adfBoundsMin, 
                    psTree->psRoot->adfBoundsMax );
    }

// -------------------------------------------------------------------- //
//      If we have a file, insert all it's shapes into the tree.        //
// -------------------------------------------------------------------- //
    if( hSHP != NULL )
    {
        int	iShape, nShapeCount;
        
        SHPGetInfo( hSHP, &nShapeCount, NULL, NULL, NULL );

        for( iShape = 0; iShape < nShapeCount; iShape++ )
        {
            SHPObject	*psShape;
            
            psShape = SHPReadObject( hSHP, iShape );
            SHPTreeAddShapeId( psTree, psShape );
            SHPDestroyObject( psShape );
        }
    }        

    return psTree;
}

//////////////////////////////////////////////////////////////////////////
//                         SHPDestroyTreeNode()                         //
//////////////////////////////////////////////////////////////////////////

void CSHP::SHPDestroyTreeNode( SHPTreeNode * psTreeNode )

{
    int		i;
    
    for( i = 0; i < psTreeNode->nSubNodes; i++ )
    {
        if( psTreeNode->apsSubNode[i] != NULL )
            SHPDestroyTreeNode( psTreeNode->apsSubNode[i] );
    }
    
    if( psTreeNode->panShapeIds != NULL )
        free( psTreeNode->panShapeIds );

    if( psTreeNode->papsShapeObj != NULL )
    {
        for( i = 0; i < psTreeNode->nShapeCount; i++ )
        {
            if( psTreeNode->papsShapeObj[i] != NULL )
                SHPDestroyObject( psTreeNode->papsShapeObj[i] );
        }

        free( psTreeNode->papsShapeObj );
    }

    free( psTreeNode );
}

//////////////////////////////////////////////////////////////////////////
//                           SHPDestroyTree()                           //
//////////////////////////////////////////////////////////////////////////

void SHPAPI_CALL
CSHP::SHPDestroyTree( SHPTree * psTree )

{
    SHPDestroyTreeNode( psTree->psRoot );
    free( psTree );
}

//////////////////////////////////////////////////////////////////////////
//                       SHPCheckBoundsOverlap()                        //
//                                                                      //
//      Do the given boxes overlap at all?                              //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::SHPCheckBoundsOverlap( double * padfBox1Min, double * padfBox1Max,
                       double * padfBox2Min, double * padfBox2Max,
                       int nDimension )

{
    int		iDim;

    for( iDim = 0; iDim < nDimension; iDim++ )
    {
        if( padfBox2Max[iDim] < padfBox1Min[iDim] )
            return FALSE;
        
        if( padfBox1Max[iDim] < padfBox2Min[iDim] )
            return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//                      SHPCheckObjectContained()                       //
//                                                                      //
//      Does the given shape fit within the indicated extents?          //
//////////////////////////////////////////////////////////////////////////

static int SHPCheckObjectContained( SHPObject * psObject, int nDimension,
                           double * padfBoundsMin, double * padfBoundsMax )

{
    if( psObject->dfXMin < padfBoundsMin[0]
        || psObject->dfXMax > padfBoundsMax[0] )
        return FALSE;
    
    if( psObject->dfYMin < padfBoundsMin[1]
        || psObject->dfYMax > padfBoundsMax[1] )
        return FALSE;

    if( nDimension == 2 )
        return TRUE;
    
    if( psObject->dfZMin < padfBoundsMin[2]
        || psObject->dfZMax < padfBoundsMax[2] )
        return FALSE;
        
    if( nDimension == 3 )
        return TRUE;

    if( psObject->dfMMin < padfBoundsMin[3]
        || psObject->dfMMax < padfBoundsMax[3] )
        return FALSE;

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//                         SHPTreeSplitBounds()                         //
//                                                                      //
//      Split a region into two subregion evenly, cutting along the     //
//      longest dimension.                                              //
//////////////////////////////////////////////////////////////////////////

void SHPAPI_CALL
SHPTreeSplitBounds( double *padfBoundsMinIn, double *padfBoundsMaxIn,
                    double *padfBoundsMin1, double * padfBoundsMax1,
                    double *padfBoundsMin2, double * padfBoundsMax2 )

{
// -------------------------------------------------------------------- //
//      The output bounds will be very similar to the input bounds,     //
//      so just copy over to start.                                     //
// -------------------------------------------------------------------- //
    memcpy( padfBoundsMin1, padfBoundsMinIn, sizeof(double) * 4 );
    memcpy( padfBoundsMax1, padfBoundsMaxIn, sizeof(double) * 4 );
    memcpy( padfBoundsMin2, padfBoundsMinIn, sizeof(double) * 4 );
    memcpy( padfBoundsMax2, padfBoundsMaxIn, sizeof(double) * 4 );
    
// -------------------------------------------------------------------- //
//      Split in X direction.                                           //
// -------------------------------------------------------------------- //
    if( (padfBoundsMaxIn[0] - padfBoundsMinIn[0])
        			> (padfBoundsMaxIn[1] - padfBoundsMinIn[1]) )
    {
        double	dfRange = padfBoundsMaxIn[0] - padfBoundsMinIn[0];

        padfBoundsMax1[0] = padfBoundsMinIn[0] + dfRange * SHP_SPLIT_RATIO;
        padfBoundsMin2[0] = padfBoundsMaxIn[0] - dfRange * SHP_SPLIT_RATIO;
    }

// -------------------------------------------------------------------- //
//      Otherwise split in Y direction.                                 //
// -------------------------------------------------------------------- //
    else
    {
        double	dfRange = padfBoundsMaxIn[1] - padfBoundsMinIn[1];

        padfBoundsMax1[1] = padfBoundsMinIn[1] + dfRange * SHP_SPLIT_RATIO;
        padfBoundsMin2[1] = padfBoundsMaxIn[1] - dfRange * SHP_SPLIT_RATIO;
    }
}

//////////////////////////////////////////////////////////////////////////
//                       SHPTreeNodeAddShapeId()                        //
//////////////////////////////////////////////////////////////////////////

static int
SHPTreeNodeAddShapeId( SHPTreeNode * psTreeNode, SHPObject * psObject,
                       int nMaxDepth, int nDimension )

{
    int		i;
    
// -------------------------------------------------------------------- //
//      If there are subnodes, then consider wiether this object        //
//      will fit in them.                                               //
// -------------------------------------------------------------------- //
    if( nMaxDepth > 1 && psTreeNode->nSubNodes > 0 )
    {
        for( i = 0; i < psTreeNode->nSubNodes; i++ )
        {
            if( SHPCheckObjectContained(psObject, nDimension,
                                      psTreeNode->apsSubNode[i]->adfBoundsMin,
                                      psTreeNode->apsSubNode[i]->adfBoundsMax))
            {
                return SHPTreeNodeAddShapeId( psTreeNode->apsSubNode[i],
                                              psObject, nMaxDepth-1,
                                              nDimension );
            }
        }
    }

// -------------------------------------------------------------------- //
//      Otherwise, consider creating four subnodes if could fit into    //
//      them, and adding to the appropriate subnode.                    //
// -------------------------------------------------------------------- //
#if MAX_SUBNODE == 4
    else if( nMaxDepth > 1 && psTreeNode->nSubNodes == 0 )
    {
        double	adfBoundsMinH1[4], adfBoundsMaxH1[4];
        double	adfBoundsMinH2[4], adfBoundsMaxH2[4];
        double	adfBoundsMin1[4], adfBoundsMax1[4];
        double	adfBoundsMin2[4], adfBoundsMax2[4];
        double	adfBoundsMin3[4], adfBoundsMax3[4];
        double	adfBoundsMin4[4], adfBoundsMax4[4];

        SHPTreeSplitBounds( psTreeNode->adfBoundsMin,
                            psTreeNode->adfBoundsMax,
                            adfBoundsMinH1, adfBoundsMaxH1,
                            adfBoundsMinH2, adfBoundsMaxH2 );

        SHPTreeSplitBounds( adfBoundsMinH1, adfBoundsMaxH1,
                            adfBoundsMin1, adfBoundsMax1,
                            adfBoundsMin2, adfBoundsMax2 );

        SHPTreeSplitBounds( adfBoundsMinH2, adfBoundsMaxH2,
                            adfBoundsMin3, adfBoundsMax3,
                            adfBoundsMin4, adfBoundsMax4 );

        if( SHPCheckObjectContained(psObject, nDimension,
                                    adfBoundsMin1, adfBoundsMax1)
            || SHPCheckObjectContained(psObject, nDimension,
                                    adfBoundsMin2, adfBoundsMax2)
            || SHPCheckObjectContained(psObject, nDimension,
                                    adfBoundsMin3, adfBoundsMax3)
            || SHPCheckObjectContained(psObject, nDimension,
                                    adfBoundsMin4, adfBoundsMax4) )
        {
            psTreeNode->nSubNodes = 4;
            psTreeNode->apsSubNode[0] = SHPTreeNodeCreate( adfBoundsMin1,
                                                           adfBoundsMax1 );
            psTreeNode->apsSubNode[1] = SHPTreeNodeCreate( adfBoundsMin2,
                                                           adfBoundsMax2 );
            psTreeNode->apsSubNode[2] = SHPTreeNodeCreate( adfBoundsMin3,
                                                           adfBoundsMax3 );
            psTreeNode->apsSubNode[3] = SHPTreeNodeCreate( adfBoundsMin4,
                                                           adfBoundsMax4 );

            // recurse back on this node now that it has subnodes //
            return( SHPTreeNodeAddShapeId( psTreeNode, psObject,
                                           nMaxDepth, nDimension ) );
        }
    }
#endif // MAX_SUBNODE == 4 //

// -------------------------------------------------------------------- //
//      Otherwise, consider creating two subnodes if could fit into     //
//      them, and adding to the appropriate subnode.                    //
// -------------------------------------------------------------------- //
#if MAX_SUBNODE == 2
    else if( nMaxDepth > 1 && psTreeNode->nSubNodes == 0 )
    {
        double	adfBoundsMin1[4], adfBoundsMax1[4];
        double	adfBoundsMin2[4], adfBoundsMax2[4];

        SHPTreeSplitBounds( psTreeNode->adfBoundsMin, psTreeNode->adfBoundsMax,
                            adfBoundsMin1, adfBoundsMax1,
                            adfBoundsMin2, adfBoundsMax2 );

        if( SHPCheckObjectContained(psObject, nDimension,
                                 adfBoundsMin1, adfBoundsMax1))
        {
            psTreeNode->nSubNodes = 2;
            psTreeNode->apsSubNode[0] = SHPTreeNodeCreate( adfBoundsMin1,
                                                           adfBoundsMax1 );
            psTreeNode->apsSubNode[1] = SHPTreeNodeCreate( adfBoundsMin2,
                                                           adfBoundsMax2 );

            return( SHPTreeNodeAddShapeId( psTreeNode->apsSubNode[0], psObject,
                                           nMaxDepth - 1, nDimension ) );
        }
        else if( SHPCheckObjectContained(psObject, nDimension,
                                         adfBoundsMin2, adfBoundsMax2) )
        {
            psTreeNode->nSubNodes = 2;
            psTreeNode->apsSubNode[0] = SHPTreeNodeCreate( adfBoundsMin1,
                                                           adfBoundsMax1 );
            psTreeNode->apsSubNode[1] = SHPTreeNodeCreate( adfBoundsMin2,
                                                           adfBoundsMax2 );

            return( SHPTreeNodeAddShapeId( psTreeNode->apsSubNode[1], psObject,
                                           nMaxDepth - 1, nDimension ) );
        }
    }
#endif // MAX_SUBNODE == 2 //

// -------------------------------------------------------------------- //
//      If none of that worked, just add it to this nodes list.         //
// -------------------------------------------------------------------- //
    psTreeNode->nShapeCount++;

    psTreeNode->panShapeIds =
        SfRealloc( psTreeNode->panShapeIds,
                   sizeof(int) * psTreeNode->nShapeCount );
    psTreeNode->panShapeIds[psTreeNode->nShapeCount-1] = psObject->nShapeId;

    if( psTreeNode->papsShapeObj != NULL )
    {
        psTreeNode->papsShapeObj =
            SfRealloc( psTreeNode->papsShapeObj,
                       sizeof(void *) * psTreeNode->nShapeCount );
        psTreeNode->papsShapeObj[psTreeNode->nShapeCount-1] = NULL;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//                         SHPTreeAddShapeId()                          //
//                                                                      //
//      Add a shape to the tree, but don't keep a pointer to the        //
//      object data, just keep the shapeid.                             //
//////////////////////////////////////////////////////////////////////////

int SHPAPI_CALL
CSHP::SHPTreeAddShapeId( SHPTree * psTree, SHPObject * psObject )

{
    return( SHPTreeNodeAddShapeId( psTree->psRoot, psObject,
                                   psTree->nMaxDepth, psTree->nDimension ) );
}

//////////////////////////////////////////////////////////////////////////
//                      SHPTreeCollectShapesIds()                       //
//                                                                      //
//      Work function implementing SHPTreeFindLikelyShapes() on a       //
//      tree node by tree node basis.                                   //
//////////////////////////////////////////////////////////////////////////

void SHPAPI_CALL
CSHP::SHPTreeCollectShapeIds( SHPTree *hTree, SHPTreeNode * psTreeNode,
                        double * padfBoundsMin, double * padfBoundsMax,
                        int * pnShapeCount, int * pnMaxShapes,
                        int ** ppanShapeList )

{
    int		i;
    
// -------------------------------------------------------------------- //
//      Does this node overlap the area of interest at all?  If not,    //
//      return without adding to the list at all.                       //
// -------------------------------------------------------------------- //
    if( !SHPCheckBoundsOverlap( psTreeNode->adfBoundsMin,
                                psTreeNode->adfBoundsMax,
                                padfBoundsMin,
                                padfBoundsMax,
                                hTree->nDimension ) )
        return;

// -------------------------------------------------------------------- //
//      Grow the list to hold the shapes on this node.                  //
// -------------------------------------------------------------------- //
    if( *pnShapeCount + psTreeNode->nShapeCount > *pnMaxShapes )
    {
        *pnMaxShapes = (*pnShapeCount + psTreeNode->nShapeCount) * 2 + 20;
        *ppanShapeList = (int *)
            SfRealloc(*ppanShapeList,sizeof(int) * *pnMaxShapes);
    }

// -------------------------------------------------------------------- //
//      Add the local nodes shapeids to the list.                       //
// -------------------------------------------------------------------- //
    for( i = 0; i < psTreeNode->nShapeCount; i++ )
    {
        (*ppanShapeList)[(*pnShapeCount)++] = psTreeNode->panShapeIds[i];
    }
    
// -------------------------------------------------------------------- //
//      Recurse to subnodes if they exist.                              //
// -------------------------------------------------------------------- //
    for( i = 0; i < psTreeNode->nSubNodes; i++ )
    {
        if( psTreeNode->apsSubNode[i] != NULL )
            SHPTreeCollectShapeIds( hTree, psTreeNode->apsSubNode[i],
                                    padfBoundsMin, padfBoundsMax,
                                    pnShapeCount, pnMaxShapes,
                                    ppanShapeList );
    }
}

//////////////////////////////////////////////////////////////////////////
//                      SHPTreeFindLikelyShapes()                       //
//                                                                      //
//      Find all shapes within tree nodes for which the tree node       //
//      bounding box overlaps the search box.  The return value is      //
//      an array of shapeids terminated by a -1.  The shapeids will     //
//      be in order, as hopefully this will result in faster (more      //
//      sequential) reading from the file.                              //
//////////////////////////////////////////////////////////////////////////

// helper for qsort //
static int
compare_ints( const void * a, const void * b)
{
    return (*(int*)a) - (*(int*)b);
}

int SHPAPI_CALL1(*)
CSHP::SHPTreeFindLikelyShapes( SHPTree * hTree,
                         double * padfBoundsMin, double * padfBoundsMax,
                         int * pnShapeCount )

{
    int	*panShapeList=NULL, nMaxShapes = 0;

// -------------------------------------------------------------------- //
//      Perform the search by recursive descent.                        //
// -------------------------------------------------------------------- //
    *pnShapeCount = 0;

    SHPTreeCollectShapeIds( hTree, hTree->psRoot,
                            padfBoundsMin, padfBoundsMax,
                            pnShapeCount, &nMaxShapes,
                            &panShapeList );

// -------------------------------------------------------------------- //
//      Sort the id array                                               //
// -------------------------------------------------------------------- //

    qsort(panShapeList, *pnShapeCount, sizeof(int), compare_ints);

    return panShapeList;
}

//////////////////////////////////////////////////////////////////////////
//                          SHPTreeNodeTrim()                           //
//                                                                      //
//      This is the recurve version of SHPTreeTrimExtraNodes() that     //
//      walks the tree cleaning it up.                                  //
//////////////////////////////////////////////////////////////////////////

int CSHP::SHPTreeNodeTrim( SHPTreeNode * psTreeNode )

{
    int		i;

// -------------------------------------------------------------------- //
//      Trim subtrees, and free subnodes that come back empty.          //
// -------------------------------------------------------------------- //
    for( i = 0; i < psTreeNode->nSubNodes; i++ )
    {
        if( SHPTreeNodeTrim( psTreeNode->apsSubNode[i] ) )
        {
            SHPDestroyTreeNode( psTreeNode->apsSubNode[i] );

            psTreeNode->apsSubNode[i] =
                psTreeNode->apsSubNode[psTreeNode->nSubNodes-1];

            psTreeNode->nSubNodes--;

            i--; // process the new occupant of this subnode entry //
        }
    }

// -------------------------------------------------------------------- //
//      We should be trimmed if we have no subnodes, and no shapes.     //
// -------------------------------------------------------------------- //
    return( psTreeNode->nSubNodes == 0 && psTreeNode->nShapeCount == 0 );
}

//////////////////////////////////////////////////////////////////////////
//                       SHPTreeTrimExtraNodes()                        //
//                                                                      //
//      Trim empty nodes from the tree.  Note that we never trim an     //
//      empty root node.                                                //
//////////////////////////////////////////////////////////////////////////

void SHPAPI_CALL
CSHP::SHPTreeTrimExtraNodes( SHPTree * hTree )

{
    SHPTreeNodeTrim( hTree->psRoot );
}*/