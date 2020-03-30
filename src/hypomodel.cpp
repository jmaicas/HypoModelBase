

#include <hypomodel.h>
#include <wx/print.h>
#include "hypodef.h"


#ifdef OSX
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFString.h>
#endif // OSX

IMPLEMENT_APP(HypoApp)

MainFrame *mainframe;


HypoMain::HypoMain(const wxString& title, const wxPoint& pos, const wxSize& size, wxString path)
	: MainFrame(title, pos, size, path)
{
	wxIcon icon;
	wxColour backcolour, panelcolour;

	backcolour.Set("#ffffff");
	//panelcolour.Set("#eeeeee");     // grey
	//panelcolour.Set("#ccdaff");     // pale blue
	//panelcolour.Set("#ccffcc");       // pale green
	//panelcolour.Set("#ece9d8");     // xp default
	//panelcolour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	
	SetMinSize(wxSize(200, 670));

	// Flicker Protection

	// Check System
	wxString oslabel = wxGetOsDescription();
	SetStatusText(oslabel);
	mainframe = this;

	// Menu Flag System
	flagrefs = new RefStore();
	hypoflags = new ParamStore;
    
    // Default Prefs
	numgraphs = 8;
	numdraw = 3;
	numdraw2 = 3;
	activemod = 0;
	startmod = 13;
	pinmode = 0;
	basic = 1;
	user = 1;
	project = 1;
	diagnostic = 0;
	xstretch = 50;
	modpath = "";
    
    
    // OSX Path Check
    if(ostype == Mac) {
        homepath = getenv("HOME");
        diagbox->Write(text.Format("homepath %s\n", homepath));
        hypopath = homepath + "/Library/Application Support/HypoMod/";
        diagbox->Write(text.Format("hypopath %s\n", hypopath));
        
        if(wxDirExists(hypopath)) {
            diagbox->Write("HypoMod directory found\n");
            mainpath = hypopath;
        }
        else {
            diagbox->Write("HypoMod directory not found, using default resources\n");
            mainpath = respath;
        }
        diagbox->Write(text.Format("mainpath %s\n", mainpath));
    }
    
	OptionLoad();
	//startmod = modPlot; // modVMN;   // fix model for PLOS release   25/6/18
	//ViewLoad();
    
	toolpath = mainpath + "Tools";
    //if(modpath == "") modpath = mainpath;
    diagbox->Write("modpath " + modpath + "\n");

	MainLoad();

	spikedisp = 0;   // spike display mode   0 = plain   1 = burst   2 = hazard filter

	diagbox->Write("HypoMain initialisation\n\n");

	gpos = new GraphDisp[numgraphs];

	filebase = new FileBase(100);


	//currdata = new CurrentDat();
	//realdata = new SpikeDat();
	//burstdata = new BurstDat();
	//burstdata->spikedata = vasodata->spikedat;
	//analysisdata = new AnaDat();
	//expdata = new SpikeDat();
	//expdata->burstdata = new BurstDat();
	//expdata->burstdata->spikedata = expdata;

	// Spike Analysis Module                  July 2015
	expdata = new SpikeDat();
	expdata->burstdata = new BurstDat();
	expdata->burstdata->spikedata = expdata;
	expdata->diagbox = diagbox;

	selectdata = new SpikeDat();
	burstdata = new BurstDat();

	expdata2 = new SpikeDat();
	//expdata2->burstdata = NULL;


	//GraphData();

	burstbox = NULL;
	infobox = NULL;
	protocolbox = NULL;
	focusdata = NULL;
	dispwin = NULL;
	scalebox = NULL;
	//diagbox->Show();

	SetBackgroundColour(backcolour);
	SetIcon(icon);
	//Centre();

	mainsizer = new wxBoxSizer(wxHORIZONTAL);
	//wxBoxSizer *graphsizer = new wxBoxSizer(wxVERTICAL);
	graphsizer = new wxBoxSizer(wxVERTICAL);
	mainpos = GetPosition();

	//infobox = new InfoBox(this, "Info", wxPoint(mainpos.x+700, mainpos.y+5), wxSize(200, 300));
	//infobox = new InfoBox(this, "Info", wxPoint(320, 155), wxSize(200, 300));
	//toolset.AddBox(infobox);

	//scalebox->SetBackgroundColour(panelcolour);


	//scalebox = new ScaleBox(this, wxSize(80, 775), numdraw, gpos, greg, graphwin);
	//scalebox = new ScaleBox(this, wxDefaultSize, numdraw, gpos, graphbase, graphwin);


	if(basic) BasicMenu();
	else if(user) UserMenu();
	else UserMenu(); // FullMenu();

	//GraphOut();
	//graphpos = 0;
	//ofp = fopen("graphdiag.txt", "w");
	//wxofp = new wxTextFile("graphdiag.txt");
	//wxofp->Create();
	//wxofp->Open();
	//wxofp->Clear();

	//scalebox->GraphUpdate();

	scaletype = 0;

	diagbox->Write("HypoMain model start\n\n");

	moddex = 1;
	mod = NULL;

	ModInit();

	diagbox->Write("HypoMain graph start\n\n");

	if(!mod) {
		wxPanel *defpan = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		graphsizer->Add(defpan, 0, wxEXPAND);
		numdraw = 0;
		diagbox->Show();
		//return;
	}
    else {  
		if(mod->prefstore.check("numdraw")) {
			numdraw = mod->prefstore["numdraw"];
			diagbox->Write(text.Format("HypoMain init, numdraw set to %d\n", numdraw));
		}
		else diagbox->Write("mod numdraw not found\n");

		if(mod->gcount < numdraw || numdraw < 0) {
			diagbox->Write(text.Format("HypoMain init, out of range, numdraw set to %d\n", numdraw));
			numdraw = mod->gcount;
		}

		for(graph = 0; graph<numdraw; graph++) {
			//fprintf(ofp, "graph %d  numplots %d  plot0 title %s  gparam %d\n", graph,
			//gpos[graph].numplots, gpos[graph].plot[0]->gname.mb_str(), gpos[graph].plot[0]->gparam);
			//outline.Printf("graph %d  numplots %d  plot0 title %s  gparam %d\n",
			//graph, gpos[graph].numplots, gpos[graph].plot[0]->gname, gpos[graph].plot[0]->gparam);
			//wxofp->AddLine(outline);
			//fflush(ofp);

			graphwin[graph] = new GraphWindow3(this, this, mod, wxPoint(0, graph*250 + 10), wxSize(100, 255), &gpos[graph], graph);
			graphwin[graph]->FrontGraph(&gpos[graph]);
			graphsizer->Add(graphwin[graph], 1, wxEXPAND);

			//graphwin[graph]->Refresh();
		}

		//if(diagnostic) mod->diagbox->textbox->AppendText("scalebox call\n");
		scalebox = new ScaleBox(this, this, wxDefaultSize, numdraw, gpos, mod, graphwin, 0, scaletype);
		scalebox->GraphSwitch(0);

	
		//if(diagnostic) mod->diagbox->textbox->AppendText("scalebox call ok\n");
		if(mod->graphload) scalebox->GLoad("default");

		if (mod->gsync) {
			scalebox->gsynch = 1;
			if(ostype != Mac) scalebox->syncbutton->SetValue(true);
		}
		scalebox->xmin = mod->xmin;

		diagbox->Write(text.Format("mod path = %s\n", mod->GetPath()));

		graphsizer->AddSpacer(5);
		if(scalebox) mainsizer->Add(scalebox, 0, wxEXPAND);
		//mainsizer->Add(scalebox, 1);
		mainsizer->Add(graphsizer, 7, wxEXPAND);    
    }
    
    if(ostype == Mac && !wxDirExists(hypopath)) {
        diagbox->Write("No HypoMod directory, creating\n");
        wxMkdir(hypopath);
        mainpath = hypopath;
        tagset->UpdatePath();
        if(mod) {
            wxMkdir(hypopath + mod->path);
            mod->DataCopy(respath, hypopath);
        }
    }
    diagbox->Write(text.Format("mainpath %s\n", mainpath));
    
    //if(modpath == "") modpath = mainpath;
    //diagbox->Write("modpath " + modpath + "\n");
    
	SetSizer(mainsizer);
	Layout();
    
    this->Restore();
    this->Raise();


	//long valmax;

	//valmax = std::numeric_limits<int>::max();
	//diagbox->Write(text.Format("max int value %d\n\n", valmax));

	//double doubmax = std::numeric_limits<double>::max();
	//diagbox->Write(text.Format("max int value %.10f\n\n", doubmax));

	//OnModGen(wxCommandEvent());


	//Connect(wxEVT_PAINT, wxPaintEventHandler(MainFrame::OnPaint));
	Connect(wxEVT_MOVE, wxMoveEventHandler(HypoMain::OnMove));
	Connect(wxEVT_SIZE, wxSizeEventHandler(HypoMain::OnSize));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(HypoMain::OnClose));
	//Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MainFrame::OnDClick));
	//Connect(wxEVT_MOTION, wxMouseEventHandler(MainFrame::OnDClick));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(HypoMain::OnDClick));

	Connect(ID_Quit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnQuit));
	Connect(ID_About, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnAbout));
	Connect(ID_IGFMod, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnParams));
	Connect(ID_CortMod, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnCortMod));
	Connect(ID_OsmoMod, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnOsmoMod));
	Connect(ID_HeatMod, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnHeatMod));
	Connect(ID_GHMod, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnGHMod));
	Connect(ID_Output, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnOutput));
	Connect(ID_Info, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnInfo));
	Connect(ID_Burst, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnBurst));
	Connect(ID_Options, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnOptionPanel));
	Connect(ID_Display, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnNewDisp));
	Connect(ID_Protocol, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnProtocol));
	Connect(ID_Sound, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnSound));
	Connect(ID_GraphAdd, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnGraphAdd));
	Connect(ID_ModGen, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnModGen));
	Connect(ID_Diag, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnDiagBox));
	Connect(ID_Grid, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnGridBox));
	Connect(ID_Neuro, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnNeuroBox));
	Connect(ID_Plot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnPlotBox));
	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(HypoMain::OnEnter));
}


