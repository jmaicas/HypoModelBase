

#include "wx/wx.h"
#include <hypomodel.h>
#include "wx/grid.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

#include <fcntl.h>
//#include <unistd.h>
#include <sys/types.h>
//#include <sys/mman.h>
#include <sys/stat.h>


CellBox::CellBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ParamBox(mod, title, pos, size, "cellbox", 1)
{
	int datwidth, labelwidth;

	diagbox = mod->diagbox;
	cellcount = 0;
	paramindex = 0;
	textgrid = NULL;

	selectcount = 2;
	//selected = new SpikeDat();
	//selectdata[0] = new BurstDat(true);
    //selectdata[1] = new BurstDat(true);

	selectspikes[0] = new int[100000];
	selectspikes[1] = new int[100000];
	spikeselectLink = false;

	for(i=0; i<100000; i++) {
		selectspikes[0][i] = 0;
		selectspikes[1][i] = 0;
	}

	long notestyle = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS;
	wxAuiNotebook *tabpanel = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notestyle);

	ToolPanel *analysispanel = new ToolPanel(this, tabpanel);
	analysispanel->SetFont(boxfont);
	wxBoxSizer *analysisbox = new wxBoxSizer(wxVERTICAL);
	analysispanel->SetSizer(analysisbox);
	activepanel = analysispanel;
	paramset->panel = analysispanel;

	wxBoxSizer *histparambox = new wxBoxSizer(wxVERTICAL);
	paramset->AddNum("normscale", "Norm Scale", 10000, 0, 70, 50);
	paramset->AddNum("histrange", "Hist Range", 1000, 0, 70, 50);
	//paramset->AddNum("binsize", "Bin Size", 5, 0, 70, 50);
	PanelParamLayout(histparambox);

	wxBoxSizer *colbox = new wxBoxSizer(wxHORIZONTAL); 
	//colbox->AddStretchSpacer();
	//colbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//colbox->AddSpacer(10);
	colbox->Add(histparambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//colbox->AddStretchSpacer();

	analysisbox->AddStretchSpacer();
	analysisbox->Add(colbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	analysisbox->AddStretchSpacer();
	analysispanel->Layout();


	//

	ToolPanel *selectpanel = new ToolPanel(this, tabpanel);
	selectpanel->SetFont(boxfont);
	wxBoxSizer *selectbox = new wxBoxSizer(wxVERTICAL);
	selectpanel->SetSizer(selectbox);
	activepanel = selectpanel;
	paramset->panel = selectpanel;


	// Neuron selection

	datwidth = 50;
	labelwidth = 70;
	label = NumPanel(labelwidth, wxALIGN_CENTRE);
	spikes = NumPanel(datwidth, wxALIGN_RIGHT);
	mean = NumPanel(datwidth, wxALIGN_RIGHT);
	freq = NumPanel(datwidth, wxALIGN_RIGHT);
	sd = NumPanel(datwidth, wxALIGN_RIGHT);

	wxGridSizer *datagrid = new wxGridSizer(2, 5, 5);
	datagrid->Add(new wxStaticText(activepanel, -1, "Name"), 0, wxALIGN_CENTRE);
	datagrid->Add(label);
	datagrid->Add(new wxStaticText(activepanel, -1, "Spikes"), 0, wxALIGN_CENTRE);
	datagrid->Add(spikes);
	datagrid->Add(new wxStaticText(activepanel, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(freq);
	datagrid->Add(new wxStaticText(activepanel, -1, "Mean"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(mean);
	datagrid->Add(new wxStaticText(activepanel, -1, "Std Dev"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(sd);

	datneuron = new wxTextCtrl(activepanel, ID_Neuron, "---", wxDefaultPosition, wxSize(50, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE|wxTE_PROCESS_ENTER);
	datspin = new wxSpinButton(activepanel, wxID_ANY, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	wxBoxSizer *datbox = new wxBoxSizer(wxHORIZONTAL);
	datbox->Add(datspin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	datbox->AddSpacer(5);

	wxBoxSizer *neurobox = new wxBoxSizer(wxHORIZONTAL);
	neurobox->Add(new wxStaticText(activepanel, wxID_ANY, "Neuron"), 1, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	neurobox->Add(datneuron, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "");
	databox->AddSpacer(2);
	databox->Add(neurobox, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 5);
	databox->AddSpacer(5);
	databox->Add(datbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
	databox->AddSpacer(5);
	databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);


	// Spike selection

	paramset->AddNum("from", "From", 0, 0, 30); 
	paramset->AddNum("to", "To", 100, 0, 20); 

	wxBoxSizer *selectpanelbox = new wxBoxSizer(wxVERTICAL);
	selectbox1 = new wxStaticBoxSizer(wxHORIZONTAL, activepanel, "Selection 1");
	selectbox2 = new wxStaticBoxSizer(wxHORIZONTAL, activepanel, "Selection 2");
	wxBoxSizer *fromtobox = new wxBoxSizer(wxHORIZONTAL);

	fromtobox->Add(paramset->GetCon("from"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	fromtobox->Add(paramset->GetCon("to"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);

	int buttspace = 20;

	//AddButton(100, "Add", 40, selectbox1);
	addbutton[0] = ToggleButton(100, "Add", 40, selectbox1);	
	selectbox1->AddSpacer(buttspace);
	//AddButton(200, "Sub", 40, selectbox1);
	subbutton[0] = ToggleButton(200, "Sub", 40, selectbox1);	
	selectbox1->AddSpacer(buttspace);
	AddButton(300, "Clear", 40, selectbox1);
	selectbox1->AddSpacer(buttspace);
	AddButton(400, "Invert", 40, selectbox1);
	
	//AddButton(101, "Add", 40, selectbox2);
	addbutton[1] = ToggleButton(101, "Add", 40, selectbox2);	
	selectbox2->AddSpacer(buttspace);
	//AddButton(201, "Sub", 40, selectbox2);
	subbutton[1] = ToggleButton(201, "Sub", 40, selectbox2);	
	selectbox2->AddSpacer(buttspace);
	AddButton(301, "Clear", 40, selectbox2);
	selectbox2->AddSpacer(buttspace);
	AddButton(401, "Invert", 40, selectbox2);
	
	for(i=0; i<selectcount; i++) selectmode[i] = 1;
	currselect = 0;
	addbutton[currselect]->SetValue(true);

	selectpanelbox->Add(fromtobox, 0, wxALIGN_CENTRE_HORIZONTAL);
	selectpanelbox->AddSpacer(20);
	selectpanelbox->Add(selectbox1, 0);
	selectpanelbox->AddSpacer(10);
	selectpanelbox->Add(selectbox2, 0);

	wxBoxSizer *colbox2 = new wxBoxSizer(wxHORIZONTAL); 
	colbox2->AddStretchSpacer();
	colbox2->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	colbox2->AddSpacer(30);
	//colbox2->AddStretchSpacer();
	colbox2->Add(selectpanelbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	selectbox->AddStretchSpacer();
	selectbox->Add(colbox2, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	selectbox->AddStretchSpacer();
	selectpanel->Layout();

	// Data Loading 

	ToolPanel *loadpanel = new ToolPanel(this, tabpanel);
	loadpanel->SetFont(boxfont);
	wxBoxSizer *loadbox = new wxBoxSizer(wxVERTICAL);
	loadpanel->SetSizer(loadbox);
	activepanel = loadpanel;
	paramset->panel = loadpanel;


	wxBoxSizer *datatagbox = new wxBoxSizer(wxHORIZONTAL);
	neurodatatag = new TagBox(activepanel, ID_Select, "", wxDefaultPosition, wxSize(150, -1), "neurodatatag", mod->GetPath());
	datatagbox->Add(neurodatatag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	if(ostype == Mac) {
		AddButton(ID_Load, "Load", 60, datatagbox);
		AddButton(ID_FileBrowse, "Browse", 60, datatagbox);
	}
	else {
		AddButton(ID_Load, "Load", 40, datatagbox);
		datatagbox->AddSpacer(2);
		AddButton(ID_FileBrowse, "Browse", 50, datatagbox);
	}

	loadbox->AddStretchSpacer();
	loadbox->Add(datatagbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	loadbox->AddStretchSpacer();
	loadpanel->Layout();

	tabpanel->Freeze();
	tabpanel->AddPage(selectpanel, "Data Select" , true);
	tabpanel->AddPage(analysispanel, "Analysis" , false);
	tabpanel->AddPage(loadpanel, "Loading" , false);
	
	tabpanel->Thaw();

	winman->AddPane(tabpanel, wxAuiPaneInfo().Name("tabpane").CentrePane().PaneBorder(false));
	winman->Update();

	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CellBox::OnEnter));
	Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(CellBox::OnNext));
	Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(CellBox::OnPrev));
	Connect(ID_Load, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CellBox::OnLoadData));

	Connect(100, 105, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CellBox::OnAdd));
	Connect(200, 205, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CellBox::OnSub));
	Connect(300, 305, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CellBox::OnClear));
	Connect(400, 405, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CellBox::OnInvert));
}


CellBox::~CellBox()
{
	if(!spikeselectLink) {
		delete[] selectspikes[0];
		delete[] selectspikes[1];
	}
}


void CellBox::OnToggle(wxCommandEvent& event)
{
	int sel, type, i;
	wxString text;
	int id = event.GetId();

	type = id / 100;
	sel = id % 100;

	for(i=0; i<selectcount; i++) {
		addbutton[i]->SetValue(false);
		subbutton[i]->SetValue(false);
	}

	if(type == 1) addbutton[sel]->SetValue(true);
	if(type == 2) subbutton[sel]->SetValue(true);
	
	selectmode[sel] = type;	
	currselect = sel;

	//diagbox->Write(text.Format("\nSelect Button type %d  selection %d\n", type, sel));
}



void CellBox::OnClick(wxPoint pos)
{
	wxString text;
	int sel;
	
	wxRect selrect1 = wxRect(selectbox1->GetPosition(), selectbox1->GetSize());
	wxRect selrect2 = wxRect(selectbox2->GetPosition(), selectbox2->GetSize());

	if(selrect1.Contains(pos)) currselect = 0;
	if(selrect2.Contains(pos)) currselect = 1;

	currcell->burstdata->spikes = selectspikes[currselect];

	mainwin->SetStatusText(text.Format("Lysis Box Click x %d y %d  Select %d", pos.x, pos.y, currselect));
	mod->mainwin->scalebox->BurstDisp(1);
}


void CellBox::SetSelect(double from, double to)
{
	int id;

	id = paramset->ref["from"];
	paramset->con[id]->SetValue(from);
	id = paramset->ref["to"];
	paramset->con[id]->SetValue(to);

	if(selectmode[currselect] == 1) SelectAdd();
	if(selectmode[currselect] == 2) SelectSub();
}


void CellBox::SelectAdd()
{
	int sfrom, sto;
	wxString text;

	diagbox->Write(text.Format("Add %d\n", currselect));

	ParamStore *selectparams = GetParams();
	sfrom = (*selectparams)["from"] * 1000;         // Convert from s to ms
	sto = (*selectparams)["to"] * 1000;
	
	//selectdata[currselect]->times = currcell->times;
	//selectdata[currselect]->maxtime = currcell->times[numspikes-1];

	//diagbox->Write(text.Format("spike count %d\n", numspikes));

	for(i=0; i<currcell->spikecount; i++) {
		if(currcell->times[i] > sfrom && currcell->times[i] < sto) {
			//selectdata[currselect]->spikes[i] = currselect + 1;
			selectspikes[currselect][i] = currselect + 1;		
		}	
	}

	currcell->burstdata->spikes = selectspikes[currselect];

	//diagbox->Write(text.Format("add%d from %.2f to %.2f\n", currselect, sfrom, sto));	
	AnalyseSelection();	
	mainwin->scalebox->BurstDisp(1);
}


void CellBox::SelectSub()
{
	int sfrom, sto;

	ParamStore *selectparams = GetParams();
	sfrom = (int)(*selectparams)["from"] * 1000;
	sto = (int)(*selectparams)["to"] * 1000;

	int numspikes = currcell->spikecount;
	//selectdata[currselect]->times = currcell->times;
	//selectdata[currselect]->maxtime = currcell->times[numspikes-1];

	for(i=0; i<numspikes; i++) {
		if(currcell->times[i] > sfrom && currcell->times[i] < sto) {
			//selectdata[currselect]->spikes[i] = 0;
			selectspikes[currselect][i] = 0;
		}
	}

	currcell->burstdata->spikes = selectspikes[currselect];
	//diagbox->textbox->AppendText(text.Format("sub%d from %d to %d\n", currselect, sfrom, sto));

	mainwin->scalebox->BurstDisp(1);
	AnalyseSelection();
}


void CellBox::OnSub(wxCommandEvent& event)
{
	int i, id, sel;
	wxString text;

	id = event.GetId();
	sel = event.GetId() - 200;
	currselect = sel;
	
	SelectSub();
}


void CellBox::OnInvert(wxCommandEvent& event)
{
	int i;
	int sel = event.GetId() - 400;

	//for(i=0; i<numspikes; i++) selectdata[sel]->spikes[i] = (sel + 1) - selectdata[sel]->spikes[i];

	//sel = event.GetId() - 00;
	//currselect = sel;
	for(i=0; i<currcell->spikecount; i++) selectspikes[sel][i] = (sel + 1) - selectspikes[sel][i];

	mainwin->scalebox->BurstDisp(1);
	AnalyseSelection();
}


void CellBox::OnClear(wxCommandEvent& event)
{
	int i;
	int sel = event.GetId() - 300;

	//for(i=0; i<numspikes; i++) selectdata[sel]->spikes[i] = 0;
	//sel = event.GetId() - 300;
	//currselect = sel;
	for(i=0; i<currcell->spikecount; i++) selectspikes[sel][i] = 0;

	mainwin->scalebox->BurstDisp(1);
	AnalyseSelection();
}


void CellBox::OnAdd(wxCommandEvent& event)
{
	int i, sel, selspike;
	int numspikes, scount;
	double isi, timepoint;
	double intracount, intratime;
	wxString text;
	BurstDat *burstdata;
	datdouble isis;

	sel = event.GetId() - 100;
	currselect = sel;

	SelectAdd();
}


void CellBox::AnalyseSelection()
{
	int i, selspike;
	int numspikes, scount;
	int sfrom, sto;
	double isi, timepoint;
	double intracount, intratime;
	wxString text;
	//BurstDat *burstdata;
	
	currcell->burstdata->IntraBurstAnalysis();
	//mod->SelectBurst(selectdata[currselect]);

	//for(i=0; i<1000; i++) currcell->burstdata->hist5[i] = 20;

	diagbox->Write(text.Format("\nSelect analyse %d spikes %.2fHz test %d\n", currcell->burstdata->intraspikes, currcell->burstdata->freq, currcell->burstdata->test));

	//if(currselect == 0) mod->burstbox->BurstDataDisp(mod->spikedata, mod->burstbox->modburst);
	//if(currselect == 1) mod->burstbox->BurstDataDisp(mod->spikedata, mod->burstbox->evoburst);
}


void CellBox::OnLoadData(wxCommandEvent& event)
{
	wxString filetag, filepath, text;
	short tagpos;
	FileDat *file;

	filetag = neurodatatag->GetValue();
	//filepath = paramset->GetCon("datapath")->GetString();
	filepath = "C:/Data/VMN";
	FileDat newfile = FileDat(filetag, filepath);

	tagpos = neurodatatag->FindString(filetag);

	file = mainwin->filebase->Add(FileDat(filetag, filepath));

	if(tagpos != wxNOT_FOUND) neurodatatag->Delete(tagpos);

	//file = new FileDat(filetag, filepath);
	//mainwin->diagbox->Write(text.Format("Entry name %s path %s\n", filetag, filepath));
	//file = new FileDat(filetag, filepath);
	//mainwin->diagbox->Write(text.Format("FileDat name %s path %s\n", file->name, file->path));

	neurodatatag->Insert(filetag, 0, file);
	neurodatatag->SetValue(filetag);

	//if(mainwin->diagbox) mainwin->diagbox->Write("filetag " + filetag);
	//if(mainwin->diagbox) mainwin->diagbox->Write("\nOn Load\n");
	//LoadData(file);
	LoadDataList(file);

	//mod->filebase->newentry = false;
}


void CellBox::PanelData(NeuroDat *data)
{
	if(data->netflag) snum = "sum";
	else snum = numstring(neuroindex, 0);
	datneuron->SetLabel(snum);

	label->SetLabel(data->name);
	spikes->SetLabel(snum.Format("%d", data->spikecount));
	freq->SetLabel(snum.Format("%.2f", data->freq));
	mean->SetLabel(snum.Format("%.1f", data->meanisi));
	sd->SetLabel(snum.Format("%.2f", data->isivar));
}


void CellBox::NeuroData()
{	
	ParamStore *calcparams = GetParams();

	currcell->normscale = (*calcparams)["normscale"];
	//currcell->neurocalc(&((*cells)[neuroindex]));
	currcell->neurocalc(&(*cells)[neuroindex]);
	currcell->id = neuroindex;
	
	//PanelData(&((*cells)[neuroindex]));
	PanelData(&(*cells)[neuroindex]);
	mainwin->scalebox->GraphUpdate();	
}


void CellBox::OnPrev(wxSpinEvent& WXUNUSED(event))
{
	if(!cellcount) return;
	if(neuroindex > 0) neuroindex--;
	else neuroindex = cellcount-1;

	NeuroData();
}


void CellBox::OnNext(wxSpinEvent& WXUNUSED(event))
{
	if(!cellcount) return;
	if(neuroindex < cellcount-1) neuroindex++;
	else neuroindex = 0;

	NeuroData();
}


void CellBox::OnEnter(wxCommandEvent& event)
{
	int id = event.GetId();
	long data;

	// Enter pressed for neuron selection
	if(id == ID_Neuron) {
		datneuron->GetValue().ToLong(&data);
		if(data >= 0 && data < cellcount) {
			neuroindex = data;
			NeuroData();
		}
		return;
	}
	else NeuroData();
}


void CellBox::LoadDataList(FileDat *file)
{
	int i, row, col;
	int start, filecount;
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
	filecount = 0;
	filename = file->String().ToStdString();

	ifstream infile(filename.c_str());
	if(infile.is_open()) {
		while(getline(infile, line)) {
			wxString readline(line);
			mainwin->diagbox->Write(readline + "\n");
			LoadNeuroData(FileDat(readline, file->path), filecount++);
			//textgrid->ParseLine(row++, col, line);
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


void CellBox::LoadNeuroData(FileDat file, int col)
{
	int i, row;
	int start;
	wxString filepath;
	wxString datstring, readline, text;
	string line, filename;
	long timeval;
	double dataval;

	if(file.path == "") {
		mainwin->diagbox->Write("No file path\n");
		return;
	}
	if(file.name == "") {
		mainwin->diagbox->Write("No file name\n");
		return;
	}

	mainwin->diagbox->Write(text.Format("FileDat path %s name %s\n", file.path, file.name));

	row = 0;
	//col = 0;
	filename = file.String().ToStdString();

	ifstream infile(filename.c_str());

	if(!infile.is_open()) {
		filename = filename + ".txt";
		infile.open(filename.c_str());
	}

	if(infile.is_open()) {
		while(getline(infile, line)) {
			//wxString readline(line);
			//mainwin->diagbox->Write(readline + "\n");
			//LoadNeuroData(FileDat(readline, file->path));
			if(line == ":") {
				mainwin->diagbox->Write("Dumping :\n");
				continue;
			}
			if(line.front() == '\'') {
				mainwin->diagbox->Write("Label detect\n");
				textgrid->ParseLabel(row++, col, line);
				continue;
			}
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


OutBox::OutBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, int rows, int cols, bool bmode)
	: ParamBox(model, title, pos, size, "outbox")
{
	int gridrows, gridcols;
	wxBoxSizer *vdubox;
	mod = model;

	redtag = "";
	gridrows = rows;
	gridcols = cols;
	bookmode = bmode;
	vdumode = bookmode;
	delete parambox;

	//InitMenu();
	//SetModFlag(ID_FileIO, "ioflag", "IO Mode", 0); 

	diagbox = mod->diagbox;

	if(bookmode) notebook = new wxNotebook(panel, -1, wxPoint(-1,-1), wxSize(-1, 400), wxNB_TOP);

	//textgrid = new TextGrid(panel, wxSize(gridrows, gridcols));
	
	if(bookmode) {
		textgrid = new TextGrid(notebook, wxSize(gridrows, gridcols));
		notebook->AddPage(textgrid, text.Format("Data %d", 0));
	}
	else textgrid = new TextGrid(panel, wxSize(gridrows, gridcols));

	//for(i=0; i<gridrows; i++) textgrid->SetRowSize(i, 25);
	//for(i=0; i<gridcols; i++) textgrid->SetColSize(i, 60);
	textgrid->SetDefaultRowSize(20, true);
	textgrid->SetDefaultColSize(60, true);
	//textgrid->SetRowLabelSize(80); 
	textgrid->SetRowLabelSize(50); 
	textgrid->vdu = NULL;
	textgrid->gauge = NULL;
	gauge = NULL;

	wxBoxSizer *controlbox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *storebox = StoreBox();

	buttonbox = new wxBoxSizer(wxHORIZONTAL);
	buttonbox->Add(storebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	AddButton(ID_Undo, "Undo", 40, buttonbox);
	buttonbox->AddSpacer(2);
	AddButton(ID_Copy, "Copy", 40, buttonbox);

	if(vdumode) {
		vdu = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1), wxBORDER_RAISED|wxTE_MULTILINE);
		vdu->SetFont(confont);
		vdu->SetForegroundColour(wxColour(0,255,0)); // set text color
		vdu->SetBackgroundColour(wxColour(0,0,0)); // set text back color
	
		//wxBoxSizer *statusbox = new wxBoxSizer(wxHORIZONTAL);
		//statusbox->Add(status, 1, wxEXPAND);

		gauge = new wxGauge(panel, wxID_ANY, 10);
		vdubox = new wxBoxSizer(wxVERTICAL);
		//displaybox->Add(vdu, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
		vdubox->Add(vdu, 1, wxEXPAND);
		//displaybox->Add(gauge, 0, wxEXPAND);
	}

	wxBoxSizer *leftbox = new wxBoxSizer(wxVERTICAL);
	leftbox->Add(buttonbox, 0);  
	if(vdumode) leftbox->Add(gauge, 0, wxEXPAND);

	//controlbox->Add(storebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->AddSpacer(10);
	//controlbox->Add(buttonbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->Add(leftbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->AddSpacer(10);
	//controlbox->Add(vdu, 100, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//controlbox->Add(displaybox, 100, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	if(vdumode) controlbox->Add(vdubox, 100, wxEXPAND);
	controlbox->AddSpacer(10);

	if(bookmode) mainbox->Add(notebook, 1, wxEXPAND);
	else mainbox->Add(textgrid, 1, wxEXPAND);
	mainbox->Add(controlbox, 0);
	mainbox->AddSpacer(2);

	//GridDefault();

	//histmode = 1;
	//HistLoad();
	//paramstoretag->SetLabel(initparams);

	panel->Layout();

	textgrid->vdu = vdu;
	textgrid->gauge = gauge;
	textgrid->outbox = this;

	Connect(ID_paramstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OutBox::OnGridStore));
	Connect(ID_paramload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OutBox::OnGridLoad));
	Connect(ID_Undo, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OutBox::OnUndo));
	Connect(ID_Copy, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OutBox::OnCopy));
	Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(OutBox::OnRightClick));
};


void OutBox::OnUndo(wxCommandEvent& event)
{
	textgrid->Undo();
}


void OutBox::OnButton(wxCommandEvent& event)
{
	wxString text;

	mainwin->diagbox->Write("Button: Neuro data scan\n");
}


void OutBox::OnCopy(wxCommandEvent& event)
{
	textgrid->Copy();
}


void OutBox::OnRightClick(wxMouseEvent& event)
{
	int i;
	//int id = event.GetId();
	//wxWindow *pos = FindWindowById(id, toolpanel);
	//wxPoint point = this->GetPosition();
	textgrid->SetCellValue(10, 0, "right");

	wxPoint pos = event.GetPosition();
	//wxSize size = this->GetSize();
	//menuPlot->Check(1000 + gpos->GetFront()->gindex, true);
	//PopupMenu(rightmenu, pos.x + 20, pos.y);
}


void OutBox::HistLoad()
{
	wxString filename, readline;
	TextFile infile;

	// Output Grid History
	filename =  "outgridhist.ini";
	if(infile.Open(filename)) {
		readline = infile.ReadLine();
		//tofp.WriteLine(readline);
		while(!readline.IsEmpty()) {
			readline = readline.AfterFirst(' ');
			readline.Trim();
			initparams = readline;
			paramstoretag->Insert(initparams, 0);
			readline = infile.ReadLine();
			//tofp.WriteLine(readline);
		}
		infile.Close();	
	}
	paramstoretag->SetValue(initparams);
}



void OutBox::TestGrid()
{
	int i;
	wxString text;

	for(i=0; i<1000; i++) textgrid->SetCellValue(i, 0, text.Format("%.1f", i*0.1 + 10));
}


void OutBox::GridDefault()
{
	textgrid->SetCellValue(0, 0, "date");
	textgrid->SetCellValue(1, 0, "breath rhy");
	textgrid->SetCellValue(2, 0, "odour");
	textgrid->SetCellValue(3, 0, "bedding");
	textgrid->SetCellValue(4, 0, "val");
	textgrid->SetCellValue(5, 0, "hex");
	textgrid->SetCellValue(6, 0, "air");
	textgrid->SetCellValue(7, 0, "other");

	textgrid->SetCellValue(9, 0, "phasic");
	textgrid->SetCellValue(10, 0, "other");
	textgrid->SetCellValue(11, 0, "vas");

	textgrid->SetCellValue(13, 0, "data");
	textgrid->SetCellValue(14, 0, "spikes");
	textgrid->SetCellValue(15, 0, "freq");
	textgrid->SetCellValue(16, 0, "mean isi");
	textgrid->SetCellValue(17, 0, "isi SD");
}


void OutBox::OnGridStore(wxCommandEvent& event)
{
	GridStore();
}


void OutBox::OnGridLoad(wxCommandEvent& event)
{
	textgrid->CopyUndo();
	//GridLoad();
	//int ioflag = (*modflags)["ioflag"];
	int ioflag = true;
	if(ioflag) GridLoadFast();
	else GridLoad();

	//textgrid->AutoSizeColumns(false);
}


void OutBox::ColumnSelect(int col)
{
	wxString text;

	WriteVDU(text.Format("Column Select %d\n", col));
}


int OutBox::ColumnData(int col, datdouble *data)
{
	int row, count;
	double value;
	wxString celltext;

	count = 0;
	for(row=0; row<textgrid->GetNumberRows(); row++) {
		celltext = textgrid->GetCellValue(row, col);
		if(celltext != "") {
			celltext.ToDouble(&value);
			count++;
		}
		else value = 0;
		(*data)[row] = value;
	}
	return count;
}


void OutBox::GridStore()
{
	TextFile ofp;
	int row, col;
	wxString celltext, text, filename, filetag, filepath;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	Index columnindex;

	/*
	filetag = gstag->GetValue();
	filename = "graph-" + filetag + ".dat";

	// Tag history
	short tagpos = gstag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) gstag->Delete(tagpos);
	gstag->Insert(filetag, 0);

	// Check and warn existing file
	wxTextFile checkfile(filepath + "/" + filename);*/

	filepath = mod->GetPath() + "/Grids";
	if(!wxDirExists(filepath)) wxMkdir(filepath);

	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	if(ofp.Exists(filename) && redtag != filetag) {
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

	//ofp.New(filename);
	sfilename = filename.ToStdString();
	ofstream outfile(sfilename.c_str());

	if(!outfile.is_open()) {
		paramstoretag->SetValue("File error");
		return;
	}

	WriteVDU("Writing file...");

	for(row=0; row<textgrid->GetNumberRows(); row++) {
		if(gauge) gauge->SetValue(100 * (row + 1) / textgrid->GetNumberRows());
		for(col=0; col<textgrid->GetNumberCols(); col++) {
			celltext = textgrid->GetCellValue(row, col);
			celltext.Trim();                                                                     // Fixes odd line endings in pasted data  23/4/15
			//if(!celltext.IsEmpty()) ofp.WriteLine(text.Format("%d %d %s", row, col, celltext));
			if(!celltext.IsEmpty()) {
				text.Printf("%d %d %s\n", row, col, celltext);
				columnindex.Add(col);
				outfile << text.ToStdString();
			}
		}
	}

	outfile.close();
	if(gauge) gauge->SetValue(0);
	WriteVDU("OK\n");

	ofp.New(filetag + "-gridsize.txt");
	for(i=0; i<columnindex.count; i++) {
		col = columnindex.list[i];
		ofp.WriteLine(text.Format("col %d %d", col, textgrid->GetColSize(col)));
	}
	ofp.Close();
}


void OutBox::GridLoad()            // Replaced by GridLoadFast()
{
	TextFile ifp;
	int row, col;
	long numdat;
	wxString text, filetag, filename, filepath, cell;
	wxString datstring;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	int numlines, linecount, cellcount;

	filepath = mod->GetPath() + "/Grids";
	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	/*
	filename = filetag + "-grid.txt";
	if(!ifp.Open(filename)) {
	paramstoretag->SetValue("Not found");
	return;
	}*/

	sfilename = filename.ToStdString();
	ifstream infile(sfilename.c_str());
	if(!infile.is_open()) {
		paramstoretag->SetValue("Not found");
		return;
	}

	// Param file history
	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	textgrid->ClearGrid();

	WriteVDU("Reading file...");


	numlines = count(istreambuf_iterator<char>(infile), istreambuf_iterator<char>(), '\n');
	infile.clear();
	infile.seekg(0, ios::beg);
	linecount = 0;
	cellcount = 0;

	//readline = ifp.ReadLine();
	while(getline(infile, line)) {
		wxString readline(line);
		//diagbox->Write(readline + "\n");
		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		row = numdat;
		readline = readline.AfterFirst(' ');

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		col = numdat;
		readline = readline.AfterFirst(' ');

		readline.Trim();
		cell = readline;
		textgrid->SetCell(row, col, cell);
		cellcount++;
		//diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
		//readline = ifp.ReadLine();
		//diagbox->Write("Read " + readline + "\n");
		linecount++;
		if(gauge) gauge->SetValue(100 * linecount / numlines);
	}
	infile.close();
	diagbox->Write("OK\n");
	//WriteVDU("OK\n");
	WriteVDU(text.Format("OK, %d cells\n", cellcount));
	if(gauge) gauge->SetValue(0);
}


void OutBox::GridLoadFast()
{
	TextFile ifp;
	int row, col, width;
	long numdat;
	wxString text, filetag, filepath, filename, cell;
	wxString datstring, readline;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	int numlines, linecount, cellcount;


	filepath = mod->GetPath() + "/Grids";
	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	if(!ifp.Exists(filename)) {
		filename = filetag + "-grid.txt";            // Backwards compatibility, try old grid file location
		if(!ifp.Exists(filename)) {
			paramstoretag->SetValue("Not found");
			return;
		}
	}
	sfilename = filename.ToStdString();
	ifstream readfile(sfilename.c_str());

	// Param file history
	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	textgrid->ClearGrid();

	WriteVDU("Reading file...");

	numlines = count(istreambuf_iterator<char>(readfile), istreambuf_iterator<char>(), '\n');
	if(!numlines) {
		WriteVDU("File empty\n");
		return;
	}
	readfile.clear();
	//readfile.seekg(0, ios::beg);
	linecount = 0;

	//string filetext = (ReadFile(filename.c_str()));
	//istringstream filetext(ReadFile(filename.c_str()));

	//ifstream infile(filename, std::ios::in | std::ios::binary);

	/*
	string contents;
	readfile.seekg(0, ios::end);
	contents.resize(readfile.tellg());
	readfile.seekg(0, ios::beg);
	readfile.read(&contents[0], contents.size());
	readfile.close();
	istringstream infile(contents);
	*/

	string contents;
	readfile.seekg(0, readfile.end);
	contents.resize(readfile.tellg());
	readfile.seekg(0, readfile.beg);
	readfile.read(&contents[0], contents.size());
	readfile.close();
	istringstream infile(contents);

	/*
	diagbox->Write("Contents codes:\n");
	for(i=0; i<contents.length(); i++) {
	diagbox->Write(text.Format("%d ", contents[i]));
	if(contents[i] == '\n') diagbox->Write("\n");
	}
	diagbox->Write("EOF Contents\n");

	diagbox->Write(text.Format("File length %d size %d numlines %d :\n%s\n", (int)contents.length(), (int)contents.size(), numlines, contents));
	*/

	cellcount = 0;

	//readline = ifp.ReadLine();
	while(getline(infile, line)) {
		//diagbox->Write(text.Format(" line length %d first %d\n", (int)line.length(), (char)line[0]));
		wxString readline(line);
		/*
		diagbox->Write("Line codes:");
		for(i=0; i<readline.Len(); i++) diagbox->Write(text.Format("%d ", readline[i]));
		diagbox->Write("\n");
		*/
		//readline.Trim(false);
		//readline.Trim();
		//diagbox->Write("readline " + readline);
		if(readline.IsEmpty() || !readline[0]) break;
		//else diagbox->Write(text.Format(" length %d ", (int)readline.Len()));
		//diagbox->Write(readline + "\n");
		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		row = numdat;
		readline = readline.AfterFirst(' ');

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		col = numdat;
		readline = readline.AfterFirst(' ');

		readline.Trim();
		cell = readline;
		textgrid->SetCell(row, col, cell);
		//diagbox->Write(text.Format(" setcell %d %d %s\n", row, col, cell));
		cellcount++;
		//diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
		//readline = ifp.ReadLine();
		//diagbox->Write("Read " + readline + "\n");
		linecount++;
		if(gauge) gauge->SetValue(100 * linecount / numlines);
	}
	//infile.close();
	//diagbox->Write("OK\n");
	//WriteVDU(text.Format("OK, %d grid cells\n", cellcount));
	WriteVDU("OK\n");
	if(gauge) gauge->SetValue(0);

	if(!ifp.Open(filetag + "-gridsize.txt")) return;

	readline = ifp.ReadLine();
	while(!readline.IsEmpty()) {
		col = ParseLong(&readline, 'l');
		width = ParseLong(&readline, 0);
		//WriteVDU(text.Format("col %d %d\n", col, width));
		textgrid->SetColSize(col, width);
		if(ifp.End()) break;
		readline = ifp.ReadLine();
	}

	ifp.Close();

}



/*CellBox::CellBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size)
: ParamBox(mod, title, pos, size, "cellbox")
{
int datwidth;

diagbox = mod->diagbox;
cellcount = 0;
paramindex = 0;


notebook = new wxNotebook(panel, -1, wxPoint(-1,-1), wxSize(-1, 400), wxNB_TOP);


ToolPanel *analysispanel = new ToolPanel(mainwin, notebook);
analysispanel->SetFont(boxfont);
wxBoxSizer *analysisbox = new wxBoxSizer(wxVERTICAL);
analysispanel->SetSizer(analysisbox);
activepanel = analysispanel;
paramset->panel = analysispanel;


notebook->AddPage(analysispanel, "Analysis");

//chromesizer->AddStretchSpacer();
//chromesizer->Add(chromestatbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
//chromesizer->AddStretchSpacer();


wxBoxSizer *histparambox = new wxBoxSizer(wxVERTICAL);
paramset->AddNum("normscale", "Norm Scale", 10000, 0, 70, 50);
paramset->AddNum("histrange", "Hist Range", 1000, 0, 70, 50);
//paramset->AddNum("binsize", "Bin Size", 5, 0, 70, 50);
PanelParamLayout(histparambox);


datwidth = 50;
spikes = NumPanel(datwidth, wxALIGN_RIGHT);
mean = NumPanel(datwidth, wxALIGN_RIGHT);
freq = NumPanel(datwidth, wxALIGN_RIGHT);
sd = NumPanel(datwidth, wxALIGN_RIGHT);

wxGridSizer *datagrid = new wxGridSizer(2, 5, 5);
datagrid->Add(new wxStaticText(activepanel, -1, "Spikes"), 0, wxALIGN_CENTRE);
datagrid->Add(spikes);
datagrid->Add(new wxStaticText(activepanel, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(freq);
datagrid->Add(new wxStaticText(activepanel, -1, "Mean"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(mean);
datagrid->Add(new wxStaticText(activepanel, -1, "Std Dev"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(sd);

datneuron = new wxTextCtrl(activepanel, ID_Neuron, "---", wxDefaultPosition, wxSize(50, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE|wxTE_PROCESS_ENTER);
datspin = new wxSpinButton(activepanel, wxID_ANY, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
wxBoxSizer *datbox = new wxBoxSizer(wxHORIZONTAL);
datbox->Add(datspin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
datbox->AddSpacer(5);

wxBoxSizer *neurobox = new wxBoxSizer(wxHORIZONTAL);
neurobox->Add(new wxStaticText(activepanel, wxID_ANY, "Neuron"), 1, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
neurobox->Add(datneuron, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "");
databox->AddSpacer(2);
databox->Add(neurobox, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 5);
databox->AddSpacer(5);
databox->Add(datbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
databox->AddSpacer(5);
databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

wxBoxSizer *colbox = new wxBoxSizer(wxHORIZONTAL); 
//colbox->AddStretchSpacer();
colbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
colbox->AddSpacer(10);
colbox->Add(histparambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
//colbox->AddStretchSpacer();



analysisbox->AddStretchSpacer();
analysisbox->Add(colbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
analysisbox->AddStretchSpacer();

analysispanel->Layout();

mainbox->Add(notebook, 1, wxEXPAND);

//panel->Layout();


Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CellBox::OnEnter));
Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(CellBox::OnNext));
Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(CellBox::OnPrev));
}*/