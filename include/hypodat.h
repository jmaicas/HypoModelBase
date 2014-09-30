// *************************************************************************************************
// *********************************      hypodat.h              ***********************************
//
// Defines classes for managing data of different measures, signals and graphs
//	1. ModDat ?
//	2. EvoDat ?
//	3. FitDat	Fitness state and its parameters.
//	4. NeuroDat	Basic data for neurosignal
//	5. SpikeDat Data for a spike signal with statistical measurements, AHP, KHP, DAP, ISIS, and burst for graphing.  
//	6. CurrentDat Concrete values AHP, DAP, Interval? for a spike?
//	7. VarDat	tau, k and halflife values for a spike
//	8. burst basic values for a burst
//	9. BurstDat	Complete data for a burst including SpikeDat class
//	10. AnaDat Analysis of Spike Data
//	11. GraphScale Time and Frequency scales for 2-D graphs
//	12. GraphDat	Collect data before drawing the graph
//	13. GraphSet	Offer the possibility of having a set of graphs ?
//	14. GraphBase	Basic Set of Graphs? Allow to Save and Load saved configurations of graphs?
//	15. graphdisp	Display a single graph?
//
// *************************************************************************************************


#ifndef HYPODAT_H
#define HYPODAT_H


#include "wx/wx.h"
#include "hypobase.h"
#include <vector>

//	1. ModDat ?
class ModDat{
public:
	int model;
};

//	2. EvoDat ?
class EvoDat{
	float *params;
};

//	3. FitDat	Fitness state and its parameters.
class FitDat{
public:
	double haz5chisq;
	double burstheadchisq;
	double intrafreq;
	double ISImode;
	double ISImean;
	double RMSFirstNBins;
	double RMSBinRange;
	double RMSHaz;
	double RMSFirstNBinsBurst;
	double burstmode;
	double burstlengthmean;
	double burstlengthsd;
	double burstsilencemean;
	double burstsilencesd;
	double burstintrafreq;
	double score;
	double scoresum;
	double weightsum;
};

//	4. NeuroDat	Basic data for neurosignal
class NeuroDat{
public:
	double times[100000];
	short srate[100000];
	int count;
	int spikecount;
	int isicount;
	int netflag;
	int type;
	static const int maxtime = 100000;
	
	double freq;
	double meanisi;
	double isivar;
	
	//void neurocalc();
	void ratecalc();
};

//	5. SpikeDat Data for a spike signal with statistical measurements, AHP, KHP, DAP, ISIS, and burst for graphing.  
class SpikeDat{
public:
	SpikeDat();
	~SpikeDat();
	datdouble times;
	datdouble isis;
	BurstDat *burstdata;
	
	datdouble haz1;
	datdouble haz5;
	datdouble hazquad;
	datdouble hist1;
	datdouble hist5;
	datdouble histquad;
	datdouble histquadx;
	datdouble histquadsm;
	datdouble histquadlin;
	datint srate;
	datint srate1;
	datint srate100;
	datint srate10;
	datdouble synsim;
	datdouble vasomean;
	datdouble winfreq;
	//datint autocorr;
	
	//double *inputrec;
	datdouble netinputrec;
	datdouble raterec;
	double threshrec[10000];
	int *rate;
	int graphindex;
	
	int id;
	int count;
	int spikecount;
	int isicount;
	//int index[100000];
	int maxtime;
	int maxspikes;
	char name[100];
	double start;
	double freq;
	double meanisi;
	double isivar;
	double isisd;
	double histquadmode;
	short freqwindow;
	int hazquadbins;
	
	double memtau, tauHAP, tauAHP, tauDAP;
	double kHAP, kAHP, kDAP;
	double th0, vrest;
	MainFrame *mainwin;
	
	void datacalc();
	void neurocalc(NeuroDat *datneuron = NULL);
	void ISIanalysis();
	//void autocalc();
	void netneurocalc(int);
	void output(wxString, wxString);
	void inputsim(double);
	void BurstScan(BurstBox *);
	void IntraBurstAnalysis();
	void FitScore(SpikeDat *, FitDat *);
	void BurstProfile();
	int GraphSet(GraphBase *, wxString, int, int, int light = 0, wxString reftag="", wxString btag="Intra-Burst ");
	int GraphSetLysis(GraphBase *, wxString, int, int, int light = 0, wxString reftag="", wxString btag="Intra-Burst ");
	void Clear();
	void ReSize(int);
};

//	6. CurrentDat Concrete values AHP, DAP, Interval? for a spike?
class CurrentDat{
public:
	datdouble AHP;
	datdouble DAP;
	datdouble DAP2;
	datdouble I;
	double *val;
	
	CurrentDat();
};

//	7. VarDat	tau, k and halflife values for a spike
class VarDat{
public:
	double halflife;
	double tau;
	double k;
	//datdouble store;
};

//	8. burst basic values for a burst
class burst{
public:
	int start;
	int end;
	int count;
	double time;
	int numpulse;
	double pmax;
	int length;
};

//	9. BurstDat	Complete data for a burst including SpikeDat class
class BurstDat{
public:
	SpikeDat *spikedata;
	wxString snum;
	int burstdisp;
	double maxtime;
	
	datdouble haz1;
	datdouble hist1;
	datdouble haz5;
	datdouble hist5;
	burst bustore[20000];
	datdouble spikes;
	datdouble times;
	datdouble profile;
	datdouble tailprofile;
	datdouble bursthaz;
	datdouble length;
	datdouble profilesm;
	datdouble tailprofilesm;

	//datdouble tailspikesum;
	datdouble tailmean;
  datdouble tailstdev;
  datdouble tailstder;
  datint tailnum;
	
