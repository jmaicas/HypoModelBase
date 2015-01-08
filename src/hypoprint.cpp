

#include "wx/graphics.h"
#include "wx/metafile.h"
#include <hypoprint.h>
#include <hypomodel.h>


void GraphWindow3::PrintEPS()
{
	TextFile out;
	GraphDat *graph;

	int i, j, k;
	wxString snum, gname, text;
	wxString filepath, filetag, filename;

	int ylabels, xlabels;
	double xval, xscale, xdis, xtime, xshift;
	double yval, yscale;
	double xfrom, xto, yfrom, yto;
	double xfromAxis, xtoAxis;
	double srangex, srangey;
	double binsize, bargap;
	int gtype, sample, gpar;
	int xplot, yplot, xstretch;
	double xbase, ybase;
	wxSize textsize;
	int xylab, res;
	datint *gdatav;
	datdouble *gdatadv;
	double timepoint, maxtime;
	double y, xrange, yrange, xnum;
	double xpos, ypos;
	int xoffset, xindex;
	double oldx, oldy, mpoint, preval;
	double axisstroke, plotstroke;
	//wxColour colour;
	int colour;

	if(mod->diagbox) mod->diagbox->textbox->AppendText(text.Format("Graph EPS %d\n", graphindex));

	// General plot parameters
	xbase = 100; 
	ybase = 100;
	yplot = 200;
	xplot = 500;
	xstretch = mainwin->xstretch;
	xlabels = 10;
	ylabels = mainwin->ylabels;
	xoffset = 1;
	axisstroke = 0.75;

	// Get Graph parameters
	graph = graphset[0]->plot[0];
	gpar = graph->gparam;
	xscale = graph->xscale;
	xshift = graph->xshift;
	yscale = 1;
	xdis = graph->xdis;
	yfrom = graph->yfrom;
	yto = graph->yto;
	xfrom = graph->xfrom * xscale;
	xto = graph->xto * xscale;
	gname = graph->gname;
	binsize = graph->binsize;
	gtype = graph->type;
	colour = graph->colour;
	sample = graph->samprate;
	xtime = xto - xfrom;
	plotstroke = graph->plotstroke;
	xplot = graph->xplot; 
	yplot = graph->yplot; 
	xlabels = graph->xlabels;
	ylabels = graph->ylabels;

	//xfrom = xfrom - xstart;                    // shift x-axis for non-zero start on data (to make 0 in figure)
	//xto = xto - xstart;

	// Set graph data pointers
	if(gpar == -3) gdatav = graph->gdatav;
	if(gpar == -4) gdatadv = graph->gdatadv;
	if(graph->spikedata != NULL) burstdata = graph->spikedata->burstdata;
	else burstdata = NULL;

	// Set output file path

	filepath = mainwin->outpath;
	//filetag = paramstoretag->GetValue();
	filetag = "test";
	filename = filepath + "/" + filetag + "-" + gname + ".eps";

	// Initialise postscript file and write header
	//out.New("C:/Users/Duncan/Desktop/plot.eps");
	out.New(filename);
	out.WriteLine("%!PS-Adobe-3.0 EPSF-3.0");
	out.WriteLine("%%BoundingBox: 0 0 1000 500");
	out.WriteLine("/pu {1 mul} def");                        // pu = plot units, set scaling to points
	out.WriteLine("/Helvetica findfont 10 scalefont setfont");
	out.WriteLine("0 0 0 setrgbcolor");
	out.WriteLine("1 setlinecap");
	out.WriteLine("1 setlinejoin");
	out.WriteLine("");

	out.WriteLine(text.Format("%s setrgbcolor", ColourString(colourpen[colour])));   

	// Set drawing scales
	xtoAxis = xto;
	xfromAxis = xfrom;
	xto /= binsize;
	xfrom /= binsize;

	yrange = (double)yplot / (yto - yfrom);
	xrange = (double)xplot / (xto - xfrom); 
	xnum = (double)(xto - xfrom) / xplot;

	// Draw graph data

	out.WriteLine(text.Format("%.2f setlinewidth", plotstroke));

	if(gtype == 7) {                             // scaled width bars
		if(mod->diagbox) mod->diagbox->textbox->AppendText(text.Format("Drawing type 7, scaled bars\n"));
		for(i=0; i<(xto - xfrom); i++) {
			if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
			if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
			if(y == 0) continue;
			xpos = i * xrange + xbase;
			out.WriteLine("newpath");
			out.MoveTo(xpos, ybase);
			out.LineTo(xpos, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange, ybase);
			out.WriteLine("closepath");
			out.WriteLine("fill");
			out.WriteLine("newpath");
			out.MoveTo(xpos, ybase);
			out.LineTo(xpos, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange, ybase);
			out.WriteLine("closepath");
			out.WriteLine("stroke");
		}
	}

	if(gtype == 1) {                             // scaled width bars, Histogram    
		if(mod->diagbox) mod->diagbox->textbox->AppendText(text.Format("Drawing type 0, histogram\n"));
		bargap = 1.5;
		if(xrange < 3) bargap = 0;
		if(xrange < 5) bargap = 1;
		//out.WriteLine("0.5 setlinewidth");

		for(i=0; i<(xto - xfrom); i++) {
			if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
			if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
			xpos = i * xrange + xbase;

			//if(xrange <= 1) DrawLine(dc, gc, xpos, yplot + ybase, xpos, yplot + ybase - (int)(yrange * (y - yfrom)));
			//else for(k=0; k<xrange-1; k++) DrawLine(dc, gc, xpos + k, yplot + ybase, xpos + k, yplot + ybase - (int)(yrange * (y - yfrom)));	

			out.WriteLine("newpath");
			out.MoveTo(xpos, ybase);
			out.LineTo(xpos, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange - bargap, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange - bargap, ybase);
			out.WriteLine("closepath");
			out.WriteLine("fill");
			
			out.WriteLine("newpath");
			out.MoveTo(xpos, ybase);
			out.LineTo(xpos, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange - bargap, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange - bargap, ybase);
			out.WriteLine("closepath");
			out.WriteLine("stroke");
		}
	}

	if(gtype == 3) {                             // spike rate data with optional burst colouring
		if(mod->diagbox) mod->diagbox->textbox->AppendText(text.Format("Drawing type 3, spike rate\n"));
		int spikestep = 0;
		int burstcolour = 0;
		bargap = 1.5;
		if(xrange < 3) bargap = 0;
		if(xrange < 5) bargap = 1;
		//out.WriteLine("0.5 setlinewidth");

		for(i=0; i<(xto - xfrom); i++) {
			if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
			if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];
			if(y == 0) continue;

			if(binsize == 1) res = 0;
			if(binsize == 0.1) res = 1;
			if(binsize == 0.01) res = 2;
			if(binsize == 0.001) res = 3;

			if(burstdata == NULL || burstdata->burstdisp == 0) out.SetColour(ColourString(colourpen[red]));

			else {                  // burst colouring
				burstcolour = 0;
				if(res > 0 && res < 3) burstcolour = 0;
				if(res == 0) {
					timepoint = (xfrom + i + 1) * binsize * 1000;
					while(timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
						if(!burstcolour) burstcolour = burstdata->spikes[spikestep];
						spikestep++;
					}
				}
				if(res == 3) {
					timepoint = (xfrom + i) * binsize * 1000;
					while(timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
						burstcolour = burstdata->spikes[spikestep];
						spikestep++;
					}
				}			
				if(burstcolour == 0)
					out.SetColour(ColourString(colourpen[red]));
				else if(burstcolour % 2 == 0)
					out.SetColour(ColourString(colourpen[blue]));
				else if(burstcolour % 2 == 1)
					out.SetColour(ColourString(colourpen[green]));
			}

			xpos = i * xrange + xbase;
			out.WriteLine("newpath");
			out.MoveTo(xpos, ybase);
			out.LineTo(xpos, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange - bargap, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange - bargap, ybase);
			out.WriteLine("closepath");
			out.WriteLine("stroke");	

			out.WriteLine("newpath");
			out.MoveTo(xpos, ybase);
			out.LineTo(xpos, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange - bargap, ybase + yrange * (y - yfrom));
			out.LineTo(xpos + xrange - bargap, ybase);
			out.WriteLine("closepath");
			out.WriteLine("fill");	
		}	
	}

	if(gtype == 8) {                         // scatter with sampling
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
		oldy = ybase + yrange * (preval - yfrom);

		out.WriteLine("newpath");
		out.WriteLine(text.Format("%s setrgbcolor", ColourString(colourpen[colour]))); 
		for(i=0; i<=(xto - xfrom) / sample; i++) {		
			xindex = i + ceil(xfrom / sample);
			xpos = (int)(xindex * sample - xfrom) * xrange;
			mpoint = (*gdatadv)[xindex];
			y = mpoint;
			//dc.LineTo(xindex + 100 + xmove, (int)(ymove + 130.0 - yrange * (y - yfrom)));
			out.DrawLine(oldx, oldy, xpos + xbase + xoffset, ybase + yrange * (y - yfrom));
			//dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), 2);
			oldx = xpos + xbase + xoffset;
			oldy = ybase + yrange * (y - yfrom);
		}
		out.WriteLine("stroke");

		out.WriteLine(text.Format("%s setrgbcolor", ColourString(colourpen[black]))); 
		for(i=0; i<=(xto - xfrom) / sample; i++) {		
			xindex = i + ceil(xfrom / sample);
			xpos = (xindex * sample - xfrom) * xrange;
			mpoint = (*gdatadv)[xindex];
			y = mpoint;
			//dc.LineTo(xindex + 100 + xmove, (int)(ymove + 130.0 - yrange * (y - yfrom)));
			//DrawLine(dc, gc, oldx, oldy, (int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)));
			//dc.DrawCircle((int)(xpos + xbase + xoffset), (int)(yplot + ybase - yrange * (y - yfrom)), 2);
			out.WriteLine("newpath");
			out.WriteLine(text.Format("%.2f pu %.2f pu %.2f pu 0 350 arc", xpos + xbase + xoffset, ybase + yrange * (y - yfrom), 2.0));
			out.WriteLine("stroke");
			oldx = xpos + xbase + xoffset;
			oldy = ybase + yrange * (y - yfrom);
		}
	}

	if(gtype == 4 || gtype == 5) {                         // line graph
		xoffset = 0;
		out.WriteLine("newpath");
		out.WriteLine(text.Format("%s setrgbcolor", ColourString(colourpen[colour]))); 

		oldx = xbase;
		oldy = ybase + yrange * ((*gdatadv)[xfrom] - yfrom);            // TODO proper start coordinates

		for(i=1; i<=(xto - xfrom); i++) {		
			xindex = i + xfrom;
			xpos = (xindex - xfrom) * xrange;
			y = (*gdatadv)[xindex];
			out.DrawLine(oldx, oldy, xpos + xbase + xoffset, ybase + yrange * (y - yfrom));
			oldx = xpos + xbase + xoffset;
			oldy = ybase + yrange * (y - yfrom);
		}
		out.WriteLine("stroke");
	}


	// Draw Axes

	xto = xtoAxis;
	xfrom = xfromAxis;

	ybase = ybase - (axisstroke / 2);    // offset to account for line width

	out.WriteLine(text.Format("%s setrgbcolor", ColourString(colourpen[black]))); 

	out.WriteLine(text.Format("%.2f setlinewidth", axisstroke));
	out.WriteLine("newpath");
	
	out.DrawLine(xbase, ybase, xbase, ybase + yplot);
	out.DrawLine(xbase, ybase, xbase + xplot + xstretch, ybase);
	

	out.WriteLine("");
	out.WriteLine("");

	// Draw Ticks

	double xcoord, ycoord;
	double xplotstep, yplotstep;

	if(graph->xtickmode && graph->xstep > 0) {
		xlabels = (int)((xto - xfrom) / (xscale * graph->xstep));
		xplotstep = (xplot * graph->xstep) / (xto - xfrom);  
	}

	for(i=0; i<=xlabels && xlabels > 0; i++) {
		xcoord = i * xplot / xlabels;
		if(graph->xtickmode) xcoord = xplotstep * i;
		out.DrawLine(xbase + xcoord, ybase, xbase + xcoord, ybase - 5);
	}

	if(graph->ytickmode && graph->ystep > 0) {
		ylabels = (int)((yto - yfrom) / (yscale * graph->ystep));
		yplotstep = (yplot * graph->ystep) / (yto - yfrom);  
	}

	for(i=0; i<=ylabels && ylabels > 0; i++) {
		ycoord = i * yplot / ylabels;
		if(graph->ytickmode) ycoord = yplotstep * i;
		out.DrawLine(xbase, ybase + ycoord, xbase - 5, ybase + ycoord);
	}

	//for(i=0; i<=ylabels; i+=1) out.DrawLine(xbase, ybase + i*yplot/ylabels, xbase - 5, ybase + i*yplot/ylabels);

	out.WriteLine("stroke");

	// Draw Labels

	for(i=0; i<=xlabels && xlabels > 0; i++) {
		out.WriteLine("newpath");
		xcoord = i * xplot / xlabels;
		if(graph->xtickmode) xcoord = xplotstep * i;
		xval = ((double)((xto - xfrom) / xlabels*i + xfrom) / xscale) * graph->xunitscale - xshift;
		if(graph->xtickmode) xval = (graph->xstep * i) * graph->xunitscale - xshift;
		srangex = (xto - xfrom) / xscale * graph->xunitscale;
		snum.Printf("%.0f", xval + xdis);	
		if(srangex < 10) snum.Printf("%.1f", xval + xdis);	
		if(srangex < 1) snum.Printf("%.2f", xval + xdis);
		if(srangex < 0.1) snum.Printf("%.3f", xval + xdis);	
		out.MoveTo(xbase + xcoord, ybase - 17);
		out.WriteLine(text.Format("(%s) dup stringwidth pop 2 div neg 0 rmoveto show", snum));
		out.WriteLine("stroke");
	}

	//if(yplot < 150 && ylabels >= 10) dc.SetFont(smallfont);
	xylab = 8;

	out.WriteLine("newpath");
	for(i=0; i<=ylabels; i+=1) {
		ycoord = i * yplot / ylabels;
		if(graph->ytickmode) ycoord = yplotstep * i;
		yval = ((double)(yto - yfrom) / ylabels*i + yfrom) / yscale;
		if(graph->ytickmode) yval = graph->ystep * i;
		if(yto - yfrom < 0.1) snum.Printf("%.3f", yval);
		else if(yto - yfrom < 1) snum.Printf("%.2f", yval);
		else if(yto - yfrom < 10) snum.Printf("%.1f", yval);
		else snum.Printf("%.0f", yval);
		out.MoveTo(xbase - xylab, ybase + ycoord - 2.75);
		out.WriteLine(text.Format("(%s) dup stringwidth pop neg 0 rmoveto show", snum));
	}
	out.WriteLine("stroke");

	//if(yplot < 150) dc.SetFont(textfont);

	//textsize = dc->GetTextExtent(gname);
	//dc->DrawText(gname, xplot + 50 - textsize.GetWidth(), 30);

	out.MoveTo(xbase + xplot, ybase + yplot - 30);
	out.WriteLine(text.Format("(%s) dup stringwidth pop neg 0 rmoveto show", gname));

	if(mod->diagbox) mod->diagbox->textbox->AppendText(text.Format("EPS Written OK\n"));

	

	out.Close();
}



