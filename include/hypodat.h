

#ifndef HYPODAT_H
#define HYPODAT_H


#include "wx/wx.h"
#include "hypobase.h"
#include <vector>



class ModDat{
public:
	int model;
	//Model *mod;
};


class EvoDat{
	float *params;
};


class FitMeasure{
public:
	wxString tag, label;
	int index;
	double score;
	double weight;
	bool use;

	FitMeasure() {
		use = true;
		weight = 0;
		score = 0;
		tag = "";
	};

	FitMeasure(wxString newtag, wxString newlabel, double newweight) {
		use = true;
		score = 0;
		tag = newtag;
		label = newlabel;
		weight = newweight;
	};
};


class FitCon{
public:
	wxString tag, label;
	double value;

	FitCon() {
		tag = "";
		label = "";
		value = 0;
	};

	FitCon(wxString newtag, wxString newlabel, double newvalue) {
		tag = newtag;
		label = newlabel;
		value = newvalue;
	};
};


class FitConSet{
public:
	int count, max;
	ParamStore ref;
	wxString *tags;
	FitCon *cons;
	//ParamStore fitcon;
	//double *values;

	FitConSet(int size);
	~FitConSet();

	void AddCon(wxString tag, wxString label, double value);
	FitCon GetCon(wxString tag);
};


class FitSet{
public:
	int measureCount, measureMax;
	ParamStore ref;
	wxString *tags;
	FitMeasure *measures;

	FitSet(int size);
	~FitSet();
	void AddMeasure(wxString tag, wxString label, double weight);
	FitMeasure GetMeasure(wxString tag);
};


class FitDat{
public:
	ParamStore scores;
	double haz5chisq;
	double burstheadchisq;
	double intrafreq;
	double ISImode;
	double ISImean;
	double RMSFirstNBins;
	double RMSBinRange;
	double RMSHaz;
	double RMSIoD;
	double RMSFirstNBinsBurst;
	//double IoD;
	double burstmode;
	double burstlengthmean;
	double burstlengthsd;
	double burstsilencemean;
	double burstsilencesd;
	double burstintrafreq;
	double score;
	double scoresum;
	double weightsum;

	/*FitDat::FitDat() {
		scores = ParamStore;
	}*/

	//FitSet *fitset;
	//FitDat::FitDat(FitSet *fs) {
	//	fitset = fs;
	//};
};


class NeuroDat{
public:
	//double *times;
	//int *srate;
	//int *srate10;

	std::vector<double> times;
	std::vector<int> srate;
	std::vector<int> srate10;

	int srate100[10000];
	int count;
	int spikecount;
	int isicount;
	int netflag;
	int type;
	wxString name;
	static const int maxtime = 100000;
	static const int maxspikes = 100000;
	
	double freq;
	double meanisi;
	double isivar;
	
	NeuroDat();
	~NeuroDat();
	void ratecalc();
	void ratereset();
};


class SpikeDatTest{
public:
	SpikeDatTest();
	~SpikeDatTest();

	DiagBox *diagbox;

	/*
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

	datdouble hist1norm;
	datdouble hist5norm;
	//datdouble haz1norm;
	//datdouble haz5norm;

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
	void IntraSelectAnalysis();
	void FitScore(SpikeDat *, FitDat *);
	void BurstProfile();
	int GraphSet(GraphBase *, wxString, int, int light = 0, wxString reftag="", wxString btag="Intra-Burst ");
	int GraphSetLysis(GraphBase *, wxString, int, int light = 0, wxString reftag="", wxString btag="Intra-Burst ");
	void Clear();
	void ReSize(int);*/
};


class SpikeDat{
public:
	SpikeDat();
	~SpikeDat();

	DiagBox *diagbox;

	datdouble times;
	datdouble mag;
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

	datdouble hist1norm;
	datdouble hist5norm;
	//datdouble haz1norm;
	datdouble haz5norm;

	datint srate;      // change to srate1s
	datint srate1;
	datint srate100;
	datint srate10;
	datint srate10s;
	datint srate100s;
	datdouble synsim;
	datdouble vasomean;
	datdouble winfreq;
	//datint autocorr;
	//double *inputrec;
	datdouble netinputrec;
	datdouble raterec;
	datdouble IoDdata;
	datdouble IoDdataX;

	datdouble meanV;

	// BurstProfile arrays
	double prosum[1000], prosumtail[1000];
	int procount[1000], procounttail[1000];

	double threshrec[10000];
	int *rate;
	int graphindex;

	int normscale;
	int binsize;
	
	int id;
	int count;
	int spikecount;
	int isicount;
	//int index[100000];
	int maxtime;
	int maxspikes;
	char name[100];
	double start;
	double freq;              // mean frequency of detected spikes
	double spikefreq;         // mean frequency over run time
	double meanisi;
	double isivar;
	double isisd;
	double histquadmode;
	short freqwindow;
	int hazquadbins;
	bool graphs;
	
	double memtau, tauHAP, tauAHP, tauDAP;
	double kHAP, kAHP, kDAP;
	double th0, vrest;
	MainFrame *mainwin;

