
#ifndef HYPOCONTROLS_H
#define HYPOCONTROLS_H



#include "wx/wx.h"
#include "wx/spinbutt.h"
//#include "hypomodel.h"
#include "hypodat.h"
//#include "hypomods.h"
#include "wx/notebook.h"
#include "wx/aui/aui.h"
#include "wx/tglbtn.h"



class MainFrame;
class ToolBox;
class Model;
class ToolSet;
class ToolPanel;
class ParamSet;
//class HypoMain;



class TextBox: public wxTextCtrl
{
public:
	int val;
	//TextBox(wxWindow, wxWindowID, wxString, wxPoint, wxSize, long);
	TextBox(wxWindow *parent, wxWindowID id, wxString value, wxPoint pos, wxSize size, long style);
	//TextBox(panel, wxID_ANY, snum, wxDefaultPosition, wxSize(45, -1), wxTE_PROCESS_ENTER);

};


class TagBox: public wxComboBox
{
public:
	wxString name, tagpath, tagfilepath, tagfilename;
    wxString boxpath, text;
	wxString redtag;
	bool labelset, histmode;
	DiagBox *diagbox;
	MainFrame *mainwin;
	bool diagnostic;

	TagBox(MainFrame *mainwin, ToolPanel *panel, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, wxString name, wxString path);
	~TagBox();

	void HistLoad();
	void HistStore();
	void SetLabel(wxString);
	void SetFile(wxString filename);
	void ChooseFile();
	void OnDClick(wxMouseEvent & event);
	void OnRClick(wxMouseEvent & event);
    void PathUpdate();
	wxString LoadTag(wxString path, wxString suffix="");
	wxString StoreTag(wxString path, wxString suffix="");
	wxBoxSizer *TagCon(ToolBox *box, int storeid, int loadid, int orient=wxHORIZONTAL);
};


class ToolText: public wxStaticText
{
public:
	DiagBox *diagbox;
	ToolBox *toolbox;
	wxString text;
	wxString tag;
	//int conID;

	virtual void OnLeftClick(wxMouseEvent & event);
	void OnLeftDClick(wxMouseEvent & event);
	void OnRightDClick(wxMouseEvent & event);
	void OnMouseMove(wxMouseEvent & event);

	ToolText(wxWindow *parent, ToolBox *tbox, wxString tag, wxString label, const wxPoint& pos, const wxSize& size, long style);
};


class ToolButton: public wxButton
{
public:
	bool compmode;
	DiagBox *diagbox;

	int ID, linkID;
	wxWindow *parent;

	virtual void OnLeftUp(wxMouseEvent & event);
	virtual void OnLeftDClick(wxMouseEvent & event);
	virtual void Press();

	ToolButton(wxWindow *parent, wxWindowID id, wxString label, const wxPoint& pos, const wxSize& size, DiagBox *diagbox);
};



// ToolStore, attempt at separate class for packaging tag store and controls, NOT IN USE, too much parameter passing required, replaced by ToolBox::TagStoreBox()
class ToolStore
{
public:
	TagBox *tagbox;
	wxBoxSizer *boxsizer;
	wxString tag;
	ToolPanel *panel;
	ToolBox *box;

	//ToolStore(ToolBox *, wxString tag, wxString path, int width=100, ToolPanel *panel=NULL);
	ToolStore(ToolBox *, TagBox *, int storeid, int loadid);
};





class ToolSet
{
public:
	int i, numtools, maxtools;
	ToolBox *box[50];
	ParamStore tagindex;
	wxString tags[50];
	
	ToolSet() {
		numtools = 0;
		maxtools = 50;
	};
	
	void AddBox(ToolBox *newbox, bool serve=false, bool child=false); 
	
	void RemoveBox(int index) {
		box[index] = NULL;
	};

	int GetIndex(wxString tag);
	wxString GetTag(int index);
	ToolBox *GetBox(wxString tag);

	//void RemoveBox(wxString tag) {

};


class TagDat
{
public:
	wxString tag;
	TagBox *box;
};


class TagSet
{
public:
    int numtags;
	TagDat tagdata[100];
    
    TagSet() {
        numtags = 0;
    }
    
    void AddTag(wxString boxtag, TagBox *newbox);
    void UpdatePath();
	TagBox *GetBox(wxString tag);
};


class ParamText: public wxControl
{
public:
	wxTextCtrl *textbox;
	wxBoxSizer *sizer;
	wxStaticText *label;
	wxButton *button;
	wxString snum;
	wxString name;
	wxFont textfont, confont;
	
