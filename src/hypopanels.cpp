
#include <hypomodel.h>
//#include "hypomain.h"
//#include "hypopanels.h"
//#include "hypospin.h"
//#include "../art/down.xpm"
//#include "../art/up.xpm"
//#include "../art/forward.xpm"
//#include "../art/back.xpm"
#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <wx/sysopt.h>
#include <wx/dcclient.h>
#include <wx/scopedarray.h>


ScalePanel::ScalePanel(GraphWindow3 *graphw, const wxString & title)
//: ParamBox(NULL, title, wxDefaultPosition, wxSize(450, 450), "Axes", 0)
: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(250, 300),
		wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER)
{
	int i;
	int labelwidth;
	graphwin = graphw; 
	
	wxRadioButton *xrad[2];
	wxString text;

	ostype = GetSystem();

	buttonheight = 23;
	boxfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	confont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	if(ostype == Mac) {
		buttonheight = 25;
		boxfont = wxFont(12, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
		confont = wxFont(10, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	}
	column = 0;

	panel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
    panel->SetFont(boxfont);
	mainbox = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(mainbox);

	paramset = new ParamSet(panel);
	boxout = new BoxOut(NULL, graphwin->mainwin->diagbox, "Axis Panel"); 
	parambox = new wxBoxSizer(wxHORIZONTAL);

	labelwidth = 40;
    if(ostype == Mac) labelwidth = 50;
	graph = graphwin->graphset[0]->plot[0];
	paramset->AddNum("xlabels", "X Ticks", (double)graph->xlabels, 0, labelwidth);
	paramset->AddNum("xstep", "X Step", graph->xstep, 0, labelwidth);
	paramset->AddNum("ylabels", "Y Ticks", (double)graph->ylabels, 0, labelwidth);
	paramset->AddNum("ystep", "Y Step", graph->ystep, 0, labelwidth);
	ParamLayout(2);

	wxStaticBoxSizer *radbox = new wxStaticBoxSizer(wxHORIZONTAL, panel, "X Mode");
	xrad[0] = new wxRadioButton(panel, 0, "Count");
	xrad[1] = new wxRadioButton(panel, 1, "Step");
	radbox->Add(xrad[0], 1, wxTOP | wxBOTTOM, 3);
	radbox->Add(xrad[1], 1, wxTOP | wxBOTTOM, 3);
	xrad[graph->xtickmode]->SetValue(true);

	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	wxButton *okButton = new wxButton(panel, wxID_OK, "Ok", wxDefaultPosition, wxSize(70, 30));
	wxButton *closeButton = new wxButton(panel, wxID_CANCEL, "Close", wxDefaultPosition, wxSize(70, 30));
	buttonbox->Add(okButton, 1);
	buttonbox->Add(closeButton, 1, wxLEFT, 5);

	//status = StatusBar();
	status = new wxStaticText(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 
                              wxALIGN_CENTRE|wxBORDER_DOUBLE|wxST_NO_AUTORESIZE);
	status->SetFont(confont);
	wxBoxSizer *statusbox = new wxBoxSizer(wxHORIZONTAL);
	statusbox->Add(status, 1, wxEXPAND);
	
	mainbox->AddSpacer(5);
	mainbox->Add(parambox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddStretchSpacer();
	mainbox->Add(radbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 10);
	mainbox->AddStretchSpacer();
	mainbox->Add(buttonbox, 0, wxALIGN_CENTRE | wxTOP | wxBOTTOM, 10);
	mainbox->Add(statusbox, 0, wxEXPAND);

	panel->Layout();
	
	Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(ScalePanel::OnRadio));
	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScalePanel::OnOK));
	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ScalePanel::OnOK));
	
	ShowModal();
	Destroy(); 
}


void ScalePanel::OnRadio(wxCommandEvent& event)
{
	//if(event.GetId() == ID_BlankPanel) mainwin->startmod = 0;
	//if(event.GetId() == ID_IGFPanel) mainwin->startmod = 1;
	//if(event.GetId() == ID_VasoPanel) mainwin->startmod = 2;
	//if(event.GetId() == ID_VMHPanel) mainwin->startmod = 3;
	//if(event.GetId() == ID_CortPanel) mainwin->startmod = 4;
	//if(event.GetId() == ID_OsmoPanel) mainwin->startmod = 5;
	//if(event.GetId() == ID_HeatMod) mainwin->startmod = modHeat;
	//if(event.GetId() == ID_GHMod) mainwin->startmod = modGH;
	//if(event.GetId() == ID_LysisMod) mainwin->startmod = modLysis;
	//if(event.GetId() == ID_AgentMod) mainwin->startmod = modAgent;
	//mainwin->startmod = event.GetId();

	graph = graphwin->graphset[0]->plot[0];

	if(event.GetId() == 0) graph->xtickmode = 0;
	if(event.GetId() == 1) graph->xtickmode = 1;

}


