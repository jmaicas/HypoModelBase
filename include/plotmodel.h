/*
*  plotmodel.h
*
*  Created by Duncan MacGregor.  
*
*/


#ifndef PLOTMOD_H
#define PLOTMOD_H


#include "wx/wx.h"
#include "hypomodel.h"


enum {
	ID_plotflag = 9000,
	ID_ioflag
};



class PlotModel;


class PlotDataBox: public OutBox
{
public:
	PlotModel *mod;
	PlotBox *plotbox;

	TextGrid *histgrid;
	TextGrid *hazgrid;

	PlotDataBox(PlotModel *mod, const wxString& title, const wxPoint& pos, const wxSize& size, int rows=100, int cols=20);

	void OnNeuro(wxCommandEvent& event);
	void ColumnSelect(int);
};


class PlotDat: public ModDat
{
public:
	PlotDat(int size);

	int runtime;
	datdouble water;
	datdouble salt;
	datdouble osmo;
	datdouble heat;
};


class PlotMod : public ModThread
{
public:
	ParamStore *plotparams;
	ScaleBox *scalebox;
	HypoMain *mainwin;
	PlotDat *plotdata;
	PlotBox *plotbox;
	PlotModel *mod;

	// Model flags
	int randomflag;

	// Model variables
	double water;
	double salt;
	double osmo;
	double heat;

	// Model parameters
	int runtime;
	double waterloss;

	PlotMod(PlotModel *mod);
	virtual void *Entry();
	void initialise();
	void plotmodel();
};


class PlotModel : public Model
{
public:
	PlotDat *plotdata;
	PlotBox *plotbox;
	PlotDataBox *databox;
	CellBox *cellbox;

	SpikeDat *spikedata;
	datdouble coldata[10];
	datdouble xcoldata;
	datdouble wheeldata;
	datdouble bindata;
	int datacount;

	SpikeDat *viewcell;
	vector<NeuroDat>celldata;
	//NeuroDat *celldata;
	//int cellcount[10];
	int cellcount;

	bool wheelmode;
	bool cellmode;
	bool plotmode;

	FileBase *filebase;

	PlotModel(int, wxString, HypoMain *);
	~PlotModel();

	void RunModel();
	void GraphData();
	void GSwitch(GraphDisp *gpos, ParamStore *gflags);
	void StoreClear();
	void WheelBin(int);
	void NeuroAnalysis();
	void ModClose();
	void DataSelect(double, double);
};


#endif