GraphPrint::GraphPrint(HypoMain *main, GraphDat *graphdat, const wxString &title)
	: wxPrintout(title)
{
	mainwin = main; 
	graph = graphdat;

	ostype = GetSystem();

	colourpen[0].Set("#000000");       // 0 black
	colourpen[1].Set("#F50000");       // 1 red
	colourpen[2].Set("#00F500");       // 2 green
	colourpen[3].Set("#0000F5");       // 3 blue
	colourpen[4].Set("#F5F500");       // 4 yellow
	colourpen[5].Set("#F500F5");       // 5 purple
	colourpen[6].Set("#FF8080");       // 6 light red
	colourpen[7].Set("#80FF80");       // 7 light green
	colourpen[8].Set("#8080FF");       // 8 light blue
}


bool GraphPrint::OnPrintPage(int page)
{
	wxDC *dc = GetDC();
	if (dc)
	{
		if (page == 1)
			Plot();
		else if (page == 2)
			DrawPageTwo();

		// Draw page numbers at top left corner of printable area, sized so that
		// screen size of text matches paper size.
		MapScreenSizeToPage();

		dc->DrawText(wxString::Format("PAGE %d", page), 0, 0);

		return true;
	}
	else
		return false;
}


bool GraphPrint::OnBeginDocument(int startPage, int endPage)
{
	if (!wxPrintout::OnBeginDocument(startPage, endPage))
		return false;

	return true;
}