HypoMain::~HypoMain()
{

	// See HypoMain::CleanUp();

	//delete flagrefs;
	//delete hypoflags;

	//delete expdata;
	//delete expdata2;
	//delete selectdata;
}


void HypoMain::ToolLoad()
{
	if((*mod->toolflags)["burstbox"]) SpikeBox(2); 

	if((*mod->toolflags)["spikebox"]) SpikeBox(1); 
}


void HypoMain::OnGraphAdd(wxCommandEvent& WXUNUSED(event))
{
	SetStatusText("Add Graph");

	AddGraph();
}


void HypoMain::AddGraph()
{
	graph = numdraw;

	graphwin[graph] = new GraphWindow3(this, this, mod, wxPoint(0, graph*250 + 10), wxSize(100, 255), &gpos[graph], graph);
	graphwin[graph]->FrontGraph(&gpos[graph]);
	graphsizer->Add(graphwin[graph], 1, wxEXPAND);

	//Refresh();

	numdraw++;
	Layout();
}


void HypoMain::RemoveGraph(int gindex)
{
	SetStatusText("Remove Graph");

	graphsizer->Detach(graphwin[gindex]);
	numdraw--;
	graphwin[gindex]->Hide();

	//graphsizer->Remove(
}


void HypoMain::CleanUp() {

	//for(i=0; i<numdraw; i++) delete graphwin[i];

	//if(graphbox) graphbox->wxDialog::Destroy();

	delete mod;

	delete[] gpos;
	delete flagrefs;
	delete hypoflags;
	//if(expdata->burstdata) delete expdata->burstdata;
	delete expdata;

	delete expdata2;
	delete selectdata;
	delete burstdata;
	delete filebase;
	//delete mod;
}


