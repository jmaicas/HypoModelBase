

#include "hypoproject.h"



Project::Project(HypoMain *main)
{
	mainwin = main;
	mod = NULL;
	protag = "";
	path = "";
	//tagcount = 0;
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


/*
void Project::AddTag(wxString tag, TagBox *box)
{
tagset[tagcount].tag = tag;
tagset[tagcount].box = box;
tagcount++;
}
*/


void Project::Store()
{
	TextFile outfile;
	wxString text, filepath;

	filepath = path + "/" + projectfile;

	outfile.Open(filepath);
	outfile.WriteLine("box " + boxfile);
	outfile.WriteLine("box " + boxfile);
	outfile.WriteLine("box " + boxfile);
}


void Project::Load()
{}



void Project::TagSetDisp()
{
	int i;


}