

#ifndef HYPOPANELS_H
#define HYPOPANELS_H

#include "wx/wx.h"
#include <wx/richtext/richtextctrl.h>
#include "hypocontrols.h"
#include "hypotools.h"
#include "hypodat.h"
//#include "hypomods.h"

using namespace std;

class Model;
class HypoMain;


class OptionPanel : public wxDialog
{
public:
	HypoMain *mainwin;

	ParamNum *numdrawcon;
	ParamNum *viewheightcon;
	ParamNum *ylabelcon;
	ParamNum *datsamplecon;
	ParamText *datapathcon;
	ParamText *outpathcon;
	ParamText *parampathcon;
	ParamText *modpathcon;
	OptionPanel(HypoMain *, const wxString&);

	void OnOK(wxCommandEvent& event);
	void OnBrowse(wxCommandEvent& event);
	//void OnBrowseOut(wxCommandEvent& event);
	void OnModRadio(wxCommandEvent& event);
};


class histqueue
{
public:
	short i;
	short size;
	wxString histstore[10];
	wxString holder;

	histqueue() {
		size = 10;
		for(i=0; i<size; size++) histstore[i] = "";
	};

	void Add(wxString newtag) {
		for(i=size-1; i>0; i--) histstore[i] = histstore[i-1];
		histstore[0] = newtag;
	};
};


class ParamBox: public ToolBox
{
public:
	int numparams;	
	double countmark;
	int column;
	int redflag;
	int modtype;
	int version;
	int parallel;
	int labelwidth;
	int defbutt;
	wxString redtag;
	wxString storehist[10];
	wxString initparams;
	wxString name;
	short histmode;
	bool defstore;

	HypoMain *mainwin;
	Model *mod;
	
	wxString labelset[100];
	ParamSet *paramset;
	ParamStore *modparams;
	ParamStore *modflags;
	RefStore *flagrefs;
	RefStore *checkrefs;
	RefStore *panelrefs;
	//FlagSet *flagset;

	int autorun;
	//int labelwidth;
	//int columns;
	//int colsize;

	wxStaticText *runtime;
	wxStaticText *mean;
	wxStaticText *spikes;
	wxStaticText *freq;
	wxStaticText *sd;
	wxStaticText *runcount;

	wxMenu *menuControls;
	wxMenu *menuModel;
	wxMenu *menuParamSet;
	wxMenu *menuData;
	wxMenu *menuTools;
	wxMenuBar *menuBar;

	wxButton *runbutton; 
	wxButton *resetbutton; 
	//wxTextCtrl *paramstoretag;
	TagBox *paramstoretag;
	wxTextCtrl *datastoretag;

	//wxBoxSizer *vbox1;
	//wxBoxSizer *vbox2;
	//wxBoxSizer *vbox3;
	//wxBoxSizer *vbox4;
	//wxBoxSizer *vbox5;
	wxBoxSizer *vbox[5];
	wxBoxSizer *parambox;
	wxBoxSizer *buttonbox;

	ParamBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size, wxString name = "", int type=0);
	//ParamBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size, bool close);
	virtual ~ParamBox();

	void Initialise();
	void SetVBox(int);
	virtual void OnRun(wxCommandEvent& event);
	virtual void OnDefault(wxCommandEvent& event);
	void OnSpin(wxSpinEvent& event);
	void OnDClick(wxMouseEvent& event);
	void OnAutorun(wxCommandEvent& event);
	void OnParamStore(wxCommandEvent& event);
	void StoreParam(wxString tag="");
	void OnParamLoad(wxCommandEvent& event);
	void ParamLoad(wxString tag="", bool compmode = false);
	void HistStore();
	void HistLoad();
	void OnFlag(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
	void OnPanel(wxCommandEvent& event);
	void SetCheck(wxCheckBox *, bool state);
	void SetPanel(int, ToolBox *);
	void OnFocus(wxFocusEvent& event);
	void SetCount(double);
	void SetStatus(wxString);
	void WriteVDU(wxString);
	void InitMenu();
    void DataMenu();
	void SetModFlag(int, wxString, wxString, int state=0, wxMenu *menu=NULL); 
	wxCheckBox *SetModCheck(int, wxString, wxString, int state=0); 
	void ModData();
	wxBoxSizer *StoreBox(wxString label="", wxPanel *storepanel=NULL);
	ParamStore *GetParams(ParamStore *pstore=NULL);
	ParamStore *GetNumParams();
	void ParamLayout(int columns=1);
	void OnClose(wxCloseEvent& event);
	wxBoxSizer *RunBox();
};


class ModGenBox : public ToolBox
{
public:
	ParamSet *paramset;
	wxString modname, modtag, modnum;
	int modindex, platecount;
	wxNotebook *notebook;
	wxRichTextCtrl **page;
	wxString plate[10];
	//wxTextCtrl *page[10];

	ModGenBox(HypoMain *main, const wxString& title, const wxPoint& pos, const wxSize& size);

	void OnGen(wxCommandEvent& event);
	void OnOutput(wxCommandEvent& event);
	void GenModCode();
	void Line(int, wxString);
	void Read();
	void DefGen();
	void PageRead(int index, wxString name, wxString path);
};


class CellBox: public ParamBox
{
public:
	Model *mod;
	DiagBox *diagbox;

	int neuroindex;
	int cellcount;
	SpikeDat *currcell;
	//NeuroDat *cells;
	vector<NeuroDat> *cells;

