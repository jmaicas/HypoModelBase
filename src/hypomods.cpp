

#include <hypomodel.h>
#include <time.h>

DEFINE_EVENT_TYPE(wxEVT_SPIKE)
DEFINE_EVENT_TYPE(wxEVT_SYNCH)



NeuroMod::NeuroMod(int type, wxString name, HypoMain *main)
	: Model(type, name, main)
{
	evodata = NULL;
}


NeuroMod::~NeuroMod()
{
	if(evodata) delete evodata;
}


void NeuroMod::EvoGraphs()
{
	evodata->GraphSet(graphbase, "Evo ", purple, 1, "evo");

	GraphSet *graphset = graphbase->NewSet("Evo Intervals", "evointervals");
	graphset->AddFlag("hazmode1", 10);
	graphset->AddFlag("binrestog1", 1);
	graphset->AddFlag("burstmode", 100);
	graphset->AddFlag("normtog", 1000);
	graphset->AddFlag("quadtog", 10000);
	graphset->Add("evohist1ms", 0);
	graphset->Add("evohaz1ms", 10);
	graphset->Add("evohist5ms", 1);
	graphset->Add("evohaz5ms", 11);
	graphset->Add("evobursthist1ms", 100);
	graphset->Add("evobursthaz1ms", 110);
	graphset->Add("evobursthist5ms", 101);
	graphset->Add("evobursthaz5ms", 111);
	graphset->Add("evonormhist1ms", 1000);
	graphset->Add("evonormhist5ms", 1001);
	graphset->Add("evohistquadsmooth", 10001);
	graphset->Add("evohistquadsmooth", 11001);
	if(diagbox) diagbox->textbox->AppendText(graphset->Display());

	graphbase->Add(GraphDat(&evodata->IoDdata, 0, 70, 0, 2, "Evo IoD", 9, 1, lightred), "iodevo");
	graphbase->GetGraph("iodevo")->gdatax = &evodata->IoDdataX;
	graphbase->GetGraph("iodevo")->xcount = 7;  
	graphbase->GetGraph("iodevo")->synchx = false; 
	graphbase->GetGraph("iodevo")->barshift  = 20;
}



Model::Model(int type, wxString name, HypoMain *main)
{
	mainwin = main;
	modtype = type;
	modname = name;
	modtag = name;
	modthread = NULL;
	modbox = NULL;
	diagbox = NULL;
	graphbase = new GraphBase(10);
	graphbase->mainwin = mainwin;
	modeflags = new ParamStore();
	toolflags = new ParamStore();
	//prefstore = new ParamStore();
	scalebox = mainwin->scalebox;
	diagbox = mainwin->diagbox;

	ostype = GetSystem();
	graphload = true;
	gsync = 0;
	initparams = "inittest";
	storesize = 100000;
	xmin = -100000;
	path = "";
	oldhist = true;
	xscaletoggle = 0;

	prefstore["numdraw"] = 2;
}


Model::~Model()
{
	delete graphbase;
	delete modeflags;
	delete toolflags;
}


void Model::GSwitch(GraphDisp *gpos, ParamStore *gflags)
{
	int i, gdex;
	GraphSet *graphset;
	wxString text;

	mainwin->diagbox->Write("GSwitch call\n");

	if(gsmode == 1) {
		for(i=0; i<gcount; i++) {
			graphset = graphbase->GetSet(gcodes[i]);
			if(graphset) gdex = graphset->GetPlot(gflags);
			else continue;
			if(diagbox) diagbox->textbox->AppendText(text.Format("gpos %d   gcode %s   set %s   plot %d   modesum %d   sdex %d  sync %d\n", 
				i, gcodes[i], graphset->tag, gdex, graphset->modesum, graphset->sdex, (*graphbase)[gdex]->synchx));
			gpos[i].Front((*graphbase)[gdex]);
			gpos[i].sdex = graphset->sdex;
		}
	}
}


wxString Model::GetPath()
{
	wxString fullpath;

	if(mainwin->modpath == "") {
		if(path != "") fullpath = path;
		else fullpath = "Init";
	}
	else {
		if(path != "") fullpath = mainwin->modpath + "\\" + path;
		else fullpath = mainwin->modpath;
	}

	if(!wxDirExists(fullpath)) wxMkdir(fullpath);

	return fullpath;
}


void Model::RunModel()
{
	mainwin->SetStatusText("Base Model Run");

	//modthread->Create();
	//modthread->Run();
}


void Model::ModClose()
{
}


void Model::Output()
{
	int i;
}


