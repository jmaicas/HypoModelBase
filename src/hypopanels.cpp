
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


GraphBox::GraphBox(GraphWindow3 *graphw, const wxString & title)
	//: ParamBox(NULL, title, wxDefaultPosition, wxSize(450, 450), "Axes", 0)
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(250, 600),
	wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER)
{
	int i;
	int labelwidth, numwidth;
	graphwin = graphw; 
	diagbox = graphwin->mainwin->diagbox;

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
	numwidth = 50;
	if(ostype == Mac) labelwidth = 50;
	graph = graphwin->graphset[0]->plot[0];
	paramset->AddNum("xlabels", "X Ticks", (double)graph->xlabels, 0, labelwidth, numwidth);
	paramset->AddNum("xstep", "X Step", graph->xstep, 1, labelwidth, numwidth);
	paramset->AddNum("ylabels", "Y Ticks", (double)graph->ylabels, 0, labelwidth, numwidth);
	paramset->AddNum("ystep", "Y Step", graph->ystep, 1, labelwidth, numwidth);
	wxBoxSizer *tickparams = ParamLayout(2);

	wxStaticBoxSizer *xradbox = new wxStaticBoxSizer(wxVERTICAL, panel, "X Tick Mode");
	xrad[0] = new wxRadioButton(panel, 0, "Count", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	xrad[1] = new wxRadioButton(panel, 1, "Step");
	xradbox->Add(xrad[0], 1, wxTOP | wxBOTTOM, 3);
	xradbox->Add(xrad[1], 1, wxTOP | wxBOTTOM, 3);
	xrad[graph->xtickmode]->SetValue(true);

	wxStaticBoxSizer *yradbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Y Tick Mode");
	yrad[0] = new wxRadioButton(panel, 2, "Count", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	yrad[1] = new wxRadioButton(panel, 3, "Step");
	yradbox->Add(yrad[0], 1, wxTOP | wxBOTTOM, 3);
	yradbox->Add(yrad[1], 1, wxTOP | wxBOTTOM, 3);
	yrad[graph->ytickmode]->SetValue(true);

	wxBoxSizer *radbox = new wxBoxSizer(wxHORIZONTAL);
	radbox->Add(xradbox, 1, wxALL, 5);
	radbox->Add(yradbox, 1, wxALL, 5);

	numwidth = 50;

	paramset->AddNum("xshift", "XShift", graph->xshift, 2, labelwidth, numwidth);
	paramset->AddNum("xsample", "Sample", graph->xsample, 0, labelwidth, numwidth);
	paramset->AddNum("xplot", "Width", graph->xplot, 0, labelwidth, numwidth);
	paramset->AddNum("xlabelgap", "X Gap", graph->xlabelgap, 0, labelwidth, numwidth);
	paramset->AddNum("xscale", "XScale", graph->xunitscale, 3, labelwidth, numwidth);
	paramset->AddNum("xdscale", "XDScale", graph->xunitdscale, 1, labelwidth, numwidth);
	paramset->AddNum("yplot", "Height", graph->yplot, 0, labelwidth, numwidth);
	paramset->AddNum("ylabelgap", "Y Gap", graph->ylabelgap, 0, labelwidth, numwidth);
	wxBoxSizer *plotparams = ParamLayout(2);

	paramset->AddNum("labelfontsize", "Font Size", graph->labelfontsize, 2, 50);
	clipcheck = new wxCheckBox(panel, ID_clipmode, "Clip");
	clipcheck->SetFont(confont);
	clipcheck->SetValue(graph->clipmode);
	wxBoxSizer *fontparams = new wxBoxSizer(wxHORIZONTAL);
	fontparams->Add(paramset->GetCon("labelfontsize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	fontparams->AddSpacer(5);
	fontparams->Add(clipcheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	paramset->currlay++;

	linecheck = new wxCheckBox(panel, ID_line, "");
	linecheck->SetFont(confont);
	linecheck->SetValue(graph->linemode);

	scattercheck = new wxCheckBox(panel, ID_scatter, "");
	scattercheck->SetFont(confont);
	scattercheck->SetValue(graph->scattermode);

	//wxBoxSizer *checkbox = new wxBoxSizer(wxHORIZONTAL);
	//scatterparams->Add(paramset->GetCon("scattersize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//scatterparams->AddSpacer(5);
	//checkbox->Add(linecheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//checkbox->Add(scattercheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//paramset->currlay++;

	//wxGridSizer *plotgrid = new wxFlexGridSizer(2, 5, 5);
	//plotgrid->Add(paramset->GetCon("labelfontsize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxST_NO_AUTORESIZE);
	//plotgrid->Add(clipcheck, 0, wxALIGN_CENTRE_VERTICAL);
	//plotgrid->Add(paramset->GetCon("scattersize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxST_NO_AUTORESIZE);
	//plotgrid->Add(scattercheck, 0, wxALIGN_CENTRE_VERTICAL);

	strokepicker = new wxColourPickerCtrl(panel, 0, graph->strokecolour, wxDefaultPosition, wxSize(70, 25), wxCLRP_USE_TEXTCTRL);
	paramset->AddNum("plotstroke", "Stroke", graph->plotstroke, 2, labelwidth);
	wxBoxSizer *strokebox = new wxBoxSizer(wxHORIZONTAL);
	//wxStaticText *label = new wxStaticText(panel, wxID_ANY, "Stroke");
	//label->SetFont(confont);
	//colourbox->Add(label, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	strokebox->Add(paramset->con[paramset->GetID("plotstroke")], wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	paramset->currlay++;
	strokebox->Add(linecheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	strokebox->Add(strokepicker);

	fillpicker = new wxColourPickerCtrl(panel, 0, graph->fillcolour, wxDefaultPosition, wxSize(70, 25), wxCLRP_USE_TEXTCTRL);
	paramset->AddNum("scattersize", "Scatter Size", graph->scattersize, 2, labelwidth);
	wxBoxSizer *fillbox = new wxBoxSizer(wxHORIZONTAL);
	fillbox->Add(paramset->con[paramset->GetID("scattersize")], wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	paramset->currlay++;
	fillbox->Add(scattercheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	fillbox->Add(fillpicker);

	typeset = TypeSet();
	typeset.Add("Line", 5);
	typeset.Add("Line with X data", 2);
	typeset.Add("Line with Sampling", 6);
	typeset.Add("Scatter with Sampling", 8);
	typeset.Add("Bar", 7);
	typeset.Add("Histogram", 1);
	typeset.Add("Spike Rate", 3);

	/*
	typestrings[0] = "Line";
	typestrings[1] = "Line with Sampling";
	typestrings[2] = "Scatter with Sampling";
	typestrings[3] = "Bar";
	typestrings[4] = "Histogram";
	typestrings[5] = "Spike Rate";*/

	typechoice = new wxChoice(panel, 0, wxDefaultPosition, wxSize(150, -1), typeset.numtypes, typeset.names);
	typechoice->SetSelection(typeset.GetIndex(graph->type));
	//typechoice->SetSelection(0);
	wxBoxSizer *typebox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *label = new wxStaticText(panel, wxID_ANY, "Plot Type");
	label->SetFont(confont);
	typebox->Add(label, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	typebox->AddSpacer(5);
	typebox->Add(typechoice);
	

	paramset->AddText("gname", "Name", graph->gname, labelwidth);
	paramset->AddText("xtag", "X Label", graph->xtag, labelwidth);
	paramset->AddText("ytag", "Y Label", graph->ytag, labelwidth);
	wxBoxSizer *labelparams = ParamLayout(1);

	//wxBoxSizer *gapparams = ParamLayout(2);

	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	wxButton *okButton = new wxButton(panel, wxID_OK, "Ok", wxDefaultPosition, wxSize(65, 30));
	wxButton *printButton = new wxButton(panel, ID_Print, "Export EPS", wxDefaultPosition, wxSize(65, 30));
	wxButton *closeButton = new wxButton(panel, wxID_CANCEL, "Close", wxDefaultPosition, wxSize(65, 30));
	buttonbox->Add(okButton, 1);
	buttonbox->Add(printButton, 1, wxLEFT, 5);
	buttonbox->Add(closeButton, 1, wxLEFT, 5);

	//status = StatusBar();
	status = new wxStaticText(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE|wxBORDER_DOUBLE|wxST_NO_AUTORESIZE);
	status->SetFont(confont);
	wxBoxSizer *statusbox = new wxBoxSizer(wxHORIZONTAL);
	statusbox->Add(status, 1, wxEXPAND);

	mainbox->AddSpacer(5);
	mainbox->Add(tickparams, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddStretchSpacer();
	mainbox->Add(radbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->AddStretchSpacer();
	mainbox->Add(plotparams, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddStretchSpacer();
	mainbox->Add(fontparams, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//mainbox->Add(scatterparams, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//mainbox->Add(plotgrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddStretchSpacer();
	mainbox->Add(strokebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->Add(fillbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->AddStretchSpacer();
	mainbox->Add(typebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->AddStretchSpacer();
	mainbox->Add(labelparams, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//mainbox->Add(gapparams, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddStretchSpacer();
	mainbox->Add(buttonbox, 0, wxALIGN_CENTRE | wxTOP | wxBOTTOM, 5);
	mainbox->Add(statusbox, 0, wxEXPAND);

	panel->Layout();

	Connect(wxEVT_CHOICE, wxCommandEventHandler(GraphBox::OnChoice));
	Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(GraphBox::OnRadio));
	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GraphBox::OnOK));
	Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCloseEventHandler(GraphBox::OnClose));
	Connect(ID_Print, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GraphBox::OnPrint));
	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(GraphBox::OnOK));
	Connect(wxEVT_SIZE, wxSizeEventHandler(GraphBox::OnSize));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(GraphBox::OnClose));

	//ShowModal();
	//Destroy(); 
	Show();
}


void GraphBox::SetGraph(GraphWindow3 *newgraphwin)
{
	int i, type;
	wxString tag;
	double pval;

	SetParams();

	if(newgraphwin) graphwin = newgraphwin;            // default newgraphwin=NULL for updating panel without changing graph window
	graph = graphwin->graphset[0]->plot[0];

	paramset->GetCon("gname")->SetValue(graph->gname);
	//paramset->GetCon("gname")->SetLabel("test label");
	paramset->GetCon("xtag")->SetValue(graph->xtag);
	paramset->GetCon("ytag")->SetValue(graph->ytag);

	paramset->GetCon("xlabels")->SetValue(graph->xlabels);
	paramset->GetCon("ylabels")->SetValue(graph->ylabels);
	paramset->GetCon("xstep")->SetValue(graph->xstep);
	paramset->GetCon("ystep")->SetValue(graph->ystep);
	paramset->GetCon("xplot")->SetValue(graph->xplot);
	paramset->GetCon("yplot")->SetValue(graph->yplot);
	paramset->GetCon("xshift")->SetValue(graph->xshift);
	paramset->GetCon("xsample")->SetValue(graph->xsample);
	paramset->GetCon("xscale")->SetValue(graph->xunitscale);
	paramset->GetCon("xdscale")->SetValue(graph->xunitdscale);
	paramset->GetCon("xlabelgap")->SetValue(graph->xlabelgap);
	paramset->GetCon("ylabelgap")->SetValue(graph->ylabelgap);
	paramset->GetCon("plotstroke")->SetValue(graph->plotstroke);
	paramset->GetCon("labelfontsize")->SetValue(graph->labelfontsize);
	paramset->GetCon("scattersize")->SetValue(graph->scattersize);

	clipcheck->SetValue(graph->clipmode);
	scattercheck->SetValue(graph->scattermode);
	xrad[graph->xtickmode]->SetValue(true);
	yrad[graph->ytickmode]->SetValue(true);

	strokepicker->SetColour(graph->strokecolour);
	fillpicker->SetColour(graph->fillcolour);
}


void GraphBox::OnClose(wxCloseEvent& event)
{
	diagbox->Write("Axis box close\n");
	graphwin->mainwin->graphbox = NULL;
	wxDialog::Destroy();
}


void GraphBox::OnSize(wxSizeEvent& event)
{	
	wxString snum;

	wxSize newsize = GetSize();
	wxPoint pos = GetPosition();
	snum.Printf("Box Size X %d Y %d", newsize.x, newsize.y);
	graphwin->mainwin->SetStatusText(snum);
	//boxsize = newsize;
	wxDialog::OnSize(event);
}


void GraphBox::OnChoice(wxCommandEvent& event)
{
	wxString text;

	graph = graphwin->graphset[0]->plot[0];
	int selection = typechoice->GetSelection();
	graph->type = typeset.GetType(selection);
	//status->SetLabel(text.Format("Type %d", typeset.GetType(selection)));
	graphwin->UpdateScroll();
}


void GraphBox::OnRadio(wxCommandEvent& event)
{
	graph = graphwin->graphset[0]->plot[0];

	if(event.GetId() == 0) graph->xtickmode = 0;
	if(event.GetId() == 1) graph->xtickmode = 1;
	if(event.GetId() == 2) graph->ytickmode = 0;
	if(event.GetId() == 3) graph->ytickmode = 1;
}


void GraphBox::OnPrint(wxCommandEvent& event)
{
	OnOK(event);
	graphwin->PrintEPS();
}


void GraphBox::SetParams()
{
	ParamStore *params = paramset->GetParamsNew(boxout);

	graph->xlabels = (*params)["xlabels"];
	graph->ylabels = (*params)["ylabels"];
	graph->xstep = (*params)["xstep"];
	graph->ystep = (*params)["ystep"];
	graph->xplot = (*params)["xplot"];
	graph->yplot = (*params)["yplot"];
	graph->xshift = (*params)["xshift"];
	graph->xsample = (*params)["xsample"];
	graph->xunitscale = (*params)["xscale"];
	graph->xunitdscale = (*params)["xdscale"];
	graph->plotstroke = (*params)["plotstroke"];
	graph->xlabelgap = (*params)["xlabelgap"];
	graph->ylabelgap = (*params)["ylabelgap"];
	graph->labelfontsize = (*params)["labelfontsize"];
	graph->scattersize = (*params)["scattersize"];

	graph->clipmode = clipcheck->GetValue();
	graph->scattermode = scattercheck->GetValue();
	graph->strokecolour = strokepicker->GetColour();
	graph->fillcolour = fillpicker->GetColour();
	graph->colour = custom;

	graph->gname = paramset->GetCon("gname")->GetString();
	graph->xtag = paramset->GetCon("xtag")->GetString();
	graph->ytag = paramset->GetCon("ytag")->GetString();
}


void GraphBox::OnOK(wxCommandEvent& WXUNUSED(event))
{
	long stringnum;
	wxString snum, text;

	graph = graphwin->graphset[0]->plot[0];

	SetParams();
	
	graphwin->UpdateScroll();
	diagbox->Write(text.Format("colourstring %s\n", ColourString(graph->strokecolour)));
}


wxBoxSizer *GraphBox::ParamLayout(int columns)
{
	// Only works for one or two columns currently, columns parameter bigger than two treated like one
	//
	// paramset->currlay allows repeated use after adding more parameters, for separate layout

	int i;
	int colsize = 0;

	wxBoxSizer *box = new wxBoxSizer(wxHORIZONTAL);

	if(columns != 2) colsize = paramset->numparams - paramset->currlay;
	if(columns == 2) {
		if(!column) colsize = (paramset->numparams+1 - paramset->currlay) / 2;
		else colsize = column; 
	}

	//SetVBox(columns);
	for(i=0; i<columns; i++) {
		vbox[i] = new wxBoxSizer(wxVERTICAL);
		vbox[i]->AddSpacer(5);
	}

	for(i=paramset->currlay; i<paramset->currlay + colsize; i++) {
		vbox[0]->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		vbox[0]->AddSpacer(5);
	}
	box->Add(vbox[0], 0);

	if(columns == 2) {
		for(i=paramset->currlay + colsize; i<paramset->numparams; i++) {
			vbox[1]->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
			vbox[1]->AddSpacer(5);
		}
		box->Add(vbox[1], 0);
	}

	paramset->currlay = paramset->numparams;
	return box;
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
	defstore = false;
	mainwin = mod->mainwin;

	model->mainwin->diagbox->Write("ParamBox init\n");

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
	delete panelrefs;

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


void ParamBox::WriteVDU(wxString text)
{
	vdu->AppendText(text);
}


void ParamBox::Initialise()
{	
	modparams = new ParamStore;
	modflags = new ParamStore;
	activepanel = panel;
	paramset = new ParamSet(activepanel);
	flagrefs = new RefStore();
	checkrefs = new RefStore();
	panelrefs = new RefStore();

	paramstoretag = NULL;
	if(boxtype == 0 || boxtype == 1) {
		mainwin->diagbox->Write("Store Box initialise\n");
		paramstoretag = TextInputCombo(100, 20, "", boxname, mod->GetPath());
		paramstoretag->Show(false);
	}
	
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
	Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(ParamBox::OnDClick));
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


void ParamBox::ParamLayout(int columns)                   // Currently for one or two columns, need to generalise
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


void ParamBox::SetPanel(int id, ToolBox *toolbox)
{
	panelrefs->AddTool(id, toolbox);
	Connect(id, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ParamBox::OnPanel));

	mainwin->diagbox->Write(text.Format("SetPanel %d %s\n", id, toolbox->boxname));
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


void ParamBox::OnPanel(wxCommandEvent& event)
{
	int id = event.GetId();

	ToolBox *toolbox = panelrefs->GetTool(id);

	mainwin->diagbox->Write("OnPanel\n");

	if(toolbox->IsShown()) toolbox->Show(false);
	else toolbox->Show(true);
}



void ParamBox::DataMenu()
{
    menuData = new wxMenu;
    menuData->Append(ID_Select, "Selection");
    menuData->Append(ID_Wheel, "Wheel Data");
    menuData->Append(ID_Plot, "Plotting");
    
    menuBar = new wxMenuBar;
    menuBar->Append(menuControls, "Data Tools");
    SetMenuBar(menuBar);
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


void ParamBox::OnDClick(wxMouseEvent& event)
{
	mainwin->diagbox->Write("Param DClick\n");
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
			if(con->label) {
				SetStatus(text.Format("Parameter \'%s\' out of range", con->label->GetLabel()));
				mainwin->diagbox->Write(text.Format("Parameter \'%s\' out of range, min %.2f max %.2f\n", con->label->GetLabel(), con->min, con->max));
			}
		}
		if(value > con->max) {
			//value = con->max;
			value = con->oldvalue;
			con->SetValue(value);
			if(con->label) SetStatus(text.Format("Parameter %s out of range", con->label->GetLabel()));
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
	bool compmode = false;

	if(event.GetId() == ID_Compare) {
		compmode = true;
		mainwin->diagbox->Write(boxname + " param compare\n");
	}
	ParamLoad("", compmode);
}


void ParamBox::ParamLoad(wxString tag, bool compmode)
{
	int id;
	long flagval;
	double datval;
	short diagnostic;
	wxString filetag, filename, filepath;
	wxString readline, datname;
	wxColour redpen("#dd0000"), blackpen("#000000"), greenpen("#009900"), bluepen("#0000dd");
	//TextFile ofp;
	wxString text;

	diagnostic = 0;

	ParamStore *oldparams = GetParams();

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
		if(paramset->ref.check(datname)) {
			id = paramset->ref[datname];
			if(paramset->con[id]->type != textcon) {
				readline.ToDouble(&datval);
				paramset->con[id]->SetPen(blackpen);
				if(compmode && datval != (*oldparams)[datname]) {
					paramset->con[id]->SetPen(greenpen);
					mainwin->diagbox->Write(datname + " param change\n");
				}
				//paramset->con[id]->SetForegroundColour(greenpen);
				paramset->con[id]->SetValue("");
				paramset->con[id]->SetValue(datval);
			}
			else paramset->con[id]->SetValue(readline);
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
	StoreParam();
}


void ParamBox::StoreParam(wxString tag)
{
	wxString filetag, filename, filepath;
	wxString outline;
	wxColour redpen("#dd0000"), blackpen("#000000");
	wxString text;
	//TextFile paramfile;
	//bool check;

	mainwin->diagbox->Write(text.Format("param store %s\n", boxname));

	//if(mod->path == "") filepath = mod->mainwin->parampath;
	//else filepath = mod->path + "/Params";
	//if(!wxDirExists(filepath)) wxMkdir(filepath);
	//filepath = mod->GetPath();
	filepath = mod->GetPath() + "/Params";
	if(!wxDirExists(filepath)) wxMkdir(filepath);

	// Param data file
	if(tag == "") filetag = paramstoretag->GetValue();
	else filetag = tag;
	filename = filepath + "/" + filetag + "-" + boxname + "param.dat";

	// Param file history
	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

  wxTextFile paramfile(filename);
	//check = paramfile.Open(filename);
	if(!paramfile.Exists()) paramfile.Create();
	else if(tag == "" && redtag != filetag) {
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
		if(paramset->con[i]->type != textcon) {
			paramset->con[i]->numbox->GetValue().ToDouble(&((*modparams)[paramset->con[i]->name]));
			outline.Printf("%.8f", (*modparams)[paramset->con[i]->name]);
		}
		else outline = paramset->con[i]->GetString();
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

	mainwin->diagbox->Write("Param File OK\n");
}


