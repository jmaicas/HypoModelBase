

//#include "hypomodel.h"
//#include "hypopanels.h"
#include "wx/graphics.h"
#include "hypoprint.h"
#include "hypomodel.h"
#include <math.h>


DispWin::DispWin(HypoMain *main, const wxString& title, const wxPoint& pos, const wxSize& size, int start, int numgraphs)
	: wxFrame(main, wxID_ANY, title, pos, size,
	wxFRAME_TOOL_WINDOW | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER)
{
	//int graph;

	i = 0;
	mainwin = main;
	gpos = mainwin->gpos;
	startpos = start;
	numdraw = numgraphs;

	//scalebox = new ScaleBox(mainwin, this, wxSize(80, 780), numdraw, gpos, mainwin->mod, graphwin, startpos);

	wxBoxSizer *mainsizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *graphsizer = new wxBoxSizer(wxVERTICAL); 

	graphpanel[0] = new GraphPanel(this);

	//graphsizer->AddSpacer(5);
	//mainsizer->Add(scalebox, 1, wxEXPAND);
	mainsizer->Add(graphsizer, 7, wxEXPAND);


	graphsizer->Add(graphpanel[0], 1, wxEXPAND);

	/*
	for(graph = 0; graph<numdraw; graph++) {
	graphwin[graph] = new GraphWindow3(mainwin, this, mainwin->mod, wxPoint(0, graph*250 + 10), wxSize(100, 255), &gpos[graph+startpos], graph+startpos);
	graphsizer->Add(graphwin[graph], 1, wxEXPAND);
	}*/

	CreateStatusBar();
	SetSizer(mainsizer);
	Layout();
	//mainpos = GetPosition();
}


void DispWin::GraphUpdate()
{
	int graph;

	for(graph=0; graph<numdraw; graph++) graphwin[graph]->UpdateScroll();
}


GraphWindow3::GraphWindow3(HypoMain *main, wxFrame *parent, Model *model, wxPoint pos, wxSize size, GraphDisp *gdisp, int index)
	: wxPanel(parent, wxID_ANY, pos, size)
{
	mainwin = main;
	drawframe = parent;
	gpos = gdisp;
	graphindex = index;
	mod = model;
	graphbase = mod->graphbase;

	int max, scrollxto;
	//double xdiff;
	wxString snum, outline, text;
	int i, j, gindex;
	//GraphSet *graphset;
	//wxMenu *newsub;

	ostype = GetSystem();

	yplot = 200;
	xplot = 500;
	max = 1440;
	xbase = 40;
	ybase = 10;
	xstretch = mainwin->xstretch;

	numdisps = 0;
	currentgraph = 0;
	spikedisp = 0;
	gsynch = 1;

	selectband = false;
	//overlay = wxOverlay();

	colourpen = mainwin->colourpen;

	//overlay = new wxOverlay();

	//dc(this);
	//wxDCOverlay odc(overlay, &dc);
	//wxGraphicsContext* ctx = wxGraphicsContext::Create(dc);
	////ctx->SetPen(*wxGREY_PEN);
	//ctx->SetBrush(wxBrush(wxColour(192, 192, 255, 64)));
	//wxRect newrect(anchorpos, currentpos);
	//ctx->DrawRectangle(newrect.x, newrect.y, newrect.width, newrect.height);


	if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("\ngraphwindow %d\n", graphindex));

	wxImage::AddHandler(new wxPNGHandler);
	radio_on = wxBitmap(mainwin->mainpath + "Init/radio_on-18.png", wxBITMAP_TYPE_PNG);
	radio_off = wxBitmap(mainwin->mainpath + "Init/radio_off-18.png", wxBITMAP_TYPE_PNG);

	SetBackgroundColour("WHITE");

	if(ostype == Mac) {
        textfont = wxFont(wxFontInfo(10).FaceName("Tahoma"));
        smallfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
		//textfont = wxFont(10, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Helvetica");
		//smallfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Helvetica");
	}
	else {
        textfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
        smallfont = wxFont(wxFontInfo(6).FaceName("Tahoma"));
		//textfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Helvetica");
		//smallfont = wxFont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Helvetica");
	}

	//double cf = 0.96;

	/*
	colourstring[0] = "0 0 0";
	colourstring[1] = "0.96 0 0";
	colourstring[2] = "0 0.96 0";
	colourstring[3] = "0 0 0.96";
	colourstring[4] = "0.96 0.96 0";
	colourstring[5] = "0.96 0 0.96";
	colourstring[6] = "1 0.5 0.5";
	colourstring[7] = "0.5 1 0.5";
	colourstring[8] = "0.5 0.5 1";*/

	scrollbar = new wxScrollBar(this, wxID_ANY, wxPoint(30, yplot + 35), wxSize(xplot + 50, 15));
	scrollbar->SetScrollbar(0, 40, xplot, 40);

	//wxBufferedPaintDC dc(this);
	//dc = new wxBufferedPaintDC(this);

	//Connect(wxEVT_PAINT, wxPaintEventHandler(GraphWindow3::OnPaint));
	Connect(wxEVT_PAINT, wxPaintEventHandler(GraphWindow3::OnPaintGC));
	Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(GraphWindow3::OnEraseBackground));

	Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(GraphWindow3::OnScroll)); 
	Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(GraphWindow3::OnScroll)); 
	Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(GraphWindow3::OnScroll)); 
	Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(GraphWindow3::OnScroll)); 
	Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(GraphWindow3::OnScroll)); 
	Connect(wxEVT_MOTION, wxMouseEventHandler(GraphWindow3::OnMouseMove));
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(GraphWindow3::OnLeftDown));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(GraphWindow3::OnLeftUp));
	Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(GraphWindow3::OnRightClick));
	//Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraph), 1000, 1050);
	//Connect(1000, 1000 + mod->graphbase->numgraphs - 1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraph));
	Connect(ID_GraphRemove, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphRemove));
	Connect(ID_GraphPrint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphPrint));
	Connect(ID_GraphEPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphEPS));
	Connect(ID_MultiEPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnMultiEPS));
	Connect(ID_MultiCell, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnMultiCell));
	Connect(ID_Scale, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnScale));
	Connect(ID_UnZoom, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnUnZoom));
}


GraphWindow3::~GraphWindow3()
{
	//delete overlay;
	//delete menuPlot;
}


void GraphWindow3::OnUnZoom(wxCommandEvent& event)
{
	if(!graph->oldset) return;
	graph->xfrom = graph->oldxfrom;	
	graph->xto = graph->oldxto;	
	graph->yfrom = graph->oldyfrom;	
	graph->yto = graph->oldyto;	
	mainwin->scalebox->ScaleUpdate();
}


void GraphWindow3::OnGraphRemove(wxCommandEvent& event)
{
	Refresh();
	mainwin->RemoveGraph(graphindex);
	//this->Destroy();
}


void GraphWindow3::OnGraphEPS(wxCommandEvent& event)
{
	PrintEPS();
}


void GraphWindow3::OnMultiEPS(wxCommandEvent& event)
{
	MultiEPS();
}


void GraphWindow3::OnMultiCell(wxCommandEvent& event)
{
	MultiCell();
}


void GraphWindow3::OnGraphPrint(wxCommandEvent& event)
{
	wxString text; 
	GraphDat *graph;

	if(mod->diagbox) mod->diagbox->textbox->AppendText(text.Format("Print Graph %d\n", graphindex));

	graph = dispset[0]->plot[0];

	// Pass two printout objects: for preview, and possible printing.
	wxPrintDialogData printDialogData(*mainwin->printdata);
	wxPrintPreview *preview = new wxPrintPreview(new GraphPrint(mainwin, graph), new GraphPrint(mainwin, graph), &printDialogData);
	wxPreviewFrame *frame = new wxPreviewFrame(preview, this, "Graph Preview", wxPoint(100, 100), wxSize(600, 650));
	frame->Centre(wxBOTH);
	frame->Initialize();
	frame->Show();	

	//(*g_printData) = printer.GetPrintDialogData().GetPrintData();
}


void GraphWindow3::OnScale(wxCommandEvent& event)
{
	//GraphBox *graphbox = new GraphBox(this, "Axis Panel");
	if(!mainwin->graphbox) mainwin->graphbox = new GraphBox(this, "Plot Panel");
	else mainwin->graphbox->SetGraph(this);
	mainwin->graphbox->Show(true);
}


void GraphWindow3::OnGraphSelectSet(wxCommandEvent& event)
{
	short id = event.GetId();
	double xfrom, xto;
	//int modesum = -1;
	GraphSet *graphset;
	GraphDat *graph;
	int gdex;
	wxString text;

	snum.Printf("Graph Set Select ID %d\n", id);
	//mainwin->SetStatusText(snum);
	if(mod->diagbox) mod->diagbox->textbox->AppendText(snum);
	xfrom = gpos->GetFront()->xfrom;
	xto = gpos->GetFront()->xto;

	
	graphset = mod->graphbase->GetSet(id - 1000);
	gdex = graphset->GetPlot(mainwin->scalebox->GetFlags());
	gpos->gdex = gdex;
	gpos->sdex = graphset->sdex;
	

	gpos->Front((*mod->graphbase)[gdex]);
	
	//gpos->Front((*mod->graphbase)[id-1000]);

	graph = (*mod->graphbase)[gdex];

	mod->diagbox->Write(text.Format("OnGraph id %d set %d name %s plot %d name %s\n", id, graphset->sdex, graphset->name, gdex, graph->gname));
	mod->diagbox->Write(graphset->Display());

	gpos->GetFront()->xfrom = xfrom;
	gpos->GetFront()->xto = xto;
	//mod->gcodes[graphindex] = mod->graphbase->GetTag(id-1000);
	mod->gcodes[graphindex] = mod->graphbase->GetSetTag(id-1000);
	mainwin->scalebox->ScaleUpdate();
	//Refresh();
}


void GraphWindow3::OnGraphSelectPlot(wxCommandEvent& event)
{
	short id = event.GetId();
	double xfrom, xto;
	//int modesum = -1;
	GraphSet *graphset;
	GraphDat *graph;
	int gdex;
	wxString text;

	snum.Printf("Graph Set Select ID %d\n", id);
	//mainwin->SetStatusText(snum);
	if(mod->diagbox) mod->diagbox->textbox->AppendText(snum);
	xfrom = gpos->GetFront()->xfrom;
	xto = gpos->GetFront()->xto;

    gdex = id - 2000;
	gpos->gdex = gdex;
	gpos->sdex = mod->graphbase->GetGraph(gdex)->sdex;
    graphset = mod->graphbase->GetSet(gpos->sdex);
	
	gpos->Front((*mod->graphbase)[gdex]);
	graph = (*mod->graphbase)[gdex];

	mod->diagbox->Write(text.Format("OnGraph id %d set %d name %s plot %d name %s\n", id, graphset->sdex, graphset->name, gdex, graph->gname));
	mod->diagbox->Write(graphset->Display());

	gpos->GetFront()->xfrom = xfrom;
	gpos->GetFront()->xto = xto;
	//mod->gcodes[graphindex] = mod->graphbase->GetTag(id-1000);
	//mod->gcodes[graphindex] = mod->graphbase->GetSetTag(id-1000);
	mainwin->scalebox->ScaleUpdate();
	//Refresh();
}


wxRealPoint GraphWindow3::GraphPos(wxPoint)
{
	return wxRealPoint(0, 0);
}


void GraphWindow3::OnLeftDown(wxMouseEvent &event)
{
	wxPoint pos = event.GetPosition();
	mousedown = pos;

	double xdiff = graph->xto - graph->xfrom;
	double xscale = xdiff / xplot;
	double xgraph = (mousedown.x - xbase) * xscale + graph->xfrom;

	double ydiff = graph->yto - graph->yfrom;
	double yscale = ydiff / yplot;
	double ygraph = (yplot - mousedown.y + ybase) * yscale + graph->yfrom;
	//ygraph = (yplot - pos.y + ybase) * yscale + graph->yfrom;

	snum.Printf("LDown X %d Y %d  graph %.2f %.2f", pos.x, pos.y, xgraph, ygraph);
	if(mainwin->diagnostic) mainwin->SetStatusText(snum);

	//int x, y, xx, yy ;
	//event.GetPosition(&x,&y);
	//CalcUnscrolledPosition( x, y, &xx, &yy );
	/*anchorpos = pos;
	currentpos = anchorpos;
	selectband = true;
	CaptureMouse();*/

	CaptureMouse();
	anchorpos = pos;
	if(anchorpos.x < xbase) anchorpos.x = xbase;
	if(anchorpos.x > xbase + xplot) anchorpos.x = xbase + xplot;
	if(anchorpos.y < ybase) currentpos.y = ybase;
	if(anchorpos.y > ybase + yplot) anchorpos.y = ybase + yplot;
}


void GraphWindow3::OnLeftUp(wxMouseEvent &event)
{
	double xdiff, xscale, xgraphFrom, xgraphTo;
	int xplaces;

	double ydiff, yscale, ygraphFrom, ygraphTo;
	int yplaces;


	// Graph select

	if(mainwin->graphbox) mainwin->graphbox->SetGraph(this);
	if(mainwin->plotbox) mainwin->plotbox->SetGraph(this);


	/*
	xdiff = graph->xto - graph->xfrom;
	xscale = xdiff / xplot;
	xgraph = (pos.x - xbase) * xscale + graph->xfrom;
	xplaces = numplaces(xdiff);

	ydiff = graph->yto - graph->yfrom;
	yscale = ydiff / yplot;
	ygraph = (yplot - pos.y + ybase) * yscale + graph->yfrom;
	yplaces = numplaces(ydiff);
	*/

	// Data Selection drag 

	wxPoint pos = event.GetPosition();
	if(pos.x - mousedown.x > 5) {
		graph = gpos->plot[0];

		xdiff = graph->xto - graph->xfrom;
		xscale = xdiff / xplot;
		xgraphFrom = (mousedown.x - xbase) * xscale + graph->xfrom;
		xgraphTo = (pos.x - xbase) * xscale + graph->xfrom;
		
		//xgraph = (pos.x - xbase) * xscale + graph->xfrom;

		ydiff = graph->yto - graph->yfrom;
		yscale = ydiff / yplot;
		ygraphFrom = (yplot - mousedown.y + ybase) * yscale + graph->yfrom;
		ygraphTo = (yplot - pos.y + ybase) * yscale + graph->yfrom;
		//ygraphTo = (pos.y - ybase) * yscale + graph->yfrom;

		// Range check and correct
		if(xgraphFrom > xgraphTo) std::swap(xgraphFrom, xgraphTo);        // reverse range for drag from right to left
		if(ygraphFrom > ygraphTo) std::swap(ygraphFrom, ygraphTo);

		if(xgraphFrom < graph->xfrom) xgraphFrom = graph->xfrom;          // trim range to current plot (for drag past axes)
		if(xgraphTo > graph->xto) xgraphTo = graph->xto;
		if(ygraphFrom < graph->yfrom) ygraphFrom = graph->yfrom;
		if(ygraphTo > graph->yto) ygraphTo = graph->yto;
		
		// Zoom Select
		if((*mainwin->hypoflags)["zoom"]) {

			graph->oldxfrom = graph->xfrom;
			graph->oldxto = graph->xto;
			graph->oldyfrom = graph->yfrom;
			graph->oldyto = graph->yto;
			graph->oldset = true;

			graph->xfrom = xgraphFrom;
			graph->xto = xgraphTo; 
			graph->yfrom = ygraphFrom;
			graph->yto = ygraphTo;
		
			scalebox->ScaleUpdate();
		}

		// Data Select
		mod->DataSelect(xgraphFrom, xgraphTo);

		// Diagnostic display
		xplaces = numplaces(xdiff);
		yplaces = numplaces(ydiff);
		snum.Printf("LUp x%d y%d  drag X %s To %s (%s)   Y %s To %s (%s)   xfrom %.2f yfrom %.2f", pos.x, pos.y, 
			numstring(xgraphFrom, xplaces), numstring(xgraphTo, xplaces), numstring(xgraphTo - xgraphFrom, xplaces),
			numstring(ygraphFrom, yplaces), numstring(ygraphTo, yplaces), numstring(ygraphTo - ygraphFrom, yplaces),
			graph->xfrom, graph->yfrom);
	}
	else snum.Printf("LUp %d", pos.x);

	if(mainwin->diagnostic) mainwin->SetStatusText(snum);

	if(!HasCapture()) return;
	ReleaseMouse();
	//wxRect permRect = wxRect(anchorpos, pos);
	overlay.Reset();
	Refresh();

	/*
	selectband = false;
	ReleaseMouse();
	{wxClientDC dc(this);
	PrepareDC(dc);
	wxDCOverlay overlaydc(overlay, &dc);
	overlaydc.Clear();}
	overlay.Reset();*/
}


