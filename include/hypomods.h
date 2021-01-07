
#ifndef HYPOMODS_H
#define HYPOMODS_H


#include "wx/wx.h"
#include "hypodat.h"
#include <hypotools.h>
//#include "hypopanels.h"
//#include "hypograph.h"
//#include "evofitbasic.h"


class HypoMain;
class EvoFitBox;
class EvoChrome;
class ParamBox;

//wxDECLARE_EVENT(wxEVT_COMMAND_MODTHREAD_COMPLETED, wxThreadEvent);

//wxDEFINE_EVENT(wxEVT_COMMAND_MODTHREAD_COMPLETED, wxThreadEvent);

wxDECLARE_EVENT(EVT_MODTHREAD_COMPLETED, wxThreadEvent);

wxDECLARE_EVENT(EVT_DIAG_WRITE, wxThreadEvent);

//wxDEFINE_EVENT(wxEVT_COMMAND_MODTHREAD_COMPLETED, wxThreadEvent);


class ModThread : public wxThread
{
public:
	ParamBox *modbox;
	HypoMain *mainwin;
	wxString snum;

	bool diag;

	ModThread(ParamBox *box, HypoMain *main) { modbox = box; mainwin = main; diag = false; };
};


class TextGridMod : public TextGrid
{
public:
	Model *mod;

	TextGridMod(Model *mod, wxWindow *parent, wxSize size);
};



class Model : public wxEvtHandler
{
public:
	short modtype;
	short ostype;
	short gsmode;
	short gcount;
	bool graphload;
	short gsync;
	int storesize;
	double xmin;
	int xscaletoggle;
	bool oldhist;
	int basicmode;
	bool evoflag;
	int burstmode;
	int projmode;
    
    // Threads
    wxMutex *runmute;
    bool runflag;
    //wxCommandEvent diagevent(wxEVT_COMMAND_TEXT_UPDATED, ID_Diagnostic);

	// Prefs
	int numdraw;

	wxString modname;
	wxString modtag;
	wxString initparams;
	wxString initgraph;
	wxString initdata;
	wxString path;

	ParamBox *modbox;
	DiagBox *diagbox;
	//ModDat *moddata;
	HypoMain *mainwin;
	GraphBase *graphbase;
	ModThread *modthread;
	ParamStore *modeflags;
	ParamStore *toolflags;
	ParamStore prefstore;
	ToolSet modtools;
	wxString gcodes[10], gtags[10];
	ScaleBox *scalebox;
	GraphWindow3 **graphwin;

	DatStore *recdata;
	GridBox *gridbox;

	NeuroBox *neurobox;
	SoundBox *soundbox;
	EvoFitBox *fitbox;
	EvoChrome *fitchrome;

	// Data Storage
	SpikeDat *expdata;
	datdouble testhist[5], testhistx[5];
	datdouble datahist[5], datahistx[5];


	Model(int, wxString, HypoMain *);
	virtual ~Model();

	long ReadNextData(wxString *);
	wxString GetPath();
    void DiagWrite(wxString);
	virtual void RunModel();
	virtual void Output() {};
	virtual void GSwitch(GraphDisp *gpos, ParamStore *gflags);
	virtual void ScaleSwitch(double xscale = 0) {};
	virtual int ModeSum(ParamStore *gflags);
	virtual int SoundLink(SpikeDat **, datdouble **);
	virtual void SoundOn() {}; 
	virtual void ModStore();
	virtual void ModLoad();
	virtual void DataSelect(double, double) {};
	//virtual void GStore(wxComboBox *);
	//virtual void GLoad(wxComboBox *);
	virtual void GHistStore();
	virtual void GHistLoad(wxComboBox *);
	virtual void ModClose() {};
	virtual void EvoRun() {};
	virtual void ScaleConsoleAbove(ScaleBox *scalebox, int condex) {};
	virtual void ScaleConsoleBelow(ScaleBox *scalebox, int condex) {};
	virtual void SetCell(int cellindex, GraphDat *) {};
	virtual int GetCellIndex();
	virtual void ParamScan() {};
	virtual void SpikeDataSwitch(SpikeDat *);
	virtual void BurstUpdate();
	virtual void GridColumn(int col);
	virtual void GridRow(int row) {};
    virtual void DataCopy(wxString oldpath, wxString newpath);
    virtual void OnModThreadCompletion(wxThreadEvent&);
	virtual void DataOutput() {};
    void OnDiagWrite(wxThreadEvent&);
};



#endif