	int fitType; 
	bool fitdiag;
	wxString label;
	
	void datacalc();
	void neurocalc(NeuroDat *datneuron = NULL, ParamStore *calcparams = NULL);
	void neurocalcBasic(NeuroDat *datneuron = NULL, ParamStore *calcparams = NULL);
	double dispcalc(int binsize);
	void quadhist();
	void ISIanalysis();
	//void autocalc();
	void netneurocalc(int);
	void output(wxString, wxString);
	void inputsim(double);
	void BurstScan(BurstBox *);
	void BurstScanFit(ParamStore *);
	void IntraSelectAnalysis();
	void FitScore(SpikeDat *, FitDat *, FitSet *, FitConSet *, ParamStore *burstparams = NULL);
	void FitScoreBasic(SpikeDat *, FitDat *, FitSet *, FitConSet *);
	void FitScoreOxy(SpikeDat *, FitDat *, FitSet *, FitConSet *);
	void FitScoreVaso(SpikeDat *, FitDat *, FitSet *, FitConSet *, ParamStore *);
	void FitScoreVasoFast(SpikeDat *, FitDat *, FitSet *, ParamStore *, ParamStore *);
	//void FitScoreOxy(SpikeDat *, FitDat *);
	void BurstProfile();
	void MeanSpikeForm(datdouble V, int timerange, int filter = false, int substeps = 1);
	int GraphSet(GraphBase *, wxString, int, int light = 0, wxString reftag="", wxString btag="Intra-Burst ");
	int GraphSetLysis(GraphBase *, wxString, int, int light = 0, wxString reftag="", wxString btag="Intra-Burst ");
	void Clear();
	void ReSize(int);
};


class IoDDat{
public:
	int *srate;
	double mean, variance, dispersion;
	int laststep;
	int binsize;
	SpikeDat *spikedata;

	IoDDat(int binsize, SpikeDat *);
	~IoDDat();
	void ratecalc();
	double dispcalc();
};


class CurrentDat{
public:
	datdouble AHP;
	datdouble DAP;
	datdouble DAP2;
	datdouble I;
	double *val;
	
	CurrentDat();
};


class VarDat{
public:
	double halflife;
	double tau;
	double k;
	//datdouble store;
};



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


class BurstDat{
public:
	SpikeDat *spikedata;
	wxString snum;
	int burstdisp;
	int maxbursts;
	double maxtime;
	bool selectmode;
	
	datdouble haz1;
	datdouble hist1;
	datdouble hist1norm;
	datdouble haz5;
	datdouble hist5;
	datdouble hist5norm;
	//datdouble spikes;
	datdouble times;
	datdouble profile;
	datdouble tailprofile;
	datdouble bursthaz;
	datdouble length;
	datdouble profilesm;
	datdouble tailprofilesm;
	datdouble IoDdata;
	datdouble IoDdataX;

	int *spikes;
	int *burstspikes;

	//burst *bustore;
	std::vector <burst> bustore;

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
	int test;
	
	BurstDat(bool select=false);
	~BurstDat();
	int spikeburst(int);
	void IntraBurstAnalysis();
	void FitAnalysis();
	//void Scan(BurstBox *);
};


class AnaDat{
public:
	SpikeDat *spikedata;
	wxString snum;
	
	datint autocorr;
	
	AnaDat();
	void autocalc(SpikeDat *);
};


class GraphScale
{
public:
	double xf;
	double xt;
	double yf;
	double yt;

	GraphScale(double newxf, double newxt, double newyf, double newyt) { xf = newxf; xt = newxt; yf = newyf; yt = newyt; };
};


class TypeSet
{
public:
	int numtypes;
	int refindex[20];
	int typeindex[20];
	wxString names[20];

	TypeSet() {
		numtypes = 0; 
	};

	void Add(wxString, int);
	int GetIndex(int);
	int GetType(int);
	wxString List();
};


class PlotDat: public ModDat
{
public:
	wxString gtag;
	int ycol;
	int xcol;
	int gtype;

	PlotDat(wxString, int, int, int);
	wxString StoreDat(int);
};


class PlotSet
{
public:
	int plotcount;
	vector<PlotDat>plotdata;
	int dispindex;

	PlotSet() {
		dispindex = 0;
		plotcount = 0;
	};

	void AddPlot(PlotDat);
	int RemovePlot(int);
	int SetPlot(int, PlotDat);
};


class GraphDat{                               // Basic single graph class
public:
	GraphDat();
	GraphDat(datdouble *, double, double, double, double, wxString, int type, double bin = 1, int colour = red, int xscale = 1, int xdis = 0);
	GraphDat(datint *, double, double, double, double, wxString, int type, double bin = 1, int colour = red);
	GraphDat(datint *, double, double, double, double, wxString, SpikeDat *, double bin = 1, int colour = red);

	DiagBox *diagbox;

	int gparam;
	int *gdata;
	short gindex;
	double *gdatad;
	datdouble *gdatadv;
	datint *gdatav;
	datdouble *gdatax;
	PlotDat *plotdata;
	int datatype;    // proposed replacement for gparam, not yet implemented 11/7/18

