

//#include "hypomodel.h"
//#include "hypopanels.h"
#include "wx/graphics.h"
#include <hypoprint.h>
#include <hypomodel.h>


DispWin::DispWin(HypoMain *main, const wxString& title, const wxPoint& pos, const wxSize& size, int start, int numgraphs)
	: wxFrame(main, wxID_ANY, title, pos, size,
	wxFRAME_TOOL_WINDOW | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER)
{
	int graph;

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


GraphWindow3::GraphWindow3(HypoMain *main, wxFrame *parent, Model *model, wxPoint pos, wxSize size, graphdisp *gdisp, int index)
	: wxPanel(parent, wxID_ANY, pos, size)
{
	mainwin = main;
	drawframe = parent;
	gpos = gdisp;
	graphindex = index;
	mod = model;

	int max, scrollxto;
	double xdiff;
	wxString snum, outline, text;
	int i, j, gindex;
	GraphSet *graphset;
	wxMenu *newsub;

	ostype = GetSystem();

	yplot = 200;
	xplot = 500;
	max = 1440;
	xbase = 40;
	ybase = 10;
	xstretch = mainwin->xstretch;

	numgraphs = 0;
	currentgraph = 0;
	spikedisp = 0;
	gsynch = 1;
	
	selectband = false;

	colourpen = mainwin->colourpen;

	//overlay = new wxOverlay();

	if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("\ngraphwindow %d\n", graphindex));

	menuPlot = new wxMenu;
	if(mainwin->diagnostic) {
		menuPlot->Append(ID_GraphRemove, "Delete Graph");
		menuPlot->Append(ID_GraphPrint, "Print Graph");
		menuPlot->Append(ID_GraphEPS, "Graph EPS");
		menuPlot->Append(ID_Scale, "Adjust Axes");
		menuPlot->AppendSeparator();
	}
	for(i=0; i<mod->graphbase->numsets; i++) {
		menuPlot->AppendRadioItem(1000 + i, mod->graphbase->setstore[i].name);
		//if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("menu set Index %d Name %s\n", 1000+i, mod->graphbase->setstore[i].name));
		/*graphset = mod->graphbase->GetSet(i);
		if(graphset->single) 
		menuPlot->AppendRadioItem(1000 + i, mod->graphbase->setstore[i].name);
		else {
		newsub = new wxMenu;
		for(j=0; j<graphset->numgraphs; j++) {
		//newsub->Append(wxID_ANY, mod->graphbase->graphset
		newsub->AppendRadioItem(wxID_ANY, (*mod->graphbase)[graphset->gindex[j]]->gname);
		}
		menuPlot->AppendSubMenu(newsub, mod->graphbase->setstore[i].name); 
		}*/
	}



	//menuPlot->Check(1000 + gpos->GetFront()->index, true);
	//snum.Printf("plot0 = %d", gpos->plot[0]->index);
	//gindex = gpos->plot[0]->index;
	//graph = gpos->plot[0];
	//gindex = graph->gindex;
	//	//mainwin->SetStatusText(snum);
	//parent->SetStatusText(snum);
	//menuPlot->Check(1005, true);


	//menuPlot->AppendRadioItem(1000 + 2, "Plot2" );
	//menuPlot->AppendRadioItem(1000 + 3, "Plot3" );

	//graph = gpos->plot[0];
	//outline.Printf("GWin3 plot 0 title %s gparam %d", graph->gname, graph->gparam);
	//((MainFrame *)mainwin)->wxofp->AddLine(outline);

	//if(graph->gparam == -3) {
	//graph->gdatav->max = 500;
	//	max = graph->gdatav->max / graph->xscale;
	//}
	//if(graph->gparam == -4) max = graph->gdatadv->max / graph->xscale;


	//xdiff = graph->xto - graph->xfrom;

	//xdiff = 500;
	//scrollxto = max - (int)xdiff + 100;	 
	//outline.Printf("xdiff  %.2f  max %d  XTo %d", xdiff, max, scrollxto);
	//((MainFrame *)mainwin)->wxofp->AddLine(outline);

	SetBackgroundColour("WHITE");

	if(ostype == Mac) {
		textfont = wxFont(10, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Helvetica");
		smallfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Helvetica");
	}
	else {
		textfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Helvetica");
		smallfont = wxFont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Helvetica");
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

	Connect(wxEVT_PAINT, wxPaintEventHandler(GraphWindow3::OnPaint));
	//Connect(wxEVT_PAINT, wxPaintEventHandler(GraphWindow3::OnPaintGC));
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
	Connect(1000, 1000 + mod->graphbase->numgraphs - 1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraph));
	Connect(ID_GraphRemove, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphRemove));
	Connect(ID_GraphPrint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphPrint));
	Connect(ID_GraphEPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnGraphEPS));
	Connect(ID_Scale, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GraphWindow3::OnScale));
}


