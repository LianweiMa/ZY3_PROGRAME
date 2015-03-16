// SHP.h: interface for the CSHP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHP_H__36AD6449_ADB0_45EB_96E2_9E859916320C__INCLUDED_)
#define AFX_SHP_H__36AD6449_ADB0_45EB_96E2_9E859916320C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////SHP 结构体的定义///////////////////////////////////
#include <stdio.h>

#ifdef USE_DBMALLOC
#include <dbmalloc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
//                        Configuration options.                        //
//////////////////////////////////////////////////////////////////////////

// -------------------------------------------------------------------- //
//      Should the DBFReadStringAttribute() strip leading and           //
//      trailing white space?                                           //
// -------------------------------------------------------------------- //
#define TRIM_DBF_WHITESPACE

// -------------------------------------------------------------------- //
//      Should we write measure values to the Multipatch object?        //
//      Reportedly ArcView crashes if we do write it, so for now it     //
//      is disabled.                                                    //
// -------------------------------------------------------------------- //
#define DISABLE_MULTIPATCH_MEASURE

// -------------------------------------------------------------------- //
//      SHPAPI_CALL                                                     //
//                                                                      //
//      The following two macros are present to allow forcing           //
//      various calling conventions on the Shapelib API.                //
//                                                                      //
//      To force __stdcall conventions (needed to call Shapelib         //
//      from Visual Basic and/or Dephi I believe) the makefile could    //
//      be modified to define:                                          //
//                                                                      //
//        /DSHPAPI_CALL=__stdcall                                       //
//                                                                      //
//      If it is desired to force export of the Shapelib API without    //
//      using the shapelib.def file, use the following definition.      //
//                                                                      //
//        /DSHAPELIB_DLLEXPORT                                          //
//                                                                      //
//      To get both at once it will be necessary to hack this           //
//      include file to define:                                         //
//                                                                      //
//        #define SHPAPI_CALL __declspec(dllexport) __stdcall           //
//        #define SHPAPI_CALL1 __declspec(dllexport) * __stdcall        //
//                                                                      //
//      The complexity of the situtation is partly caused by the        //
//      peculiar requirement of Visual C++ that __stdcall appear        //
//      after any "*"'s in the return value of a function while the     //
//      __declspec(dllexport) must appear before them.                  //
// -------------------------------------------------------------------- //

#ifdef SHAPELIB_DLLEXPORT
#  define SHPAPI_CALL __declspec(dllexport)
#  define SHPAPI_CALL1(x)  __declspec(dllexport) x
#endif

#ifndef SHPAPI_CALL
#  define SHPAPI_CALL
#endif

#ifndef SHPAPI_CALL1
#  define SHPAPI_CALL1(x)      x SHPAPI_CALL
#endif
    
//////////////////////////////////////////////////////////////////////////
//                             SHP Support.                             //
//////////////////////////////////////////////////////////////////////////
typedef	struct
{
    FILE        *fpSHP;
    FILE	*fpSHX;

    int		nShapeType;				// SHPT_* //
    
    int		nFileSize;				// SHP file //

    int         nRecords;
    int		nMaxRecords;
    int		*panRecOffset;
    int		*panRecSize;

    double	adBoundsMin[4];
    double	adBoundsMax[4];

    int		bUpdated;

    unsigned char *pabyRec;
    int         nBufSize;
} SHPInfo;

typedef SHPInfo * SHPHandle;

// -------------------------------------------------------------------- //
//      Shape types (nSHPType)                                          //
// -------------------------------------------------------------------- //
#define SHPT_NULL	0
#define SHPT_POINT	1
#define SHPT_ARC	3
#define SHPT_POLYGON	5
#define SHPT_MULTIPOINT	8
#define SHPT_POINTZ	11
#define SHPT_ARCZ	13
#define SHPT_POLYGONZ	15
#define SHPT_MULTIPOINTZ 18
#define SHPT_POINTM	21
#define SHPT_ARCM	23
#define SHPT_POLYGONM	25
#define SHPT_MULTIPOINTM 28
#define SHPT_MULTIPATCH 31


// -------------------------------------------------------------------- //
//      Part types - everything but SHPT_MULTIPATCH just uses           //
//      SHPP_RING.                                                      //
// -------------------------------------------------------------------- //

