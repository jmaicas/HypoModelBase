

#ifndef HYPOPLOT_H
#define HYPOPLOT_H


#include "hypograph.h"
#include "hypopanels.h"


class PlotModel;


class PlotBox: public ParamBox
{
public:
	PlotModel *mod;
	wxCheckBox *synccheck;
	TextGrid *textgrid;
	wxString text;

	TagBox *datatag, *datapath;
	GraphDat *graph;
	GraphWindow3 *graphwin;
	GraphDisp *graphdisp;

	int plotcount;
	int plotlayer;
	int graphindex;
	wxString coltag;

	PlotBox(PlotModel *mod, wxString tag, const wxString& title, const wxPoint& pos, const wxSize& size);
	//~PlotBox();
	void OnPlotXY(wxCommandEvent& event);
	void OnPlotY(wxCommandEvent& event);
	void OnBin(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);
	void OnBrowse(wxCommandEvent& event);
	void OnSelect(wxCommandEvent& event);
	void LoadData(FileDat *);
	void LoadTxt(FileDat *);
	void SetGraph(GraphWindow3 *);
	void TextClick(wxString tag);
	void SetColumn(int col);
};


#endif