

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
: ParamBox(mod, title, pos, size, "cellbox")
{
	int datwidth;

	diagbox = mod->diagbox;
	cellcount = 0;

	paramset->AddNum("normscale", "Norm Scale", 10000, 0, 70, 50);
	ParamLayout(1);

	datwidth = 50;
	spikes = NumPanel(datwidth, wxALIGN_RIGHT);
	mean = NumPanel(datwidth, wxALIGN_RIGHT);
	freq = NumPanel(datwidth, wxALIGN_RIGHT);
	sd = NumPanel(datwidth, wxALIGN_RIGHT);

	wxGridSizer *datagrid = new wxGridSizer(2, 5, 5);
	datagrid->Add(new wxStaticText(panel, -1, "Spikes"), 0, wxALIGN_CENTRE);
	datagrid->Add(spikes);
	datagrid->Add(new wxStaticText(panel, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(freq);
	datagrid->Add(new wxStaticText(panel, -1, "Mean"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(mean);
	datagrid->Add(new wxStaticText(panel, -1, "Std Dev"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(sd);
	
	datneuron = new wxTextCtrl(panel, ID_Neuron, "---", wxDefaultPosition, wxSize(50, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE|wxTE_PROCESS_ENTER);
	datspin = new wxSpinButton(panel, wxID_ANY, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	wxBoxSizer *datbox = new wxBoxSizer(wxHORIZONTAL);
	datbox->Add(datspin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	datbox->AddSpacer(5);
	
	wxBoxSizer *neurobox = new wxBoxSizer(wxHORIZONTAL);
	neurobox->Add(new wxStaticText(panel, wxID_ANY, "Neuron"), 1, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	neurobox->Add(datneuron, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, panel, "");
	databox->AddSpacer(2);
	databox->Add(neurobox, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 5);
	databox->AddSpacer(5);
	databox->Add(datbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
	databox->AddSpacer(5);
	databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	mainbox->AddStretchSpacer();
	mainbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	mainbox->AddStretchSpacer();
	mainbox->Add(parambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	mainbox->AddStretchSpacer();

	panel->Layout();

	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CellBox::OnEnter));
	Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(CellBox::OnNext));
	Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(CellBox::OnPrev));
}


void CellBox::PanelData(NeuroDat *data)
{
	if(data->netflag) snum = "sum";
	else snum = numstring(neuroindex, 0);
	datneuron->SetLabel(snum);
	spikes->SetLabel(snum.Format("%d", data->spikecount));
	freq->SetLabel(snum.Format("%.2f", data->freq));
	mean->SetLabel(snum.Format("%.1f", data->meanisi));
	sd->SetLabel(snum.Format("%.2f", data->isivar));
}


void CellBox::NeuroData()
{	
	ParamStore *calcparams = GetParams();
	currcell->normscale = (*calcparams)["normscale"];

	currcell->neurocalc(&((*cells)[neuroindex]));
	currcell->id = neuroindex;
	PanelData(&((*cells)[neuroindex]));
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


OutBox::OutBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size, int rows, int cols)
: ParamBox(mod, title, pos, size, "outbox")
{
	int gridrows, gridcols;

	redtag = "";
	gridrows = rows;
	gridcols = cols;
	delete parambox;

	//InitMenu();
	//SetModFlag(ID_FileIO, "ioflag", "IO Mode", 0); 

	diagbox = mod->diagbox;

	notebook = new wxNotebook(panel, -1, wxPoint(-1,-1), wxSize(-1, 400), wxNB_TOP);

	//textgrid = new TextGrid(panel, wxSize(gridrows, gridcols));
	textgrid = new TextGrid(notebook, wxSize(gridrows, gridcols));
	notebook->AddPage(textgrid, text.Format("Data %d", 0));
	//for(i=0; i<gridrows; i++) textgrid->SetRowSize(i, 25);
	//for(i=0; i<gridcols; i++) textgrid->SetColSize(i, 60);
	textgrid->SetDefaultRowSize(20, true);
	textgrid->SetDefaultColSize(60, true);
	

	wxBoxSizer *controlbox = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *storebox = StoreBox();

	buttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Undo, "Undo", 40, buttonbox);
	buttonbox->AddSpacer(2);
	AddButton(ID_Copy, "Copy", 40, buttonbox);

	vdu = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1), wxBORDER_RAISED|wxTE_MULTILINE);
	vdu->SetFont(confont);
	vdu->SetForegroundColour(wxColour(0,255,0)); // set text color
	vdu->SetBackgroundColour(wxColour(0,0,0)); // set text back color
	//wxBoxSizer *statusbox = new wxBoxSizer(wxHORIZONTAL);
	//statusbox->Add(status, 1, wxEXPAND);

	gauge = new wxGauge(panel, wxID_ANY, 100);
	wxBoxSizer *displaybox = new wxBoxSizer(wxVERTICAL);
	//displaybox->Add(vdu, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	displaybox->Add(vdu, 1, wxEXPAND);
	displaybox->Add(gauge, 0, wxEXPAND);

	controlbox->Add(storebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->AddSpacer(10);
	controlbox->Add(buttonbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->AddSpacer(10);
	//controlbox->Add(vdu, 100, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//controlbox->Add(displaybox, 100, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->Add(displaybox, 100, wxEXPAND);
	controlbox->AddSpacer(10);

	//mainbox->Add(textgrid, 1, wxEXPAND);
	mainbox->Add(notebook, 1, wxEXPAND);
	mainbox->Add(controlbox, 0);

	//GridDefault();

	//histmode = 1;
	//HistLoad();
	//paramstoretag->SetLabel(initparams);

	panel->Layout();

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
	wxString celltext, text, filename, filetag;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;

	filetag = paramstoretag->GetValue();
	filename = filetag + "-grid.txt";

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
	sfilename = filetag.ToStdString() + "-grid.txt";
	ofstream outfile(sfilename.c_str());

	if(!outfile.is_open()) {
		paramstoretag->SetValue("File error");
		return;
	}

	WriteVDU("Writing file...");

	for(row=0; row<textgrid->GetNumberRows(); row++) {
		gauge->SetValue(100 * (row + 1) / textgrid->GetNumberRows());
		for(col=0; col<textgrid->GetNumberCols(); col++) {
			celltext = textgrid->GetCellValue(row, col);
			celltext.Trim();                                                                     // Fixes odd line endings in pasted data  23/4/15
			//if(!celltext.IsEmpty()) ofp.WriteLine(text.Format("%d %d %s", row, col, celltext));
			if(!celltext.IsEmpty()) {
				text.Printf("%d %d %s\n", row, col, celltext);
				outfile << text.ToStdString();
			}
		}
	}

	gauge->SetValue(0);
	WriteVDU("OK\n");

	//ofp.Close();
	outfile.close();
}


void OutBox::GridLoad()
{
	TextFile ifp;
	int row, col;
	long numdat;
	wxString text, filetag, cell;
	wxString datstring;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, filename;
	int numlines, linecount, cellcount;

	filetag = paramstoretag->GetValue();
	/*
	filename = filetag + "-grid.txt";
	if(!ifp.Open(filename)) {
		paramstoretag->SetValue("Not found");
		return;
	}*/

	filename = filetag.ToStdString() + "-grid.txt";
	ifstream infile(filename.c_str());
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
		gauge->SetValue(100 * linecount / numlines);
	}
	infile.close();
	diagbox->Write("OK\n");
	//WriteVDU("OK\n");
	WriteVDU(text.Format("OK, %d cells\n", cellcount));
	gauge->SetValue(0);
}



void OutBox::GridLoadFast()
{
	TextFile ifp;
	int row, col;
	long numdat;
	wxString text, filetag, cell;
	wxString datstring;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, filename;
	int numlines, linecount, cellcount;

	filetag = paramstoretag->GetValue();
	

	filename = filetag.ToStdString() + "-grid.txt";
	ifstream readfile(filename.c_str());
	if(!readfile.is_open()) {
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

  numlines = count(istreambuf_iterator<char>(readfile), istreambuf_iterator<char>(), '\n');
	readfile.clear();
	readfile.seekg(0, ios::beg);
	linecount = 0;

	//string filetext = (ReadFile(filename.c_str()));
	//istringstream filetext(ReadFile(filename.c_str()));

	//ifstream infile(filename, std::ios::in | std::ios::binary);
  
	
		string contents;
		readfile.seekg(0, ios::end);
		contents.resize(readfile.tellg());
		readfile.seekg(0, ios::beg);
		readfile.read(&contents[0], contents.size());
		readfile.close();
		istringstream infile(contents);
	
	cellcount = 0;

	//readline = ifp.ReadLine();
	while(getline(infile, line)) {
		wxString readline(line);
		if(readline.IsEmpty()) break;
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
		//diagbox->Write(text.Format("setcell %d %d %s\n", row, col, cell));
		cellcount++;
		//diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
		//readline = ifp.ReadLine();
		//diagbox->Write("Read " + readline + "\n");
		linecount++;
		gauge->SetValue(100 * linecount / numlines);
	}
	//infile.close();
	diagbox->Write("OK\n");
	//WriteVDU(text.Format("OK, %d cells\n", cellcount));
	WriteVDU("OK\n");
	gauge->SetValue(0);
}
