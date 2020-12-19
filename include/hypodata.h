#pragma once


#include "wx/grid.h"
#include "hypocontrols.h"
#include "hypotools.h"
#include "hypodat.h"
#include "hypomods.h"
#include "hypopanels.h"


class NeuroBox: public ParamBox
{
public:
	Model *mod;
	DiagBox *diagbox;

	int neuroindex;
	int cellcount;
	SpikeDat *currcell;
	//SpikeDat *selected;
	//NeuroDat *cells;
	vector<NeuroDat>*cells;
	GridBox *gridbox;
	BurstBox *burstbox;

	wxTextCtrl *datneuron;
	wxSpinButton *datspin;
	TagBox *neurodatatag;
	TextGrid *textgrid;

	wxStaticBoxSizer *selectbox1, *selectbox2;  //, *filterbox;
	wxToggleButton *addbutton[5];
	wxToggleButton *subbutton[5];
	int selectmode[5];
	int selectcount;
	int currselect;
	//BurstDat **selectdata;
	//BurstDat *selectdata[5];
	int *selectspikes[2];
	bool spikeselectLink;

	wxCheckBox *filtercheck;


	NeuroBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size);
	~NeuroBox();
	void NeuroData(bool dispupdate = true);
	//void NeuroAnalysis();
	void PanelData(NeuroDat *data = NULL);
	void OnNext(wxSpinEvent& event);
	void OnPrev(wxSpinEvent& event);
	void OnEnter(wxCommandEvent& event);
	void OnLoadData(wxCommandEvent& event);
	void OnBrowse(wxCommandEvent& event);
	void LoadDataList(FileDat *);
	void LoadNeuroData(FileDat file, int col);

	void OnAdd(wxCommandEvent& event);
	void OnSub(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnInvert(wxCommandEvent& event);
	void AnalyseSelection();
	void SetSelectRange(double, double);
	void OnClick(wxPoint);
	//void OnClick(wxMouseEvent &event);
	void OnToggle(wxCommandEvent& event);
	void SelectAdd();
	void SelectSub();
	void SelectUpdate();
	void AddSubToggle(int sel, int type);

	void OnBoxCheck(wxCommandEvent& event);
	void OnGridFilter(wxCommandEvent& event);
	void OnSelectStore(wxCommandEvent& event);
	void OnSelectLoad(wxCommandEvent& event);

	NeuroDat *GetCell(wxString name);  


	// Functions ported from PlotModel
	void DataSelect(double, double);
	void SetCell(int, GraphDat*);
	int GetCellIndex();
	void Analysis();
};


class GridNumDat
{
public:
	int row;
	int col;
	double data;
};


class GridTextDat
{
public:
	int row;
	int col;
	wxString data;
};


class DataGrid
{
public:
	int count;
	int grow;      // expansion step for increasing storage
	int rowmax;
	int colmax;

	DataGrid() {
		count = 0;
		rowmax = 0;
		colmax = 0;
	};

	void Clear() {
		count = 0;
		rowmax = 0;
		colmax = 0;
	}
};


class GridLoadThread : public wxThread
{
public:
	GridBox *gridbox;

	GridLoadThread(GridBox *gridbox);
	virtual void *Entry();
};


class GridBox: public ParamBox
{
public:
	Model *mod;
	wxTextCtrl *textbox;
	TextGrid *textgrid[10];   // grid store array
	TagStore gridindex;
	DiagBox *diagbox;
	wxNotebook *notebook;
	//PlotBox *plotbox;
	bool bookmode, vdumode;
	bool undomode;
	bool startshift;
	int colflag[1000];
	int numgrids;

	// NeuroBox links
	NeuroBox *neurobox;
	vector<NeuroDat>* celldata;   

	// Grid data vectors for large grids, currently not in use 
	vector<GridNumDat> numdata;
	DataGrid numdatagrid;
	vector<GridTextDat> textdata;
	DataGrid textdatagrid;

	// Standard grid links
	TextGrid *currgrid;   // pointer to selected grid, textgrid[0] by default
	TextGrid *datagrid;
	TextGrid *outputgrid;
	TextGrid *paramgrid;

	GridBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size, int rows=100, int cols=20, bool bookmode=true, bool vdumode=true);

	virtual void GridDefault();
	virtual void TestGrid();
	void GridStore();
	void GridStoreAll();
	void GridLoad();
	void GridLoadAll();
	void HistLoad();
	void HistStore();

	void OnGridStore(wxCommandEvent& event);
	void OnGridLoad(wxCommandEvent& event);
	void OnRightClick(wxMouseEvent& event);
	void OnUndo(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnButton(wxCommandEvent& event);

	int ColumnData(int, datdouble *);
	void AddGrid(wxString label, wxSize size);
	void SetCurrentGrid();
	void ParamButton();
	void NeuroButton();
	void PlotButton();

	void OnParamMode(wxCommandEvent& event);
	void OnParamScan(wxCommandEvent& event);
	void OnNeuroScan(wxCommandEvent& event);
	virtual void OnPlot(wxCommandEvent& event);
	void OnGridSelect(wxBookCtrlEvent& event);

	void NeuroGridFilter();
	void NeuroScan();

	virtual void OnCellChange(wxGridEvent& event);
	virtual void ColumnSelect(int);
	virtual void RowSelect(int);

	void SetNumCell(int row, int col, double data);
	void SetTextCell(int row, int col, wxString data);
};
