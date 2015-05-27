

#include "wx/wx.h"
#include <hypomodel.h>
#include "wx/grid.h"


OutBox::OutBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size, int rows, int cols)
: ParamBox(mod, title, pos, size, "outbox")
{
	int gridrows, gridcols;

	redtag = "";
	gridrows = rows;
	gridcols = cols;
	delete parambox;

	diagbox = mod->diagbox;

	textgrid = new TextGrid(panel, wxSize(gridrows, gridcols));
	for(i=0; i<gridrows; i++) textgrid->SetRowSize(i, 25);

	wxBoxSizer *storebox = StoreBox();
	AddButton(ID_Undo, "Undo", 50, storebox);
	storebox->AddSpacer(2);
	AddButton(ID_Copy, "Copy", 50, storebox);

	mainbox->Add(textgrid, 1, wxEXPAND);
	mainbox->Add(storebox, 0);

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
	GridLoad();
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

	ofp.New(filename);

	for(row=0; row<textgrid->GetNumberRows(); row++)
		for(col=0; col<textgrid->GetNumberCols(); col++) {
			celltext = textgrid->GetCellValue(row, col);
			celltext.Trim();                                                                     // Fixes odd line endings in pasted data  23/4/15
			if(!celltext.IsEmpty()) ofp.WriteLine(text.Format("%d %d %s", row, col, celltext));
		}

	ofp.Close();
}


void OutBox::GridLoad()
{
	TextFile ifp;

	int row, col;
	long numdat;
	wxString text, filename, filetag, cell;
	wxString readline, datstring;
	wxColour redpen("#dd0000"), blackpen("#000000");

	filetag = paramstoretag->GetValue();
	filename = filetag + "-grid.txt";
	if(!ifp.Open(filename)) {
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

	readline = ifp.ReadLine();
	while(!readline.IsEmpty()) {
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
		textgrid->SetCellValue(row, col, cell);
		//diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
		readline = ifp.ReadLine();
		//diagbox->Write("Read " + readline + "\n");
	}
	ifp.Close();
}

