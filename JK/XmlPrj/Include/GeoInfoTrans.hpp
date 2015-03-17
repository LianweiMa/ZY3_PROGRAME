////GeoInfoTrans.hpp
/********************************************************************
geoinfo file write and read
created:	2014/06/14
author:		mlw
purpose:	This file is for geo info file write and read function
*********************************************************************/
#ifndef _WHU_GEO_FILE_MLW_20140614
#define _WHU_GEO_FILE_MLW_20140614

#include "gdal_1_10_1/ogr_geometry.h"

typedef 	const char*     LPCSTR;
typedef 	unsigned char	BYTE;
#define MAXLENGTH 512

#define SPGC_PI   3.1415926535897932384626433832795e0
#define SPGC_R2D  57.295779513082320876798154814105e0
#define SPGC_D2R  0.017453292519943295769236907684886e0

enum ELPS_TYPE
{
	ET_WGS84 = 0,
	ET_BEIJING54 = 1,
	ET_XIAN80 = 2,
	ET_CN2000 = 3,
	ET_CUSTOM = 4,
};

#define SEMIMAJOR_WGS84      6378137.0	
#define SEMIMINOR_WGS84      6356752.31424517929

#define SEMIMAJOR_BEIJING54  6378245.0	
#define SEMIMINOR_BEIJING54  6356863.018773047267

#define SEMIMAJOR_XIAN80     6378140.0	
#define SEMIMINOR_XIAN80     6356755.2881575287

#define SEMIMAJOR_CN2000     6378137.0	
#define SEMIMINOR_CN2000     6356752.3141

enum PROJECTION_TYPE
{
	GEODETIC_LONLAT = 0,   //经纬度
	TM_PROJECTION = 1,   //横轴墨卡托投影	
	GAUSS_PROJECTION = 2,   //高斯－克吕格投影	
	UTM_PROJECTION = 3,   //通用横轴墨卡托投影
	LAMBERT_PROJECTION = 4,   //兰波特等角投影
	GEODETIC_CENTRXYZ = 5,   //地心坐标    
};

enum ELEVATION_TYPE
{
	GEODETIC_HEI = 0,
	GEOID_DATA = 1,
};

const char*		elps_tag[] = { "GCS_WGS_1984", "GCS_Beijing_1954", "GCS_Xian_1980", "GCS_CGCS_2000", "GCS_USER" };
const double	elps_a[] = { SEMIMAJOR_WGS84, SEMIMAJOR_BEIJING54, SEMIMAJOR_XIAN80, SEMIMAJOR_CN2000 };
const double	elps_b[] = { SEMIMINOR_WGS84, SEMIMINOR_BEIJING54, SEMIMINOR_XIAN80, SEMIMINOR_CN2000 };
//const int		proj_tag[] = { GEODETIC_LONLAT, TM_PROJECTION, GAUSS_PROJECTION, UTM_PROJECTION, LAMBERT_PROJECTION, GEODETIC_CENTRXYZ };

#ifndef _GEOREFF
#define  _GEOREFF
typedef struct tagGEOREFF
{
	int elpsType;//椭球类型
	int projType;//投影类型
	double a, b;//椭球长短半径
	double Origin_Latitude;//初始纬度
	double Central_Meridian;//中央经线
	double False_Easting;//东偏
	double False_Northing;//北偏
	double Scale_Factor;//投影比例
	int eleType;
}GeoRef;
#endif // !_GEOREFF

