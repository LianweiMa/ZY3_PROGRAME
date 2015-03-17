/*
	File	:	MipAdjTsk.hpp
	Brief	:	writer for adjtsk of mip project
	
	Author	:	Yi Wan
	Date	:	2013/12/08, 11:14	
	E-mail	:	yi.wan@outlook.com
*/
#ifndef YIWAN_IBUNDLE_SAT_MIP_MIPADJTSK_HPP
#define YIWAN_IBUNDLE_SAT_MIP_MIPADJTSK_HPP

#include <stdio.h>

#include <vector>
#include <map>
#include <list>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <string>
#include <exception>
#include <assert.h>
#include <cfloat>

#ifndef WY_Assert
#define WY_Assert(expr)	assert(expr)
#endif

namespace Wanyi{


/*
	class:	ControlInfo
	brief:	parser of Control Information file
	time :	2013/12/07, 21:25
*/
class ControlInfo
{
public:
	enum GCPTYPE{WGS84 = 0,	LOCAL = 1,};
	enum DEMTYPE{SRTM  = 0, OTHER = 1,};
	enum HEITYPE{GEODETIC = 0, NORMAL = 1,};
public:
	ControlInfo(){	errHeader = "ControlInfo::";	}
	~ControlInfo(){}

	void SetGcpInfo(std::string gcpf, std::string icpf, GCPTYPE gcpTy, HEITYPE heiTy, std::string locGcd, std::string wgs84Gcd, std::string par7Gcd, std::string egmf = std::string()){
		try{
			gcpFile = gcpf;				icpFile = icpf;	
			eGcpType = gcpTy;			eGcpHeiType = heiTy;	
			gcpLocGcdFile = locGcd;		gcpWgs84GcdFile = wgs84Gcd;
			gcpTransGcdFile = par7Gcd;	gcpEgmFile = egmf;

			if (eGcpType == WGS84)		gcpType = "WGS-84";
			else if(eGcpType == LOCAL)	gcpType = "Local";
			else	throw std::runtime_error("Invalid gcpType! ");

			if (eGcpHeiType == NORMAL)			gcpHeiType = "Normal";
			else if (eGcpHeiType == GEODETIC)	gcpHeiType = "Geodetic";
			else	throw std::runtime_error("Invalid gcp Height type! ");
		} catch (const std::runtime_error& e) {
			std::runtime_error err(errHeader + e.what());
			std::cerr << err.what() << std::endl;
			throw err;
		}
	}

	void SetDemInfo(DEMTYPE demTy, HEITYPE demHeiTy, std::string demf, std::string locGcd, std::string wgs84Gcd, std::string par7Gcd, std::string egmf = std::string()){
		try {
			eDemType = demTy;			eDemHeiType = demHeiTy;
			addDemPath = demf;			demLocGcdFile = locGcd;
			demWgs84GcdFile = wgs84Gcd;	demTransGcdFile = par7Gcd;
			demEgmFile = egmf;

			if (demTy == SRTM)			demType = "SRTM";
			else if (demTy == OTHER)	demType = "Other";
			else	throw std::runtime_error("Invalid Dem Type! ");

			if (demHeiTy == NORMAL)			demHeiType = "Normal";
			else if (demHeiTy == GEODETIC)	demHeiType = "Geodetic";
			else	throw std::runtime_error("Invalid Dem Height type! ");
		} catch (const std::runtime_error& e) {
			std::runtime_error err(errHeader + e.what());
			std::cerr << err.what() << std::endl;
			throw err;
		}
	}