void GraphPrint::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
	*minPage = 1;
	*maxPage = 1;
	*selPageFrom = 1;
	*selPageTo = 1;
}


bool GraphPrint::HasPage(int pageNum)
{
	//return (pageNum == 1 || pageNum == 2);
	return (pageNum == 1);
}


void GraphPrint::DrawPageTwo()
{
	wxDC *dc = GetDC();

	// Get the logical pixels per inch of screen and printer
	int ppiScreenX, ppiScreenY;
	GetPPIScreen(&ppiScreenX, &ppiScreenY);
	int ppiPrinterX, ppiPrinterY;
	GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

	// This scales the DC so that the printout roughly represents the screen
	// scaling. The text point size _should_ be the right size but in fact is
	// too small for some reason. This is a detail that will need to be
	// addressed at some point but can be fudged for the moment.
	float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

	// Now we have to check in case our real page size is reduced (e.g. because
	// we're drawing to a print preview memory DC)
	int pageWidth, pageHeight;
	int w, h;
	dc->GetSize(&w, &h);
	GetPageSizePixels(&pageWidth, &pageHeight);

	// If printer pageWidth == current DC width, then this doesn't change. But w
	// might be the preview bitmap width, so scale down.
	float overallScale = scale * (float)(w/(float)pageWidth);
	dc->SetUserScale(overallScale, overallScale);

	// Calculate conversion factor for converting millimetres into logical
	// units. There are approx. 25.4 mm to the inch. There are ppi device units
	// to the inch. Therefore 1 mm corresponds to ppi/25.4 device units. We also
	// divide by the screen-to-printer scaling factor, because we need to
	// unscale to pass logical units to DrawLine.

	// Draw 50 mm by 50 mm L shape
	float logUnitsFactor = (float)(ppiPrinterX/(scale*25.4));
	float logUnits = (float)(50*logUnitsFactor);
	dc->SetPen(* wxBLACK_PEN);
	dc->DrawLine(50, 250, (long)(50.0 + logUnits), 250);
	dc->DrawLine(50, 250, 50, (long)(250.0 + logUnits));

	dc->SetBackgroundMode(wxTRANSPARENT);
	dc->SetBrush(*wxTRANSPARENT_BRUSH);

	// GetTextExtent demo:
	wxString words[7] = {"This ", "is ", "GetTextExtent ", "testing ", "string. ", "Enjoy ", "it!" };
	wxCoord width, height;
	long x = 200, y= 250;
	wxFont fnt(15, wxSWISS, wxNORMAL, wxNORMAL);

	dc->SetFont(fnt);

	for(int i=0; i<7; i++) {
		wxString word = words[i];
		word.Remove( word.Len()-1, 1 );
		dc->GetTextExtent(word, &w, &h);
		dc->DrawRectangle(x, y, w, h);
		dc->GetTextExtent(words[i], &w, &h);
		dc->DrawText(words[i], x, y);
		x += w;
	}


}

