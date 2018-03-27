/*
 *  plotpanels.cpp
 *  HypoModel
 *
 *  Created by Duncan MacGregor.
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <wx/filedlg.h>
using namespace std;

#include "plotmodel.h"


PlotDataBox::PlotDataBox(PlotModel *plotmodel, const wxString& title, const wxPoint& pos, const wxSize& size, int rows, int cols)
: OutBox(plotmodel, title, pos, size, rows, cols)
{
	mod = plotmodel;
	buttonbox->AddSpacer(2);
	AddButton(ID_Neuron, "Neuro", 40, buttonbox);
	Connect(ID_Neuron, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotDataBox::OnNeuro));

	plotbox = mod->plotbox;

	/*
	menuTools = new wxMenu;
  menuTools->Append(ID_Select, "Selection");
  menuTools->Append(ID_Wheel, "Wheel Data");
  menuTools->Append(ID_Plot, "Plotting");
    
  menuBar = new wxMenuBar;
  menuBar->Append(menuTools, "Data Tools");
  SetMenuBar(menuBar);*/

	notebook->SetPageText(0, "Spikes");

	histgrid = new TextGrid(notebook, wxSize(501, 10));
	notebook->AddPage(histgrid, "Histograms");
	for(i=0; i<501; i++) histgrid->SetRowSize(i, 25);
	histgrid->SetDefaultRowSize(20, true);
	histgrid->SetDefaultColSize(60, true);

	hazgrid = new TextGrid(notebook, wxSize(501, 10));
	notebook->AddPage(hazgrid, "Hazards");
	for(i=0; i<501; i++) hazgrid->SetRowSize(i, 25);
	hazgrid->SetDefaultRowSize(20, true);
	hazgrid->SetDefaultColSize(60, true);
}


void PlotDataBox::ColumnSelect(int col)
{
	OutBox::ColumnSelect(col);

	plotbox->paramset->GetCon("ycol")->SetValue(col);
}


void PlotDataBox::OnNeuro(wxCommandEvent& event)
{
	int col, row;
	int spikecount, cellcount;
	wxString text, celltext;
	double celldata;
	int view = 0;

	diagbox->Write("Neuro data scan\n");
	//mod->cellbox->datneuron->SetLabel("OK");
	WriteVDU("Neural data scan..."); 

	cellcount = 0;
	col = 0;
	celltext = textgrid->GetCell(0, 0);
	celltext.Trim();   

	while(!celltext.IsEmpty()) {
		celltext = textgrid->GetCell(0, col);
		celltext.Trim();
		if(mod->celldata.size() <= cellcount) mod->celldata.resize(cellcount + 10);
		//diagbox->Write(text.Format("cellcount %d  cell data size %d\n", cellcount, (int)mod->celldata.size()));
		mod->celldata[cellcount].name = celltext;
		celltext = textgrid->GetCell(1, col);
		celltext.Trim();
		spikecount = 0;
		row = 1;

		while(!celltext.IsEmpty()) {
			celltext.ToDouble(&celldata);
			mod->celldata[cellcount].times[spikecount] = celldata * 1000;
			spikecount++;
			row++;
			celltext = textgrid->GetCell(row, col);
			celltext.Trim();
		}

		mod->celldata[cellcount].spikecount = spikecount;
		cellcount++;
		col++;
		celltext = textgrid->GetCell(0, col);
		celltext.Trim(); 
	}

	if(!cellcount) {
		diagbox->Write("Neuro scan: NO DATA\n");
		//mod->cellbox->datneuron->SetLabel("NO DATA");
	}
	else {
		diagbox->Write(text.Format("Neuro scan: %d cells read OK\n", cellcount)); 
		mod->cellcount = cellcount;
		mod->cellbox->neuroindex = 0;
		mod->cellbox->cellcount = cellcount;
		mod->NeuroAnalysis();
		mod->cellbox->NeuroData();
	}
	WriteVDU("OK\n"); 
}