void HypoMain::SetMenuFlag(int id, wxString flagname, wxString flagtext, int state, wxMenu *menu)
{
	(*hypoflags)[flagname] = state;
	flagrefs->AddRef(id, flagname);
	menu->Append(id, flagtext, "Toggle " + flagtext, wxITEM_CHECK);
	menu->Check(id, state);
	Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HypoMain::OnFlag));
}


void HypoMain::OnFlag(wxCommandEvent& event)
{
	int id = event.GetId();
	wxString flag = flagrefs->GetRef(id);

	if((*hypoflags)[flag] == 0) (*hypoflags)[flag] = 1;
	else (*hypoflags)[flag] = 0;
}


void HypoMain::FullMenu()
{
	bool display = true;

	wxMenu *menuFile = new wxMenu;
	wxMenu *menuControls = new wxMenu;
	wxMenu *menuAnalysis = new wxMenu;
	wxMenu *menuTools = new wxMenu;
	wxMenu *menuSystem = new wxMenu;

	menuFile->Append(ID_About, "&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Output, "Output Data");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, "E&xit");

	menuControls->Append(ID_IGFMod, "IGF Model");
	menuControls->Append(ID_CortMod, "Corticotroph Model");
	menuControls->Append(ID_VasoMod, "Vasopressin Network Model");
	menuControls->Append(ID_OsmoMod, "Osmotic System Model");
	menuControls->Append(ID_HeatMod, "Heat System Model");
	menuControls->Append(ID_GHMod, "GH Model");

	//menuAnalysis->Append(ID_Info, "Info");
	//menuAnalysis->Append(ID_Burst, "Burst");
	//menuAnalysis->Append(ID_XYPos, "XY Pos");
	SetMenuFlag(ID_XYPos, "xypos", "XY Pos", 1, menuAnalysis); 
	SetMenuFlag(ID_Zoom, "zoom", "Graph Zoom", 0, menuAnalysis); 

	//menuTools->Append(ID_Info, "Info");
	menuTools->Append(ID_Burst, "Spike Analysis");
	//menuTools->Append(ID_Protocol, "Protocol");
	menuTools->Append(ID_Sound, "Sonic");
	menuTools->Append(ID_GraphAdd, "Add Graph");
	menuTools->Append(ID_ModGen, "Mod Gen");
	menuTools->Append(ID_Diag, "Diagnostic Box");

	menuSystem->Append(ID_Options, "Options");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuControls, "Models");
	menuBar->Append(menuAnalysis, "Analysis");
	menuBar->Append(menuTools, "Tools");
	menuBar->Append(menuSystem, "System");

	if(display) {
		wxMenu *menuDisplay = new wxMenu;
		menuDisplay->Append(ID_Display, "New Display");
		menuBar->Append(menuDisplay, "Display");
	}

	SetMenuBar(menuBar);

	//delete menuFile;
	//delete menuControls;
}