void GraphWindow3::OnRightClick(wxMouseEvent& event)
{
	int i, j;
	wxString text;
	GraphSet *graphset;
	wxPoint pos = event.GetPosition();
	wxMenuItem *menuitem;
	wxMenu *menuPlot, *subPlot;
		
	menuPlot = new wxMenu;

	if(!mainwin->basic) {
		if(mainwin->project) {
			menuPlot->Append(ID_GraphEPS, "Export EPS");
			menuPlot->Append(ID_Scale, "Plot Panel");
			menuPlot->Append(ID_UnZoom, "Zoom Undo");
			menuPlot->AppendSeparator();
		}
		else {
		//menuPlot->Append(ID_GraphRemove, "Delete Graph");
		menuPlot->Append(ID_GraphEPS, "Export EPS");
		menuPlot->Append(ID_MultiEPS, "Multi EPS");
		menuPlot->Append(ID_MultiCell, "Multi Cell");
		menuPlot->Append(ID_Scale, "Plot Panel");
		menuPlot->Append(ID_UnZoom, "Zoom Undo");
		menuPlot->AppendSeparator();
		}
	}

	for(i=0; i<mod->graphbase->numsets; i++) {
		graphset = mod->graphbase->GetSet(i);
		if(!graphset->submenu) {
			menuitem = new wxMenuItem(menuPlot, 1000 + i, graphset->name, "", wxITEM_CHECK);
#ifndef OSX
			//menuitem->SetBitmaps(radio_on, radio_off);
#endif
			menuPlot->Append(menuitem);
			menuitem->Check(false);
			//menuPlot->AppendRadioItem(1000 + i, graphset->name);
		}
		else {
			subPlot = new wxMenu;
			for(j=0; j<graphset->numgraphs; j++) {
				menuitem = new wxMenuItem(subPlot, 2000 + graphset->gindex[j], graphset->GetPlot(j)->gname, "", wxITEM_CHECK);
#ifndef OSX
				//menuitem->SetBitmaps(radio_on, radio_off);
#endif
				subPlot->Append(menuitem);
				menuitem->Check(false);
			}
			//subPlot->AppendRadioItem(2000 + graphset->gindex[j], graphset->GetPlot(j)->gname);
			menuPlot->Append(ID_subplot, graphset->name, subPlot);
			//menuPlot->Check(ID_subplot, true);
		}
	}

	Connect(1000, 1000 + mod->graphbase->numsets - 1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphSelectSet));
	Connect(2000, 2000 + mod->graphbase->numgraphs, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphSelectPlot));

	//menuPlot->Check(1000, false);
	graphset = mod->graphbase->GetSet(gpos->sdex);
	if(!graphset->submenu) menuPlot->Check(1000 + gpos->sdex, true);
	else subPlot->Check(2000 + gpos->gdex, true);
	mainwin->diagbox->Write(text.Format("graph menu set %d\n", gpos->sdex));
	PopupMenu(menuPlot, pos.x + 20, pos.y);
}


void GraphWindow3::OnRightClickOld(wxMouseEvent& event)
{
	int i;
	wxString text;
	//int id = event.GetId();
	//wxWindow *pos = FindWindowById(id, toolpanel);
	//wxPoint point = this->GetPosition();

	wxPoint pos = event.GetPosition();
	//wxSize size = this->GetSize();

	//mainwin->diagbox->textbox->AppendText(text.Format("graph menu set %d\n", gpos->GetFront()->sdex));

	wxMenu *menuPlot = new wxMenu;
	if(!mainwin->basic) {
		if(mainwin->project) {
			menuPlot->Append(ID_GraphEPS, "Export EPS");
			menuPlot->Append(ID_Scale, "Plot Panel");
			menuPlot->Append(ID_UnZoom, "Zoom Undo");
			menuPlot->AppendSeparator();
		}
		else {
			//menuPlot->Append(ID_GraphRemove, "Delete Graph");
			//menuPlot->Append(ID_GraphPrint, "Print Graph");
			menuPlot->Append(ID_GraphEPS, "Export EPS");
			menuPlot->Append(ID_MultiEPS, "Multi EPS");
			menuPlot->Append(ID_MultiCell, "Multi Cell");
			menuPlot->Append(ID_Scale, "Plot Panel");
			menuPlot->Append(ID_UnZoom, "Zoom Undo");
			menuPlot->AppendSeparator();
		}
	}

	for(i=0; i<mod->graphbase->numsets; i++) {
		menuPlot->AppendRadioItem(1000 + i, mod->graphbase->setstore[i].name);
	}

	Connect(1000, 1000 + mod->graphbase->numgraphs - 1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphSelectSet));

	menuPlot->Check(1000 + gpos->sdex, true);
	mainwin->diagbox->Write(text.Format("graph menu set %d\n", gpos->sdex));
	PopupMenu(menuPlot, pos.x + 20, pos.y);
}


void GraphWindow3::AddGraph(GraphDisp *newgraph)
{
	if(numdisps < 5) {
		dispset[numdisps] = newgraph;
		numdisps++;
	}
}


void GraphWindow3::SetGraph(int index, GraphDisp *newgraph)
{
	if(index > numdisps) AddGraph(newgraph);
	else dispset[index] = newgraph;
}


void GraphWindow3::FrontGraph(GraphDisp *newgraph)
{
	dispset[0] = newgraph;
	if(numdisps == 0) numdisps = 1;
}


void GraphWindow3::UpdateScroll(int pos)
{
	double xdiff;
	wxString snum;
	int max, scrollxto;
	int section;

	max = 1440;

	graph = gpos->plot[0];
	if(graph->gparam == -3) max = graph->gdatav->max / graph->xscale;
	else if(graph->gparam == -4) max = graph->gdatadv->max / graph->xscale;
	else {
		mod->diagbox->Write("plot " + graph->gname + " no data\n");
		return;
	}
	//if(pos >= 0) graph->scrollpos = pos;
	//graph->scrollpos = 100;

	//if(graph->xscale > 1) {
	//	snum.Printf(wxT("max = %d"), max);
	//	//mainwin->SetStatusText(snum);
	//}

	xdiff = graph->xto - graph->xfrom;
	scrollxto = max * graph->binsize - (int)xdiff;
	section = xdiff / 10;
	scrollbar->SetScrollbar(graph->scrollpos, section, scrollxto + section + xdiff/5, section);

	Refresh();
}


void GraphWindow3::OnMouseMove(wxMouseEvent &event)
{
	double xdiff, xscale, xgraph;
	double ydiff, yscale, ygraph;
	int xplaces, yplaces;
	short gid;
	wxPoint pos;

	pos = event.GetPosition();

	if((*mainwin->hypoflags)["xypos"]) {
		graph = gpos->plot[0];
		gid = graph->gindex;

		xdiff = graph->xto - graph->xfrom;
		xscale = xdiff / xplot;
		xgraph = (pos.x - xbase) * xscale + graph->xfrom;
		xplaces = numplaces(xdiff);

		ydiff = graph->yto - graph->yfrom;
		yscale = ydiff / yplot;
		ygraph = (yplot - pos.y + ybase) * yscale + graph->yfrom;
		yplaces = numplaces(ydiff);

		//snum.Printf("GMove X %d Y %d gX %.2f gY %.2f", pos.x, pos.y, xgraph, ygraph);
	
		if(mainwin->diagnostic) snum.Printf("Graph Position X %s Y %s  ID %d", numstring(xgraph, xplaces), numstring(ygraph, yplaces), gid);
		else snum.Printf("Graph Position X %s Y %s", numstring(xgraph, xplaces), numstring(ygraph, yplaces));
		mainwin->SetStatusText(snum);
	}

	if(!HasCapture()) return;

	currentpos = pos;
	if(currentpos.y > ybase + yplot - 1) currentpos.y = ybase + yplot - 1;
	if(currentpos.y < ybase + 1) currentpos.y = ybase + 1;
	if(currentpos.x > xbase + xplot - 1) currentpos.x = xbase + xplot - 1;
	if(currentpos.x < xbase + 1) currentpos.x = xbase + 1;
	//anchorpos.y = ybase + 1; // - 10;
	//currentpos.y = ybase + yplot - 1;

	//wxBufferedPaintDC dc(this);
	wxClientDC dc(this);
	wxDCOverlay overlaydc(overlay, &dc);
	overlaydc.Clear();

	wxGraphicsContext *ctx = wxGraphicsContext::Create(dc);
    //ctx->SetPen(*wxGREY_PEN);
	ctx->SetBrush(wxBrush(wxColour(192,192,255,64)));
	wxRect newrect(anchorpos, currentpos);
	ctx->DrawRectangle(newrect.x, newrect.y, newrect.width, newrect.height);
	

	/*
	if(selectband) {
		//int x,y, xx, yy;
		//event.GetPosition(&x,&y);
		//CalcUnscrolledPosition( x, y, &xx, &yy );

		currentpos = pos;
		if(currentpos.y > ybase + yplot - 1) currentpos.y = ybase + yplot - 1;
		if(currentpos.y < ybase + 1) currentpos.y = ybase + 1;
		if(currentpos.x > xbase + xplot - 1) currentpos.x = xbase + xplot - 1;
		if(currentpos.x < xbase + 1) currentpos.x = xbase + 1;
		anchorpos.y = ybase + 1; // - 10;
		currentpos.y = ybase + yplot - 1;

		wxRect newrect(anchorpos, currentpos);
		{
			wxClientDC dc(this);
			PrepareDC(dc);
			wxDCOverlay overlaydc(overlay, &dc, xbase, ybase, xplot, yplot);
			//wxDCOverlay overlaydc(overlay, &dc);
			overlaydc.Clear();
#ifdef __WXMAC__
			dc.SetPen(*wxGREY_PEN);
			dc.SetBrush(wxColour(192,192,192,64));
#else
			//dc.SetPen(wxPen(*wxBLUE, 2));
			dc.SetPen(wxPen(*wxLIGHT_GREY, 2));
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			//dc.SetBrush( *wxBLUE_BRUSH );
#endif	
			//dc.SetAlpha(0xA0);
			dc.DrawRectangle(newrect);
		}
	}
	*/
}


void GraphWindow3::XYSynch()
{
	int i;

	for(i=0; i<numdisps; i++) dispset[i]->XYSynch();
}


void GraphWindow3::ReSize(int newxplot, int newyplot)
{
	yplot = newyplot;
	xplot = newxplot;

	//UpdateScroll();
	//scrollbar->SetInitialSize();
	scrollbar->SetSize(xplot + xstretch + 30, -1);
	scrollbar->SetScrollbar(0, 40, xplot, 40);
	scrollbar->Move(30, yplot + 35);
	//Layout();
	Refresh();
}


void GraphWindow3::OnScroll(wxScrollEvent &event)
{
	int scrollpos;
	wxString snum;
	double xmax;
	double xdiff;
	int datatype;
	int plotindex;

	scrollpos = event.GetPosition();
	//snum.Printf("graph scroll = %d", scrollpos);
	//mainwin->SetStatusText(snum);	

	for(plotindex=0; plotindex<numdisps; plotindex++) {
		graph = dispset[plotindex]->plot[0];
		xdiff = graph->xto - graph->xfrom;
		graph->xfrom = scrollpos;
		graph->xto = scrollpos + xdiff;
		snum.Printf("%.1f", graph->xfrom);
		xf->SetValue(snum);
		snum.Printf("%.1f", graph->xto);
		xt->SetValue(snum);
		graph->scrollpos = scrollpos;
	}
	//scalebox->UpdateScale();
	if(gsynch) scalebox->ScrollUpdate(graphindex, scrollpos);
	else Refresh();
	//Refresh();
}


void GraphWindow3::PaintBackground(wxDC& dc)
{
	wxColour backgroundColour = GetBackgroundColour();
	//if(!backgroundColour.Ok()) backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

	dc.SetBrush(wxBrush(backgroundColour));
	dc.SetPen(wxPen(backgroundColour, 1));

	wxRect windowRect(wxPoint(0, 0), GetClientSize());    
	dc.DrawRectangle(windowRect);
}


void GraphWindow3::OnEraseBackground(wxEraseEvent& event)
{
}


void GraphWindow3::DrawLine(wxDC& dc, wxGraphicsContext *gc, int xstart, int ystart, int xend, int yend)
{
	//dc.DrawLine(xstart, ystart, xend, yend);

	gc->StrokeLine(xstart, ystart, xend, yend);
}


