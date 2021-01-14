

#include "hypoproject.h"



Project::Project(HypoMain *main)
{
	mainwin = main;
	mod = NULL;
	protag = "";
	path = "";
	tagset = mainwin->tagset;
}


void Project::Init(wxString tag, Model *model)
{
	wxString text;

	if(!model) mod = mainwin->mod;
	else mod = model;

	protag = tag;
	path = mod->GetPath() + "/Projects";

	//boxfile = mod->modname + "-" + protag + "-box.ini";
	//prefsfile = mod->modname + "-" + protag + "-prefs.ini";
	boxfile = protag + "-box.ini";
	prefsfile = protag + "-prefs.ini";
	tagfile = protag + "-tags.ini";

	mainwin->diagbox->Write(text.Format("Project boxfile %s\n", boxfile));
}


void Project::TagSetDisp()
{
	int i;
	wxString text;

	for(i=0; i<tagset->numtags; i++) {
		mainwin->diagbox->Write(text.Format("Tag %d %s\n", i, tagset->tagdata[i].tag));
	}
}


void Project::Store()
{
	int i;
	TextFile outfile;
	wxString text, filepath, tagpath;
	
	if(!wxDirExists(path)) wxMkdir(path);
	filepath = path + "/" + projectfile;

	outfile.Open(filepath);
	outfile.WriteLine("box " + boxfile);
	outfile.WriteLine("box " + boxfile);
	outfile.WriteLine("box " + boxfile);

	// Prefs Store
	mod->prefstore["mainsizeX"] = mainwin->GetSize().GetX();
	mod->prefstore["mainsizeY"] = mainwin->GetSize().GetY();

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
	outfile.New(path + "/" + tagfile);
	for(i=0; i<tagset->numtags; i++) {
		outfile.WriteLine(text.Format("%s %s", tagset->tagdata[i].tag, tagset->tagdata[i].box->tagfilename));
	}
	outfile.Close();

}


void Project::Load()
{
	int i;
	TextFile infile;
	wxString text, filepath;
	wxString readline, numstring, tag, tagboxfile;
	long numdat;
	int boxindex;
	bool check;
	wxPoint pos;
	wxSize size;

	filepath = path + "/" + projectfile;

	// Prefs Load
	check = infile.Open(filepath + "/" + prefsfile);
	if(!check) mainwin->diagbox->Write("Project prefs file not found\n");
	else {
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

		mainwin->SetSize(wxSize(mod->prefstore["mainsizeX"], mod->prefstore["mainsizeY"]));

		mainwin->numdraw_set = mod->prefstore["numdraw"];
		mainwin->optionpanel->numdrawcon->SetValue(mainwin->numdraw_set);
		mainwin->GraphPanelsUpdate();
	}
	
	// Box Load
	check = infile.Open(filepath + "/" + boxfile);
	if(!check) mainwin->diagbox->Write("Project box file not found\n");
	else {
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
			mod->modtools.box[i]->SetPosition();
		}
	}

	// Tag Load
	check = infile.Open(filepath + "/" + tagfile);
	if(!check) mainwin->diagbox->Write("Project tag file not found\n");
	else {
		readline = infile.ReadLine();
		while(!readline.IsEmpty()) {
			tag = readline.BeforeFirst(' ');
			tagboxfile = readline.AfterFirst(' ');
			tagboxfile.Trim();
			//mainwin->diagbox->Write(text.Format("Project Load Tag %d %s %s\n", i, tag, tagboxfile));
			tagset->GetBox(tag)->SetFile(tagboxfile);
			readline = infile.ReadLine(); 
		}
	}
	mainwin->scalebox->GLoad();
}
