
#include "hypomodel.h"
#include "hypograph.h"
#include "hypodef.h"



ScaleBox::ScaleBox(HypoMain *main, wxFrame *draw, const wxSize& size, int gnum, GraphDisp *gdisp, Model *model, GraphWindow3 **gwin, int btype)
	: wxPanel(draw, wxID_ANY, wxPoint(0, 0), size, wxBORDER_SIMPLE | wxFULL_REPAINT_ON_RESIZE)
{
	int i;
	mainwin = main;
	drawframe = draw;
	boxtype = btype;
	ostype = GetSystem();
	if(ostype == Mac) buttonheight = 20; else buttonheight = 23;
	gbase = model->graphbase;
	//greg = gbase->graphstore;
	gpos = gdisp;
	graphwin = gwin;
	numgraphs = gnum;
	//startgraph = start; 
	startgraph = 0;
	gmod = model;
	mod = model;
    
    wxString iconpath = main->respath + "Init/";

	SetDoubleBuffered(true);

	gflags = new ParamStore();
	gflagrefs = new RefStore();

	//boxfont = new wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	//boxfont.New(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	//boxfont.New(12, wxFONTFAMILY_ROMAN, wxNORMAL, wxNORMAL);
	//boxfont = wxFont(12, wxFONTFAMILY_ROMAN, wxNORMAL, wxNORMAL);
	//boxfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	//boxfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL);
    
    boxfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
    
	hazmode1 = 0;
	hazmode2 = 0;
	binrestog1 = 0;
	binrestog2 = 0;
	overtog = 0;
	overtog2 = 0;
	vmhflag = 0;
	nettog = 0;
	timeres = 0;
	rateres = 0;
	burstmode = 0;
	filtermode = 0;
	vasoflag = 0;
	cortflag = 0;
	vasosecflag = 0;
	vasonetflag = 0;
	inputflag = 0;
	expdatflag = 0;
	profmode = 0;
	proftype = 0;
	secmode = 0;
	sectype = 0;
	dispmod = blank;
	gsynch = 0;
	expmode = 1;
	dendmode = 0;
	ratedata = 0;
	internflag = 0;

	//normtog = 0;

	synchcon = 0;

	xmin = -1000000;
	xmax = 10000000;   // 1000000, extend for VasoMod long runs
	ymin = -1000000;
	ymax = 1000000;

	overpan1 = 1;
	overpan2 = 2;

	backgroundcolour = GetBackgroundColour();
	SetBackgroundColour(backgroundcolour);
	SetFont(boxfont);                       // //
	//dc.SetTextBackground(backgroundColour);

	panel = this;
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	vbox->AddSpacer(5);

	//wxBitmap downarrow(down_xpm);
	//wxBitmap uparrow(up_xpm);
	wxImage::AddHandler(new wxPNGHandler);
	rightarrow = wxBitmap(iconpath + "rightarrow.png", wxBITMAP_TYPE_PNG);
	leftarrow = wxBitmap(iconpath + "leftarrow.png", wxBITMAP_TYPE_PNG);
	uparrow = wxBitmap(iconpath + "uparrow.png", wxBITMAP_TYPE_PNG);
    downarrow = wxBitmap(iconpath + "downarrow.png", wxBITMAP_TYPE_PNG);
	if(ostype == Mac) {
		//downarrow = wxBitmap("Init/arrow-down-15.png", wxBITMAP_TYPE_PNG);
        uparrow = wxBitmap(iconpath + "uparrow12.png", wxBITMAP_TYPE_PNG);
		downarrow = wxBitmap(iconpath + "downarrow12.png", wxBITMAP_TYPE_PNG);
        rightarrow = wxBitmap(iconpath + "rightarrow12.png", wxBITMAP_TYPE_PNG);
        leftarrow = wxBitmap(iconpath + "leftarrow12.png", wxBITMAP_TYPE_PNG);
	}
    
    zbheight = 16;
    zbwidth = 24;

	vconbox = new wxBoxSizer(wxVERTICAL);
	for(i=0; i<numgraphs; i++) {
		AddGraphConsole(i, graphwin[i]);
		gsync[i] = NULL;
	}
	vbox->Add(vconbox, 1);


	//if(boxtype == modVMN) {
		//gsync[i] = new wxCheckBox(panel, i, "Sync");
		//psetbox->Add(gsync[i], 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 2);
	//}


	//vbox->AddStretchSpacer(5);
	
	wxBoxSizer *modbuttonbox = new wxBoxSizer(wxHORIZONTAL);
	if(boxtype == modAgent) {
		ScaleButton(ID_xscale, "XScale", 50, modbuttonbox);
	}
	mod->diagbox->Write(text.Format("\nscale boxtype %d\n", boxtype));


	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	if(ostype == Mac) {
		ScaleButton(wxID_OK, "OK", 43, buttonbox);
		buttonbox->AddSpacer(2);
		ScaleButton(ID_Sync, "Sync", 43, buttonbox);	
	}
	else {
		ScaleButton(wxID_OK, "OK", 35, buttonbox);
		buttonbox->AddSpacer(2);
		syncbutton = ToggleButton(ID_Sync, "Sync", 35, buttonbox);	
		//ScaleButton(ID_Sync, "Sync", 35, buttonbox);	
	}

	vbox->Add(modbuttonbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	vbox->AddSpacer(3);
	vbox->Add(buttonbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	vbox->AddSpacer(3);
	wxSizer *gsfilebox = GSBox("gtest1");
	//vbox-Add(gsfilebox);
	vbox->Add(gsfilebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	

	//vbox->Add(okbutton, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	//gmod->GHistLoad();

	dispmod = mod->modtype;
	mod->GHistLoad(gstag);
	//GraphSwitch(0);

	panel->SetSizer(vbox);

	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnOK));
	if(ostype == Mac) Connect(ID_Sync, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnSync));
	else Connect(ID_Sync, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnSync));

	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ScaleBox::OnOK));
    Connect(wxEVT_TEXT_ENTER, wxCommandEventHandler(ScaleBox::OnOK));
    
	Connect(ID_expdat, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnExpMode));
	Connect(ID_secretion, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnSecMode));
	Connect(ID_dendmode, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnDendMode));
	Connect(ID_overlay, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnOverlay));
	Connect(ID_overlay2, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnOverlay));
	Connect(ID_position, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnPosition));
	Connect(ID_position2, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnPosition));
	Connect(ID_xscale, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnXScale));

	Connect(ID_spikes, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnSpikes));
	Connect(ID_rateres, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnRateRes));
	Connect(ID_data, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnData));
	Connect(ID_intern, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnIntern));
	Connect(1000+startgraph, 1000+startgraph+numgraphs-1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnYZoomIn));
	Connect(1010+startgraph, 1010+startgraph+numgraphs-1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnYZoomOut));
	Connect(1100+startgraph, 1100+startgraph+numgraphs-1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnXZoomIn));
	Connect(1110+startgraph, 1110+startgraph+numgraphs-1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnXZoomOut));
	//Connect(1000, 1000+numgraphs-1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnYZoomIn));
	//Connect(1010, 1010+numgraphs-1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnYZoomOut));
	//Connect(1100, 1100+numgraphs-1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnXZoomIn));
	//Connect(1110, 1110+numgraphs-1, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnXZoomOut));

	Connect(ID_gstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnGStore));
	Connect(ID_gload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnGLoad));

	Connect(wxEVT_PAINT, wxPaintEventHandler(ScaleBox::OnPaint));
	Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(ScaleBox::OnEraseBackground));
	//Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ScaleBox::OnClose));
}