void GraphWindow3::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);
	//dc = new wxBufferedPaintDC(this);

	int gdex, gpar, gtype;
	int i, j, k, dir;
	int colour, res;
	int xpos, ypos, xgpos, ygpos, xindex;
	double xfrom, xto, yfrom, yto;
	double y, xrange, yrange, xnum;
	double srangex, srangey;
	double preval, mpoint;
	int *gdata;
	double *gdatad;
	datint *gdatav;
	datdouble *gdatadv;
	double binsize;
	int xbasefix;
	int xview, yview, oldx, oldy;
	int gdisp, gplot;
	int ylabels, xlabels;
	double xval, xscale, xdis;
	double yval, yscale;
	double timepoint, maxtime;
	double data;
	int pdir, sample, xtime;
	int xlabelgap;
	int barwidth, bargap;

	int diag;
	bool drawdiag;
	int highon;    // 9/2/17  Prototype highlighting code
	int xylab;
	double xlogmax, ylogmax;    // 8/12/17  Log axis scaling
	double xlogbase = 2.71828182845904523536028747135266250;   // 3;
	double ylogbase = 2.71828182845904523536028747135266250;   // 10;         // default values replaced by graph specific below

	wxString snum, gname, text;
	wxSize textsize;
	int roundcount;
	double ymax, ycalc;
	double xlogrange, ylogrange;

	FILE *ofp;
	TextFile opfile;
	//ofp = fopen("graph.txt", "w");
	//if(graphindex == 0) ofp = fopen("graph.txt", "w");

	//mainwin->diagbox->Write(text.Format("Graph Paint Call %d\n", graphindex));

	diag = mainwin->diagnostic;
	diag = 0;
	drawdiag = false;

	PaintBackground(dc);
	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	//wxFont textfont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));
	//wxFont smallfont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));

	xlabels = 10;
	ylabels = mainwin->ylabels;

	for(gdisp=0; gdisp<numdisps; gdisp++)
	for(gplot=0; gplot<dispset[gdisp]->numplots; gplot++) {
		//graph = gpos->plot[gplot];
		graph = dispset[gdisp]->plot[gplot];
		gpar = graph->gparam;
		if(gpar == -1) gdata = graph->gdata;
		if(gpar == -2) gdatad = graph->gdatad;
		if(gpar == -3) gdatav = graph->gdatav;
		if(gpar == -4) gdatadv = graph->gdatadv;
		xscale = graph->xscale;
		yscale = 1;
		xdis = graph->xdis;
		//xdis = 200;
		yfrom = graph->yfrom;
		yto = graph->yto;
		xfrom = graph->xfrom * xscale;
		xto = graph->xto * xscale;
		if(!graph->xlabels) graph->xlabels = 10; 
		if(!graph->ylabels) graph->ylabels = mainwin->ylabels;
		if(!graph->xstep) graph->xstep = (xto - xfrom) / xlabels;
		if(!graph->ystep) graph->ystep = (yto - yfrom) / ylabels;
		xlabels = graph->xlabels;
		ylabels = graph->ylabels;
		gname = graph->gname;
		binsize = graph->binsize;
		gtype = graph->type;
		colour = graph->colour;
		colourpen[custom] = graph->strokecolour;
		sample = graph->samprate;
		if(graph->spikedata != NULL) burstdata = graph->spikedata->burstdata;
		else burstdata = NULL;
		xtime = xto - xfrom;
		barwidth = graph->barwidth;
		bargap = graph->bargap;
		//burstdata = graph->burstdata;
		//spikedata = graph->spikedata;
		xlogbase = graph->xlogbase;
		ylogbase = graph->ylogbase;

		//if(gtype == 3) ofp = fopen("graph.txt", "w");
		if(drawdiag && burstdata != NULL && burstdata->burstdisp == 1) ofp = fopen("graph.txt", "w");  

		if(colour < 0 || colour > 9) colour = 3;

		dc.SetPen(colourpen[black]);
		//gc->SetPen(colourpen[black]);


		DrawLine(dc, gc, xbase, ybase, xbase, ybase + yplot);
		DrawLine(dc, gc, xbase, ybase + yplot, xbase + xplot + xstretch, ybase + yplot);

		//gc->StrokeLine(xbase, ybase, xbase, ybase + yplot);
		//gc->StrokeLine(xbase, ybase + yplot, xbase + 550, ybase + yplot);

		// Ticks and Labels

		dc.SetFont(textfont);

		//int ticks;

		int xcoord, ycoord;
		double xplotstep, yplotstep;

        if(graph->xtickmode) { // && graph->xstep > 0) {
			xlabels = (int)((xto - xfrom) / (xscale * graph->xstep));
			xplotstep = (xplot * graph->xstep) / (xto - xfrom);  
		}

		if(graph->xscalemode == 1 && xfrom > 0) xlogmax = log(xto / xfrom) / log(xlogbase);
		else xlogmax = 0;

		if(graph->yscalemode == 1 && yfrom > 0) ylogmax = log(yto / yfrom) / log(ylogbase);
		else ylogmax = 0;

		//V = Vmin * b ** (logmax * X / Xmax)
		//xval = xfrom * exp(xlogmax * 

		//if(graph->xscalemode == 1 && xfrom > 0) xpos = (int)((double)xplot * log(xval / xfrom) / xlogmax);  // log scaled x-axis  December 2017
		//else xpos = (xval - xfrom) * xrange;

		for(i=0; i<=xlabels && xlabels > 0; i++) {
			xcoord = i * xplot / xlabels;
			if(graph->xtickmode) xcoord = (int)(xplotstep * i);
			dc.DrawLine(xbase + xcoord, ybase + yplot, xbase + xcoord, ybase + yplot + 5);
			if(!graph->xlabelmode || (graph->xlabelmode == 2 && i > 0 && i < xlabels)) continue;
			xval = ((double)(xto - xfrom) / xlabels*i + xfrom) / xscale * graph->xunitscale / graph->xunitdscale - graph->xshift;
			if(graph->xtickmode) xval = (xfrom + graph->xstep * i) * graph->xunitscale / graph->xunitdscale - graph->xshift;

			// log scale mode
			if(graph->xscalemode == 1 && xfrom > 0) xval = xfrom * pow(xlogbase, xlogmax * xval / xto);

			srangex = abs((xto - xfrom) / xscale * graph->xunitscale / graph->xunitdscale);
			//snum.Printf("%.0f", xval + xdis);	
			//if(srangex < 10) snum.Printf("%.1f", xval + xdis);	
			//if(srangex < 1) snum.Printf("%.2f", xval + xdis);
			//if(srangex < 0.1) snum.Printf("%.3f", xval + xdis);	

			if(graph->xlabelplaces == -1) {
				if(srangex < 0.1) snum.Printf("%.3f", xval + xdis);
				else if(srangex < 1) snum.Printf("%.2f", xval + xdis);
				else if(srangex < 10) snum.Printf("%.1f", xval + xdis);
				else snum.Printf("%.0f", xval + xdis);
			}
			else snum = numstring(xval + xdis, graph->xlabelplaces);

			textsize = dc.GetTextExtent(snum);
			if(ostype == Mac)
				dc.DrawText(snum, xbase + xcoord - textsize.GetWidth()/2, ybase + yplot + 10);
			else
				dc.DrawText(snum, xbase + xcoord - textsize.GetWidth()/2, ybase + yplot + 10);
		}

		if(yplot < 150 && ylabels >= 10) dc.SetFont(smallfont);
		xylab = 5;

		if(graph->ytickmode && graph->ystep > 0) {                     // tick count and spacing for defined linear step size
			ylabels = (int)((yto - yfrom) / (yscale * graph->ystep));
			yplotstep = (yplot * graph->ystep) / (yto - yfrom);  
		}

		if(graph->ytickmode == 1 && graph->yscalemode == 1) {          // tick count and spacing for log scale, step size specifies exponent step
			ylogrange = log(yto - yfrom) / log(ylogbase);
			ylabels = (int)(ylogrange / graph->ystep);
			yplotstep = (yplot * graph->ystep) / ylogrange;
			mod->diagbox->Write(text.Format("ylog tick step %.2f ylabels %d stepsize %.2f ylogrange %.4f\n", graph->ystep, ylabels, yplotstep, ylogrange)); 
		}

		for(i=0; i<=ylabels && ylabels > 0; i+=1) {
			ycoord = i * yplot / ylabels;
			if(graph->ytickmode) ycoord = (int)(yplotstep * i);
			dc.DrawLine(xbase, ybase + yplot - ycoord, xbase - 5, ybase + yplot - ycoord);
			if(!graph->ylabelmode || (graph->ylabelmode == 2 && i > 0 && i < ylabels)) continue;
			if(graph->ytickmode == 0) yval = ((double)(yto - yfrom) / ylabels*i + yfrom) / yscale * graph->yunitscale - graph->yshift;
			if(graph->ytickmode == 1) yval = (yfrom + graph->ystep * i) * graph->yunitscale - graph->yshift;

			// log scale mode
			if(graph->yscalemode == 1 && yfrom > 0) {
				if(graph->ytickmode == 1) yval = pow(ylogbase, graph->ystep * i);
				if(graph->ytickmode == 0) yval = yfrom * pow(ylogbase, ylogmax * yval / yto);
			}

			srangey = abs((yto - yfrom) / yscale * graph->yunitscale);
			if(graph->ylabelplaces == -1) {
				if(srangey < 0.1) snum.Printf("%.3f", yval);
				else if(srangey < 1) snum.Printf("%.2f", yval);
				else if(srangey < 10) snum.Printf("%.1f", yval);
				else snum.Printf("%.0f", yval);
			}
			else snum = numstring(yval, graph->ylabelplaces);
			textsize = dc.GetTextExtent(snum);
			if(ostype == Mac)
				dc.DrawText(snum, xbase - xylab - textsize.GetWidth(), ybase + yplot - ycoord - textsize.GetHeight()/2);
			else
				dc.DrawText(snum, xbase - xylab - textsize.GetWidth(), ybase + yplot - ycoord - 7);
		}
		//gname.Printf("GText width %d height %d", textsize.x, textsize.y);

		if(yplot < 150) dc.SetFont(textfont);
		textsize = dc.GetTextExtent(gname);
		dc.DrawText(gname, xplot + 50 - textsize.GetWidth(), 30 + 15 * gplot); 

		dc.SetPen(colourpen[colour]);
		highon = 0;

		// Set drawing scales
		xto /= binsize;
		xfrom /= binsize;

		yrange = (double)yplot / (yto - yfrom);
		xrange = (double)xplot / (xto - xfrom); 
		xnum = (double)(xto - xfrom) / xplot;


		/* Plot Types

		1 - scaled width bars, histogram

		7 - scaled width bars

		4 - normal line graph

		5 - line graph with scaling fix

		2 - line graph with x data

		6 - line graph with sampling

		8 - scatter with sampling

		3 - spike rate data with optional burst colouring

		9 - bar chart with x data

		10 - scatter plot with x data

		11 - mean field plot

		*/

		//mainwin->diagbox->Write(text.Format("Graph %d %s type %d\n", gdisp, gname, gtype)); 


		if(gtype == 1) {                             // scaled width bars, Histogram    
			for(i=0; i<(xto - xfrom); i++) {
				if(gpar == -1) y = (double)gdata[i + (int)xfrom];
				if(gpar == -2) y = gdatad[i + (int)xfrom];
				if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
				if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
				xpos = i * xrange + xbase;

				if(i == graph->highstart) {					// new highlighting code  9/2/17
					highon = 1;   
					dc.SetPen(colourpen[graph->highcolour]);
				}
				if(i == graph->highstop) {
					highon = 0;
					dc.SetPen(colourpen[colour]);
				}
				
				if(xrange <= 1) {
					DrawLine(dc, gc, xpos, yplot + ybase, xpos, yplot + ybase - (int)(yrange * (y - yfrom)));
				}
				else {
					for(k=0; k<xrange-1; k++) {
						DrawLine(dc, gc, xpos + k, yplot + ybase, xpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
					}
				}
			}
		}

		if(gtype == 7) {                             // scaled width bars    
			for(i=0; i<(xto - xfrom); i++) {
				if(gpar == -1) y = (double)gdata[i + (int)xfrom];
				if(gpar == -2) y = gdatad[i + (int)xfrom];
				if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
				if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
				xpos = i * xrange + xbase;
				if(xrange <= 1) {
					DrawLine(dc, gc, xpos, yplot + ybase, xpos, yplot + ybase - (int)(yrange * (y - yfrom)));
				}
				else {
					for(k=0; k<xrange; k++) {
						DrawLine(dc, gc, xpos + k, yplot + ybase, xpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
					}
				}
			}
		}

		if(gtype == 4) {                         // normal line graph   - no longer in use, see type 5 below
			dir = 1;   // 1 for ascending, 0 for descending
			preval = (*gdatadv)[0];
			oldx = xbase;
			oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

			//mod->diagbox->Write(text.Format("line plot xrange %.4f yscalemode %d\n", xrange, graph->yscalemode));

			// subpixel scale drawing mode - drawing data in limited x-axis resolution, xrange gives ratio of plot pixels to data points, use this mode if xrange < 1
			//
			// attempt to preserve maxima and minima
			// 'dir' gives current direction of plot progression
			// 'xnum' gives number of data points for current pixel position, reciprocal of xrange
			// choose lowest or highest data point for plot value depending on direction

			//if(graph->xscalemode == 1 && xfrom > 0) xpos = (int)((double)xplot * (log(xval / xfrom) / log(xlogbase)) / xlogmax);  // log scaled x-axis  December 2017
		    //else xpos = (xval - xfrom) * xrange;

			for(i=0; i<xplot; i++) {
				if(xrange < 1) {
					xindex = (int)((i * xnum) + xfrom);
					mpoint = (*gdatadv)[xindex];
					for(j=1; j<xnum; j++) {
						if(dir) { 
							if((*gdatadv)[xindex + j] > mpoint) mpoint = (*gdatadv)[xindex + j];
						}
						else {
							if((*gdatadv)[xindex + j] < mpoint) mpoint = (*gdatadv)[xindex + j];
						}
					}
					if(preval < mpoint) dir = 1; else dir = 0;
					yval = mpoint;
					preval = mpoint;

					if(graph->yscalemode == 1 && yfrom > 0) ypos = (int)((double)yplot * (log(yval / yfrom) / log(ylogbase)) / ylogmax);  // log scaled y-axis  March 2018
					else ypos = (yval - yfrom) * yrange;
					DrawLine(dc, gc, oldx, oldy, i + xbase, (int)(yplot + ybase - ypos));	
					oldx = i + xbase;
					oldy = (int)(yplot + ybase - ypos);
				} 
				else {
					yval = (*gdatadv)[(int)(i + xfrom)];
					if(graph->yscalemode == 1 && yfrom > 0) ypos = (int)((double)yplot * (log(yval / yfrom) / log(ylogbase)) / ylogmax);  // log scaled y-axis  March 2018
					else ypos = (yval - yfrom) * yrange;
					DrawLine(dc, gc, oldx, oldy, (int)(i * xrange + xbase), (int)(yplot + ybase - ypos));
					oldx = i * xrange + xbase;
					oldy = (int)(yplot + ybase - ypos);
				}
				//fprintf(pofp, "xindex %d  y %.2f\n  dir %d", xindex, y, dir);
			}
		}

		int xoffset = 1;

		if(gtype == 5) {                         // line graph with scaling fix
			if(drawdiag) ofp = fopen("graph.txt", "w");
			//mod->diagbox->Write(text.Format("line plot xrange %.4f  yscalemode %d  ylogbase %.4f  ylogmax %.4f\n", xrange, graph->yscalemode, ylogbase, ylogmax));

			dir = 1;
			pdir = 0;
			xindex = (int)xfrom;
			if(gpar == -3) preval = (*gdatav)[xindex];
			if(gpar == -4) preval = (*gdatadv)[xindex];
			oldx = xbase + xoffset;
			oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

			for(i=1; i<xplot; i++) {
				if(xrange < 1) {
					xindex = (int)((i * xnum) + xfrom);
					//mpoint = (*gdatadv)[xindex];
					if(gpar == -3) mpoint = (*gdatav)[xindex];
					if(gpar == -4) mpoint = (*gdatadv)[xindex];
					if(drawdiag) fprintf(ofp, "xdraw %d  preval %.4f  dir %d\n", i, preval, dir);
					for(j=1; j<xnum; j++) {
						//data = (*gdatadv)[xindex + j];
						if(gpar == -3) data = (*gdatav)[xindex + j];
						if(gpar == -4) data = (*gdatadv)[xindex + j];
						if(drawdiag) fprintf(ofp, "xdraw %d, xnum %d, data %.4f\n", i, j, data);
						if(dir) {
							if(data > mpoint) mpoint = data;
							else if(data < mpoint) mpoint = data;
							//if(!dir && (*gdatadv)[xindex + j] < mpoint) mpoint = (*gdatadv)[xindex + j];
						}
					}
					if(preval <= mpoint || preval < 0.000001) dir = 1; else dir = 0;
					yval = mpoint;
					preval = mpoint;
					if(drawdiag) fprintf(ofp, "xdraw %d  preval %.4f  mpoint %.4f  point %.4f\n", i, preval, mpoint, y);

					if(graph->yscalemode == 1 && yfrom > 0) {
						ypos = (int)((double)yplot * (log(yval / yfrom) / log(ylogbase)) / ylogmax);  // log scaled y-axis  March 2018
						if(yval < yfrom) {
							ypos = -yfrom * yrange;
							//mod->diagbox->Write(text.Format("line draw log low value yval %.4f ypos %d\n", yval, ypos));
						}
					}
					else ypos = (yval - yfrom) * yrange;
					DrawLine(dc, gc, oldx, oldy, i + xbase + xoffset, (int)(yplot + ybase - ypos));	
					oldx = i + xbase + xoffset;
					oldy = (int)(yplot + ybase - ypos);

					//DrawLine(dc, gc, oldx, oldy, i + xbase + xoffset, (int)(yplot + ybase - yrange * (y - yfrom)));	
					//oldx = i + xbase + xoffset;
					//oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				} 
				else {
					//yval = (*gdatadv)[(int)(i + xfrom)];
					if(gpar == -3) yval = (*gdatav)[(int)(i + xfrom)];
					if(gpar == -4) yval = (*gdatadv)[(int)(i + xfrom)];
					if(graph->yscalemode == 1 && yfrom > 0) {
						ypos = (int)((double)yplot * (log(yval / yfrom) / log(ylogbase)) / ylogmax);  // log scaled y-axis  March 2018
						if(yval < yfrom) ypos = -yfrom * yrange;
					}
					else ypos = yrange * (yval - yfrom);
					DrawLine(dc, gc, oldx, oldy, (int)(i * xrange + xbase + xoffset), (int)(yplot + ybase - ypos));
					oldx = i * xrange + xbase + xoffset;
					oldy = (int)(yplot + ybase - ypos);
				}
				//fprintf(pofp, "xindex %d  y %.2f\n  dir %d", xindex, y, dir);
			}
			if(drawdiag) fclose(ofp);
		}

		double prevx, xvalnext;
		int xposnext;

		if(gtype == 2 && graph->gdatax) {				                            // line graph with X data
			//mainwin->diagbox->Write(text.Format("Graph Type 2  xcount %d xrange %.4f xplot %d\n", graph->xcount, xrange, xplot));
			oldx = xbase + xoffset;
			oldy = (int)(yplot + ybase - yrange * (yfrom));
			//mainwin->diagbox->Write(text.Format("\n XY graph maxindex %d xcount %d\n", graph->gdatax->maxindex, graph->xcount));
			for(i=0; i<graph->xcount; i++) {
				xval = (*graph->gdatax)[i];
				//if(xval <= prevx) break;
				if(xval >= xfrom && xval <= xto) {
					xpos = (int)((xval - xfrom) * xrange);
					y = (*gdatadv)[i];
					/*
					xposnext = (int)((*graph->gdatax)[i+1] - xfrom) * xrange;                  // proto code for scaling dense data for screen display 25/5/16
					while(xposnext == xpos) {
						i++;
						xposnext = (int)((*graph->gdatax)[i+1] - xfrom) * xrange;
					}*/

					//mainwin->diagbox->Write(text.Format("\n X %d val %.2f pos %d\n", i, xval, xpos));
					//mainwin->diagbox->Write(text.Format("XY graph line X %.4f Y %.4f\n", xval, y));

					dc.SetPen(colourpen[colour]);
					DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
					dc.SetPen(colourpen[black]);
					if(graph->scattermode) dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), graph->scattersize);
					oldx = xpos + xbase + xoffset;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				}
				prevx = xval;
			}
		}

		double xmin, xmax, ymin, ymax, xmid, ymid;
		int scatterfield = graph->scatterfield;
		int scattermean = graph->scattermean;;
		int width, height;
		double xmean, ymean, xvar, yvar, xsd, ysd;
		double xsdneg, xsdpos, ysdneg, ysdpos;


		if(gtype == 10 && graph->gdatax) {	           // scatter graph with X data and optional range fields
			//mainwin->diagbox->Write(text.Format("Graph Type 10  xcount %d xrange %.4f xplot %d\n", graph->xcount, xrange, xplot));
			//mainwin->diagbox->Write(text.Format("\n XY graph maxindex %d xcount %d\n", graph->gdatax->maxindex, graph->xcount));

			//if(graph->xscalemode == 1 && xfrom > 0) xlogmax = log(xto / xfrom) / log(logbase);
			xmean = 0;
			ymean = 0;
			for(i=0; i<graph->xcount; i++) {
				xval = (*graph->gdatax)[i];
				xmean += xval / graph->xcount;
				yval = (*gdatadv)[i];
				ymean += yval / graph->xcount;
				if(xval >= xfrom && xval <= xto) {
					if(graph->xscalemode == 1 && xfrom > 0) xpos = (int)((double)xplot * (log(xval / xfrom) / log(xlogbase)) / xlogmax);  // log scaled x-axis  December 2017
					else xpos = (xval - xfrom) * xrange;
					ypos = (yval - yfrom) * yrange;
					if(i == 0) {
						xmin = xpos;
						xmax = xpos;
						ymin = ypos;
						ymax = ypos;
					}
					else {
						if(xpos < xmin) xmin = xpos;
						if(xpos > xmax) xmax = xpos;
						if(ypos < ymin) ymin = ypos;
						if(ypos > ymax) ymax = ypos;
					}
					dc.SetPen(colourpen[colour]);
					//dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (yval - yfrom)), graph->scattersize);	
					dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - ypos), graph->scattersize);	
				}
			}

			if(graph->xcount > 0 && scattermean) {
				xvar = 0;
				yvar = 0;
				for(i=0; i<graph->xcount; i++) {
					xvar += ((*graph->gdatax)[i] - xmean) * ((*graph->gdatax)[i] - xmean);
					yvar += ((*graph->gdatadv)[i] - ymean) * ((*graph->gdatadv)[i] - ymean);
				}
				xsd = sqrt(xvar / graph->xcount);
				ysd = sqrt(yvar / graph->xcount);
				if(graph->xscalemode == 1 && xfrom > 0) {
					xpos = (int)((double)xplot * (log(xmean / xfrom) / log(xlogbase)) / xlogmax);  
					xsdneg = (int)((double)xplot * (log((xmean - xsd) / xfrom) / log(xlogbase)) / xlogmax);
					xsdpos = (int)((double)xplot * (log((xmean + xsd) / xfrom) / log(xlogbase)) / xlogmax);  
				}
				else {
					xpos = (xmean - xfrom) * xrange;
					xsdneg = (xmean - xsd - xfrom) * xrange;
					xsdpos = (xmean + xsd - xfrom) * xrange;
				}
				ypos = (ymean - yfrom) * yrange;
				ysdneg = (ymean - ysd - yfrom) * yrange;
				ysdpos = (ymean + ysd - yfrom) * yrange;

				dc.SetPen(colourpen[black]);
				dc.SetBrush(*wxBLACK_BRUSH);
				dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - ypos), 3);	
				dc.DrawLine((int)(xsdneg + xbase + xoffset), (int)(yplot + ybase - ypos), (int)(xsdpos + xbase + xoffset), (int)(yplot + ybase - ypos));
				dc.DrawLine((int)(xpos + xbase + xoffset), (int)(yplot + ybase - ysdneg), (int)(xpos + xbase + xoffset), (int)(yplot + ybase - ysdpos));
			}

			if(graph->xcount > 0 && scatterfield) {
				xmid = (xmin + xmax) / 2;
				ymid = (ymin + ymax) / 2;
				width = (double)(xmax - xmin) * 1.5;
				height = (double)(ymax - ymin) * 1.5;
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(colourpen[colour]);
				dc.DrawEllipse((int)(xmid + xbase + xoffset - width/2), (int)(yplot + ybase - ymid - height/2), width, height);
			}
		}

		double xfield, yfield;

		if(gtype == 11 && graph->gdatax && graph->xcount > 1) {         // mean field plot - oval with centre at xy mean and xy StdDev dimensions      December 2017
			//mainwin->diagbox->Write(text.Format("Graph Type 11  xcount %d xrange %.4f xplot %d\n", graph->xcount, xrange, xplot));
			//mainwin->diagbox->Write(text.Format("\n XY graph maxindex %d xcount %d\n", graph->gdatax->maxindex, graph->xcount));

			if(graph->xscalemode == 1 && xfrom > 0) xlogmax = log(xto / xfrom);
			xval = (*graph->gdatax)[0];
			yval = (*gdatadv)[0];
			xfield = (*graph->gdatax)[1];
			yfield = (*gdatadv)[1];
			width = xfield * xrange * 2;
			height = yfield * yrange * 2;

			if(graph->xscalemode == 1 && xfrom > 0) {
				xpos = (int)((double)xplot * log(xval / xfrom) / xlogmax);  
				//xfield = 
			}
			else xpos = (int)((xval - xfrom) * xrange);
			dc.SetPen(colourpen[colour]);
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (yval - yfrom)), 10);
			dc.DrawEllipse((int)(xpos + xbase + xoffset - width), (int)(yplot + ybase - yrange * (yval - yfrom) - height), width*2, height*2);	
		}


		// xrange = (double)xplot / (xto - xfrom);  code copy comment for reference

		int barshift = graph->barshift;
		int barpos;

		if(gtype == 9 && graph->gdatax) {				                            // bar chart with X data
			//mainwin->diagbox->Write(text.Format("\n XY bar graph maxindex %d xcount %d\n", graph->gdatax->maxindex, graph->xcount));
			for(i=0; i<graph->xcount; i++) {
				xval = (*graph->gdatax)[i];
				//if(xval <= prevx) break;
				if(xval >= xfrom && xval <= xto) {
					xpos = (int)(xval - xfrom) * xrange;
					barshift = (barwidth * numdisps + (numdisps - 1) * bargap) / 2;
					barpos = xbase + xpos - barshift + gdisp * (barwidth + bargap);
					y = (*gdatadv)[i];
					//mainwin->diagbox->Write(text.Format("\n XY graph line X %.4f Y %.4f\n", xval, y));
					dc.SetPen(colourpen[colour]);
					//DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
					for(k=0; k<barwidth; k++) {
						DrawLine(dc, gc, barpos + k, yplot + ybase, barpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
					}
					//dc.SetPen(colourpen[black]);
					//if(graph->scattermode) dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), graph->scattersize);			
				}
			}
		}

		/*
		if(gtype == 7) {                             // scaled width bars    
		for(i=0; i<(xto - xfrom); i++) {
		if(gpar == -1) y = (double)gdata[i + (int)xfrom];
		if(gpar == -2) y = gdatad[i + (int)xfrom];
		if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
		if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
		xpos = i * xrange + xbase;
		if(xrange <= 1) {
		DrawLine(dc, gc, xpos, yplot + ybase, xpos, yplot + ybase - (int)(yrange * (y - yfrom)));
		}
		else {
		for(k=0; k<xrange; k++) {
		DrawLine(dc, gc, xpos + k, yplot + ybase, xpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
		}
		}
		}
		}*/


		if(gtype == 6) {                         // line graph with sampling
			oldx = xbase + xoffset;
			xindex = (int)xfrom / sample;
			if(xfrom > 0) {
				if((int)xfrom % sample != 0) {
					double sydiff = (*gdatadv)[xindex] - (*gdatadv)[xindex - 1];
					double sxdiff = (xfrom - xindex * sample) / sample;
					preval = sydiff * sxdiff + (*gdatadv)[xindex - 1];
				}
				else preval = (*gdatadv)[xindex - 1];
			}
			else preval = (*gdatadv)[0];

			//if(mainwin->diagnostic && xindex > 0) mod->diagbox->textbox->AppendText(text.Format("xindex %d xpredata %.4f preval %.4f\n\n", xindex, (*gdatadv)[xindex-1], preval)); 

			oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

			for(i=0; i<=(xto - xfrom) / sample; i++) {		
				xindex = i + ceil(xfrom / sample);
				xpos = (int)(xindex * sample - xfrom) * xrange;
				mpoint = (*gdatadv)[xindex];
				y = mpoint;
				//dc.LineTo(xindex + 100 + xmove, (int)(ymove + 130.0 - yrange * (y - yfrom)));
				DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
				oldx = xpos + xbase + xoffset;
				oldy = (int)(yplot + ybase - yrange * (y - yfrom));
			}
		}

		if(gtype == 8) {                         // scatter with sampling
			wxBrush brush(wxColour(255,255,255));
			dc.SetBrush(brush);
			//dc.SetPen(colourpen[black]);

			oldx = xbase + xoffset;
			xindex = (int)xfrom / sample;
			if(xfrom > 0) {
				if((int)xfrom % sample != 0) {
					double sydiff = (*gdatadv)[xindex] - (*gdatadv)[xindex - 1];
					double sxdiff = (xfrom - xindex * sample) / sample;
					preval = sydiff * sxdiff + (*gdatadv)[xindex - 1];
				}
				else preval = (*gdatadv)[xindex - 1];
			}
			else preval = (*gdatadv)[0];

			//if(mainwin->diagnostic && xindex > 0) mod->diagbox->textbox->AppendText(text.Format("xindex %d xpredata %.4f preval %.4f\n\n", xindex, (*gdatadv)[xindex-1], preval)); 

			oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

			dc.SetPen(colourpen[colour]);
			for(i=0; i<=(xto - xfrom) / sample; i++) {		
				xindex = i + ceil(xfrom / sample);
				xpos = (int)(xindex * sample - xfrom) * xrange;
				mpoint = (*gdatadv)[xindex];
				y = mpoint;
				//dc.LineTo(xindex + 100 + xmove, (int)(ymove + 130.0 - yrange * (y - yfrom)));
				DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
				//dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), 2);
				oldx = xpos + xbase + xoffset;
				oldy = (int)(yplot + ybase - yrange * (y - yfrom));
			}

			double xtodraw = xto;
			if(graph->xstop && xto > graph->xstop) xtodraw = graph->xstop;

			dc.SetPen(colourpen[black]);
			for(i=0; i<=(xtodraw - xfrom) / sample; i++) {		
				xindex = i + ceil(xfrom / sample);
				xpos = (int)(xindex * sample - xfrom) * xrange;
				mpoint = (*gdatadv)[xindex];
				y = mpoint;
				//dc.LineTo(xindex + 100 + xmove, (int)(ymove + 130.0 - yrange * (y - yfrom)));
				//DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
				dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), 2);
				oldx = xpos + xbase + xoffset;
				oldy = (int)(yplot + ybase - yrange * (y - yfrom));
			}
		}


		if(gtype == 3) {                             // spike rate data with optional burst colouring
			int spikestep = 0;
			int burstcolour = 0;
			//opfile.New("spikegraph.txt");
			//for(i=0; i<10; i++) opfile.WriteLine(text.Format("spike %d  Burst time %.2f\n", i, burstdata->times[i]));

			for(i=0; i<(xto - xfrom); i++) {
				if(gpar == -1) y = (double)gdata[i + (int)xfrom];
				if(gpar == -2) y = gdatad[i + (int)xfrom];	
				if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
				if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];

				res = 0;
				if(binsize == 1) res = 0;
				if(binsize == 0.1) res = 1;
				if(binsize == 0.01) res = 2;
				if(binsize == 0.001) res = 3;

				//wxString diag;
				//diag.Printf("spikedisp %d  res %d ", gpos->spikedisp, res);
				//mainwin->SetStatusText(diag);

				if(burstdata == NULL || burstdata->burstdisp == 0) {
					dc.SetPen(colourpen[red]);
					//mainwin->SetStatusText("no colour");
				}

				else {                  // burst colouring
					burstcolour = 0;
					//fprintf(ofp, "spikedisp 1\n");
					if(binsize == 0.1 || binsize == 0.01) {
						burstcolour = 0;
						//opfile.WriteLine("sub res");
						//if(burstdata[res][(i + (int)xfrom)] == 0) dc.SetPen(colourpen[red]);
						//else if(burstdata[res][(i + (int)xfrom)] % 2 == 0) dc.SetPen(colourpen[blue]);		
						//else if(burstdata[res][(i + (int)xfrom)] % 2 == 1) dc.SetPen(colourpen[green]);
					}

					if(binsize == 10 || binsize == 100) {
						timepoint = (xfrom + i + 1) * binsize * 1000;
						while(timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
							//while(burstdata->times[spikestep] < timepoint * binsize + 0.0005) {
							//opfile.WriteLine(text.Format("while  i %d  spike %d  time %.2f\n", i, spikestep, burstdata->times[spikestep]));
							if(!burstcolour) burstcolour = burstdata->spikes[spikestep];
							spikestep++;
						}
					}

					if(binsize == 1) {
						//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d\n", i, (i+xfrom)*binsize, spikestep);
						timepoint = (xfrom + i + 1) * binsize * 1000;
						while((timepoint - binsize) < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
							//while(burstdata->times[spikestep] < timepoint * binsize + 0.0005) {
							//opfile.WriteLine(text.Format("while  i %d  spike %d  time %.2f\n", i, spikestep, burstdata->times[spikestep]));
							if(!burstcolour) burstcolour = burstdata->spikes[spikestep];
							spikestep++;
						}
						//fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);
						//opfile.WriteLine(text.Format("spike %d time %.2f\n", spikestep, burstdata->times[spikestep]));
						//for(i=0; i<10; i++) fprintf(ofp, "Burst time %.2f\n", burstdata->times[i]);
						//fprintf(ofp, "Burst time %.2f\n", burstdata->times[100]);
						//opfile.WriteLine(text.Format("i %d  spike %d  burstcolour %d\n", i, spikestep, burstcolour));
					}

					if(binsize == 0.001) {
						//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d\n", i, (i+xfrom)*binsize, spikestep);
						timepoint = (xfrom + i) * binsize * 1000;
						while(timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
							//fprintf(ofp, "spike %d time %.10f comp %.10f y %.2f\n", 
							//	spikestep, burstdata->spikedata->times[spikestep], (i + xfrom) * binsize + 0.0005, y);
							burstcolour = burstdata->spikes[spikestep];
							spikestep++;
						}
						//fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);


						//if(i*binsize+0.0005 > graphdata[18][spikestep]) {
						//	burstcolour = graphdata[16][spikestep];
						//	spikestep++;
					}
					//fflush(ofp);
					if(burstcolour == 0) 	
						dc.SetPen(colourpen[red]);
					else if(burstcolour % 2 == 0)
						dc.SetPen(colourpen[blue]);
					else if(burstcolour % 2 == 1)
						dc.SetPen(colourpen[green]);	
				}


				/*
				if(spikedisp == 2) {                      // hazard filter
				if(res < 3) {
				if(hfdata[res][(i + (int)xfrom)] == 0)
				dc.SelectObject(redpen);
				else dc.SelectObject(greenpen);
				}

				if(res == 3) {
				//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d ", i, (i+xfrom)*binsize, spikestep);
				while(graphdata[18][spikestep] < (i+xfrom)*binsize+0.0005) {
				burstcolour = filterflag[spikestep];
				spikestep++;
				}
				//fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);	
				if(burstcolour == 0) 
				dc.SelectObject(redpen);
				else if(burstcolour % 2 == 0) 
				dc.SelectObject(bluepen);
				else if(burstcolour % 2 == 1) 
				dc.SelectObject(greenpen);
				}		
				}
				*/

				xpos = i * xrange + xbase;
				if(xrange <= 1) 
					DrawLine(dc, gc, xpos, ybase + yplot, xpos, ybase + yplot - (int)(yrange * (y - yfrom)));
				else {
					for(k=0; k<xrange-1; k++) 
						DrawLine(dc, gc, xpos + k, ybase + yplot, xpos + k, ybase + yplot - (int)(yrange * (y - yfrom))); 
				}	
			}
			//opfile.Close();
		}
	}
	//if(gtype == 3) fclose(ofp);
	//if(gpos->spikedisp == 1) fclose(ofp);
	if(drawdiag && burstdata != NULL && burstdata->burstdisp == 1) fclose(ofp);
	//fclose(ofp);
}