	void WriteCtiFile(std::string ctiFile){
		try {
			FILE* fpCti = fopen(ctiFile.c_str(), "wt");
			if (fpCti == NULL)	throw std::runtime_error("Can not create cti file! ");
			fprintf(fpCti, "Control Points : \"%s\"\n",				gcpFile.c_str());
			fprintf(fpCti, "Check Points : \"%s\"\n",					icpFile.c_str());
			fprintf(fpCti, "Control Point Coordinates : \"%s\"\n",	gcpType.c_str());
			fprintf(fpCti, "Local Coordinates GCD File : \"%s\"\n",	gcpLocGcdFile.c_str());
			fprintf(fpCti, "Transform GCD File : \"%s\"\n",			gcpTransGcdFile.c_str());
			fprintf(fpCti, "WGS-84 Coordinates GCD File : \"%s\"\n",	gcpWgs84GcdFile.c_str());
			fprintf(fpCti, "Control Point Height System : \"%s\"\n",	gcpHeiType.c_str());
			fprintf(fpCti, "EGM File for Control Point : \"%s\"\n",	gcpEgmFile.c_str());
			fprintf(fpCti, "DEM Type £º	\"%s\"\n",					demType.c_str());
			fprintf(fpCti, "Additional Dem File : \"%s\"\n",			addDemPath.c_str());	
			fprintf(fpCti, "Local Coordinates GCD File : \"%s\"\n",	demLocGcdFile.c_str());
			fprintf(fpCti, "Transform GCD File : \"%s\"\n",			demTransGcdFile.c_str());		
			fprintf(fpCti, "WGS-84 Coordinates GCD File : \"%s\"\n",	demWgs84GcdFile.c_str());
			fprintf(fpCti, "DEM Height Type : \"%s\"\n",				demHeiType.c_str());		
			fprintf(fpCti, "EGM File for DEM: \"%s\"",				demEgmFile.c_str());
			fclose(fpCti);	fpCti = NULL;
		} catch (const std::runtime_error& e) {
			std::runtime_error err(errHeader + e.what());
			std::cerr << err.what() << std::endl;
			throw err;
		}
	}

public:
	std::string gcpFile;				//ground control points
	std::string icpFile;				//independent check points
	std::string gcpType;				//gcp type : WGS-84 or Local
	GCPTYPE     eGcpType;
	std::string gcpLocGcdFile;			//Local Coordinates GCD File
	std::string gcpTransGcdFile;		//Transform GCD File of 7 parameters
	std::string gcpWgs84GcdFile;		//WGS-84 Coordinates GCD File 
	std::string gcpHeiType;				//Control Point Height System 
	HEITYPE     eGcpHeiType;
	std::string gcpEgmFile;				//EGM File for Control Point

	std::string demType;				//dem type : SRTM or Other
	DEMTYPE     eDemType;
	std::string addDemPath;				//additional dem file path
	std::string demLocGcdFile;			//Local Coordinates GCD File
	std::string demTransGcdFile;		//Transform GCD File of 7 parameters
	std::string demWgs84GcdFile;		//WGS-84 Coordinates GCD File 
	std::string demHeiType;				//DEM Height Type 
	HEITYPE     eDemHeiType;			
	std::string demEgmFile;				//EGM File for DEM

	std::string errHeader;
};


/*
	class:	ProjectInfo
	brief:	parser of Prject Information file
	time :	2013/12/07, 21:26
*/
class ProjectInfo
{
public:
	ProjectInfo(){	errHeader = "ProjectInfo::";}
	~ProjectInfo(){}

	void WriteProjFile(std::string projFile){
		try {
			FILE* fpPrj = fopen(projFile.c_str(), "wt");
			if (fpPrj == NULL)	throw std::runtime_error("Can not create project file! ");
			fprintf(fpPrj, "Project Path : \"%s\"",	projPath.c_str());
			fclose(fpPrj);	fpPrj = NULL;
		} catch (const std::runtime_error& e) {
			std::runtime_error err(errHeader + e.what());
			std::cerr << err.what() << std::endl;
			throw err;
		}
	}
public:
	std::string projPath;				//Project Path 
	std::string errHeader;
};


/*
	class:	ImgTskList
	brief:	
	time :	2013/12/07, 21:27
*/
class ImgTskList
{
public:
	typedef struct InputImgInfo{
		std::string srcPath;
		std::string mchPath;
		std::string outPath;
		int time;
		double sampSz, lineSz;
		int mdlId;
		std::vector<double> parMean;
		std::vector<double> parMse;
		struct InputImgInfo() : sampSz(0), lineSz(0), mdlId(0), time(0){}

		void InitForZy3Tlc(double lSz, std::string srcf, std::string mchf, std::string outf, int t = 0, double sSz = 0.0){
			srcPath = srcf;		mchPath = mchf;		outPath = outf;		lineSz = lSz;	time = t;	sampSz = sSz;
			mdlId = 1;	parMse.resize(6);	parMean.resize(6, double(0));
			parMse[0] = parMse[3] = 30;
			parMse[1] = parMse[2] = parMse[4] = parMse[5] = 2e-4;
		}

		void InitForZy3Mux(int mdl, double lSz, std::string srcf, std::string mchf, std::string outf, int t = 0, double sSz = 0.0){
			srcPath = srcf;		mchPath = mchf;		outPath = outf;		lineSz = lSz;	time = t;	sampSz = sSz;
			mdlId = mdl;
			if (mdlId == 1){
				parMse.resize(6);	parMean.resize(6, double(0));
				parMse[0] = parMse[3] = -1;
				parMse[1] = parMse[2] = parMse[4] = parMse[5] = 2e-4;
			} else if (mdlId == 2) {
				parMse.resize(8);	parMean.resize(8, double(0));
				parMse[0] = parMse[3] = -1;
				parMse[1] = parMse[2] = parMse[4] = parMse[5] = 5e-4;
				parMse[6] = parMse[7] = 1e-5;
			}
		}

		void InitForJb(int mdl, double lSz, std::string srcf, std::string mchf, std::string outf, int t = 0, double sSz = 0.0){
			srcPath = srcf;		mchPath = mchf;		outPath = outf;		lineSz = lSz;	time = t;	sampSz = sSz;
			mdlId = mdl;
			if (mdlId == 1){
				parMse.resize(6);	parMean.resize(6, double(0));
				parMse[0] = parMse[3] = parMse[4] = -1;
				parMse[1] = parMse[2] = parMse[5] = 2e-2;
			} else if (mdlId == 3) {
				parMse.resize(10);	parMean.resize(10, double(0));
				parMse[0] = parMse[3] = parMse[4] = -1;
				parMse[1] = parMse[2] = parMse[5] = 2e-2;
				parMse[6] = parMse[7] = parMse[8] = parMse[9] = -1;
			}
		}
	}InputImgInfo;

