


#include "wx/wx.h"
#include <hypotools.h>
#include "hypodef.h"



DiagBox::DiagBox(MainFrame *mainwin, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ToolBox(mainwin, title, pos, size)
{
	textbox = new wxTextCtrl(panel, -1, "", wxDefaultPosition, wxSize(300, 400), wxTE_MULTILINE);

	//Layout();
	mainbox->Add(textbox, 1, wxEXPAND);
	panel->Layout();
	//Centre();
};


void DiagBox::Write(wxString text)
{
	textbox->AppendText(text);	
}


TextGrid::TextGrid(wxWindow *parent, wxSize size)
	: wxGrid(parent, wxID_ANY)
{
	//wxSize size(30, 30);
	ostype = GetSystem();

	CreateGrid(size.x, size.y);
	SetRowLabelSize(35);
	SetColLabelSize(25);
	SetRowLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
	SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE); 	
	SetLabelFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	undogrid = new wxGridStringTable(size.x, size.y);

	rightmenu = new wxMenu;
	rightmenu->Append(ID_SelectAll, "Select All", "Grid Select", wxITEM_NORMAL);
	rightmenu->Append(ID_Copy, "Copy", "Copy Selection", wxITEM_NORMAL);
	rightmenu->Append(ID_Paste, "Paste", "Paste Clipboard", wxITEM_NORMAL);
	rightmenu->Append(ID_Undo, "Undo", "Undo", wxITEM_NORMAL);
	//rightmenu->Append(ID_Bold, "Bold", "Set Bold", wxITEM_NORMAL);
	//for(i=0; i<mod->graphbase->numgraphs; i++) menuPlot->AppendRadioItem(1000 + i, (*mod->graphbase)[i]->gname);

	//Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(TextGrid::OnRightClick));
	Connect(wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler(TextGrid::OnRightClick));
	Connect(wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler(TextGrid::OnLeftClick));
	Connect(ID_SelectAll, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnSelectAll));
	Connect(ID_Copy, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnCopy));
	Connect(ID_Paste, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnPaste));
	Connect(ID_Undo, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnUndo));
	Connect(ID_Bold, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnBold));
	Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(TextGrid::OnKey));  
	Connect(wxID_ANY, wxEVT_CHAR, wxKeyEventHandler(TextGrid::OnTypeKey));  
}


TextGrid::~TextGrid()
{
	delete undogrid;
	delete rightmenu;
}


wxString TextGrid::GetCell(int row, int col)
{
	int numrows = GetNumberRows();
	int numcols = GetNumberCols();

	if(row >= numrows || col >= numcols) return ""; 
	else return GetCellValue(row, col);
}


void TextGrid::SetCell(int row, int col, wxString data)
{
	int numrows = GetNumberRows();
	int numcols = GetNumberCols();

	if(row >= numrows) {
		AppendRows(row - numrows + 1);
	}
	if(col >= numcols) {
		AppendCols(col - numcols + 1);
	}
	SetCellValue(row, col, data);
}


void TextGrid::OnTypeKey(wxKeyEvent &event)
{
	wxString text;

	int r = GetGridCursorRow();
	int c = GetGridCursorCol();
	//SetCellValue(r, c, text.Format("%c ", event.GetKeyCode()));
	event.Skip();
}


void TextGrid::OnKey(wxKeyEvent &event)
{
	wxString text;

	//SetCellValue(10, 0, text.Format("%d", event.GetUnicodeKey()));

	if(event.GetUnicodeKey() == 'C' && event.ControlDown() == true) Copy();

	else if(event.GetUnicodeKey() == 'V' && event.ControlDown() == true) Paste();

	else if(event.GetUnicodeKey() == 'Z' && event.ControlDown() == true) Undo();

	else if(event.GetUnicodeKey() == 'A' && event.ControlDown() == true) SelectAll();

	else if(event.GetKeyCode() == WXK_DELETE) {
		Delete();
		return;
	}

	/*
	else if(event.GetKeyCode() == 'T') {
	int r = GetGridCursorRow();
	int c = GetGridCursorCol();
	//SetCellValue(r, c, text.Format("%c %c", 'e', event.GetKeyCode()));
	EnableCellEditControl();
	}*/

	event.Skip();
}


void TextGrid::ParseLine(int row, int col, wxString readline)
{
	int i;
	wxString text;

	while(!readline.IsEmpty()) {
		text = readline.BeforeFirst(' ');
		text.Trim();
		SetCellValue(row, col++, text);
		readline = readline.AfterFirst(' ');
	}
}
 

void TextGrid::ClearCol(int col)
{
	int i;

	for(i=0; i<GetNumberRows(); i++) SetCellValue(i, col, "");
}


