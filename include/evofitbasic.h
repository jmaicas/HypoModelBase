
/*
*  evofitbasic.h
*  
*  Created by Duncan MacGregor 
*  University of Edinburgh 2016
*  Released under MIT license, see https://opensource.org/licenses/MIT
*
*/


#ifndef EVOFITBASIC_H
#define EVOFITBASIC_H


#include "wx/wx.h"
//#include <hypomodel.h>
//#include "vasodat.h"
#include "evodat.h"
#include "hypomods.h"


enum {
	ID_chromespin = 1,
	ID_fitscore,
	ID_evofitscore,
	ID_loadfitscore,
	ID_EvoRunRamp
};


class EvoFit;


class ParentDat{
public:
	int chromeindex;
	vector<float> times;
	int spikecount;

	ParentDat() {
		spikecount = 0;
		times.resize(20000);
	};
};


class SpikeFitDat{
public:
	bool allocated;
	int chromecount;
	//float *Ints;
	vector<float> Ints;
	vector<float> ISIs;
	vector<float> Bursts;
	vector<float> BurstMean;
	vector<float> BurstSD;
	vector<float> SilenceMean;
	vector<float> SilenceSD;
	vector<float> IntraFreq;
	vector<float> ExtraFreq;
	vector<float> SpikeCounts;

	bool extracount;
	int spikecounts[200];

	//datdouble ISIhist;
	//datdouble burstprof;

	SpikeFitDat() { 
		chromecount = 0;
		allocated = false;
		//ISIhist.setsize(1000);
		//burstprof.setsize(1000);
		extracount = false;
		//spikecounts = new int[200];
		//Ints = NULL;
		//ISIs = NULL;
		//Bursts = NULL;
		//BurstMean = NULL;
		//BurstSD = NULL;
		//SilenceMean = NULL;
		//SilenceSD = NULL;
		//IntraFreq = NULL;
		//ExtraFreq = NULL;
		//SpikeCounts = NULL;
	};

	~SpikeFitDat();

	void DeAllocate();
	void Select(int);
};


class FitPanel
{
public:
	wxPanel *panel;
	SpikeDat *spikedata;

	wxStaticText *histfirstrms, *histfirstrmsWeight;
	wxStaticText *histrangerms;
	wxStaticText *hazrms;
	wxStaticText *hazheadrms;
	wxStaticText *iodrms;

	wxStaticText *burstfirstrms;
	wxStaticText *burstmode;
	wxStaticText *burstlengthmean;
	wxStaticText *burstlengthsd;
	wxStaticText *burstsilencemean;
	wxStaticText *burstsilencesd;
	wxStaticText *burstintrafreq;

	wxStaticText *lengthmean;
	wxStaticText *silencemean;
	wxStaticText *intrafreq;

	wxStaticText *weightscore;
};


class EvoFitBox: public ParamBox
{
public:	
	Model *mod;
	//VasoEvoFit *fitthread;

	TagBox *datfiletag;
	wxStaticText *datstatus;

	//DiagBox *diagbox;
	//wxStaticText *hazchisquare;

	EvoFit *evothread;

	FitPanel *modpanel;
	FitPanel *evopanel;
	DatPanel *fitdatapanel;

	/*
	wxStaticText *histfirstrms;
	wxStaticText *histrangerms;
	wxStaticText *hazrms;

	wxStaticText *burstfirstrms;
	wxStaticText *burstmode;
	wxStaticText *burstlengthmean;
	wxStaticText *burstlengthsd;
	wxStaticText *burstsilencemean;
	wxStaticText *burstsilencesd;
	wxStaticText *burstintrafreq;*/

	wxStaticText *lengthmean;
	wxStaticText *silencemean;
	wxStaticText *intrafreq;
	wxStaticText *spikes;
	wxCheckBox *seedcheck;

	// Data Panel
	wxStaticText *expspikes;
	wxStaticText *expfreq;
	wxStaticText *expbursts;

