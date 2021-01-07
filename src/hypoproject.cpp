

#include "hypoproject.h"



Project::Project(HypoMain *main)
{
	mainwin = main;
	mod = NULL;
	protag = "";
	path = "";
}

/*
Project::Project(Model *model, wxString projtag)
{
mod = model;
protag = projtag;
path = mod->GetPath();

tagcount = 0;

boxfile = mod->modname + "-" + protag + "-box.ini";
graphfile = mod->modname + "-" + protag + "-graph.ini";
tagfile = mod->modname + "-" + protag + "-tag.ini";
projectfile = mod->modname + "-" + protag + "-project.ini";
}
*/


void Project::Init(wxString tag, Model *model)
{
	wxString text;

	if(!model) mod = mainwin->mod;
	else mod = model;

	protag = tag;
	path = mod->GetPath();

	//boxfile = mod->modname + "-" + protag + "-box.ini";
	//prefsfile = mod->modname + "-" + protag + "-prefs.ini";
	boxfile = protag + "-box.ini";
	prefsfile = protag + "-prefs.ini";

	mainwin->diagbox->Write(text.Format("Project boxfile %s\n", boxfile));
}


void Project::Store()
{
	int i;
	TextFile outfile;
	wxString text, filepath;
	
	filepath = path + "/" + projectfile;

	outfile.Open(filepath);
	outfile.WriteLine("box " + boxfile);
	outfile.WriteLine("box " + boxfile);
	outfile.WriteLine("box " + boxfile);

	// Prefs Store
	outfile.New(filepath + "/" + prefsfile);
	for(i=0; i<mod->prefstore.size(); i++) {
		outfile.WriteLine(text.Format("%s %.0f", mod->prefstore.gettag(i), mod->prefstore[i]));
	}
	outfile.Close();

	// Box Store
	outfile.New(path + "/" + boxfile);
	for(i=0; i<mod->modtools.numtools; i++)
		if(mod->modtools.box[i]) {
			outfile.WriteLine(text.Format("%d %d %d %d %d %d %s", i, 
				mod->modtools.box[i]->mpos.x, mod->modtools.box[i]->mpos.y, mod->modtools.box[i]->boxsize.x, mod->modtools.box[i]->boxsize.y, 
				mod->modtools.box[i]->IsVisible(), mod->modtools.box[i]->boxtag));
		}
	outfile.Close();

	// Tag Store


}


void Project::Load()
{
	int i;
	TextFile infile;
	wxString text, filepath;
	wxString readline, numstring, tag;
	long numdat;
	int boxindex;
	bool check;
	wxPoint pos;
	wxSize size;

	filepath = path + "/" + projectfile;

	// Prefs Load
	check = infile.Open(filepath + "/" + prefsfile);
	if(!check) mainwin->diagbox->Write("Project prefs file not found\n");
	if(check) {
		readline = infile.ReadLine();
		while(!readline.IsEmpty()) {
			tag = readline.BeforeFirst(' ');
			readline = readline.AfterFirst(' ');
			readline.Trim();
			readline.ToLong(&numdat);
			mod->prefstore[tag] = numdat;
			readline = infile.ReadLine();
		}
		infile.Close();
	}
	mainwin->numdraw_set = mod->prefstore["numdraw"];
	mainwin->GraphPanelsUpdate();

	// Box Load
	check = infile.Open(filepath + "/" + boxfile);
	if(!check) return;
	readline = infile.ReadLine();
	
	while(!readline.IsEmpty()) {
		numstring = readline.BeforeFirst(' ');
		numstring.ToLong(&numdat);
		boxindex = numdat;
		if(boxindex >= mod->modtools.numtools) break;
		pos.x = ReadNextData(&readline);
		pos.y = ReadNextData(&readline);
		size.x = ReadNextData(&readline);
		size.y = ReadNextData(&readline);
		if(mod->modtools.box[boxindex]->servant) mod->modtools.box[boxindex]->visible = (bool)ReadNextData(&readline);
		else mod->modtools.box[boxindex]->visible = true;
		if(pos.x >= -5000 && pos.x < 5000 && pos.y >= -5000 && pos.y < 5000) mod->modtools.box[boxindex]->mpos = pos;
		if(size.x >= 50 && size.x < 2000 && size.y >= 50 && size.y < 2000) mod->modtools.box[boxindex]->boxsize = size;
		readline = infile.ReadLine();  // Read next line										
	}

	infile.Close();
	for(i=0; i<mod->modtools.numtools; i++) {
		mod->modtools.box[i]->ReSize();
		mod->modtools.box[i]->Show(mod->modtools.box[i]->visible);
	}

	// Tag Load

}



void Project::TagSetDisp()
{
	int i;
	wxString text;

	for(i=0; i<tagset->numtags; i++) {
		mainwin->diagbox->Write(text.Format("Tag %d %s\n", i, tagset->tagdata[i].tag));
	}
}