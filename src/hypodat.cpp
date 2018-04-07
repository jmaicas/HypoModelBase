

#include <hypodat.h>
#include <hypotools.h>


NeuroDat::NeuroDat()
	//: times(std::make_shared<double>(maxspikes)), 
	//srate(std::make_shared<int>(maxtime)),
	//srate10(std::make_shared<int>(maxtime))
{
	//times = new double[maxspikes];
	//srate = new int[maxtime];
	//srate10 = new int[maxtime];
	times.resize(maxspikes);
	srate.resize(maxtime);
	srate10.resize(maxtime);
}


NeuroDat::~NeuroDat()
{
	//delete[] times;
	//delete[] srate;
	//delete[] srate10;
}


void NeuroDat::ratereset()
{
	int i;

	for(i=0; i<maxtime; i++) {
		srate[i] = 0;
		srate10[i] = 0;
	}
	for(i=0; i<10000; i++) srate100[i] = 0;
}


void NeuroDat::ratecalc()
{
	short spikestep;
	int i;
	int maxspikes = 100000;
	//int maxtime = 10000;
	//datdouble times;
	double *isis;
	double stime, isimean;

	//times.setsize(spikecount);
	isis = new double[spikecount+1];
	stime = 0;
	isimean = 0;
	isicount = spikecount - 1;


	for(i=0; i<maxtime; i++) srate[i] = 0;

	for(i=0; i<spikecount-1; i++) {
		//isis[i] = (currentset->times[i+1] - currentset->times[i]) * 1000;   // real
		if(i > maxspikes-1) break;
		//if(neurodat) isis[i] = datneuron->isis[i];
		isis[i] = (times[i+1] - times[i]); // * 1000;
		//stime = stime + isis[i];
		//times[i] = stime/1000;
		//index[i] = i;
		//if(hist1.max < (int)isis[i]) hist1.max = (int)isis[i];
		//if(hist1.data.size() < hist1.max + 1) hist1.data.resize(hist1.max + 1);
		//hist1[(int)isis[i]]++;	
		isimean = isimean + (double)(isis[i] / spikecount);
		//variance = isis[i] * isis[i] / spikecount + variance;
	}

	if(isimean == 0) freq = 0;
	else freq = 1000/isimean;

	spikestep = 0;
	//srate.max = (int)(times[spikecount-1] + 0.5); 
	for(i=0; i<=times[spikecount-1] && i < maxtime; i++) {	     // spike rate count (1s)
		//fprintf(ofp, "%ds. ", i);
		if(spikestep > spikecount || i >= 10000) {
			//fprintf(ofp, "break spikestep %d  spikecount %d\n", spikestep, spikecount);
			break;
		}
		while(times[spikestep] < i+1) {
			//fprintf(ofp, "spike %d  ", spikestep);
			srate[i]++;
			spikestep++;
			if(spikestep >= spikecount) break;
		}
		//fprintf(ofp, "srate %d\n", srate[i]);
	}
	delete[] isis;
}



void SpikeDat::Clear()
{
	int i;

	for(i=0; i<10000; i++) {
		hist1[i] = 0;
		hist5[i] = 0;
		haz1[i] = 0;
		haz5[i] = 0;
	}
	for(i=0; i<100000; i++) {
		srate[i] = 0;
		times[i] = 0;
		isis[i] = 0;
	}
	for(i=0; i<1000000; i++) {
		srate1[i] = 0;
		synsim[i] = 0;
	}
	spikecount = 0;
}


void SpikeDat::ReSize(int newsize)
{
	//srate.data.resize(newsize);
	//srate.max = newsize;
	srate.setsize(newsize);
	times.data.resize(newsize);
	times.max = newsize;
	isis.data.resize(newsize);
	isis.max = newsize;

	maxspikes = newsize;
}


