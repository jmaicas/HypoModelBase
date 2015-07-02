

#include <hypomodel.h>
#include <time.h>

DEFINE_EVENT_TYPE(wxEVT_SPIKE)
DEFINE_EVENT_TYPE(wxEVT_SYNCH)



Model::Model(short type, wxString name, HypoMain *main)
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
	scalebox = mainwin->scalebox;

	ostype = GetSystem();
	graphload = true;
	gsync = 0;
	initparams = "inittest";
	storesize = 100000;
	xmin = -100000;
	path = "";
}


Model::~Model()
{
	delete graphbase;
	delete modeflags;
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


void Model::DataSelect(double x, double y)
{
	int i;
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

	if(mainwin->diagnostic) tofp.New("histdiag.txt");

	filename = modname + "gshist.ini";
	
	check = infile.Open(filepath + "/" + filename);
	if(!check) return;
	readline = infile.ReadLine();
	if(mainwin->diagnostic) tofp.WriteLine(readline);
	while(!readline.IsEmpty()) {
		readline = readline.AfterFirst(' ');
		//readline.Trim();
		initgraph = readline;
		gstag->Insert(initgraph, 0);
		if(mainwin->diagnostic) tofp.WriteLine(text.Format("insert %s", initgraph));
		readline = infile.ReadLine();
		if(mainwin->diagnostic) tofp.WriteLine(readline);
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

	if(mainwin->diagnostic) tofp.Close();
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
	short i;
	wxString filename, filepath;
	wxString outline, text;

	TextFile outfile, opfile;

	//if(path == "") filepath = "Init";
	//else filepath = path;
	//if(!wxDirExists(filepath)) wxMkdir(filepath);
	filepath = GetPath();

	// Default Parameters

	//if(modbox->defstore) 
		
  //modbox->StoreParam("default");

	// Parameter history
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

	TextFile infile, opfile;
	wxPoint pos;
  wxSize size;

	diagbox->Write("ModLoad....\n");

	filepath = GetPath();

	// parameter history load                       // Redundant, leave in for updating old models
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