	int ostype;
	int textwidth, labelwidth, buttonwidth;
	
	ParamText(wxPanel *panel, wxString name, wxString labelname, wxString initval, int labelwidth=60, int textwidth=150);
	void AddButton(wxString label, int id, int width=50);
	wxString GetValue();
	
protected:
	virtual wxSize DoGetBestSize() const;
};


class ParamNum: public wxControl
{
public:
	wxTextCtrl *numbox;
	wxBoxSizer *sizer;
	wxStaticText *label;
	wxString snum;
	wxString name;
	wxFont textfont, smalltextfont;
	
	int ostype;
	int decimals;
	double nummin, nummax;
	
	ParamNum(wxPanel *panel, wxString name, wxString labelname, double initval, int places=0);
	wxString GetValue();
	void SetValue(double val);
	
protected:
	virtual wxSize DoGetBestSize() const;
};


class ParamCon: public wxControl
{
public:
	int ostype;
	int type;
	int numwidth, labelwidth;
    int numheight;
	int pad;
	wxFont textfont, smalltextfont;
	wxTextCtrl *numbox;
	wxSpinButton *spin;
	wxBoxSizer *sizer;
	ToolText *label;
	wxString snum;
	wxString name;
	wxString plabel;
	double value, oldvalue;
	MainFrame *mainwin;
	ToolPanel *panel;
	
	double numstep;
	int decimals;
	double min, max;
	bool cycle;
	
	ParamCon(ToolPanel *panel, int type, wxString name, wxString labelname, double initval, double step, int places, int labelwidth=60, int numwidth=45);
	ParamCon(ToolPanel *panel, wxString name, wxString labelname, wxString initval, int labelwidth, int textwidth);
	void OnSpinUp(wxSpinEvent& event);
	void OnSpinDown(wxSpinEvent& event);
	void OnSpin(wxSpinEvent& event);
	void OnEnter(wxCommandEvent& event);
	double GetValue();
	wxString GetString();
	void SetValue(double val);
	void SetPen(wxColour);
	void SetValue(wxString);
	void SetMinMax(double min, double max, bool cycle=false);
	void Select();
	
protected:
	virtual wxSize DoGetBestSize() const;
	virtual void DoMoveWindow(int x, int y, int width, int height);
};


class FlagSet
{
public:
	int numflags;
	wxString name[100];
	int id[100];
	
	FlagSet() { numflags = 0; };
	void AddFlag(wxString fname, int fid) { name[numflags] = fname; id[numflags] = fid; numflags++; };
};


class CheckSet
{
public:
	int numchecks;
	wxCheckBox *box[100];
	int id[100];

	CheckSet() { numchecks = 0; };
	void Add(wxCheckBox *cbox, int cid) { box[numchecks] = cbox; id[numchecks] = cid; numchecks++; };
};


class ParamSet
{
public:
	int labelwidth;   // not in use
	int numwidth;     // not in use
	int numparams;
	int currlay;      // Current parameter for panel layout, starts at 0
	int type;
	ToolPanel *panel;
	ParamCon *con[100];
	ParamNum *num[100];
	ParamStore ref;
	ParamStore paramstore;
	TagStore tagstore;     // new December 2020, ParamCon tag store and checking, currently in parallel to old 'ref' system

	// New default width settings   Novemeber 2018
	int num_labelwidth, num_numwidth;
	int con_labelwidth, con_numwidth;
	int text_labelwidth, text_textwidth;

	//ParamSet(ToolPanel *);
	ParamSet();
	~ParamSet();

	ParamCon *AddNum(wxString name, wxString labelname, double initval, int places, int labelwidth=-1, int numwidth=-1);
	ParamCon *AddCon(wxString name, wxString labelname, double initval, double step, int places, int labelwidth=-1, int numwidth=-1);
	ParamCon *AddText(wxString name, wxString labelname, wxString initval, int labelwidth=-1, int textwidth=-1);
	ParamCon *GetCon(wxString);
	ParamStore *GetParams();
	ParamStore *GetParamsNew(BoxOut *boxout);
	int GetID(wxString);
	wxString GetText(wxString);
	double GetValue(wxString);
	void SetValue(wxString, double);
	void SetMinMax(wxString tag, double, double);
};


class ToolPanel: public wxPanel
{
public:
	wxString snum;
	ToolBox *toolbox;
	MainFrame *mainwin;
	int controlborder;
	wxFont boxfont, confont;
	int buttonheight;