SpikeDat::SpikeDat()
{
	diagbox = NULL;

	hist1.setsize(10000);
	hist5.setsize(10000);
	haz1.setsize(10000);
	haz5.setsize(10000);
	histquad.setsize(1000);
	histquadx.setsize(1000);
	histquadsm.setsize(1000);
	histquadlin.setsize(1000);
	hazquad.setsize(10000);

	hist1norm.setsize(10000);
	hist5norm.setsize(10000);
	//haz1norm.setsize(10000);
	haz5norm.setsize(10000);

	srate.setsize(100000);
	srate1.setsize(1000000);
	srate10.setsize(100000);
	srate100.setsize(100000);
	srate10s.setsize(100000);
	srate100s.setsize(100000);

	synsim.data.resize(1000100);
	synsim.max = 1000000;

	maxspikes = 100000;

	times.data.resize(maxspikes);
	times.max = 100000;
	isis.data.resize(maxspikes);
	isis.max = 100000;
	winfreq.data.resize(11000);
	winfreq.max = 10000;

	raterec.setsize(1000);
	netinputrec.setsize(1000);

	IoDdata.setsize(100);
	IoDdataX.setsize(100);

	meanV.setsize(5000);

	burstdata = NULL;
	vasomean.data.resize(250);
	vasomean.max = 200;

	mag.setsize(maxspikes);

	count = 0;
	spikecount = 0;
	start = 0;
	freqwindow = 100;
	normscale = 10000;
	mainwin = NULL;
	graphs = false;
}


SpikeDat::~SpikeDat()
{
	if(burstdata) delete burstdata;
}


SpikeDatTest::SpikeDatTest()
{
	diagbox = NULL;

	/*
	hist1.setsize(10000);
	hist5.setsize(10000);
	haz1.setsize(10000);
	haz5.setsize(10000);
	histquad.setsize(1000);
	histquadx.setsize(1000);
	histquadsm.setsize(1000);
	histquadlin.setsize(1000);
	hazquad.setsize(10000);

	hist1norm.setsize(10000);
	hist5norm.setsize(10000);
	//haz1norm.setsize(10000);
	//haz5norm.setsize(10000);

	srate.setsize(100000);
	srate1.setsize(1000000);
	srate100.setsize(100000);

	synsim.data.resize(1000100);
	synsim.max = 1000000;

	times.data.resize(100000);
	times.max = 100000;
	isis.data.resize(100000);
	isis.max = 100000;
	winfreq.data.resize(11000);
	winfreq.max = 10000;

	raterec.setsize(1000);
	netinputrec.setsize(1000);

	burstdata = NULL;
	vasomean.data.resize(250);
	vasomean.max = 200;

	count = 0;
	spikecount = 0;
	start = 0;
	freqwindow = 100;
	maxspikes = 100000;
	mainwin = NULL;*/
}


SpikeDatTest::~SpikeDatTest()
{
	//if(burstdata) delete burstdata;
}


BurstDat::BurstDat(bool select)
{
	selectmode = select; 

	burstdisp = 0;
	numbursts = 0;
	hist1.data.resize(10000);
	hist5.data.resize(10000);
	hist1norm.data.resize(10000);
	hist5norm.data.resize(10000);
	haz1.data.resize(10000);
	haz5.data.resize(10000);
	//spikes.data.resize(100000);
	//spikes.max = 100000;
	profile.data.resize(1000);
	profile.max = 500;
	tailprofile.data.resize(1000);
	tailprofile.max = 500;
	bursthaz.data.resize(1000);
	bursthaz.max = 500;
	length.data.resize(250);
	length.max = 200;

	tailnum.data.resize(1000);
	tailnum.max = 1000;
	tailmean.data.resize(1000);
	tailmean.max = 1000;
	tailstdev.data.resize(1000);
	tailstdev.max = 1000;
	tailstder.data.resize(1000);
	tailstder.max = 1000;

	profilesm.setsize(500);
	tailprofilesm.setsize(500);
	IoDdata.setsize(100);
	IoDdataX.setsize(100);

	maxbursts = 1000;
	//bustore = new burst[maxbursts];
	bustore.resize(maxbursts);
	burstspikes = new int[100000];
	spikes = burstspikes;
}


BurstDat::~BurstDat()
{
	//delete[] bustore;
	delete [] burstspikes;
}