inline bool SetEllipsoidMode(char *strVal, GeoRef &geoRef, char*strDescType)
{
	if (!strcmp(strDescType, "Standard"))
	{
		if (!strcmp(strVal, "WGS84")) {
			geoRef.elpsType = ET_WGS84; return true;
		}
		else if (!strcmp(strVal, "CGCS2000")) {
			geoRef.elpsType = ET_CN2000; return true;
		}
		else if (!strcmp(strVal, "BeiJing54")) {
			geoRef.elpsType = ET_BEIJING54; return true;
		}
		else if (!strcmp(strVal, "XiAn80")) {
			geoRef.elpsType = ET_XIAN80; return true;
		}
		else return false;
	}
	else if (!strcmp(strDescType, "WKT"))
	{
		if (!strcmp(strVal, "GCS_WGS_1984")) {
			geoRef.elpsType = ET_WGS84; return true;
		}
		else if (!strcmp(strVal, "GCS_CGCS_2000")) {
			geoRef.elpsType = ET_CN2000; return true;
		}
		else if (!strcmp(strVal, "GCS_Beijing_1954")) {
			geoRef.elpsType = ET_BEIJING54; return true;
		}
		else if (!strcmp(strVal, "GCS_Xian_1980")) {
			geoRef.elpsType = ET_XIAN80; return true;
		}
		else return false;
	}
	else return false;
}
inline bool SetProjectionMode(char *strVal, GeoRef &geoRef, char*strDescType)
{
	if (!strcmp(strDescType, "Standard"))
	{
		if (!strcmp(strVal, "Gauss")) {
			geoRef.projType = GAUSS_PROJECTION; return true;
		}
		else if (!strcmp(strVal, "UTM")) {
			geoRef.projType = UTM_PROJECTION; return true;
		}
		else if (!strcmp(strVal, "Albert")) {
			geoRef.projType = LAMBERT_PROJECTION; return true;
		}
		else return false;
	}
	else if (!strcmp(strDescType, "WKT"))
	{
		if (!strcmp(strVal, "Gauss_Kruger")) {
			geoRef.projType = GAUSS_PROJECTION; return true;
		}
		else if (!strcmp(strVal, "Transverse_Mercator")) {
			geoRef.projType = UTM_PROJECTION; return true;
		}
		else if (!strcmp(strVal, "Lambert")) {
			geoRef.projType = LAMBERT_PROJECTION; return true;
		}
		else return false;
	}
	else return false;
}
inline void	GetCvtPar(OGRSpatialReference& srcSpaRef,
	int *elpsType,
	int *projType,
	double *a, double *b,
	double *Origin_Latitude,
	double *Central_Meridian,
	double *False_Easting,
	double *False_Northing,
	double *Scale_Factor,
	int *eleType){
	const char* pInfo = srcSpaRef.GetAttrValue("GEOGCS");
	for (int i = 0; i < sizeof(elps_tag) / sizeof(elps_tag[0]); i++){
		if (!strcmp(pInfo, elps_tag[i])) {
			*elpsType = i;	break;
		}
	}
	OGRErr ret;
	*a = srcSpaRef.GetSemiMajor(&ret);	if (ret != OGRERR_NONE) *a = elps_a[*elpsType>ET_CN2000 ? ET_CN2000 : *elpsType];
	*b = srcSpaRef.GetSemiMinor(&ret);	if (ret != OGRERR_NONE) *b = elps_b[*elpsType > ET_CN2000 ? ET_CN2000 : *elpsType];

	*eleType = GEODETIC_HEI;
	*Origin_Latitude = *Central_Meridian = *False_Easting = *False_Northing = *Scale_Factor = 0.0;

	if (srcSpaRef.IsGeocentric()){
		*projType = GEODETIC_CENTRXYZ;
		return;
	}
	if (srcSpaRef.IsGeographic()){
		*projType = GEODETIC_LONLAT;
		return;
	}

	pInfo = srcSpaRef.GetAttrValue("PROJECTION");
	if (!strcmp(pInfo, "Gauss_Kruger")) {
		*projType = GAUSS_PROJECTION;
	}
	else if (!strcmp(pInfo, "Transverse_Mercator")) {
		*projType = UTM_PROJECTION;
	}
	else if (!strcmp(pInfo, "Lambert")) {
		*projType = LAMBERT_PROJECTION;
	}
	*Origin_Latitude = srcSpaRef.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN);
	*Scale_Factor = srcSpaRef.GetProjParm(SRS_PP_SCALE_FACTOR);
	*Central_Meridian = srcSpaRef.GetProjParm(SRS_PP_CENTRAL_MERIDIAN);
	*False_Easting = srcSpaRef.GetProjParm(SRS_PP_FALSE_EASTING);
	*False_Northing = srcSpaRef.GetProjParm(SRS_PP_FALSE_NORTHING);

}
inline void SetCvtPar(
	int elpsType,
	int projType,
	double a, double b,
	double Origin_Latitude,
	double Central_Meridian,
	double False_Easting,
	double False_Northing,
	double Scale_Factor,
	int eleType,
	OGRSpatialReference* pSrcSpaRef)
{
	switch (projType)
	{
	case GEODETIC_LONLAT: //经纬度
		pSrcSpaRef->SetProjCS("Longitude_Latitude");
		break;
	case TM_PROJECTION://横轴墨卡托投影
		pSrcSpaRef->SetProjCS("TM");
		break;
	case GAUSS_PROJECTION://高斯－克吕格投影
		pSrcSpaRef->SetProjCS("Gauss");
		break;
	case UTM_PROJECTION://通用横轴墨卡托投影
		pSrcSpaRef->SetProjCS("UTM");
		break;
	case LAMBERT_PROJECTION://兰波特等角投影
		pSrcSpaRef->SetProjCS("Albert");
		break;
	case GEODETIC_CENTRXYZ://地心坐标    
		pSrcSpaRef->SetProjCS("Cen_XYZ");
		break;
	default:
		pSrcSpaRef->SetProjCS("Gauss");
		break;
	}
	switch (elpsType)
	{
	case ET_WGS84:
		pSrcSpaRef->SetGeogCS("GCS_WGS_1984",
			"WGS_1984",
			"WGS_1984",
			a, a / (a - b));
		break;
	case ET_BEIJING54:
		pSrcSpaRef->SetGeogCS("GCS_Beijing_1954",
			"Beijing_1954",
			"Beijing_1954",
			a, a / (a - b));
		break;
	case ET_XIAN80:
		pSrcSpaRef->SetGeogCS("GCS_Xian_1980",
			"Xian_1980",
			"Xian_1980",
			a, a / (a - b));
		break;
	case ET_CN2000:
		pSrcSpaRef->SetGeogCS("GCS_CGCS_2000",
			"CGCS_2000",
			"CGCS_2000",
			a, a / (a - b));
		break;
	default:
		pSrcSpaRef->SetGeogCS("GCS_WGS_1984",
			"WGS_1984",
			"WGS_1984",
			a, a / (a - b));
		break;
	}
	//////////////////投影比例///////////////////////////
	pSrcSpaRef->SetProjParm(SRS_PP_SCALE_FACTOR, Scale_Factor);
	//////////////////中央经线/////////////////////
	pSrcSpaRef->SetProjParm(SRS_PP_CENTRAL_MERIDIAN, Central_Meridian / SPGC_D2R);
	///////////////东偏///////////////////////
	pSrcSpaRef->SetProjParm(SRS_PP_FALSE_EASTING, False_Easting);
	///////////////北偏///////////////////////
	pSrcSpaRef->SetProjParm(SRS_PP_FALSE_NORTHING, False_Northing);
	/////////////////Origin_Latitude/////////////////////
	pSrcSpaRef->GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN, Origin_Latitude);
}