	enum IMGPTCOORD {LEFTUP = 0, LEFTDOWN = 1};
public:
	ImgTskList(){ 
		errHeader = "ImgTskList::";	
		eIpCoord = LEFTDOWN;
		ipCoord = "LEFTDOWN";
	}
	~ImgTskList(){}

	void SetImgPointCoord(IMGPTCOORD ipc = LEFTDOWN){
		eIpCoord = ipc;
		if (ipc == LEFTDOWN)	ipCoord = "LEFTDOWN";
		else					ipCoord = "LEFTUP";
	}

	void WriteImgListFile(std::string listFile){
		try {
			FILE* fpLst = fopen(listFile.c_str(), "wt");
			if (fpLst == NULL)	throw std::runtime_error("Can not create image list file! ");
			fprintf(fpLst, "models : %d\n", mdl.size());
			for (int i = 0; i < mdl.size(); ++i){
				fprintf(fpLst, "\"%s\"  \"%s\"  \"%s\"  %d  %d  %d  %d  ", mdl[i].srcPath.c_str(), mdl[i].mchPath.c_str(), mdl[i].outPath.c_str(), 
					mdl[i].time, int(mdl[i].sampSz), int(mdl[i].lineSz), mdl[i].mdlId);
				for (int ci = 0; ci < mdl[i].parMean.size(); ++ci)
					fprintf(fpLst, "%e  %e  ", mdl[i].parMean[ci], mdl[i].parMse[ci]);
				fprintf(fpLst, "\n");
			}			
			fclose(fpLst);	fpLst = NULL;
		} catch (const std::runtime_error& e) {
			std::runtime_error err(errHeader + e.what());
			std::cerr << err.what() << std::endl;
			throw err;
		}
	}

public:
	std::vector<InputImgInfo>	mdl;
	std::string					ipCoord;
	IMGPTCOORD					eIpCoord;

	std::string					errHeader;
};


/*
	class:	MipAdjTskWriter
	brief:	
	time :	2013/12/13, 16:14
*/
class MipAdjTskWriter
{
public:
	MipAdjTskWriter()	{	errHeader = "MipAdjTsk::";	}
	~MipAdjTskWriter(){}

	void SetTaskInfo(std::string tskPath, std::string tskName, bool isCombine){
		if (tskPath.at( tskPath.size()-1 ) != '\\')	tskPath = tskPath + "\\";
		path = tskPath;	name = tskName;	isComb = isCombine;
		if (isCombine)	comb = "TRUE";
		else			comb = "FALSE";
	}

	void Write(){
		try {
			std::string ctiPth = path + name + ".cti";
			std::string projPth = path + name + ".prj";
			std::string imgLstPth = path + name + ".imglist";
			std::string tskPth = path + name + ".tsk";

			FILE* fpTsk = fopen(tskPth.c_str(), "wt");
			if (fpTsk == NULL)	throw std::runtime_error("Can not create task file! ");
			fprintf(fpTsk, "Control Information : \"%s\"\n", ctiPth.c_str());
			fprintf(fpTsk, "Project Information : \"%s\"\n", projPth.c_str());
			fprintf(fpTsk, "Image List File : \"%s\"\n", imgLstPth.c_str());
			fprintf(fpTsk, "Image Point Coordinates : \"%s\"\n", img.ipCoord.c_str());
			fprintf(fpTsk, "Is Coefficient Combine  : \"%s\"\n", comb.c_str());
			fclose(fpTsk);

			ci.WriteCtiFile(ctiPth);
			pi.WriteProjFile(projPth);
			img.WriteImgListFile(imgLstPth);

		} catch (const std::runtime_error& e) {
			std::runtime_error err(errHeader + e.what());
			std::cerr << err.what() << std::endl;
			throw err;
		}
	}
	
public:
	std::string errHeader;
	ControlInfo ci;
	ProjectInfo pi;
	ImgTskList  img;
	std::string	comb;
	bool		isComb;
	std::string path;
	std::string name;
};



}


#endif