void ScalePanel::OnOK(wxCommandEvent& WXUNUSED(event))
{
	long stringnum;
	wxString snum;

	ParamStore *params = paramset->GetParamsNew(boxout);
	//mainwin->SetStatus(wxT("Scale OK")); 

	graph = graphwin->graphset[0]->plot[0];
	graph->xlabels = (*params)["xlabels"];
	graph->ylabels = (*params)["ylabels"];
	graph->xstep = (*params)["xstep"];
	graph->ystep = (*params)["ystep"];
	graphwin->UpdateScroll(-1);
	
	/*
	numdrawcon->numbox->GetValue().ToLong(&stringnum);
	mainwin->numdraw = stringnum;
	
	viewheightcon->numbox->GetValue().ToLong(&stringnum);
	mainwin->viewheight = stringnum;

	ylabelcon->numbox->GetValue().ToLong(&stringnum);
	mainwin->ylabels = stringnum;

	datsamplecon->numbox->GetValue().ToLong(&stringnum);
	mainwin->datsample = stringnum;

	mainwin->parampath = parampathcon->GetValue();
	mainwin->datapath = datapathcon->GetValue();
	mainwin->outpath = outpathcon->GetValue();
	mainwin->modpath = modpathcon->GetValue();
	
	snum.Printf("ok numdraw %d", mainwin->numdraw);
	mainwin->SetStatus(snum);
	*/
	Close();
}


void ScalePanel::ParamLayout(int columns)
{
	int i;
	int colsize = 0;

	if(columns != 2) colsize = paramset->numparams;
	if(columns == 2) {
		if(!column) colsize = (paramset->numparams+1) / 2;
		else colsize = column; 
	}

	//SetVBox(columns);
	for(i=0; i<columns; i++) {
		vbox[i] = new wxBoxSizer(wxVERTICAL);
		vbox[i]->AddSpacer(5);
	}
	
	for(i=0; i<colsize; i++) {
		vbox[0]->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		vbox[0]->AddSpacer(5);
	}
	parambox->Add(vbox[0], 0);
	
	if(columns == 2) {
		for(i=colsize; i<paramset->numparams; i++) {
			vbox[1]->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
			vbox[1]->AddSpacer(5);
		}
		parambox->Add(vbox[1], 0);
	}
}


ParamBox::ParamBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, wxString name, int type)
: ToolBox(model->mainwin, title, pos, size, type)
//wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX)
{	
	autorun = 0;
	//blankevent = new wxCommandEvent();
	boxname = name;
	redtag = "";
	histmode = 0;
	mod = model;
	boxtype = type;
	//status = NULL;
	defbutt = 0;
	mainwin = mod->mainwin;

	Initialise();
}


/*
ParamBox::ParamBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, bool close)
: ToolBox(model->mainwin, title, pos, size, close)
{	
	autorun = 0;
	boxname = "";
	redtag = "";
	histmode = 0;
	mod = model;
	boxtype = 0;

	Initialise();
}*/


ParamBox::~ParamBox()
{
	delete modparams;
	delete modflags;
	delete paramset;
	delete flagrefs;
	delete checkrefs;

	//delete vbox1;
	//delete vbox2;
}


void ParamBox::SetVBox(int num)
{
	int i;

	for(i=0; i<num; i++) {
		vbox[i] = new wxBoxSizer(wxVERTICAL);
		vbox[i]->AddSpacer(5);
	}
}