class  CGeoFile
{
public:
	CGeoFile(){
		m_ElpsType = ET_CN2000;
		m_ProjType = GAUSS_PROJECTION;
		 m_eleType=0;
		 m_ElpsA = SEMIMAJOR_CN2000, m_ElpsB = SEMIMINOR_CN2000;
		 m_OriginLatitude=0.0;  // degree
		 m_CentralMeridian=117.0; // degree
		 m_FalseEasting=500000.0;
		 m_FalseNorthing=0.0;
		 m_ScaleFactor=1.0;
		memset(m_strHeiAdj,0,sizeof(char)*MAXLENGTH);
		memset(m_strPathName, 0, sizeof(char)*MAXLENGTH);
	}
	virtual ~CGeoFile(){}

	void   Set_Cvt_Par(int elpsType,
		int projType,
		double a,
		double b,
		double Origin_Latitude,  // rad
		double Central_Meridian, // rad
		double False_Easting,
		double False_Northing,
		double Scale_Factor,
		int eleType,
		LPCSTR lpstrHeiAdj = NULL)
	{
		m_ElpsType = elpsType;
		m_ProjType = projType;

		m_ElpsA = a; m_ElpsB = b;
		m_OriginLatitude = Origin_Latitude;
		m_CentralMeridian = Central_Meridian;
		m_FalseEasting = False_Easting;
		m_FalseNorthing = False_Northing;
		m_ScaleFactor = Scale_Factor;
		m_eleType = eleType;
	}

	void   Get_Cvt_Par(int *elpsType,
		int *projType,
		double *a,
		double *b,
		double *Origin_Latitude,  // degree
		double *Central_Meridian, // degree
		double *False_Easting,
		double *False_Northing,
		double *Scale_Factor,
		int *eleType,
		char *lpstrHeiAdj = NULL
		)
	{
		*elpsType = m_ElpsType;
		*projType = m_ProjType;

		*a = m_ElpsA;  *b = m_ElpsB;
		*Origin_Latitude = m_OriginLatitude;
		*Central_Meridian = m_CentralMeridian;
		*False_Easting = m_FalseEasting;
		*False_Northing = m_FalseNorthing;
		*Scale_Factor = m_ScaleFactor;
		*eleType = m_eleType;
		//strcpy(lpstrHeiAdj, m_strHeiAdj);
	}