#define SHPP_TRISTRIP	0
#define SHPP_TRIFAN	1
#define SHPP_OUTERRING	2
#define SHPP_INNERRING	3
#define SHPP_FIRSTRING	4
#define SHPP_RING	5

// -------------------------------------------------------------------- //
//      SHPObject - represents on shape (without attributes) read       //
//      from the .shp file.                                             //
// -------------------------------------------------------------------- //
typedef struct
  {
    int		nSHPType;	//Shape Type (SHPT_* - see list above)

    int		nShapeId; 	//Shape Number (-1 is unknown/unassigned)

    int		nParts;		//# of Parts (0 implies single part with no info)
    int		*panPartStart;  //Start Vertex of part
    int		*panPartType;	//Part Type (SHPP_RING if not SHPT_MULTIPATCH)
    
    int		nVertices;	//Vertex list 
    double	*padfX;		
    double	*padfY;
    double	*padfZ;		//(all zero if not provided)
    double	*padfM;		//(all zero if not provided)

    double	dfXMin;		//Bounds in X, Y, Z and M dimensions
    double	dfYMin;
    double	dfZMin;
    double	dfMMin;

    double	dfXMax;
    double	dfYMax;
    double	dfZMax;
    double	dfMMax;
  } SHPObject;


// -------------------------------------------------------------------- //
//      Shape quadtree indexing API.                                    //
// -------------------------------------------------------------------- //

// this can be two or four for binary or quad tree //
#define MAX_SUBNODE	4

typedef struct shape_tree_node
{
    // region covered by this node //
    double	adfBoundsMin[4];
    double	adfBoundsMax[4];

    // list of shapes stored at this node.  The papsShapeObj pointers
    //  or the whole list can be NULL 
    int		nShapeCount;
    int		*panShapeIds;
    SHPObject   **papsShapeObj;

    int		nSubNodes;
    struct shape_tree_node *apsSubNode[MAX_SUBNODE];
    
} SHPTreeNode;

typedef struct
{
    SHPHandle   hSHP;
    
    int		nMaxDepth;
    int		nDimension;
    
    SHPTreeNode	*psRoot;
} SHPTree;

//////////////////////////////////////////////////////////////////////////
//                             DBF Support.                             //
//////////////////////////////////////////////////////////////////////////
typedef	struct
{
    FILE	*fp;

    int         nRecords;              //记录的总数

    int		nRecordLength;             //记录的总长度
    int		nHeaderLength;             //文件头信息的总长度
    int		nFields;                   //字段的个数
    int		*panFieldOffset;           //字段偏移量
    int		*panFieldSize;             //字段大小
    int		*panFieldDecimals;         //小数位
    char	*pachFieldType;            //?类型

    char	*pszHeader;

    int		nCurrentRecord;
    int		bCurrentRecordModified;
    char	*pszCurrentRecord;
    
    int		bNoHeader;
    int		bUpdated;
} DBFInfo;

typedef DBFInfo * DBFHandle;

typedef enum {
	FTString,		// fixed length string field 		//
	FTInteger,		// numeric field with no decimals 	//
	FTDouble,		// numeric field with decimals 		//
	FTLogical,		// logical field.                   //
	FTInvalid       // not a recognised field type 		//
} DBFFieldType;
#define XBASE_FLDHDR_SZ       32

/////////////////////////////////////////////////////////////////////

class CSHP  
{
public:
	CSHP();
	virtual ~CSHP();
	///////////////////////////////////////////////////////////////////

	//static void	SwapWord( int length, void * wordP );
	//static void	_SHPSetBounds( uchar * pabyRec, SHPObject * psShape );
	/*void SHPWriteHeader( SHPHandle psSHP ); 
	static void SHPDestroyTreeNode( SHPTreeNode * psTreeNode );
	void SHPAPI_CALL
		SHPTreeCollectShapeIds( SHPTree *hTree, SHPTreeNode * psTreeNode,
		double * padfBoundsMin, double * padfBoundsMax,
		int * pnShapeCount, int * pnMaxShapes,
		int ** ppanShapeList );
	static int SHPTreeNodeTrim( SHPTreeNode * psTreeNode );
	static int
	SHPTreeNodeAddShapeId( SHPTreeNode * psTreeNode, SHPObject * psObject,
		int nMaxDepth, int nDimension );*/
	///////////////////////////////////////////////////////////////////
	