void GraphWindow3::OnPaintGC(wxPaintEvent& WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);
	//dc = new wxBufferedPaintDC(this);

	int gdex, gpar, gtype;
	int i, j, k, dir;
	int colour, res;
	int xpos, ypos, xgpos, ygpos, xindex;
	double xfrom, xto, yfrom, yto;
	double y, xrange, yrange, xnum;
	double srangex, srangey;
	double preval, mpoint;
	int* gdata;
	double* gdatad;
	datint* gdatav;
	datdouble* gdatadv;
	double binsize;
	int xbasefix;
	int xview, yview, oldx, oldy;
	int gdisp, gplot;
	int ylabels, xlabels;
	double xval, xscale, xdis;
	double yval, yscale;
	double timepoint, maxtime;
	double data;
	int pdir, sample, xtime;
	int xlabelgap;
	int barwidth, bargap;
	double textwidth, textheight;

	int diag;
	bool drawdiag;
	int highon;    // 9/2/17  Prototype highlighting code
	int xylab;
	double xlogmax, ylogmax;    // 8/12/17  Log axis scaling
	double xlogbase = 2.71828182845904523536028747135266250;   // 3;
	double ylogbase = 2.71828182845904523536028747135266250;   // 10;         // default values replaced by graph specific below

	wxString snum, gname, text;
	wxSize textsize;
	int roundcount;
	double ymax, ycalc;
	double xlogrange, ylogrange;

	FILE* ofp;
	TextFile opfile;
	//ofp = fopen("graph.txt", "w");
	//if(graphindex == 0) ofp = fopen("graph.txt", "w");

	//mainwin->diagbox->Write(text.Format("Graph Paint Call %d\n", graphindex));

	diag = mainwin->diagnostic;
	diag = 0;
	drawdiag = false;

	PaintBackground(dc);
	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	//wxFont textfont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));
	//wxFont smallfont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));

	xlabels = 10;
	ylabels = mainwin->ylabels;

	for(gdisp=0; gdisp<numdisps; gdisp++)
		for(gplot=0; gplot<dispset[gdisp]->numplots; gplot++) {
			//graph = gpos->plot[gplot];
			graph = dispset[gdisp]->plot[gplot];
			gpar = graph->gparam;
			if(gpar == -1) gdata = graph->gdata;
			if(gpar == -2) gdatad = graph->gdatad;
			if(gpar == -3) gdatav = graph->gdatav;
			if(gpar == -4) gdatadv = graph->gdatadv;
			xscale = graph->xscale;
			yscale = 1;
			xdis = graph->xdis;
			//xdis = 200;
			yfrom = graph->yfrom;
			yto = graph->yto;
			xfrom = graph->xfrom * xscale;
			xto = graph->xto * xscale;
			if(!graph->xlabels) graph->xlabels = 10;
			if(!graph->ylabels) graph->ylabels = mainwin->ylabels;
			if(!graph->xstep) graph->xstep = (xto - xfrom) / xlabels;
			if(!graph->ystep) graph->ystep = (yto - yfrom) / ylabels;
			xlabels = graph->xlabels;
			ylabels = graph->ylabels;
			gname = graph->gname;
			binsize = graph->binsize;
			gtype = graph->type;
			colour = graph->colour;
			colourpen[custom] = graph->strokecolour;
			sample = graph->samprate;
			if(graph->spikedata != NULL) burstdata = graph->spikedata->colourdata;
			else burstdata = NULL;
			xtime = xto - xfrom;
			barwidth = graph->barwidth;
			bargap = graph->bargap;
			//burstdata = graph->burstdata;
			//spikedata = graph->spikedata;
			xlogbase = graph->xlogbase;
			ylogbase = graph->ylogbase;

			//if(gtype == 3) ofp = fopen("graph.txt", "w");
			if(drawdiag && burstdata != NULL && burstdata->burstdisp == 1) ofp = fopen("graph.txt", "w");

			if(colour < 0 || colour > 9) colour = 3;

			gc->SetPen(colourpen[black]);
			DrawLine(dc, gc, xbase, ybase, xbase, ybase + yplot);
			DrawLine(dc, gc, xbase, ybase + yplot, xbase + xplot + xstretch, ybase + yplot);

			//gc->StrokeLine(xbase, ybase, xbase, ybase + yplot);
			//gc->StrokeLine(xbase, ybase + yplot, xbase + 550, ybase + yplot);

			// Ticks and Labels

			gc->SetFont(textfont, colourpen[black]);

			//int ticks;

			int xcoord, ycoord;
			double xplotstep, yplotstep;

			if(graph->xtickmode) { // && graph->xstep > 0) {
				xlabels = (int)((xto - xfrom) / (xscale * graph->xstep));
				xplotstep = (xplot * graph->xstep) / (xto - xfrom);
			}

			if(graph->xscalemode == 1 && xfrom > 0) xlogmax = log(xto / xfrom) / log(xlogbase);
			else xlogmax = 0;

			if(graph->yscalemode == 1 && yfrom > 0) ylogmax = log(yto / yfrom) / log(ylogbase);
			else ylogmax = 0;

			//V = Vmin * b ** (logmax * X / Xmax)
			//xval = xfrom * exp(xlogmax * 

			//if(graph->xscalemode == 1 && xfrom > 0) xpos = (int)((double)xplot * log(xval / xfrom) / xlogmax);  // log scaled x-axis  December 2017
			//else xpos = (xval - xfrom) * xrange;

			for(i=0; i<=xlabels && xlabels > 0; i++) {
				xcoord = i * xplot / xlabels;
				if (graph->xtickmode) xcoord = (int)(xplotstep * i);
				DrawLine(dc, gc, xbase + xcoord, ybase + yplot, xbase + xcoord, ybase + yplot + 5);
				if (!graph->xlabelmode || (graph->xlabelmode == 2 && i > 0 && i < xlabels)) continue;
				xval = ((double)(xto - xfrom) / xlabels * i + xfrom) / xscale * graph->xunitscale / graph->xunitdscale - graph->xshift;
				if (graph->xtickmode) xval = (xfrom + graph->xstep * i) * graph->xunitscale / graph->xunitdscale - graph->xshift;

				// log scale mode
				if(graph->xscalemode == 1 && xfrom > 0) xval = xfrom * pow(xlogbase, xlogmax * xval / xto);

				srangex = abs((xto - xfrom) / xscale * graph->xunitscale / graph->xunitdscale);
				//snum.Printf("%.0f", xval + xdis);	
				//if(srangex < 10) snum.Printf("%.1f", xval + xdis);	
				//if(srangex < 1) snum.Printf("%.2f", xval + xdis);
				//if(srangex < 0.1) snum.Printf("%.3f", xval + xdis);	

				if(graph->xlabelplaces == -1) {
					if (srangex < 0.1) snum.Printf("%.3f", xval + xdis);
					else if (srangex < 1) snum.Printf("%.2f", xval + xdis);
					else if (srangex < 10) snum.Printf("%.1f", xval + xdis);
					else snum.Printf("%.0f", xval + xdis);
				}
				else snum = numstring(xval + xdis, graph->xlabelplaces);


				if(ostype == Mac) {
					textsize = dc.GetTextExtent(snum);
					dc.DrawText(snum, xbase + xcoord - textsize.GetWidth() / 2, ybase + yplot + 10);
				}
				else {
					gc->GetTextExtent(snum, &textwidth, &textheight);
					gc->DrawText(snum, xbase + xcoord - textwidth / 2, ybase + yplot + 10);
				}
			}

			if(yplot < 150 && ylabels >= 10) gc->SetFont(smallfont, colourpen[black]);
			xylab = 5;

			if(graph->ytickmode && graph->ystep > 0) {                     // tick count and spacing for defined linear step size
				ylabels = (int)((yto - yfrom) / (yscale * graph->ystep));
				yplotstep = (yplot * graph->ystep) / (yto - yfrom);
			}

			if(graph->ytickmode == 1 && graph->yscalemode == 1) {          // tick count and spacing for log scale, step size specifies exponent step
				ylogrange = log(yto - yfrom) / log(ylogbase);
				ylabels = (int)(ylogrange / graph->ystep);
				yplotstep = (yplot * graph->ystep) / ylogrange;
				mod->diagbox->Write(text.Format("ylog tick step %.2f ylabels %d stepsize %.2f ylogrange %.4f\n", graph->ystep, ylabels, yplotstep, ylogrange));
			}

			for(i=0; i<=ylabels && ylabels>0; i+=1) {
				ycoord = i * yplot / ylabels;
				if(graph->ytickmode) ycoord = (int)(yplotstep * i);
				DrawLine(dc, gc, xbase, ybase + yplot - ycoord, xbase - 5, ybase + yplot - ycoord);
				if(!graph->ylabelmode || (graph->ylabelmode == 2 && i > 0 && i < ylabels)) continue;
				if(graph->ytickmode == 0) yval = ((double)(yto - yfrom) / ylabels * i + yfrom) / yscale * graph->yunitscale - graph->yshift;
				if(graph->ytickmode == 1) yval = (yfrom + graph->ystep * i) * graph->yunitscale - graph->yshift;

				// log scale mode
				if(graph->yscalemode == 1 && yfrom > 0) {
					if(graph->ytickmode == 1) yval = pow(ylogbase, graph->ystep * i);
					if(graph->ytickmode == 0) yval = yfrom * pow(ylogbase, ylogmax * yval / yto);
				}

				srangey = abs((yto - yfrom) / yscale * graph->yunitscale);
				if(graph->ylabelplaces == -1) {
					if (srangey < 0.1) snum.Printf("%.3f", yval);
					else if (srangey < 1) snum.Printf("%.2f", yval);
					else if (srangey < 10) snum.Printf("%.1f", yval);
					else snum.Printf("%.0f", yval);
				}
				else snum = numstring(yval, graph->ylabelplaces);
				
				if(ostype == Mac) {
					textsize = dc.GetTextExtent(snum);
					dc.DrawText(snum, xbase - xylab - textsize.GetWidth(), ybase + yplot - ycoord - textsize.GetHeight() / 2);
				}
				else {
					gc->GetTextExtent(snum, &textwidth, &textheight);
					gc->DrawText(snum, xbase - xylab - textwidth, ybase + yplot - ycoord - 7);
				}
			}
			//gname.Printf("GText width %d height %d", textsize.x, textsize.y);

			if(yplot < 150) gc->SetFont(textfont, colourpen[black]);
			gc->GetTextExtent(gname, &textwidth, &textheight);
			gc->DrawText(gname, xplot + 50 - textwidth, 30 + 15 * gplot);

			gc->SetPen(colourpen[colour]);
			highon = 0;

			// Set drawing scales
			xto /= binsize;
			xfrom /= binsize;

			yrange = (double)yplot / (yto - yfrom);
			xrange = (double)xplot / (xto - xfrom);
			xnum = (double)(xto - xfrom) / xplot;


			/* Plot Types

			1 - scaled width bars, histogram

			7 - scaled width bars

			4 - normal line graph

			5 - line graph with scaling fix

			2 - line graph with x data

			6 - line graph with sampling

			8 - scatter with sampling

			3 - spike rate data with optional burst colouring

			9 - bar chart with x data

			10 - scatter plot with x data

			11 - mean field plot

			*/

			//mainwin->diagbox->Write(text.Format("Graph %d %s type %d\n", gdisp, gname, gtype)); 

			//if(gdisp == 2) mainwin->diagbox->Write(text.Format("xrange %.4f  xrange int %d  xrange fix %d\n", xrange, (int)xrange, (int)(xrange + 0.5)));


			double prevx, xvalnext;
			int xposnext;

			double xmin, xmax, ymin, ymax, xmid, ymid;
			int scatterfield = graph->scatterfield;
			int scattermean = graph->scattermean;
			int width, height;
			double xmean, ymean, xvar, yvar, xsd, ysd;
			double xsdneg, xsdpos, ysdneg, ysdpos;
			int xoffset = 1;

			bool filediag = false;


			if(gtype == 1) {                             // scaled width bars, Histogram    
				for (i=0; i<(xto - xfrom); i++) {
					if(gpar == -1) y = (double)gdata[i + (int)xfrom];
					if(gpar == -2) y = gdatad[i + (int)xfrom];
					if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
					if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
					xpos = i * xrange + xbase;

					if(i == graph->highstart) {					// new highlighting code  9/2/17
						highon = 1;
						gc->SetPen(colourpen[graph->highcolour]);
					}
					if(i == graph->highstop) {
						highon = 0;
						gc->SetPen(colourpen[colour]);
					}

					if((int)xrange <= 1) {
						DrawLine(dc, gc, xpos, yplot + ybase, xpos, yplot + ybase - (int)(yrange * (y - yfrom)));
					}
					else {
						for(k=0; k<(int)(xrange-0.5); k++) {
							DrawLine(dc, gc, xpos + k, yplot + ybase, xpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
						}
					}
				}
			}


			if(gtype == 2 && graph->gdatax) {				                            // line graph with X data
		   																		//mainwin->diagbox->Write(text.Format("Graph Type 2  xcount %d xrange %.4f xplot %d\n", graph->xcount, xrange, xplot));
				oldx = xbase + xoffset;
				oldy = (int)(yplot + ybase - yrange * (yfrom));
				//mainwin->diagbox->Write(text.Format("\n XY graph maxindex %d xcount %d\n", graph->gdatax->maxindex, graph->xcount));
				for(i=0; i<graph->xcount; i++) {
					xval = (*graph->gdatax)[i];
					//if(xval <= prevx) break;
					if(xval >= xfrom && xval <= xto) {
						xpos = (int)((xval - xfrom) * xrange);
						y = (*gdatadv)[i];
						/*
						xposnext = (int)((*graph->gdatax)[i+1] - xfrom) * xrange;                  // proto code for scaling dense data for screen display 25/5/16
						while(xposnext == xpos) {
						i++;
						xposnext = (int)((*graph->gdatax)[i+1] - xfrom) * xrange;
						}*/

						//mainwin->diagbox->Write(text.Format("\n X %d val %.2f pos %d\n", i, xval, xpos));
						//mainwin->diagbox->Write(text.Format("XY graph line X %.4f Y %.4f\n", xval, y));

						gc->SetPen(colourpen[colour]);
						DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
						gc->SetPen(colourpen[black]);
						//if(graph->scattermode) dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), graph->scattersize);
						if(graph->scattermode) gc->DrawEllipse((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), graph->scattersize, graph->scattersize);
						oldx = xpos + xbase + xoffset;
						oldy = (int)(yplot + ybase - yrange * (y - yfrom));
					}
					prevx = xval;
				}
			}


			if(gtype == 3) {                             // spike rate data with optional burst colouring
				int spikestep = 0;
				int burstcolour = 0;
				if(filediag) opfile.New("spikegraph.txt");
				//for(i=0; i<10; i++) opfile.WriteLine(text.Format("spike %d  Burst time %.2f\n", i, burstdata->times[i]));
				if(filediag) opfile.WriteLine(text.Format("plot %s  binsize %.4f  xrange %.4f  gpar %d\n\n", graph->gname, binsize, xrange, gpar));

				for(i=0; i<(xto-xfrom); i++) {
					if(gpar == -1) y = (double)gdata[i + (int)xfrom];
					if(gpar == -2) y = gdatad[i + (int)xfrom];
					if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
					if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];

					res = 0;
					if(binsize == 1) res = 0;
					if(binsize == 0.1) res = 1;
					if(binsize == 0.01) res = 2;
					if(binsize == 0.001) res = 3;

					//wxString diag;
					//diag.Printf("spikedisp %d  res %d ", gpos->spikedisp, res);
					//mainwin->SetStatusText(diag);

					if(burstdata == NULL || burstdata->burstdisp == 0) {
						gc->SetPen(colourpen[red]);
						//mainwin->SetStatusText("no colour");
					}

					else {                  // burst colouring
						burstcolour = 0;
						//fprintf(ofp, "spikedisp 1\n");
						if(binsize == 0.1 || binsize == 0.01) {
							burstcolour = 0;
							//opfile.WriteLine("sub res");
							//if(burstdata[res][(i + (int)xfrom)] == 0) dc.SetPen(colourpen[red]);
							//else if(burstdata[res][(i + (int)xfrom)] % 2 == 0) dc.SetPen(colourpen[blue]);		
							//else if(burstdata[res][(i + (int)xfrom)] % 2 == 1) dc.SetPen(colourpen[green]);
						}

						if(binsize == 10 || binsize == 100) {
							timepoint = (xfrom + i + 1) * binsize * 1000;
							while (timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
								//while(burstdata->times[spikestep] < timepoint * binsize + 0.0005) {
								//opfile.WriteLine(text.Format("while  i %d  spike %d  time %.2f\n", i, spikestep, burstdata->times[spikestep]));
								if (!burstcolour) burstcolour = burstdata->spikes[spikestep];
								spikestep++;
							}
						}

						if(binsize == 1) {
							//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d\n", i, (i+xfrom)*binsize, spikestep);
							timepoint = (xfrom + i + 1) * binsize * 1000;
							//opfile.WriteLine(text.Format("i %d  timepoint %.4f  maxtime %.4f  time %.4f", i, timepoint, burstdata->maxtime, burstdata->times[spikestep]));
							//while ((timepoint - binsize) < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
							while(spikestep < burstdata->spikedata->spikecount && burstdata->times[spikestep] < timepoint + 0.0005) {
								//while(burstdata->times[spikestep] < timepoint * binsize + 0.0005) {
								//opfile.WriteLine(text.Format("while  i %d  spike %d  time %.2f", i, spikestep, burstdata->times[spikestep]));
								if(!burstcolour) burstcolour = burstdata->spikes[spikestep];
								spikestep++;
							}
							//fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);
							//opfile.WriteLine(text.Format("spike %d time %.2f\n", spikestep, burstdata->times[spikestep]));
							//for(i=0; i<10; i++) fprintf(ofp, "Burst time %.2f\n", burstdata->times[i]);
							//fprintf(ofp, "Burst time %.2f\n", burstdata->times[100]);
							//opfile.WriteLine(text.Format("i %d  spike %d  burstcolour %d", i, spikestep, burstcolour));
						}

						if(binsize == 0.001) {
							//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d\n", i, (i+xfrom)*binsize, spikestep);
							//timepoint = (xfrom + i) * binsize * 1000;
							timepoint = (xfrom + i + 0.5) * binsize * 1000;
							if(filediag) opfile.WriteLine(text.Format("i %d  timepoint %.4f  maxtime %.2f  time %.4f", i, timepoint, burstdata->maxtime, burstdata->times[spikestep]));
							//while(timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
							while(spikestep < burstdata->spikedata->spikecount && burstdata->times[spikestep] < timepoint - 0.0005) {
								//fprintf(ofp, "spike %d time %.10f comp %.10f y %.2f\n", 
								//	spikestep, burstdata->spikedata->times[spikestep], (i + xfrom) * binsize + 0.0005, y);
								if(filediag) opfile.WriteLine(text.Format("while  i %d  spike %d  time %.4f  timepoint %.4f", i, spikestep, burstdata->times[spikestep], timepoint+0.0005));
								burstcolour = burstdata->spikes[spikestep];
								spikestep++;
							}
							//fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);


							//if(i*binsize+0.0005 > graphdata[18][spikestep]) {
							//	burstcolour = graphdata[16][spikestep];
							//	spikestep++;
						}
						//fflush(ofp);
						if(burstcolour == 0)
							gc->SetPen(colourpen[red]);
						else if(burstcolour % 2 == 0)
							gc->SetPen(colourpen[blue]);
						else if(burstcolour % 2 == 1)
							gc->SetPen(colourpen[green]);
					}


					/*
					if(spikedisp == 2) {                      // hazard filter
					if(res < 3) {
					if(hfdata[res][(i + (int)xfrom)] == 0)
					dc.SelectObject(redpen);
					else dc.SelectObject(greenpen);
					}

					if(res == 3) {
					//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d ", i, (i+xfrom)*binsize, spikestep);
					while(graphdata[18][spikestep] < (i+xfrom)*binsize+0.0005) {
					burstcolour = filterflag[spikestep];
					spikestep++;
					}
					//fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);
					if(burstcolour == 0)
					dc.SelectObject(redpen);
					else if(burstcolour % 2 == 0)
					dc.SelectObject(bluepen);
					else if(burstcolour % 2 == 1)
					dc.SelectObject(greenpen);
					}
					}
					*/

					xpos = i * xrange + xbase;
					if(xrange <= 1)
						DrawLine(dc, gc, xpos, ybase + yplot, xpos, ybase + yplot - (int)(yrange * (y - yfrom)));
					else {
						for(k=0; k<xrange-1; k++)
							DrawLine(dc, gc, xpos + k, ybase + yplot, xpos + k, ybase + yplot - (int)(yrange * (y - yfrom)));
					}

					if(filediag && y > 0) opfile.WriteLine(text.Format("xpos %d  y %.2f  spike %d  burstcolour %d", xpos, y, spikestep, burstcolour));
				}
				if(filediag) opfile.Close();
			}


			if(gtype == 4) {                         // normal line graph   - no longer in use, see type 5 below
				dir = 1;   // 1 for ascending, 0 for descending
				preval = (*gdatadv)[0];
				oldx = xbase;
				oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

				//mod->diagbox->Write(text.Format("line plot xrange %.4f yscalemode %d\n", xrange, graph->yscalemode));

				// subpixel scale drawing mode - drawing data in limited x-axis resolution, xrange gives ratio of plot pixels to data points, use this mode if xrange < 1
				//
				// attempt to preserve maxima and minima
				// 'dir' gives current direction of plot progression
				// 'xnum' gives number of data points for current pixel position, reciprocal of xrange
				// choose lowest or highest data point for plot value depending on direction

				//if(graph->xscalemode == 1 && xfrom > 0) xpos = (int)((double)xplot * (log(xval / xfrom) / log(xlogbase)) / xlogmax);  // log scaled x-axis  December 2017
				//else xpos = (xval - xfrom) * xrange;

				for(i=0; i<xplot; i++) {
					if(xrange < 1) {
						xindex = (int)((i * xnum) + xfrom);
						mpoint = (*gdatadv)[xindex];
						for (j=1; j<xnum; j++) {
							if(dir) {
								if((*gdatadv)[xindex + j] > mpoint) mpoint = (*gdatadv)[xindex + j];
							}
							else {
								if((*gdatadv)[xindex + j] < mpoint) mpoint = (*gdatadv)[xindex + j];
							}
						}
						if(preval < mpoint) dir = 1; else dir = 0;
						yval = mpoint;
						preval = mpoint;

						if(graph->yscalemode == 1 && yfrom > 0) ypos = (int)((double)yplot * (log(yval / yfrom) / log(ylogbase)) / ylogmax);  // log scaled y-axis  March 2018
						else ypos = (yval - yfrom) * yrange;
						DrawLine(dc, gc, oldx, oldy, i + xbase, (int)(yplot + ybase - ypos));
						oldx = i + xbase;
						oldy = (int)(yplot + ybase - ypos);
					}
					else {
						yval = (*gdatadv)[(int)(i + xfrom)];
						if (graph->yscalemode == 1 && yfrom > 0) ypos = (int)((double)yplot * (log(yval / yfrom) / log(ylogbase)) / ylogmax);  // log scaled y-axis  March 2018
						else ypos = (yval - yfrom) * yrange;
						DrawLine(dc, gc, oldx, oldy, (int)(i * xrange + xbase), (int)(yplot + ybase - ypos));
						oldx = i * xrange + xbase;
						oldy = (int)(yplot + ybase - ypos);
					}
					//fprintf(pofp, "xindex %d  y %.2f\n  dir %d", xindex, y, dir);
				}
			}


			if(gtype == 5) {                         // line graph with scaling fix
				if(drawdiag) ofp = fopen("graph.txt", "w");
				//mod->diagbox->Write(text.Format("line plot xrange %.4f  yscalemode %d  ylogbase %.4f  ylogmax %.4f\n", xrange, graph->yscalemode, ylogbase, ylogmax));

				dir = 1;
				pdir = 0;
				xindex = (int)xfrom;
				if(gpar == -3) preval = (*gdatav)[xindex];
				if(gpar == -4) preval = (*gdatadv)[xindex];
				oldx = xbase + xoffset;
				oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

				for(i=1; i<xplot; i++) {
					if(xrange < 1) {
						xindex = (int)((i * xnum) + xfrom);
						//mpoint = (*gdatadv)[xindex];
						if(gpar == -3) mpoint = (*gdatav)[xindex];
						if(gpar == -4) mpoint = (*gdatadv)[xindex];
						if(drawdiag) fprintf(ofp, "xdraw %d  preval %.4f  dir %d\n", i, preval, dir);
						for(j=1; j<xnum; j++) {
							//data = (*gdatadv)[xindex + j];
							if(gpar == -3) data = (*gdatav)[xindex + j];
							if(gpar == -4) data = (*gdatadv)[xindex + j];
							if(drawdiag) fprintf(ofp, "xdraw %d, xnum %d, data %.4f\n", i, j, data);
							if(dir) {
								if(data > mpoint) mpoint = data;
								else if(data < mpoint) mpoint = data;
								//if(!dir && (*gdatadv)[xindex + j] < mpoint) mpoint = (*gdatadv)[xindex + j];
							}
						}
						if(preval <= mpoint || preval < 0.000001) dir = 1; else dir = 0;
						yval = mpoint;
						preval = mpoint;
						if(drawdiag) fprintf(ofp, "xdraw %d  preval %.4f  mpoint %.4f  point %.4f\n", i, preval, mpoint, y);

						if(graph->yscalemode == 1 && yfrom > 0) {
							ypos = (int)((double)yplot * (log(yval / yfrom) / log(ylogbase)) / ylogmax);  // log scaled y-axis  March 2018
							if(yval < yfrom) {
								ypos = -yfrom * yrange;
								//mod->diagbox->Write(text.Format("line draw log low value yval %.4f ypos %d\n", yval, ypos));
							}
						}
						else ypos = (yval - yfrom) * yrange;
						DrawLine(dc, gc, oldx, oldy, i + xbase + xoffset, (int)(yplot + ybase - ypos));
						oldx = i + xbase + xoffset;
						oldy = (int)(yplot + ybase - ypos);

						//DrawLine(dc, gc, oldx, oldy, i + xbase + xoffset, (int)(yplot + ybase - yrange * (y - yfrom)));	
						//oldx = i + xbase + xoffset;
						//oldy = (int)(yplot + ybase - yrange * (y - yfrom));
					}
					else {
						//yval = (*gdatadv)[(int)(i + xfrom)];
						if(gpar == -3) yval = (*gdatav)[(int)(i + xfrom)];
						if(gpar == -4) yval = (*gdatadv)[(int)(i + xfrom)];
						if(graph->yscalemode == 1 && yfrom > 0) {
							ypos = (int)((double)yplot * (log(yval / yfrom) / log(ylogbase)) / ylogmax);  // log scaled y-axis  March 2018
							if(yval < yfrom) ypos = -yfrom * yrange;
						}
						else ypos = yrange * (yval - yfrom);
						DrawLine(dc, gc, oldx, oldy, (int)(i * xrange + xbase + xoffset), (int)(yplot + ybase - ypos));
						oldx = i * xrange + xbase + xoffset;
						oldy = (int)(yplot + ybase - ypos);
					}
					//fprintf(pofp, "xindex %d  y %.2f\n  dir %d", xindex, y, dir);
				}
				if(drawdiag) fclose(ofp);
			}


			if(gtype == 7) {                             // scaled width bars    
				for (i = 0; i < (xto - xfrom); i++) {
					if(gpar == -1) y = (double)gdata[i + (int)xfrom];
					if(gpar == -2) y = gdatad[i + (int)xfrom];
					if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
					if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
					xpos = i * xrange + xbase;
					if(xrange <= 1) {
						DrawLine(dc, gc, xpos, yplot + ybase, xpos, yplot + ybase - (int)(yrange * (y - yfrom)));
					}
					else {
						for(k=0; k<xrange; k++) {
							DrawLine(dc, gc, xpos + k, yplot + ybase, xpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
						}
					}
				}
			}


			if(gtype == 10 && graph->gdatax) {	           // scatter graph with X data and optional range fields
				//mainwin->diagbox->Write(text.Format("Graph Type 10  xcount %d xrange %.4f xplot %d\n", graph->xcount, xrange, xplot));
				//mainwin->diagbox->Write(text.Format("\n XY graph maxindex %d xcount %d\n", graph->gdatax->maxindex, graph->xcount));

				//if(graph->xscalemode == 1 && xfrom > 0) xlogmax = log(xto / xfrom) / log(logbase);
				xmean = 0;
				ymean = 0;
				for(i=0; i<graph->xcount; i++) {
					xval = (*graph->gdatax)[i];
					xmean += xval / graph->xcount;
					yval = (*gdatadv)[i];
					ymean += yval / graph->xcount;
					if(xval >= xfrom && xval <= xto) {
						if(graph->xscalemode == 1 && xfrom > 0) xpos = (int)((double)xplot * (log(xval / xfrom) / log(xlogbase)) / xlogmax);  // log scaled x-axis  December 2017
						else xpos = (xval - xfrom) * xrange;
						ypos = (yval - yfrom) * yrange;
						if(i == 0) {
							xmin = xpos;
							xmax = xpos;
							ymin = ypos;
							ymax = ypos;
						}
						else {
							if (xpos < xmin) xmin = xpos;
							if (xpos > xmax) xmax = xpos;
							if (ypos < ymin) ymin = ypos;
							if (ypos > ymax) ymax = ypos;
						}
						gc->SetPen(colourpen[colour]);
						//dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (yval - yfrom)), graph->scattersize);	
						gc->DrawEllipse((int)(xpos + xbase + xoffset), (int)(yplot + ybase - ypos), graph->scattersize, graph->scattersize);
					}
				}

				if(graph->xcount > 0 && scattermean) {
					xvar = 0;
					yvar = 0;
					for (i = 0; i < graph->xcount; i++) {
						xvar += ((*graph->gdatax)[i] - xmean) * ((*graph->gdatax)[i] - xmean);
						yvar += ((*graph->gdatadv)[i] - ymean) * ((*graph->gdatadv)[i] - ymean);
					}
					xsd = sqrt(xvar / graph->xcount);
					ysd = sqrt(yvar / graph->xcount);
					if (graph->xscalemode == 1 && xfrom > 0) {
						xpos = (int)((double)xplot * (log(xmean / xfrom) / log(xlogbase)) / xlogmax);
						xsdneg = (int)((double)xplot * (log((xmean - xsd) / xfrom) / log(xlogbase)) / xlogmax);
						xsdpos = (int)((double)xplot * (log((xmean + xsd) / xfrom) / log(xlogbase)) / xlogmax);
					}
					else {
						xpos = (xmean - xfrom) * xrange;
						xsdneg = (xmean - xsd - xfrom) * xrange;
						xsdpos = (xmean + xsd - xfrom) * xrange;
					}
					ypos = (ymean - yfrom) * yrange;
					ysdneg = (ymean - ysd - yfrom) * yrange;
					ysdpos = (ymean + ysd - yfrom) * yrange;

					gc->SetPen(colourpen[black]);
					gc->SetBrush(*wxBLACK_BRUSH);
					gc->DrawEllipse((int)(xpos + xbase + xoffset), (int)(yplot + ybase - ypos), 3, 3);
					DrawLine(dc, gc, (int)(xsdneg + xbase + xoffset), (int)(yplot + ybase - ypos), (int)(xsdpos + xbase + xoffset), (int)(yplot + ybase - ypos));
					DrawLine(dc, gc, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - ysdneg), (int)(xpos + xbase + xoffset), (int)(yplot + ybase - ysdpos));
				}

				if(graph->xcount > 0 && scatterfield) {
					xmid = (xmin + xmax) / 2;
					ymid = (ymin + ymax) / 2;
					width = (double)(xmax - xmin) * 1.5;
					height = (double)(ymax - ymin) * 1.5;
					gc->SetBrush(*wxTRANSPARENT_BRUSH);
					gc->SetPen(colourpen[colour]);
					gc->DrawEllipse((int)(xmid + xbase + xoffset - width / 2), (int)(yplot + ybase - ymid - height / 2), width, height);
				}
			}

			double xfield, yfield;

			if(gtype == 11 && graph->gdatax && graph->xcount > 1) {         // mean field plot - oval with centre at xy mean and xy StdDev dimensions      December 2017
				//mainwin->diagbox->Write(text.Format("Graph Type 11  xcount %d xrange %.4f xplot %d\n", graph->xcount, xrange, xplot));
				//mainwin->diagbox->Write(text.Format("\n XY graph maxindex %d xcount %d\n", graph->gdatax->maxindex, graph->xcount));

				if (graph->xscalemode == 1 && xfrom > 0) xlogmax = log(xto / xfrom);
				xval = (*graph->gdatax)[0];
				yval = (*gdatadv)[0];
				xfield = (*graph->gdatax)[1];
				yfield = (*gdatadv)[1];
				width = xfield * xrange * 2;
				height = yfield * yrange * 2;

				if(graph->xscalemode == 1 && xfrom > 0) {
					xpos = (int)((double)xplot * log(xval / xfrom) / xlogmax);
					//xfield = 
				}
				else xpos = (int)((xval - xfrom) * xrange);
				gc->SetPen(colourpen[colour]);
				gc->SetBrush(*wxTRANSPARENT_BRUSH);
				gc->DrawEllipse((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (yval - yfrom)), 10, 10);
				gc->DrawEllipse((int)(xpos + xbase + xoffset - width), (int)(yplot + ybase - yrange * (yval - yfrom) - height), width * 2, height * 2);
			}


			// xrange = (double)xplot / (xto - xfrom);  code copy comment for reference

			int barshift = graph->barshift;
			int barpos;

			if(gtype == 9 && graph->gdatax) {				                            // bar chart with X data
				//mainwin->diagbox->Write(text.Format("\n XY bar graph maxindex %d xcount %d\n", graph->gdatax->maxindex, graph->xcount));
				for(i=0; i<graph->xcount; i++) {
					xval = (*graph->gdatax)[i];
					//if(xval <= prevx) break;
					if(xval >= xfrom && xval <= xto) {
						xpos = (int)(xval - xfrom) * xrange;
						barshift = (barwidth * numdisps + (numdisps - 1) * bargap) / 2;
						barpos = xbase + xpos - barshift + gdisp * (barwidth + bargap);
						y = (*gdatadv)[i];
						//mainwin->diagbox->Write(text.Format("\n XY graph line X %.4f Y %.4f\n", xval, y));
						gc->SetPen(colourpen[colour]);
						//DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
						for (k = 0; k < barwidth; k++) {
							DrawLine(dc, gc, barpos + k, yplot + ybase, barpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
						}
						//dc.SetPen(colourpen[black]);
						//if(graph->scattermode) dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), graph->scattersize);			
					}
				}
			}

			/*
			if(gtype == 7) {                             // scaled width bars
			for(i=0; i<(xto - xfrom); i++) {
			if(gpar == -1) y = (double)gdata[i + (int)xfrom];
			if(gpar == -2) y = gdatad[i + (int)xfrom];
			if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
			if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
			xpos = i * xrange + xbase;
			if(xrange <= 1) {
			DrawLine(dc, gc, xpos, yplot + ybase, xpos, yplot + ybase - (int)(yrange * (y - yfrom)));
			}
			else {
			for(k=0; k<xrange; k++) {
			DrawLine(dc, gc, xpos + k, yplot + ybase, xpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
			}
			}
			}
			}*/


			if(gtype == 6) {                         // line graph with sampling
				oldx = xbase + xoffset;
				xindex = (int)xfrom / sample;
				if(xfrom > 0) {
					if((int)xfrom % sample != 0) {
						double sydiff = (*gdatadv)[xindex] - (*gdatadv)[xindex - 1];
						double sxdiff = (xfrom - xindex * sample) / sample;
						preval = sydiff * sxdiff + (*gdatadv)[xindex - 1];
					}
					else preval = (*gdatadv)[xindex - 1];
				}
				else preval = (*gdatadv)[0];

				//if(mainwin->diagnostic && xindex > 0) mod->diagbox->textbox->AppendText(text.Format("xindex %d xpredata %.4f preval %.4f\n\n", xindex, (*gdatadv)[xindex-1], preval)); 

				oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

				for(i=0; i<=(xto-xfrom)/sample; i++) {
					xindex = i + ceil(xfrom / sample);
					xpos = (int)(xindex * sample - xfrom) * xrange;
					mpoint = (*gdatadv)[xindex];
					y = mpoint;
					//dc.LineTo(xindex + 100 + xmove, (int)(ymove + 130.0 - yrange * (y - yfrom)));
					DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
					oldx = xpos + xbase + xoffset;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				}
			}

			if(gtype == 8) {                         // scatter with sampling
				wxBrush brush(wxColour(255, 255, 255));
				gc->SetBrush(brush);
				//dc.SetPen(colourpen[black]);

				oldx = xbase + xoffset;
				xindex = (int)xfrom / sample;
				if (xfrom > 0) {
					if ((int)xfrom % sample != 0) {
						double sydiff = (*gdatadv)[xindex] - (*gdatadv)[xindex - 1];
						double sxdiff = (xfrom - xindex * sample) / sample;
						preval = sydiff * sxdiff + (*gdatadv)[xindex - 1];
					}
					else preval = (*gdatadv)[xindex - 1];
				}
				else preval = (*gdatadv)[0];

				//if(mainwin->diagnostic && xindex > 0) mod->diagbox->textbox->AppendText(text.Format("xindex %d xpredata %.4f preval %.4f\n\n", xindex, (*gdatadv)[xindex-1], preval)); 

				oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

				gc->SetPen(colourpen[colour]);
				for(i=0; i<=(xto-xfrom)/sample; i++) {
					xindex = i + ceil(xfrom / sample);
					xpos = (int)(xindex * sample - xfrom) * xrange;
					mpoint = (*gdatadv)[xindex];
					y = mpoint;
					//dc.LineTo(xindex + 100 + xmove, (int)(ymove + 130.0 - yrange * (y - yfrom)));
					DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
					//dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), 2);
					oldx = xpos + xbase + xoffset;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				}

				double xtodraw = xto;
				if(graph->xstop && xto > graph->xstop) xtodraw = graph->xstop;

				gc->SetPen(colourpen[black]);
				for(i=0; i<=(xtodraw-xfrom)/sample; i++) {
					xindex = i + ceil(xfrom / sample);
					xpos = (int)(xindex * sample - xfrom) * xrange;
					mpoint = (*gdatadv)[xindex];
					y = mpoint;
					//dc.LineTo(xindex + 100 + xmove, (int)(ymove + 130.0 - yrange * (y - yfrom)));
					//DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
					gc->DrawEllipse((int)(xpos + xbase - 2), (int)(yplot + ybase - yrange * (y - yfrom) - 2), 4, 4);
					//dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), 2);
					oldx = xpos + xbase + xoffset;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				}
			}

		}
	//if(gtype == 3) fclose(ofp);
	//if(gpos->spikedisp == 1) fclose(ofp);
	if(drawdiag && burstdata != NULL && burstdata->burstdisp == 1) fclose(ofp);
	//fclose(ofp);
}