int BurstDat::spikeburst(int spike)
{
	int bindex = 1;

	for(bindex = 1; bindex <= numbursts; bindex++) {
		if(spike <= bustore[bindex].end && spike >= bustore[bindex].start) return bindex;
	}

	return 0;
}


AnaDat::AnaDat()
{
	autocorr.data.resize(10100);
	autocorr.max = 10000;
}


CurrentDat::CurrentDat()
{
	int storesize = 1000000;

	I.data.resize(storesize * 1.1);
	I.max = storesize;

	DAP.data.resize(storesize * 1.1);
	DAP.max = storesize;

	DAP2.data.resize(storesize * 1.1);
	DAP2.max = storesize;

	AHP.data.resize(storesize * 1.1);
	AHP.max = storesize;
}


void TypeSet::Add(wxString name, int type)
{
	names[numtypes] = name;
	typeindex[numtypes] = type;
	refindex[type] = numtypes;
	numtypes++;
}


int TypeSet::GetIndex(int type)
{
	if(type == 4) type = 5;
	return refindex[type];
}


int TypeSet::GetType(int ref)
{
	return typeindex[ref];
}


wxString TypeSet::List()
{
	wxString list, text;
	int i;

	list = "";
	for(i=0; i<numtypes; i++) 
		list = list + text.Format("Index %d Type %d Name %s\n", i, typeindex[i], names[i]); 
	return list;
}


GraphDat::GraphDat()             // See more specific versions below
{
	scrollpos = 0;
	gparam = 0;
	xscale = 1;
	xdis = 0;
	xstop = 0;
	negscale = 0;
	//burstdata = NULL;
	spikedata = NULL;
}


wxString GraphDat::StoreDat(wxString tag)
{
	wxString gtext1, gtext2;
	wxString storegname, storextag, storeytag;

	storegname = gname;                       // replace spaces with underscores for textfile storing
	storegname.Replace(" ", "_");
	storextag = xtag;
	storextag.Replace(" ", "_");
	storeytag = ytag;
	storeytag.Replace(" ", "_");

	gtext1.Printf("v8 index %d tag %s xf %.4f xt %.4f yf %.4f yt %.4f xl %d xs %.4f xm %d yl %d ys %.4f ym %d c %d crgb %s xs %.4f xu %.4f ps %.4f name %s xtag %s ytag %s xp %d yp %d pf %.4f cm %d type %d xd %.4f xsam %.4f bw %d bg %d yu %.4f ", 
		gindex, tag, xfrom, xto, yfrom, yto, xlabels, xstep, xtickmode, ylabels, ystep, ytickmode, colour, ColourString(strokecolour, 1), 
		xshift, xunitscale, plotstroke, storegname, storextag, storeytag, xplot, yplot, labelfontsize, clipmode, type, xunitdscale, xsample, barwidth, bargap, yunitscale);
		
	gtext2.Printf("xl %d yl %d xm %d ym %d xs %d ys %d", 
		xlabelplaces, ylabelplaces, xlabelmode, ylabelmode, xscalemode, yscalemode);

	return gtext1 + gtext2;
}