void Model::ScaleSwitch(double newxscale)
{
	int i;
}


void Model::DataSelect(double x, double y)
{
	int i;
}


void Model::EvoRun()
{
}


void Model::SetCell(int cellindex, GraphDat *graph)
{
}


int Model::GetCellIndex()
{
	return 0;
}


void Model::ScaleCon(ScaleBox *scalebox, int condex)
{
}


long Model::ReadNextData(wxString *readline)
{
	long numdat;
	wxString numstring;

	*readline = readline->AfterFirst(' ');
	numstring = readline->BeforeFirst(' ');
	numstring.ToLong(&numdat);

	return numdat;
}


void Model::GHistStore()
{
	wxString filename, filepath;
	wxString text;
	TextFile outfile;
	int i;

	//if(path == "") filepath = "Init";
	//else filepath = path;
	filepath = GetPath();

	wxComboBox *gstag = mainwin->scalebox->gstag;
	
	filename = modname + "gshist.ini";
	outfile.New(filepath + "/" + filename);
	for(i=gstag->GetCount()-1; i>=0; i--) 
		outfile.WriteLine(text.Format("graph %s", gstag->GetString(i)));
	outfile.Close();
}


void Model::GHistLoad(wxComboBox *gstag)
{
	wxString filename, filepath;
	wxString readline, text;
	short check;
	TextFile infile, tofp;
	initgraph = "";

	//if(path == "") filepath = "Init";
	//else filepath = path;
	filepath = GetPath();

	bool diagflag = false;

	if(diagflag) tofp.New("histdiag.txt");

	filename = modname + "gshist.ini";
	
	check = infile.Open(filepath + "/" + filename);
	if(!check) return;
	readline = infile.ReadLine();
	if(mainwin->diagnostic && diagflag) tofp.WriteLine(readline);
	while(!readline.IsEmpty()) {
		readline = readline.AfterFirst(' ');
		//readline.Trim();
		initgraph = readline;
		gstag->Insert(initgraph, 0);
		if(diagflag) tofp.WriteLine(text.Format("insert %s", initgraph));
		readline = infile.ReadLine();
		if(diagflag) tofp.WriteLine(readline);
	}
	infile.Close();	

	/*
	wxTextFile opfile(filename);
	if(!opfile.Exists()) return;

	opfile.Open();
	readline = opfile.GetLine(0);
	while(!readline.IsEmpty()) {
		readline = readline.AfterFirst(' ');
		readline.Trim();
		initparams = readline;
		gstag->Insert(initgraph, 0);
		readline = opfile.GetNextLine();
	}
	opfile.Close();	*/

	gstag->SetLabel(initgraph);

	if(diagflag) tofp.Close();
}

/*
void Model::GStore(wxComboBox *gstag)
{
	int i;
	wxString filename, filetag;
	wxString outline, text;
	TextFile outfile;

	filetag = gstag->GetValue();
	
	// Tag history
	short tagpos = gstag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) gstag->Delete(tagpos);
	gstag->Insert(filetag, 0);

	filename = "graph-" + filetag + ".dat";
	outfile.New(filename);
	for(i=0; i<gcount; i++)
		outfile.WriteLine(text.Format("%d %s", i, gcodes[i]));
	outfile.Close();
}*/

/*
void Model::GLoad(wxComboBox *gstag)
{
	long numdat;
	short check;
	int gindex;
	wxString filename, filetag;
	wxString readline, numstring, glabel;
	TextFile infile;

	filetag = gstag->GetValue();

	// Tag history
	short tagpos = gstag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) gstag->Delete(tagpos);
	gstag->Insert(filetag, 0);

	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	filename = "graph-" + filetag + ".dat";
	check = infile.Open(filename);
	if(!check) return;
	readline = infile.ReadLine();

	while(!readline.IsEmpty()) {
		numstring = readline.BeforeFirst(' ');
		numstring.ToLong(&numdat);
		gindex = numdat;
		glabel = readline.AfterFirst(' ');
		gcodes[gindex] = glabel;
		readline = infile.ReadLine();
	}
	infile.Close();
}*/