	// -------------------------------------------------------------------- //
	//      SHP API Prototypes                                              //
	// -------------------------------------------------------------------- //
	//打开shp和shx文件,并读取shp头信息和shx整个文件
	SHPHandle SHPAPI_CALL
		SHPOpen( const char * pszShapeFile, const char * pszAccess ); 
	//打开shp和shx文件,并读取shp头信息和shx整个文件,Txt格式
	SHPHandle SHPAPI_CALL
		SHPOpenTxt( const char * pszShapeFile, const char * pszAccess );
	//创建SHP文件和SHX文件,并写入头信息
	SHPHandle SHPAPI_CALL
		SHPCreate( const char * pszShapeFile, int nShapeType ); 

	//创建SHP文件和SHX文件,并写入头信息,txt格式
	SHPHandle SHPAPI_CALL
		SHPCreateHeaderTxt( SHPHandle psSHP,const char * pszLayer, int nShapeType );
		
	//获取文件基本信息:实体个数,类型,边界double Min[4]Max[4]
	void SHPAPI_CALL
		SHPGetInfo( SHPHandle hSHP, int * pnEntities, int * pnShapeType,  
		double * padfMinBound, double * padfMaxBound );              
	
	SHPObject SHPAPI_CALL1(*)
		SHPReadObject( SHPHandle hSHP, int iShape );
	//写入单个实体信息
	int SHPAPI_CALL
		SHPWriteObject( SHPHandle hSHP, int iShape, SHPObject * psObject );
	//写入单个实体信息,Txt格式	
	int SHPAPI_CALL
		SHPWriteObjectTxt(SHPHandle psSHP,SHPHandle psOutSHP, int nShapeId, SHPObject * psObject);
	
	void SHPAPI_CALL
		SHPDestroyObject( SHPObject * psObject );
	void SHPAPI_CALL
		SHPComputeExtents( SHPObject * psObject );
	SHPObject SHPAPI_CALL1(*)
		SHPCreateObject( int nSHPType, int nShapeId,
		int nParts, int * panPartStart, int * panPartType,
		int nVertices, double * padfX, double * padfY,
		double * padfZ, double * padfM );
	SHPObject SHPAPI_CALL1(*)
		SHPCreateSimpleObject( int nSHPType, int nVertices,
		double * padfX, double * padfY, double * padfZ );
	
	int SHPAPI_CALL
		SHPRewindObject( SHPHandle hSHP, SHPObject * psObject );
	//关闭二进制文件	
	void SHPAPI_CALL
		SHPClose( SHPHandle hSHP ); 
	//关闭文本文件	
	void SHPAPI_CALL
		SHPCloseTxt(SHPHandle psSHP );
	
	const char SHPAPI_CALL1(*)
		SHPTypeName( int nSHPType );
	const char SHPAPI_CALL1(*)
		SHPPartTypeName( int nPartType );	
	
	// -------------------------------------------------------------------- //
	//      Shape quadtree indexing API.                                    //
	// -------------------------------------------------------------------- //
	SHPTree SHPAPI_CALL1(*)
		SHPCreateTree( SHPHandle hSHP, int nDimension, int nMaxDepth,
		double *padfBoundsMin, double *padfBoundsMax );
	void    SHPAPI_CALL
		SHPDestroyTree( SHPTree * hTree );
	
	int	SHPAPI_CALL
		SHPWriteTree( SHPTree *hTree, const char * pszFilename );
	SHPTree SHPAPI_CALL
		SHPReadTree( const char * pszFilename );
	
	int	SHPAPI_CALL
		SHPTreeAddObject( SHPTree * hTree, SHPObject * psObject );
	int	SHPAPI_CALL
		SHPTreeAddShapeId( SHPTree * hTree, SHPObject * psObject );
	int	SHPAPI_CALL
		SHPTreeRemoveShapeId( SHPTree * hTree, int nShapeId );
	
