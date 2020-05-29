

/*
*  evodat.h
*  
*  Created by Duncan MacGregor 
*  University of Edinburgh 2016
*
*/


#ifndef EVODAT_H
#define EVODAT_H


//#include <hypomodel.h>
#include "hypodat.h"
#include "hypotools.h"

class ParamBox;


class EvoParam
{
public:
	double value;
	double min;
	double max;
	bool adapt;
	int places;
	wxString label;
	ParamCon *paramcon;

	EvoParam(double, double, double, bool adapt=false, wxString label="", int places=2);
	EvoParam();
	void Generate();
};


class EvoChrome
{
public:
	int i;
	int index;
	int gen;
	int numparams, maxparams;
	ParamStore ref;
	ParamBox *parambox;

	DiagBox *diagbox;

	float *ISIhist;
	float *BurstProf;
	FitDat fitdata, fitdata2;

	double burstmean;
	double burstSD;
	double silencemean;
	double silenceSD;
	double intrafreq;
	double extrafreq;

	double fitness, fitness2;
	double fithead, fittail, fithaz, fitIoD;

	int dataindex;

	wxString tags[50];
	ParamCon *cons[50];
	EvoParam params[50];
	int AddParam(wxString, ParamCon *, double, double, bool adapt=false, wxString label = "", int places = 2);
	int AddParam(wxString, ParamCon *, EvoParam *);
	EvoParam GetParam(wxString);

	EvoChrome();
	~EvoChrome();
	void Initialise();
	void Output(wxString filename = "chrome.txt");
	void Output(TextFile *ofp, int mode = 0);
	void Diagnostic();
	wxString OutLine();
	double Val(wxString);
};


class EvoFitDat{
public:
	bool allocated;
	int chromecount;
	float *Ints;
	float *ISIs;
	float *Bursts;
	float *BurstMean;
	float *BurstSD;
	float *SilenceMean;
	float *SilenceSD;
	float *IntraFreq;
	float *ExtraFreq;

	datdouble ISIhist;
	datdouble burstprof;

	EvoFitDat() { 
		chromecount = 0;
		allocated = false;
		ISIhist.setsize(1000);
		burstprof.setsize(1000);
	};

	void Select(int);
};


/*
*
*	Associative array class, stores floating point numbers indexed by string
*
*/
/*

class ParamStore{
	struct ParamData{
		wxString indexName;
		double data;
	};
private:
	std::vector<ParamData> store;
	
public:
	unsigned long size() {
		return store.size();
	}
	void add(wxString indexName, double data){
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].indexName == indexName) {
				store[i].data = data;
				return;
			}                  
		ParamData param;
		param.indexName = indexName;
		param.data = data;
		store.push_back(param);
	}
	double &operator [](long index){
		if(index < 0) index=0;
		return store[index].data;
	}
	double &operator [](wxString indexName){
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].indexName == indexName) {
				return store[i].data;
			}
		long idx = store.size();
		ParamData param;
		param.indexName = indexName;
		store.push_back(param);  
		return store[idx].data;             
	} 
	int check(wxString indexName) {
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].indexName == indexName) return 1;
		return 0;
	}
};
*/

#endif