void ParamBox::SetStatus(wxString text)
{
	//status = NULL;

	if(status != NULL) status->SetLabel(text);

}

	
void ParamBox::Initialise()
{	
	modparams = new ParamStore;
	modflags = new ParamStore;
	activepanel = panel;
	paramset = new ParamSet(activepanel);
	flagrefs = new RefStore();
	checkrefs = new RefStore();
	//if(boxtype == 0 || boxtype == 1) {
	if(boxtype == 0 || boxtype == 1) {
		paramstoretag = TextInputCombo(100, 20, "", boxname, mod->GetPath());
		paramstoretag->Show(false);
	}
	else paramstoretag = NULL;
	//flagset = new FlagSet();
	
	//panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize); 
	//panel->SetFont(boxfont);
	
	//vbox1 = new wxBoxSizer(wxVERTICAL);
	//vbox2 = new wxBoxSizer(wxVERTICAL);
	//vbox3 = new wxBoxSizer(wxVERTICAL);
	//vbox4 = new wxBoxSizer(wxVERTICAL);
	//vbox5 = new wxBoxSizer(wxVERTICAL);
	
	if(boxtype == 0) parambox = new wxBoxSizer(wxHORIZONTAL);
	//mainbox = new wxBoxSizer(wxVERTICAL);
	
	//vbox1->AddSpacer(5);
	//vbox2->AddSpacer(5);
	//vbox3->AddSpacer(5);
	//vbox4->AddSpacer(5);
	//vbox5->AddSpacer(5);
	
	//panel->SetSizer(mainbox);
	
	Connect(ID_autorun, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ParamBox::OnAutorun));
	Connect(ID_Run, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ParamBox::OnRun));
	Connect(ID_Default, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ParamBox::OnDefault));
	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ParamBox::OnRun));
	//Connect(wxEVT_MOVE, wxMoveEventHandler(ParamBox::OnMove));
	//Connect(wxEVT_SIZE, wxSizeEventHandler(ParamBox::OnSize));
	Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(ParamBox::OnFocus));
	Connect(wxEVT_SPIN, wxSpinEventHandler(ParamBox::OnSpin));
	Connect(ID_paramstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ParamBox::OnParamStore));
	Connect(ID_paramload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ParamBox::OnParamLoad));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ParamBox::OnClose));
}


void ParamBox::OnDefault(wxCommandEvent& event)
{
	ParamLoad("default");
	if(autorun) OnRun(event);
}


void ParamBox::OnClose(wxCloseEvent& event)
{
	if(histmode) HistStore();
	ToolBox::OnClose(event);
}


void ParamBox::ParamLayout(int columns)
{
	int colsize = 0;

	if(columns != 2) colsize = paramset->numparams;
	if(columns == 2) {
		if(!column) colsize = (paramset->numparams+1) / 2;
		else colsize = column; 
	}

	SetVBox(columns);
	
	for(i=0; i<colsize; i++) {
		vbox[0]->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		vbox[0]->AddSpacer(5);
	}
	parambox->Add(vbox[0], 0);
	
	if(columns == 2) {
		for(i=colsize; i<paramset->numparams; i++) {
			vbox[1]->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
			vbox[1]->AddSpacer(5);
		}
		parambox->Add(vbox[1], 0);
	}
}


void ParamBox::HistStore()
{
	short i;
	wxString filename, filepath;
	wxString outline;
	
	filepath = mod->GetPath();
	filename = filepath + "/" + boxname + "-hist.ini";
	initparams = paramstoretag->GetValue();
	
	wxTextFile opfile(filename);
	if(!opfile.Exists()) opfile.Create();
	opfile.Open();
	opfile.Clear();
	for(i=paramstoretag->GetCount()-1; i>=0; i--) {
		outline.Printf("initparams %s", paramstoretag->GetString(i));
		opfile.AddLine(outline);
	}
	opfile.Write();
	opfile.Close();
}


void ParamBox::HistLoad()
{
	//long numdat;
	
	wxString filename, filepath;
	wxString readline, numstring;
	
	filepath = mod->GetPath();
	filename = filepath + "/" + boxname + "-hist.ini";
	
	wxTextFile opfile(filename);
	
	if(!opfile.Exists()) {
		return;
	}
	
	opfile.Open();
	readline = opfile.GetLine(0);
	while(!readline.IsEmpty()) {
		readline = readline.AfterFirst(' ');
		readline.Trim();
		initparams = readline;
		paramstoretag->Insert(initparams, 0);
		readline = opfile.GetNextLine();
	}
	
	opfile.Close();	
}