	virtual void OnLeftClick(wxMouseEvent & event);
	void OnLeftDClick(wxMouseEvent & event);
	void OnRightDClick(wxMouseEvent & event);
	void OnMouseMove(wxMouseEvent & event);
	virtual void OnToggle(wxCommandEvent& event) {
		event.Skip();
};
	//wxToggleButton *ToggleButton(int id, wxString label, int width, wxBoxSizer *box, int point = 10, wxPanel *pan = NULL);
	wxToggleButton *ToggleButton(int id, wxString label, int width, wxBoxSizer *sizer, int point = 10, ToolBox *box = NULL);
	void Init();
	virtual void OnClick(wxPoint);

	ToolPanel(ToolBox *tbox, const wxPoint& pos, const wxSize& size);
	ToolPanel(MainFrame *main, const wxPoint& pos, const wxSize& size, long style=wxTAB_TRAVERSAL | wxNO_BORDER);
	ToolPanel(wxDialog *dbox, const wxPoint& pos, const wxSize& size);
	ToolPanel(wxNotebook *notebook, const wxPoint& pos, const wxSize& size);
	//ToolPanel(wxAuiNotebook *notebook, ToolBox *tbox, const wxPoint& pos, const wxSize& size);
	ToolPanel(ToolBox *tbox, wxWindow *parent);
};


class ToolBox: public wxFrame
{
public:
	MainFrame *mainwin;
	//Model *mod;
	ToolSet *toolset;
	//ParamBox **mainlink;
	DiagBox *diagbox;

	int i;
	int buttonheight;
	int labelwidth;
	int pinmode;
	int boxindex;
	int ostype;
	int boxtype;
	bool servant;
	bool child;
	bool visible;
	bool canclose;
	bool selfstore;   // Defaults to false, if true box stores its parameter values to file on closing
	wxPoint mpos, oldpos;
	wxSize boxsize;
	wxFont boxfont, confont;
	wxString boxtag, boxlabel;
	wxString snum, text;
	wxCommandEvent *blankevent;
	ToolPanel *panel;
	ToolPanel *activepanel;
	wxNotebook *tabpanel;
	wxBoxSizer *mainbox;
	wxAuiManager *winman;
	wxAuiNotebook *auitabpanel;
	wxStaticText *status;
	wxTextCtrl *vdu;
	wxGauge *gauge;

	ParamSet paramset;
	ParamStore toolparams;
	wxString toolpath;

	wxPoint SetPosition();
	void ReSize();
	void OnMove(wxMoveEvent& event);
	void OnSize(wxSizeEvent& event);
	void Init();
	virtual void OnClose(wxCloseEvent& event);
	//virtual void OnClick(wxPoint);
	//void OnDClick(wxMouseEvent & event);
	//void OnMouseMove(wxMouseEvent & event);
	//void OnLeftClick(wxMouseEvent & event);

	void AddButton(int id, wxString label, int width, wxBoxSizer *box, int pad = 1, int height = 0, wxPanel *pan = NULL);
	//void AddButton(int id, wxString label, int width, wxBoxSizer *box, int pad = 1);
	//wxToggleButton *ToggleButton(int id, wxString label, int width, wxBoxSizer *box, int point = 10, wxPanel *pan = NULL);
	wxStaticText *GridLabel(int, wxString label = "0");
	wxStaticText *NumPanel(int width = 80, int align = wxALIGN_RIGHT, wxString label = "0");
	wxStaticText *TextLabel(wxString);
	wxStaticText *StatusBar();
	wxTextCtrl *TextInput(int width = 80, int height = -1, wxString label = "---");
	TagBox *TextInputCombo(int width = 80, int height = -1, wxString label = "---", wxString name = "", wxString path = "");
	//wxBoxSizer *TagStoreBox(TagBox *storetagbox, int storeid, int loadid);

	//ToolBox(MainFrame *main, const wxString& title, const wxPoint& pos, const wxSize& size, int type=0, bool serve=false, bool child=false);
	ToolBox(MainFrame *main, wxString tag, const wxString& title, const wxPoint& pos, const wxSize& size, bool close);
	ToolBox(MainFrame *main, wxString tag, const wxString& title, const wxPoint& pos, const wxSize& size, int type=0, bool close=true);

	~ToolBox();

	virtual void OnToggle(wxCommandEvent& event) {};
	virtual void TextClick(wxString tag);
	virtual void SpinClick(wxString tag);
	virtual void BoxEnter(wxString tag);
	virtual void Load();
	virtual void Store();
};


#endif