	wxTextCtrl *datneuron;
	wxSpinButton *datspin;

	CellBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size);
	void NeuroData();
	//void NeuroAnalysis();
	void PanelData(NeuroDat *);
	void OnNext(wxSpinEvent& event);
	void OnPrev(wxSpinEvent& event);
	void OnEnter(wxCommandEvent& event);
};


class OutBox: public ParamBox
{
public:
	Model *mod;
	wxTextCtrl *textbox;
	TextGrid *textgrid;
	DiagBox *diagbox;
	wxNotebook *notebook;

	OutBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size, int rows=100, int cols=20);

	virtual void GridDefault();
	virtual void TestGrid();
	void GridStore();
	void GridLoad();
	void GridLoadFast();
	void OnGridStore(wxCommandEvent& event);
	void OnGridLoad(wxCommandEvent& event);
	void HistLoad();
	void HistStore();
	void OnRightClick(wxMouseEvent& event);
	void OnUndo(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnButton(wxCommandEvent& event);
	int ColumnData(int, datdouble *);
};


class InfoBox : public ToolBox
{
public:
	HypoMain *mainwin;
	//wxPoint mpos, oldpos;

	//int i, buttonheight;
	//wxString snum;
	//wxPanel *panel; 
	//wxStaticText *NumPanel(int width, wxString inittext);
	wxTextCtrl *datfiletag;

	ParamStore *artparams;
	ParamSet *paramset;

	InfoBox(HypoMain *main, const wxString& title, const wxPoint& pos, const wxSize& size);
	//void BurstData();
	//void SpikeDat();
	void GetArtParams();
	void OnDatLoad(wxCommandEvent& event);
	//void BurstScan();
	//void SetPosition();
	//void OnMove(wxMoveEvent& event);
	void OnClose(wxCloseEvent& event);
};


class ProtocolBox : public ParamBox
{
public:

	//wxTextCtrl *datfiletag;

	//ParamStore *artspikeparams;
	//ParamStore *protoparams;
	//ParamSet *artspikecon;
	wxStaticText *currentinput;
	wxStaticText *status;

	ProtocolBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size);
	//void BurstData();
	//void SpikeDat();
	//void GetArtSpikeParams();
	void OnRun(wxCommandEvent& event);
	//void BurstScan();
	//void SetPosition();
	//void OnMove(wxMoveEvent& event);
	//void OnClose(wxCloseEvent& event);
};



class BurstPanel
{
public:
	wxPanel *panel;
	SpikeDat *spikedata;

	wxStaticText *numbursts;
	wxStaticText *meanspikes;
	wxStaticText *meanlength;
	wxStaticText *meansilence;
	wxStaticText *sdlength;
	wxStaticText *sdsilence;
	wxStaticText *actQ;
	wxStaticText *modetime;
	wxStaticText *moderate;

	wxStaticText *intraspikes;
	wxStaticText *intrafreq;
	wxStaticText *intraisimean;
	wxStaticText *intraisisd;

	//BurstDispData(wxPanel *);
};



class BurstBox: public ToolBox
{
public:
	int maxint;
	int minspikes;
	int maxspikes;
	int startspike;
	int endspike;
	int burstcount;
	double filterthresh;
	double units;

	HypoMain *mainwin;

	wxString filetag, datname;
	SpikeDat *spikedata;
	SpikeDat *loaddata;
	SpikeDat *moddata;
	ParamStore *burstparams;
	BurstPanel *datburst;
	BurstPanel *modburst;
	BurstPanel *evoburst;

	//BurstDat *burstdata;
	ParamSet *paramset;
	wxStaticText *numbursts;
	wxStaticText *meanspikes;
	wxStaticText *meanlength;
	wxStaticText *meansilence;
	wxStaticText *sdlength;
	wxStaticText *sdsilence;
	wxStaticText *datnumbursts;
	wxStaticText *datmeanspikes;
	wxStaticText *datmeanlength;
	wxStaticText *datmeansilence;
	wxStaticText *datsdlength;
	wxStaticText *datsdsilence;
	wxStaticText *allspikes;
	wxStaticText *allfreq;
	wxStaticText *allisimean;
	wxStaticText *allisisd;
	wxStaticText *intraspikes;
	wxStaticText *intrafreq;
	wxStaticText *intraisimean;
	wxStaticText *intraisisd;
	wxStaticText *datintraspikes;
	wxStaticText *datintrafreq;
	wxStaticText *datintraisimean;
	wxStaticText *datintraisisd;
	//wxTextCtrl *datfiletag;
	wxComboBox *datfiletag;
	wxStaticText *datstatus;

	BurstBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, SpikeDat *spikedat=NULL, wxString intratag ="Intra Burst");
	void BurstDataDisp(SpikeDat *dispdata=NULL, BurstPanel *datpanel=NULL);
	void BurstDataPanel(BurstPanel *);
	void SpikeDataDisp(SpikeDat *dispdata=NULL);
	void Scan(SpikeDat *);
	ParamStore *GetParams();
	void OnScan(wxCommandEvent& event);
	void OnDatLoad(wxCommandEvent& event);
	void OnDatOutput(wxCommandEvent& event);
	void OnDatRadio(wxCommandEvent& event);

	//void OnClose(wxCloseEvent& event);
	//void OnMouseMove(wxMouseEvent& event);
	~BurstBox();
};




#endif