void HypoMain::UserMenu()
{
	wxMenu *menuFile = new wxMenu;
	wxMenu *menuAnalysis = new wxMenu;
	wxMenu *menuTools = new wxMenu;
	wxMenu *menuSystem = new wxMenu;

	menuFile->Append(ID_About, "&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, "E&xit");

	SetMenuFlag(ID_XYPos, "xypos", "XY Pos", 1, menuAnalysis); 
	SetMenuFlag(ID_Zoom, "zoom", "Graph Zoom", 0, menuAnalysis); 

	menuTools->Append(ID_Diag, "Diagnostic Box");
	menuTools->Append(ID_Grid, "Data Grid");
	menuTools->Append(ID_Neuro, "Neuro Box");
	menuTools->Append(ID_Plot, "Plot Box");

	menuSystem->Append(ID_Options, "Options");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuAnalysis, "Analysis");
	menuBar->Append(menuTools, "Tools");
	menuBar->Append(menuSystem, "System");

	SetMenuBar(menuBar);
}


void HypoMain::BasicMenu()
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_About, "&About...");
	//menuFile->AppendSeparator();
	//menuFile->Append(ID_Output, "Output Data");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, "E&xit");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");

	SetMenuBar(menuBar);
}


void HypoMain::OnEnter(wxCommandEvent& WXUNUSED(event))
{
	SetStatusText("Main Enter");
}


void HypoMain::OnSound(wxCommandEvent& WXUNUSED(event))
{
#ifdef HYPOSOUND
	wxSize boxsize;

	SetStatusText("Sound Box");
	if(ostype == Mac) boxsize = wxSize(285, 380);
	else boxsize = wxSize(320, 430);

	mainpos = GetPosition();
	soundbox = new SoundBox(mod, "Sonic Spike Analysis", wxPoint(320, 455), boxsize);
	toolset.AddBox(soundbox);
	soundbox->Show(true);
#endif
}


void HypoMain::OnDClick(wxMouseEvent& event)
{
	pinmode = 1 - pinmode;
	snum.Printf("DClick %d", pinmode);
	SetStatusText(snum);
}


void HypoMain::DiagText(wxString text)
{
	if(diagbox) diagbox->Write(text + "\n");
}


void HypoMain::SetStatus(wxString message)
{
	SetStatusText(message);
}


void HypoMain::GraphOut()
{
	FILE *ofp;

	ofp = fopen("graphout.txt", "w");
	fprintf(ofp, "numgraphs %d\n\n", numgraphs);

	//for(i=0; i<numgraphs; i++)
	//	fprintf(ofp, "graph %d  numplots %d  plot0 title %s  gparam %d\n", i,
	//	gpos[i].numplots, gpos[i].plot[0]->gname.mb_str(), gpos[i].plot[0]->gparam);

	fclose(ofp);
}


void HypoMain::OnClose(wxCloseEvent& event)
{
	OptionStore();
	//ViewStore();
	MainStore();
    if(mod) {
        mod->ModClose();
        mod->ModStore();
        mod->GHistStore();
    }
	CleanUp();
	Destroy();

	//_CrtDumpMemoryLeaks();
}


void HypoMain::OnMove(wxMoveEvent& event)
{
	int i;
	wxString snum;
	wxPoint mainpos = GetPosition();
	wxPoint diagpos;
	wxSize mainsize = GetSize();

	//if(cortbox != NULL) cortbox->Move(mainpos.x + mainsize.x, mainpos.y + 10);

	snum.Printf("Main Pos X %d Y %d", mainpos.x, mainpos.y);
	//DiagText(snum);

	for(i=0; i<toolset->numtools; i++) {
		if(toolset->box[i] != NULL) diagpos = toolset->box[i]->SetPosition();
		//snum.Printf("Move %d tools, box 0 = %s xpos %d ypos %d", toolset.numtools, toolset.box[0]->boxtag, diagpos.x, diagpos.y);
		//snum.Printf("Move %d tools, box 0 = %s", toolset.numtools, toolset.box[0]->boxtag);
	}
	//if(diagnostic) SetStatusText(snum);

    if(mod) {
        for(i=0; i<mod->modtools.numtools; i++) if(mod->modtools.box[i] != NULL) mod->modtools.box[i]->SetPosition();
        snum.Printf("Move %d modtools", mod->modtools.numtools);
        if(diagnostic) SetStatusText(snum);
    }

	//gwindow->Refresh();
}