wxBoxSizer *ParamBox::StoreBox(wxString label, wxPanel *storepanel)
{
	wxBoxSizer *paramfilebox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *parambuttons = new wxBoxSizer(wxHORIZONTAL);

	if(!storepanel) storepanel = panel;
	
	//paramstoretag = TextInput(100, 20, label);
	//if(!paramstoretag) paramstoretag = TextInputCombo(100, 20, "");
	if(activepanel != panel) paramstoretag->Reparent(activepanel);

	if(label != "") paramstoretag->SetLabel(label);
	paramstoretag->Show(true);
	if(ostype == Mac) {
		AddButton(ID_paramstore, "Store", 60, parambuttons);
		AddButton(ID_paramload, "Load", 60, parambuttons);
	}
	else {
		AddButton(ID_paramstore, "Store", 40, parambuttons);
		parambuttons->AddSpacer(2);
		AddButton(ID_paramload, "Load", 40, parambuttons);
	}	
	paramfilebox->Add(paramstoretag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	paramfilebox->Add(parambuttons, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	return paramfilebox;
}


void ParamBox::OnFocus(wxFocusEvent& event)
{
	mainwin->SetStatusText("Box Focus");
}


wxCheckBox *ParamBox::SetModCheck(int id, wxString checktag, wxString checktext, int state)
{
	wxCheckBox *newcheck;
	(*modflags)[checktag] = state;
	newcheck = new wxCheckBox(panel, id, checktext);
	newcheck->SetFont(confont);
	newcheck->SetValue(state);
	checkrefs->AddRef(id, checktag, newcheck);
	Connect(id, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ParamBox::OnCheck));

	return newcheck;
}


void ParamBox::SetModFlag(int id, wxString flagname, wxString flagtext, int state, wxMenu *menu)
{
	if(menu == NULL) menu = menuModel;
	(*modflags)[flagname] = state;
	flagrefs->AddRef(id, flagname);
	menu->Append(id, flagtext, "Toggle " + flagtext, wxITEM_CHECK);
	menu->Check(id, state);
	Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ParamBox::OnFlag));
}


void ParamBox::SetCheck(wxCheckBox *checkbox, bool state)
{
	wxString checktag = checkrefs->GetRef(checkbox->GetId());
	(*modflags)[checktag] = state;
	checkbox->SetValue(state);
}


void ParamBox::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();
	wxString checktag = checkrefs->GetRef(id);

	if((*modflags)[checktag] == 0) (*modflags)[checktag] = 1;
	else (*modflags)[checktag] = 0;
}


void ParamBox::OnFlag(wxCommandEvent& event)
{
	int id = event.GetId();
	wxString flag = flagrefs->GetRef(id);
	
	if((*modflags)[flag] == 0) (*modflags)[flag] = 1;
	else (*modflags)[flag] = 0;

	if(autorun) OnRun(event);
}


void ParamBox::InitMenu()
{
	menuControls = new wxMenu;
	menuControls->Append(ID_autorun, "Auto Run", "Toggle Autorun", wxITEM_CHECK);
	menuControls->Check(ID_autorun, autorun);
	
	menuModel = new wxMenu;
	
	//menuParamSet = new wxMenu;
	
	menuBar = new wxMenuBar;

	menuBar->Append(menuControls, "Controls");
	menuBar->Append(menuModel, "Model");
	//menuBar->Append(menuParamSet, "Parameters");
	SetMenuBar(menuBar);
}


void ParamBox::SetCount(double count)
{
	wxString text; 

	//if(mainwin->diagnostic) mod->diagbox->textbox->AppendText(text.Format("Count call, count %.2f, countmark %.2f\n", count, countmark));
	if(count > countmark + 0.5) {
		countmark = count;
		snum.Printf("%.0f %%", count);
		runcount->SetLabel(snum);
	}
	//runcount->SetLabel("test");
}

/*
void ParamBox::OnRun(wxCommandEvent& WXUNUSED(event))
{
	mainwin->SetStatus("ParamBox Run");
	countmark = 0;
	GetParams();
	mainwin->RunModel(this);

	//for(i=0; i<numparams; i++)
	//	paramset[i]->numbox->GetValue().ToDouble(&((*modparams)[paramset[i]->name]));
}*/


void ParamBox::OnRun(wxCommandEvent& WXUNUSED(event))
{
	if(mainwin->diagnostic) mainwin->SetStatusText(boxname + " Run");

	countmark = 0;
	//GetParams();
	mod->RunModel();
}