GraphWindow3::~GraphWindow3()
{
	//delete overlay;
	delete menuPlot;
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

void GraphWindow3::OnGraphPrint(wxCommandEvent& event)
{
	wxString text; 
	GraphDat *graph;

	if(mod->diagbox) mod->diagbox->textbox->AppendText(text.Format("Print Graph %d\n", graphindex));

	graph = graphset[0]->plot[0];

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
	if(!mainwin->graphbox) mainwin->graphbox = new GraphBox(this, "Axis Panel");
	else mainwin->graphbox->SetGraph(this);
	mainwin->graphbox->Show(true);
}


void GraphWindow3::OnGraph(wxCommandEvent& event)
{
	short id = event.GetId();
	double xfrom, xto;
	int modesum = -1;
	GraphSet *graphset;
	int gdex;

	snum.Printf("Graph Set Select ID %d\n", id);
	//mainwin->SetStatusText(snum);
	if(mod->diagbox) mod->diagbox->textbox->AppendText(snum);
	xfrom = gpos->GetFront()->xfrom;
	xto = gpos->GetFront()->xto;

	graphset = mod->graphbase->GetSet(id-1000);
	gdex = graphset->GetPlot(mainwin->scalebox->GetFlags());
	gpos->Front((*mod->graphbase)[gdex]);
	gpos->sdex = graphset->sdex;
	//gpos->Front((*mod->graphbase)[id-1000]);

	gpos->GetFront()->xfrom = xfrom;
	gpos->GetFront()->xto = xto;
	//mod->gcodes[graphindex] = mod->graphbase->GetTag(id-1000);
	mod->gcodes[graphindex] = mod->graphbase->GetSetTag(id-1000);
	mainwin->scalebox->ScaleUpdate();
	//Refresh();
}


wxRealPoint GraphWindow3::GraphPos(wxPoint)
{
	int i;

	return wxRealPoint(0, 0);
}


void GraphWindow3::OnLeftDown(wxMouseEvent &event)
{
	wxPoint pos = event.GetPosition();
	mousedown = pos;

	snum.Printf("LDown %d", pos.x);
	if(mainwin->diagnostic) mainwin->SetStatusText(snum);

	int x,y,xx,yy ;
  //event.GetPosition(&x,&y);
  //CalcUnscrolledPosition( x, y, &xx, &yy );
  anchorpos = pos;
  currentpos = anchorpos;
  selectband = true;
  //CaptureMouse() ;
}


void GraphWindow3::OnLeftUp(wxMouseEvent &event)
{
	double xdiff, xscale, xgraphFrom, xgraphTo;

	int xplaces;


	// Graph select

	if(mainwin->graphbox) mainwin->graphbox->SetGraph(this);

	// Data Selection drag 

	wxPoint pos = event.GetPosition();
	if(pos.x - mousedown.x > 5) {
		graph = gpos->plot[0];

		xdiff = graph->xto - graph->xfrom;
		xscale = xdiff / xplot;
		xplaces = numplaces(xdiff);
		xgraphFrom = (mousedown.x - xbase) * xscale + graph->xfrom;
		xgraphTo = (pos.x - xbase) * xscale + graph->xfrom;
		if(xgraphFrom < graph->xfrom) xgraphFrom = graph->xfrom;
		if(xgraphTo > graph->xto) xgraphTo = graph->xto;

		snum.Printf("LUp %d  drag From %s To %s", pos.x, numstring(xgraphFrom, xplaces), numstring(xgraphTo, xplaces));
		mod->DataSelect(xgraphFrom, xgraphTo);
	}
	else snum.Printf("LUp %d", pos.x);

	if(mainwin->diagnostic) mainwin->SetStatusText(snum);

	selectband = false;
	//ReleaseMouse();
	{wxClientDC dc(this);
	PrepareDC(dc);
	wxDCOverlay overlaydc(overlay, &dc);
	overlaydc.Clear();}
  overlay.Reset();
}


void GraphWindow3::OnRightClick(wxMouseEvent& event)
{
	wxString text;
	//int id = event.GetId();
	//wxWindow *pos = FindWindowById(id, toolpanel);
	//wxPoint point = this->GetPosition();

	wxPoint pos = event.GetPosition();
	//wxSize size = this->GetSize();

	//mainwin->diagbox->textbox->AppendText(text.Format("graph menu set %d\n", gpos->GetFront()->sdex));

	menuPlot->Check(1000 + gpos->sdex, true);
	mainwin->diagbox->Write(text.Format("graph menu set %d\n", gpos->sdex));
	PopupMenu(menuPlot, pos.x + 20, pos.y);
}


void GraphWindow3::AddGraph(graphdisp *newgraph)
{
	if(numgraphs < 5) {
		graphset[numgraphs] = newgraph;
		numgraphs++;
	}
}


void GraphWindow3::FrontGraph(graphdisp *newgraph)
{
	graphset[0] = newgraph;
	if(numgraphs == 0) numgraphs = 1;
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
	if(graph->gparam == -4) max = graph->gdatadv->max / graph->xscale;
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

	if(selectband) {
		int x,y, xx, yy;
		event.GetPosition(&x,&y);
		//CalcUnscrolledPosition( x, y, &xx, &yy );
	  currentpos = pos;
		if(currentpos.y > ybase + yplot) currentpos.y = ybase + yplot;
		anchorpos.y = ybase - 10;
		currentpos.y = ybase + yplot;
		wxRect newrect(anchorpos, currentpos);
		wxClientDC dc(this);
		PrepareDC(dc);
		//wxDCOverlay overlaydc(overlay, &dc, xbase, ybase, xplot, yplot);
		wxDCOverlay overlaydc(overlay, &dc);
		overlaydc.Clear();
#ifdef __WXMAC__
		dc.SetPen(*wxGREY_PEN);
		dc.SetBrush(wxColour(192,192,192,64));
#else
		dc.SetPen(wxPen(*wxBLUE, 2));
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		//dc.SetBrush( *wxBLUE_BRUSH );
#endif
		dc.DrawRectangle(newrect);
	}
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

	for(plotindex=0; plotindex<numgraphs; plotindex++) {
		graph = graphset[plotindex]->plot[0];
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
	dc.DrawLine(xstart, ystart, xend, yend);

	//gc->StrokeLine(xbase, ybase, xbase, ybase + yplot);
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
	int gplot;
	int ylabels, xlabels;
	double xval, xscale, xdis;
	double yval, yscale;
	double timepoint, maxtime;
	double data;
	int pdir, sample, xtime;
	int xlabelgap;
	//SpikeDat *spikedata;
	int diag;
	bool drawdiag;
	int xylab;

	wxString snum, gname, text;
	wxSize textsize;
	int roundcount;
	double ymax, ycalc;

	FILE *ofp;
	TextFile opfile;
	//ofp = fopen("graph.txt", "w");
	//if(graphindex == 0) ofp = fopen("graph.txt", "w");

	diag = mainwin->diagnostic;
	diag = 0;
	drawdiag = false;

	PaintBackground(dc);
	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	//wxFont textfont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));
	//wxFont smallfont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));

	xlabels = 10;
	ylabels = mainwin->ylabels;

	for(gplot=0; gplot<numgraphs; gplot++) {
		//graph = gpos->plot[gplot];
		graph = graphset[gplot]->plot[0];
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
		//burstdata = graph->burstdata;
		//spikedata = graph->spikedata;

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

		if(graph->xtickmode && graph->xstep > 0) {
			xlabels = (int)((xto - xfrom) / (xscale * graph->xstep));
			xplotstep = (xplot * graph->xstep) / (xto - xfrom);  
		}

		for(i=0; i<=xlabels && xlabels > 0; i++) {
			xcoord = i * xplot / xlabels;
			if(graph->xtickmode) xcoord = (int)(xplotstep * i);
			dc.DrawLine(xbase + xcoord, ybase + yplot, xbase + xcoord, ybase + yplot + 5);
			xval = ((double)(xto - xfrom) / xlabels*i + xfrom) / xscale * graph->xunitscale - graph->xshift;
			if(graph->xtickmode) xval = (xfrom + graph->xstep * i) * graph->xunitscale - graph->xshift;
			srangex = (xto - xfrom) / xscale * graph->xunitscale;
			snum.Printf("%.0f", xval + xdis);	
			if(srangex < 10) snum.Printf("%.1f", xval + xdis);	
			if(srangex < 1) snum.Printf("%.2f", xval + xdis);
			if(srangex < 0.1) snum.Printf("%.3f", xval + xdis);	
			textsize = dc.GetTextExtent(snum);
			if(ostype == Mac)
				dc.DrawText(snum, xbase + xcoord - textsize.GetWidth()/2, ybase + yplot + 10);
			else
				dc.DrawText(snum, xbase + xcoord - textsize.GetWidth()/2, ybase + yplot + 10);
		}

		if(yplot < 150 && ylabels >= 10) dc.SetFont(smallfont);
		xylab = 5;

		if(graph->ytickmode && graph->ystep > 0) {
			ylabels = (int)((yto - yfrom) / (yscale * graph->ystep));
			yplotstep = (yplot * graph->ystep) / (yto - yfrom);  
		}

		for(i=0; i<=ylabels && ylabels > 0; i+=1) {
			ycoord = i * yplot / ylabels;
			if(graph->ytickmode) ycoord = (int)(yplotstep * i);
			dc.DrawLine(xbase, ybase + yplot - ycoord, xbase - 5, ybase + yplot - ycoord);
			yval = ((double)(yto - yfrom) / ylabels*i + yfrom) / yscale;
			if(graph->ytickmode) yval = yfrom + graph->ystep * i;
			if(yto - yfrom < 0.1) snum.Printf("%.3f", yval);
			else if(yto - yfrom < 1) snum.Printf("%.2f", yval);
			else if(yto - yfrom < 10) snum.Printf("%.1f", yval);
			else snum.Printf("%.0f", yval);
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

		if(gtype == 4) {                         // normal line graph
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
					DrawLine(dc, gc, oldx, oldy, i + xbase, (int)(yplot + ybase - yrange * (y - yfrom)));	
					oldx = i + xbase;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				} 
				else {
					y = (*gdatadv)[(int)(i + xfrom)];
					DrawLine(dc, gc, oldx, oldy, (int)(i*xrange + xbase), (int)(yplot + ybase - yrange * (y - yfrom)));
					oldx = i*xrange + xbase;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				}
				//fprintf(pofp, "xindex %d  y %.2f\n  dir %d", xindex, y, dir);
			}
		}

		int xoffset = 1;

		if(gtype == 5) {                         // line graph with scaling fix
			if(drawdiag) ofp = fopen("graph.txt", "w");
			dir = 1;
			pdir = 0;
			xindex = (int)xfrom;
			preval = (*gdatadv)[xindex];
			oldx = xbase + xoffset;
			oldy = (int)(yplot + ybase - yrange * (preval - yfrom));

			for(i=1; i<xplot; i++) {
				if(xrange < 1) {
					xindex = (int)((i * xnum) + xfrom);
					mpoint = (*gdatadv)[xindex];
					if(drawdiag) fprintf(ofp, "xdraw %d  preval %.4f  dir %d\n", i, preval, dir);
					for(j=1; j<xnum; j++) {
						data = (*gdatadv)[xindex + j];
						if(drawdiag) fprintf(ofp, "xdraw %d, xnum %d, data %.4f\n", i, j, data);
						if(dir)
							if(data > mpoint) mpoint = data;
							else if(data < mpoint) mpoint = data;
							//if(!dir && (*gdatadv)[xindex + j] < mpoint) mpoint = (*gdatadv)[xindex + j];
					}
					if(preval <= mpoint || preval < 0.000001) dir = 1; else dir = 0;
					y = mpoint;
					preval = mpoint;
					if(drawdiag) fprintf(ofp, "xdraw %d  preval %.4f  mpoint %.4f  point %.4f\n", i, preval, mpoint, y);
					DrawLine(dc, gc, oldx, oldy, i + xbase + xoffset, (int)(yplot + ybase - yrange * (y - yfrom)));	
					oldx = i + xbase + xoffset;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				} 
				else {
					y = (*gdatadv)[(int)(i + xfrom)];
					DrawLine(dc, gc, oldx, oldy, (int)(i*xrange + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
					oldx = i*xrange + xbase + xoffset;
					oldy = (int)(yplot + ybase - yrange * (y - yfrom));
				}
				//fprintf(pofp, "xindex %d  y %.2f\n  dir %d", xindex, y, dir);
			}
			if(drawdiag) fclose(ofp);
		}


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

			dc.SetPen(colourpen[black]);
			for(i=0; i<=(xto - xfrom) / sample; i++) {		
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

				if(binsize == 1) res = 0;
				if(binsize == 0.1) res = 1;
				if(binsize == 0.01) res = 2;
				if(binsize == 0.001) res = 3;

				wxString diag;
				diag.Printf("spikedisp %d  res %d ", gpos->spikedisp, res);
				mainwin->SetStatusText(diag);

				if(burstdata == NULL || burstdata->burstdisp == 0) {
					dc.SetPen(colourpen[red]);
					//mainwin->SetStatusText("no colour");
				}

				else {                  // burst colouring
					burstcolour = 0;
					//fprintf(ofp, "spikedisp 1\n");
					if(res > 0 && res < 3) {
						burstcolour = 0;
						//opfile.WriteLine("sub res");
						//if(burstdata[res][(i + (int)xfrom)] == 0) dc.SetPen(colourpen[red]);
						//else if(burstdata[res][(i + (int)xfrom)] % 2 == 0) dc.SetPen(colourpen[blue]);		
						//else if(burstdata[res][(i + (int)xfrom)] % 2 == 1) dc.SetPen(colourpen[green]);
					}

					if(res == 0) {
						//fprintf(ofp, "i = %d, time = %.3f, spikestep before = %d\n", i, (i+xfrom)*binsize, spikestep);
						timepoint = (xfrom + i + 1) * binsize * 1000;
						while(timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
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

					if(res == 3) {
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
	wxFont textfont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("Helvetica"));
	//wxFont smallfont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif"));
	wxFont smallfont(6, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("Helvetica"));

	for(gplot=0; gplot<gpos->numplots; gplot++) {
		graph = gpos->plot[gplot];
		gpar = graph->gparam;
		if(gpar == -1) gdata = graph->gdata;
		if(gpar == -2) gdatad = graph->gdatad;
		if(gpar == -3) gdatav = graph->gdatav;
		if(gpar == -4) gdatadv = graph->gdatadv;
		xscale = graph->xscale;
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
/*
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