void HypoMain::OnSize(wxSizeEvent& event)
{
	int gspace, gspacex;
	int scalewidth = 155;
	wxSize statusSize;
	int i;
    
    if(!numdraw) {
        wxFrame::OnSize(event);
        return;
    }

	wxSize newsize = GetSize();
	wxSize graphsize = graphsizer->GetSize();
	wxPoint mainpos = GetPosition();
	snum.Printf("Main Size X %d Y %d", newsize.x, newsize.y);
	SetStatusText(snum);
	//DiagText(snum);

	statusSize = statusbar->GetSize();

	yplot = 200;
	xplot = 500;

	//if(newsize.y < 825) yplot = 150;
	//if(newsize.y < 675) yplot = 100;
	//yplot = 150;

	gspacex = newsize.x - scalewidth;
	if(gspacex < 500) xplot = 450;
	if(gspacex < 450) xplot = 400;
	if(gspacex < 400) xplot = 350;
	if(gspacex < 350) xplot = 300;
	xplot = gspacex - 15;

	//gspace = newsize.y - numdraw * 75 - statusSize.y;
	//gspace = newsize.y - numdraw * 75 - 5;
	gspace = graphsize.y - numdraw * 55 - 5;
	if(gspace / numdraw < 200) yplot = 150;
	if(gspace / numdraw < 150) yplot = 100;
	yplot = gspace/numdraw;

	snum.Printf("Size X %d Y %d, Graph X %d Y %d, yplot %d", newsize.x, newsize.y, graphsize.x, graphsize.y, yplot);
	SetStatusText(snum);

	viewwidth = newsize.x;
	viewheight = newsize.y;

	for(graph=0; graph<numdraw; graph++) {
		graphwin[graph]->SetInitialSize();
		//graphwin[graph]->SetSize(0, 200, xplot, yplot);
		graphwin[graph]->ReSize(xplot, yplot);
	}

	for(i=0; i<toolset->numtools; i++) if(toolset->box[i] != NULL) toolset->box[i]->SetPosition();
	for(i=0; i<mod->modtools.numtools; i++) if(mod->modtools.box[i] != NULL) mod->modtools.box[i]->SetPosition();
	/*
	if(igfbox != NULL && ostype != 2) {
	igfbox->Move(mainpos.x + newsize.x, mainpos.y + 10);
	if(newsize.y < 605) igfbox->SetSize(320, newsize.y - 10);
	if(newsize.y >= 605) igfbox->SetSize(320, 750);
	}*/
	Layout();
}


void HypoMain::DispParam(ParamStore *params, wxString key)
{
	double value;

	value = (*params)[key];
	snum.Printf("%.2f", value);
	//SetStatusText(snum);
}


void HypoMain::RunModel(ParamBox *modbox)
{
	//ModThread *modthread = NULL;

	activemod = modbox->modtype;

	/*
	if(modbox->modtype == modIGF) {
	igfbox = modbox;
	focusdata = igfdata;
	modthread = new IGFMod((IGFBox *)modbox, igfdata, currdata, this);
	}

	modthread->Create();
	modthread->Run();*/
}


/*
void MainFrame::RunCortModel(CortBox *modbox)
{
//cortbox = modbox;
//activemod = cort_mod;
//SetStatusText("Run Cort Model");

//CortMod *cortmod = new CortMod(cortbox, cortdata, scalebox, this);
//cortmod->Create();
//cortmod->Run();
mod->RunModel();
}*/


void HypoMain::GraphUpdate()
{
	for(graph=0; graph<numdraw; graph++)
		graphwin[graph]->UpdateScroll();
	if(dispwin != NULL) dispwin->GraphUpdate();
}


void HypoMain::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	//Close(TRUE);
	SetStatusText("Quit Signal");
	Close(TRUE);
}

void HypoMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString message;

	if(basic) message.Printf("GH Model (teaching version)\n\nDuncan MacGregor 2020\n\nSystem: %s", wxGetOsDescription());
	else if(user) message.Printf("HypoMod Modelling Toolkit\n\nDuncan MacGregor 2010-2020\n\nSystem: %s", wxGetOsDescription());
	else message.Printf("HypoMod Modelling Toolkit\n\nDuncan MacGregor 2010-2020\n\nSystem: %s", wxGetOsDescription());

	wxMessageBox(message, "About Hypo Model", wxOK | wxICON_INFORMATION, this);
}


void HypoMain::OnNewDisp(wxCommandEvent& WXUNUSED(event))
{
	SetStatusText("New Disp");

	mainpos = GetPosition();
	dispwin = new DispWin(this, "Graph Display", wxPoint(mainpos.x+20, mainpos.y+20), wxSize(700, 830), numdraw, numdraw2);
	dispwin->Show(true);
}


void HypoMain::OnModGen(wxCommandEvent& WXUNUSED(event))
{
	ModGenBox *modgenbox = new ModGenBox(this, "Mod Gen", wxPoint(0, 0), wxSize(650, 750));
	toolset->AddBox(modgenbox);
	modgenbox->Show(true);
}


void HypoMain::OnDiagBox(wxCommandEvent& WXUNUSED(event))
{
	if(diagbox) diagbox->Show(true);
}


