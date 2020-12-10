

#ifndef HYPOMODEL_H
#define HYPOMODEL_H


//#define HYPOSOUND 1
//#define CUDA 1 


#include "wx/wx.h"
#include "hypograph.h"
#include "hypoplot.h"
#include "hypopanels.h"
#include "hypomods.h"
#include "hypodat.h"
//#include <hypodef.h>
#include "wx/generic/printps.h"
#include "wx/generic/prntdlgg.h"

#ifdef HYPOSOUND
#include "hyposound.h"
#endif

using namespace std;


class HypoApp: public wxApp
{
public:
	int viewheight, viewwidth;
     wxString mainpath;

	virtual bool OnInit();
	void LoadPrefs();
};


class ModelDat
{
public:
	int index;
	wxString title;
	int modcount;

	void Set(int dex, wxString tit) {
		index = dex;
		title = tit;
	}
};


class ModelSet
{
public:
	int modcount; 

	ModelDat modeldat[50];

	ModelSet() {modcount = 0;}

	void Add(int dex, wxString tit) {
		modeldat[modcount].Set(dex, tit);
		modcount++;
	}

	int GetDex(int id) {
		for(int i=0; i<modcount; i++) if(modeldat[i].index == id) return i;
		return -1;
	}
};


class HypoMain: public MainFrame
{
public:
	//MainFrame *mainwin;

	//wxFrame *frame;
	//wxPoint mainpos;
	//wxSize screensize;
	//wxString snum;
	//wxBoxSizer *mainsizer;
	wxCommandEvent blankevent;

	int graph, numgraphs, yplot, xplot;
	int numdraw, numdraw2;  // number of graph panels (GraphWindow3 objects)
	int numdraw_set;  // new options set value, before graph panels updated
	int spikedisp;
	int screenx;
	int viewheight;
	int viewwidth;
	int ylabels;
	int xstretch;
	int datsample;
	int activemod;
	int startmod;
	int pinmode;
	int scaletype;
	TypeSet fontset;

	FILE *ofp;
	wxTextFile *wxofp;
	wxString outline; 
	wxString datapath, outpath, parampath;
    //wxString modpath; // mainpath;
	wxBoxSizer *graphsizer;
	ParamStore prefstore;
	wxPrintData *printdata;
	wxPageSetupDialogData *pageSetupData;

	int moddex;
	//ModelDat moddata[20];
	ModelSet modset;

	// Display
	//ScaleBox *scalebox;
	//GraphBox *graphbox;
	//ToolBox *toolset[10];
	//GraphWindow3 *graphwin[10];
	GraphDisp *gpos;
	GraphDat *greg;
	GraphBase *graphbase;
	DispWin *dispwin;
	GraphWindow3 *graphwin2[10];
	SpikeDat *focusdata;
	FileBase *filebase;
	//graphdisp *gpos2;

	// Tools and Analysis 
	InfoBox *infobox;
	BurstBox *burstbox;
	BurstDat *burstdata;
	SpikeDat *currentdata;
	AnaDat *analysisdata;
	SpikeDat *expdata, *expdata2, *selectdata;

	ParamStore *hypoflags;
	RefStore *flagrefs;

	ProtocolBox *protocolbox;

	// Plot Data
	


	//DiagBox *diagbox;

	// IGF Model
	//ParamBox *igfbox;
	//ParamStore *igfparams;
	//ParamStore *igfmodflags;
	//SpikeDat *igfdata;
	SpikeDat *realdata;
	//CurrentDat *currdata;

	// Corticoptroph Model
	//CortBox *cortbox;
	//CortDat *cortdata;


	// New Model Modules
	Model *mod;

	HypoMain(const wxString& title, const wxPoint& pos, const wxSize& size, wxString path="");
	~HypoMain();

	//void OnPaint2(wxPaintEvent& event);
	void SetStatus(wxString);
    void DiagText(wxString);
	void DispParam(ParamStore *, wxString);
	//void GraphData();
	void RunModel(ParamBox *);
	//void RunCortModel(CortBox *);
	void GraphUpdate();
	void GraphOut();
	//void CortOutput();
	//void VMHOutput();
	void OptionStore();
	void OptionLoad();
	void ViewLoad();
	void ViewStore();

	void ToolLoad(Model *);
	void SpikeModule(Model *);
	//void SpikeBox(int modmode = 0);
	void BurstModule(Model *, int mode);
	//void EvoSpikeModule(Model *);
	void SoundModule(Model *);
	void PlotModule(Model *);

	void BasicMenu();
	void FullMenu();
	void UserMenu();
	void ModInit();
	void CleanUp();
	void AddGraph();
	void RemoveGraph(int);
	void GraphPanelsUpdate();
	void SizeUpdate();

	void OnFlag(wxCommandEvent& event);
	void SetMenuFlag(int, wxString, wxString, int state, wxMenu *menu); 

	void OnMove(wxMoveEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnDClick(wxMouseEvent& event);

	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnParams(wxCommandEvent& event);
	void OnCortMod(wxCommandEvent& event);
	void OnOsmoMod(wxCommandEvent& event);
	void OnHeatMod(wxCommandEvent& event);
	void OnGHMod(wxCommandEvent& event);

	void OnOutput(wxCommandEvent& event);
	void OnInfo(wxCommandEvent& event);
	void OnBurst(wxCommandEvent& event);
	//void OnNeuroGen(wxCommandEvent& event);
	void OnOptionPanel(wxCommandEvent& event);
	void OnNewDisp(wxCommandEvent& event);
	void OnProtocol(wxCommandEvent& event);
	void OnSound(wxCommandEvent& event);
	void OnGraphAdd(wxCommandEvent& event);
	void OnEnter(wxCommandEvent& event);
	void OnModGen(wxCommandEvent& event);
	void OnDiagBox(wxCommandEvent& event);
	void OnGridBox(wxCommandEvent& event);
	void OnNeuroBox(wxCommandEvent& event);
	void OnPlotBox(wxCommandEvent& event);
	void OnModBox(wxCommandEvent& event);
};


#endif
