
#ifndef HYPOCONTROLS_H
#define HYPOCONTROLS_H



#include "wx/wx.h"
#include "wx/spinbutt.h"
//#include "hypomodel.h"
#include <hypodat.h>
//#include "hypomods.h"
#include <wx/notebook.h>
#include "wx/aui/aui.h"
#include <wx/tglbtn.h>



class MainFrame;
class ToolBox;
class Model;
class ToolSet;
class ToolPanel;



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
	wxString name, path;
	bool labelset, histmode;
	DiagBox *diagbox;

	TagBox(ToolPanel *panel, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, wxString name, wxString modpath);
	~TagBox();

	void HistLoad();
	void HistStore();
};


class ToolButton: public wxButton
{
public:
	bool compmode;
	DiagBox *diagbox;

	virtual void OnLeftUp(wxMouseEvent & event);
	virtual void OnLeftDClick(wxMouseEvent & event);

	ToolButton(wxWindow *parent, wxWindowID id, wxString label, const wxPoint& pos, const wxSize& size, DiagBox *diagbox);
};


class ToolPanel: public wxPanel
{
public:
	wxString snum;
	ToolBox *toolbox;
	MainFrame *mainwin;
	
	virtual void OnLeftClick(wxMouseEvent & event);
	void OnLeftDClick(wxMouseEvent & event);
	void OnRightDClick(wxMouseEvent & event);
	void OnMouseMove(wxMouseEvent & event);
	
	ToolPanel(ToolBox *tbox, const wxPoint& pos, const wxSize& size);
	ToolPanel(MainFrame *main, const wxPoint& pos, const wxSize& size);
	ToolPanel(wxDialog *dbox, const wxPoint& pos, const wxSize& size);
  ToolPanel(MainFrame *main, wxWindow *parent);
};


class ToolBox: public wxFrame
{
public:
	MainFrame *mainwin;
	//Model *mod;
	ToolSet *toolset;
	//ParamBox **mainlink;
	
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
	wxPoint mpos, oldpos;
	wxSize boxsize;
	wxFont boxfont, confont;
	wxString boxname, boxlabel;
	wxString snum, text;
	wxCommandEvent *blankevent;
	ToolPanel *panel;
	ToolPanel *activepanel;
	wxNotebook *tabpanel;
	wxBoxSizer *mainbox;
	wxAuiManager *winman;
	wxStaticText *status;
	wxTextCtrl *vdu;
	wxGauge *gauge;
	
	wxPoint SetPosition();
	void ReSize();
	void OnMove(wxMoveEvent& event);
	void OnSize(wxSizeEvent& event);
	void Init();
	virtual void OnClose(wxCloseEvent& event);
	virtual void OnClick(wxPoint);
	//void OnDClick(wxMouseEvent & event);
	//void OnMouseMove(wxMouseEvent & event);
	//void OnLeftClick(wxMouseEvent & event);

	void AddButton(int id, wxString label, int width, wxBoxSizer *box, int pad = 1, int height = 0, wxPanel *pan = NULL);
	//void AddButton(int id, wxString label, int width, wxBoxSizer *box, int pad = 1);
	wxToggleButton *ToggleButton(int id, wxString label, int width, wxBoxSizer *box, int point = 10, wxPanel *pan = NULL);
	wxStaticText *GridLabel(int, wxString label = "0");
	wxStaticText *NumPanel(int width = 80, int align = wxALIGN_RIGHT, wxString label = "0");
	wxStaticText *TextLabel(wxString);
	wxStaticText *StatusBar();
	wxTextCtrl *TextInput(int width = 80, int height = -1, wxString label = "---");
  TagBox *TextInputCombo(int width = 80, int height = -1, wxString label = "---", wxString name = "", wxString path = "");
	
	ToolBox(MainFrame *main, const wxString& title, const wxPoint& pos, const wxSize& size, int type=0, bool serve=false, bool child=false);
	ToolBox(MainFrame *main, const wxString& title, const wxPoint& pos, const wxSize& size, bool close);

	~ToolBox() {
		if(boxtype == 1) {
			winman->UnInit(); 
			delete winman;
		}
	} 

	virtual void OnToggle(wxCommandEvent& event);
};





class ToolSet
{
public:
	int i, numtools;
	ToolBox *box[50];
	//Model *mod;
	ParamStore tagindex;
	
	ToolSet() {
		numtools = 0;
		//mod = NULL;
	};
	
	void AddBox(ToolBox *newbox, bool serve=false, bool child=false); 
	
	void RemoveBox(int index) {
		box[index] = NULL;
	};

	//void RemoveBox(wxString tag) {

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
	
protected:
	virtual wxSize DoGetBestSize() const;
};


class ParamCon: public wxControl
{
public:
	int ostype;
	int type;
	int numwidth, labelwidth;
  int pad;
	wxFont textfont, smalltextfont;
	wxTextCtrl *numbox;
	wxSpinButton *spin;
	wxBoxSizer *sizer;
	wxStaticText *label;
	wxString snum;
	wxString name;
	wxString plabel;
	double value, oldvalue;
	MainFrame *mainwin;
	
	double numstep;
	int decimals;
	double min, max;
	
	ParamCon(ToolPanel *panel, int type, wxString name, wxString labelname, double initval, double step, int places, int labelwidth=60, int numwidth=45);
	ParamCon(ToolPanel *panel, wxString name, wxString labelname, wxString initval, int labelwidth, int textwidth);
	void OnSpinUp(wxSpinEvent& event);
	void OnSpinDown(wxSpinEvent& event);
    void OnSpin(wxSpinEvent& event);
	double GetValue();
	wxString GetString();
	void SetValue(double val);
	void SetPen(wxColour);
	void SetValue(wxString);
	void SetMinMax(double min, double max);
	
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
	int numparams;
	int currlay;      // Current parameter for panel layout, starts at 0
	int type;
	ToolPanel *panel;
	ParamCon *con[100];
	ParamNum *num[100];
	ParamStore ref;
	ParamStore *paramstore;
	
	ParamSet(ToolPanel *);
	~ParamSet();

	void AddNum(wxString name, wxString labelname, double initval, int places, int labelwidth=65, int numwidth=40);
	void AddCon(wxString name, wxString labelname, double initval, double step, int places, int labelwidth=60, int numwidth=60);
	void AddText(wxString name, wxString labelname, wxString initval, int labelwidth=60, int textwidth=150);
	ParamCon *GetCon(wxString);
	ParamStore *GetParams();
	ParamStore *GetParamsNew(BoxOut *boxout);
	int GetID(wxString);
	wxString GetText(wxString);
	double GetValue(wxString);
	void SetMinMax(wxString tag, double, double);
};


#endif