	// Control flags
	bool autoscore;
	bool burstmode;
	int burstfitmode;
	wxToggleButton *burstToggle;

	// Spike Data links
	SpikeDat *moddata;
	SpikeDat *expdata;
	SpikeDat *loaddata;
	SpikeDat *evodata;
	vector<SpikeDat> evodataset;

	// Fit Data Links
	SpikeFitDat *spikefitdata;
	BurstBox *burstbox;

	// Parent Data
	int parentcount;
	vector<ParentDat> parentdata;

	// Fit Mechanism Storage

	int popsize, parentrange;
	vector<EvoChrome> chromepop;
	vector<EvoChrome> chromeresult;
	EvoChrome *fitchrome;
	bool chromepopinit;
	int numparams;
	int chromeindex;
	int rescount;

	// Fit Analysis Storage
	double parammean[50][50];
    double paramsd[50][50];
	double fitmean[50];
	double fitsd[50];

	wxCheckBox **fitchecks;
	wxStaticText *chromedex;
	wxStaticText *chromefit;
	wxStaticText *datchrome;
	wxSpinButton *chromespin;
	wxStaticText *runstatus;
	//wxButton *fitbutton;
	FitDat *fitdata, *evofitdata;
	FitDat *loadfitdata;
	FitSet *fitset;
	FitConSet *fitconset;

	EvoFitBox(Model *mod, EvoChrome *, const wxString& title, const wxPoint& pos, const wxSize& size, bool burstmode = false);	
	void OnFitScore(wxCommandEvent& event);
	void OnEvoRun(wxCommandEvent& event);
	void OnToggle(wxCommandEvent& event);

	wxFlexGridSizer *FitScorePanel(FitPanel *);
	wxFlexGridSizer *FitDataPanel(FitSet *, DatPanel *);
	wxFlexGridSizer *FitWeightPanel(FitSet * = NULL);
	wxFlexGridSizer *FitConPanel(FitConSet * = NULL);
	void CreatePanels();
	void FitChromeUpdate();
	void FitWeightUpdate();
	void FitControlUpdate();
	void ChromeData();
	void ChromeParams(EvoChrome *);
	void FitDisp(FitDat *data = NULL, FitPanel *fitpanel = NULL);
	void FitDispNew(FitDat *data = NULL, DatPanel *fitpanel = NULL);
	void ModFitScore();
	void ExpDataDisp();
	void SetToggle(bool state, int ID=ID_Burst);


	void OnSpinNext(wxSpinEvent& event);
	void OnSpinPrev(wxSpinEvent& event);
	void OnChromeNext();
	void OnChromePrev();

	void OnDataLoad(wxCommandEvent& event);

	~EvoFitBox();
};



class EvoFit : public wxThread
{
public:
	Model *mod;
	EvoFitBox *fitbox;
	DiagBox *diagbox;

	vector<EvoChrome> *chromepop;
	vector<EvoChrome> *chromeresult;
	EvoChrome *nextgen;
	float *chromearray;
	EvoChrome *chrome;

	EvoChrome *fitchrome;
	int chromeparams;

	int popsize;
	int parentrange;
	int generations;
	//int numparams;
	int numruns;
	double mutateprob;
	double dualfit;
	bool burstmode;
	TextFile ofp;
	bool genmon;
	bool genlysis;
	bool seedgen;
	long evoseed;

	int blocksize;
	int gpuparams, maxparams;
	float runtime;
	int *threaddata;
	bool multirun;
	int gpudevice;
	int scorethreads;

	EvoFit(Model *, EvoFitBox *);
	virtual void InitPop();
	void quicksort(vector<EvoChrome> *a, int low, int high);
	int partition(vector<EvoChrome> *a, int low, int high, int pivotindex);
	virtual void Evaluate(int start, int pop, double dual=0);
	virtual void Evolve();
	void AddChromeParam(int, double);
	void ResultChromeOutput();

	virtual void *Entry();
};




#endif