void GraphDat::LoadDat(wxString data, int version)                    // Not in use, see GraphBase::BaseLoad - now in use
{
	wxString readline, numstring, stringdat;
	long numdat;
	long red, green, blue;
	//int version;

	//if(diagbox) diagbox->Write(data + '\n');

	readline = data.AfterFirst('f');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&xfrom);

	readline = readline.AfterFirst('t');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&xto);

	readline = readline.AfterFirst('f');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&yfrom);

	readline = readline.AfterFirst('t');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&yto);

	readline = readline.AfterFirst('l');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	xlabels = numdat;

	readline = readline.AfterFirst('s');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&xstep);

	readline = readline.AfterFirst('m');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	xtickmode = numdat;

	readline = readline.AfterFirst('l');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	ylabels = numdat;

	readline = readline.AfterFirst('s');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&ystep);

	readline = readline.AfterFirst('m');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	ytickmode = numdat;

	readline = readline.AfterFirst('c');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	colour = numdat;

	readline = readline.AfterFirst('b');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&red);
	readline = readline.AfterFirst(' ');
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&green);
	readline = readline.AfterFirst(' ');
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&blue);
	strokecolour = wxColour(red, green, blue);

	if(version > 0) {
		xshift = ParseDouble(&readline, 's');
		xunitscale = ParseDouble(&readline, 'u');
		plotstroke = ParseDouble(&readline, 's');

		//if(diagbox) diagbox->Write(readline + '\n');
		
		gname = ParseString(&readline, 'e');
		gname.Replace("_", " ");

		xtag = ParseString(&readline, 'g');
		xtag.Replace("_", " ");

		ytag = ParseString(&readline, 'g');
		ytag.Replace("_", " ");
	}	

	if(version > 2) {
		xplot = ParseDouble(&readline, 'p');
		yplot = ParseDouble(&readline, 'p');
		labelfontsize = ParseDouble(&readline, 'f');
		clipmode = ParseLong(&readline, 'm');
	}

	if(version > 3) type = ParseLong(&readline, 'e');

	if(version > 4) {
		xunitdscale = ParseDouble(&readline, 'd');
		xsample = ParseDouble(&readline, 'm');
	}

	if(version > 5) {
		barwidth = ParseLong(&readline, 'w');
		bargap = ParseLong(&readline, 'g');
	}

	if(version > 6) {
		yunitscale = ParseDouble(&readline, 'u');
	}

	if(version > 7) {
		xlabelplaces = ParseLong(&readline, 'l');
		ylabelplaces = ParseLong(&readline, 'l');
		xlabelmode = ParseLong(&readline, 'm');
		ylabelmode = ParseLong(&readline, 'm');
		xscalemode = ParseLong(&readline, 's');
		yscalemode = ParseLong(&readline, 's');
	}
}


GraphDat::GraphDat(datdouble *newdat, double xf, double xt, double yf, double yt, wxString name, int gtype, double bin, int gcolour, int xs, int xd)
{
	xscale = xs;
	xdis = xd;
	spikedata = NULL;

	gdatax = NULL;
	xcount = 0;
	ycount = 0;
	gdatadv = newdat;
	gparam = -4;
	type = gtype;
	samprate = 1;
	scattermode = 0;
	linemode = 1;
	scattersize = 2;

	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	colour = gcolour;
	binsize = bin;
	Init();
}


GraphDat::GraphDat(datint *newdat, double xf, double xt, double yf, double yt, wxString name, int gtype, double bin, int gcolour)
{
	scrollpos = 0;
	xscale = 1;
	xdis = 0;
	negscale = 0;
	spikedata = NULL;

	gdatav = newdat;
	gparam = -3;
	type = gtype;
	
	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	colour = gcolour;
	binsize = bin;
	Init();
}


GraphDat::GraphDat(datint *newdat, double xf, double xt, double yf, double yt, wxString name, SpikeDat *newspikedata, double bin, int gcolour)
{
	xscale = 1;
	xdis = 0;
	negscale = 0;

	type = 3;
	spikedata = newspikedata;
	gdatav = newdat;
	gparam = -3;

	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	colour = gcolour;
	binsize = bin;
	Init();
}


void GraphDat::Init()
{
	diagbox = NULL;
	plotdata = NULL;

	scrollpos = 0;
	negscale = 0;
	xlabels = 0;
	ylabels = 0;
	xstep = 0;
	ystep = 0;
	xtickmode = 0;
	ytickmode = 0;
	plotstroke = 0.5;
	xplot = 500;
	yplot = 200;
	xshift = 0;
	yshift  = 0;
	xsample = 1;
	xunitscale = 1;
	yunitscale = 1;
	xunitdscale = 1;
	strokecolour.Set(0, 0, 0);
	xtag = "X";
	ytag = "Y";
	xlabelgap = 30; //40;
	ylabelgap = 20; //30;
	labelfontsize = 10;
	tickfontsize = 10;
	clipmode = 0;
	synchx = true;
	barshift = 0;
	barwidth = 10;
	bargap = 10;
	xstop = 0;
	highstart = 0;
	highstop = 0;
	highcolour = red;
	oldset = false;

	xlabelmode = 1;
	ylabelmode = 1;
	xticklength = 5;
	yticklength = 5;
	xlabelplaces = -1;
	ylabelplaces = -1;
	xscalemode = 0;
	yscalemode = 0;
	xlogbase = 2.7183;
	ylogbase = 10;

	scatterfield = 0;
	scattermean = 0;
}