/*
void GraphWindow3::OnPaintGC(wxPaintEvent &WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);

	int gdex, gpar, gtype;
	int i, j, k, dir;
	int colour, res;
	int xpos, ypos, xgpos, ygpos, xindex;
	double xfrom, xto, yfrom, yto;
	double y, xrange, yrange, xnum;
	double srangex, srangey;
	double preval, mpoint;
	int *gdata;
	double *gdatad;
	datint *gdatav;
	datdouble *gdatadv;
	double binsize;
	int xbase, ybase, xbasefix;
	int xview, yview, oldx, oldy;
	int gplot;
	double xval, xscale;
	double textwidth, textheight;
	double textdescent, textlead;

	wxString snum, gname;
	wxColour colourpen[10];
	wxSize textsize;

	int roundcount;
	double ymax, ycalc;

	FILE *ofp;
	//ofp = fopen("graph.txt", "w");

	PaintBackground(dc);
	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	colourpen[0].Set(wxT("#000000"));       // 0 black
	colourpen[1].Set(wxT("#F50000"));       // 1 red
	colourpen[2].Set(wxT("#00F500"));       // 2 green
	colourpen[3].Set(wxT("#0000F5"));       // 3 blue
	colourpen[4].Set(wxT("#F5F500"));       // 4 yellow
	colourpen[5].Set(wxT("#F500F5"));       // 5 purple
	colourpen[6].Set(wxT("#FF8080"));       // 6 light red
	colourpen[7].Set(wxT("#80FF80"));       // 7 light green
	colourpen[8].Set(wxT("#8080FF"));       // 8 light blue

	//wxFont textfont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));
	//wxFont textfont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("Helvetica"));
	//wxFont smallfont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));
	//wxFont smallfont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("Helvetica"));
    
    wxFont textfont(wxFontInfo(8).FaceName("Helvetica"));
    wxFont smallfont(wxFontInfo(6).FaceName("Helvetica"));

	for(gplot=0; gplot<gpos->numplots; gplot++) {
		graph = gpos->plot[gplot];
		gpar = graph->gparam;
		if(gpar == -1) gdata = graph->gdata;
		if(gpar == -2) gdatad = graph->gdatad;
		if(gpar == -3) gdatav = graph->gdatav;
		if(gpar == -4) gdatadv = graph->gdatadv;
		xscale = graph->xscale;
		if(xscale < 0) {                // use of xscale to switch on log scaling
			xscale = 1;
			graph->xscalemode = 1;}
		else graph->xscalemode = 0;
		yfrom = graph->yfrom;
		yto = graph->yto;
		xfrom = graph->xfrom * xscale;
		xto = graph->xto * xscale;
		gname = graph->gname;
		binsize = graph->binsize;
		gtype = graph->type;
		colour = graph->colour;

		if(gtype == 3) ofp = fopen("graph.txt", "w");

		if(colour < 0 || colour > 8) colour = 3;

		xbase = 40;
		ybase = 10;

		//dc.SetPen(colourpen[black]);
		gc->SetPen(colourpen[black]);

		//dc.DrawLine(xbase, ybase, xbase, ybase + yplot);
		//dc.DrawLine(xbase, ybase + yplot, xbase + 550, ybase + yplot);
		gc->StrokeLine(xbase, ybase, xbase, ybase + yplot);
		gc->StrokeLine(xbase, ybase + yplot, xbase + 550, ybase + yplot);



		// Ticks and Labels
		//dc.SetFont(textfont);
		//gc->SetFont(textfont, *wxBLACK);
		gc->SetFont(textfont, colourpen[black]);

		for(i=0; i<=10; i++) {
			gc->StrokeLine(xbase + i*50, ybase + yplot, xbase + i*50, ybase + yplot + 5);
			xval = ((double)(xto - xfrom) / 10*i + xfrom) / xscale;
			srangex = (xto - xfrom) / xscale;
			snum.Printf(wxT("%.0f"), xval);	
			if(xto - xfrom < 10) snum.Printf(wxT("%.1f"), xval);	
			if(xto - xfrom < 1) snum.Printf(wxT("%.2f"), xval);
			if(xto - xfrom < 0.1) snum.Printf(wxT("%.3f"), xval);	
			gc->GetTextExtent(snum, &textwidth, &textheight, &textdescent, &textlead);
			//dc.DrawText(snum, xbase + i*50 - textsize.GetWidth()/2, ybase + yplot + 10);
			gc->DrawText(snum, xbase + i*50 - textwidth/2, ybase + yplot + 10);
		}

		if(yplot < 150) dc.SetFont(smallfont);

		for(i=0; i<=10; i+=1) {
			gc->StrokeLine(xbase, ybase + yplot - i*yplot/10, xbase - 5, ybase + yplot - i*yplot/10);
			if(yto - yfrom < 0.1)
				snum.Printf(wxT("%.3f"), (double)(yto - yfrom) / 10*i + yfrom);
			else if(yto - yfrom < 1)
				snum.Printf(wxT("%.2f"), (double)(yto - yfrom) / 10*i + yfrom);
			else if(yto - yfrom < 10)
				snum.Printf(wxT("%.1f"), (double)(yto - yfrom) / 10*i + yfrom);
			else
				snum.Printf(wxT("%.0f"), (double)(yto - yfrom) / 10*i + yfrom);
			//textsize = gc->GetTextExtent(snum);
			gc->GetTextExtent(snum, &textwidth, &textheight, &textdescent, &textlead);
			//dc.DrawText(snum, xbase - 10 - textsize.GetWidth(), ybase + yplot - i*yplot/10 - 7);
			gc->DrawText(snum, xbase - 10 - textwidth, ybase + yplot - i*yplot/10 - 7);
		}

		if(yplot < 150) dc.SetFont(textfont);
		//textsize = dc.GetTextExtent(gname);
		gc->GetTextExtent(gname, &textwidth, &textheight, &textdescent, &textlead);
		gc->DrawText(gname, 550 - textwidth, 30 + 15 * gplot); 

		gc->SetPen(colourpen[colour]);

		// Set drawing scales
		xto /= binsize;
		xfrom /= binsize;

		yrange = (double)yplot / (yto - yfrom);
		xrange = (double)xplot / (xto - xfrom); 
		xnum = (double)(xto - xfrom) / xplot;

		if(gtype == 1) {                             // scaled width bars, Histogram    
			for(i=0; i<(xto - xfrom); i++) {
				if(gpar == -1) y = (double)gdata[i + (int)xfrom];
				if(gpar == -2) y = gdatad[i + (int)xfrom];
				if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
				if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
				xpos = i * xrange + xbase;
				if(xrange <= 1) {
					gc->StrokeLine(xpos, yplot + ybase, xpos, yplot + ybase - (int)(yrange * (y - yfrom)));
				}
				else {
					for(k=0; k<xrange-1; k++) {
						gc->StrokeLine(xpos + k, yplot + ybase, xpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));
					}
				}
			}
		}

		if(gtype == 5) {   
			dir = 1;
			preval = (*gdatadv)[0];
			oldx = xbase;
			oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

			for(i=0; i<xplot; i++) {
				if(xrange < 1) {
					xindex = (int)((i * xnum) + xfrom);
					mpoint = (*gdatadv)[xindex];
					for(j=1; j<xnum; j++) {
						if(dir) { 
							if((*gdatadv)[xindex + j] > mpoint) mpoint = (*gdatadv)[xindex + j];
						}
						else {
							if((*gdatadv)[xindex + j] < mpoint) mpoint = (*gdatadv)[xindex + j];
						}
					}
					if(preval < mpoint) dir = 1; else dir = 0;
					y = mpoint;
					preval = mpoint;
					gc->StrokeLine(oldx, oldy, i + xbase, (int)(yplot + ybase - yrange * (y - yfrom)));	
					oldx = i + xbase;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				} 
				else {
					y = (*gdatadv)[(int)(i + xfrom)];
					gc->StrokeLine(oldx, oldy, (int)(i*xrange + xbase), (int)(yplot + ybase - yrange * (y - yfrom)));
					oldx = i*xrange + xbase;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				}
				//fprintf(pofp, "xindex %d  y %.2f\n  dir %d", xindex, y, dir);
			}
		}

		if(gtype == 3) {                             // spike rate data with optional burst colouring
			int spikestep = 0;
			int burstcolour = 0;

			for(i=0; i<(xto - xfrom); i++) {
				if(gpar == -1) y = (double)gdata[i + (int)xfrom];
				if(gpar == -2) y = gdatad[i + (int)xfrom];	
				if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
				if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];

				if(binsize == 1) res = 0;
				if(binsize == 0.1) res = 1;
				if(binsize == 0.01) res = 2;
				if(binsize == 0.001) res = 3;

				//wxString diag;
				//diag.Printf(wxT("spikedisp %d"), gpos->spikedisp);
				//mainwin->SetStatus(diag);

				if(gpos->spikedisp == 0 || burstdata->numbursts == 0) dc.SetPen(colourpen[red]);

				if(gpos->spikedisp == 1) {                  // burst colouring
					burstcolour = 0;
					//fprintf(ofp, "spikedisp 1\n");
					if(res < 3) {
						burstcolour = 0;
						//fprintf(ofp, "sub res\n");
						//if(burstdata[res][(i + (int)xfrom)] == 0) dc.SetPen(colourpen[red]);
						//else if(burstdata[res][(i + (int)xfrom)] % 2 == 0) dc.SetPen(colourpen[blue]);		
						//else if(burstdata[res][(i + (int)xfrom)] % 2 == 1) dc.SetPen(colourpen[green]);
					}

					if(res == 0) {
						//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d\n", i, (i+xfrom)*binsize, spikestep);
						while(burstdata->spikedata->times[spikestep] < (i + xfrom + 1) * binsize + 0.0005) {
							//fprintf(ofp, "spike %d time %.2f\n", spikestep, burstdata->spikedata->times[spikestep]);
							if(!burstcolour) burstcolour = burstdata->spikes[spikestep];
							spikestep++;
						}
						//fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);
					}

					if(res == 3) {
						//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d\n", i, (i+xfrom)*binsize, spikestep);
						while(burstdata->spikedata->times[spikestep] < (i + xfrom) * binsize + 0.0005) {
							//fprintf(ofp, "spike %d time %.10f comp %.10f y %.2f\n", 
							//	spikestep, burstdata->spikedata->times[spikestep], (i + xfrom) * binsize + 0.0005, y);
							burstcolour = burstdata->spikes[spikestep];
							spikestep++;
						}
						//fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);


						//if(i*binsize+0.0005 > graphdata[18][spikestep]) {
						//	burstcolour = graphdata[16][spikestep];
						//	spikestep++;
					}
					if(burstcolour == 0) 	
						dc.SetPen(colourpen[red]);
					else if(burstcolour % 2 == 0)
						dc.SetPen(colourpen[blue]);
					else if(burstcolour % 2 == 1)
						dc.SetPen(colourpen[green]);	
				}


				
				//if(spikedisp == 2) {                      // hazard filter
				//if(res < 3) {
				//if(hfdata[res][(i + (int)xfrom)] == 0)
				//dc.SelectObject(redpen);
				//else dc.SelectObject(greenpen);
				//}

				//if(res == 3) {
				////fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d ", i, (i+xfrom)*binsize, spikestep);
				//while(graphdata[18][spikestep] < (i+xfrom)*binsize+0.0005) {
				//burstcolour = filterflag[spikestep];
				//spikestep++;
				//}
				////fprintf(ofp, "spikestep after = %d burstcolour = %d y = %.2f\n", spikestep, burstcolour, y);	
				//if(burstcolour == 0) 
				//dc.SelectObject(redpen);
				//else if(burstcolour % 2 == 0) 
				//dc.SelectObject(bluepen);
				//else if(burstcolour % 2 == 1) 
				//dc.SelectObject(greenpen);
				//}		
				//}
				


				xpos = i * xrange + xbase;
				if(xrange <= 1) 
					gc->StrokeLine(xpos, ybase + yplot, xpos, ybase + yplot - (int)(yrange * (y - yfrom)));
				else {
					for(k=0; k<xrange-1; k++) 
						gc->StrokeLine(xpos + k, ybase + yplot, xpos + k, ybase + yplot - (int)(yrange * (y - yfrom))); 
				}	

			}
		}	
	}
	if(gtype == 3) fclose(ofp);
}
*/

