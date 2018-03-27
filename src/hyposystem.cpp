

#include "hypomodel.h"
//#include "hypomain.h"
//#include "hypopanels.h"


OptionPanel::OptionPanel(HypoMain *main, const wxString & title)
: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(450, 450),
		wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER)
{
	int i;
	int nummods = 9;
	mainwin = main; 
	wxPanel *panel = new wxPanel(this, -1);
	wxRadioButton *modrad[20];
	wxString text;
	
	wxBoxSizer *mainbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *panelbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *panelhbox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *prefbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *pathbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	
	wxStaticBoxSizer *startbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Start Model");
	
	
	numdrawcon = new ParamNum(panel, "numdraw", "Num Draw", mainwin->numdraw);
	viewheightcon = new ParamNum(panel, "viewheight", "View Height", mainwin->viewheight);
	ylabelcon = new ParamNum(panel, "ylabels", "Y Labels", mainwin->ylabels);
	datsamplecon = new ParamNum(panel, "datsample", "Data Sample Rate", mainwin->datsample);

	datapathcon = new ParamText(panel, "datapath", "Data", mainwin->datapath, 30, 320);
	datapathcon->AddButton("Path", ID_DataBrowse, 40);
	outpathcon = new ParamText(panel, "outpath", "Out", mainwin->outpath, 30, 320);
	outpathcon->AddButton("Path", ID_OutputBrowse, 40);
	parampathcon = new ParamText(panel, "browsepath", "Param", mainwin->parampath, 30, 320);
	parampathcon->AddButton("Path", ID_ParamBrowse, 40);
	modpathcon = new ParamText(panel, "modpath", "Mod", mainwin->modpath, 30, 320);
	modpathcon->AddButton("Path", ID_ModBrowse, 40);

	nummods = mainwin->moddex - 1;
	nummods = mainwin->modset.modcount;

	//modrad[0] = new wxRadioButton(panel, 0, "Blank", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	//startbox->Add(modrad[0], 1, wxTOP | wxBOTTOM, 3);
	
	for(i=0; i<nummods; i++) {
		modrad[i+1] = new wxRadioButton(panel, mainwin->modset.modeldat[i].index, mainwin->modset.modeldat[i].title);
		mainwin->diagbox->Write(text.Format("button %d %s\n", mainwin->modset.modeldat[i].index, mainwin->modset.modeldat[i].title)); 
		startbox->Add(modrad[i+1], 1, wxTOP | wxBOTTOM, 3);
	}
	modrad[mainwin->modset.GetDex(mainwin->startmod) + 1]->SetValue(true);

	mainwin->diagbox->Write(text.Format("startmod %d button %d\n", mainwin->startmod, mainwin->modset.GetDex(mainwin->startmod) + 1));
	
	prefbox->AddSpacer(10);
	prefbox->Add(numdrawcon, 1);
	prefbox->Add(viewheightcon, 1);
	prefbox->Add(ylabelcon, 1);
	prefbox->Add(datsamplecon, 1);

	panelhbox->Add(startbox, 0);
	panelhbox->AddSpacer(20);
	panelhbox->Add(prefbox, 0);

	pathbox->Add(datapathcon, 1);
	pathbox->Add(parampathcon, 1);
	pathbox->Add(outpathcon, 1);
	pathbox->Add(modpathcon, 1);

	panelbox->Add(panelhbox, 0);
	panelbox->AddStretchSpacer(5);
	panelbox->Add(pathbox, 0);

	panel->SetSizer(panelbox);
	panel->Layout();
	
	wxButton *okButton = new wxButton(this, wxID_OK, "Ok", wxDefaultPosition, wxSize(70, 30));
	wxButton *closeButton = new wxButton(this, wxID_CANCEL, "Close", wxDefaultPosition, wxSize(70, 30));
	buttonbox->Add(okButton, 1);
	buttonbox->Add(closeButton, 1, wxLEFT, 5);
	mainbox->Add(panel, 1, wxALL, 10);
	mainbox->Add(buttonbox, 0, wxALIGN_CENTRE | wxTOP | wxBOTTOM, 10);
	SetSizer(mainbox);
	Layout();
	
	Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(OptionPanel::OnModRadio));
	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OptionPanel::OnOK));
	Connect(ID_DataBrowse, ID_ModBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OptionPanel::OnBrowse));
	//Connect(ID_OutputBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OptionPanel::OnBrowse));
	
	ShowModal();
	Destroy(); 
}


void OptionPanel::OnModRadio(wxCommandEvent& event)
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
	mainwin->startmod = event.GetId();
}


void OptionPanel::OnOK(wxCommandEvent& WXUNUSED(event))
{
	long stringnum;
	wxString snum;
	//mainwin->SetStatus(wxT("Scale OK")); 
	
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
	
	Close();
}


void OptionPanel::OnBrowse(wxCommandEvent& event)
{
	int i;

	if(event.GetId() == ID_DataBrowse) {
		wxDirDialog *d = new wxDirDialog(this, "Choose a directory", mainwin->datapath, 0, wxDefaultPosition);
		if(d->ShowModal() == wxID_OK) datapathcon->textbox->SetLabel(d->GetPath()); 
	}
	if(event.GetId() == ID_OutputBrowse) {
		wxDirDialog *d = new wxDirDialog(this, "Choose a directory", mainwin->outpath, 0, wxDefaultPosition);
		if(d->ShowModal() == wxID_OK) outpathcon->textbox->SetLabel(d->GetPath()); 
	}
	if(event.GetId() == ID_ParamBrowse) {
		wxDirDialog *d = new wxDirDialog(this, "Choose a directory", mainwin->parampath, 0, wxDefaultPosition);
		if(d->ShowModal() == wxID_OK) parampathcon->textbox->SetLabel(d->GetPath()); 
	}
	if(event.GetId() == ID_ModBrowse) {
		wxDirDialog *d = new wxDirDialog(this, "Choose a directory", mainwin->modpath, 0, wxDefaultPosition);
		if(d->ShowModal() == wxID_OK) modpathcon->textbox->SetLabel(d->GetPath()); 
	}
}



