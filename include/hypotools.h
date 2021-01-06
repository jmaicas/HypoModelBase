

#ifndef HYPOTOOLS_H
#define HYPOTOOLS_H



//#include "wx/wx.h"
//#include "hypobase.h"
#include "hypocontrols.h"
//#include "hypopanels.h"
//#include "hypomodel.h"



class GridBox;
class Model;


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
	int ostype;
	DiagBox *diagbox;
	Model *mod;

	int selectcol, selectrow;

	wxGridStringTable *undogrid;
	wxTextCtrl *vdu;
	GridBox *gridbox;
	wxGauge *gauge;

	TextGrid(wxWindow *parent, wxSize size);
	~TextGrid();

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
	void Paste(int mode=0);  // mode 0 normal, mode 1 transpose
	void Cut();
	void Delete();
	void SetBold();
	void ClearCol(int);
	void ParseLine(int row, int col, wxString line);
	void ParseLabel(int row, int col, wxString line);
	void SetCell(int row, int col, wxString data);
	double ReadDouble(int row, int col);
	bool CheckDouble(int row, int col, double *data);
	wxString GetCell(int row, int col);
	void CopyColumn(int source, int dest);
};


#endif
