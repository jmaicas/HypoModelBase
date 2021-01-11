

#include "hypomodel.h"
#include <time.h>
#include <wx/dir.h>
#include <wx/filename.h>

DEFINE_EVENT_TYPE(wxEVT_SPIKE)
DEFINE_EVENT_TYPE(wxEVT_SYNCH)

//wxDECLARE_EVENT(wxEVT_COMMAND_MODTHREAD_COMPLETED, wxThreadEvent);
//wxDEFINE_EVENT(wxEVT_COMMAND_MODTHREAD_COMPLETED, wxThreadEvent);

wxDEFINE_EVENT(EVT_MODTHREAD_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(EVT_DIAG_WRITE, wxThreadEvent);


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
	graphwin = mainwin->graphwin;
    
    //wxDEFINE_EVENT(EVT_MODTHREAD_COMPLETED, wxThreadEvent);
    //wxDEFINE_EVENT(EVT_DIAGNOSTIC, wxCommandEvent);

	ostype = GetSystem();
	graphload = true;
	gsync = 0;
	initparams = "inittest";
	storesize = 100000;
	xmin = -100000;
	path = "";
	oldhist = true;
	xscaletoggle = 0;
	burstmode = 0;
	projmode = 0;  // 0 for old model based prefs and box store, 1 for new Project store
    
    runflag = false;   // Set to indicate model thread is running
    runmute = new wxMutex;

	for(int i=0; i<10; i++) gtags[i] = "";    // plot tags indexed by graph window, only used for submenu graph sets

	prefstore["numdraw"] = 2;
	evoflag = false;
    
    Connect(wxID_ANY, EVT_MODTHREAD_COMPLETED, wxThreadEventHandler(Model::OnModThreadCompletion));
    Connect(wxID_ANY, EVT_DIAG_WRITE, wxThreadEventHandler(Model::OnDiagWrite));
}


Model::~Model()
{
	delete graphbase;
	delete modeflags;
	delete toolflags;
}


void Model::OnModThreadCompletion(wxThreadEvent&)
{
    runmute->Lock();
    runflag = 0;
    runmute->Unlock();
    mainwin->scalebox->GraphUpdate();

    diagbox->textbox->AppendText("thread exit\n");
}


void Model::OnDiagWrite(wxThreadEvent& event)
{
    diagbox->Write(event.GetString());
}


void Model::DiagWrite(wxString text)
{
    //diagevent.SetString(text);
    //AddPendingEvent(diagevent);
    //wxQueueEvent(this, &diagevent);
    
    wxThreadEvent event(EVT_DIAG_WRITE, ID_Diagnostic);
    event.SetString(text);
    //pFrame is a wxEvtHandler*
    QueueEvent(event.Clone());
    
}


void Model::DataCopy(wxString oldpath, wxString newpath)
{
    wxString oldparampath, parampath;
    wxString oldgraphpath, graphpath;
    wxString oldgridpath, gridpath;
    wxString filepath;
    wxFileName filename;
    
    oldparampath = oldpath + path + "/Params/";
    parampath = newpath + path + "/Params/";
    wxMkdir(parampath);
    filepath = wxFindFirstFile(oldparampath + "*.*");
    while(!filepath.empty()) {
        filename.Assign(filepath);
        wxCopyFile(filepath, parampath + filename.GetFullName());
        //diagbox->Write("filepath " + filepath + "\n");
        //diagbox->Write("filename " + filename.GetFullName() + "\n");
        filepath = wxFindNextFile();
    }
    
    oldgraphpath = oldpath + path + "/Graphs/";
    graphpath = newpath + path + "/Graphs/";
    wxMkdir(graphpath);
    filepath = wxFindFirstFile(oldgraphpath + "*.*");
    while(!filepath.empty()) {
        filename.Assign(filepath);
        wxCopyFile(filepath, graphpath + filename.GetFullName());
        filepath = wxFindNextFile();
    }
    
    oldgridpath = oldpath + path + "/Grids/";
    gridpath = newpath + path + "/Grids/";
    wxMkdir(gridpath);
    filepath = wxFindFirstFile(oldgridpath + "*.*");
    while(!filepath.empty()) {
        filename.Assign(filepath);
        wxCopyFile(filepath, gridpath + filename.GetFullName());
        filepath = wxFindNextFile();
    }
}