	int intraspikes;
	int numbursts;
	double meancount;
	double meantime;
	double meanlength;
	//double meanburstdur;
	double meansilence;
	double sdlength;
	double sdsilence;
	double freq;
	double meanisi;
	double isivar;
	double isisd;
	double actQ;
	double pmoderate;
	double pmodetime;
	int pnzcount;
	
	BurstDat();
	//void Scan(BurstBox *);
};

//	10. AnaDat Analysis of Spike Data
class AnaDat{
public:
	SpikeDat *spikedata;
	wxString snum;
	
	datint autocorr;
	
	AnaDat();
	void autocalc(SpikeDat *);
};

//	11. GraphScale Time and Frequency scales for 2-D graphs
class GraphScale
{
public:
	double xf;
	double xt;
	double yf;
	double yt;

	GraphScale(double newxf, double newxt, double newyf, double newyt) { xf = newxf; xt = newxt; yf = newyf; yt = newyt; };
};

//	12. GraphDat	Collect data before drawing the graph
class GraphDat{
public:
	GraphDat();
	GraphDat(datdouble *, double, double, double, double, wxString, int type, double bin = 1, int colour = red, int xscale = 1, int xdis = 0);
	GraphDat(datint *, double, double, double, double, wxString, int type, double bin = 1, int colour = red);
	GraphDat(datint *, double, double, double, double, wxString, SpikeDat *, double bin = 1, int colour = red);
	int gparam;
	int *gdata;
	short gindex;
	double *gdatad;
	datdouble *gdatadv;
	datint *gdatav;
	double yfrom;
	double yto;
	double xfrom;
	double xto;
	double binsize;
	wxString gname;
	int samprate;    // 0 for no limit
	int type;        // 0 for normal, 1 for histogram
	int pos;         // graph position
	int dataset;
	int colour;
	int sdex;
	double xmax;
	int scrollpos;
	double xscale;
	double xdis;
	int negscale;
	int burstdisp;
	BurstDat *burstdata;
	SpikeDat *spikedata;

	wxString StoreDat();
	void LoadDat(wxString);
};



class GraphBase;
class graphdisp;

//	13. GraphSet	Offer the possibility of having a set of graphs ?
class GraphSet{
public:
	int sdex;
	wxString tag;
	wxString name;
	GraphBase *graphbase;
	//graphdisp *gpos;
	int numgraphs;
	int numflags;
	int gindex[20];
	int gpos;
	int gcodes[20];
	int modesum;
	bool single;
	wxString modeflag[10];
	int modeweight[10];

	GraphSet() {};

	/*
	GraphDat *operator[](wxString tag) {
		int index = tagindex[tag];
		if(index < 0) index = 0;
		//if(index >= numgraphs) return NULL;
		return &(graphstore[index]);
	};*/

	GraphSet(GraphBase *gbase, int gdex = -1);

	void Add(int gdex, int gcode=-1);
	void Add(wxString tag, int gcode=-1);
	void AddFlag(wxString, int);
	int GetPlot(ParamStore *);
	wxString Display();
};

//	14. GraphBase	Basic Set of Graphs? Allow to Save and Load saved configurations of graphs?
class GraphBase{
public:
	int numsets;
	int setstoresize;
	short numgraphs;
	short storesize;
	wxString initpath;
	//int datdex[10];
	wxString indextag[1000];
	wxString setindextag[1000];
	std::vector <GraphDat> graphstore;
	std::vector <GraphSet> setstore;
	ParamStore tagindex;
	ParamStore settagindex;
	MainFrame *mainwin;
	//Model *mod;

	GraphBase(int size, int setsize=10) {
		//mod = model;
		graphstore.resize(size);
		setstore.resize(setsize);
		numsets = 0;
		numgraphs = 0;
		storesize = size;
		setstoresize = setsize;
		initpath = "Init/";
	};

	~GraphBase() {
		//delete graphstore;
	}

	GraphDat *operator[](int index) {
		if(index < 0) index = 0;
		//if(index >= numgraphs) return NULL;
		return &(graphstore[index]);
	};

	GraphDat *operator[](wxString tag) {
		int index = tagindex[tag];
		if(index < 0) index = 0;
		//if(index >= numgraphs) return NULL;
		return &(graphstore[index]);
	};

	wxString GetTag(int index) {
		return indextag[index];
	};

	wxString GetSetTag(int index) {
		return setindextag[index];
	};


	int Add(GraphDat newgraph, wxString tag, wxString settag = "", bool set=true);
	GraphSet *NewSet(wxString name, wxString tag);
	//int AddNewSet(wxString tag, int gdex);
	GraphSet *GetSet(wxString);
	GraphSet *GetSet(int);

	/*
	GraphDat &operator[](wxString tag) {
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].indexName == indexName) {
				return store[i].data;
			}
		long idx = store.size();
		ParamData param;
		param.indexName = indexName;
		store.push_back(param);  
		return store[idx].data;             
	} */  

	void BaseStore(wxString, wxString);
	void BaseLoad(wxString, wxString, wxTextCtrl *textbox = NULL);
};

//	15. graphdisp	Display a single graph?
class graphdisp{
public:
	/*int data;
	wxTextCtrl *yf;
	wxTextCtrl *yt;
	wxTextCtrl *xf;
	wxTextCtrl *xt;
	wxBitmapButton *yzoomout;
	wxBitmapButton *yzoomin;
	wxBitmapButton *xzoomout;
	wxBitmapButton *xzoomin;
	int ypos;
	int xpos;*/
	GraphDat *plot[5];
	int numplots;
	int currentplot;
	int spikedisp;
	int sdex;
	//BurstDat *burstdata;
	
	graphdisp();
	void Add(GraphDat *);
	void Front(GraphDat *);
	GraphDat *GetFront();
};


#endif