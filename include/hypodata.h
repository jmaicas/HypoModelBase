#pragma once


#include "hypocontrols.h"
#include "hypotools.h"
#include "hypodat.h"
#include "hypomods.h"
#include "hypopanels.h"



class SpikePanel: public ToolPanel
{
public:
	NeuroBox *neurobox;
	SpikeDat *currneuron;
	std::vector<NeuroDat>*neurons;

	int i;
	wxString text;
	bool diagnostic = true;

	int neuroindex;
	int neurocount;
	int currselect;
	int selectcount;
	int cellmode;

	int selectmode[2];
	std::vector<int>selectspikes[2];

	wxStaticBoxSizer *selectbox[2];
	wxToggleButton *addbutton[2];
	wxToggleButton *subbutton[2];

	wxTextCtrl *datneuron;
	wxSpinButton *datspin;
	ParamCon *fromcon; 
	ParamCon *tocon;

	wxStaticText *label;
	wxStaticText *spikes;
	wxStaticText *freq;
	wxStaticText *selectspikecount;
	wxStaticText *selectfreq;
	wxCheckBox *filtercheck;

	SpikePanel(NeuroBox *);

	// Data processing and display
	void PanelData(NeuroDat *data = NULL);
	void NeuroData(bool dispupdate = true);
	void AnalyseSelection();

	// Button and panel commands
	void OnNext(wxSpinEvent& event);
	void OnPrev(wxSpinEvent& event);
	void OnEnter(wxCommandEvent& event);
	void OnAdd(wxCommandEvent& event);
	void OnSub(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnInvert(wxCommandEvent& event);
	void OnClick(wxPoint);
	void OnToggle(wxCommandEvent& event);

	// External commands
	void SetSelectRange(double, double);
	void SetData(SpikeDat *, std::vector<NeuroDat>*);

	// Selection
	void SelectAdd();
	void SelectSub();
	void SelectUpdate();
	void AddSubToggle(int sel, int type);
	void SelectStore();
	void SelectLoad();
	NeuroDat *GetCell(wxString name);  
};


class NeuroBox: public ParamBox
{
public:
	Model *mod;
	DiagBox *diagbox;

	int neuroindex;
	int cellcount;

	//SpikeDat *currcell;
	//std::vector<NeuroDat>*cells;
	//SpikeDat *currmodcell;
	//std::vector<NeuroDat>*modcells;

	GridBox *gridbox;
	BurstBox *burstbox;

	wxTextCtrl *datneuron;
	wxSpinButton *datspin;
	TagBox *neurodatatag;
	TagBox *selectstoretag;
	TextGrid *textgrid;

	SpikePanel *cellpanel, *modpanel;

	wxStaticBoxSizer *selectbox1, *selectbox2;  //, *filterbox;
	wxStaticBoxSizer *selectbox3, *selectbox4;
	wxToggleButton *addbutton[5];
	wxToggleButton *subbutton[5];
	int selectmode[5];
	int selectcount;
	int currselect;
	int currselectmod;
	
	std::vector<int>selectspikes[4];
	bool spikeselectLink;

	wxCheckBox *filtercheck;


	NeuroBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size);
	//~NeuroBox();
	//void NeuroData(bool dispupdate = true);
	//void NeuroAnalysis();
	//void PanelData(NeuroDat *data = NULL);
	//void OnNext(wxSpinEvent& event);
	//void OnPrev(wxSpinEvent& event);
	//void OnEnter(wxCommandEvent& event);
	void OnLoadData(wxCommandEvent& event);
	void OnBrowse(wxCommandEvent& event);
	void LoadDataList(FileDat *);
	void LoadNeuroData(FileDat file, int col);

	void OnGridFilter(wxCommandEvent& event);
	void OnSelectStore(wxCommandEvent& event);
	void OnSelectLoad(wxCommandEvent& event);
	void OnBoxCheck(wxCommandEvent& event);


	// Functions ported from PlotModel
	void DataSelect(double, double);
	//void SetCell(int, GraphDat*);
	int GetCellIndex();
	void Analysis();

	void AddModSpikePanel(SpikeDat *, std::vector<NeuroDat>*);
	void SetGraph(GraphWindow3 *newgraphwin=NULL);
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
