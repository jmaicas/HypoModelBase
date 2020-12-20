

#include <wx/graphics.h>
#include <hypomodel.h>


GraphPanel::GraphPanel(wxFrame *parent)
	: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
	usebuffer = false;
	SetBackgroundColour("WHITE");

	Connect(wxEVT_PAINT, wxPaintEventHandler(GraphPanel::OnPaint));
}


void GraphPanel::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	if(usebuffer) {
		wxBufferedPaintDC bpdc(this);
		Draw(bpdc);
	}
	else {
		wxPaintDC pdc(this);
		Draw(pdc);
	}
}


void GraphPanel::Draw(wxDC& dc)
{
	PrepareDC(dc);

	//m_owner->PrepareDC(dc);
	PaintBackground(dc);
}


void GraphPanel::PaintBackground(wxDC& dc)
{
	wxColour backgroundColour = GetBackgroundColour();
	//if(!backgroundColour.Ok()) backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

	dc.SetBrush(wxBrush(backgroundColour));
	dc.SetPen(wxPen(backgroundColour, 1));

	wxRect windowRect(wxPoint(0, 0), GetClientSize());    
	dc.DrawRectangle(windowRect);
}