void HypoMain::OnGridBox(wxCommandEvent& WXUNUSED(event))
{
	if(gridbox) gridbox->Show(true);
}


void HypoMain::OnNeuroBox(wxCommandEvent& WXUNUSED(event))
{
	if(neurobox) neurobox->Show(true);
}


void HypoMain::OnPlotBox(wxCommandEvent& WXUNUSED(event))
{
	if(plotbox) plotbox->Show(true);
}


void HypoMain::OnProtocol(wxCommandEvent& WXUNUSED(event))
{
	/*
	wxSize boxsize;

	SetStatusText("Protocol Box");
	//if(ostype == Mac) boxsize = wxSize(285, 380);
	if(ostype == Mac) boxsize = wxSize(315, 380);
	else boxsize = wxSize(320, 430);

	mainpos = GetPosition();
	protocolbox = new ProtocolBox(mod, "Protocol", wxPoint(320, 455), boxsize);
	toolset.AddBox(protocolbox);
	protocolbox->Show(true);*/
}


void HypoMain::OnInfo(wxCommandEvent& WXUNUSED(event))
{
	SetStatusText("Info Box");

	mainpos = GetPosition();
	//infobox = new InfoBox(this, "Info", wxPoint(mainpos.x+700, mainpos.y+5), wxSize(200, 300));
	infobox->Show(true);
}


void HypoMain::OnCortMod(wxCommandEvent& WXUNUSED(event))
{
	//mod = new CortModel(modCort, "Corticotroph Model", this);
}


void HypoMain::OnOsmoMod(wxCommandEvent& WXUNUSED(event))
{
	//mod = new OsmoModel(modOsmo, "Osmotic System Model", this);
}


void HypoMain::OnHeatMod(wxCommandEvent& WXUNUSED(event))
{
	//mod = new HeatModel(modHeat, "Heat System Model", this);
}


void HypoMain::OnGHMod(wxCommandEvent& WXUNUSED(event))
{
	//mod = new GHModel(modGH, "Heat System Model", this);
}


void HypoMain::OnParams(wxCommandEvent& WXUNUSED(event))
{
	SetStatusText("Params Box");

	//mod = new IGFModel(modIGF, "IGF Model", this);
	//mainpos = GetPosition();
	//igfbox = new IGFBox(this, "IGF Parameters", wxPoint(mainpos.x+700, mainpos.y+5), wxSize(320, 595), igfparams);
	//igfbox = new IGFBox(this, "IGF Model", wxPoint(0, 0), wxSize(305, 705), igfparams);
	//igfbox = new IGFBox(this, "IGF Model", wxPoint(0, 5), wxSize(320, 750), igfparams);
	//toolset.AddBox(igfbox);
	//igfbox->Show(true);
}



void HypoMain::SpikeBox(int modmode)
{
	int boxwidth, boxheight;

	//wxString tag;

	//SetStatusText("Burst Box");
	//burstdata->spikedata = vasodata->spikedat;
	if(ostype == Mac) {
		boxwidth = 285;
		boxheight = 380;
	}
	else {
		boxwidth = 425;
		boxheight = 500;
	}

	diagbox->Write(text.Format("Spike box init type %d\n", modmode));


	if(modmode == 2) burstbox = new BurstBox(mod, "Spike Data Load and Analysis", wxPoint(0, 500), wxSize(boxwidth, boxheight), 0, "Selected");

	if(modmode == 1) burstbox = new BurstBox(mod, "Spike Data", wxPoint(0, 500), wxSize(boxwidth, boxheight), 0, "Selected", false, 0);
	//mainpos = GetPosition();
	
	//burstbox = new BurstBox(this, "Analysis", wxPoint(320, 485), wxSize(330, 430), 0, "Selected");
	burstbox->loaddata = expdata;

	diagbox->Write(text.Format("SpikeModule modmode %d\n", modmode));

	if(!expdata->graphs) {
		SpikeModule(mod);
		if(!modmode) scalebox->GraphSwitch();
	}

	diagbox->Write(text.Format("SpikeModule OK\n"));

	mod->modtools.AddBox(burstbox, true);

	//toolset->AddBox(burstbox);
	burstbox->Show(true);
}


void HypoMain::OnBurst(wxCommandEvent& WXUNUSED(event))
{ 
	if(burstbox) burstbox->Show(true); 
	//else mod->SpikeBox();
}