void Model::ModStore()
{
	int i;
	int prefcount;
	wxString filename, filepath;
	wxString outline, text;

	TextFile outfile, opfile;

	filepath = GetPath();

	// Default Parameters

	//if(modbox->defstore) 
		
  //modbox->StoreParam("default");


	// Prefs                                  February 2018
	filename = modname + "prefs.ini";
	outfile.New(filepath + "/" + filename);

	prefcount = prefstore.size();
	for(i=0; i<prefcount; i++) {
		outfile.WriteLine(text.Format("%s %.0f", prefstore.gettag(i), prefstore[i]));
	}
	outfile.Close();

	// Parameter history                   - no longer in use, replaced by box specific file tag history 
	if(oldhist) {
		filename = modname + "prefs.ini";
		initparams = modbox->paramstoretag->GetValue();

		//wxTextFile opfile("Init//" + filename);
		//if(!opfile.Exists()) opfile.Create();

		opfile.New(filepath + "/" + filename);

		for(i=modbox->paramstoretag->GetCount()-1; i>=0; i--) {
			outline.Printf("initparams %s", modbox->paramstoretag->GetString(i));
			opfile.WriteLine(outline);
		}
		opfile.Close();
	}

	// box store
	filename = modname + "box.ini";
	outfile.New(filepath + "/" + filename);
	
	for(i=0; i<modtools.numtools; i++)
		if(modtools.box[i]) {
			outfile.WriteLine(text.Format("%d %d %d %d %d %d %s", i, 
			modtools.box[i]->mpos.x, modtools.box[i]->mpos.y, modtools.box[i]->boxsize.x, modtools.box[i]->boxsize.y, 
			modtools.box[i]->IsVisible(), modtools.box[i]->boxname));
		}
	outfile.Close();
}


void Model::ModLoad()
{
	long numdat;
	int check, boxindex;

	wxString filename, filepath;
	wxString readline, numstring;
	wxString tag;

	TextFile infile, opfile;
	wxPoint pos;
	wxSize size;

	diagbox->Write("ModLoad....\n");

	filepath = GetPath();

	// parameter history load                       // Redundant, leave in for updating old models
	if(oldhist) {
		if(!modbox->paramstoretag->labelset) {
			filename = modname + "prefs.ini";
			check = opfile.Open(filepath + "/" + filename);
			if(!check) return;
			readline = opfile.ReadLine();
			while(!readline.IsEmpty()) {
				readline = readline.AfterFirst(' ');
				readline.Trim();
				initparams = readline;
				modbox->paramstoretag->Insert(initparams, 0);
				readline = opfile.ReadLine();
			}
			opfile.Close();	
			modbox->paramstoretag->SetLabel(initparams);
		}
	}

	// Prefs load
	filename = modname + "prefs.ini";
	check = infile.Open(filepath + "/" + filename);
	if(check) {
		readline = infile.ReadLine();
		while(!readline.IsEmpty()) {
				tag = readline.BeforeFirst(' ');
				readline = readline.AfterFirst(' ');
				readline.Trim();
				readline.ToLong(&numdat);
				prefstore[tag] = numdat;
				readline = infile.ReadLine();
		}
		infile.Close();
	}

	diagbox->Write("ModLoad history ok, reading boxes...\n");

	// Box Load
	filename = modname + "box.ini";

	check = infile.Open(filepath + "/" + filename);
	if(!check) return;
	readline = infile.ReadLine();
	//tofp.WriteLine(readline);
	while(!readline.IsEmpty()) {
		numstring = readline.BeforeFirst(' ');
		numstring.ToLong(&numdat);
		boxindex = numdat;
		if(boxindex >= modtools.numtools) break;
		pos.x = ReadNextData(&readline);
		//if(pos.x < 0) pos.x = 0;
		pos.y = ReadNextData(&readline);
		size.x = ReadNextData(&readline);
		size.y = ReadNextData(&readline);
		if(modtools.box[boxindex]->servant) modtools.box[boxindex]->visible = (bool)ReadNextData(&readline);
		else modtools.box[boxindex]->visible = true;

		//if(pos.x >= 0 && pos.x < 5000 && pos.y >= 0 && pos.y < 5000) modtools.box[boxindex]->mpos = pos;
		if(pos.x >= -5000 && pos.x < 5000 && pos.y >= -5000 && pos.y < 5000) modtools.box[boxindex]->mpos = pos;
		if(size.x >= 50 && size.x < 2000 && size.y >= 50 && size.y < 2000) modtools.box[boxindex]->boxsize = size;

		readline = infile.ReadLine();          // Read next line
		//tofp.WriteLine(readline);
	}
	infile.Close();

	diagbox->Write("ModLoad....OK\n");
}


int Model::SoundLink(SpikeDat **dataref, datdouble **waveref)
{
	dataref = NULL;
	waveref = NULL;
	return -1;
}


int Model::ModeSum(ParamStore *gflags)
{
	return 0;
}



