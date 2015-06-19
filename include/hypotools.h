

#ifndef HYPOTOOLS_H
#define HYPOTOOLS_H



//#include "wx/wx.h"
//#include "hypobase.h"
#include "hypocontrols.h"
#include <wx/clipbrd.h>
#include <wx/grid.h>



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

		wxGridStringTable *undogrid;

		void OnRightClick(wxGridEvent& event);
		void OnLeftClick(wxGridEvent& event);
		void OnKey(wxKeyEvent& event);
		void OnTypeKey(wxKeyEvent &event);
		void OnSelectAll(wxCommandEvent& event);
		void OnCopy(wxCommandEvent& event);
		void Undo();
		void OnPaste(wxCommandEvent& event);
		void OnUndo(wxCommandEvent& event);
		void OnBold(wxCommandEvent& event);
		void CopyUndo();
		void Copy();
		void Paste();
		void Delete();
		void SetBold();
		void ClearCol(int);
		void ParseLine(int row, int col, wxString line);
		void SetCell(int row, int col, wxString data);
		wxString GetCell(int row, int col);
};


#endif