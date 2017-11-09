#include <plotmodel.h>


using namespace std;


PlotDat::PlotDat(int size)
{
	water.setsize(size);
	salt.setsize(size);
	osmo.setsize(size);
	heat.setsize(size);
}


PlotModel::PlotModel(int type, wxString name, HypoMain *main)
	: Model(type, name, main)
{
	int i;
	int numview = 2;
	int celldatamax = 100;

	wheelmode = false;
	cellmode = true;
	plotmode = false;

	storesize = 100000;
	int wheelstoresize = 1000000;
	int binstoresize = 100000;

	mainwin->SetMinSize(wxSize(470, 690));
	mainwin->xstretch = 0;
	xmin = 0;
	path = "Plot";

	diagbox = mainwin->diagbox;
	filebase = new FileBase(100);

	plotdata = new PlotDat(storesize);
	for(i=0; i<10; i++) coldata[i].setsize(storesize);
	xcoldata.setsize(storesize);

	wheeldata.setsize(wheelstoresize);
	bindata.setsize(binstoresize);
	//spikedata = new SpikeDat();
	viewcell = new SpikeDat[numview];
	//celldata = new NeuroDat[10];
	celldata.resize(10);
	//for(i=0; i<numcurrcells; i++) cellcount[i] = 0;

	plotbox = new PlotBox(this, "plotbox", modname + " Model", wxPoint(0, 0), wxSize(320, 500));
	mainwin->plotbox = plotbox;
	databox = new PlotDataBox(this, "Data Grid", wxPoint(0, 0), wxSize(320, 500), 1000, 20);
	cellbox = new CellBox(this, "Neuro Data", wxPoint(0, 0), wxSize(320, 500));

	//cellbox->cells = &celldata;
	cellbox->cells = &celldata;
	cellbox->currcell = &viewcell[0];
	cellbox->currcell->burstdata = new BurstDat();
	cellbox->currcell->burstdata->spikedata = cellbox->currcell;
	cellbox->currcell->burstdata->test = 20;
	cellbox->currcell->diagbox = diagbox;
	cellbox->textgrid = databox->textgrid;

	plotbox->textgrid = databox->textgrid;

	modtools.AddBox(plotbox, true);
	modtools.AddBox(databox, true);
	modtools.AddBox(cellbox, true);
	modbox = plotbox;

	ModLoad();
	for(i=0; i<modtools.numtools; i++) {
		modtools.box[i]->ReSize();
		modtools.box[i]->Show(modtools.box[i]->visible);
	}

	plotbox->ParamLoad("default");
	graphload = false;

	gsync = 0;

	//plotbox->Show(true);
	GraphData();
}


void PlotModel::DataSelect(double from, double to)
{
    cellbox->SetSelect(from, to);
}


void PlotModel::ModClose()
{
	mainwin->diagbox->Write("Mod Close\n");
	plotbox->StoreParam("default");
	filebase->Store(FileDat("filebase.txt", GetPath()));
}


void PlotModel::StoreClear()
{
	int i;

	for(i=0; i<storesize; i++) {
		plotdata->water[i] = 0;
	}
}


void PlotModel::RunModel()
{
	if(mainwin->diagnostic) mainwin->SetStatusText("Plot Model Run");
	modthread = new PlotMod(this);
	modthread->Create();
	modthread->Run();
}


PlotModel::~PlotModel()
{
	delete plotdata;
	delete filebase;
	delete[] viewcell;

	//delete[] celldata;
	//delete spikedata;
}