void HypoMain::SpikeModule(Model *mod)
{
	GraphSet *graphset;
	wxString gtag, tag = "exp";

	diagbox->Write("SpikeModule entered\n");

	expdata->PlotSet(mod->graphbase, "Exp ", green, 1, "exp");

	diagbox->Write("SpikeModule graphset ok\n");

	graphset = mod->graphbase->NewSet("Exp Spikes", tag + "spikes");
	graphset->AddFlag("timeres", 1);
	graphset->Add(tag + "rate1s", 0);
	graphset->Add(tag + "spikes1ms", 1);
	if(diagbox) diagbox->textbox->AppendText(graphset->Display());

	graphset = mod->graphbase->NewSet("Exp Intervals", tag + "intervals");
	graphset->AddFlag("binrestog1", 1);
	graphset->AddFlag("hazmode1", 10);
	graphset->AddFlag("normtog", 100);
	graphset->Add(tag + "hist1ms", 0);
	graphset->Add(tag + "hist5ms", 1);
	graphset->Add(tag + "haz1ms", 10);
	graphset->Add(tag + "haz5ms", 11);
	graphset->Add(tag + "normhist1ms", 100);
	graphset->Add(tag + "normhist5ms", 101);
	graphset->Add(tag + "haz1ms", 110);
	graphset->Add(tag + "haz5ms", 111);
	if(diagbox) diagbox->textbox->AppendText(graphset->Display());

	gtag = tag + "iod";
	mod->graphbase->Add(GraphDat(&expdata->IoDdata, 0, 70, 0, 2, "Exp IoD", 9, 1, lightgreen), gtag);
	mod->graphbase->GetGraph(gtag)->gdatax = &expdata->IoDdataX;
	mod->graphbase->GetGraph(gtag)->xcount = 7;  
	mod->graphbase->GetGraph(gtag)->synchx = false; 
	mod->graphbase->GetGraph(gtag)->barshift  = 20;
	mod->graphbase->GetGraph(gtag)->ylabelplaces  = 0;
}


void HypoMain::OnOutput(wxCommandEvent& WXUNUSED(event))
{
	wxString wxfiletag;
	SetStatusText("Output Data");

	mod->Output();
	/*
	if(activemod == 1) {
	sprintf(filetag, "igf");
	igfdata->output(filetag);
	}
	if(activemod == modVaso) {
	wxfiletag = vasobox->paramstoretag->GetValue();
	currvaso->output(wxfiletag.mb_str());
	expdata->output("vasoexp");
	}
	if(activemod == 2) VMHOutput();
	if(activemod == 4) CortOutput();
	*/

}


void HypoMain::OnOptionPanel(wxCommandEvent& event)
{
	SetStatus("Sys Panel");

	//int ID = event->GetId;

	//if(event.GetId() == ID_IGFPanel) {}
	OptionPanel *optionpanel = new OptionPanel(this, "System Panel");
	optionpanel->Show(true);
	OptionStore();
}


void HypoMain::ViewStore()
{
	wxString filename;
	wxString outline;

	filename = mainpath + "Init/hypoview.ini";

	wxTextFile opfile(filename);
	if(!opfile.Exists()) opfile.Create();
	opfile.Open();
	opfile.Clear();
	outline.Printf("startmod %d", startmod);
	opfile.AddLine(outline);
	outline.Printf("numdraw %d", numdraw);
	opfile.AddLine(outline);
	outline.Printf("viewheight %d", viewheight);
	opfile.AddLine(outline);
	opfile.Write();
	opfile.Close();
}


void HypoMain::ViewLoad()
{
	long numdat;

	wxString filename;
	wxString readline, numstring;

	filename = mainpath + "Init/hypoprefs.ini";

	wxTextFile opfile(filename);

	if(!opfile.Exists()) {
		startmod = 13;
		numdraw = 3;
		return;
	}

	opfile.Open();
	readline = opfile.GetLine(0);
	numstring = readline.AfterFirst(' ');
	numstring.ToLong(&numdat);
	startmod = (int)numdat;
	readline = opfile.GetLine(1);
	numstring = readline.AfterFirst(' ');
	numstring.ToLong(&numdat);
	numdraw = (int)numdat;
	readline = opfile.GetLine(2);
	numstring = readline.AfterFirst(' ');
	numstring.ToLong(&numdat);
	viewheight = (int)numdat;
	opfile.Close();
}


void HypoMain::OptionStore()
{
	wxString filename;
	wxString outline;
    
    initpath = mainpath + "Init/";
    if(!wxDirExists(initpath)) wxMkdir(initpath);

	filename = initpath + "hypoprefs.ini";

	TextFile opfile;
	opfile.New(filename);

	opfile.WriteLine(outline.Format("startmod %d", startmod));

	opfile.WriteLine(outline.Format("numdraw %d", numdraw));

	opfile.WriteLine(outline.Format("viewheight %d", viewheight));

	opfile.WriteLine(outline.Format("viewwidth %d", viewwidth));

	opfile.WriteLine(outline.Format("ylabels %d", ylabels));

	opfile.WriteLine(outline.Format("datsample %d", datsample));

	opfile.WriteLine(outline.Format("basic %d", basic));

	opfile.WriteLine(outline.Format("user %d", user));

	opfile.WriteLine(outline.Format("project %d", project));

	opfile.WriteLine(outline.Format("diagnostic %d", diagnostic));

	opfile.Close();

	filename = initpath + "hypopaths.ini";
	opfile.New(filename);
	opfile.WriteLine(datapath);
	opfile.WriteLine(parampath);
	opfile.WriteLine(outpath);
	opfile.WriteLine(modpath);
	opfile.Close();
}