PlotBox::PlotBox(PlotModel *plotmodel, wxString boxtag, const wxString& title, const wxPoint& pos, const wxSize& size)
: ParamBox(plotmodel, title, pos, size, boxtag)
{
	column = 0;
	boxname = "Plot";
	mod = plotmodel;

	InitMenu();

	SetModFlag(ID_plotflag, "plotflag", "Plot Example Flag", 0); 

	paramstoretag->SetLabel("default");
	defstore = true;
	plotcount = 1;
	plotlayer = 0;
	

	// Parameter controls
	//
	// AddCon(tag string, display string, initial value, click increment, decimal places)
	// ----------------------------------------------------------------------------------


	wxBoxSizer *ycolbox = new wxBoxSizer(wxHORIZONTAL);
	paramset->AddCon("ycol", "Y Column", 0, 1, 0, 60, 40); 
	ycolbox->Add(paramset->GetCon("ycol"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);

	wxBoxSizer *xcolbox = new wxBoxSizer(wxHORIZONTAL);
	paramset->AddCon("xcol", "X Column", 0, 1, 0, 60, 40); 
	xcolbox->Add(paramset->GetCon("xcol"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);

	wxStaticBoxSizer *dataplotbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Data Plot");
	dataplotbox->Add(xcolbox, 0);
	dataplotbox->Add(ycolbox, 0);
	dataplotbox->AddSpacer(5);
	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Plot, "Plot", 50, buttonbox);
	buttonbox->AddSpacer(5);
	AddButton(ID_AddPlot, "Add Plot", 50, buttonbox);
	//AddButton(ID_Load, "Load", 50, buttonbox);
	dataplotbox->Add(buttonbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	//wxSizer *paramfilebox = StoreBox();

	paramset->AddCon("binsize", "Bin Size", 10, 1, 0, 60, 40); 
	wxStaticBoxSizer *databinbox = new wxStaticBoxSizer(wxVERTICAL, panel, "Data Bin");
	databinbox->Add(paramset->GetCon("binsize"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	//databinbox->Add(paramset->GetCon("binsize"), 0, wxEXPAND|wxRIGHT|wxLEFT, 5);
	databinbox->AddStretchSpacer();
	//databinbox->AddSpacer(50);
	AddButton(ID_Bin, "Bin Data", 50, databinbox);
	
	wxBoxSizer *datapathbox = new wxBoxSizer(wxHORIZONTAL);
	paramset->AddText("datapath", "Data Path", "", 60, 350);
	datapathbox->Add(paramset->GetCon("datapath"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	AddButton(ID_PathBrowse, "Browse", 60, datapathbox);

	wxBoxSizer *outpathbox = new wxBoxSizer(wxHORIZONTAL);
	paramset->AddText("outpath", "Out Path", "", 60, 350);
	outpathbox->Add(paramset->GetCon("outpath"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	AddButton(ID_OutputBrowse, "Browse", 60, outpathbox);

	wxBoxSizer *datatagbox = new wxBoxSizer(wxHORIZONTAL);
	datatag = new TagBox(activepanel, ID_Select, "", wxDefaultPosition, wxSize(150, -1), "datatag", mod->GetPath());
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

	wxBoxSizer *conbox = new wxBoxSizer(wxHORIZONTAL);
	conbox->Add(dataplotbox, 0, wxEXPAND|wxALL, 5);	
	conbox->AddSpacer(10);
	conbox->Add(databinbox, 0, wxEXPAND|wxALL, 5);	
	
	delete parambox;
	
	mainbox->AddSpacer(5);
	//mainbox->Add(parambox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddStretchSpacer();
	mainbox->Add(conbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);	
	mainbox->AddStretchSpacer();
	//mainbox->Add(runbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	//mainbox->AddSpacer(5);
	mainbox->Add(datafilebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	mainbox->AddSpacer(5);

	Connect(ID_Plot, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnPlotXY));
	Connect(ID_AddPlot, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnPlotXY));
	Connect(ID_Bin, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnBin));
	Connect(ID_PathBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnBrowse));
	Connect(ID_FileBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnBrowse));
	Connect(ID_OutputBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnBrowse));
	Connect(ID_Load, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotBox::OnLoad));
	Connect(ID_Select, wxEVT_COMBOBOX, wxCommandEventHandler(PlotBox::OnSelect));
	
	panel->Layout();
}


void PlotBox::OnBin(wxCommandEvent& event)
{
	int binsize;

	ParamStore *params = GetParams();
	binsize = (*params)["binsize"];

	mod->WheelBin(binsize);
	mod->graphbase->GetGraph("bin")->xscale = (double)1/binsize;

	mainwin->scalebox->GraphUpdate();
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
	graphdisp = graphwin->dispset[0];

	//if(mainwin->graphbox) layer = mainwin->graphbox->paramset->GetValue("plotlayer");
	//graph = graphdisp->plot[layer];
	if(mainwin->graphbox) graph = mainwin->graphbox->graph;
	else graph = graphdisp->plot[plotlayer];

	//GraphUpdate();
}


void PlotBox::OnBrowse(wxCommandEvent& event)
{
	int i;

	if(event.GetId() == ID_PathBrowse) {
		wxDirDialog *dirdialog = new wxDirDialog(this, "Choose a directory", paramset->GetCon("datapath")->GetLabel(), 0, wxDefaultPosition);
		if(dirdialog->ShowModal() == wxID_OK) paramset->GetCon("datapath")->SetValue(dirdialog->GetPath()); 
	}

	if(event.GetId() == ID_OutputBrowse) {
		wxDirDialog *dirdialog = new wxDirDialog(this, "Choose a directory", paramset->GetCon("outpath")->GetLabel(), 0, wxDefaultPosition);
		if(dirdialog->ShowModal() == wxID_OK) paramset->GetCon("outpath")->SetValue(dirdialog->GetPath()); 
	}

	if(event.GetId() == ID_FileBrowse) {
		wxFileDialog *filedialog = new wxFileDialog(this, "Choose a file", datatag->GetLabel(), "", "", 0, wxDefaultPosition);
		if(filedialog->ShowModal() == wxID_OK) datatag->SetValue(filedialog->GetFilename()); 
	}
}


void PlotBox::OnPlotXY(wxCommandEvent& event)
{
	int xcol, ycol;
	int xcount;
	wxString text, tag;
	//GraphDat *graph;
	datdouble *plotdata;

	if(mainwin->graphbox) graph = mainwin->graphbox->graph;

	if(event.GetId() == ID_AddPlot) {
		if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("\nAdd Plot numplots %d\n", graphdisp->numplots));
	    graph = mod->graphbase->GetGraph(tag.Format("coldata%d", plotcount++));
		//graph = mod->graphbase->GetGraph("coldata1");
		//graphdisp->Set(1, graph); 
		graphdisp->Add(graph);
		graphdisp->XYSynch();
		if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("Add Plot numplots %d OK\n", graphdisp->numplots));
	}

	ParamStore *params = GetParams();
	ycol = (*params)["ycol"];
	xcol = (*params)["xcol"];

	plotdata = graph->gdatadv;

	mod->databox->ColumnData(ycol, plotdata);
	xcount = mod->databox->ColumnData(xcol, &(mod->xcoldata));

	//gpos[i].Front((*graphbase)[gdex]);
	//gtag.Printf("coldata%d", ycol);
	//graph = mod->graphbase->GetGraph(gtag);

	
	//graphwin->gpos[0].Set
	//graphdisp->Set(0, graph);

	//graph = mod->graphbase->GetGraph("coldata0");
	//graph->gdatadv = &(mod->coldata[ycol]);
	graph->gdatax = &(mod->xcoldata);
	graph->xcount = xcount; 

	mainwin->scalebox->GraphUpdate();
	if(mainwin->diagbox) mainwin->diagbox->Write("\nOn Plot\n");
}