	bool   Load4File(LPCSTR lpstrPathName)
	{
		//FILE*fp = fopen(lpstrPathName,"r");
		return false;
	}
	bool   Save2File(LPCSTR lpstrPathName)
	{
		FILE*fp = fopen(lpstrPathName, "w");
		if (fp)
		{
			fprintf(fp, "%s\n", "[GeoCvtInf]" );
			fprintf(fp, "%s%d\n", "Ellipsoid=",m_eleType );
			fprintf(fp, "%s%d\n", "Projectid=", m_ProjType);
			fprintf(fp, "%s%lf\n", "EllipsoA=", m_ElpsA);
			fprintf(fp, "%s%lf\n", "EllipsoB=", m_ElpsB);
			fprintf(fp, "%s%lf\n", "ProjectScale=", m_ScaleFactor);
			fprintf(fp, "%s%d\n", "ProjectHemi=", 1);
			fprintf(fp, "%s%lf\n", "ProjectOriLat=", m_OriginLatitude);
			fprintf(fp, "%s%lf\n", "ProjectCenMeri=", m_CentralMeridian);
			fprintf(fp, "%s%lf\n", "ProjectFalseNor=", m_FalseNorthing);
			fprintf(fp, "%s%lf\n", "ProjectFalseEas=", m_FalseEasting);
			fprintf(fp, "%s%d\n", "ProjectZoneNo=", 0);
			fprintf(fp, "%s%d\n", "Elevationid=", m_eleType);
			fprintf(fp, "%s%s\n", "ElevationHeiAdj=", m_strHeiAdj);

			fclose(fp);
			return true;
		}
		else { printf("ERROR: file open error: %s\n", lpstrPathName); return false;}	
	}

	// Some Default Value Seting
	static void GetEplsDefPar(int eplsIdx, double *a, double *b, double *e = 0)
	{
		switch (eplsIdx)
		{
		case ET_WGS84:
			*a = SEMIMAJOR_WGS84;
			*b = SEMIMINOR_WGS84;
			if (e)*e = (*a - *b) / *a;
			break;
		case ET_BEIJING54:
			*a = SEMIMAJOR_BEIJING54;
			*b = SEMIMINOR_BEIJING54;
			if (e)*e = (*a - *b) / *a;
			break;
		case ET_XIAN80:
			*a = SEMIMAJOR_XIAN80;
			*b = SEMIMINOR_XIAN80;
			if (e)*e = (*a - *b) / *a;
			break;
		case ET_CN2000:
			*a = SEMIMAJOR_CN2000;
			*b = SEMIMINOR_CN2000;
			if (e)*e = (*a - *b) / *a;
			break;
		}
	}
	static void GetProjDefPar(int projIdx, double *scale)
	{
		switch (projIdx)
		{
		case GAUSS_PROJECTION:
			*scale = 1;
			break;
		case UTM_PROJECTION:
			*scale = 0.9996;
			break;
		}
	}
	static void GetUTMDefPar(int zone, char hemiSphere, double *Central_Meridian, double *False_Northing)
	{
		*Central_Meridian = (6 * zone - 3)*3.14159265358979323 / 180.0;
		*False_Northing = (hemiSphere == 'S') ? 10000000 : 0;
	}

	static bool Save7Para2File(LPCSTR lpstrPathName, double dx, double dy, double dz, double rx, double ry, double rz, double scale)
	{
		FILE*fp = fopen(lpstrPathName, "w");
		if (fp)
		{
			fprintf(fp, "%s\n", "[Param7Inf]");
			fprintf(fp, "%s%e\n", "Dx=", dx);
			fprintf(fp, "%s%e\n", "Dy=", dy);
			fprintf(fp, "%s%e\n", "Dz=", dz);
			fprintf(fp, "%s%e\n", "Rx=", rx);
			fprintf(fp, "%s%e\n", "Ry=", ry);
			fprintf(fp, "%s%e\n", "Rz=", rz);
			fprintf(fp, "%s%e\n", "Scale=", scale);

			fclose(fp);
			return true;
		}
		else { printf("ERROR:file open error:%s\n", lpstrPathName); return false; }
	}

private:
	int    m_ElpsType;
	int    m_ProjType;
	int    m_eleType;
	double m_ElpsA, m_ElpsB;
	double m_OriginLatitude;  // degree
	double m_CentralMeridian; // degree
	double m_FalseEasting;
	double m_FalseNorthing;
	double m_ScaleFactor;
	char   m_strHeiAdj[MAXLENGTH];
	char   m_strPathName[MAXLENGTH];
};
#endif