void HypoMain::OptionLoad()
{
	long numdat;
	int check;
	wxString filename;
	wxString readline, numstring, tag;
    
    initpath = mainpath + "Init/";
    if(!wxDirExists(initpath)) {
        diagbox->Write("Init not found, no option load fail\n");
    };

	filename = initpath + "hypoprefs.ini";

	wxTextFile opfile(filename);

	if(!opfile.Exists()) {
		startmod = 13;
		numdraw = 3;
		ylabels = 5;
		diagnostic = 1;
		datsample = 1;
		basic = 0;
		user = 0;
		project = 0;
	}
	else {
		opfile.Open();
		readline = opfile.GetFirstLine();

		while(!readline.IsEmpty()) {
			tag = readline.BeforeFirst(' ');
			readline = readline.AfterFirst(' ');
			readline.Trim();
			readline.ToLong(&numdat);
			prefstore[tag] = numdat;
			//if(diagnostic) ofp.WriteLine(text.Format("Model Param ID %d, Value %.4f\n", id, datval));
			if(opfile.Eof()) return;
			readline = opfile.GetNextLine();
		}

		opfile.Close();

		startmod = prefstore["startmod"];
		numdraw = prefstore["numdraw"];
		viewheight = prefstore["viewheight"];
		viewwidth = prefstore["viewwidth"];
		ylabels = prefstore["ylabels"];
		datsample = prefstore["datsample"];
		basic = prefstore["basic"];
		user = prefstore["user"];
		project = prefstore["project"];
		diagnostic = prefstore["diagnostic"];
	}

	filename = initpath + "hypopaths.ini";
	TextFile infile;
	check = infile.Open(filename);

	if(!check) {
		datapath = "Data";
		parampath = "Params";
		outpath = "";
		modpath = "";
		return;
	}

	readline = infile.ReadLine();
	datapath = readline.Trim();
	readline = infile.ReadLine();
	parampath = readline.Trim();
	readline = infile.ReadLine();
	outpath = readline.Trim();
	readline = infile.ReadLine();
	modpath = readline.Trim();

	opfile.Close();
}


bool HypoApp::OnInit()
{
	int x = 700;
	int y = 920; // 1100   // 850
	wxPoint pos;
    wxString homepath, hypopath, respath;

	if(GetSystem() == Mac) {
		x = 685;
		pos.x = 50;
		pos.y = 23;    // 0;
	}
	else pos = wxDefaultPosition;
    
    mainpath = "";
#ifdef OSX
    CFBundleRef mainBundle;
    char *path = new char[1024];
    mainBundle = CFBundleGetMainBundle();
    CFURLRef main_bundle_URL = CFBundleCopyBundleURL(mainBundle);
    CFStringRef cf_string_ref = CFURLCopyFileSystemPath(main_bundle_URL, kCFURLPOSIXPathStyle);
    CFStringGetCString(cf_string_ref, path, 1024, kCFStringEncodingUTF8);
    CFRelease(main_bundle_URL);
    CFRelease(cf_string_ref);
    respath = path + string("/Contents/Resources/");
    
    homepath = getenv("HOME");
    hypopath = homepath + "/Library/Application Support/HypoMod/";
    if(wxDirExists(hypopath)) mainpath = hypopath;
    else mainpath = respath;
#endif // OSX

	LoadPrefs();
	if(viewwidth < 400) viewwidth = 400;
	y = viewheight;
	x = viewwidth;

	wxSize screensize = wxGetDisplaySize();
	if(x > screensize.GetX()) x = screensize.GetX() - 50;
	if(y > screensize.GetY()) y = screensize.GetY() - 50;

	HypoMain *mainwin = new HypoMain("HypoMod", pos, wxSize(x, y), respath);   // 850   // 920
	mainwin->SetTransparent(254);
	mainwin->Show();
	SetTopWindow(mainwin);
	//mainwin->SetDoubleBuffered(true);
	return TRUE;
}


void HypoApp::LoadPrefs()
{
	long numdat;
	wxString filename;
	wxString readline, numstring;

	filename = mainpath + "Init/hypoprefs.ini";

	wxTextFile opfile(filename);

	if(!opfile.Exists()) {
		viewheight = 920;
		viewwidth = 700;
		return;
	}

	opfile.Open();
	readline = opfile.GetLine(2);
	numstring = readline.AfterFirst(' ');
	numstring.ToLong(&numdat);
	viewheight = numdat;
	readline = opfile.GetLine(3);
	numstring = readline.AfterFirst(' ');
	numstring.ToLong(&numdat);
	viewwidth = numdat;

	opfile.Close();
}
