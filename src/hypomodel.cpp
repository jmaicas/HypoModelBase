

#include <hypomodel.h>
#include <wx/print.h>
#include "hypodef.h"
//#include "hypomods.h"
//#include "hypopanels.h"
//#include "cortmod.h"


/*
 #include "osmomod.h"
 #include "igfmod.h"
 #include "vasomod.h"
 #include "heatmod.h"
 #include "ghmod.h"
 #include "lysismod.h"
 #include "vmnmodel.h"
 #include "agentmodel.h"*/


//class CortModel;


IMPLEMENT_APP(HypoApp)


MainFrame *mainframe;


HypoMain::HypoMain(const wxString& title, const wxPoint& pos, const wxSize& size)
: MainFrame(title, pos, size)
{
    wxIcon icon;
    wxColour backcolour, panelcolour;
    
    backcolour.Set("#ffffff");
    //panelcolour.Set("#eeeeee");     // grey
    //panelcolour.Set("#ccdaff");     // pale blue
    //panelcolour.Set("#ccffcc");       // pale green
    //panelcolour.Set("#ece9d8");     // xp default
    //panelcolour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    //ostype = GetSystem();
    //mainwin = this;
    SetMinSize(wxSize(200, 670));
    //mainwing = this;
    //CreateStatusBar();
    
    // Check System
    wxString oslabel = wxGetOsDescription();
    SetStatusText(oslabel);
    mainframe = this;
    
    //blankevent = new wxCommandEvent();
    
    /*
     screensize = wxGetDisplaySize();
     printdata = new wxPrintData;
     pageSetupData = new wxPageSetupDialogData;
     printdata->SetPaperId(wxPAPER_A5);
     printdata->SetOrientation(wxLANDSCAPE);
     
     // copy over initial paper size from print record
     *pageSetupData = *printdata;
     
     // Set some initial page margins in mm.
     pageSetupData->SetMarginTopLeft(wxPoint(15, 15));
     pageSetupData->SetMarginBottomRight(wxPoint(15, 15));*/
    
    numgraphs = 8;
    numdraw = 3;
    numdraw2 = 3;
    activemod = 0;
    startmod = 13;
    pinmode = 0;
    basic = 1;
    diagnostic = 0;
    xstretch = 50;
    modpath = "";
    
    //toolset = new ToolSet();
    //toolset->AddBox(diagbox);
    
    OptionLoad();
    //ViewLoad();
    MainLoad();
    
    spikedisp = 0;   // spike display mode   0 = plain   1 = burst   2 = hazard filter
    
    //diagbox->Show(true);
    
    diagbox->Write("HypoMain initialisation\n\n");
    
    gpos = new graphdisp[numgraphs];
    //greg = new GraphDat[100];
    //graphbase = new GraphBase(200);
    //greg = graphbase->graphstore;
    
    
    //currdata = new CurrentDat();
    //realdata = new SpikeDat();
    //burstdata = new BurstDat();
    //burstdata->spikedata = vasodata->spikedat;
    //analysisdata = new AnaDat();
    //expdata = new SpikeDat();
    //expdata->burstdata = new BurstDat();
    //expdata->burstdata->spikedata = expdata;
    
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
    else FullMenu();
    
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
        return;
    }
    
    if(mod->gcount < numdraw) numdraw = mod->gcount;
    
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
    SetSizer(mainsizer);
    Layout();
    
    //if(mod->path != "" && !wxDirExists(mod->path)) wxMkdir(mod->path);
    //scalebox->SetMod(mod);
    
    /*wxSize boxsize;
     if(ostype == Mac) boxsize = wxSize(315, 380);
     else boxsize = wxSize(320, 430);
     mainpos = GetPosition();
     protocolbox = new ProtocolBox(mod, "Protocol", wxPoint(320, 455), boxsize);
     toolset.AddBox(protocolbox);
     protocolbox->Show(true);*/
    
    
    //burstbox = new BurstBox(this, "Burst Analysis", wxPoint(mainpos.x+1020, mainpos.y+340), wxSize(300, 350), burstdata);
    //burstbox->Show(true);
    
    
    //SetStatusText("Hypo Net Model");
    //fclose(ofp);
    //wxofp->Write();
    //wxofp->Close();
    
    
    
    //diagbox->Raise();
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
    Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(HypoMain::OnEnter));
}