void PlotModel::GraphData()
{
	int i;
	int numcols;
	wxString text, tag;
	GraphSet *graphset;

	// Data graphs
	//
	// GraphDat(data pointer, xfrom, xto, yfrom, yto, label string, graph type, bin size, colour)
	// ----------------------------------------------------------------------------------

	tag = "cell0";

	viewcell[0].GraphSet(graphbase, "Cell ", green, 1, tag, "Select ");

	graphset = graphbase->NewSet("Cell Spikes", "cellspikes");
	graphset->AddFlag("timeres", 1);
	graphset->AddFlag("rateres", 10);
	graphset->Add(tag + "rate1s", 0);
	graphset->Add(tag + "spikes1ms", 1);
	graphset->Add(tag + "rate10s", 10);
	if(diagbox) diagbox->textbox->AppendText(graphset->Display());

	graphset = graphbase->NewSet("Cell Intervals", "cellintervals");
	graphset->AddFlag("binrestog1", 1);
	graphset->AddFlag("hazmode1", 10);
	graphset->AddFlag("normtog", 100);
	graphset->AddFlag("allselect", 1000);
	graphset->Add("cell0hist1ms", 0);
	graphset->Add("cell0hist5ms", 1);
	graphset->Add("cell0haz1ms", 10);
	graphset->Add("cell0haz5ms", 11);
	graphset->Add("cell0normhist1ms", 100);
	graphset->Add("cell0normhist5ms", 101);
	graphset->Add("cell0haz1ms", 110);
	graphset->Add("cell0haz5ms", 111);

	graphset->Add("cell0bursthist1ms", 1000);
	graphset->Add("cell0bursthist5ms", 1001);
	graphset->Add("cell0bursthaz1ms", 1010);
	graphset->Add("cell0bursthaz5ms", 1011);
	graphset->Add("cell0burstnormhist1ms", 1100);
	graphset->Add("cell0burstnormhist5ms", 1101);
	graphset->Add("cell0bursthaz1ms", 1110);
	graphset->Add("cell0bursthaz5ms", 1111);

	if(diagbox) diagbox->textbox->AppendText(graphset->Display());

	graphbase->Add(GraphDat(&viewcell[0].IoDdata, 0, 100, 0, 100, "IoD Cell", 9, 1, lightblue), "iodcell", "iodcell");
	graphbase->GetGraph("iodcell")->gdatax = &viewcell[0].IoDdataX;
	graphbase->GetGraph("iodcell")->xcount = 7;  
	graphbase->GetGraph("iodcell")->synchx = false;

	graphbase->Add(GraphDat(&viewcell[0].burstdata->IoDdata, 0, 100, 0, 100, "IoD Cell Selected", 9, 1, lightgreen), "iodcellselect", "iodcell");
	graphbase->GetGraph("iodcellselect")->gdatax = &viewcell[0].burstdata->IoDdataX;
	graphbase->GetGraph("iodcellselect")->xcount = 7;  
	graphbase->GetGraph("iodcellselect")->synchx = false;

	graphset = graphbase->NewSet("Cell IoD", "celliod");
	graphset->AddFlag("allselect", 1);
	graphset->Add("iodcell", 0);
	graphset->Add("iodcellselect", 1);

	//graphbase->Add(GraphDat(&wheeldata, 0, 2000, 0, 100, "wheel", 5, 1, lightblue), "wheel");
	//graphbase->Add(GraphDat(&bindata, 0, 2000, 0, 100, "bin", 5, 1, lightblue), "bin");

	numcols = 10;
	for(i=0; i<numcols; i++) {
		graphbase->Add(GraphDat(&(coldata[i]), 0, 2000, 0, 100, text.Format("Data %d", i), 2, 1, lightgreen), text.Format("coldata%d", i));
	}

	gcodes[0] = "cellspikes";
	gcodes[1] = "cellintervals";
	gcodes[2] = "coldata0";
	gcodes[3] = "coldata1";
	gcodes[4] = "coldata2";
	gcodes[5] = "coldata2";

	gcount = 6;
	gsmode = 1;

	plotbox->graph = graphbase->GetGraph("coldata0");   // Set default graph panel selection
	plotbox->graphdisp = &mainwin->gpos[2];
}


void PlotModel::GSwitch(GraphDisp *gpos, ParamStore *gflags)
{
	int i, gdex;
	GraphSet *graphset;
	wxString text;

	// Specify graphs for display

	// Position 0
	gpos[0].Front((*graphbase)["water"]);

	// Position 1
	gpos[1].Front((*graphbase)["salt"]);

	// Position 2
	gpos[2].Front((*graphbase)["osmo"]);

	// Position 3
	gpos[3].Front((*graphbase)["heat"]);

	mainwin->diagbox->Write("GSwitch call\n");

	if(gsmode == 1)
		for(i=0; i<gcount; i++) {
			graphset = graphbase->GetSet(gcodes[i]);
			gdex = graphset->GetPlot(gflags);
			if(diagbox) diagbox->textbox->AppendText(text.Format("gpos %d   gcode %s   set %s   plot %d   modesum %d   sdex %d\n",
				i, gcodes[i], graphset->tag, gdex, graphset->modesum, graphset->sdex));
			gpos[i].Front((*graphbase)[gdex]);
			gpos[i].sdex = graphset->sdex;
		}
}


void PlotModel::NeuroAnalysis()
{
	int i, j;
	int binsize = 5;
	int timerange = 1000;
	int gridmax = 500;
	int histcount;
	wxString text;

	ParamStore *calcparams = cellbox->GetParams();
	viewcell[0].normscale = (*calcparams)["normscale"];
	timerange = (*calcparams)["histrange"];
	//binsize = (*calcparams)["binsize"];
	if(timerange < 0) timerange = 0;
	histcount = timerange / binsize;
	if(histcount > gridmax) histcount = gridmax;

	for(i=0; i<cellcount; i++) {
		//diagbox->Write(text.Format("Scanning cell %d %s ", i, celldata[i].name));
		databox->histgrid->SetCell(0, i, celldata[i].name);
		databox->hazgrid->SetCell(0, i, celldata[i].name);
		viewcell[0].neurocalc(&(celldata[i]));
		//diagbox->Write("OK\n"); 
		for(j=0; j<histcount; j++) {
			databox->histgrid->SetCell(j+1, i, text.Format("%.4f", viewcell[0].hist5norm[j]));
			databox->hazgrid->SetCell(j+1, i, text.Format("%.4f", viewcell[0].haz5[j]));
		}
	}
	databox->notebook->SetPageText(1, text.Format("%dms Histograms", binsize));
	databox->notebook->SetPageText(2, text.Format("%dms Hazards", binsize));
}


void PlotModel::WheelBin(int binsize)
{
	int i;

	datacount = 100000;
	for(i=0; i<100000; i++) bindata[i] = 0;  
	for(i=0; i<datacount; i++) bindata[i/binsize] += wheeldata[i];   
}


void FileBase::Store(FileDat file)
{
	int i;
	TextFile outfile;
	wxString text, filename, filetag;

	filename = file.path + "\\" + file.name;

	outfile.New(filename);
	for(i=0; i<numfiles; i++) {
		outfile.WriteLine(filestore[i].path + " " + filestore[i].name);
	}
	outfile.Close();
}


void FileBase::Load(FileDat file)
{
	int i;
}