void TextGrid::SetBold()
{
	int i, j;

	CopyUndo();

	for(i=0; i<GetNumberRows(); i++)     
		for(j=0; j<GetNumberCols(); j++)
			if(IsInSelection(i, j)) SetCellFont(i, j, wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	Refresh();
}


void TextGrid::OnSelectAll(wxCommandEvent& event)
{
	SelectAll();
}


void TextGrid::OnCopy(wxCommandEvent& event)
{
	Copy();
}


void TextGrid::OnPaste(wxCommandEvent& event)
{
	Paste();
}


void TextGrid::OnBold(wxCommandEvent& event)
{
	SetBold();
}


void TextGrid::Delete()
{
	int i, j;

	CopyUndo();

	SetCellValue(GetGridCursorRow(), GetGridCursorCol(), "");

	for(i=0; i<GetNumberRows(); i++)     
		for(j=0; j<GetNumberCols(); j++)
			if(IsInSelection(i, j)) SetCellValue(i, j, "");
}


void TextGrid::Copy()
{
	int i, j;

	// grid copy code from wxwidgets forum

	wxString copy_data;
	bool used_line;

	copy_data.Clear();

	for(i=0; i<GetNumberRows(); i++) {     
		used_line = false;   
		for(j=0; j<GetNumberCols(); j++) { 
			if(IsInSelection(i, j)) {
				if(!used_line) { 
					if(!copy_data.IsEmpty()) copy_data.Append("\n"); 
					used_line = true;
				}
				else copy_data.Append("\t");   
				copy_data.Append(GetCellValue(i, j));
			}
		}
	}


	if(ostype == Mac) {
		if (wxTheClipboard->Open()) {
			wxTextDataObject *clipboard_data = new wxTextDataObject();
			clipboard_data->SetData(wxDF_TEXT,strlen(copy_data.c_str()), copy_data.c_str());
			wxTheClipboard->Clear();

			if (!wxTheClipboard->SetData(clipboard_data)) {
				wxMessageBox(_("Unable to copy data to clipboard."), _("Error"));
			}
			wxTheClipboard->Close();
		} else {
			wxMessageBox(_("Error opening clipboard."), _("Error"));
		}
	}
	else {
#ifndef OSXClip
		wxOpenClipboard();         
		wxEmptyClipboard();
		wxSetClipboardData(wxDF_TEXT, copy_data.c_str(), 0, 0);
		wxCloseClipboard();
#endif
	} 
}


void TextGrid::Paste()
{
	int i, j, k;

	// grid paste code from wxwidgets forum

	CopyUndo();

	wxString copy_data, cur_field, cur_line;

	if(ostype == Mac) {
		wxTheClipboard->Open();
		wxTextDataObject data;
		wxTheClipboard->GetData(data);
		copy_data = data.GetText();
		wxTheClipboard->Close();
	}
	else {
#ifndef OSXClip
		wxOpenClipboard(); 
		copy_data = (char *)wxGetClipboardData(wxDF_TEXT);
		wxCloseClipboard();
#endif
	}

	i = GetGridCursorRow();
	j = GetGridCursorCol();
	k = j;

	while(!copy_data.IsEmpty()) {
		cur_line = copy_data.BeforeFirst('\n');
		while(!cur_line.IsEmpty()) {
			cur_field = cur_line.BeforeFirst('\t');
			if(i < GetNumberRows() && j < GetNumberCols()) SetCellValue(i, j, cur_field);
			j++;
			cur_line  = cur_line.AfterFirst ('\t');
		}
		i++;
		j = k;
		copy_data = copy_data.AfterFirst('\n');
	} 
}


void TextGrid::OnUndo(wxCommandEvent& event)
{
	Undo();
}


void TextGrid::OnLeftClick(wxGridEvent& event)
{
	int i, r, c;

	wxPoint pos = event.GetPosition();
	r = GetGridCursorRow();
	c = GetGridCursorCol();

	/*
	if(event.GetRow() == r && event.GetCol() == c) 
	EnableCellEditControl();
	else 
	GoToCell(event.GetRow(), event.GetCol());*/

	event.Skip();

	//wxGridCellEditor* celledit = GetCellEditor(r, c);

	/*
	wxGridCellEditor* pEditor = GetCellEditor(ev.GetRow(), ev.GetCol());
	wxPoint* pClickPoint = (wxPoint*)pEditor->GetClientData();
	if (pClickPoint)
	{
	*pClickPoint = ClientToScreen(ev.GetPosition());
	#ifndef __WINDOWS__
	EnableCellEditControl(true);
	#endif
	}
	// hack to prevent selection from being lost when click combobox
	if (ev.GetCol() == 0 && IsInSelection(ev.GetRow(), ev.GetCol()))
	{
	m_selTemp = m_selection;
	m_selection = NULL;
	}
	pEditor->DecRef();
	ev.Skip();
	*/
}


void TextGrid::OnRightClick(wxGridEvent& event)
{
	int i;
	//int id = event.GetId();
	//wxWindow *pos = FindWindowById(id, toolpanel);
	//wxPoint point = this->GetPosition();
	//SetCellValue(10, 0, "right");

	wxPoint pos = event.GetPosition();
	//wxSize size = this->GetSize();
	//menuPlot->Check(1000 + gpos->GetFront()->gindex, true);
	PopupMenu(rightmenu, pos.x - 20, pos.y);
}


void TextGrid::CopyUndo()
{
	int x, y;
	wxString data;

	if(GetNumberRows() > undogrid->GetNumberRows()) undogrid->AppendRows(GetNumberRows() - undogrid->GetNumberRows());
	if(GetNumberCols() > undogrid->GetNumberCols()) undogrid->AppendCols(GetNumberCols() - undogrid->GetNumberCols());

	for(x=0; x<GetNumberCols(); x++)
		for(y=0; y<GetNumberRows(); y++) {
			data = GetCellValue(y, x);
			undogrid->SetValue(y, x, data);
		}
}


void TextGrid::Undo()
{
	int x, y;
	wxString data;

	if(GetNumberRows() > undogrid->GetNumberRows()) undogrid->AppendRows(GetNumberRows() - undogrid->GetNumberRows());
	if(GetNumberCols() > undogrid->GetNumberCols()) undogrid->AppendCols(GetNumberCols() - undogrid->GetNumberCols());

	for(x=0; x<GetNumberCols(); x++)
		for(y=0; y<GetNumberRows(); y++) {
			data = undogrid->GetValue(y, x);
			SetCellValue(y, x, data);
		}
}