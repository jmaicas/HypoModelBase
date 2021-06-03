

#ifndef HYPOPLOT_H
#define HYPOPLOT_H


#include "hypograph.h"
#include "hypopanels.h"


enum {
	ID_plotflag = 9000,
	ID_ioflag,
	ID_V1data,
	ID_neurodice
};



class PlotBox: public ParamBox
{
public:
	Model *mod;
	TextGrid *textgrid;
	wxString text;

	TagBox *datatag, *datapath;
	GraphDat *graph;
	GraphWindow3 *graphwin;
	GraphDisp *graphdisp;

	GridBox *gridbox;
	//PlotSet *plotset;
	NeuroBox *neurobox;

	vector<datdouble>ycoldata;
	vector<datdouble>xcoldata;
	int plotsetindex;
	//PlotSet *plotset;

	int plotcount;
	int plotlayer;
	int graphindex;
	int dispindex;
	wxString coltag;

	wxCheckBox *errcheck;

	PlotBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size);
	//~PlotBox();
	void OnPlotXY(wxCommandEvent& event);
	void OnPlotY(wxCommandEvent& event);
	void OnBin(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);
	void OnBrowse(wxCommandEvent& event);
	void OnSelect(wxCommandEvent& event);
	void OnRemovePlot(wxCommandEvent& event);
	void LoadData(FileDat *);
	void LoadTxt(FileDat *);
	void SetGraph(GraphWindow3 *);
	void TextClick(wxString tag);
	void SpinClick(wxString tag);
	void BoxEnter(wxString tag);
	void SetColumn(int col);
	void SetLayer();
	void DataEdit(int col);
	void OnParamStore(wxCommandEvent& event);
	void OnParamLoad(wxCommandEvent& event);
    
    //void OnV1(wxCommandEvent& event);
	void OnNeuroDice(wxCommandEvent& event);

	// Copied from PlotModel
	void PlotStore(wxString);
	void PlotLoad(wxString);
	void DataSelect(double, double);
	void SetCell(int, GraphDat *);
	int GetCellIndex();

	// Running Wheel Data
	datdouble wheeldata;
	datdouble bindata;
	int datacount;
	void WheelBin(int);
};


#endif