ScaleBox::~ScaleBox()
{
	delete gflags;
	delete gflagrefs;
}


void ScaleBox::RemoveGraphConsole(int index)
{
	consolebox[index]->Clear(true);
	vconbox->Detach(consolebox[index]);
	numgraphs--;
	GraphWindow3 *graphpanel = graphwin[index]; 



	//delete graphpanel->yf;
	//delete graphpanel->yt; 
	//delete graphpanel->xf; 
	//delete graphpanel->xt; 
	//delete graphpanel->yzoomin; 
	//delete graphpanel->yzoomout; 
	//delete graphpanel->xzoomin; 
	//delete graphpanel->xzoomout; 
}


void ScaleBox::AddGraphConsole(int index, GraphWindow3 *graphpanel)
{
	graphpanel->scalebox = this;
	wxBoxSizer *psetbox = new wxBoxSizer(wxVERTICAL);
	consolebox[index] = new wxBoxSizer(wxVERTICAL);

	if(ostype == Mac) psetbox->AddSpacer(2);

	graphpanel->yf = AddScaleParam("YF", 0, psetbox, index);
	graphpanel->yt = AddScaleParam("YT", 10, psetbox, index);
	graphpanel->xf = AddScaleParam("XF", 0, psetbox, index);
	graphpanel->xt = AddScaleParam("XT", 500, psetbox, index);
	wxBoxSizer *zoombox = new wxBoxSizer(wxHORIZONTAL);
	if(ostype == Mac) {
		graphpanel->yzoomin = new wxBitmapButton(this, 1000 + index, downarrow, wxDefaultPosition, wxSize(zbwidth, zbheight));
		graphpanel->yzoomout = new wxBitmapButton(this, 1010 + index, uparrow, wxDefaultPosition, wxSize(zbwidth, zbheight));
		graphpanel->xzoomin = new wxBitmapButton(this, 1100 + index, leftarrow, wxDefaultPosition, wxSize(zbwidth, zbheight));
		graphpanel->xzoomout = new wxBitmapButton(this, 1110 + index, rightarrow, wxDefaultPosition, wxSize(zbwidth, zbheight));
	}
	else {
		graphpanel->yzoomin = new wxBitmapButton(this, 1000 + index, downarrow, wxDefaultPosition, wxSize(20, 20));
		graphpanel->yzoomout = new wxBitmapButton(this, 1010 + index, uparrow, wxDefaultPosition, wxSize(20, 20));	
		graphpanel->xzoomin = new wxBitmapButton(this, 1100 + index, leftarrow, wxDefaultPosition, wxSize(20, 20));
		graphpanel->xzoomout = new wxBitmapButton(this, 1110 + index, rightarrow, wxDefaultPosition, wxSize(20, 20));
	}
	zoombox->Add(graphpanel->yzoomin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	zoombox->Add(graphpanel->yzoomout, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	zoombox->AddSpacer(2);
	zoombox->Add(graphpanel->xzoomin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	zoombox->Add(graphpanel->xzoomout, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	if(ostype == Mac) psetbox->AddSpacer(2);

	psetbox->Add(zoombox, 0, wxALIGN_CENTRE_HORIZONTAL);
	
	// Console Buttons
	if(boxtype == -1) mod->ScaleConsoleAbove(this, index);
	consolebox[index]->Add(psetbox, 0, wxALIGN_CENTRE_HORIZONTAL);
	if(boxtype == -1) mod->ScaleConsoleBelow(this, index);
	if(boxtype != -1) ConsoleButtons(index);  // Old switched mod specific controls
	vconbox->Add(consolebox[index], 1, wxALIGN_CENTRE_HORIZONTAL, 0);
}


void ScaleBox::ConsoleButtons(int panel)
{
	if(boxtype == 0) {
		if(panel == 0) {
			wxBoxSizer *resbox = new wxBoxSizer(wxHORIZONTAL); 
			wxBoxSizer *modebox = new wxBoxSizer(wxHORIZONTAL); 

			if(ostype == Mac) {
				ScaleButton(ID_data, "Dat", 43, modebox);
				ScaleButton(ID_intern, "Int", 43, modebox);
				ScaleButton(ID_spikes, "Spike", 43, resbox);     // 37
				ScaleButton(ID_rateres, "Rate", 43, resbox);    // 37
			}
			else {
				ScaleButton(ID_spikes, "Spikes", 37, resbox); 
				resbox->AddSpacer(2);
				ScaleButton(ID_rateres, "Rate", 37, resbox); 
				ScaleButton(ID_data, "Data", 37, modebox);
				modebox->AddSpacer(2);
				ScaleButton(ID_intern, "Intern", 37, modebox);
			}
			consolebox[panel]->Add(modebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
			consolebox[panel]->Add(resbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
			//consolebox[panel]->Add(rightarrow, 0);
		}
		//consolebox[panel]->AddSpacer(2);

		if(panel == 1) {
			/*
			if(ostype == Mac) {
			ScaleButton(ID_histhaz1, "Hist / Haz", 70, consolebox[panel]);
			ScaleButton(ID_binres1, "Bin Res", 60, consolebox[panel]);
			ScaleButton(ID_allburst, "All / Burst", 74, consolebox[panel]);
			}
			else {
			ScaleButton(ID_histhaz1, "Hist / Haz", 54, consolebox[panel]);
			ScaleButton(ID_binres1, "Bin Res", 43, consolebox[panel]);
			ScaleButton(ID_allburst, "All / Burst", 55, consolebox[panel]);
			}	*/	

			// GraphButton replaces ScaleButton, gradual phase out    16/6/16

			wxBoxSizer *binbox = new wxBoxSizer(wxHORIZONTAL); 
			if(ostype == Mac) {
				GraphButton("hazmode1", 0, ID_histhaz1, "Hist / Haz", 70, consolebox[panel]);
				GraphButton("binrestog1", 0, ID_binres1, "Bin Res", 45, binbox);
				GraphButton("normtog", 0, ID_norm, "Norm", 45, binbox);
				//ScaleButton(ID_allburst, "All / Burst", 74, consolebox[panel]);
			}
			else {
				GraphButton("hazmode1", 0, ID_histhaz1, "Hist / Haz", 54, consolebox[panel]);
				GraphButton("binrestog1", 0, ID_binres1, "Bin Res", 43, binbox);
				GraphButton("normtog", 0, ID_norm, "Norm", 35, binbox);
				//ScaleButton(ID_allburst, "All / Burst", 55, consolebox[panel]);
			}		
			consolebox[panel]->Add(binbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
		}

		//consolebox[panel]->AddSpacer(2);
		if(panel == 2) {		
			wxBoxSizer *profbox = new wxBoxSizer(wxHORIZONTAL); 
			wxBoxSizer *quadbox = new wxBoxSizer(wxHORIZONTAL); 
			GraphButton("quadtog", 0, ID_quad, "Quad", 35, quadbox);
			ScaleButton(ID_expdat, "Exp", 35, quadbox);
			consolebox[panel]->Add(quadbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
			GraphButton("proftype", 0, ID_profile, "Prof", 35, profbox);
			profbox->AddSpacer(2);
			GraphButton("profsmooth", 0, ID_smooth, "Smoo", 35, profbox);
			//wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
			wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
			if(ostype == Mac) {
				//ScaleButton(ID_histhaz2, "His", 43, hbox2);
				//ScaleButton(ID_binres2, "Bin", 43, hbox2);
				//ScaleButton(ID_secretion, "Sec", 43, secbox);
				//secbox->AddSpacer(2);
				//ScaleButton(ID_dendmode, "Den", 45, secbox);
				ScaleButton(ID_overlay, "Ovl", 43, hbox);
				ScaleButton(ID_position, "Pos", 43, hbox);
			}
			else {
				//ScaleButton(ID_histhaz2, "His/Hz", 40, hbox2);
				//hbox2->AddSpacer(2);
				//ScaleButton(ID_binres2, "BinRes", 40, hbox2);
				//ScaleButton(ID_secretion, "Sec", 37, secbox);
				//secbox->AddSpacer(2);
				//ScaleButton(ID_dendmode, "Dend", 37, secbox);
				ScaleButton(ID_overlay, "Over", 35, hbox);
				hbox->AddSpacer(2);
				ScaleButton(ID_position, "Pos", 35, hbox);
			}
			consolebox[panel]->Add(profbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
			consolebox[panel]->Add(hbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
		}
	}

	if(boxtype == modAgent) {
	}

	if(boxtype == modHeat) {
		if(panel == 1) {
			wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
			if(ostype == Mac) {
				ScaleButton(ID_overlay, "Ovl", 43, hbox);
				ScaleButton(ID_position, "Pos", 43, hbox);
			}
			else {
				ScaleButton(ID_overlay, "Over", 35, hbox);
				hbox->AddSpacer(2);
				ScaleButton(ID_position, "Pos", 35, hbox);
			}
			consolebox[panel]->Add(hbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
		}
	}

	if(boxtype == modPlot) {
		if(panel == 0) {
			wxBoxSizer *resbox = new wxBoxSizer(wxHORIZONTAL); 

			if(ostype == Mac) {
				ScaleButton(ID_spikes, "Sp", 40, resbox);   
				ScaleButton(ID_rateres, "Ra", 40, resbox); 
			}
			else {
				ScaleButton(ID_spikes, "Spikes", 37, resbox); 
				resbox->AddSpacer(2);
				ScaleButton(ID_rateres, "Rate", 37, resbox); 
			}
			consolebox[panel]->Add(resbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
		}
		if(panel == 1) {
			wxBoxSizer *binbox = new wxBoxSizer(wxHORIZONTAL); 
			if(ostype == Mac) {
				GraphButton("hazmode1", 0, ID_histhaz1, "Hist / Haz", 70, consolebox[panel]);
				GraphButton("binrestog1", 0, ID_binres1, "Bin Res", 45, binbox);
				GraphButton("normtog", 0, ID_norm, "Norm", 45, binbox);
			}
			else {
				GraphButton("hazmode1", 0, ID_histhaz1, "Hist / Haz", 60, consolebox[panel]);
				GraphButton("allselect", 0, ID_allselect, "All / Select", 60, consolebox[panel]);
				GraphButton("binrestog1", 0, ID_binres1, "Bin Res", 43, binbox);
				GraphButton("normtog", 0, ID_norm, "Norm", 35, binbox);
			}		
			consolebox[panel]->Add(binbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
		}
	}

	if(boxtype == modOxy || boxtype == modOxySec) {
		if(panel == 0) {
			wxBoxSizer *resbox = new wxBoxSizer(wxHORIZONTAL); 

			if(ostype == Mac) {
				ScaleButton(ID_spikes, "Spike", 40, resbox);
				ScaleButton(ID_rateres, "Rate", 40, resbox);
			}
			else {
				ScaleButton(ID_spikes, "Spikes", 37, resbox); 
				resbox->AddSpacer(2);
				ScaleButton(ID_rateres, "Rate", 37, resbox); 
			}
			consolebox[panel]->Add(resbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
		}
		if(panel == 1) {
			wxBoxSizer *binbox = new wxBoxSizer(wxHORIZONTAL); 
			if(ostype == Mac) {
				GraphButton("hazmode1", 0, ID_histhaz1, "Hist / Haz", 70, consolebox[panel]);
				GraphButton("binrestog1", 0, ID_binres1, "Bin", 45, binbox);
				GraphButton("normtog", 0, ID_norm, "Norm", 45, binbox);
			}
			else {
				GraphButton("hazmode1", 0, ID_histhaz1, "Hist / Haz", 54, consolebox[panel]);
				GraphButton("binrestog1", 0, ID_binres1, "Bin Res", 43, binbox);
				GraphButton("normtog", 0, ID_norm, "Norm", 35, binbox);
			}		
			consolebox[panel]->Add(binbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
		}

		if(panel == 2) {
			wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
			if(ostype == Mac) {
				ScaleButton(ID_overlay, "Ovl", 43, hbox);
				ScaleButton(ID_position, "Pos", 43, hbox);
			}
			else {
				ScaleButton(ID_overlay, "Over", 35, hbox);
				hbox->AddSpacer(2);
				ScaleButton(ID_position, "Pos", 35, hbox);
			}
			consolebox[panel]->Add(hbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
			overpan1 = 2;
			overpan2 = 3;
		}
	}

	if(boxtype == modVMN) {
		if(panel == 0) {
			wxBoxSizer *resbox = new wxBoxSizer(wxHORIZONTAL); 
			wxBoxSizer *modebox = new wxBoxSizer(wxHORIZONTAL); 

			if(ostype == Mac) {
				ScaleButton(ID_spikes, "Spike", 40, resbox);
				ScaleButton(ID_rateres, "Rate", 40, resbox);
				GraphButton("nettog", 0, ID_net, "Net", 43, modebox);
			}
			else {
				ScaleButton(ID_spikes, "Spikes", 37, resbox); 
				resbox->AddSpacer(2);
				ScaleButton(ID_rateres, "Rate", 37, resbox); 
				//ScaleButton(ID_net, "Net", 37, modebox);
				GraphButton("nettog", 0, ID_net, "Net", 37, modebox);
			}
			consolebox[panel]->Add(resbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
			consolebox[panel]->Add(modebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);

		}
		if(panel == 1) {
			wxBoxSizer *binbox = new wxBoxSizer(wxHORIZONTAL); 
			if(ostype == Mac) {
				GraphButton("hazmode1", 0, ID_histhaz1, "Hist / Haz", 70, consolebox[panel]);
				GraphButton("binrestog1", 0, ID_binres1, "Bin", 45, binbox);
				GraphButton("normtog", 0, ID_norm, "Norm", 45, binbox);
				//ScaleButton(ID_allburst, "All / Burst", 74, consolebox[panel]);
			}
			else {
				GraphButton("hazmode1", 0, ID_histhaz1, "Hist / Haz", 54, consolebox[panel]);
				GraphButton("binrestog1", 0, ID_binres1, "Bin Res", 43, binbox);
				//ScaleButton(ID_norm, "Norm", 35, binbox);
				GraphButton("normtog", 0, ID_norm, "Norm", 35, binbox);
				//ScaleButton(ID_allburst, "All / Burst", 55, consolebox[panel]);
			}		
			consolebox[panel]->Add(binbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
		}
		if(panel == 2) {
			wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
			if(ostype == Mac) {
				ScaleButton(ID_overlay, "Ovl", 43, hbox);
				ScaleButton(ID_position, "Pos", 43, hbox);
			}
			else {
				ScaleButton(ID_overlay, "Over", 35, hbox);
				hbox->AddSpacer(2);
				ScaleButton(ID_position, "Pos", 35, hbox);
			}
			consolebox[panel]->Add(hbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
			overpan1 = 2;
			overpan2 = 3;
		}
		if(panel == 4) {
			wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
			if(ostype == Mac) {
				ScaleButton(ID_overlay2, "Ovl", 43, hbox);
				ScaleButton(ID_position2, "Pos", 43, hbox);
			}
			else {
				ScaleButton(ID_overlay2, "Over", 35, hbox);
				hbox->AddSpacer(2);
				ScaleButton(ID_position2, "Pos", 35, hbox);
			}
			consolebox[panel]->Add(hbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
			overpan3 = 4;
			overpan4 = 5;
		}
	}
}


void ScaleBox::OnXScale(wxCommandEvent& event)
{
	if(mainwin->diagnostic) mainwin->diagbox->textbox->AppendText("\nOnXScale\n");
	mod->xscaletoggle = 1 - mod->xscaletoggle;
	mod->ScaleSwitch();
}

void ScaleBox::OnConFocus(wxFocusEvent& event)
{
	wxString text; 

	//mainwin->diagbox->Write("\nFocus Event\n");

	int id = event.GetId();
	TextBox *con = (TextBox *)FindWindowById(id); 

	//con->SetFocus();
	synchcon = con->val;
	//if(mainwin->diagnostic) gmod->diagbox->textbox->AppendText(text.Format("con click %d\n", con->val));
	mainwin->diagbox->Write(text.Format("con click %d\n", con->val));

	event.Skip();
}


void ScaleBox::OnGStore(wxCommandEvent& event)
{
	int i;
	wxString filename, filetag, filepath;
	wxString outline, text;
	wxColour redpen("#dd0000"), blackpen("#000000");
	TextFile outfile;

	//if(mod->path == "") filepath = "Init";
	//else filepath = mod->path + "/Graphs";
	filepath = mod->GetPath() + "/Graphs";
	if(!wxDirExists(filepath)) wxMkdir(filepath);

	filetag = gstag->GetValue();
	filename = "graph-" + filetag + ".dat";

	// Tag history
	short tagpos = gstag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) gstag->Delete(tagpos);
	gstag->Insert(filetag, 0);

	// Check and warn existing file
	wxTextFile checkfile(filepath + "/" + filename);
	if(!checkfile.Exists()) checkfile.Create();
	else if(redtag != filetag) {
		gstag->SetForegroundColour(redpen);
		gstag->SetValue("");
		gstag->SetValue(filetag);
		redtag = filetag;
		return;
	}
	redtag = "";
	gstag->SetForegroundColour(blackpen);
	gstag->SetValue("");
	gstag->SetValue(filetag);

	outfile.New(filepath + "/" + filename);
	for(i=0; i<gmod->gcount; i++) 
		outfile.WriteLine(text.Format("%d %s", i, gmod->gcodes[i]));

	outfile.WriteLine("");
	for(i=0; i<gflagrefs->numrefs; i++) {
		outline.Printf("%.0f", (*gflags)[gflagrefs->refbase[i].label]);  
		outfile.WriteLine(gflagrefs->refbase[i].label + " " + outline);
	}

	outfile.Close();

	/*
	outfile.New(filepath + "/" + filename);
	for(i=0; i<gmod->gcount; i++)
		outfile.WriteLine(text.Format("%d %s", i, gmod->gcodes[i]));
	outfile.Close();

	for(i=0; i<flagrefs->numrefs; i++) {
		outline.Printf("%.0f", (*modflags)[flagrefs->refbase[i].label]);  
		paramfile.AddLine(flagrefs->refbase[i].label + " " + outline);
	}*/

	if(mainwin->graphbox) mainwin->graphbox->SetParams();

	gmod->graphbase->BaseStore(filepath, filetag);
}


void ScaleBox::GLoad(wxString tag)
{
	long numdat;
	short check;
	int gindex;
	wxString filename, filetag, filepath;
	wxString readline, numstring, gtag;
	wxColour redpen("#dd0000"), blackpen("#000000");
	TextFile infile;

	//if(mod->path == "") filepath = "Init";
	//else filepath = mod->path + "/Graphs";
	//if(!wxDirExists(filepath)) wxMkdir(filepath);
	filepath = mod->GetPath() + "/Graphs";

	if(tag == "") filetag = gstag->GetValue();
	else filetag = tag;

	filename = "graph-" + filetag + ".dat";

	// Tag history
	if(tag != "default") {
		short tagpos = gstag->FindString(filetag);
		if(tagpos != wxNOT_FOUND) gstag->Delete(tagpos);
		gstag->Insert(filetag, 0);

		// Set existing file check
		redtag = "";
		gstag->SetForegroundColour(blackpen);
		gstag->SetValue("");
		gstag->SetValue(filetag);
	}

	// Load file
	check = infile.Open(filepath + "/" + filename);
	if(!check) return;
	readline = infile.ReadLine();

	// Read file
	while(!readline.IsEmpty()) {
		numstring = readline.BeforeFirst(' ');
		numstring.ToLong(&numdat);
		gindex = (int)numdat;
		gtag = readline.AfterFirst(' ');
		if(gbase->GraphExists(gtag) || gbase->SetExists(gtag)) gmod->gcodes[gindex] = gtag;
		else gmod->diagbox->Write(text.Format("GLoad graph/set %s not found\n", gtag));
		readline = infile.ReadLine();
	}

	if(!infile.End()) readline = infile.ReadLine();      // Check for next section, graph flags
	else readline = "";

	// Read graph flags
	long flagval;
	while(!readline.IsEmpty()) {
		tag = readline.BeforeFirst(' ');
		readline = readline.AfterFirst(' ');
		readline.Trim();
		readline.ToLong(&flagval);
		(*gflags)[tag] = flagval;
		mainwin->diagbox->Write(text.Format("Graph flag tag %s, value %ld\n", tag, flagval)); 
		readline = infile.ReadLine();	
	}

	infile.Close();

	if(gmod->diagbox != NULL) gmod->graphbase->BaseLoad(filepath, filetag, gmod->diagbox->textbox);
	else gmod->graphbase->BaseLoad(filepath, filetag);
	//gmod->graphbase->BaseLoad(filetag);
	mainwin->diagbox->Write("BaseLoad\n");

	//if(mainwin->graphbox) mainwin->graphbox->SetGraph();

	gmod->gsmode = 1;
	GraphSwitch(0);
}


void ScaleBox::OnGLoad(wxCommandEvent& event)
{
	GLoad();
}


void ScaleBox::OnSync(wxCommandEvent& event)
{
	if(mainwin->diagnostic) mainwin->diagbox->textbox->AppendText("\nOnSync\n");
	gsynch = 1 - gsynch;
	ScaleUpdate();
}


void ScaleBox::PaintBackground(wxDC& dc)
{
	//wxColour backgroundColour = GetBackgroundColour();
	//if(!backgroundColour.Ok()) backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

	dc.SetBrush(wxBrush(backgroundcolour));
	dc.SetPen(wxPen(backgroundcolour, 1));

	wxRect windowRect(wxPoint(0, 0), GetClientSize());
	dc.DrawRectangle(windowRect);
}


// Empty implementation, to prevent flicker
void ScaleBox::OnEraseBackground(wxEraseEvent &WXUNUSED(event))
{
}


void ScaleBox::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);
	//wxPaintDC dc(this);

	//wxColour bordercolour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME);
	//wxColour border1 = wxColour(0, 72, 241);
	//wxColour border2 = wxColour(0, 30, 161);
	//wxColour border3 = wxColour(0, 19, 140);
	// Shifts the device origin so we don't have to worry
	// about the current scroll position ourselves
	//PrepareDC(dc);

	// Paint the background
	PaintBackground(dc);

	// Paint the graphic

	wxColour border1 = wxColour(79, 83, 188);
	wxColour border2 = wxColour(109, 116, 205);
	wxColour border3 = wxColour(117, 134, 220);

	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	wxSize sz = GetClientSize();
	wxRect rect1(2, 2, sz.x-4, sz.y-4);
	wxRect rect2(1, 1, sz.x-2, sz.y-2);
	wxRect rect3(0, 0, sz.x, sz.y);

	//dc.SetPen(border1);
	//dc.DrawRectangle(rect1);
	//dc.SetPen(border2);
	//dc.DrawRectangle(rect2);
	//dc.SetPen(border3);
	//dc.DrawRectangle(rect3);
}

/*
void ScaleBox::OnYZoomIn(wxCommandEvent& event)
{
int pos = event.GetId() - 1000;
//graph = gpos[pos].plot[0];
graph = graphwin[pos]->graphset[0]->plot[0];
//graphdisp *gpos = graphwin[pos]->gpos;
double diff = graph->yto - graph->yfrom;
if(graph->negscale || graph->yfrom < 0) {
graph->yto = graph->yto - diff / 4;
graph->yfrom = graph->yfrom + diff / 4;
}
else graph->yto = graph->yto - diff / 2;
UpdateScale();
GraphUpdate();
}*/


void ScaleBox::OnYZoomIn(wxCommandEvent& event)
{
	int pos = event.GetId() - 1000;
	if(graphwin[pos]->numdisps == 0) return;
	graph = graphwin[pos]->dispset[0]->plot[0];
	double diff = graph->yto - graph->yfrom;
	if(graph->negscale || graph->yfrom < 0) {
		graph->yto = graph->yto - diff / 4;
		graph->yfrom = graph->yfrom + diff / 4;
	}
	else graph->yto = graph->yto - diff / 2;

	graphwin[pos]->XYSynch();
	synchcon = startgraph + pos;
	mainwin->SetStatusText("");
	ScaleUpdate();
}


void ScaleBox::OnYZoomOut(wxCommandEvent& event)
{
	int pos = event.GetId() - 1010;
	if(graphwin[pos]->numdisps == 0) return;
	graph = graphwin[pos]->dispset[0]->plot[0];
	double diff = graph->yto - graph->yfrom;
	if(graph->negscale || graph->yfrom < 0) {
		graph->yto = graph->yto + diff / 2;
		graph->yfrom = graph->yfrom - diff / 2;
	}
	else graph->yto = graph->yto + diff;

	graphwin[pos]->XYSynch();
	synchcon = startgraph + pos;
	mainwin->SetStatusText("");
	ScaleUpdate();
}


void ScaleBox::OnXZoomIn(wxCommandEvent& event)
{
	int pos = event.GetId() - 1100;
	if(graphwin[pos]->numdisps == 0) return;
	graph = graphwin[pos]->dispset[0]->plot[0];
	double diff = graph->xto - graph->xfrom;
	graph->xto = graph->xto - diff / 2;

	graphwin[pos]->XYSynch();
	synchcon = startgraph + pos;
	mainwin->SetStatusText("");
	ScaleUpdate();

	mainwin->diagbox->Write("XZoom\n");
}


void ScaleBox::OnXZoomOut(wxCommandEvent& event)
{
	double oldxto; 

	int pos = event.GetId() - 1110;
	snum.Printf("zoom ID %d", pos);
	if(mainwin->diagnostic) mainwin->SetStatusText(snum);
	if(graphwin[pos]->numdisps == 0) return;
	graph = graphwin[pos]->dispset[0]->plot[0];
	oldxto = graph->xto;
	double diff = graph->xto - graph->xfrom;
	graph->xto = graph->xto + diff;
	if(graph->xto < xmin || graph->xto > xmax) {
		mainwin->SetStatusText("X To, out of range, max 100000");
		graph->xto = oldxto;
		return;
	}

	graphwin[pos]->XYSynch();
	mainwin->SetStatusText("");
	synchcon = startgraph + pos;
	ScaleUpdate();
}


wxButton *ScaleBox::ScaleButton(int id, wxString label, int width, wxBoxSizer *box, int point)
{
    confont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	//confont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
    if(ostype == Mac) confont = wxFont(wxFontInfo(point).FaceName("Tahoma"));
	wxButton *button = new wxButton(this, id, label, wxDefaultPosition, wxSize(width, buttonheight), 0);
	button->SetFont(confont);
	box->Add(button, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM, 1);
	return button;
}


wxButton *ScaleBox::GraphButton(wxString tag, int state, int id, wxString label, int width, wxBoxSizer *box, int type, int point)
{
	(*gflags)[tag] = state;
	gflagrefs->AddRef(id, tag, type);
	Connect(id, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ScaleBox::OnGraphButton));

	return ScaleButton(id, label, width, box, point);
}


void ScaleBox::OnGraphButton(wxCommandEvent& event)
{
	wxString text;
	int id = event.GetId();
	wxString tag = gflagrefs->GetRef(id);
	int type = gflagrefs->GetType(id);

	if((*gflags)[tag] == type) (*gflags)[tag] = 0;
	else (*gflags)[tag]++;                          // new code for multi-state cycling buttons, i.e. all/burst/select, 'type' encodes number of states
	
	//if((*gflags)[tag] == 0) (*gflags)[tag] = 1;
	//else (*gflags)[tag] = 0;

	mainwin->diagbox->Write(text.Format("\ngraphbutton %s %.0f type %d\n", tag, (*gflags)[tag], type));

	GraphSwitch();
}


void ScaleBox::GraphCommand(int commid)
{
	wxString text;
	int id = commid;
	wxString tag = gflagrefs->GetRef(id);

	if((*gflags)[tag] == 0) (*gflags)[tag] = 1;
	else (*gflags)[tag] = 0;

	mainwin->diagbox->Write(text.Format("graphcommand %s %.0f\n", tag, (*gflags)[tag]));

	GraphSwitch();
}

/*
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
}*/


wxToggleButton *ScaleBox::ToggleButton(int id, wxString label, int width, wxBoxSizer *box, int point)
{
    confont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	//confont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
    
    if(ostype == Mac) confont = wxFont(wxFontInfo(point).FaceName("Tahoma"));
	//if(ostype == Mac) confont = wxFont(point, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	wxToggleButton *button = new wxToggleButton(this, id, label, wxDefaultPosition, wxSize(width, buttonheight), 0);
	button->SetFont(confont);
	box->Add(button, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM, 1);
	return button;
}


TextBox *ScaleBox::AddScaleParam(wxString name, double initval, wxBoxSizer *psetbox, int gpos)
{
    int boxwidth = 45;
    int boxheight = -1;
	int boxgap = 2;
    
	snum.Printf("%.1f", initval);
    confont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	//confont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
    if(ostype == Mac) {
        confont = wxFont(wxFontInfo(11).FaceName("Tahoma"));
        //confont = wxFont(11, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
        boxwidth = 45;
        boxheight = 20;
    }

	wxBoxSizer *pbox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *label = new wxStaticText(panel, wxID_STATIC, name, wxDefaultPosition, wxSize(-1, -1), 0);
	//wxTextCtrl *numbox = new wxTextCtrl(panel, wxID_ANY, snum, wxDefaultPosition, wxSize(45, -1), wxTE_PROCESS_ENTER);
	TextBox *numbox = new TextBox(panel, wxID_ANY, snum, wxDefaultPosition, wxSize(boxwidth, boxheight), wxTE_PROCESS_ENTER);
	label->SetFont(confont);
	numbox->SetFont(confont);
	pbox->Add(label, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);
	pbox->Add(numbox, 0, wxALIGN_CENTER_VERTICAL, 0);
	if(ostype == Mac) psetbox->Add(pbox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);
	else psetbox->Add(pbox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, boxgap);

	numbox->Bind(wxEVT_SET_FOCUS, &ScaleBox::OnConFocus, this);
	numbox->val = gpos;

	return numbox;
}


void ScaleBox::OnOK(wxCommandEvent& WXUNUSED(event))
{
	int i;
	wxString fontname, sizetext, ostext, text;
	wxSize panelsize;
	double oldxfrom, oldxto;
	GraphDat *graph;

	//mainwin->SetStatus("Scale OK");
	mainwin->SetStatusText("");
	//for(i=startgraph; i<startgraph+numgraphs; i++) {
	for(i=0; i<numgraphs; i++) {
		graph = graphwin[i]->dispset[0]->plot[0];
		//oldxfrom = graph->xfrom;
		//oldxto = graph->xto;
		oldxfrom = graphwin[i]->dispset[0]->plot[0]->xfrom;
		oldxto = graphwin[i]->dispset[0]->plot[0]->xto;

		graphwin[i]->yf->GetValue().ToDouble(&(graphwin[i]->dispset[0]->plot[0]->yfrom));
		graphwin[i]->yt->GetValue().ToDouble(&(graphwin[i]->dispset[0]->plot[0]->yto));
		graphwin[i]->xf->GetValue().ToDouble(&(graphwin[i]->dispset[0]->plot[0]->xfrom));
		graphwin[i]->xt->GetValue().ToDouble(&(graphwin[i]->dispset[0]->plot[0]->xto));
		if(graphwin[i]->dispset[0]->plot[0]->xfrom < xmin || graphwin[i]->dispset[0]->plot[0]->xfrom > xmax) {
			mainwin->SetStatusText("X From, value out of range, max 100000");
			graphwin[i]->dispset[0]->plot[0]->xfrom = oldxfrom;
			//graphwin[i]->xf->SetValue(
			if(graph->xto < 1) graphwin[i]->xf->SetValue(text.Format("%.2f", oldxfrom));
			else graphwin[i]->xf->SetValue(text.Format("%.1f", oldxfrom));
		}
		if(graphwin[i]->dispset[0]->plot[0]->xto < xmin || graphwin[i]->dispset[0]->plot[0]->xto > xmax) {
			mainwin->SetStatusText("X To, value out of range, max 100000");
			graphwin[i]->dispset[0]->plot[0]->xto = oldxto;
		}
		graphwin[i]->XYSynch();
	}
	snum.Printf("start %d num %d", startgraph, numgraphs);
	if(mainwin->diagnostic) mainwin->SetStatusText(snum);
	fontname = boxfont.GetNativeFontInfoUserDesc();
	panelsize = GetSize();
	sizetext.Printf(" Size x %d y %d", panelsize.x, panelsize.y);
	ostext.Printf(" OS type %d", ostype);
	//mainwin->SetStatusText(boxfont.GetFaceName());
	//mainwin->SetStatusText(boxfont.GetNativeFontInfoUserDesc());
	if(mainwin->diagnostic) mainwin->SetStatusText("Font: " + fontname + sizetext + ostext);

	ScaleUpdate();
	//XSynch();
	//GraphUpdate();
}


void ScaleBox::OnHistHaz1(wxCommandEvent& WXUNUSED(event))
{
	if(profmode) profmode = 0;
	else if (secmode) secmode = 0;
	else hazmode1 = 1 - hazmode1;
	GraphSwitch();
	//GraphUpdate();
}


void ScaleBox::OnHistHaz2(wxCommandEvent& WXUNUSED(event))
{
	if(profmode) profmode = 0;
	else hazmode2 = 1 - hazmode2;
	GraphSwitch();
	//GraphUpdate();
}

void ScaleBox::OnBinRes1(wxCommandEvent& WXUNUSED(event))
{
	binrestog1 = 1 - binrestog1;
	GraphSwitch();
	//GraphUpdate();
}


void ScaleBox::OnBinRes2(wxCommandEvent& WXUNUSED(event))
{
	binrestog2 = 1 - binrestog2;
	GraphSwitch();
	//GraphUpdate();
}


void ScaleBox::OnNetMode(wxCommandEvent& WXUNUSED(event))
{
	nettog = 1 - nettog;
	GraphSwitch();
}

/*
void ScaleBox::OnNorm(wxCommandEvent& WXUNUSED(event))
{
	normtog = 1 - normtog;
	GraphSwitch();
}*/


/*void ScaleBox::OnAllBurst(wxCommandEvent& WXUNUSED(event))
{
	burstmode = 1 - burstmode;
	GraphSwitch();
	//GraphUpdate();
}*/


void ScaleBox::OnProfMode(wxCommandEvent& WXUNUSED(event))
{
	if(!profmode) profmode = 1;
	else proftype = 1 - proftype;
	secmode = 0;
	//profmode++;
	//if(profmode > 2) profmode = 0;
	GraphSwitch();
	//GraphUpdate();
}


void ScaleBox::OnExpMode(wxCommandEvent& WXUNUSED(event))
{
	//if(!expmode) expmode = 1;
	expmode = 1 - expmode;
	//else proftype = 1 - proftype;
	//secmode = 0;
	//profmode++;
	//if(profmode > 2) profmode = 0;
	GraphSwitch();
	//GraphUpdate();
}


void ScaleBox::OnSecMode(wxCommandEvent& WXUNUSED(event))
{
	if(!secmode) secmode = 1;
	else sectype = 1 - sectype;
	profmode = 0;
	GraphSwitch();
	//GraphUpdate();
}


void ScaleBox::OnDendMode(wxCommandEvent& WXUNUSED(event))
{
	//if(secmode) secmode = 0;
	dendmode = 1 - dendmode;
	GraphSwitch();
}


void ScaleBox::OnSpikes(wxCommandEvent& WXUNUSED(event))
{
	if(timeres == 0) timeres = 1;
	else if(timeres == 1) timeres = 0;
	GraphSwitch();
	//GraphUpdate();
}


void ScaleBox::OnData(wxCommandEvent& WXUNUSED(event))
{
	graph = gpos[0].plot[0]; 

	//if((*gflags)[tag] == type) (*gflags)[tag] = 0;
	//else (*gflags)[tag]++;   

	if(ratedata == graph->spikedata->dispmodemax) ratedata = 0;
	else ratedata++;

	//ratedata = 1 - ratedata;
	(*gflags)["ratedata"] = ratedata;

	graph->spikedata->ColourSwitch(ratedata);

	GraphSwitch();
}


void ScaleBox::OnRateRes(wxCommandEvent& WXUNUSED(event))
{
	rateres = 1 - rateres;
	GraphSwitch();
}


void ScaleBox::OnIntern(wxCommandEvent& WXUNUSED(event))
{
	internflag = 1 - internflag;
	//gsynch = 1 - gsynch;
	if(internflag) gsynch = 1;
	GraphSwitch();

}


/*
void CScaleBox::OnRateRes()
{
double gxfrom, gxto, scale;

SetDlgItemText(ID_errout, "rate res");

gxfrom = greg[gpos[0].data].xfrom;
gxto = greg[gpos[0].data].xto;
scale = gxto - gxfrom;

i = 0;

if(gpos[0].data == 0) {
i = 9;
if(scale > 50) {
greg[9].xfrom = gxfrom + (scale-50)/2;        // zoom middle
greg[9].xto = greg[9].xfrom + 50;
}
else {
greg[9].xfrom = gxfrom;
greg[9].xto = gxto;
}
}

if(gpos[0].data == 9) i = 24;

//greg[9].xfrom = gstart;
//greg[9].xto = gstart + 50;

greg[24].xfrom = gstart;
greg[24].xto = gstart + 500;

gpos[0].data = i;
grate = i;

UpdateScale(0);
}
*/


void ScaleBox::SetMod(Model *model)
{
	/*
	if(dispmod != model->modtype) {
		dispmod = model->modtype;
		gbase = model->graphbase;
		gmod = model;
		GraphSwitch(0);
	}*/
	if(mod != model) {
		mod = model;
		gbase = model->graphbase;
		gmod = model;
		GraphSwitch(0);
	}
}


ParamStore *ScaleBox::GetFlags()
{

	// Old manually defined gflags

	(*gflags)["cortflag"] = cortflag;
	(*gflags)["rateres"] = rateres;
	(*gflags)["timeres"] = timeres;
	//(*gflags)["nettog"] = nettog;
	(*gflags)["expdatflag"] = expdatflag;
	//(*gflags)["burstmode"] = burstmode;
	(*gflags)["ratedata"] = ratedata;
	(*gflags)["profmode"] = profmode;
	//(*gflags)["proftype"] = proftype;
	(*gflags)["internflag"] = internflag;
	(*gflags)["secmode"] = secmode;
	(*gflags)["sectype"] = sectype;
	(*gflags)["dendmode"] = dendmode;
	(*gflags)["vmhflag"] = vmhflag;
	//(*gflags)["normtog"] = 0;

	/*
	if(boxtype != modVMN) {
		(*gflags)["hazmode1"] = hazmode1;
		(*gflags)["hazmode2"] = hazmode2;
		(*gflags)["binrestog1"] = binrestog1;
		(*gflags)["binrestog2"] = binrestog2;
	}*/

	return gflags;
}


void ScaleBox::GraphSwitch(int disp)
{
	GetFlags();
	gmod->GSwitch(gpos, gflags);
	ScaleUpdate();
	if(mainwin->graphbox) mainwin->graphbox->SetGraph();
	//if(disp) GraphUpdate();
}



void ScaleBox::VMHswitch(int newflag)
{
	vmhflag = newflag;
	//GSwitch();
}


void ScaleBox::VasoSwitch(int newflag)
{
	vasoflag = newflag;
	//GSwitch();
}


void ScaleBox::VasoSecSwitch(int newflag)
{
	vasosecflag = newflag;
	//GSwitch();
}


void ScaleBox::VasoNetSwitch(int newflag)
{
	vasonetflag = newflag;
	//GSwitch();
}


void ScaleBox::CortSwitch(int newflag)
{
	cortflag = newflag;
	//GSwitch();
}


void ScaleBox::NetSwitch(int newflag)
{
	nettog = newflag;
	//GSwitch();
}


void ScaleBox::BurstDisp(int newflag)
{
	gpos[0].spikedisp = newflag;
	GraphUpdate();
}


int ScaleBox::CheckValue()
{
	return 1;
}


void ScaleBox::GraphUpdate(int pos)
{
	for(int g=0; g<numgraphs; g++)
		graphwin[g]->UpdateScroll(pos);
}


void ScaleBox::XSynch(int pos)
{
	if(gsynch) {
		GraphDat *graph0 = graphwin[synchcon]->dispset[0]->plot[0];
		if(!graph0->synchx) return;
		for(int i=startgraph; i<startgraph+numgraphs; i++) {
			if(gsync[i] && !gsync[i]->GetValue()) {
				//mainwin->diagbox->Write(text.Format("Sync break pos %d\n", i));
				continue;
			}
			//mainwin->diagbox->Write(text.Format("Sync go pos %d\n", i));
			graph = graphwin[i]->dispset[0]->plot[0];
			if(!graph->synchx) continue;
			if(pos >= 0) graph->scrollpos = pos;
			graph->xfrom = graph0->xfrom;
			graph->xto = graph0->xto;
		}
	}
}


void ScaleBox::SynchScale()
{
	int i, g;
	GraphDat *plot0, *plot;

	for(i=startgraph; i<startgraph+numgraphs; i++) {
		plot0 = graphwin[i]->dispset[0]->plot[0];
		for(g=1; g<graphwin[i]->numdisps; g++) {
			plot = graphwin[i]->dispset[g]->plot[0];
			plot->yfrom = plot0->yfrom; 
			plot->yto = plot0->yto; 
			plot->xfrom = plot0->xfrom; 
			plot->xto = plot0->xto; 
		}
	}
}


void ScaleBox::ScrollUpdate(int index, int pos)
{
	synchcon = index;
	XSynch(pos);
	PanelUpdate();
	//if(gsynch) GraphUpdate(pos);
	//if(gsynch) ;
	//else 
	GraphUpdate();
}


void ScaleBox::ScaleUpdate()
{
	XSynch();
	PanelUpdate();
	GraphUpdate();
}


void ScaleBox::PanelUpdate()
{
	int i, g;
	GraphDat *plot0, *plot;
	//int yplaces;
	wxString text;

	//for(i=startgraph; i<startgraph+numgraphs; i++) {
	for(i=0; i<numgraphs; i++) {
		if(graphwin[i]->numdisps > 0) {
			graph = graphwin[i]->dispset[0]->plot[0];
			plot0 = graphwin[i]->dispset[0]->plot[0];

			if(abs(graph->yto - graph->yfrom) < 10) {
				snum.Printf("%.2f", graph->yfrom);
				graphwin[i]->yf->SetValue(snum);
				snum.Printf("%.2f", graph->yto);
				graphwin[i]->yt->SetValue(snum);
			}
			else {
				snum.Printf("%.1f", graph->yfrom);
				graphwin[i]->yf->SetValue(snum);
				snum.Printf("%.1f", graph->yto);
				graphwin[i]->yt->SetValue(snum);
			}

			if(abs(graph->xto - graph->xfrom) < 1) {
				graphwin[i]->xf->SetValue(text.Format("%.3f", graph->xfrom));
				graphwin[i]->xt->SetValue(text.Format("%.3f", graph->xto));
			}
			else if(abs(graph->xto - graph->xfrom) < 10) {
				graphwin[i]->xf->SetValue(text.Format("%.2f", graph->xfrom));
				graphwin[i]->xt->SetValue(text.Format("%.2f", graph->xto));
			}
			else {
				graphwin[i]->xf->SetValue(text.Format("%.1f", graph->xfrom));
				graphwin[i]->xt->SetValue(text.Format("%.1f", graph->xto));	
			}

			for(g=1; g<graphwin[i]->numdisps; g++) {
				plot = graphwin[i]->dispset[g]->plot[0];
				plot->yfrom = plot0->yfrom; 
				plot->yto = plot0->yto; 
				plot->xfrom = plot0->xfrom; 
				plot->xto = plot0->xto; 
			}
		}
	}
	//if(gsynch) GraphUpdate(pos);
	//else GraphUpdate();
}


void ScaleBox::OnOverlay(wxCommandEvent& event)
{
	int pan1, pan2;
	int toggle;

	if(event.GetId() == ID_overlay) {
		pan1 = overpan1;  
		pan2 = overpan2;
		toggle = overtog;
		overtog = 1 - overtog;
	}
	else {
		pan1 = overpan3;
		pan2 = overpan4;
		toggle = overtog2;
		overtog2 = 1 - overtog2;
	}
	
	if(!toggle) {	
		GraphDisp *pos = graphwin[pan1]->dispset[0];
		graph = pos->plot[0];
		graphwin[pan2]->AddGraph(pos);
		graphwin[pan1]->numdisps--;
		graph->yto = graphwin[pan2]->dispset[0]->plot[0]->yto;
		graph->yfrom = graphwin[pan2]->dispset[0]->plot[0]->yfrom;
		graph->xto = graphwin[pan2]->dispset[0]->plot[0]->xto;
		graph->xfrom = graphwin[pan2]->dispset[0]->plot[0]->xfrom;
	}
	else {
		graphwin[pan2]->numdisps--;
		graphwin[pan1]->AddGraph(graphwin[pan2]->dispset[graphwin[pan2]->numdisps]);	
	}	
	//overtog = 1 - overtog;
	//snum.Printf("overlay = %d", overtog);
	//mainwin->SetStatusText(snum);
	ScaleUpdate();
}


void ScaleBox::OnPosition(wxCommandEvent& event)
{
	if(graphwin[overpan1]->numdisps > 1) {
		GraphDisp *pos = graphwin[overpan1]->dispset[0];
		graphwin[overpan1]->dispset[0] = graphwin[overpan1]->dispset[1];
		graphwin[1]->dispset[1] = pos;
	}
	if(graphwin[overpan2]->numdisps > 1) {
		GraphDisp *pos = graphwin[overpan2]->dispset[0];
		graphwin[overpan2]->dispset[0] = graphwin[overpan2]->dispset[1];
		graphwin[overpan2]->dispset[1] = pos;
	}
	if(gpos[3].numplots > 1) {
		graph = gpos[3].plot[0];
		gpos[3].plot[0] = gpos[3].plot[1];
		gpos[3].plot[1] = graph;
	}
	GraphUpdate();
}


void ScaleBox::AddButton(int id, wxString label, int width, wxBoxSizer *box, int pad, int height)
{
	if(height == 0) height = buttonheight;
	wxButton *button = new wxButton(panel, id, label, wxDefaultPosition, wxSize(width, height));
	button->SetFont(confont);
	box->Add(button, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM, pad);
}


wxBoxSizer *ScaleBox::GSBox(wxString label, wxPanel *storepanel)
{
	wxBoxSizer *gsfilebox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *gsbuttons = new wxBoxSizer(wxHORIZONTAL);

	if(!storepanel) storepanel = panel;

	//gstag = TextInputCombo(100, 20, label);
	gstag = new wxComboBox(panel, wxID_ANY, label, wxDefaultPosition, wxSize(80, 23));
	gstag->SetFont(confont);
	//return text;

	gstag->SetLabel(label);
	gstag->Show(true);
	if(ostype == Mac) {
		AddButton(ID_gstore, "Store", 43, gsbuttons);
		gsbuttons->AddSpacer(2);
		AddButton(ID_gload, "Load", 43, gsbuttons);
	}
	else {
		AddButton(ID_gstore, "Store", 40, gsbuttons);
		gsbuttons->AddSpacer(2);
		AddButton(ID_gload, "Load", 40, gsbuttons);
	}	
	gsfilebox->Add(gstag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	gsfilebox->Add(gsbuttons, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	return gsfilebox;
}

/*
void ScaleBox::OnClose(wxCloseEvent& event)
{
gmod->GHistStore(gstag);
Destroy();
}*/

/*
void ScaleBox::OnPosition(wxCommandEvent& WXUNUSED(event))
{
if(gpos[2].numplots > 1) {
graph = gpos[2].plot[0];
gpos[2].plot[0] = gpos[2].plot[1];
gpos[2].plot[1] = graph;
}
if(gpos[3].numplots > 1) {
graph = gpos[3].plot[0];
gpos[3].plot[0] = gpos[3].plot[1];
gpos[3].plot[1] = graph;
}
GraphUpdate();
}
*/
