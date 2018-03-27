// **************************************************************
// **************      hypomods.h             *******************
// **************************************************************
//
//  For Handling events. 
//

#ifndef HYPOMODS_H
#define HYPOMODS_H


#include "wx/wx.h"
#include "hypodat.h"
#include "hypopanels.h"
//#include "hypograph.h"


class HypoMain;


class ModThread : public wxThread
{
public:
	ParamBox *modbox;
	HypoMain *mainwin;
	wxString snum;

	ModThread(ParamBox *box, HypoMain *main) { modbox = box, mainwin = main; };
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

	wxString modname;
	wxString modtag;
	wxString initparams;
	wxString initgraph;
	wxString initdata;
	wxString path;

	ParamBox *modbox;
	DiagBox *diagbox;
	ModDat *moddata;
	HypoMain *mainwin;
	GraphBase *graphbase;
	ModThread *modthread;
	ParamStore *modeflags;
	ParamStore *toolflags;
	ToolSet modtools;
	wxString gcodes[10];
	ScaleBox *scalebox;

	DatStore *recdata;

	Model(int, wxString, HypoMain *);
	virtual ~Model();

	long ReadNextData(wxString *);
	wxString GetPath();
	virtual void RunModel();
	virtual void Output();
	virtual void GSwitch(GraphDisp *gpos, ParamStore *gflags);
	virtual void ScaleSwitch(double xscale = 0);
	virtual int ModeSum(ParamStore *gflags);
	virtual int SoundLink(SpikeDat **, datdouble **);
	virtual void ModStore();
	virtual void ModLoad();
	virtual void DataSelect(double, double);
	//virtual void GStore(wxComboBox *);
	//virtual void GLoad(wxComboBox *);
	virtual void GHistStore();
	virtual void GHistLoad(wxComboBox *);
	virtual void ModClose();
	virtual void EvoRun();
};


class NeuroMod : public Model
{
public:
	// Data
	SpikeDat *evodata;

	NeuroMod(int, wxString, HypoMain *);
	virtual ~NeuroMod();

	void EvoGraphs();
};


#endif