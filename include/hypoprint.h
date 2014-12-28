


#include "wx/wx.h"
#include "hypobase.h"
#include "hypodat.h"
#include "hypomods.h"
#include <wx/print.h>



class GraphEPS
{
public:
	HypoMain *mainwin;

	void WriteEPS();
};



// Defines a new printout class to print our document
class GraphPrint: public wxPrintout
{
public:
    GraphPrint(HypoMain *main, GraphDat *graphdat, const wxString &title = "Graph Print");
     //   : wxPrintout(title);
		
		//{mainwin = main; graph = graphdat;}

    virtual bool OnPrintPage(int page);
    virtual bool HasPage(int page);
    virtual bool OnBeginDocument(int startPage, int endPage);
    virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

    void Plot();
    void DrawPageTwo();

    // Writes a header on a page. Margin units are in millimetres.
    bool WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString& text, float mmToLogical);

private:
    HypoMain *mainwin;
		GraphDat *graph;
		wxColour colourpen[10];
		int ostype;
		BurstDat *burstdata;
};