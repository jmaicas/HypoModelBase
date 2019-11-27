

#ifndef HYPOTOOLS_H
#define HYPOTOOLS_H



//#include "wx/wx.h"
//#include "hypobase.h"
#include "hypocontrols.h"
#include "hypopanels.h"
#include <wx/clipbrd.h>
#include <wx/grid.h>


class OutBox;


class DiagBox: public ToolBox
{
public:
	wxTextCtrl *textbox;

	void Write(wxString);
	DiagBox(MainFrame *main, const wxString& title, const wxPoint& pos, const wxSize& size);
};


class TextGrid : public wxGrid
{
public:
	wxMenu *rightmenu;
	TextGrid(wxWindow *parent, wxSize size);
	~TextGrid();
	int ostype;
	DiagBox *diagbox;

	wxGridStringTable *undogrid;
	wxTextCtrl *vdu;
	OutBox *outbox;
	wxGauge *gauge;

	void OnRightClick(wxGridEvent& event);
	void OnLeftClick(wxGridEvent& event);
	void OnLabelClick(wxGridEvent& event);
	void OnKey(wxKeyEvent& event);
	void OnTypeKey(wxKeyEvent &event);
	void OnSelectAll(wxCommandEvent& event);
	void OnCut(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void Undo();
	void OnPaste(wxCommandEvent& event);
	void OnUndo(wxCommandEvent& event);
	void OnBold(wxCommandEvent& event);
	void CopyUndo();
	void Copy();
	void Paste();
	void Cut();
	void Delete();
	void SetBold();
	void ClearCol(int);
	void ParseLine(int row, int col, wxString line);
	void ParseLabel(int row, int col, wxString line);
	void SetCell(int row, int col, wxString data);
	double ReadDouble(int row, int col);
	wxString GetCell(int row, int col);
	void CopyColumn(int source, int dest);
};


#endif