wxBoxSizer *ParamBox::RunBox()
{
	wxBoxSizer *runbox = new wxBoxSizer(wxHORIZONTAL);
	runcount = NumPanel(50, wxALIGN_CENTRE, "---");
	if(ostype == Mac) AddButton(ID_Run, "RUN", 50, runbox);
    else AddButton(ID_Run, "RUN", 70, runbox);
	runbox->AddSpacer(5);
	runbox->Add(runcount, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	if(defbutt) {
		runbox->AddSpacer(5);
		if(ostype == Mac) AddButton(ID_Default, "RESET", 50, runbox);
        else AddButton(ID_Default, "RESET", 70, runbox);
	}

	return runbox;
}


void ParamBox::OnSpin(wxSpinEvent& event)
{
	if(mainwin->diagnostic) mainwin->SetStatusText("on spin");
	if(autorun) OnRun(event);
}


void ParamBox::OnAutorun(wxCommandEvent& WXUNUSED(event))
{
	wxString fontname;
	
	autorun = 1 - autorun;
  //wxFont fontcheck = runbutton->GetFont();
	//fontname = fontcheck.GetNativeFontInfoUserDesc();
	//mainwin->SetStatusText("Font: " + fontname);	
}


/*
ParamStore *ParamBox::GetParams(ParamStore *pstore)
{ 
	double value;
	if(pstore == NULL) pstore = modparams;
	for(i=0; i<paramset->numparams; i++)
		paramset->con[i]->numbox->GetValue().ToDouble(&((*pstore)[paramset->con[i]->name]));
	return pstore;
}*/


ParamStore *ParamBox::GetParams(ParamStore *pstore)
{ 
	double value;
	ParamCon *con;
	wxString text;

	//SetStatus("");

	//if(mainwin->diagnostic) mod->diagbox->textbox->AppendText(text.Format("%s get params\n", boxname));

	if(pstore == NULL) pstore = modparams;
	for(i=0; i<paramset->numparams; i++) {
		con = paramset->con[i];
		value = con->GetValue();
		if(value < con->min) {
			//value = con->min;
			value = con->oldvalue;
			con->SetValue(value);
			SetStatus(text.Format("Parameter \'%s\' out of range", con->label->GetLabel()));
			mainwin->diagbox->Write(text.Format("Parameter \'%s\' out of range, min %.2f max %.2f\n", con->label->GetLabel(), con->min, con->max));
		}
		if(value > con->max) {
			//value = con->max;
			value = con->oldvalue;
			con->SetValue(value);
			SetStatus(text.Format("Parameter %s out of range", con->label->GetLabel()));
		}
		//paramset->con[i]->numbox->GetValue().ToDouble(&((*pstore)[paramset->con[i]->name]));
		(*pstore)[paramset->con[i]->name] = value;
		con->oldvalue = value;
	}

	return pstore;
}



ParamStore *ParamBox::GetNumParams()
{ 
	for(i=0; i<paramset->numparams; i++)
		paramset->num[i]->numbox->GetValue().ToDouble(&((*modparams)[paramset->num[i]->name]));
	return modparams;
}


void ParamBox::OnParamLoad(wxCommandEvent& event)
{
	ParamLoad();
}


void ParamBox::ParamLoad(wxString tag)
{
	int id;
	long flagval;
	double datval;
	short diagnostic;
	wxString filetag, filename, filepath;
	wxString readline, datname;
	wxColour redpen("#dd0000"), blackpen("#000000");
	//TextFile ofp;
	wxString text;

	diagnostic = 0;

	//mod->diagbox->Write(text.Format("param load %s\n", boxname));

	//if(mod->path == "") filepath = mod->mainwin->parampath;
	//else filepath = mod->path + "/Params";
	filepath = mod->GetPath() + "/Params";

	//if(diagnostic) ofp.New(boxname + "paramload.txt");
	
	// Param data file

	//if(diagnostic) ofp.WriteLine(text.Format("tag %s", tag));
	if(tag == "") filetag = paramstoretag->GetValue();
	else filetag = tag;

	filename = filepath + "/" + filetag + "-" + boxname + "param.dat";
	
	wxTextFile paramfile(filename);
	
	if(!paramfile.Exists()) {
		if(paramstoretag) paramstoretag->SetValue("Not found");
		//if(diagnostic) ofp.Close();
		return;
	}

	// Param file history
	if(tag != "default") {
		short tagpos = paramstoretag->FindString(filetag);
		if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
		paramstoretag->Insert(filetag, 0);

		redtag = "";
		paramstoretag->SetForegroundColour(blackpen);
		paramstoretag->SetValue("");
		paramstoretag->SetValue(filetag);
	}
	
	paramfile.Open();
	//numparams = paramfile.GetLineCount();
	readline = paramfile.GetFirstLine();
	
	//for(i=0; i<numparams; i++) {
	while(!readline.IsEmpty()) {
		//if(readline.IsEmpty()) continue;
		//storetag->SetValue(readline);
		datname = readline.BeforeFirst(' ');
		if(datname == "GKleak") datname = "gKL";
		readline = readline.AfterFirst(' ');
		//sdat = readline.BeforeFirst('k');
		readline.Trim();
		//storetag->SetValue(readline);
		readline.ToDouble(&datval);
		if(paramset->ref.check(datname)) {
			id = paramset->ref[datname];
			paramset->con[id]->SetValue(datval);
		}
		//if(diagnostic) ofp.WriteLine(text.Format("Model Param ID %d, Value %.4f\n", id, datval)); 
		if(paramfile.Eof()) return;
		readline = paramfile.GetNextLine();	
	}
	readline = paramfile.GetNextLine();
	//if(paramfile.Eof()) break;
	//while(!paramfile.Eof()) {
	while(!readline.IsEmpty()) {
		datname = readline.BeforeFirst(' ');
		readline = readline.AfterFirst(' ');
		readline.Trim();
		readline.ToLong(&flagval);
		(*modflags)[datname] = flagval;
		id = flagrefs->GetID(datname);
		if(id) menuModel->Check(id, flagval);
		//if(diagnostic) ofp.WriteLine(text.Format("Model flag ID %d, Set %d\n", id, flagval)); 
		readline = paramfile.GetNextLine();	
	}

	if(!paramfile.Eof()) readline = paramfile.GetNextLine();
	else readline = "";

	//readline = paramfile.GetNextLine();
	while(!readline.IsEmpty()) {
		datname = readline.BeforeFirst(' ');
		readline = readline.AfterFirst(' ');
		readline.Trim();
		readline.ToLong(&flagval);
		(*modflags)[datname] = flagval;
		id = checkrefs->GetID(datname);
		//menuModel->Check(id, flagval);
		((wxCheckBox *)checkrefs->GetBox(id))->SetValue(flagval);
		//if(diagnostic) ofp.WriteLine(text.Format("Model flag ID %d, Set %d\n", id, flagval)); 
		readline = paramfile.GetNextLine();	
	}
	paramfile.Close();
	//if(diagnostic) ofp.Close();
}


void ParamBox::OnParamStore(wxCommandEvent& event)
{
	wxString filetag, filename, filepath;
	wxString outline;
	wxColour redpen("#dd0000"), blackpen("#000000");
	wxString text;

	//mod->diagbox->Write(text.Format("param store %s\n", boxname));

	//if(mod->path == "") filepath = mod->mainwin->parampath;
	//else filepath = mod->path + "/Params";
	//if(!wxDirExists(filepath)) wxMkdir(filepath);
	//filepath = mod->GetPath();
	filepath = mod->GetPath() + "/Params";
	
	// Param data file
	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-" + boxname + "param.dat";

	// Param file history
	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);
	
	wxTextFile paramfile(filename);
	if(!paramfile.Exists()) paramfile.Create();
	else if(redtag != filetag) {
		paramstoretag->SetForegroundColour(redpen);
		paramstoretag->SetValue("");
		paramstoretag->SetValue(filetag);
		redtag = filetag;
		return;
	}
	
	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);
	
	paramfile.Open();
	paramfile.Clear();
	
	for(i=0; i<paramset->numparams; i++) {
		paramset->con[i]->numbox->GetValue().ToDouble(&((*modparams)[paramset->con[i]->name]));
		outline.Printf("%.8f", (*modparams)[paramset->con[i]->name]);  
		paramfile.AddLine(paramset->con[i]->name + " " + outline);
	}
	paramfile.AddLine("");
	for(i=0; i<flagrefs->numrefs; i++) {
		outline.Printf("%.0f", (*modflags)[flagrefs->refbase[i].label]);  
		paramfile.AddLine(flagrefs->refbase[i].label + " " + outline);
	}
	paramfile.AddLine("");
	for(i=0; i<checkrefs->numrefs; i++) {
		outline.Printf("%.0f", (*modflags)[checkrefs->refbase[i].label]);  
		paramfile.AddLine(checkrefs->refbase[i].label + " " + outline);
	}
	paramfile.Write();
	paramfile.Close();
}