void HypoMain::OnGraphAdd(wxCommandEvent& WXUNUSED(event))
{
    SetStatusText("Add Graph");
    
    AddGraph();
}


void HypoMain::AddGraph()
{
    int i;
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
    int i;
    
    //for(i=0; i<numdraw; i++) delete graphwin[i];
    
    if(graphbox) graphbox->Destroy();
    
    delete[] gpos;
    delete mod;
    
    
    //wxMenu *menuTools = new wxMenu;
    //wxMenu *menuSystem = new wxMenu;
    //wxMenu *menuDisplay = new wxMenu;
}


void HypoMain::FullMenu()
{
    bool display = true;
    
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuControls = new wxMenu;
    //wxMenu *menuAnalysis = new wxMenu;
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
    //menuTools->Append(ID_Info, "Info");
    //menuTools->Append(ID_Burst, "Burst");
    //menuTools->Append(ID_Protocol, "Protocol");
    menuTools->Append(ID_Sound, "Sonic");
    menuTools->Append(ID_GraphAdd, "Add Graph");
    menuTools->Append(ID_ModGen, "Mod Gen");
    menuTools->Append(ID_Diag, "Diagnostic Box");
    
    menuSystem->Append(ID_Options, "Options");
    
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuControls, "Models");
    //menuBar->Append(menuAnalysis, "Analysis");
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
    int i;
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
    mod->ModClose();
    mod->ModStore();
    mod->GHistStore();
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
        //snum.Printf("Move %d tools, box 0 = %s xpos %d ypos %d", toolset.numtools, toolset.box[0]->boxname, diagpos.x, diagpos.y);
        //snum.Printf("Move %d tools, box 0 = %s", toolset.numtools, toolset.box[0]->boxname);
    }
    //if(diagnostic) SetStatusText(snum);
    
    for(i=0; i<mod->modtools.numtools; i++) if(mod->modtools.box[i] != NULL) mod->modtools.box[i]->SetPosition();
    snum.Printf("Move %d modtools", mod->modtools.numtools);
    if(diagnostic) SetStatusText(snum);
    
    //gwindow->Refresh();
}


