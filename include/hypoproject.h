#pragma once


#include "wx/wx.h"
#include "hypodat.h"
#include <hypotools.h>
#include <hypomods.h>


class Project
{
public:
	Model *mod;
	TagSet *tagset;
	wxString protag;
	HypoMain *mainwin;
	//TagDat tagset[100];
	//int tagcount;

	wxString path;
	wxString boxfile;
	wxString graphfile;
	//wxString paramfile;
	wxString tagfile;
	wxString projectfile;

	//Project(Model *mod, wxString tag);
	Project(HypoMain *mainwin);

	void Load();
	void Store();
	void TagSetDisp();
	//void AddTag(wxString tag, TagBox *box);
};