	void 	SHPAPI_CALL
		SHPTreeTrimExtraNodes( SHPTree * hTree );
	
	int    SHPAPI_CALL1(*)
		SHPTreeFindLikelyShapes( SHPTree * hTree,
		double * padfBoundsMin,
		double * padfBoundsMax,
		int * );
	int     SHPAPI_CALL
		SHPCheckBoundsOverlap( double *, double *, double *, double *, int );
	//////////////////////////////////////////////////////////////////////////
	//                             DBF Support.                             //
	//////////////////////////////////////////////////////////////////////////
	//打开dbf文件,读取文件头信息和记录的相关信息
	DBFHandle SHPAPI_CALL
		DBFOpen( const char * pszDBFFile, const char * pszAccess );
	DBFHandle SHPAPI_CALL
		DBFCreate( const char * pszDBFFile );
	//获取字段的总个数
	int	SHPAPI_CALL
		DBFGetFieldCount( DBFHandle psDBF );
	////获取记录的总个数
	int	SHPAPI_CALL
		DBFGetRecordCount( DBFHandle psDBF );
	int	SHPAPI_CALL
		DBFAddField( DBFHandle hDBF, const char * pszFieldName,
		DBFFieldType eType, int nWidth, int nDecimals );
	
	//获取字段类型
	DBFFieldType SHPAPI_CALL
		DBFGetFieldInfo( DBFHandle psDBF, int iField, 
		char * pszFieldName, int * pnWidth, int * pnDecimals );
	
	int SHPAPI_CALL
		DBFGetFieldIndex(DBFHandle psDBF, const char *pszFieldName);
	
	//读取某个记录某个字段的整型信息(整型属性信息)
	int 	SHPAPI_CALL
		DBFReadIntegerAttribute( DBFHandle hDBF, int iShape, int iField );
	//读取某个记录某个字段的双精度信息(双精度属性信息)
	double 	SHPAPI_CALL
		DBFReadDoubleAttribute( DBFHandle hDBF, int iShape, int iField );
	//读取某个记录某个字段的字符串信息(字符串属性信息)
	const char SHPAPI_CALL1(*)
		DBFReadStringAttribute( DBFHandle hDBF, int iShape, int iField );
	//读取某个记录某个字段的逻辑型信息(逻辑型属性信息)
	const char SHPAPI_CALL1(*)
		DBFReadLogicalAttribute( DBFHandle hDBF, int iShape, int iField );
	int     SHPAPI_CALL
		DBFIsAttributeNULL( DBFHandle hDBF, int iShape, int iField );	
	
	int SHPAPI_CALL
		DBFWriteIntegerAttribute( DBFHandle hDBF, int iShape, int iField, 
		int nFieldValue );
	int SHPAPI_CALL
		DBFWriteDoubleAttribute( DBFHandle hDBF, int iShape, int iField,
		double dFieldValue );
	int SHPAPI_CALL
		DBFWriteStringAttribute( DBFHandle hDBF, int iShape, int iField,
		const char * pszFieldValue );
	int SHPAPI_CALL
		DBFWriteNULLAttribute( DBFHandle hDBF, int iShape, int iField );
	
	int SHPAPI_CALL
		DBFWriteLogicalAttribute( DBFHandle hDBF, int iShape, int iField,
		const char lFieldValue);
	int SHPAPI_CALL
		DBFWriteAttributeDirectly(DBFHandle psDBF, int hEntity, int iField,
		void * pValue );
	const char SHPAPI_CALL1(*)
		DBFReadTuple(DBFHandle psDBF, int hEntity );
	int SHPAPI_CALL
		DBFWriteTuple(DBFHandle psDBF, int hEntity, void * pRawTuple );
	
	DBFHandle SHPAPI_CALL
		DBFCloneEmpty(DBFHandle psDBF, const char * pszFilename );
	
	void	SHPAPI_CALL
		DBFClose( DBFHandle hDBF );
	char    SHPAPI_CALL
		DBFGetNativeFieldType( DBFHandle hDBF, int iField );

};

#ifdef __cplusplus
}
#endif

#endif // !defined(AFX_SHP_H__36AD6449_ADB0_45EB_96E2_9E859916320C__INCLUDED_)
