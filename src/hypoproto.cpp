


#include "vasomod.h"
#include <wx/notebook.h>


ProtocolBox::ProtocolBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ParamBox(model, title, pos, size, "PROTO", 1)
{
	int artnum, antinum, inpnum, rampnum;
	int numwidth;
	boxtag = "PROTO";

	long notestyle = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS;
	wxAuiNotebook *tabpanel = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notestyle);

	ToolPanel *antipanel = new ToolPanel(this, tabpanel);
	antipanel->SetFont(boxfont);
	wxBoxSizer *antibox = new wxBoxSizer(wxVERTICAL);
	antipanel->SetSizer(antibox);

	ToolPanel *inputpanel = new ToolPanel(this, tabpanel);
	inputpanel->SetFont(boxfont);
	wxBoxSizer *inputsizer = new wxBoxSizer(wxVERTICAL);
	inputpanel->SetSizer(inputsizer);

	wxStaticBoxSizer *artspikebox = new wxStaticBoxSizer(wxVERTICAL, antipanel, "Artificial Burst");
	wxStaticBoxSizer *antispikebox = new wxStaticBoxSizer(wxVERTICAL, antipanel, "Antidromic Stim");


	// Artificial Spike Panel

	activepanel = antipanel;
	paramset.panel = activepanel;
	labelwidth = 50;
	numwidth = 40;
	paramset.AddNum("artfreq", "Freq", 4, 0, labelwidth, numwidth); 
	paramset.AddNum("artburst", "Burst", 100, 0, labelwidth, numwidth); 
	paramset.AddNum("artsilence", "Silence", 50, 0, labelwidth, numwidth); 
	paramset.AddNum("startspike", "Start", 0, 0, labelwidth, numwidth); 
	paramset.AddNum("endspike", "End", 0, 0, labelwidth, numwidth); 
	artnum = paramset.numparams;

	paramset.AddNum("antistart", "Start", 0, 0, labelwidth, numwidth); 
	paramset.AddNum("antifreq", "Freq", 20, 0, labelwidth, numwidth); 
	paramset.AddNum("anticount", "Spikes", 4, 0, labelwidth, numwidth); 
	paramset.AddNum("antiperiod", "Period", 5000, 0, labelwidth, numwidth); 
	paramset.AddNum("antistim", "Count", 100, 0, labelwidth, numwidth); 
	paramset.AddNum("antipulsedur", "Pulse Dur", 5000, 0, labelwidth, numwidth); 
	paramset.AddNum("antipulsepsps", "Pulse PSP", 100, 0, labelwidth, numwidth); 
	antinum = paramset.numparams;

	for(i=0; i<artnum; i++) {
		artspikebox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		//artspikebox->AddSpacer(2);
	}

	for(i=artnum; i<antinum; i++) {
		antispikebox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		//artspikebox->AddSpacer(2);
	}

	wxBoxSizer *spikebox = new wxBoxSizer(wxHORIZONTAL);
	spikebox->Add(artspikebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 5);
	spikebox->AddStretchSpacer();
	spikebox->Add(antispikebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 5);

	antibox->AddSpacer(10);
	antibox->Add(spikebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	antipanel->Layout();


	// Synaptic Panel

	ToolPanel *synpanel = new ToolPanel(this, tabpanel);
	synpanel->SetFont(boxfont);
	wxBoxSizer *synbox = new wxBoxSizer(wxVERTICAL);
	synpanel->SetSizer(synbox);

	activepanel = synpanel;
	paramset.panel = activepanel;
	labelwidth = 50;
	numwidth = 40;

	paramset.AddNum("synstart", "Start", 0, 0, labelwidth, numwidth); 
	paramset.AddNum("synfreq", "Freq", 20, 0, labelwidth, numwidth); 
	paramset.AddNum("syncount", "Spikes", 4, 0, labelwidth, numwidth); 
	paramset.AddNum("synperiod", "Period", 5000, 0, labelwidth, numwidth); 
	paramset.AddNum("synstim", "Count", 100, 0, labelwidth, numwidth); 
	paramset.AddNum("synpulsedur", "Pulse Dur", 5000, 0, labelwidth, numwidth); 
	paramset.AddNum("synpulsepsps", "Pulse PSP", 100, 0, labelwidth, numwidth); 
	short synnum = paramset.numparams;

	wxStaticBoxSizer *synpulsebox = new wxStaticBoxSizer(wxVERTICAL, synpanel, "Synaptic Pulse");
	synpulsebox->AddSpacer(5);
	for(i=antinum; i<synnum; i++) {
		synpulsebox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		//artspikebox->AddSpacer(2);
	}
	synpulsebox->AddSpacer(10);
	AddButton(ID_RunSyn, "Run", 50, synpulsebox);

	paramset.AddNum("synratestart", "Start", 0, 0, labelwidth, numwidth); 
	paramset.AddNum("synrateamp", "Pulse Amp", 100, 0, labelwidth, numwidth); 
	paramset.AddNum("synratedur", "Pulse Dur", 1, 2, labelwidth, numwidth); 
	paramset.AddNum("synrateperiod", "Pulse Per", 100, 1, labelwidth, numwidth); 
	paramset.AddNum("synratecount", "Pulse Count", 1, 0, labelwidth, numwidth); 
	paramset.AddNum("synbaseval", "Base Start", 200, 0, labelwidth, numwidth); 
	paramset.AddNum("synstepval", "Base Step", 10, 0, labelwidth, numwidth); 
	paramset.AddNum("synnumvals", "Num Vals", 1, 0, labelwidth, numwidth); 
	short synratenum = paramset.numparams;

	wxStaticBoxSizer *synratebox = new wxStaticBoxSizer(wxVERTICAL, synpanel, "Synaptic Rate");
	synpulsebox->AddSpacer(5);
	for(i=synnum; i<synratenum; i++) {
		synratebox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		//artspikebox->AddSpacer(2);
	}
	synratebox->AddSpacer(10);
	AddButton(ID_RunSynRate, "Run", 50, synratebox);

	wxBoxSizer *synHbox = new wxBoxSizer(wxHORIZONTAL);
	synHbox->Add(synpulsebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 5);
	synHbox->AddStretchSpacer();
	synHbox->Add(synratebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 5);

	synbox->AddSpacer(15);
	synbox->Add(synHbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	synpanel->Layout();



	// Injection Panel

	ToolPanel *osmopanel = new ToolPanel(this, tabpanel);
	osmopanel->SetFont(boxfont);
	wxBoxSizer *osmopansizer = new wxBoxSizer(wxVERTICAL);
	osmopanel->SetSizer(osmopansizer);

	activepanel = osmopanel;
	paramset.panel = activepanel;
	labelwidth = 60;
	numwidth = 55;

	paramset.AddNum("injecttime", "Inject Time", 1000, 0, labelwidth, numwidth); 
	paramset.AddNum("injectstep", "Inject Step", 20, 2, labelwidth, numwidth); 
	short osmonum = paramset.numparams;

	wxStaticBoxSizer *osmobox = new wxStaticBoxSizer(wxVERTICAL, osmopanel, "Osmotic Injection");
	osmobox->AddSpacer(5);
	for(i=synratenum; i<osmonum; i++) {
		osmobox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		//artspikebox->AddSpacer(2);
	}

	paramset.AddNum("vasoinjecttime", "Inject Time", 1000, 0, labelwidth, numwidth); 
	paramset.AddNum("vasoinjectstep", "Inject Step", 20, 2, labelwidth, numwidth); 
	short vasonum = paramset.numparams;
	wxStaticBoxSizer *vasobox = new wxStaticBoxSizer(wxVERTICAL, osmopanel, "Vasopressin Injection");
	vasobox->AddSpacer(5);
	for(i=osmonum; i<vasonum; i++) {
		vasobox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	}

	wxBoxSizer *osmopanhbox = new wxBoxSizer(wxHORIZONTAL);
	osmopanhbox->Add(osmobox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	osmopanhbox->AddStretchSpacer();
	osmopanhbox->Add(vasobox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	osmopansizer->AddSpacer(15);
	osmopansizer->Add(osmopanhbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//inputsizer->AddStretchSpacer(10);
	osmopanel->Layout();




	// Input Panel

	activepanel = inputpanel;
	paramset.panel = activepanel;

	labelwidth = 50;
	numwidth = 45;

	paramset.AddNum("baseval", "Start", 200, 0, labelwidth, numwidth); 
	paramset.AddNum("stepval", "Step", 10, 0, labelwidth, numwidth); 
	paramset.AddNum("numvals", "Num Vals", 20, 0, labelwidth, numwidth); 
	paramset.AddNum("numruns", "Num Runs", 1, 0, labelwidth, numwidth); 
	inpnum = paramset.numparams;

	wxStaticBoxSizer *inputrangebox = new wxStaticBoxSizer(wxVERTICAL, inputpanel, "Input Range");
	for(i=vasonum; i<inpnum; i++) {
		inputrangebox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		//artspikebox->AddSpacer(2);
	}
	wxBoxSizer *inputrunbox = new wxBoxSizer(wxHORIZONTAL);

	inputrunbox->Add(TextLabel("Input"), 1, wxALIGN_CENTRE);
	currentinput = NumPanel(40, wxALIGN_CENTRE, "---"); 
	inputrunbox->AddSpacer(10);
	inputrunbox->Add(currentinput, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	inputrangebox->AddSpacer(10);
	inputrangebox->Add(inputrunbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	inputrangebox->AddSpacer(10);
	AddButton(ID_Run, "Run", 50, inputrangebox);

	paramset.AddNum("rampbase", "Base", 200, 0, labelwidth, numwidth); 
	paramset.AddNum("rampstart", "Start", 200, 0, labelwidth, numwidth); 
	paramset.AddNum("rampstop", "Stop", 300, 0, labelwidth, numwidth); 
	paramset.AddNum("rampinit", "Initial", 200, 0, labelwidth, numwidth); 
	paramset.AddNum("rampstep", "1s Step", 0.1, 2, labelwidth, numwidth); 
	rampnum = paramset.numparams;

	wxStaticBoxSizer *rampbox = new wxStaticBoxSizer(wxVERTICAL, inputpanel, "Input Ramp");
	for(i=inpnum; i<rampnum; i++) {
		rampbox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		//artspikebox->AddSpacer(2);
	}
	rampbox->AddSpacer(10);
	AddButton(ID_RunRamp, "Run", 50, rampbox);

	wxBoxSizer *inputparambox = new wxBoxSizer(wxHORIZONTAL);
	inputparambox->Add(inputrangebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 5);
	inputparambox->AddStretchSpacer();
	inputparambox->Add(rampbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 5);
	//leftbox->Add(artspikebox, 0);
	//leftbox->AddSpacer(20);
	inputsizer->AddSpacer(10);
	//inputsizer->Add(inputparambox, 0);
	inputsizer->Add(inputparambox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//inputsizer->AddStretchSpacer(10);
	inputpanel->Layout();

	// Main Structure

	tabpanel->Freeze();
	tabpanel->AddPage(antipanel, "Antidromic" , false);
	tabpanel->AddPage(inputpanel, "Input" , false);
	tabpanel->AddPage(osmopanel, "Injection" , false);
	tabpanel->AddPage(synpanel, "Synaptic" , false);
	tabpanel->Thaw();

	//wxPanel *storepanel = new wxPanel(this, wxID_ANY);
	ToolPanel *storepanel = panel;
	wxBoxSizer *storesizer = new wxBoxSizer(wxVERTICAL);
	storepanel->SetSizer(storesizer);

	activepanel = storepanel;
	wxSizer *paramfilebox = StoreBox("ns1", storepanel);

	status = NumPanel(150, wxALIGN_RIGHT, "");

	storesizer->Add(status, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	storesizer->Add(paramfilebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	storesizer->Layout();

	winman->AddPane(tabpanel, wxAuiPaneInfo().Name("tabpane").CentrePane().PaneBorder(false));
	winman->AddPane(storepanel, wxAuiPaneInfo().Name("storepane").Bottom().CaptionVisible(false).BestSize(-1, 60).PaneBorder(false));
	//winman->AddPane(storepanel, wxAuiPaneInfo().Name("storepane").CaptionVisible(false));

	winman->Update();

	histmode = 1;
	HistLoad();
	paramstoretag->SetLabel(initparams);

	Connect(ID_Run, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ProtocolBox::OnRun));
	Connect(ID_RunSyn, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ProtocolBox::OnRun));
	Connect(ID_RunRamp, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ProtocolBox::OnRun));
	Connect(ID_RunSynRate, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ProtocolBox::OnRun));
	//Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(InfoBox::OnClose));
	Connect(ID_paramstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ProtocolBox::OnParamStore));
	Connect(ID_paramload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ProtocolBox::OnParamLoad));
}


void ProtocolBox::OnRun(wxCommandEvent& event)
{
	(*(mod->modeflags))["proto"] = 0;
	(*(mod->modeflags))["inputproto"] = 0;

	if(event.GetId() == ID_Run) (*(mod->modeflags))["proto"] = 1;
	if(event.GetId() == ID_RunSyn) (*(mod->modeflags))["prototype"] = 1;
	if(event.GetId() == ID_RunSynRate) (*(mod->modeflags))["prototype"] = synrate;
	if(event.GetId() == ID_RunRamp) (*(mod->modeflags))["prototype"] = ramp;

	mod->RunModel();
} 