GraphDisp::GraphDisp()
{
	numplots = 0;
	currentplot = 0;
	spikedisp = 0;
	plotset = NULL;
}


void GraphDisp::Add(GraphDat *newplot)
{
	if(numplots < maxplots) {
		plot[numplots] = newplot;
		numplots++;
	}
}


void GraphDisp::Remove(int index)       // Remove plot layer from GraphDisp set
{
	int i;

	if(index >= numplots) return;
	for(i=index; i<numplots-1; i++) plot[i] = plot[i+1];   // re-index
	numplots--;	
}



void GraphDisp::Set(int index, GraphDat *newplot)
{
	if(numplots <= index) Add(newplot);
	else plot[index] = newplot;
}


void GraphDisp::XYSynch(GraphDat *graph0)      // Synchronise X and Y axes for all plots
{
	int i;
	GraphDat *graph;

	//graph0 = plot[0];
	if(!graph0) graph0 = plot[0];
	
	for(i=1; i<numplots; i++) {
		graph = plot[i];
		graph->yfrom = graph0->yfrom; 
		graph->yto = graph0->yto; 
		graph->xfrom = graph0->xfrom; 
		graph->xto = graph0->xto; 
	}
}


void GraphDisp::Front(GraphDat *newplot)
{
	plot[0] = newplot;
	if(numplots == 0) numplots = 1;
}


GraphDat *GraphDisp::GetFront()
{
	return plot[0];
}


GraphSet::GraphSet(GraphBase *gbase, int gdex) 
{
	graphbase = gbase; 
	numgraphs = 0;
	numflags = 0;
	modesum = 0;
	single = true;
	if(gdex != -1) Add(gdex);
};


void GraphSet::Add(int gdex, int gcode) 
{
	gindex[numgraphs] = gdex;
	gcodes[numgraphs] = gcode;
	numgraphs++;
	if(numgraphs > 1) single = false;
}


void GraphSet::Add(wxString tag, int gcode) 
{
	gindex[numgraphs] = graphbase->tagindex[tag];
	gcodes[numgraphs] = gcode;
	numgraphs++;
	if(numgraphs > 1) single = false;
}


void GraphSet::AddFlag(wxString flag, int weight)
{
	modeflag[numflags] = flag;
	modeweight[numflags] = weight;
	numflags++;
}


int GraphSet::GetPlot(ParamStore *gflags)
{
	int i, gdex;

	if(single) return gindex[0];

	modesum = 0;
	gdex = gindex[0];
	for(i=0; i<numflags; i++) modesum += (*gflags)[modeflag[i]] * modeweight[i];

	for(i=0; i<numgraphs; i++)
		if(gcodes[i] == modesum) gdex = gindex[i];

	return gdex;
}


wxString GraphSet::Display()
{
	int i;
	wxString text, output = "";

	output += text.Format("\nSet %s\n", tag);
	for(i=0; i<numgraphs; i++) output += text.Format("setindex %d  index %d  code %d\n", i, gindex[i], gcodes[i]);  

	return output;
}