void PlotBox::OnPlotY(wxCommandEvent& event)
{
	int xcol, ycol;
	wxString text;

	ParamStore *params = GetParams();
	ycol = (*params)["ycol"];
	xcol = (*params)["xcol"];

	mod->databox->ColumnData(ycol, &(mod->coldata[0]));
	mod->databox->ColumnData(xcol, &(mod->xcoldata));
	mainwin->scalebox->GraphUpdate();

	if(mainwin->diagbox) mainwin->diagbox->Write("\nOn Plot\n");
}


void PlotBox::OnLoad(wxCommandEvent& event)
{
	wxString filetag, filepath, text;
	short tagpos;
	FileDat *file;
	
	filetag = datatag->GetValue();
	filepath = paramset->GetCon("datapath")->GetString();
	//FileDat newfile = FileDat(filetag, filepath);
	
	tagpos = datatag->FindString(filetag);

	file = mod->filebase->Add(FileDat(filetag, filepath));

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

	mod->filebase->newentry = false;
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
		mod->wheeldata[0] = dataval;
		i = 1;
		while(getline(infile, line)) {
			wxString readline(line); 
			readline = readline.AfterFirst(' ');
			readline.Trim();
			readline.ToDouble(&dataval);
			mod->wheeldata[i++] = dataval;
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
		if(mod->wheeldata[i] > 0) textgrid->SetCellValue(row++, 0, text.Format("%.1f", mod->wheeldata[i]));
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