//void Model::SoundOn()
//{}


void Model::SpikeDataSwitch(SpikeDat *data)
{}


void Model::BurstUpdate()
{}


void Model::GridColumn(int col)
{}


void Model::GSwitch(GraphDisp *gpos, ParamStore *gflags, int command)
{
	int i, gdex;
	GraphSet *graphset;
	wxString text;
	GraphDat *refgraph, *newgraph;

	mainwin->diagbox->Write("GSwitch call\n");

	if(gsmode == 1) {
		for(i=0; i<gcount; i++) {
			graphset = graphbase->GetSet(gcodes[i]);
			if(!graphset) continue;
			gdex = graphset->GetPlot(i, gflags);
			if(!gdex) continue;
			if(diagbox) diagbox->textbox->AppendText(text.Format("gpos %d   gcode %s   set %s   plot %d   modesum %d   sdex %d  sync %d  current %d\n", 
				i, gcodes[i], graphset->tag, gdex, graphset->modesum, graphset->sdex, (*graphbase)[gdex]->synchx, graphset->current));

			// Graph Switch commands
			if(command == XSYNCH) {
				refgraph = gpos[i].GetFront();
				newgraph = (*graphbase)[gdex];
				newgraph->xto = refgraph->xto;
				newgraph->xfrom = refgraph->xfrom;
			}

			gpos[i].Front((*graphbase)[gdex]);
			gpos[i].sdex = graphset->sdex;
		}
	}
}

/*
refgraph = graphwin[pan1]->dispset[0]->plot[0];
for(i=0; i<graphwin[pan2]->numdisps; i++) {
	graph = graphwin[pan2]->dispset[i]->plot[0];
	graph->yto = refgraph->yto;
	graph->yfrom = refgraph->yfrom;
	graph->xto = refgraph->xto;
	graph->xfrom = refgraph->xfrom;
}
*/


wxString Model::GetPath()
{
	wxString fullpath, text;

	if(mainwin->modpath == "") {
		if(path != "") fullpath = mainwin->mainpath + path;
		else fullpath = mainwin->mainpath + "Init";
	}
	else {
		if(path != "") fullpath = mainwin->modpath + "/" + path;
		else fullpath = mainwin->modpath;
	}

	if(!wxDirExists(fullpath)) wxMkdir(fullpath);
    mainwin->diagbox->Write(text.Format("Model GetPath %s\n", fullpath));

	return fullpath;
}


void Model::RunModel()
{
	mainwin->SetStatusText("Base Model Run");

	//modthread->Create();
	//modthread->Run();
}



int Model::GetCellIndex()
{
	return 0;
}


//void Model::ScaleConsoleBelow(ScaleBox *scalebox, int condex)
//{}


long Model::ReadNextData(wxString *readline)
{
	long numdat;
	wxString numstring;

	*readline = readline->AfterFirst(' ');
	numstring = readline->BeforeFirst(' ');
	numstring.ToLong(&numdat);

	return numdat;
}


void Model::GHistStore()     // 6/1/21 being phased out of use, replaced with TagBox
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


void Model::GHistLoad(wxComboBox *gstag)     // 6/1/21 being phased out of use, replaced with TagBox
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
			modtools.box[i]->IsVisible(), modtools.box[i]->boxtag));
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


int Model::SoundLink(SoundBox *soundbox)
{
	soundbox->spikedata = NULL;
	soundbox->wavedata = NULL;
	return -1;
}


int Model::ModeSum(ParamStore *gflags)
{
	return 0;
}