int GraphBase::Add(GraphDat newgraph, wxString tag, wxString settag, bool set)       // default settag = "", set=true
{
	int sdex;
	wxString text;
	GraphSet *graphset = NULL;

	newgraph.diagbox = mainwin->diagbox;
	newgraph.strokecolour = mainwin->colourpen[newgraph.colour];
	newgraph.gtag = tag;

	// If single graph, create new single graph set, otherwise add to set 'settag'
	if(set) {
		if(settag == "") graphset = NewSet(newgraph.gname, tag);
		else graphset = GetSet(settag);
		if(graphset) graphset->Add(numgraphs);
		//newgraph.sdex = graphset->sdex;
	}

	// Expand graphbase if necessary
	if(numgraphs == storesize) {
		storesize++;
		graphstore.resize(storesize);
	}

	// Add the new graph to graphbase
	graphstore[numgraphs] = newgraph;
	graphstore[numgraphs].gindex = numgraphs;
	tagindex[tag] = numgraphs;
	nameindex[newgraph.gname] = numgraphs;
	indextag[numgraphs] = tag;

	//if(graphset && mainwin->diagbox) mainwin->diagbox->Write(text.Format("new graph sdex %d\n", graphset->sdex));
	
	numgraphs++;
	return numgraphs-1; 
};


GraphSet *GraphBase::NewSet(wxString name, wxString tag) 
{
	wxString text; 

	if(numsets == setstoresize) {
		setstoresize++;
		setstore.resize(setstoresize);
	}
	setstore[numsets] = GraphSet(this);
	setstore[numsets].tag = tag;
	setstore[numsets].name = name;
	setstore[numsets].sdex = numsets;
	settagindex[tag] = numsets;
	setindextag[numsets] = tag;
	numsets++;

	//if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("new set name %s tag %s numsets %d\n", name, tag, numsets));

	return &setstore[numsets-1];
}

/*
int GraphBase::AddNewSet(wxString tag, int gdex) 
{
	if(numsets == setstoresize) {
		setstoresize++;
		setstore.resize(setstoresize);
	}
	setstore[numsets] = GraphSet(this, gdex);
	setstore[numsets].Add(gdex);
	setstore[numsets].tag = tag;
	settagindex[tag] = numsets;
	setindextag[numsets] = tag;
	numsets++;
	return numsets-1;
}*/


GraphSet *GraphBase::GetSet(wxString tag) 
{
	if(!settagindex.check(tag)) return NULL;
	int index = settagindex[tag];
	return &(setstore[index]);
}


GraphDat *GraphBase::GetGraph(wxString tag) 
{
	if(!tagindex.check(tag)) return NULL;
	int index = tagindex[tag];
	return &(graphstore[index]);
}


GraphDat *GraphBase::GetGraphFromName(wxString name) 
{
	if(!nameindex.check(name)) return NULL;
	int index = nameindex[name];
	return &(graphstore[index]);
}


GraphSet *GraphBase::GetSet(int index) 
{
	return &(setstore[index]);
}


void GraphBase::BaseStore(wxString path, wxString tag)
{
	int i;
	TextFile outfile;
	wxString text, filename, filetag;

	filename = "gbase-" + tag + ".dat";

	//outfile.New(initpath + "/Graphs/" + filename);
	outfile.New(path + "/" + filename);
	for(i=0; i<numgraphs; i++) {
		outfile.WriteLine(graphstore[i].StoreDat(GetTag(i)));
		//outfile.WriteLine(text.Format
	}
	outfile.Close();
}


