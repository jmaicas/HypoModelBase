

#include "hypoplot.h"
#include <hypomodel.h>

#include <iostream>
#include <fstream>
#include <string>
#include <wx/filedlg.h>


PlotBox::PlotBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ParamBox(model, title, pos, size, "plotbox", 0, 1)
{
	column = 0;
	boxtag = "Plot";
	mod = model;
	bool v1mode = false;
	bool dicemode = false;

	wxStaticBoxSizer* dicepanel;
	wxStaticBoxSizer* v1datapanel;

	//InitMenu();
	//SetModFlag(ID_plotflag, "plotflag", "Plot Example Flag", 0); 

	//paramstoretag->SetLabel("default");
	defstore = true;
	plotcount = 0;
	plotlayer = 0;
	coltag = "ycol";

	gridbox = mainwin->gridbox;
	//plotset = mainwin->plotset;
	neurobox = mainwin->neurobox;

	dispindex = 0;
	graphdisp = &mainwin->gpos[dispindex];


	// Parameter controls
	//
	// AddCon(tag string, display string, initial value, click increment, decimal places)
	// ----------------------------------------------------------------------------------


	wxBoxSizer *ycolbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddCon("ycol", "Y Column", 0, 1, 0, 60, 40); 
	ycolbox->Add(paramset.GetCon("ycol"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);

	wxBoxSizer *xcolbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddCon("xcol", "X Column", 0, 1, 0, 60, 40); 
	xcolbox->Add(paramset.GetCon("xcol"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);

	errcheck = new wxCheckBox(panel, ID_errcheck, "");
	errcheck->SetFont(confont);
	errcheck->SetValue(0);

	//errcheck = SetModCheck(ID_errcheck, "errmode", "", false);

	wxBoxSizer *errcolbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddCon("errcol", "Err Column", 0, 1, 0, 60, 40); 
	errcolbox->Add(paramset.GetCon("errcol"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	errcolbox->Add(errcheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	wxBoxSizer *layerbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddCon("plotlayer", "Layer", 0, 1, 0, 60, 40); 
	layerbox->Add(paramset.GetCon("plotlayer"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	wxStaticBoxSizer *dataplotbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Data Plot");
	dataplotbox->Add(xcolbox, 0);
	dataplotbox->Add(ycolbox, 0);
	dataplotbox->Add(errcolbox, 0);
	dataplotbox->Add(layerbox, 0);
	dataplotbox->AddSpacer(5);
	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Plot, "Plot", 50, buttonbox);
	buttonbox->AddSpacer(5);
	AddButton(ID_AddPlot, "Add Plot", 50, buttonbox);
	buttonbox->AddSpacer(5);
	AddButton(ID_RemovePlot, "Remove", 50, buttonbox);
	//AddButton(ID_Load, "Load", 50, buttonbox);
	dataplotbox->Add(buttonbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	//wxSizer *paramfilebox = StoreBox();

	paramset.AddCon("binsize", "Bin Size", 10, 1, 0, 60, 40); 
	wxStaticBoxSizer *databinbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Data Bin");
	databinbox->Add(paramset.GetCon("binsize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	//databinbox->Add(paramset.GetCon("binsize"), 0, wxEXPAND|wxRIGHT|wxLEFT, 5);
	databinbox->AddStretchSpacer();
	//databinbox->AddSpacer(50);
	AddButton(ID_Bin, "Bin Data", 50, databinbox);

	if(dicemode) {
		dicepanel = new wxStaticBoxSizer(wxVERTICAL, panel, "Neuro Dice");
		//wxBoxSizer *v1parambox = new wxBoxSizer(wxHORIZONTAL);
		paramset.AddCon("rolls", "Rolls", 0, 1, 0, 60, 40);
		paramset.AddCon("sides", "Sides", 0, 1, 0, 60, 40);
		paramset.AddCon("fire", "Fire", 0, 1, 0, 60, 40);
		dicepanel->Add(paramset.GetCon("rolls"), 0, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL | wxRIGHT | wxLEFT, 5);
		dicepanel->Add(paramset.GetCon("sides"), 0, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL | wxRIGHT | wxLEFT, 5);
		dicepanel->Add(paramset.GetCon("fire"), 0, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL | wxRIGHT | wxLEFT, 5);
		dicepanel->AddStretchSpacer();
		//databinbox->AddSpacer(50);
		AddButton(ID_neurodice, "Dice", 50, dicepanel);
	}

	if(v1mode) {
		v1datapanel = new wxStaticBoxSizer(wxVERTICAL, panel, "V1 Data");
		//wxBoxSizer *v1parambox = new wxBoxSizer(wxHORIZONTAL);
		paramset.AddCon("v1cells", "Cells", 0, 1, 0, 60, 40);
		paramset.AddCon("v1comp", "Comp", 0, 1, 0, 60, 40);
		paramset.AddCon("v1trials", "Trials", 0, 1, 0, 60, 40);
		v1datapanel->Add(paramset.GetCon("v1cells"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		v1datapanel->Add(paramset.GetCon("v1comp"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		v1datapanel->Add(paramset.GetCon("v1trials"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		v1datapanel->AddStretchSpacer();
		//databinbox->AddSpacer(50);
		AddButton(ID_V1data, "V1 Ana", 50, v1datapanel);
	}

	/*
	wxBoxSizer *datapathbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddText("datapath", "Data Path", "", 60, 350);
	datapathbox->Add(paramset.GetCon("datapath"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	AddButton(ID_PathBrowse, "Browse", 60, datapathbox);

	wxBoxSizer *outpathbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddText("outpath", "Out Path", "", 60, 350);
	outpathbox->Add(paramset.GetCon("outpath"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	AddButton(ID_OutputBrowse, "Browse", 60, outpathbox);

	wxBoxSizer *datatagbox = new wxBoxSizer(wxHORIZONTAL);
	datatag = new TagBox(mainwin, activepanel, ID_Select, "", wxDefaultPosition, wxSize(150, -1), "datatag", mod->path);
	datatagbox->Add(datatag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	if(ostype == Mac) {
		AddButton(ID_Load, "Load", 60, datatagbox);
		AddButton(ID_FileBrowse, "Browse", 60, datatagbox);
	}
	else {
		AddButton(ID_Load, "Load", 40, datatagbox);
		datatagbox->AddSpacer(2);
		AddButton(ID_FileBrowse, "Browse", 50, datatagbox);
	}
	//datapathcon->AddButton("Path", ID_DataBrowse, 40);
	//datafilebox->Add(parambuttons, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	wxBoxSizer *datafilebox = new wxBoxSizer(wxVERTICAL);
	datafilebox->Add(outpathbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	datafilebox->AddSpacer(5);
	datafilebox->Add(datapathbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	datafilebox->AddSpacer(5);
	datafilebox->Add(datatagbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	*/

	wxBoxSizer *conbox = new wxBoxSizer(wxHORIZONTAL);
	conbox->Add(dataplotbox, 0, wxEXPAND|wxALL, 5);	
	conbox->AddSpacer(10);
	conbox->Add(databinbox, 0, wxEXPAND|wxALL, 5);

	if(v1mode) {
		conbox->AddSpacer(10);
		conbox->Add(v1datapanel, 0, wxEXPAND | wxALL, 5);
	}

	if(dicemode) {
		conbox->AddSpacer(10);
		conbox->Add(dicepanel, 0, wxEXPAND | wxALL, 5);
	}

	wxBoxSizer *storebox = StoreBoxSync();
	//wxBoxSizer *storebox = StoreBox();

	delete parambox;

	mainbox->AddSpacer(5);
	//mainbox->Add(parambox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddStretchSpacer();
	mainbox->Add(conbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);	
	mainbox->AddStretchSpacer();
	//mainbox->Add(runbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	//mainbox->AddSpacer(5);
	//mainbox->Add(datafilebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	mainbox->AddStretchSpacer();
	mainbox->Add(storebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	mainbox->AddSpacer(5);

	Connect(ID_Plot, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnPlotXY));
	Connect(ID_AddPlot, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnPlotXY));
	Connect(ID_RemovePlot, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnRemovePlot));
	Connect(ID_Bin, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnBin));
	Connect(ID_PathBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnBrowse));
	Connect(ID_FileBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnBrowse));
	Connect(ID_OutputBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnBrowse));
	Connect(ID_Load, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnLoad));
	Connect(ID_Select, wxEVT_COMBOBOX, wxCommandEventHandler(PlotBox::OnSelect));

	Connect(ID_paramstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnParamStore));
	Connect(ID_paramload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnParamLoad));

	//Connect(ID_V1data, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnV1));
	Connect(ID_neurodice, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnNeuroDice));

	panel->Layout();
}


void PlotBox::OnParamStore(wxCommandEvent& event)
{
	wxString filetag;

	if(synccheck->GetValue()) {
		filetag = paramstoretag->GetValue();
		gridbox->paramstoretag->SetValue(filetag);
		gridbox->OnGridStore(event);
	}
	ParamBox::OnParamStore(event);	

	PlotStore(filetag);
}


void PlotBox::OnParamLoad(wxCommandEvent& event)
{
	wxString filetag;

	if(synccheck->GetValue()) {
		filetag = paramstoretag->GetValue();
		gridbox->paramstoretag->SetValue(filetag);
		//gridbox->GridLoad();
		//gridbox->GridLoadFast();
		//gridbox->GridLoadAll();
		gridbox->OnGridLoad(event);
	}
	ParamBox::ParamLoad();	

	PlotLoad(filetag);
	SetLayer();
}


void PlotBox::TextClick(wxString tag)
{
	diagbox->Write("plotbox textclick " + tag + "\n");
	if(tag == "xcol" || tag == "ycol") coltag = tag;
}


void PlotBox::SpinClick(wxString tag)
{
	diagbox->Write("plotbox spinclick " + tag + "\n");
	if(tag == "plotlayer") SetLayer();
}


void PlotBox::BoxEnter(wxString tag)
{
	diagbox->Write("plotbox boxenter " + tag + "\n");
	if(tag == "plotlayer") SetLayer();
}


void PlotBox::SetColumn(int col)         // set selected column (stored in 'coltag') to new value, used for setting column by clicking data grid
{
	paramset.GetCon(coltag)->SetValue(col);
}


void PlotBox::DataEdit(int col)          // Update plots in response to edit in grid data
{
	int i;
	int xcol, ycol;

	for(i=0; i<plotset.count; i++) {
		xcol = plotset.plotdata[i].xcol;
		ycol = plotset.plotdata[i].ycol;
		if(xcol == col || ycol == col) {
			//graph = graphdisp->plot[i];
			graph = plotset.plotdata[i].graph;
			gridbox->ColumnData(ycol, graph->gdatadv);
			graph->xcount = gridbox->ColumnData(xcol, graph->gdatax);
		}
	}
	mainwin->scalebox->GraphUpdate();
}


void PlotBox::SetLayer()
{
	int xcol, ycol;
	int errcol, errmode;
	int dispcount;
	PlotDat *plotdata;

	plotlayer = paramset.GetCon("plotlayer")->GetValue();
	//plotcount = plotset.count;
	dispcount = plotset.dispcount[dispindex];

	if(dispcount == 0) {
		plotlayer = 0;
		xcol = 0;
		ycol = 0;
		errcol = 0;
		errmode = 0;
	}
	else {
		if(plotlayer >= dispcount) plotlayer = dispcount - 1;
		plotdata = plotset.GetPlot(dispindex, plotlayer);
		//xcol = graphdisp->plotset.plotdata[plotlayer].xcol;
		//ycol = graphdisp->plotset.plotdata[plotlayer].ycol;
		//errcol = graphdisp->plotset.plotdata[plotlayer].errcol;
		//errmode = graphdisp->plotset.plotdata[plotlayer].errmode;
		xcol = plotdata->xcol;
		ycol = plotdata->ycol;
		errcol = plotdata->errcol;
		errmode = plotdata->errmode;
	}

	diagbox->Write(text.Format("SetLayer %d xcol %d ycol %d\n", plotlayer, xcol, ycol));

	paramset.GetCon("plotlayer")->SetValue(plotlayer);
	paramset.GetCon("xcol")->SetValue(xcol);
	paramset.GetCon("ycol")->SetValue(ycol);
	paramset.GetCon("errcol")->SetValue(errcol);
	errcheck->SetValue(errmode);
}


void PlotBox::OnBin(wxCommandEvent& event)
{
	int binsize;

	ParamStore *params = GetParams();
	binsize = (*params)["binsize"];

	WheelBin(binsize);
	mod->graphbase->GetGraph("bin")->xscale = (double)1/binsize;

	mainwin->scalebox->GraphUpdate();
}


void PlotBox::OnNeuroDice(wxCommandEvent& event)
{
	int i, j;
	int rolls, sides; 
	int fire, dice, count;
	int counts[100];
	int dicecheck[100];

	ParamStore *params = GetParams();
	rolls = (*params)["rolls"];
	sides = (*params)["sides"];
	fire = (*params)["fire"];

	diagbox->Write(text.Format("NeuroDice rolls %d  sides %d\n", rolls, sides));

	for(i=0; i<100; i++) counts[i] = 0;
	for(i=0; i<100; i++) dicecheck[i] = 0;


	for(i=0; i<rolls; i++) {
		if(fire > dice) break;
		dice = (int)(mrand01() * sides) + 1;
		count = 1;
		while(dice < fire) {
			dice = (int)(mrand01() * sides) + 1;
			if(dice <= 100) dicecheck[dice]++;
			else diagbox->Write("dice check out of range\n");
			count++;
		}
		if(count < 100) counts[count]++;
		else diagbox->Write("roll count out of range\n");
	}

	for(i=1; i<100; i++) {
		gridbox->currgrid->SetCell(i-1, 0, text.Format("%d", i));
		gridbox->currgrid->SetCell(i-1, 1, text.Format("%d", counts[i]));
	}

	for(i=1; i<10; i++) {
		gridbox->currgrid->SetCell(i-1, 2, text.Format("%d", dicecheck[i]));
	}

	DataEdit(1);
}


void PlotBox::OnSelect(wxCommandEvent& event)
{
	wxString tag, text;
	int index;

	mainwin->diagbox->Write("\nFile Select\n");

	tag = datatag->GetStringSelection();
	index = datatag->GetCurrentSelection();
}


void PlotBox::SetGraph(GraphWindow3 *graphwin)
{
	wxString text; 

	graphdisp = graphwin->dispset[0];
	graphindex = graphwin->graphindex;
	dispindex = graphindex;

	SetLayer();
	graph = graphdisp->plot[plotlayer];

	mainwin->diagbox->Write(text.Format("PlotBox Set Graph %d %s\n", graphindex, graph->gname));
}


void PlotBox::OnBrowse(wxCommandEvent& event)
{
	int i;

	if(event.GetId() == ID_PathBrowse) {
		wxDirDialog *dirdialog = new wxDirDialog(this, "Choose a directory", paramset.GetCon("datapath")->GetLabel(), 0, wxDefaultPosition);
		if(dirdialog->ShowModal() == wxID_OK) paramset.GetCon("datapath")->SetValue(dirdialog->GetPath()); 
	}

	if(event.GetId() == ID_OutputBrowse) {
		wxDirDialog *dirdialog = new wxDirDialog(this, "Choose a directory", paramset.GetCon("outpath")->GetLabel(), 0, wxDefaultPosition);
		if(dirdialog->ShowModal() == wxID_OK) paramset.GetCon("outpath")->SetValue(dirdialog->GetPath()); 
	}

	if(event.GetId() == ID_FileBrowse) {
		wxFileDialog *filedialog = new wxFileDialog(this, "Choose a file", datatag->GetLabel(), "", "", 0, wxDefaultPosition);
		if(filedialog->ShowModal() == wxID_OK) datatag->SetValue(filedialog->GetFilename()); 
	}
}


void PlotBox::OnRemovePlot(wxCommandEvent& event)
{
	int plotindex;

	ParamStore *params = GetParams();
	plotlayer = (*params)["plotlayer"];

	plotindex  = plotset.GetIndex(dispindex, plotlayer);
	if(plotindex < 0) return;

	plotset.RemovePlot(plotindex);
	graphdisp->Remove(plotlayer);
	SetLayer();
	mainwin->scalebox->GraphUpdate();
}


void PlotBox::OnPlotXY(wxCommandEvent& event) 
{
	int xcol, ycol;
	int errcol, errmode;
	int xcount;
	int plotindex;
	int basetype;
	wxString text, tag;
	datdouble *plotdata;
	//PlotSet *plotset;

	ParamStore *params = GetParams();
	ycol = (*params)["ycol"];
	xcol = (*params)["xcol"];
	errcol = (*params)["errcol"];
	errmode = errcheck->GetValue();
	plotlayer = (*params)["plotlayer"];

	//if(!graphdisp->plotset) graphdisp->plotset = plotset;      // currently only manages a single plotset for one graph panel   // *18/12/20 FIX THIS*
	//if(!graphdisp->plotset) plotset = new PlotSet();
	//plotset = &graphdisp->plotset;
	//plotcount = plotset->count;

	//plotlayer = 0; // temp

	if(graphdisp->numplots > plotset.dispcount[dispindex]) graphdisp->Remove(0);   // clear existing graph on new GraphDisp

	if(plotset.dispcount[dispindex] == 0 || event.GetId() == ID_AddPlot) {
		diagbox->Write(text.Format("\nAdd Plot numplots %d\n", graphdisp->numplots));
		graph = mod->graphbase->GetGraph(tag.Format("gridplot%d", plotcount++));
		graphdisp->Add(graph);
		graphdisp->XYSynch();
		//graph->type = graphdisp->plot[0]->type;
		graph->type = 2;  // default to XY line/scatter plot
		plotindex = plotset.dispcount[dispindex];
		diagbox->Write(text.Format("Add Plot numplots %d OK\n", graphdisp->numplots));
	}
	else {
		graph = graphdisp->plot[plotlayer];
		plotindex = plotlayer;
	}

	gridbox->ColumnData(ycol, graph->gdatadv);
	graph->xcount = gridbox->ColumnData(xcol, graph->gdatax);
	if(graph->gdataerr) gridbox->ColumnData(errcol, graph->gdataerr);
	graph->errmode = errmode;

	if(plotset.dispcount[dispindex] <= plotindex) plotset.AddPlot(PlotDat(dispindex, graph->gtag, graph->type, xcol, ycol, errcol, errmode));
	else plotset.SetPlot(plotset.GetIndex(dispindex, plotindex), PlotDat(dispindex, graph->gtag, graph->type, xcol, ycol, errcol, errmode));

	diagbox->Write(text.Format("Plot xcol %d ycol %d\n", xcol , ycol));

	mainwin->scalebox->GraphUpdate();
	//mod->PlotStore();
	if(mainwin->diagbox) mainwin->diagbox->Write("\nOn Plot\n");
}


void PlotBox::OnPlotY(wxCommandEvent& event)
{
	int xcol, ycol;
	wxString text;

	ParamStore *params = GetParams();
	ycol = (*params)["ycol"];
	xcol = (*params)["xcol"];

	gridbox->ColumnData(ycol, &(ycoldata[0]));
	gridbox->ColumnData(xcol, &(xcoldata[0]));
	mainwin->scalebox->GraphUpdate();

	if(mainwin->diagbox) mainwin->diagbox->Write("\nOn Plot\n");
}


void PlotBox::OnLoad(wxCommandEvent& event)
{
	wxString filetag, filepath, text;
	short tagpos;
	FileDat *file;

	filetag = datatag->GetValue();
	filepath = paramset.GetCon("datapath")->GetString();
	//FileDat newfile = FileDat(filetag, filepath);

	tagpos = datatag->FindString(filetag);

	file = mainwin->filebase->Add(FileDat(filetag, filepath));

	/*
	if(tagpos == wxNOT_FOUND) {
	file = mod->filebase->Add(FileDat(filetag, filepath));
	}
	else {
	datatag->Delete(tagpos);
	file = mod->filebase->Find(FileDat(filetag, filepath));
	//file = (FileDat *)datatag->GetClientObject(datatag->GetSelection());
	//if(!datatag->GetClientObject(datatag->GetSelection())) mainwin->diagbox->Write("No data object\n");
	//else file = (FileDat *)datatag->GetClientObject(datatag->GetSelection());
	}*/

	if(tagpos != wxNOT_FOUND) datatag->Delete(tagpos);

	//file = new FileDat(filetag, filepath);
	mainwin->diagbox->Write(text.Format("Entry name %s path %s\n", filetag, filepath));
	//file = new FileDat(filetag, filepath);
	mainwin->diagbox->Write(text.Format("FileDat name %s path %s\n", file->name, file->path));

	datatag->Insert(filetag, 0, file);
	datatag->SetValue(filetag);

	if(mainwin->diagbox) mainwin->diagbox->Write("filetag " + filetag);
	if(mainwin->diagbox) mainwin->diagbox->Write("\nOn Load\n");
	//LoadData(file);
	LoadTxt(file);

	mainwin->filebase->newentry = false;
}


void PlotBox::LoadTxt(FileDat *file)
{
	int i, row, col;
	int start;
	wxString filepath;
	wxString datstring, readline, text;
	string line, filename;
	long timeval;
	double dataval;

	if(!file) {
		mainwin->diagbox->Write("Bad file\n");
		return;
	}
	if(file->path == "") {
		mainwin->diagbox->Write("No file path\n");
		return;
	}
	if(file->name == "") {
		mainwin->diagbox->Write("No file name\n");
		return;
	}

	mainwin->diagbox->Write(text.Format("FileDat path %s name %s\n", file->path, file->name));

	row = 0;
	col = 0;
	filename = file->String().ToStdString();

	ifstream infile(filename.c_str());
	if(infile.is_open()) {
		while(getline(infile, line)) {
			wxString readline(line);
			mainwin->diagbox->Write(line + "\n");
			textgrid->ParseLine(row++, col, line);
			//if(i%100000 == 0) mainwin->diagbox->Write(text.Format("Line %d\n", i)); 
		}
		//mainwin->diagbox->Write(line + "\n");
		infile.close();
	}
	else {
		mainwin->diagbox->Write("\nUnable to open file\n"); 
		mainwin->diagbox->Write(text.Format("filename %s\n", filename));
	}

	mainwin->diagbox->Write("\nFile OK\n"); 
}



void PlotBox::LoadData(FileDat *file)
{
	int i;
	int start;
	wxString filepath;
	wxString datstring, readline, text;
	string line, filename;
	long timeval;
	double dataval;


	if(!file) {
		mainwin->diagbox->Write("Bad file\n");
		return;
	}

	if(file->path == "") {
		mainwin->diagbox->Write("No file path\n");
		return;
	}
	if(file->name == "") {
		mainwin->diagbox->Write("No file name\n");
		return;
	}

	mainwin->diagbox->Write(text.Format("FileDat path %s name %s\n", file->path, file->name));

	//filename = file->path.ToStdString() + "\\" + file->name.ToStdString();
	filename = file->String().ToStdString();

	/*
	if(!infile.Open(filename)) {
	mainwin->diagbox->Write(text.Format("\n%s,file not found\n", filename));
	return;
	}

	readline = infile.ReadLine();
	mainwin->diagbox->Write(text.Format("readline %s\n", readline));
	datstring = readline.BeforeFirst(',');
	datstring.ToLong(&val);
	start = val;
	readline = readline.AfterFirst(' ');
	readline.Trim();
	readline.ToLong(&val);
	mod->wheeldata[0] = val;*/

	ifstream infile(filename.c_str());
	if(infile.is_open()) {
		getline(infile, line);
		mainwin->diagbox->Write(text.Format("readline %s\n", line));
		wxString readline(line); 
		datstring = readline.BeforeFirst(',');
		datstring.ToLong(&timeval);
		start = timeval;
		readline = readline.AfterFirst(' ');
		readline.Trim();
		readline.ToDouble(&dataval);
		wheeldata[0] = dataval;
		i = 1;
		while(getline(infile, line)) {
			wxString readline(line); 
			readline = readline.AfterFirst(' ');
			readline.Trim();
			readline.ToDouble(&dataval);
			wheeldata[i++] = dataval;
			if(i%100000 == 0) mainwin->diagbox->Write(text.Format("Line %d\n", i)); 
		}
		//mainwin->diagbox->Write(line + "\n");
		infile.close();
	}
	else {
		mainwin->diagbox->Write("\nUnable to open file\n"); 
		mainwin->diagbox->Write(text.Format("filename %s\n", filename));
	}

	mainwin->diagbox->Write("\nFile OK\n"); 

	int row = 0;
	for(i=0; i<100000; i++) {
		if(wheeldata[i] > 0) textgrid->SetCellValue(row++, 0, text.Format("%.1f", wheeldata[i]));
	}

	//mod->WheelBin(100);


	//return 0;

	/*
	for(i=1; i<100; i++) {
	readline = infile.ReadLine();
	readline = readline.AfterFirst(' ');
	readline.Trim();
	readline.ToLong(&val);
	mod->wheeldata[i] = val;
	mainwin->diagbox->Write(text.Format("Data %d %d\n", i, mod->wheeldata[i]));
	}*/

	//infile.Close();
}




void PlotBox::PlotStore(wxString filetag)
{
	int i;
	TextFile outfile;
	wxString text, filename, filepath;

	filepath = mod->GetPath() + "/Plots";
	if(!wxDirExists(filepath)) wxMkdir(filepath);

	filetag = gridbox->paramstoretag->GetValue();
	filename = "plot-" + filetag + ".dat";

	outfile.New(filepath + "/" + filename);
	for(i=0; i<plotset.count; i++) {
		outfile.WriteLine(plotset.plotdata[i].StoreDat(i));
	}
	outfile.Close();
}


void PlotBox::PlotLoad(wxString filetag)
{
	int i;
	TextFile infile;
	wxString readline, filename, filepath;
	wxString text;
	wxString gtag;
	int gtype, xcol, ycol;
	int errcol, errmode;
	int disp;
	int version;
	GraphDat *graph;

	diagbox->Write(text.Format("Loading plot set\n"));

	filepath = mod->GetPath() + "/Plots";
	filename = "plot-" + filetag + ".dat";
	if(!infile.Open(filepath + "/" + filename)) {
		diagbox->Write(text.Format("PlotSet file not found\n"));
		return;
	}

	plotset.Reset();          // reset plotset
	plotcount = 0;


	// Load plot data into current graph panel (GraphDisp) PlotSet

	readline = infile.ReadLine();
	while(!readline.IsEmpty()) {

		// Version check
		if(readline.GetChar(0) == 'v') version = ParseLong(&readline, 'v');          // check file version for backwards compatability
		else version = 0;

		// Read plotdat string

		if(version < 3) return;

		disp = ParseLong(&readline, 'p');
		gtag = ParseString(&readline, 'g');
		gtype = ParseLong(&readline, 'e');
		xcol = ParseLong(&readline, 'l');
		ycol = ParseLong(&readline, 'l');
		errcol = ParseLong(&readline, 'l');
		errmode = ParseLong(&readline, 'd');
		

		// Add plotdat to plotset
		plotset.AddPlot(PlotDat(disp, gtag, gtype, xcol, ycol, errcol, errmode));
		plotcount++;

		// Check for end of file and read next line
		if(infile.End()) break;
		readline = infile.ReadLine();		
	}
	infile.Close();


	// Generate plots from loaded data

	dispindex = 0;
	graphdisp = &mainwin->gpos[dispindex];
	for(i=0; i<plotset.count; i++) {
		// Set up graph from plotdat
		graph = mod->graphbase->GetGraph(plotset.plotdata[i].gtag); 
		graph->plotdata = &plotset.plotdata[i];
		graph->type = plotset.plotdata[i].gtype;
		gridbox->ColumnData(plotset.plotdata[i].ycol, graph->gdatadv);
		graph->xcount = gridbox->ColumnData(plotset.plotdata[i].xcol, graph->gdatax);
		gridbox->ColumnData(plotset.plotdata[i].errcol, graph->gdataerr);
		graph->errmode = plotset.plotdata[i].errmode;
		// Insert graph into graphdisp 
		graphdisp = &mainwin->gpos[plotset.plotdata[i].dispindex];
		if(graphdisp->numplots <= i) graphdisp->Add(graph);
		else graphdisp->plot[i] = graph;
	}

	mainwin->scalebox->GraphUpdate();
}


void PlotBox::SetCell(int cellindex, GraphDat *graph)
{
	neurobox->cellpanel->neuroindex = cellindex;
	//(*graph).gname.Printf("n%d", cellindex);
	(*graph).gname = (*neurobox->cellpanel->neurons)[cellindex].name;
	neurobox->cellpanel->NeuroData(false);
}


int PlotBox::GetCellIndex()
{
	return neurobox->cellpanel->neuroindex;
}


void PlotBox::DataSelect(double from, double to)
{
	neurobox->cellpanel->SetSelectRange(from, to);
}


void PlotBox::WheelBin(int binsize)
{
	int i;

	datacount = 100000;
	for(i=0; i<100000; i++) bindata[i] = 0;  
	for(i=0; i<datacount; i++) bindata[i/binsize] += wheeldata[i];   
}



PlotDat::PlotDat(int disp, wxString tag, int type, int xc, int yc, int ec, int em)
{
	gtag = tag;
	gtype = type;
	xcol = xc;
	ycol = yc;
	errcol = ec;
	errmode = em;
	dispindex = disp;
}


wxString PlotDat::StoreDat(int index)
{
	wxString ptext;

	//return ptext.Format("v1 index %d tag %s type %d xcol %d ycol %d", index, gtag, gtype, xcol, ycol); 
	//return ptext.Format("v2 index %d tag %s type %d xcol %d ycol %d errcol %d errmod %d", index, gtag, gtype, xcol, ycol, errcol, errmode); 
	return ptext.Format("v3 disp %d index %d tag %s type %d xcol %d ycol %d errcol %d errmod %d", dispindex, index, gtag, gtype, xcol, ycol, errcol, errmode); 
}


int PlotSet::AddPlot(PlotDat plot)
{
	plotdata.push_back(plot);
	dispcount[plot.dispindex]++;
	return count++;
}


int PlotSet::RemovePlot(int index)
{
	if(index >= count) return 0;
	dispcount[plotdata[index].dispindex]--;
	plotdata.erase(plotdata.begin() + index);
	count--;
	return 1;
}


int PlotSet::SetPlot(int index, PlotDat plot)
{
	if(index < 0 || index > count) return 0;
	plotdata[index] = plot;
	return 1;
}


PlotDat *PlotSet::GetPlot(int dispindex, int layerindex)
{
	int i, layer = 0;

	for(i=0; i<count; i++) {
		if(plotdata[i].dispindex == dispindex) {
			if(layer == layerindex) return &plotdata[i];
			else layer++;
		}
	}
	return NULL;
}


int PlotSet::GetIndex(int dispindex, int layerindex)
{
	int i, layer = 0;

	for(i=0; i<count; i++) {
		if(plotdata[i].dispindex == dispindex) {
			if(layer == layerindex) return i;
			else layer++;
		}
	}
	return -1;
}


void PlotSet::Reset()
{
	count = 0;
	for(int i=0; i<10; i++) dispcount[i] = 0;	
}