	int xcount, ycount;
	int scattermode;
	int linemode;
	double scattersize;

	double yfrom;
	double yto;
	double xfrom;
	double xto;
	bool oldset;
	double oldyfrom, oldyto, oldxfrom, oldxto;
	double binsize;
	wxString gname;
	wxString gtag;
	wxString xtag, ytag;
	int samprate;    // 0 for no limit
	int xstop;
	int type;        // 0 for normal, 1 for histogram    // 26/2/15 Update this!

	// 1 for histogram
	// 2 for XY line plot + scatter option
	// 4 or 5 for line (5 is new scaling fixed version)
	// 7 for bar
	// 6 line with sampling
	// 8 scatter with sampling
	// 3 for spike rate
	// 9 for XY bar plot

	int pos;         // graph position
	int dataset;
	int colour;
	int sdex;
	double xmax;
	int scrollpos;
	double xscale, xshift, xsample;
	double xdis;
	int negscale;
	int burstdisp;
	int xlabels, ylabels; 
	double xstep, ystep;
	int xtickmode, ytickmode;         
	int xlabelplaces, ylabelplaces;        // -1 for automatic
	int xlabelmode, ylabelmode;            // New November 2017  0 - none | 1 - normal | 2 - only end labels
	double xticklength, yticklength;
	double plotstroke;
	wxColour strokecolour, fillcolour;
	int xplot, yplot;
	double xlabelgap, ylabelgap;
	double labelfontsize, tickfontsize;
	double xunitscale, xunitdscale;
	double yunitscale, yshift;
	int clipmode;
	bool synchx;
	double barwidth, barshift, bargap;
	int xscalemode, yscalemode;
	double xlogbase, ylogbase;
	int scatterfield, scattermean;      // used for drawing scatter cloud oval and mean


	int highstart, highstop, highcolour;

	BurstDat *burstdata;
	SpikeDat *spikedata;

	wxString StoreDat(wxString tag);
	void LoadDat(wxString, int version);
	void Init();
};



class GraphBase;
class GraphDisp;


/* 

GraphSet used to store set of graphs such that the set makes a single menu entry and the displayed graph can be selected using scale panel buttons

*/


class GraphSet{                   // Stores a set of GraphDat objects using an array of their GraphBase indexes
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
	wxString modeflag[20];          // Set of flags is used to control the selected, displayed graph 
	int modeweight[20];
	int capacity;

	DiagBox *diagbox;
	wxString text;

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


class GraphBase{            // Dual purpose serial storage database for all GraphDat objects and all GraphSet objects
public:
	int numsets;
	int setstoresize;
	int baseversion;
	short numgraphs;
	short storesize;
	wxString initpath;
	//int datdex[10];
	wxString indextag[1000];
	wxString setindextag[1000];
	std::vector <GraphDat> graphstore;
	std::vector <GraphSet> setstore;
	ParamStore tagindex;
	ParamStore nameindex;
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
		baseversion = 2;
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

	GraphDat *GetGraph(wxString);
	GraphDat *GetGraphFromName(wxString);
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


class GraphDisp{             // used for multiple layered plots for simultaneous display
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
	GraphDat *plot[10];
	static const int maxplots = 10;
	int numplots;
	int currentplot;
	int spikedisp;
	int sdex;
	//BurstDat *burstdata;
	PlotSet *plotset;
	
	GraphDisp();
	void Add(GraphDat *);
	void Remove(int index);
	void Front(GraphDat *);
	GraphDat *GetFront();
	void Set(int index, GraphDat *newplot);   // Set(0, newplot) equivalent to Front(newplot)
	void XYSynch(GraphDat *graph = NULL);
};


class FileDat{
public:
	wxString name;
	wxString path;
	int index;

	FileDat() {
		name = "";
		path = "";
		index = -1;
	};

	FileDat(wxString fname, wxString fpath) {
		name = fname;
		path = fpath;
		index = -1;
	};

	bool Compare(FileDat test) {
		if(name == test.name && path == test.path) return true;
		else return false;
	}

	bool operator==(FileDat test)  {
		return Compare(test);
	};

	wxString String() {
		return path + "\\" + name;
	}
};


class FileBase{
public:
	int numfiles;
	wxString name[100];
	wxString path[100];
	bool newentry;

	std::vector <FileDat> filestore;

	FileBase(int size) {
		filestore.resize(size);
		numfiles = 0;
		newentry = false;
	};

	FileDat *Add(FileDat newfile) {
		FileDat *check = Find(newfile);
		if(check) return check;
		filestore[numfiles] = newfile;
		filestore[numfiles].index = numfiles;
		newentry = true;
		return &(filestore[numfiles++]);
	}

	FileDat *Find(FileDat file) {
		for(int i=0; i<numfiles; i++) 
			if(filestore[i] == file) return &(filestore[i]);
		return NULL;
	};

	FileDat operator[](int index) {
		if(index < 0) index = 0;
		return filestore[index];
	};

	void Store(FileDat);
	void Load(FileDat);
};



double IoDcalc(int binsize, int spikecount, datdouble *spiketimes);



#endif
