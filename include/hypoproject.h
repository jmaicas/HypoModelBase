#pragma once


//#include "wx/wx.h"
#include "hypodat.h"
#include "hypotools.h"
#include "hypomods.h"
#include "hypomodel.h"


class Project
{
public:
	Model *mod;
	TagSet *tagset;
	wxString protag;
	HypoMain *mainwin;

	wxString path;
	wxString boxfile;
	wxString graphfile;
	wxString prefsfile;
	wxString tagfile;
	wxString projectfile;

	Project(HypoMain *mainwin);

	void Load();
	void Store();
	void TagSetDisp();
	void Init(wxString tag, Model *mod=NULL);
};