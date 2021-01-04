
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
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(325, 910),
	wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER)
{
	int i;
	int labelwidth, numwidth;
	graphwin = graphw; 
	diagbox = graphwin->mainwin->diagbox;

	wxString text;

	ostype = GetSystem();
	autosynch = true;

	buttonheight = 23;
    boxfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
    confont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	//boxfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	//confont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	if(ostype == Mac) {
		buttonheight = 25;
        boxfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
        confont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
		//boxfont = wxFont(12, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
		//confont = wxFont(10, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	}
	column = 0;
	fontset = &graphwin->mainwin->fontset;

	panel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
	panel->SetFont(boxfont);
	mainbox = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(mainbox);

	paramset.panel = panel; // = new ParamSet(panel);
	boxout = new BoxOut(NULL, graphwin->mainwin->diagbox, "Axis Panel"); 
	parambox = new wxBoxSizer(wxHORIZONTAL);

	labelwidth = 40;
	numwidth = 50;
	if(ostype == Mac) labelwidth = 50;
	graph = graphwin->dispset[0]->plot[0];
	paramset.AddNum("xlabels", "X Ticks", (double)graph->xlabels, 0, labelwidth, numwidth);
	paramset.AddNum("xstep", "X Step", graph->xstep, 2, labelwidth, numwidth);
	paramset.AddNum("ylabels", "Y Ticks", (double)graph->ylabels, 0, labelwidth, numwidth);
	paramset.AddNum("ystep", "Y Step", graph->ystep, 2, labelwidth, numwidth);
	wxBoxSizer *tickparams = ParamLayout(2);

	wxStaticBoxSizer *xtickradbox = new wxStaticBoxSizer(wxVERTICAL, panel, "X Tick Mode");
	xtickrad[0] = new wxRadioButton(panel, 0, "None", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	xtickrad[1] = new wxRadioButton(panel, 1, "Count");
	xtickrad[2] = new wxRadioButton(panel, 2, "Step");
	xtickradbox->Add(xtickrad[0], 1, wxTOP | wxBOTTOM, 3);
	xtickradbox->Add(xtickrad[1], 1, wxTOP | wxBOTTOM, 3);
	xtickradbox->Add(xtickrad[2], 1, wxTOP | wxBOTTOM, 3);
	xtickrad[graph->xtickmode]->SetValue(true);

	wxStaticBoxSizer *xlabradbox = new wxStaticBoxSizer(wxVERTICAL, panel, "X Labels");
	xlabrad[0] = new wxRadioButton(panel, 100, "None", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	xlabrad[1] = new wxRadioButton(panel, 101, "All");
	xlabrad[2] = new wxRadioButton(panel, 102, "Ends");
	xlabradbox->Add(xlabrad[0], 1, wxTOP | wxBOTTOM, 3);
	xlabradbox->Add(xlabrad[1], 1, wxTOP | wxBOTTOM, 3);
	xlabradbox->Add(xlabrad[2], 1, wxTOP | wxBOTTOM, 3);
	if(graph->xlabelmode >= 0 && graph->xlabelmode < 3) xlabrad[graph->xlabelmode]->SetValue(true);
	else diagbox->Write(text.Format("ERROR xlabelmode %d\n", graph->xlabelmode));

	wxStaticBoxSizer *ytickradbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Y Tick Mode");
	ytickrad[0] = new wxRadioButton(panel, 3, "None", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	ytickrad[1] = new wxRadioButton(panel, 4, "Count");
	ytickrad[2] = new wxRadioButton(panel, 5, "Step");
	ytickradbox->Add(ytickrad[0], 1, wxTOP | wxBOTTOM, 3);
	ytickradbox->Add(ytickrad[1], 1, wxTOP | wxBOTTOM, 3);
	ytickradbox->Add(ytickrad[2], 1, wxTOP | wxBOTTOM, 3);
	ytickrad[graph->ytickmode]->SetValue(true);

	wxStaticBoxSizer *ylabradbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Y Labels");
	ylabrad[0] = new wxRadioButton(panel, 200, "None", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	ylabrad[1] = new wxRadioButton(panel, 201, "All");
	ylabrad[2] = new wxRadioButton(panel, 202, "Ends");
	ylabradbox->Add(ylabrad[0], 1, wxTOP | wxBOTTOM, 3);
	ylabradbox->Add(ylabrad[1], 1, wxTOP | wxBOTTOM, 3);
	ylabradbox->Add(ylabrad[2], 1, wxTOP | wxBOTTOM, 3);
	if(graph->ylabelmode >= 0 && graph->ylabelmode < 3) ylabrad[graph->ylabelmode]->SetValue(true);
	else diagbox->Write(text.Format("ERROR ylabelmode %d\n", graph->ylabelmode));

	wxBoxSizer *radbox = new wxBoxSizer(wxHORIZONTAL);
	radbox->Add(xtickradbox, 1, wxALL, 5);
	radbox->Add(xlabradbox, 1, wxALL, 5);
	radbox->Add(ytickradbox, 1, wxALL, 5);
	radbox->Add(ylabradbox, 1, wxALL, 5);

	// Scale mode controls  December 2017
	wxStaticBoxSizer *xscalemodebox = new wxStaticBoxSizer(wxVERTICAL, panel, "X Scale");
	xscalerad[0] = new wxRadioButton(panel, 10, "Linear", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	xscalerad[1] = new wxRadioButton(panel, 11, "Log");
	xscalemodebox->Add(xscalerad[0], 1, wxTOP | wxBOTTOM, 3);
	xscalemodebox->Add(xscalerad[1], 1, wxTOP | wxBOTTOM, 3);
	xscalerad[graph->xscalemode]->SetValue(true);

	wxStaticBoxSizer *yscalemodebox = new wxStaticBoxSizer(wxVERTICAL, panel, "Y Scale");
	yscalerad[0] = new wxRadioButton(panel, 12, "Linear", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	yscalerad[1] = new wxRadioButton(panel, 13, "Log");
	yscalemodebox->Add(yscalerad[0], 1, wxTOP | wxBOTTOM, 3);
	yscalemodebox->Add(yscalerad[1], 1, wxTOP | wxBOTTOM, 3);
	yscalerad[graph->yscalemode]->SetValue(true);

	// Axis mode controls  June 2020
	wxStaticBoxSizer *xaxisbox = new wxStaticBoxSizer(wxVERTICAL, panel, "X Axis");
	xaxisrad[0] = new wxRadioButton(panel, 300, "Off", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	xaxisrad[1] = new wxRadioButton(panel, 301, "On");
	xaxisbox->Add(xaxisrad[0], 1, wxTOP | wxBOTTOM, 3);
	xaxisbox->Add(xaxisrad[1], 1, wxTOP | wxBOTTOM, 3);
	xaxisrad[graph->xaxis]->SetValue(true);

	wxStaticBoxSizer *yaxisbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Y Axis");
	yaxisrad[0] = new wxRadioButton(panel, 400, "Off", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	yaxisrad[1] = new wxRadioButton(panel, 401, "On");
	yaxisbox->Add(yaxisrad[0], 1, wxTOP | wxBOTTOM, 3);
	yaxisbox->Add(yaxisrad[1], 1, wxTOP | wxBOTTOM, 3);
	yaxisrad[graph->yaxis]->SetValue(true);

	wxBoxSizer *scalemoderadbox = new wxBoxSizer(wxHORIZONTAL);
	scalemoderadbox->Add(xaxisbox, 1, wxALL, 5);
	scalemoderadbox->Add(xscalemodebox, 1, wxALL, 5);
	scalemoderadbox->Add(yscalemodebox, 1, wxALL, 5);
	scalemoderadbox->Add(yaxisbox, 1, wxALL, 5);
	
	numwidth = 50;
	paramset.AddNum("xshift", "XShift", graph->xshift, 2, labelwidth, numwidth);
	paramset.AddNum("xscale", "XScale", graph->xunitscale, 3, labelwidth, numwidth);
	paramset.AddNum("xdscale", "XDScale", graph->xunitdscale, 1, labelwidth, numwidth);
	paramset.AddNum("xplot", "Width", graph->xplot, 0, labelwidth, numwidth);
	paramset.AddNum("xlogbase", "XLogB", graph->xlogbase, 4, labelwidth, numwidth);
	paramset.AddNum("xlabelgap", "X Gap", graph->xlabelgap, 0, labelwidth, numwidth);
	paramset.AddNum("xlabelplaces", "X Places", graph->xlabelplaces, 0, labelwidth, numwidth);
	paramset.AddNum("barwidth", "Bar Wid", graph->barwidth, 0, labelwidth, numwidth);
	paramset.AddNum("yshift", "YShift", graph->yshift, 2, labelwidth, numwidth);
	paramset.AddNum("yscale", "YScale", graph->yunitscale, 3, labelwidth, numwidth);
	paramset.AddNum("ydscale", "YDScale", graph->yunitdscale, 1, labelwidth, numwidth);
	paramset.AddNum("yplot", "Height", graph->yplot, 0, labelwidth, numwidth);
	paramset.AddNum("ylogbase", "YLogB", graph->ylogbase, 4, labelwidth, numwidth);
	paramset.AddNum("ylabelgap", "Y Gap", graph->ylabelgap, 0, labelwidth, numwidth);
	paramset.AddNum("ylabelplaces", "Y Places", graph->ylabelplaces, 0, labelwidth, numwidth);
	paramset.AddNum("bargap", "Bar Gap", graph->bargap, 0, labelwidth, numwidth);

	wxBoxSizer *plotparams = ParamLayout(2);

	paramset.GetCon("xshift")->SetMinMax(-100000, 100000);
	paramset.GetCon("yshift")->SetMinMax(-100000, 100000);
	paramset.GetCon("xlabelplaces")->SetMinMax(-1, 100);
	paramset.GetCon("ylabelplaces")->SetMinMax(-1, 100);

	wxBoxSizer *samplebox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddNum("xsample", "XSample", graph->xsample, 0, labelwidth, numwidth);
	samplebox->Add(paramset.GetCon("xsample"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	clipcheck = new wxCheckBox(panel, ID_clipmode, "Clip");
	clipcheck->SetFont(confont);
	clipcheck->SetValue(graph->clipmode);
	samplebox->AddSpacer(40);
	samplebox->Add(clipcheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	samplebox->AddSpacer(25);
	paramset.currlay++;

	// Font Selector
	fontchoice = new wxChoice(panel, ID_font, wxDefaultPosition, wxSize(100, -1), fontset->numtypes, fontset->names);
	fontchoice->SetSelection(fontset->GetIndex(graph->labelfont));
	wxBoxSizer *fontbox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *fontlabel = new wxStaticText(panel, wxID_ANY, "Font");
	fontlabel->SetFont(confont);
	fontbox->Add(fontlabel, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	fontbox->AddSpacer(5);
	fontbox->Add(fontchoice, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	fontbox->AddSpacer(5);
	paramset.AddNum("labelfontsize", "Size", graph->labelfontsize, 2, 25);
	fontbox->Add(paramset.GetCon("labelfontsize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	paramset.currlay++;

	linecheck = new wxCheckBox(panel, ID_line, "");
	linecheck->SetFont(confont);
	linecheck->SetValue(graph->linemode);

	scattercheck = new wxCheckBox(panel, ID_scatter, "");
	scattercheck->SetFont(confont);
	scattercheck->SetValue(graph->scattermode);

	//wxBoxSizer *checkbox = new wxBoxSizer(wxHORIZONTAL);
	//scatterparams->Add(paramset.GetCon("scattersize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//scatterparams->AddSpacer(5);
	//checkbox->Add(linecheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//checkbox->Add(scattercheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//paramset.currlay++;

	//wxGridSizer *plotgrid = new wxFlexGridSizer(2, 5, 5);
	//plotgrid->Add(paramset.GetCon("labelfontsize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxST_NO_AUTORESIZE);
	//plotgrid->Add(clipcheck, 0, wxALIGN_CENTRE_VERTICAL);
	//plotgrid->Add(paramset.GetCon("scattersize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxST_NO_AUTORESIZE);
	//plotgrid->Add(scattercheck, 0, wxALIGN_CENTRE_VERTICAL);

	strokepicker = new wxColourPickerCtrl(panel, 0, graph->strokecolour, wxDefaultPosition, wxSize(70, 25), wxCLRP_USE_TEXTCTRL);
	paramset.AddNum("plotstroke", "Stroke", graph->plotstroke, 2, labelwidth);
	wxBoxSizer *strokebox = new wxBoxSizer(wxHORIZONTAL);
	//wxStaticText *label = new wxStaticText(panel, wxID_ANY, "Stroke");
	//label->SetFont(confont);
	//colourbox->Add(label, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	strokebox->Add(paramset.con[paramset.GetID("plotstroke")], wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	paramset.currlay++;
	strokebox->Add(linecheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	strokebox->Add(strokepicker);

	fillstrokecheck = new wxCheckBox(panel, ID_fillstroke, "");
	fillstrokecheck->SetFont(confont);
	fillstrokecheck->SetValue(graph->fillstroke);
	strokebox->Add(fillstrokecheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	fillpicker = new wxColourPickerCtrl(panel, 0, graph->fillcolour, wxDefaultPosition, wxSize(70, 25), wxCLRP_USE_TEXTCTRL|wxCLRP_SHOW_ALPHA);
	paramset.AddNum("scattersize", "Scatter Size", graph->scattersize, 2, labelwidth);
	wxBoxSizer *fillbox = new wxBoxSizer(wxHORIZONTAL);
	fillbox->Add(paramset.con[paramset.GetID("scattersize")], wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	paramset.currlay++;
	fillbox->Add(scattercheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	fillbox->Add(fillpicker);

	fillcheck = new wxCheckBox(panel, ID_fillmode, "");
	fillcheck->SetFont(confont);
	fillcheck->SetValue(graph->fillmode);
	fillbox->Add(fillcheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	// plot layer selection and synch
	paramset.AddCon("plotlayer", "Layer", 0, 1, 0, labelwidth);
	wxBoxSizer *layerbox = new wxBoxSizer(wxHORIZONTAL);
	layerbox->Add(paramset.GetCon("plotlayer"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	paramset.currlay++;
	wxButton *syncButton = new wxButton(panel, ID_Sync, "Synch", wxDefaultPosition, wxSize(50, buttonheight));
	layerbox->Add(syncButton, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	
	// plot type selection
	typeset = TypeSet();
	typeset.Add("Line", 5);
	//diagbox->Write(typeset.List() + "\n");
	typeset.Add("Line with X data", 2);
	//diagbox->Write(typeset.List() + "\n");
	typeset.Add("Line with Sampling", 6);
	//diagbox->Write(typeset.List() + "\n");
	typeset.Add("Scatter with Sampling", 8);
	//diagbox->Write(typeset.List() + "\n");
	typeset.Add("Bar", 7);
	//diagbox->Write(typeset.List() + "\n");
	typeset.Add("Histogram", 1);
	//diagbox->Write(typeset.List() + "\n");
	typeset.Add("Spike Rate", 3);
	//diagbox->Write(typeset.List() + "\n");
	typeset.Add("Bar with X data", 9);
	//diagbox->Write(typeset.List() + "\n");
	typeset.Add("Scatter with X data", 10);
	//diagbox->Write(typeset.List());
	typeset.Add("Mean field plot", 11);
	diagbox->Write(typeset.List());

	typechoice = new wxChoice(panel, ID_type, wxDefaultPosition, wxSize(150, -1), typeset.numtypes, typeset.names);
	typechoice->SetSelection(typeset.GetIndex(graph->type));
	wxBoxSizer *typebox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *label = new wxStaticText(panel, wxID_ANY, "Plot Type");
	label->SetFont(confont);
	typebox->Add(label, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	typebox->AddSpacer(5);
	typebox->Add(typechoice);
	
	paramset.AddText("gname", "Name", graph->gname, labelwidth);
	paramset.AddText("xtag", "X Label", graph->xtag, labelwidth);
	paramset.AddText("ytag", "Y Label", graph->ytag, labelwidth);
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
	mainbox->Add(scalemoderadbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->AddStretchSpacer();
	mainbox->Add(plotparams, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//mainbox->AddStretchSpacer();
	mainbox->Add(samplebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddSpacer(5);
	//mainbox->Add(scatterparams, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//mainbox->Add(plotgrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->Add(fontbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->AddStretchSpacer();
	mainbox->Add(strokebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->Add(fillbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->AddStretchSpacer();
	mainbox->Add(layerbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
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
	Connect(wxEVT_CHECKBOX, wxCommandEventHandler(GraphBox::OnCheck));
	Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(GraphBox::OnRadio));
	Connect(ID_Sync, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GraphBox::OnSynch));
	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GraphBox::OnOK));
	Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCloseEventHandler(GraphBox::OnClose));
	Connect(ID_Print, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GraphBox::OnPrint));
	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(GraphBox::OnOK));
	Connect(wxEVT_SIZE, wxSizeEventHandler(GraphBox::OnSize));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(GraphBox::OnClose));
	Connect(wxEVT_SPIN, wxSpinEventHandler(GraphBox::OnSpin));


	//ShowModal();
	//Destroy(); 
	Show();
}


// SetControls sets panel parameter controls from a selected graph
void GraphBox::SetControls()
{
	paramset.GetCon("gname")->SetValue(graph->gname);
	//paramset.GetCon("gname")->SetLabel("test label");
	paramset.GetCon("xtag")->SetValue(graph->xtag);
	paramset.GetCon("ytag")->SetValue(graph->ytag);

	paramset.GetCon("xlabels")->SetValue(graph->xlabels);
	paramset.GetCon("ylabels")->SetValue(graph->ylabels);
	paramset.GetCon("xstep")->SetValue(graph->xstep);
	paramset.GetCon("ystep")->SetValue(graph->ystep);
	paramset.GetCon("xplot")->SetValue(graph->xplot);
	paramset.GetCon("yplot")->SetValue(graph->yplot);
	paramset.GetCon("xshift")->SetValue(graph->xshift);
	paramset.GetCon("xsample")->SetValue(graph->xsample);
	paramset.GetCon("xscale")->SetValue(graph->xunitscale);
	paramset.GetCon("xdscale")->SetValue(graph->xunitdscale);
	paramset.GetCon("xlabelgap")->SetValue(graph->xlabelgap);
	paramset.GetCon("ylabelgap")->SetValue(graph->ylabelgap);
	paramset.GetCon("xlabelplaces")->SetValue(graph->xlabelplaces);
	paramset.GetCon("ylabelplaces")->SetValue(graph->ylabelplaces);
	paramset.GetCon("plotstroke")->SetValue(graph->plotstroke);
	paramset.GetCon("labelfontsize")->SetValue(graph->labelfontsize);
	paramset.GetCon("scattersize")->SetValue(graph->scattersize);
	paramset.GetCon("yscale")->SetValue(graph->yunitscale);
	paramset.GetCon("ydscale")->SetValue(graph->yunitdscale);
	paramset.GetCon("yshift")->SetValue(graph->yshift);

	clipcheck->SetValue(graph->clipmode);
	linecheck->SetValue(graph->linemode);
	scattercheck->SetValue(graph->scattermode);
	fillcheck->SetValue(graph->fillmode);
	fillstrokecheck->SetValue(graph->fillstroke);

	xtickrad[graph->xtickmode]->SetValue(true);
	ytickrad[graph->ytickmode]->SetValue(true);
	xlabrad[graph->xlabelmode]->SetValue(true);
	ylabrad[graph->ylabelmode]->SetValue(true);
	xscalerad[graph->xscalemode]->SetValue(true);
	yscalerad[graph->yscalemode]->SetValue(true);
	xaxisrad[graph->xaxis]->SetValue(true);
	yaxisrad[graph->yaxis]->SetValue(true);

	strokepicker->SetColour(graph->strokecolour);
	fillpicker->SetColour(graph->fillcolour);

	typechoice->SetSelection(typeset.GetIndex(graph->type));
	fontchoice->SetSelection(fontset->GetIndex(graph->labelfont));
}


void GraphBox::SetGraph(GraphWindow3 *newgraphwin)
{
	int i, type;
	wxString tag;
	double pval;

	SetParams();	// read and store params for previous plot

	if(newgraphwin) graphwin = newgraphwin;            // default newgraphwin=NULL for updating panel without changing graph window
	graph = graphwin->dispset[0]->plot[0];

	SetControls();
}


void GraphBox::OnClose(wxCloseEvent& event)
{
	diagbox->Write("Axis box close\n");
	graphwin->mainwin->graphbox = NULL;
	wxDialog::Destroy();
}


void GraphBox::OnSpin(wxSpinEvent& event)
{
	wxString text;

	int layer = paramset.GetValue("plotlayer");

	GraphDisp *graphdisp = graphwin->dispset[0];
	if(layer > graphdisp->numplots-1) paramset.GetCon("plotlayer")->SetValue(graphdisp->numplots-1);
	else {
		SetParams();   // read and store params for previous plot
		graph = graphdisp->plot[layer];
		diagbox->Write(text.Format("Layer %d plot %s\n", layer, graph->gname));
		SetControls();  // update params for new plot
	}
}


void GraphBox::SynchLayers() {

	int i;

	GraphDisp *graphdisp = graphwin->dispset[0];
	for(i=0; i<graphdisp->numplots; i++) SetParamsCopy(graphdisp->plot[i]);

	graphdisp->XYSynch(graph);

	graphwin->UpdateScroll();
}


void GraphBox::SynchPlotSize() {

	int i;

	HypoMain *mainwin = graphwin->mainwin;
	GraphDat *setgraph;

	for(i=0; i<mainwin->numdraw; i++) {
		setgraph = mainwin->graphwin[i]->dispset[0]->plot[0];
		setgraph->xplot = graph->xplot;
		setgraph->yplot = graph->yplot;
	}
}


void GraphBox::OnSynch(wxCommandEvent& WXUNUSED(event))
{
	SynchLayers();
	SynchPlotSize();
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


void GraphBox::OnCheck(wxCommandEvent& event)
{
	OnOK(event);
}


void GraphBox::OnChoice(wxCommandEvent& event)
{
	wxString text;
	int selection;

	if(event.GetId() == ID_type) {
		selection = typechoice->GetSelection();
		graph->type = typeset.GetType(selection);
		if(graph->plotdata) (*graph->plotdata).gtype = graph->type;
	
		graphwin->UpdateScroll();

		diagbox->Write(typeset.List());
		diagbox->Write(text.Format("Choice Select %d Type %d", selection, typeset.GetType(selection)));
	}

	if(event.GetId() == ID_font) {
		selection = fontchoice->GetSelection();
		graph->labelfont = fontset->GetType(selection);
		diagbox->Write(fontset->List());
		diagbox->Write(text.Format("Choice Select %d Font %d", selection, fontset->GetType(selection)));
	}
}


void GraphBox::OnRadio(wxCommandEvent& event)
{
	if(event.GetId() == 0) graph->xtickmode = 0;
	if(event.GetId() == 1) graph->xtickmode = 1;
	if(event.GetId() == 2) graph->xtickmode = 2;

	if(event.GetId() == 3) graph->ytickmode = 0;
	if(event.GetId() == 4) graph->ytickmode = 1;
	if(event.GetId() == 5) graph->ytickmode = 2;

	if(event.GetId() == 10) graph->xscalemode = 0;
	if(event.GetId() == 11) graph->xscalemode = 1;

	if(event.GetId() == 12) graph->yscalemode = 0;
	if(event.GetId() == 13) graph->yscalemode = 1;

	if(event.GetId() == 100) graph->xlabelmode = 0;
	if(event.GetId() == 101) graph->xlabelmode = 1;
	if(event.GetId() == 102) graph->xlabelmode = 2;

	if(event.GetId() == 200) graph->ylabelmode = 0;
	if(event.GetId() == 201) graph->ylabelmode = 1;
	if(event.GetId() == 202) graph->ylabelmode = 2;

	if(event.GetId() == 300) graph->xaxis = 0;
	if(event.GetId() == 301) graph->xaxis = 1;

	if(event.GetId() == 400) graph->yaxis = 0;
	if(event.GetId() == 401) graph->yaxis = 1;

	OnOK(event);
}


void GraphBox::OnPrint(wxCommandEvent& event)
{
	OnOK(event);
	graphwin->PrintEPS();
}


// SetParams reads panel parameters into GraphDat graph 
void GraphBox::SetParams(GraphDat *setgraph)
{
	ParamStore *params = paramset.GetParamsNew(boxout);
	if(setgraph) graph = setgraph;

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
	graph->xlabelplaces = (*params)["xlabelplaces"];
	graph->ylabelplaces = (*params)["ylabelplaces"];
	graph->labelfontsize = (*params)["labelfontsize"];
	graph->scattersize = (*params)["scattersize"];
	graph->yunitscale = (*params)["yscale"];
	graph->yunitdscale = (*params)["ydscale"];
	graph->yshift = (*params)["yshift"];

	graph->barwidth = (*params)["barwidth"];
	graph->bargap = (*params)["bargap"];

	graph->linemode = linecheck->GetValue();
	graph->clipmode = clipcheck->GetValue();
	graph->scattermode = scattercheck->GetValue();
	graph->fillmode = fillcheck->GetValue();

	graph->fillstroke = fillstrokecheck->GetValue();
	graph->strokecolour = strokepicker->GetColour();
	graph->fillcolour = fillpicker->GetColour();
	graph->colour = custom;

	graph->gname = paramset.GetCon("gname")->GetString();
	graph->xtag = paramset.GetCon("xtag")->GetString();
	graph->ytag = paramset.GetCon("ytag")->GetString();

	graph->xlogbase = (*params)["xlogbase"];
	graph->ylogbase = (*params)["ylogbase"];
}


// SetParamsCopy copies panel parameters to another GraphDat graph on the same GraphWindow (used for overlay)
void GraphBox::SetParamsCopy(GraphDat *setgraph)
{
	ParamStore *params = paramset.GetParamsNew(boxout);

	setgraph->xlabels = (*params)["xlabels"];
	setgraph->ylabels = (*params)["ylabels"];
	setgraph->xstep = (*params)["xstep"];
	setgraph->ystep = (*params)["ystep"];
	setgraph->xplot = (*params)["xplot"];
	setgraph->yplot = (*params)["yplot"];
	setgraph->xshift = (*params)["xshift"];
	setgraph->yshift = (*params)["yshift"];
	setgraph->xsample = (*params)["xsample"];
	setgraph->xunitscale = (*params)["xscale"];
	setgraph->xunitdscale = (*params)["xdscale"];
	setgraph->yunitscale = (*params)["yscale"];
	setgraph->yunitdscale = (*params)["ydscale"];
	setgraph->plotstroke = (*params)["plotstroke"];
	setgraph->xlabelgap = (*params)["xlabelgap"];
	setgraph->ylabelgap = (*params)["ylabelgap"];
	setgraph->xlabelplaces = (*params)["xlabelplaces"];
	setgraph->ylabelplaces = (*params)["ylabelplaces"];
	setgraph->labelfontsize = (*params)["labelfontsize"];
	setgraph->scattersize = (*params)["scattersize"];
	setgraph->barwidth = (*params)["barwidth"];
	setgraph->bargap = (*params)["bargap"];

	setgraph->linemode = linecheck->GetValue();
	setgraph->clipmode = clipcheck->GetValue();
	setgraph->scattermode = scattercheck->GetValue();
	setgraph->fillmode = fillcheck->GetValue();
	setgraph->fillstroke = fillstrokecheck->GetValue();
	
	setgraph->xtag = paramset.GetCon("xtag")->GetString();
	setgraph->ytag = paramset.GetCon("ytag")->GetString();
	
	setgraph->xtickmode = graph->xtickmode;
	setgraph->ytickmode = graph->ytickmode;	

	setgraph->xlabelmode = graph->xlabelmode;
	setgraph->ylabelmode = graph->ylabelmode;

	setgraph->xaxis = graph->xaxis;
	setgraph->yaxis = graph->yaxis;	

	setgraph->xscalemode = graph->xscalemode;
	setgraph->yscalemode = graph->yscalemode;

	setgraph->xlogbase = graph->xlogbase;
	setgraph->ylogbase = graph->ylogbase;

	setgraph->labelfont = graph->labelfont;
}


void GraphBox::OnOK(wxCommandEvent& WXUNUSED(event))
{
	int g;
	long stringnum;
	wxString snum, text;

	if(autosynch) SynchLayers();

	for(g=0; g<graphwin->numdisps; g++) SetParamsCopy(graphwin->dispset[g]->plot[0]);

	//graph = graphwin->dispset[0]->plot[0];      // not sure if this is needed 7/3/16
	SetParams();
	
	graphwin->UpdateScroll();
	//diagbox->Write(text.Format("colourstring %s\n", ColourString(graph->strokecolour)));
}


wxBoxSizer *GraphBox::ParamLayout(int columns)
{
	// Only works for one or two columns currently, columns parameter bigger than two treated like one
	//
	// paramset.currlay allows repeated use after adding more parameters, for separate layout

	int i;
	int colsize = 0;

	wxBoxSizer *box = new wxBoxSizer(wxHORIZONTAL);

	if(columns != 2) colsize = paramset.numparams - paramset.currlay;
	if(columns == 2) {
		if(!column) colsize = (paramset.numparams+1 - paramset.currlay) / 2;
		else colsize = column; 
	}

	//SetVBox(columns);
	for(i=0; i<columns; i++) {
		vbox[i] = new wxBoxSizer(wxVERTICAL);
		vbox[i]->AddSpacer(5);
	}

	for(i=paramset.currlay; i<paramset.currlay + colsize; i++) {
		vbox[0]->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		vbox[0]->AddSpacer(5);
	}
	box->Add(vbox[0], 0);

	if(columns == 2) {
		for(i=paramset.currlay + colsize; i<paramset.numparams; i++) {
			vbox[1]->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
			vbox[1]->AddSpacer(5);
		}
		box->Add(vbox[1], 0);
	}

	paramset.currlay = paramset.numparams;
	return box;
}


ParamBox::ParamBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, wxString tag, int type, int storeflag)
	: ToolBox(model->mainwin, tag, title, pos, size, type)
	//wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX)
{	
	autorun = 0;
	//blankevent = new wxCommandEvent();
	//boxtag = tag;
	redtag = "";
	histmode = 0;
	storemode = storeflag;
	mod = model;
	boxtype = type;
	//status = NULL;
	defbutt = 0;
	defstore = false;
	diagmode = 0;
	mainwin = mod->mainwin;
	column = 0;
	buttonwidth = 50;
	modmode = 0;

	model->mainwin->diagbox->Write("ParamBox init\n");

	Initialise();
}


/*
ParamBox::ParamBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, bool close)
: ToolBox(model->mainwin, title, pos, size, close)
{	
autorun = 0;
boxtag = "";
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
	delete conflags;
	//delete paramset;
	delete flagrefs;
	delete conflagrefs;
	delete checkrefs;
	delete panelrefs;
}


void ParamBox::CopyParams(ParamStore *params)
{
	int i;
	wxString tag;
	double pval;

	for(i=0; i<paramset.numparams; i++) {
		tag = paramset.con[i]->name;
		pval = (*params)[tag];
		paramset.con[i]->SetValue(pval);
	}

	// This requires ParamStore to match paramset
	// Consider version which loops based on ParamStore
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
	if(vdu) vdu->AppendText(text);
}


void ParamBox::Initialise()
{	
	modparams = new ParamStore;
	modflags = new ParamStore;
	conflags = new ParamStore;
	activepanel = panel;
	//paramset = new ParamSet(activepanel);
	flagrefs = new RefStore();
	conflagrefs = new RefStore();
	checkrefs = new RefStore();
	panelrefs = new RefStore();

	paramstoretag = NULL;
	if(storemode) {
		mainwin->diagbox->Write("Store Box initialise " + boxtag + "\n");
		paramstoretag = TextInputCombo(120, 20, "", boxtag, mod->path);
		paramstoretag->Show(false);
	}

	if(boxtype == 0) parambox = new wxBoxSizer(wxHORIZONTAL);

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

	if(columns != 2) colsize = paramset.numparams;
	if(columns == 2) {
		if(!column) colsize = (paramset.numparams+1) / 2;
		else colsize = column; 
	}

	SetVBox(columns);

	for(i=0; i<colsize; i++) {
		vbox[0]->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		vbox[0]->AddSpacer(5);
	}
	parambox->Add(vbox[0], 0);

	if(columns == 2) {
		for(i=colsize; i<paramset.numparams; i++) {
			vbox[1]->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
			vbox[1]->AddSpacer(5);
		}
		parambox->Add(vbox[1], 0);
	}
}


void ParamBox::PanelParamLayout(wxBoxSizer *box, int columns)
{
	for(i=paramindex; i<paramset.numparams; i++) {
		box->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	}
	paramindex = paramset.numparams;
}


void ParamBox::HistStore()
{
	short i;
	wxString filename, filepath;
	wxString outline;

	filepath = mod->GetPath();
	filename = filepath + "/" + boxtag + "-hist.ini";
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
	filename = filepath + "/" + boxtag + "-hist.ini";

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


wxBoxSizer *ParamBox::StoreBoxSync(wxString label, wxPanel *storepanel)
{
	synccheck = new wxCheckBox(panel, wxID_ANY, "Sync");
	synccheck->SetValue(true);

	wxBoxSizer *storebox = StoreBox(label, storepanel);
	storebox->Add(synccheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	return storebox;
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
		AddButton(ID_paramstore, "Store", 40, parambuttons);
		AddButton(ID_paramload, "Load", 40, parambuttons);
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


// Copied from SetModCheck, not currently in use
wxCheckBox *ParamBox::SetBoxCheck(int id, wxString checktag, wxString checktext, int state)
{
	wxCheckBox *newcheck;
	(*modflags)[checktag] = state;
	newcheck = new wxCheckBox(activepanel, id, checktext);
	newcheck->SetFont(confont);
	newcheck->SetValue(state);
	checkrefs->AddRef(id, checktag, 1, newcheck);
	Connect(id, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ParamBox::OnBoxCheck));

	return newcheck;
}


wxCheckBox *ParamBox::SetModCheck(int id, wxString checktag, wxString checktext, int state)
{
	wxCheckBox *newcheck;
	(*modflags)[checktag] = state;
	newcheck = new wxCheckBox(activepanel, id, checktext);
	newcheck->SetFont(confont);
	newcheck->SetValue(state);
	checkrefs->AddRef(id, checktag, 1, newcheck);
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


void ParamBox::SetConFlag(int id, wxString flagname, wxString flagtext, int state, wxMenu *menu)
{
	if(menu == NULL) menu = menuControls;
	(*conflags)[flagname] = state;
	conflagrefs->AddRef(id, flagname);
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

	//mainwin->diagbox->Write(text.Format("SetPanel %d %s\n", id, toolbox->boxtag));       // note, breaks if box not already initialised
}


void ParamBox::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();
	wxString checktag = checkrefs->GetRef(id);

	if((*modflags)[checktag] == 0) (*modflags)[checktag] = 1;
	else (*modflags)[checktag] = 0;
}


void ParamBox::OnBoxCheck(wxCommandEvent &event)
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


void ParamBox::InitMenu(int type)
{
	if(type == menu_model) {
		menuControls = new wxMenu;
		menuControls->Append(ID_autorun, "Auto Run", "Toggle Autorun", wxITEM_CHECK);
		menuControls->Check(ID_autorun, autorun);

		menuModel = new wxMenu;

		//menuParamSet = new wxMenu;

		menuBar = new wxMenuBar;

		menuBar->Append(menuControls, "Controls");
		menuBar->Append(menuModel, "Model");
		//menuBar->Append(menuParamSet, "Parameters");
	}

	if(type == menu_gridbox) {
		menuMode = new wxMenu;
		menuBar = new wxMenuBar;
		menuBar->Append(menuMode, "Mode");
	}

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


void ParamBox::SetCount(int count)
{
	wxString text; 

	runcount->SetLabel(text.Format("%d %%", count));

	//if(mainwin->diagnostic) mod->diagbox->textbox->AppendText(text.Format("Count call, count %.2f, countmark %.2f\n", count, countmark));
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
	if(mainwin->diagnostic) mainwin->SetStatusText(boxtag + " Run");

	countmark = 0;
	//GetParams();
	mod->RunModel();
}


ParamCon *ParamBox::GetCon(wxString tag)
{
	return paramset.GetCon(tag);
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
	//diagbox->Write("on spin\n");
}


void ParamBox::OnBox(wxCommandEvent & event)
{
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
for(i=0; i<paramset.numparams; i++)
paramset.con[i]->numbox->GetValue().ToDouble(&((*pstore)[paramset.con[i]->name]));
return pstore;
}*/


ParamStore *ParamBox::GetParams(ParamStore *pstore)
{ 
	double value;
	ParamCon *con;
	wxString text;

	//SetStatus("");

	//if(mainwin->diagnostic) mod->diagbox->textbox->AppendText(text.Format("%s get params\n", boxtag));

	if(pstore == NULL) pstore = modparams;
	for(i=0; i<paramset.numparams; i++) {
		con = paramset.con[i];
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
		//paramset.con[i]->numbox->GetValue().ToDouble(&((*pstore)[paramset.con[i]->name]));
		(*pstore)[paramset.con[i]->name] = value;
		con->oldvalue = value;
	}

	return pstore;
}


ParamStore *ParamBox::GetNumParams()
{ 
	for(i=0; i<paramset.numparams; i++)
		paramset.num[i]->numbox->GetValue().ToDouble(&((*modparams)[paramset.num[i]->name]));
	return modparams;
}


void ParamBox::OnParamLoad(wxCommandEvent& event)
{
	bool compmode = false;

	if(event.GetId() == ID_Compare) {
		compmode = true;
		mainwin->diagbox->Write(boxtag + " param compare\n");
	}
	ParamLoad("", compmode);
}


void ParamBox::ParamLoadSync(wxString filetag, bool compmode) {}


void ParamBox::ParamLoad(wxString filetag, bool compmode)
{
	int id;
	long flagval;
	double datval;
	short diagnostic;
	wxString filename, filepath;
	wxString readline, datname;
	TextFile checkfile;
	wxString text;

	diagnostic = false;

	ParamStore *oldparams = GetParams();

	mod->diagbox->Write(text.Format("param load %s\n", boxtag));

	//if(mod->path == "") filepath = mod->mainwin->parampath;
	//else filepath = mod->path + "/Params";
	filepath = mod->GetPath() + "/Params";

	//if(diagnostic) ofp.New(boxtag + "paramload.txt");

	// Param data file

	//if(diagnostic) ofp.WriteLine(text.Format("tag %s", tag));
	if(paramstoretag) {
		if(filetag == "") filetag = paramstoretag->GetValue();
		else if(filetag != "default") paramstoretag->SetValue(filetag);
	}

	filename = filepath + "/" + filetag + "-" + boxtag + "param.dat";

	diagbox->Write("paramload " + filename + "\n");

	if(!checkfile.Exists(filename)) {
		if(filename.Contains("EVOFIT")) filename = filepath + "/" + filetag + "-" + "VASOFIT" + "param.dat";
	}

	wxTextFile paramfile(filename);

	if(!paramfile.Exists()) {
		if(paramstoretag) paramstoretag->SetValue("Not found");
		//if(diagnostic) ofp.Close();
		return;
	}

	// Param file history
	if(paramstoretag && filetag != "default") {
		short tagpos = paramstoretag->FindString(filetag);
		if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
		paramstoretag->Insert(filetag, 0);
		diagbox->Write("Insert " + filetag + "\n");

		redtag = "";
		paramstoretag->SetForegroundColour(mainwin->blackpen);
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
		if(paramset.ref.check(datname)) {
			id = paramset.ref[datname];
			if(paramset.con[id]->type != textcon) {
				readline.ToDouble(&datval);
				paramset.con[id]->SetPen(mainwin->blackpen);
				if(compmode && datval != (*oldparams)[datname]) {
					paramset.con[id]->SetPen(mainwin->greenpen);
					mainwin->diagbox->Write(datname + " param change\n");
				}
				//paramset.con[id]->SetForegroundColour(greenpen);
				paramset.con[id]->SetValue("");
				paramset.con[id]->SetValue(datval);
			}
			else paramset.con[id]->SetValue(readline);
		}
		if(diagnostic) mainwin->diagbox->Write(text.Format("Model Param ID %d, Name %s, Value %.4f\n", id, datname, datval)); 
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
		if(diagnostic) mainwin->diagbox->Write(text.Format("Model flag ID %d, Name %s, Set %d\n", id, datname, flagval)); 
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
		if(id) ((wxCheckBox *)checkrefs->GetBox(id))->SetValue(flagval);
		if(diagnostic) mainwin->diagbox->Write(text.Format("Model check ID %d, Name %s, Set %d\n", id, datname, flagval)); 
		readline = paramfile.GetNextLine();	
	}
	paramfile.Close();
	//if(diagnostic) ofp.Close();
}


void ParamBox::OnParamStore(wxCommandEvent& event)
{
	StoreParam();
}


void ParamBox::StoreParam(wxString filetag)
{
	wxString filename, filepath;
	wxString outline;
	wxColour redpen("#dd0000"), blackpen("#000000");
	wxString text;
	//TextFile paramfile;
	bool newtag = false;

	if(filetag == "") newtag = true;

	mainwin->diagbox->Write(text.Format("param store %s\n", boxtag));

	//if(mod->path == "") filepath = mod->mainwin->parampath;
	//else filepath = mod->path + "/Params";
	//if(!wxDirExists(filepath)) wxMkdir(filepath);
	//filepath = mod->GetPath();
	filepath = mod->GetPath() + "/Params";
	if(!wxDirExists(filepath)) wxMkdir(filepath);

	if(paramstoretag) {
		if(filetag == "") filetag = paramstoretag->GetValue();
		else paramstoretag->SetValue(filetag);
	}

	// Param data file
	//if(tag == "") filetag = paramstoretag->GetValue();
	//else filetag = tag;
	filename = filepath + "/" + filetag + "-" + boxtag + "param.dat";

	// Param file history
	if(paramstoretag && filetag != "default") {
		short tagpos = paramstoretag->FindString(filetag);
		if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
		paramstoretag->Insert(filetag, 0);
	}

	wxTextFile paramfile(filename);
	if(!paramfile.Exists()) paramfile.Create();
	else if(newtag && redtag != filetag) {
		if(paramstoretag) {
			paramstoretag->SetForegroundColour(redpen);
			paramstoretag->SetValue("");
			paramstoretag->SetValue(filetag);
		}
		redtag = filetag;
		return;
	}

	redtag = "";
	if(paramstoretag) {
		paramstoretag->SetForegroundColour(blackpen);
		paramstoretag->SetValue("");
		paramstoretag->SetValue(filetag);
	}

	paramfile.Open();
	paramfile.Clear();

	for(i=0; i<paramset.numparams; i++) {
		if(paramset.con[i]->type != textcon) {
			paramset.con[i]->numbox->GetValue().ToDouble(&((*modparams)[paramset.con[i]->name]));
			outline.Printf("%.8f", (*modparams)[paramset.con[i]->name]);
		}
		else outline = paramset.con[i]->GetString();
		paramfile.AddLine(paramset.con[i]->name + " " + outline);
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



DatPanel::DatPanel(ToolBox *tbox, int size)
{
	box = tbox;
	numdats = 0;
	numwidth = 60;
	maxdats = size;
	datset = new wxStaticText*[size];
	tags = new wxString[size];
}


DatPanel::~DatPanel()
{
	delete[] datset;
	delete[] tags;
}


void DatPanel::AddDat(wxString tag, wxString value)
{
	ref[tag] = numdats;
	tags[numdats] = tag;
	datset[numdats] = box->NumPanel(numwidth);
	numdats++;
}


wxStaticText *DatPanel::GetDat(wxString tag)
{
	return datset[(int)ref[tag]];
}