/*
// Conversion form HSV to RGB (obtained from http://alvyray.com/Papers/hsv2rgb.htm)
#define RETURN_HSV(h, s, v) {HSV.H = h; HSV.S = s; HSV.V = v; return HSV;}
#define RETURN_RGB(r, g, b) {RGB.R = r; RGB.G = g; RGB.B = b; return RGB;}
#define UNDEFINED -1
// Theoretically, hue 0 (pure red) is identical to hue 6 in these transforms. Pure
// red always maps to 6 in this implementation. Therefore UNDEFINED can be
// defined as 0 in situations where only unsigned numbers are desired.

typedef struct {float R, G, B;} RGBType;
typedef struct {float H, S, V;} HSVType;

RGBType HSV_to_RGB(HSVType HSV)
{
// H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
// RGB are each returned on [0, 1].
float h = HSV.H, s = HSV.S, v = HSV.V, m, n, f;
int i;
RGBType RGB;

if(h==UNDEFINED)
RETURN_RGB(v, v, v);
i = floor(h);
f = h-i;
if(!(i&1))
f = 1-f; // if i is even
m = v*(1-s);
n = v*(1-s*f);

switch(i) {
case 6:
case 0: RETURN_RGB(v, n, m);
case 1: RETURN_RGB(n, v, m);
case 2: RETURN_RGB(m, v, n)
case 3: RETURN_RGB(m, n, v);
case 4: RETURN_RGB(n, m, v);
case 5: RETURN_RGB(v, m, n);
}
}


void GraphWindow3::OnPaintGCdemo(wxPaintEvent &WXUNUSED(event))
{
wxPaintDC dc(this);

const double base = 80.0;  // sizes used in shapes drawn below
const double base2 = base/2.0;
const double base4 = base/4.0;

wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
font.SetWeight(wxBOLD);
gc->SetFont(font, *wxBLACK);

// make a path that contains a circle and some lines, centered at 0,0
wxGraphicsPath path = gc->CreatePath();
path.AddCircle(0, 0, base2);
path.MoveToPoint(0, -base2);
path.AddLineToPoint(0, base2);
path.MoveToPoint(-base2, 0);
path.AddLineToPoint(base2, 0);
path.CloseSubpath();
path.AddRectangle(-base4, -base4/2, base2, base4);

// Now use that path to demonstrate various capbilites of the grpahics context
gc->PushState();             // save current translation/scale/other state
gc->Translate(60, 75);       // reposition the context origin

gc->SetPen(wxPen(wxT("navy"), 1));
gc->SetBrush(wxBrush(wxT("pink")));

// show the difference between stroking, filling and drawing
wxDouble w, h, d, l;
gc->GetTextExtent(wxT("StrokePath"), &w, &h, &d, &l);
gc->DrawText(wxT("StrokePath"), -w/2, -base2-h-4);
gc->StrokePath(path);
gc->Translate(2*base, 0);

gc->GetTextExtent(wxT("FillPath"), &w, &h, &d, &l);
gc->DrawText(wxT("FillPath"), -w/2, -base2-h-4);
gc->FillPath(path);
gc->Translate(2*base, 0);

gc->GetTextExtent(wxT("DrawPath"), &w, &h, &d, &l);
gc->DrawText(wxT("DrawPath"), -w/2, -base2-h-4);
gc->DrawPath(path);
gc->Translate(2*base, 0);

gc->PopState();              // restore saved state
gc->PushState();             // save it again
*//*
gc->Translate(60, 200);      // offset to the lower part of the window

gc->DrawText(wxT("Scale"), 0, -base2);
gc->Translate(0, 20);

// for testing clipping
//gc->Clip(0, 0, 100, 100)
//rgn = wx.RegionFromPoints([ (0,0), (75,0), (75,25,), (100, 25),
//                           (100,100), (0,100), (0,0)  ])
//gc->ClipRegion(rgn);
//gc->ResetClip();

gc->SetBrush(wxBrush(wxColour(178,  34,  34, 128)));   // 128 == half transparent
for(int i=0; i<8; i++) {
gc->Scale(1.08, 1.08);    // increase scale by 8%
gc->Translate(5,5);    
gc->DrawPath(path);
}

gc->PopState();              // restore saved state
gc->PushState();             // save it again
gc->Translate(400, 200);
gc->DrawText(wxT("Rotate"), 0, -base2);

gc->Translate(0, 75);
HSVType hsv;
RGBType rgb;
for(double angle=0.0; angle<360.0; angle+=30.0) {
gc->PushState();         // save this new current state so we can pop back to
// it at the end of the loop
hsv.H = 6.0*angle/360.0;
hsv.S = 1.0;
hsv.V = 1.0;
rgb = HSV_to_RGB(hsv);
gc->SetBrush(wxBrush(wxColour((int)(rgb.R*255), (int)(rgb.G*255), (int)(rgb.B*255), 64)));

// use translate to artfully reposition each drawn path
gc->Translate(1.5 * base2 * cos(angle/180.0*M_PI),
1.5 * base2 * sin(angle/180.0*M_PI));

// use Rotate to rotate the path
gc->Rotate(angle/180.0*M_PI);

// now draw it
gc->DrawPath(path);
gc->PopState();
}


gc->PopState();
} 
*/