void GraphPrint::Plot()
{
	wxDC *dc;
	wxFont testFont;

	int ylabels, xlabels;
	double xval, xscale, xdis, xtime;
	double xfrom, xto, yfrom, yto;
	double srangex, srangey;
	wxString snum, gname, text;
	double binsize;
	int gtype, colour, sample, gpar;
	int xbase, ybase, xplot, yplot, xstretch;
	int i, k;
	wxSize textsize;
	int xylab, res;
	datint *gdatav;
	datdouble *gdatadv;
	double timepoint, maxtime;
	double y, xrange, yrange, xnum;
	int xpos, ypos;

	dc = GetDC();

	xbase = 10; 
	ybase = 10;
	yplot = 200;
	xplot = 500;
	xstretch = mainwin->xstretch;
	xlabels = 10;
	ylabels = mainwin->ylabels;

	// We know the graphic is 230x350. If we didn't know this, we'd need to
	// calculate it.
	wxCoord maxX = 600;
	wxCoord maxY = 250;

	FitThisSizeToPageMargins(wxSize(maxX, maxY), *(mainwin->pageSetupData));
	wxRect fitRect = GetLogicalPageMarginsRect(*(mainwin->pageSetupData));

	wxCoord xoff = (fitRect.width - maxX) / 2;
	wxCoord yoff = (fitRect.height - maxY) / 2;
	OffsetLogicalOrigin(xoff, yoff);

	wxGraphicsContext *gc = NULL;

	wxPrinterDC *printer_dc = wxDynamicCast(dc, wxPrinterDC);
	if(printer_dc) gc = wxGraphicsContext::Create(*printer_dc);

	wxWindowDC *window_dc = wxDynamicCast(dc, wxWindowDC);
	if(window_dc) gc = wxGraphicsContext::Create(*window_dc);
#ifndef OSXClip
	wxEnhMetaFileDC *emf_dc = wxDynamicCast(dc, wxEnhMetaFileDC);
	if(emf_dc) gc = wxGraphicsContext::Create(*emf_dc);
#endif
	testFont.Create(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	dc->SetBackground(*wxWHITE_BRUSH);
	// dc.Clear();
	dc->SetFont(testFont);


	// Get Graph parameters
	gpar = graph->gparam;
	xscale = graph->xscale;
	xdis = graph->xdis;
	yfrom = graph->yfrom;
	yto = graph->yto;
	xfrom = graph->xfrom * xscale;
	xto = graph->xto * xscale;
	gname = graph->gname;
	binsize = graph->binsize;
	gtype = graph->type;
	colour = graph->colour;
	sample = graph->samprate;
	xtime = xto - xfrom;

	if(gpar == -3) gdatav = graph->gdatav;
	if(gpar == -4) gdatadv = graph->gdatadv;
	if(graph->spikedata != NULL) burstdata = graph->spikedata->burstdata;
	else burstdata = NULL;


	// Draw Axes

	dc->SetPen(colourpen[black]);

	dc->DrawLine(xbase, ybase, xbase, ybase + yplot);
	dc->DrawLine(xbase, ybase + yplot, xbase + xplot + xstretch, ybase + yplot);


	// Draw labels

	//dc->SetFont(textfont);

	for(i=0; i<=xlabels; i++) {
		dc->DrawLine(xbase + i*xplot/xlabels, ybase + yplot, xbase + i*xplot/xlabels, ybase + yplot + 5);
		xval = ((double)(xto - xfrom) / 10*i + xfrom) / xscale;
		srangex = (xto - xfrom) / xscale;
		snum.Printf("%.0f", xval + xdis);	
		//if(xto - xfrom < 10) snum.Printf(wxT("%.1f"), xval);	
		//if(xto - xfrom < 1) snum.Printf(wxT("%.2f"), xval);
		//if(xto - xfrom < 0.1) snum.Printf(wxT("%.3f"), xval);	
		if(srangex < 10) snum.Printf("%.1f", xval + xdis);	
		if(srangex < 1) snum.Printf("%.2f", xval + xdis);
		if(srangex < 0.1) snum.Printf("%.3f", xval + xdis);	
		textsize = dc->GetTextExtent(snum);
		if(ostype == Mac)
			dc->DrawText(snum, xbase + i*xplot/xlabels - textsize.GetWidth()/2, ybase + yplot + 10);
		else
			dc->DrawText(snum, xbase + i*xplot/xlabels - textsize.GetWidth()/2, ybase + yplot + 10);
	}

	//if(yplot < 150 && ylabels >= 10) dc.SetFont(smallfont);
	xylab = 5;

	for(i=0; i<=ylabels; i+=1) {
		dc->DrawLine(xbase, ybase + yplot - i*yplot/ylabels, xbase - 5, ybase + yplot - i*yplot/ylabels);
		if(yto - yfrom < 0.1)
			snum.Printf("%.3f", (double)(yto - yfrom) / ylabels*i + yfrom);
		else if(yto - yfrom < 1)
			snum.Printf("%.2f", (double)(yto - yfrom) / ylabels*i + yfrom);
		else if(yto - yfrom < 10)
			snum.Printf("%.1f", (double)(yto - yfrom) / ylabels*i + yfrom);
		else
			snum.Printf("%.0f", (double)(yto - yfrom) / ylabels*i + yfrom);
		textsize = dc->GetTextExtent(snum);
		if(ostype == Mac)
			dc->DrawText(snum, xbase - xylab - textsize.GetWidth(), ybase + yplot - i*yplot/ylabels - textsize.GetHeight()/2);
		else
			dc->DrawText(snum, xbase - xylab - textsize.GetWidth(), ybase + yplot - i*yplot/ylabels - 7);

	}
	//gname.Printf("GText width %d height %d", textsize.x, textsize.y);

	//if(yplot < 150) dc.SetFont(textfont);
	textsize = dc->GetTextExtent(gname);
	dc->DrawText(gname, xplot + 50 - textsize.GetWidth(), 30);


	dc->SetPen(colourpen[colour]);

	// Set drawing scales
	xto /= binsize;
	xfrom /= binsize;

	yrange = (double)yplot / (yto - yfrom);
	xrange = (double)xplot / (xto - xfrom); 
	xnum = (double)(xto - xfrom) / xplot;

	// Draw graph data

	if(gtype == 3) {                             // spike rate data with optional burst colouring
		int spikestep = 0;
		int burstcolour = 0;

		for(i=0; i<(xto - xfrom); i++) {
			if(gpar == -3) y = (*gdatav)[i + (int)xfrom];
			if(gpar == -4) y = (*gdatadv)[i + (int)xfrom];

			if(binsize == 1) res = 0;
			if(binsize == 0.1) res = 1;
			if(binsize == 0.01) res = 2;
			if(binsize == 0.001) res = 3;

			if(burstdata == NULL || burstdata->burstdisp == 0) dc->SetPen(colourpen[red]);

			else {                  // burst colouring
				burstcolour = 0;
				if(res > 0 && res < 3) burstcolour = 0;
				if(res == 0) {
					timepoint = (xfrom + i + 1) * binsize * 1000;
					while(timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
						if(!burstcolour) burstcolour = burstdata->spikes[spikestep];
						spikestep++;
					}
				}
				if(res == 3) {
					timepoint = (xfrom + i) * binsize * 1000;
					while(timepoint < burstdata->maxtime && burstdata->times[spikestep] < timepoint + 0.0005) {
						burstcolour = burstdata->spikes[spikestep];
						spikestep++;
					}
				}			
				if(burstcolour == 0) 	
					dc->SetPen(colourpen[red]);
				else if(burstcolour % 2 == 0)
					dc->SetPen(colourpen[blue]);
				else if(burstcolour % 2 == 1)
					dc->SetPen(colourpen[green]);	
			}

			xpos = i * xrange + xbase;
			if(xrange <= 1) dc->DrawLine(xpos, ybase + yplot, xpos, ybase + yplot - (int)(yrange * (y - yfrom)));
			else {
				for(k=0; k<xrange-1; k++) 
					dc->DrawLine(xpos + k, ybase + yplot, xpos + k, ybase + yplot - (int)(yrange * (y - yfrom))); 
			}	
		}
		//opfile.Close();
	}

	/*
	// dc.SetBackgroundMode(wxTRANSPARENT);

	dc->SetPen(*wxBLACK_PEN);
	dc->SetBrush(*wxLIGHT_GREY_BRUSH);

	dc->DrawRectangle(0, 0, 230, 350);
	dc->DrawLine(0, 0, 229, 349);
	dc->DrawLine(229, 0, 0, 349);
	dc->SetBrush(*wxTRANSPARENT_BRUSH);

	dc->SetBrush(*wxCYAN_BRUSH);
	dc->SetPen(*wxRED_PEN);

	dc->DrawRoundedRectangle(0, 20, 200, 80, 20);

	dc->DrawText( wxT("Rectangle 200 by 80"), 40, 40);

	dc->SetPen( wxPen(*wxBLACK, 0, wxPENSTYLE_DOT_DASH) );
	dc->DrawEllipse(50, 140, 100, 50);
	dc->SetPen(*wxRED_PEN);

	dc->DrawText("Test message: this is in 10 point text", 10, 180);
	*/




	//gc = wxGraphicsContext::Create(dc);
	/*
	if(gc) {
	// make a path that contains a circle and some lines, centered at 100,100
	gc->SetPen( *wxRED_PEN );

	wxGraphicsPath path = gc->CreatePath();
	path.AddCircle( 50.0, 50.0, 50.0 );
	path.MoveToPoint(0.0, 50.0);
	path.AddLineToPoint(100.0, 50.0);
	path.MoveToPoint(50.0, 0.0);
	path.AddLineToPoint(50.0, 100.0 );
	path.CloseSubpath();
	path.AddRectangle(25.0, 25.0, 50.0, 50.0);

	gc->StrokePath(path);

	// draw some text
	wxString text("Text by wxGraphicsContext");
	gc->SetFont(testFont, *wxBLACK);
	gc->DrawText(text, 25.0, 60.0);

	// draw rectangle around the text
	double w, h, d, el;
	gc->GetTextExtent(text, &w, &h, &d, &el);
	gc->SetPen( *wxBLACK_PEN );
	gc->DrawRectangle(25.0, 60.0, w, h);

	delete gc;
	}
	*/

}