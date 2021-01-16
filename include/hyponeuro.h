#pragma once


#include "hypomods.h"


enum {
	ID_epspsynch = 10000,
	ID_scalemode,
	ID_polymode,
	ID_synthmode,
	ID_transmode,
	ID_VSflag,
	ID_transflag,
	ID_Synth,
	ID_Proto,
	ID_storereset,
	ID_diffusion,
	ID_cellfiltercheck,
	ID_modfiltercheck
};



class NeuroMod : public Model
{
public:
	// Data
	SpikeDat *evodata, *evodata2;

	// Cell grid data - November 2019
	std::vector<SpikeDat> viewcell;
	std::vector<NeuroDat> celldata;
	int cellcount;

	// Model Neurons - November 2020
	int modneurons_max;   // current allocated number of model neuron objects

	// Boxes
	NeuroMod(int, wxString, HypoMain *);
	virtual ~NeuroMod();

	//void SpikeBox(int modmode = 0);
	void EvoGraphs();
	void IoDGraph(datdouble *, datdouble *, wxString, wxString, int, int barshift=0, wxString settag="");
	void DataSelect(wxString gname, double from, double to);
	virtual void ParamScan() {};
	//virtual void DataSwitch();
};