void HypoMain::OnSize(wxSizeEvent& WXUNUSED(event))
{
    int gspace, gspacex;
    int scalewidth = 155;
    wxSize statusSize;
    int i;
    
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
    
    if(basic) message.Printf("GH Model (teaching version)\n\nDuncan MacGregor 2013\n\nSystem: %s", wxGetOsDescription());
    else message.Printf("Hypothalamic Network Model\n\nDuncan MacGregor 2010-2015\n\nSystem: %s", wxGetOsDescription());
    
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


void HypoMain::OnBurst(wxCommandEvent& WXUNUSED(event))
{
    int boxwidth, boxheight;
    //SetStatusText("Burst Box");
    //burstdata->spikedata = vasodata->spikedat;
    if(ostype == Mac) {
        boxwidth = 285;
        boxheight = 380;
    }
    else {
        boxwidth = 300;
        boxheight = 430;
    }
    
    //mainpos = GetPosition();
    burstbox = new BurstBox(mod, "Burst Analysis", wxPoint(320, 455), wxSize(boxwidth, boxheight), focusdata);
    toolset->AddBox(burstbox);
    burstbox->Show(true);
}


void HypoMain::OnOutput(wxCommandEvent& WXUNUSED(event))
{
    char filetag[100];
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
}


void HypoMain::ViewStore()
{
    wxString filename;
    wxString outline;
    
    filename = "Init/hypoview.ini";
    
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
    
    filename = "Init/hypoprefs.ini";
    
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
    
    filename = "Init/hypoprefs.ini";
    
    TextFile opfile;
    opfile.New(filename);
    
    opfile.WriteLine(outline.Format("startmod %d", startmod));
    
    opfile.WriteLine(outline.Format("numdraw %d", numdraw));
    
    opfile.WriteLine(outline.Format("viewheight %d", viewheight));
    
    opfile.WriteLine(outline.Format("viewwidth %d", viewwidth));
    
    opfile.WriteLine(outline.Format("ylabels %d", ylabels));
    
    opfile.WriteLine(outline.Format("datsample %d", datsample));
    
    opfile.WriteLine(outline.Format("basic %d", basic));
    
    opfile.WriteLine(outline.Format("diagnostic %d", diagnostic));
    
    opfile.Close();
    
    filename = "Init/hypopaths.ini";
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
    
    filename = "Init/hypoprefs.ini";
    
    wxTextFile opfile(filename);
    
    if(!opfile.Exists()) {
        startmod = 13;
        numdraw = 3;
        ylabels = 5;
        diagnostic = 1;
        datsample = 1;
        basic = 0;
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
        diagnostic = prefstore["diagnostic"];
        
    }
    
    filename = "Init/hypopaths.ini";
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


void HypoMain::GraphData()
{
    //gpos[0].data = 0;
    gpos[0].Add(&greg[0]);
    
    //gpos[1].data = 1;
    gpos[1].Add(&greg[1]);
    
    //gpos[2].data = 2;
    gpos[2].Add(&greg[45]);
    
    //gpos[3].data = 3;
    gpos[3].Add(&greg[45]);
    
    //gpos[4].data = 4;
    gpos[4].Add(&greg[39]);
    
    //gpos[5].data = 5;
    gpos[5].Add(&greg[40]);
    
    //gpos[6].data = 5;
    gpos[6].Add(&greg[41]);
    
    //gpos[7].data = 5;
    gpos[7].Add(&greg[42]);
    
    int datsample = 10;
    
    /*
     greg[0].gparam = -3;
     greg[0].gdatav = &igfdata->srate;
     greg[0].yfrom = 0;
     greg[0].yto = 20;
     greg[0].xfrom = 0;
     greg[0].xto = 500;
     greg[0].samprate = 0;
     greg[0].gname = "Spike Rate 1s";
     greg[0].type = 3;
     greg[0].binsize = 1;
     greg[0].spikedata = igfdata;*/
    //greg[0].spikedata = NULL;
    
    //graphbase->graphstore[0].gparam = -3;
    //(*graphbase)[0].gparam = -3;
    //graphbase->Add(GraphDat(&igfdata->srate, 0, 500, 0, 20, "Spike Rate 1s", igfdata));
    //(*graphbase)[0].spikedata = igfdata;
    
    
    greg[1].gparam = -4;
    greg[1].gdatadv = &realdata->hist1;
    greg[1].yfrom = 0;
    greg[1].yto = 1000;
    greg[1].xfrom = 0;
    greg[1].xto = 500;
    greg[1].gname = "Real ISI Histogram 1ms";
    greg[1].type = 1;
    greg[1].binsize = 1;
    greg[1].colour = 2;
    
    /*
     greg[2].gparam = -4;
     greg[2].gdatadv = &igfdata->hist1;
     greg[2].yfrom = 0;
     greg[2].yto = 100;
     greg[2].xfrom = 0;
     greg[2].xto = 500;
     greg[2].gname = "Model ISI Histogram 1ms";
     greg[2].type = 1;
     greg[2].binsize = 1;
     greg[2].colour = 3;
     
     greg[3].gparam = -4;
     greg[3].gdatadv = &igfdata->hist5;
     greg[3].yfrom = 0;
     greg[3].yto = 500;
     greg[3].xfrom = 0;
     greg[3].xto = 500;
     greg[3].gname = "Model ISI Histogram 5ms";
     greg[3].type = 1;
     greg[3].binsize = 5;
     greg[3].colour = 3;
     
     greg[4].gparam = -4;
     greg[4].gdatadv = &igfdata->haz1;
     greg[4].yfrom = 0;
     greg[4].yto = 0.04;
     greg[4].xfrom = 0;
     greg[4].xto = 500;
     greg[4].gname = "Model Hazard 1ms";
     greg[4].type = 1;
     greg[4].binsize = 1;
     greg[4].colour = 3;
     
     greg[5].gparam = -4;
     greg[5].gdatadv = &igfdata->haz5;
     greg[5].yfrom = 0;
     greg[5].yto = 0.2;
     greg[5].xfrom = 0;
     greg[5].xto = 500;
     greg[5].gname = "Model Hazard 5ms";
     greg[5].type = 1;
     greg[5].binsize = 5;
     greg[5].colour = 3;
     
     
     greg[22].gparam = -3;
     greg[22].gdatav = &igfdata->srate1;
     greg[22].yfrom = 0;
     greg[22].yto = 3;
     greg[22].xfrom = 0;
     greg[22].xto = 0.5;
     greg[22].samprate = 0;
     greg[22].gname = "Spikes 1ms";
     greg[22].type = 3;
     greg[22].binsize = 0.001;
     greg[22].spikedata = igfdata;
     */
    
    /*
     greg[29].gparam = -4;
     greg[29].gdatadv = &cortdata->V;
     greg[29].yfrom = -100;
     greg[29].yto = 20;
     greg[29].xfrom = 0;
     greg[29].xto = 2000;
     greg[29].samprate = 0;
     greg[29].gname = "Corticotroph mV";
     greg[29].type = 4;
     greg[29].binsize = 1;
     greg[29].colour = 3;
     greg[29].xscale = 8;
     
     greg[30].gparam = -4;
     greg[30].gdatadv = &cortdata->Ca;
     greg[30].yfrom = -10;
     greg[30].yto = 10;
     greg[30].xfrom = 0;
     greg[30].xto = 2000;
     greg[30].samprate = 0;
     greg[30].gname = "Corticotroph Ca2+";
     greg[30].type = 4;
     greg[30].binsize = 1;
     greg[30].colour = 7;
     greg[30].xscale = 8;
     */
    
    greg[34].gparam = -4;
    greg[34].gdatadv = &burstdata->hist1;
    greg[34].yfrom = 0;
    greg[34].yto = 100;
    greg[34].xfrom = 0;
    greg[34].xto = 500;
    greg[34].gname = "Intra-Burst Histogram 1ms";
    greg[34].type = 1;
    greg[34].binsize = 1;
    greg[34].colour = 3;
    
    greg[35].gparam = -4;
    greg[35].gdatadv = &burstdata->hist5;
    greg[35].yfrom = 0;
    greg[35].yto = 500;
    greg[35].xfrom = 0;
    greg[35].xto = 500;
    greg[35].gname = "Intra-Burst Histogram 5ms";
    greg[35].type = 1;
    greg[35].binsize = 5;
    greg[35].colour = 35;
    
    greg[36].gparam = -4;
    greg[36].gdatadv = &burstdata->haz1;
    greg[36].yfrom = 0;
    greg[36].yto = 0.04;
    greg[36].xfrom = 0;
    greg[36].xto = 500;
    greg[36].gname = "Intra-Burst Hazard 1ms";
    greg[36].type = 1;
    greg[36].binsize = 1;
    greg[36].colour = 3;
    
    greg[37].gparam = -4;
    greg[37].gdatadv = &burstdata->haz5;
    greg[37].yfrom = 0;
    greg[37].yto = 0.2;
    greg[37].xfrom = 0;
    greg[37].xto = 500;
    greg[37].gname = "Intra-Burst Hazard 5ms";
    greg[37].type = 1;
    greg[37].binsize = 5;
    greg[37].colour = 3;
    
    /*
     greg[52].gparam = -4;
     greg[52].gdatadv = &cortdata->ICaL;
     greg[52].yfrom = -20;
     greg[52].yto = 80;
     greg[52].xfrom = 0;
     greg[52].xto = 2000;
     greg[52].samprate = 0;
     greg[52].gname = "Corticotroph ICaL";
     greg[52].type = 4;
     greg[52].binsize = 1;
     greg[52].colour = 7;
     greg[52].xscale = 8;
     
     greg[53].gparam = -4;
     greg[53].gdatadv = &cortdata->ICaT;
     greg[53].yfrom = -20;
     greg[53].yto = 80;
     greg[53].xfrom = 0;
     greg[53].xto = 2000;
     greg[53].samprate = 0;
     greg[53].gname = "Corticotroph ICaT";
     greg[53].type = 4;
     greg[53].binsize = 1;
     greg[53].colour = 7;
     greg[53].xscale = 8;
     
     greg[54].gparam = -4;
     greg[54].gdatadv = &cortdata->IKDR;
     greg[54].yfrom = -100;
     greg[54].yto = 100;
     greg[54].xfrom = 0;
     greg[54].xto = 2000;
     greg[54].samprate = 0;
     greg[54].gname = "Corticotroph IKDR";
     greg[54].type = 4;
     greg[54].binsize = 1;
     greg[54].colour = 7;
     greg[54].xscale = 8;
     
     greg[55].gparam = -4;
     greg[55].gdatadv = &cortdata->IKCa;
     greg[55].yfrom = -100;
     greg[55].yto = 100;
     greg[55].xfrom = 0;
     greg[55].xto = 2000;
     greg[55].samprate = 0;
     greg[55].gname = "Corticotroph IKCa";
     greg[55].type = 4;
     greg[55].binsize = 1;
     greg[55].colour = 7;
     greg[55].xscale = 8;
     
     greg[56].gparam = -4;
     greg[56].gdatadv = &cortdata->noise;
     greg[56].yfrom = -10;
     greg[56].yto = 10;
     greg[56].xfrom = 0;
     greg[56].xto = 2000;
     greg[56].samprate = 0;
     greg[56].gname = "Corticotroph noise";
     greg[56].type = 4;
     greg[56].binsize = 1;
     greg[56].colour = 7;
     greg[56].xscale = 8;
     */
    
    
    greg[59].gparam = -3;
    greg[59].gdatav = &analysisdata->autocorr;
    greg[59].yfrom = 0;
    greg[59].yto = 1000;
    greg[59].xfrom = 0;
    greg[59].xto = 1000;
    greg[59].gname = "Auto Correlation";
    greg[59].type = 1;
    greg[59].binsize = 10;
    greg[59].colour = 2;
    
    greg[60].gparam = -3;
    greg[60].gdatav = &expdata->srate;
    greg[60].yfrom = 0;
    greg[60].yto = 20;
    greg[60].xfrom = 0;
    greg[60].xto = 500;
    greg[60].samprate = 0;
    greg[60].gname = "Exp Spike Rate 1s";
    greg[60].type = 3;
    greg[60].binsize = 1;
    greg[60].spikedata = expdata;
    
    greg[61].gparam = -3;
    greg[61].gdatav = &expdata->srate1;
    greg[61].yfrom = 0;
    greg[61].yto = 3;
    greg[61].xfrom = 0;
    greg[61].xto = 0.5;
    greg[61].samprate = 0;
    greg[61].gname = "Exp Spikes 1ms";
    greg[61].type = 3;
    greg[61].binsize = 0.001;
    greg[61].spikedata = expdata;
    
    greg[62].gparam = -4;
    greg[62].gdatadv = &expdata->hist1;
    greg[62].yfrom = 0;
    greg[62].yto = 100;
    greg[62].xfrom = 0;
    greg[62].xto = 500;
    greg[62].gname = "Exp ISI Histogram 1ms";
    greg[62].type = 1;
    greg[62].binsize = 1;
    greg[62].colour = green;
    
    greg[63].gparam = -4;
    greg[63].gdatadv = &expdata->hist5;
    greg[63].yfrom = 0;
    greg[63].yto = 500;
    greg[63].xfrom = 0;
    greg[63].xto = 500;
    greg[63].gname = "Exp ISI Histogram 5ms";
    greg[63].type = 1;
    greg[63].binsize = 5;
    greg[63].colour = green;
    
    greg[64].gparam = -4;
    greg[64].gdatadv = &expdata->haz1;
    greg[64].yfrom = 0;
    greg[64].yto = 0.04;
    greg[64].xfrom = 0;
    greg[64].xto = 500;
    greg[64].gname = "Exp Hazard 1ms";
    greg[64].type = 1;
    greg[64].binsize = 1;
    greg[64].colour = green;
    
    greg[65].gparam = -4;
    greg[65].gdatadv = &expdata->haz5;
    greg[65].yfrom = 0;
    greg[65].yto = 0.2;
    greg[65].xfrom = 0;
    greg[65].xto = 500;
    greg[65].gname = "Exp Hazard 5ms";
    greg[65].type = 1;
    greg[65].binsize = 5;
    greg[65].colour = green;
    
    greg[76].gparam = -4;
    greg[76].gdatadv = &expdata->burstdata->hist1;
    greg[76].yfrom = 0;
    greg[76].yto = 100;
    greg[76].xfrom = 0;
    greg[76].xto = 500;
    greg[76].gname = "Intra-Burst Exp Hist 1ms";
    greg[76].type = 1;
    greg[76].binsize = 1;
    greg[76].colour = green;
    
    greg[77].gparam = -4;
    greg[77].gdatadv = &expdata->burstdata->hist5;
    greg[77].yfrom = 0;
    greg[77].yto = 500;
    greg[77].xfrom = 0;
    greg[77].xto = 500;
    greg[77].gname = "Intra-Burst Exp Hist 5ms";
    greg[77].type = 1;
    greg[77].binsize = 5;
    greg[77].colour = green;
    
    greg[78].gparam = -4;
    greg[78].gdatadv = &expdata->burstdata->haz1;
    greg[78].yfrom = 0;
    greg[78].yto = 0.04;
    greg[78].xfrom = 0;
    greg[78].xto = 500;
    greg[78].gname = "Intra-Burst Exp Haz 1ms";
    greg[78].type = 1;
    greg[78].binsize = 1;
    greg[78].colour = green;
    
    greg[79].gparam = -4;
    greg[79].gdatadv = &expdata->burstdata->haz5;
    greg[79].yfrom = 0;
    greg[79].yto = 0.2;
    greg[79].xfrom = 0;
    greg[79].xto = 500;
    greg[79].gname = "Intra-Burst Exp Haz 5ms";
    greg[79].type = 1;
    greg[79].binsize = 5;
    greg[79].colour = green;
    
    /*
     greg[80].gparam = -4;
     greg[80].gdatadv = &igfdata->burstdata->hist1;
     greg[80].yfrom = 0;
     greg[80].yto = 100;
     greg[80].xfrom = 0;
     greg[80].xto = 500;
     greg[80].gname = "Intra-Burst Mod Hist 1ms";
     greg[80].type = 1;
     greg[80].binsize = 1;
     greg[80].colour = 3;
     
     greg[81].gparam = -4;
     greg[81].gdatadv = &igfdata->burstdata->hist5;
     greg[81].yfrom = 0;
     greg[81].yto = 500;
     greg[81].xfrom = 0;
     greg[81].xto = 500;
     greg[81].gname = "Intra-Burst Mod Hist 5ms";
     greg[81].type = 1;
     greg[81].binsize = 5;
     greg[81].colour = 3;
     
     greg[82].gparam = -4;
     greg[82].gdatadv = &igfdata->burstdata->haz1;
     greg[82].yfrom = 0;
     greg[82].yto = 0.04;
     greg[82].xfrom = 0;
     greg[82].xto = 500;
     greg[82].gname = "Intra-Burst Mod Haz 1ms";
     greg[82].type = 1;
     greg[82].binsize = 1;
     greg[82].colour = 3;
     
     greg[83].gparam = -4;
     greg[83].gdatadv = &igfdata->burstdata->haz5;
     greg[83].yfrom = 0;
     greg[83].yto = 0.2;
     greg[83].xfrom = 0;
     greg[83].xto = 500;
     greg[83].gname = "Intra-Burst Mod Haz 5ms";
     greg[83].type = 1;
     greg[83].binsize = 5;
     greg[83].colour = 3;
     */
    
    greg[84].gparam = -4;
    greg[84].gdatadv = &expdata->burstdata->profile;
    greg[84].yfrom = 0;
    greg[84].yto = 20;
    greg[84].xfrom = 0;
    greg[84].xto = 250;
    greg[84].samprate = 0;
    greg[84].gname = "Exp Burst Profile";
    greg[84].type = 4;
    greg[84].binsize = 1;
    greg[84].colour = green;
    
    /*
     greg[85].gparam = -4;
     greg[85].gdatadv = &igfdata->burstdata->profile;
     greg[85].yfrom = 0;
     greg[85].yto = 20;
     greg[85].xfrom = 0;
     greg[85].xto = 250;
     greg[85].samprate = 0;
     greg[85].gname = "Mod Burst Profile";
     greg[85].type = 4;
     greg[85].binsize = 1;
     greg[85].colour = blue;*/
    
    greg[86].gparam = -4;
    greg[86].gdatadv = &expdata->burstdata->tailprofile;
    greg[86].yfrom = 0;
    greg[86].yto = 20;
    greg[86].xfrom = 0;
    greg[86].xto = 250;
    greg[86].samprate = 0;
    greg[86].gname = "Exp Burst Tail Profile";
    greg[86].type = 4;
    greg[86].binsize = 1;
    greg[86].colour = green;
    
    /*
     greg[87].gparam = -4;
     greg[87].gdatadv = &igfdata->burstdata->tailprofile;
     greg[87].yfrom = 0;
     greg[87].yto = 20;
     greg[87].xfrom = 0;
     greg[87].xto = 250;
     greg[87].samprate = 0;
     greg[87].gname = "Mod Burst Tail Profile";
     greg[87].type = 4;
     greg[87].binsize = 1;
     greg[87].colour = blue;*/
    
    greg[88].gparam = -4;
    greg[88].gdatadv = &expdata->burstdata->bursthaz;
    greg[88].yfrom = 0;
    greg[88].yto = 200;
    greg[88].xfrom = 0;
    greg[88].xto = 250;
    greg[88].samprate = 0;
    greg[88].gname = "Exp Burst Haz";
    greg[88].type = 1;
    greg[88].binsize = 1;
    greg[88].colour = green;
    
    /*
     greg[89].gparam = -4;
     greg[89].gdatadv = &igfdata->burstdata->bursthaz;
     greg[89].yfrom = 0;
     greg[89].yto = 200;
     greg[89].xfrom = 0;
     greg[89].xto = 250;
     greg[89].samprate = 0;
     greg[89].gname = "Mod Burst Haz";
     greg[89].type = 1;
     greg[89].binsize = 1;
     greg[89].colour = blue;*/
    
    graphbase->numgraphs = 90;
    
    graphbase->Add(GraphDat(&expdata->burstdata->tailmean, 0, 100, 0, 2, "Burst Tail Mean logint", 4, 1, green), "exptailmean");
    
    /*
     graphbase->Add(GraphDat(&vasodata->Dyno, 0, 500, 0, 300, "Vaso Net Dyno", 4, gbin, 7, 1000), "vasonetDyno");
     graphbase->Add(GraphDat(&vasodata->KLact, 0, 500, -1, 1, "Vaso Net KL act", 4, gbin, 7, 1000), "vasonetKLact");
     graphbase->Add(GraphDat(&vasodata->CaDyno, 0, 500, 0, 500, "Vaso Net Ca-Dyno", 4, gbin, 7, 1000), "vasonetCaDyno");
     graphbase->Add(GraphDat(&vasodata->Ca, 0, 500, 0, 500, "Vaso Net Ca", 4, gbin, 7, 1000), "vasonetCa");
     
     graphbase->Add(GraphDat(&vasodata->dend, 0, 500, 0, 500, "Vaso Net Dend", 4, gbin, 7, 1000), "vasonetdend");
     graphbase->Add(GraphDat(&vasodata->dendstore, 0, 500, 0, 50, "Vaso Net Dend Store", 4, gbin, 7, 1000), "vasonetdendstore");
     //graphbase->Add(GraphDat(&vasodata->global, 0, 500, 0, 50, "Vaso Net Global", 4, 1000, 7, 1000), "vasonetglobal");
     graphbase->Add(GraphDat(&vasodata->global, 0, 500, 0, 50, "Vaso Net Global", 4, 1, 7, 1), "vasonetglobal");
     graphbase->Add(GraphDat(&currvaso->burstdata->length, 0, 100, 0, 1000, "Response Range", 4, 1, 7, 1), "vasoresponserange");
     graphbase->Add(GraphDat(&currvaso->vasomean, 0, 100, 0, 1000, "Vaso Mean", 4, 10, 7, 1, 200), "vasomean");*/
}


bool HypoApp::OnInit()
{
    int x = 700;
    int y = 920; // 1100   // 850
    wxPoint pos;
    
    if(GetSystem() == Mac) {
        x = 685;
        pos.x = 50;
        pos.y = 23;    // 0;
    }
    else pos = wxDefaultPosition;
    
    OptionLoad();
    if(viewwidth < 400) viewwidth = 400;
    y = viewheight;
    x = viewwidth;
    
    wxSize screensize = wxGetDisplaySize();
    if(x > screensize.GetX()) x = screensize.GetX() - 50;
    if(y > screensize.GetY()) y = screensize.GetY() - 50;
    
    //MainFrame *mainwin = new MainFrame("Hypo Net Model", wxPoint(50, 10), wxSize(700, y));   // 850   // 920
    HypoMain *mainwin = new HypoMain("HypoMod", pos, wxSize(x, y));   // 850   // 920
    //HypoMain *mainwin = new HypoMain("HypoMod", wxDefaultPosition, wxSize(400, 500));
    mainwin->Show(TRUE);
    SetTopWindow(mainwin);
    return TRUE;
}


void HypoApp::OptionLoad()
{
    long numdat;
    
    wxString filename;
    wxString readline, numstring;
    
    filename = "Init/hypoprefs.ini";
    
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