void GraphBase::BaseLoad(wxString path, wxString tag, wxTextCtrl *textbox)
{
	int i, index;
	TextFile infile;
	wxString readline, filename, filetag;
	wxString text, numstring, namestring, basestring;
	wxString gtag, gname;
	double numdat;
	GraphDat *graph;
	int version;

	filename = "gbase-" + tag + ".dat";

	//if(!infile.Open(initpath + "/Graphs/" + filename)) return;
	if(!infile.Open(path + "/" + filename)) return;

	if(textbox) textbox->AppendText(text.Format("Loading %d graphs\n", numgraphs));

	i = 0;
	readline = infile.ReadLine();

	// Version check

	//fileversion = ParseLong(&vstring, 'v');
	//textbox->AppendText(text.Format("Base file version %d\n", fileversion));
	//if(fileversion < version) textbox->AppendText(text.Format("Create base index\n"));

	while(!readline.IsEmpty()) {

		if(readline.GetChar(0) == 'v') version = ParseLong(&readline, 'v');          // check file version for backwards compatability
		else version = 0;
		//textbox->AppendText(text.Format("Base file version %d\n", version));
		//textbox->AppendText(text.Format("Readline %s\n", readline));

		if(version >= 2) {                                                  // Modern, reference by tag
			gtag = ParseString(&readline, 'g');
			graph = GetGraph(gtag);
			if(graph) graph->LoadDat(readline, version);
		}
		else {
			//GetGraphFromName(gname)->LoadDat(readline, version);              // Should add code to chop off any tag first
			//int ndex = readline.Find("name");
			//textbox->AppendText(text.Format("Base file version %d\n", version));
			//textbox->AppendText(text.Format("Readline %s\n", readline));
			namestring = readline.Mid(readline.Find("name"));
			//textbox->AppendText(text.Format("Name string %s\n", namestring));
			gname = ParseString(&namestring, 'e');
			gname.Replace("_", " ");
			if(textbox) textbox->AppendText(text.Format("gname %s\n", gname));
			graph = GetGraphFromName(gname);
			if(graph) graph->LoadDat(readline, version);
		}
			
		//graphstore[i].LoadDat(readline, version);
			
		if(infile.End()) break;
		readline = infile.ReadLine();	
		i++;	
	}
	infile.Close();


	// Read graphbase entries

	/*
	while(!readline.IsEmpty()) {
		graphstore[i].diagbox = mainwin->diagbox;
		graphstore[i].LoadDat(readline, version);
			
		if(infile.End()) break;
		readline = infile.ReadLine();	
		i++;	
	}
	infile.Close();
	*/
}


FitConSet::FitConSet(int size)
{
	count = 0;
	max = size;
	tags = new wxString[size];
	cons = new FitCon[size];
}


FitConSet::~FitConSet()
{
	delete[] cons;
	delete[] tags;
}


void FitConSet::AddCon(wxString tag, wxString label, double value)
{
	ref[tag] = count;
	tags[count] = tag;
	cons[count] = FitCon(tag, label, value);
	count++;
}


FitCon FitConSet::GetCon(wxString tag)
{
	return cons[(int)ref[tag]];
}


FitSet::FitSet(int size)
{
	measureCount = 0;
	measureMax = size;
	measures = new FitMeasure[size];
	tags = new wxString[size];
}


FitSet::~FitSet()
{
	delete[] measures;
	delete[] tags;
}


void FitSet::AddMeasure(wxString tag, wxString label, double weight)
{
	ref[tag] = measureCount;
	tags[measureCount] = tag;
	measures[measureCount] = FitMeasure(tag, label, weight);
	measureCount++;
}


FitMeasure FitSet::GetMeasure(wxString tag)
{
	return measures[(int)ref[tag]];
}



double IoDcalc(int binsize, int spikecount, datdouble *times)
{
	int i;
	int maxbin = 10000;
	int spikerate[10000];
	int laststep;
	double mean, variance, dispersion = 0;
	double timeshift = 0;

	if((*times)[0] > 1000) timeshift = (*times)[0] - 1000;        // for data where recording starts at non-zero time point

	// calculate spike rate for binsize
	for(i=0; i<maxbin; i++) spikerate[i] = 0;
	for(i=0; i<spikecount; i++) if(((*times)[i] - timeshift) / binsize < maxbin) spikerate[(int)(((*times)[i] - timeshift) + 0.5) / binsize]++;
	laststep = ((int)((*times)[spikecount - 1] - timeshift)/ binsize) - 4;
	if(laststep > maxbin) laststep = maxbin;

	// calculate index of dispersion
	mean = 0;
	variance = 0;
	for(i=0; i<laststep; i++) mean = mean +  spikerate[i];	//mean
	mean = mean / laststep;
	for(i=0; i<laststep; i++) variance += (mean - spikerate[i]) * (mean - spikerate[i]);	// variance
	variance = variance / laststep;
	dispersion = variance / mean;		// dispersion

	return dispersion;
}