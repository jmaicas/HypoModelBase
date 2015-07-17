

#include <hypomodel.h>
#include <wx/notebook.h>
#include <wx/clipbrd.h>
//#include "hypomain.h"
//#include "hypopanels.h"

#ifdef HYPOSOUND
#include "SineWave.h"
#include "Noise.h"
#include "RtWvOut.h"
#include <cstdlib>

using namespace stk;
#endif


ModGenBox::ModGenBox(HypoMain *main, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ToolBox(main, title, pos, size)
{
	int i;
	int pagecount;
	wxString text;

	toolset = main->toolset;
	paramset = new ParamSet(panel);
	page = new wxRichTextCtrl*[10];

	//wxTextCtrl *page1;

	//paramset->AddNum("modname", "Mod Name", 10, 0, 50, 150);
	paramset->AddText("modname", "Mod Name", "Test", 50, 150);
	paramset->AddText("modtag", "Mod Tag", "test", 50, 150);
	paramset->AddNum("modindex", "Index", 9, 0);

	wxBoxSizer *parambox = new wxBoxSizer(wxVERTICAL);

	for(i=0; i<paramset->numparams; i++) {
		parambox->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		parambox->AddSpacer(5);
	}

	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Generate, "Generate", 60, buttonbox);
	buttonbox->AddSpacer(10);
	AddButton(ID_Output, "Output", 60, buttonbox);

	notebook = new wxNotebook(panel, -1, wxPoint(-1,-1), wxSize(-1, 400), wxNB_TOP);
	pagecount = 6;
	for(i=0; i<pagecount; i++) {
		page[i] = new wxRichTextCtrl(notebook, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
		notebook->AddPage(page[i], text.Format("Page %d", i+1));
		//page1 = new wxTextCtrl(panel, -1, "test", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
		//notebook->AddPage(page1, text.Format("Page %d", i+1));
	}

	//wxTextAttr *style;
	//style = new wxTextAttr();
	//style->SetTabs(100);
	/*wxArrayInt tabs;
	tabs.Add(20);
	tabs.Add(40);
	tabs.Add(60);
	tabs.Add(80);
	wxRichTextAttr style;
	style.SetFlags(wxTEXT_ATTR_TABS);
	style.SetTabs(tabs);*/

	//for(i=0; i<pagecount; i++) {
		//page[i]->SetDefaultStyle(style);
		
	//}

	status = StatusBar();
	wxBoxSizer *statusbox = new wxBoxSizer(wxHORIZONTAL);
	statusbox->Add(status, 1, wxEXPAND);

	mainbox->AddSpacer(5);
	mainbox->Add(parambox, 0, wxALIGN_CENTRE_HORIZONTAL);
	//mainbox->AddSpacer(5);
	mainbox->Add(buttonbox, 0, wxALIGN_CENTRE_HORIZONTAL);
	mainbox->AddSpacer(5);
	mainbox->Add(notebook, 1, wxEXPAND);
	//mainbox->AddSpacer(5);
	mainbox->Add(statusbox, 0, wxEXPAND);

	panel->Layout();

	Connect(ID_Generate, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ModGenBox::OnGen));
	Connect(ID_Output, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ModGenBox::OnOutput));
}


void ModGenBox::DefGen()
{
	int i, check;
	wxString inpath, outpath;
	TextFile infile;

	inpath = "src\\";
	outpath = "Newcode\\";

	check = infile.Open(inpath + plate[i]);
		if(!check) {
			status->SetLabel("Template files missing");
			return;
		}
}


void ModGenBox::OnGen(wxCommandEvent& event)
{
	modname = paramset->GetText("modname");
	modtag = paramset->GetText("modtag");
	modnum = paramset->GetText("modindex");

	// template files
	plate[0] = "xxxmodel.h";
	plate[1] = "xxxmodel.cpp";
	plate[2] = "xxxmod.cpp";
	plate[3] = "xxxpanels.cpp";
	platecount = 4;

	//GenModCode();

	Read();
}


void ModGenBox::Read()
{
	int i, check, pageindex;
	wxString inpath, outpath, readline;
	wxString outname, defname;
	wxString modTAG = modtag.Upper();
	TextFile infile, outfile;

	inpath = "Template\\";

	status->SetLabel("");

	for(i=0; i<platecount; i++) {
		page[i]->Clear();
		outname = plate[i];
		outname.Replace("xxx", modtag);
		notebook->SetPageText(i, outname);

		check = infile.Open(inpath + plate[i]);
		if(!check) {
			status->SetLabel("Template files missing");
			return;
		}

		while(!infile.file->Eof()) {
			readline = infile.ReadLine();
			readline.Replace("xxxx", modname);
			readline.Replace("xxx", modtag);
			readline.Replace("XXX", modTAG);
			readline.Replace("OOO", modnum);
			//Line(i, readline);
			page[i]->AppendText(readline + "\n");
			//outfile.WriteLine(readline);
		}

		infile.Close();
		//outfile.Close();
	}

	// hypodef.cpp : read and update

	//inpath = "src\\";
	PageRead(platecount, "hypodef.h", "include\\"); 
	PageRead(platecount+1, "hypodef.cpp", ""); 
}


void ModGenBox::OnOutput(wxCommandEvent& event)
{
	int i;
	wxString text, filename, outpath, outname;
	TextFile outfile;

	status->SetLabel("Output Init");

	outpath = "Newcode\\";
	if(!wxDirExists(outpath)) wxMkdir(outpath);

	for(i=0; i<platecount+2; i++) {
		outname = notebook->GetPageText(i);
		page[i]->SaveFile(outpath + outname, wxRICHTEXT_TYPE_TEXT);
	}

	status->SetLabel("Output OK");
	//if(mainwin->diagnostic) mainwin->mod->diagbox->
}


void ModGenBox::PageRead(int pageindex, wxString name, wxString path)
{
	int check;
	TextFile infile;
	wxString text, readline;

	page[pageindex]->Clear();
	notebook->SetPageText(pageindex, name);

	check = infile.Open(path + name);
	if(!check) {
		status->SetLabel(text.Format("%s file missing", name));
		return;
	}

	while(!infile.file->Eof()) {
			readline = infile.ReadLine();
			page[pageindex]->AppendText(readline + "\n");
	}
	infile.Close();
}


void ModGenBox::Line(int pindex, wxString code)
{
	wxString text;

	code.Replace("%", "\%");
	text.Printf(code);
	page[pindex]->AppendText(text + "\n");
}


void ModGenBox::GenModCode()
{
	wxString text;
	int pageindex;

	// xmod.h

	pageindex = 0;
	page[pageindex]->Clear();
	notebook->SetPageText(pageindex, text.Format("%smod.h", modtag));
	text.Printf("#ifndef %sMOD_H\n#define %sMOD_H\n\n\n#include \"wx/wx.h\"\n#include \"hypomodel.h\"\n", modname, modname);
	page[pageindex]->AppendText(text);
	text.Printf("\n\nenum {\n\tID_randomflag = %d00,\n\tID_heatflag\n};\n", modindex);
	page[pageindex]->AppendText(text);
	text.Printf("\n\nclass %sModel : public Model\n{\npublic:\n\t%sModel(int, wxString, HypoMain *);\n", modname, modname);
	page[pageindex]->AppendText(text);
	text.Printf("\n\tvoid RunModel();\n\tvoid GraphData();\n\tvoid GSwitch(graphdisp *gpos, ParamStore *gflags);\n};\n");
	page[pageindex]->AppendText(text);


	// xmodel.cpp

	pageindex = 1;
	page[pageindex]->Clear();
	notebook->SetPageText(pageindex, text.Format("%smodel.cpp", modtag));
	text.Printf("#include \"%smod.h\"\n\n %sDat::%sDat()\n\{\n\tunsigned int size = 350000;\n\tunsigned int max = 100000;\n\n\twater.data.resize(size);", modtag, modname, modname);
	page[pageindex]->AppendText(text);
	text.Printf("\n\twater.max = max;\n\tsalt.data.resize(size);\n\tsalt.max = max;\n\tosmo.data.resize(size);\n\tosmo.max = max;\n\theat.data.resize(size);\n\theat.max = max;\n}");
	page[pageindex]->AppendText(text);
}


InfoBox::InfoBox(HypoMain *main, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ToolBox(main, title, pos, size)
{
	mainwin = main;
	artparams = new ParamStore;
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *datfilebox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *datbuttons = new wxBoxSizer(wxHORIZONTAL);
	datfiletag = new wxTextCtrl(panel, wxID_ANY, "n0", wxDefaultPosition, wxSize(100, -1));
	//wxButton *datstorebutton = new wxButton(panel, ID_datstore, "Store", wxDefaultPosition, wxSize(40, buttonheight));
	wxButton *datloadbutton = new wxButton(panel, ID_datload, "Load", wxDefaultPosition, wxSize(40, buttonheight));
	//synccheck = new wxCheckBox(panel, wxID_ANY, "Sync");
	//synccheck->SetValue(true);
	datfilebox->Add(datfiletag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	//datbuttons->Add(datstorebutton, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2); 
	datbuttons->Add(datloadbutton, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2); 
	datfilebox->Add(datbuttons, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	//paramfilebox->Add(synccheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	wxBoxSizer *artspikebox = new wxBoxSizer(wxVERTICAL);
	paramset = new ParamSet(panel);
	labelwidth = 50;
	paramset->AddNum("artfreq", "Freq", 4, 0, labelwidth); 
	paramset->AddNum("artburst", "Burst", 100, 0, labelwidth); 
	paramset->AddNum("artsilence", "Silence", 50, 0, labelwidth); 
	paramset->AddNum("startspike", "Start", 0, 0, labelwidth); 
	paramset->AddNum("endspike", "End", 0, 0, labelwidth); 

	for(i=0; i<paramset->numparams; i++) {
		artspikebox->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		//artspikebox->AddSpacer(2);
	}

	hbox->Add(artspikebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 0);
	//hbox->AddSpacer(10);
	//hbox->Add(rightbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 0);

	mainbox->Add(datfilebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->Add(hbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);

	panel->Layout();

	Connect(ID_datload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(InfoBox::OnDatLoad));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(InfoBox::OnClose));
}


void InfoBox::GetArtParams()
{
	for(i=0; i<paramset->numparams; i++)
		paramset->con[i]->numbox->GetValue().ToDouble(&((*artparams)[paramset->con[i]->name]));
}


void InfoBox::OnClose(wxCloseEvent& event)
{
	//mainwin->infobox = NULL;
	//Destroy();
	//mainwin->infobox->Show(false);
	Show(false);
}


void InfoBox::OnDatLoad(wxCommandEvent& event)
{
	int count;
	double datval;
	double rawdata[100000];
	wxString filetag, filename;
	wxString readline, datname;

	// Param data file
	filetag = datfiletag->GetValue();
	filename = "data/" + filetag + ".dat";

	wxTextFile datfile(filename);

	if(!datfile.Exists()) {
		datfiletag->SetValue("Not found");
		return;
	}
	datfile.Open();

	readline = datfile.GetLine(0);
	readline = readline.AfterFirst('\'');
	datname = readline.BeforeFirst('\'');
	datfiletag->SetValue(datname);
	count = 0;
	readline = datfile.GetLine(3);
	while(readline.GetChar(0) != ':') {
		readline.Trim();
		readline.ToDouble(&datval);
		rawdata[count++] = datval * 1000;
		readline = datfile.GetLine(3 + count);	
	}

	for(i=0; i<count-1; i++) mainwin->expdata->isis[i] = rawdata[i+1] - rawdata[i];
	mainwin->expdata->spikecount = count-1;
	mainwin->expdata->start = rawdata[0];
	mainwin->expdata->neurocalc();
	//mainwin->scalebox->GSwitch(exp_dat);
	snum.Printf("Read %d spikes", count);
	datfiletag->SetValue(snum);
	datfile.Close();
}





/*
BurstDispData::BurstDispData(wxPanel *pan)
{
panel = pan;
}*/


void BurstBox::BurstDataPanel(BurstPanel *datpanel)
{
	int numwidth = 50;

	datpanel->numbursts = NumPanel(50);
	datpanel->meanspikes = NumPanel(50);
	datpanel->meanlength = NumPanel(50);
	datpanel->meansilence = NumPanel(50);
	datpanel->sdlength = NumPanel(50);
	datpanel->sdsilence = NumPanel(50);
	datpanel->actQ = NumPanel(50);
	datpanel->modetime = NumPanel(50);
	datpanel->moderate = NumPanel(50);

	datpanel->intraspikes = NumPanel(numwidth);
	datpanel->intrafreq = NumPanel(numwidth);
	datpanel->intraisimean = NumPanel(numwidth);
	datpanel->intraisisd = NumPanel(numwidth);
}


BurstBox::BurstBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, SpikeDat *sdat, wxString intratag)
	: ToolBox(model->mainwin, title, pos, size)
{
	int numwidth = 50;
	int gridwidth = 65;
	int numpan = 3;

	units = 1000;
	boxname = "SpikeAnalysis";

	mainwin = model->mainwin;
	//burstdata = bdat;
	burstparams = new ParamStore;
	//spikedata = burstdata->spikedata;
	spikedata = sdat;
	moddata = sdat;
	blankevent = new wxCommandEvent();

	activepanel = panel;
	BurstPanel *burstpanset[5];

	maxint = 1500;
	minspikes = 25;
	maxspikes = 0;
	startspike = 0;
	endspike = 0;

	loaddata = NULL;

	//wxFont textfont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	//SetFont(textfont);

	//panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer *parambox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

	//wxBoxSizer *mainbox = new wxBoxSizer(wxVERTICAL);

	paramset = new ParamSet(panel);
	paramset->AddNum("maxint", "Max Interval", 1500, 0); 
	paramset->AddNum("minspikes", "Min Spikes", 25, 0); 
	paramset->AddNum("maxspikes", "Max Spikes", 0, 0); 
	paramset->AddNum("startspike", "Start", 0, 0); 
	paramset->AddNum("endspike", "End", 0, 0); 

	for(i=0; i<paramset->numparams; i++) {
		parambox->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
		parambox->AddSpacer(5);
	}

	datburst = new BurstPanel();
	modburst = new BurstPanel();
	evoburst = new BurstPanel();

	BurstDataPanel(datburst);
	BurstDataPanel(modburst);
	BurstDataPanel(evoburst);

	burstpanset[0] = datburst;
	burstpanset[1] = modburst;
	burstpanset[2] = evoburst;


	/*
	numbursts = NumPanel(50);
	meanspikes = NumPanel(50);
	meanlength = NumPanel(50);
	meansilence = NumPanel(50);
	sdlength = NumPanel(50);
	sdsilence = NumPanel(50);

	datnumbursts = NumPanel(50);
	datmeanspikes = NumPanel(50);
	datmeanlength = NumPanel(50);
	datmeansilence = NumPanel(50);
	datsdlength = NumPanel(50);
	datsdsilence = NumPanel(50);
	*/

	allspikes = NumPanel(numwidth);
	allfreq = NumPanel(numwidth);
	allisimean = NumPanel(numwidth);
	allisisd = NumPanel(numwidth);

	/*
	intraspikes = NumPanel(numwidth);
	intrafreq = NumPanel(numwidth);
	intraisimean = NumPanel(numwidth);
	intraisisd = NumPanel(numwidth);

	datintraspikes = NumPanel(numwidth);
	datintrafreq = NumPanel(numwidth);
	datintraisimean = NumPanel(numwidth);
	datintraisisd = NumPanel(numwidth);*/

	if(ostype == Mac) gridwidth = 45; else gridwidth = 30;
	wxFlexGridSizer *datagrid = new wxFlexGridSizer(2, 3, 3);
	datagrid->Add(GridLabel(gridwidth, "Spikes"), 0, wxALIGN_CENTRE);
	datagrid->Add(allspikes);
	datagrid->Add(GridLabel(gridwidth, "Freq"), 0, wxALIGN_CENTRE);
	datagrid->Add(allfreq);
	datagrid->Add(GridLabel(gridwidth, "Mean"), 0, wxALIGN_CENTRE);
	datagrid->Add(allisimean);
	datagrid->Add(GridLabel(gridwidth, "SD"), 0, wxALIGN_CENTRE);
	datagrid->Add(allisisd);

	wxFlexGridSizer *intragrid = new wxFlexGridSizer(4, 3, 3);
	intragrid->Add(GridLabel(gridwidth, "Spikes"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) intragrid->Add(burstpanset[i]->intraspikes);
	//intragrid->Add(modburst->intraspikes);
	//intragrid->Add(datburst->intraspikes);
	//intragrid->Add(evoburst->intraspikes);
	intragrid->Add(GridLabel(gridwidth, "Freq"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) intragrid->Add(burstpanset[i]->intrafreq);
	//intragrid->Add(modburst->intrafreq);
	//intragrid->Add(datburst->intrafreq);
	//intragrid->Add(evoburst->intrafreq);
	intragrid->Add(GridLabel(gridwidth, "Mean"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) intragrid->Add(burstpanset[i]->intraisimean);
	//intragrid->Add(modburst->intraisimean);
	//intragrid->Add(datburst->intraisimean);
	//intragrid->Add(evoburst->intraisimean);
	intragrid->Add(GridLabel(gridwidth, "SD"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) intragrid->Add(burstpanset[i]->intraisisd);
	//intragrid->Add(modburst->intraisisd);
	//intragrid->Add(datburst->intraisisd);
	//intragrid->Add(evoburst->intraisisd);

	wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, panel, "All Data");
	//databox->Add(new wxStaticText(panel, wxID_ANY, "All Data"), 0, wxALIGN_CENTRE);
	//databox->Add(TextLabel("All Data"), 0, wxALIGN_CENTRE);
	if(ostype == Windows) databox->AddSpacer(5);
	databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	wxStaticBoxSizer *intrabox = new wxStaticBoxSizer(wxVERTICAL, panel, intratag);
	//intrabox->Add(TextLabel("Intra Burst"), 0, wxALIGN_CENTRE);
	if(ostype == Windows) intrabox->AddSpacer(5);
	intrabox->Add(intragrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	wxFlexGridSizer *burstgrid = new wxFlexGridSizer(4, 3, 5);
	if(ostype == Mac) gridwidth = 80; else gridwidth = 65;
	burstgrid->Add(GridLabel(45, "Bursts"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->numbursts);

	burstgrid->Add(GridLabel(gridwidth, "Mean Spikes"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->meanspikes);

	burstgrid->Add(GridLabel(gridwidth, "Mean Length"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->meanlength);

	burstgrid->Add(GridLabel(gridwidth, "Length SD"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->sdlength);

	burstgrid->Add(GridLabel(gridwidth, "Mean Silence"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->meansilence);

	burstgrid->Add(GridLabel(gridwidth, "Silence SD"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->sdsilence);

	burstgrid->Add(GridLabel(gridwidth, "Activity Q"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->actQ);

	burstgrid->Add(GridLabel(gridwidth, "Mode Time"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->modetime);

	burstgrid->Add(GridLabel(gridwidth, "Mode Rate"), 0, wxALIGN_CENTRE);
	for(i=0; i<numpan; i++) burstgrid->Add(burstpanset[i]->moderate);

	wxBoxSizer *datfilebox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *datbuttons = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *datcon = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *datbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *rightbox = new wxBoxSizer(wxVERTICAL);

	//datfiletag = new wxTextCtrl(panel, wxID_ANY, "n0", wxDefaultPosition, wxSize(100, -1));
	//datfiletag = TextInput(100, 20, "n0");
	wxString histpath = mainwin->modpath + "\\" + "Tools";
	if(!wxDirExists(histpath)) wxMkdir(histpath);
	datfiletag = TextInputCombo(100, 20, "", boxname, histpath);

	if(ostype == Mac) {
		AddButton(ID_datload, "Load", 60, datbuttons, 2);
		AddButton(ID_datoutput, "Output Data", 100, datcon, 2);
		AddButton(ID_scan, "Burst Scan", 90, parambox, 2);
	}
	else {
		AddButton(ID_datload, "Load", 40, datbuttons, 2);
		AddButton(ID_datoutput, "Output Data", 80, datcon, 2);
		AddButton(ID_scan, "Burst Scan", 65, parambox, 5);
	}

	//wxButton *scanbutton = new wxButton(panel, ID_scan, "Burst Scan", wxDefaultPosition, wxSize(65, -1));
	//rightbox->Add(scanbutton, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	//rightbox->AddStretchSpacer(10);
	rightbox->Add(burstgrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	//rightbox->AddStretchSpacer(10);

	hbox->Add(parambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 0);
	hbox->AddSpacer(10);
	hbox->Add(rightbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALL, 0);

	wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
	hbox2->Add(databox, 1, wxALIGN_CENTRE_VERTICAL| wxALIGN_CENTRE_HORIZONTAL);
	hbox2->AddSpacer(20);
	//hbox2->AddStretchSpacer(20);
	hbox2->Add(intrabox, 1, wxALIGN_CENTRE_VERTICAL| wxALIGN_CENTRE_HORIZONTAL);

	//wxButton *datoutputbutton = new wxButton(panel, ID_datoutput, "Output Data", wxDefaultPosition, wxSize(80, buttonheight));	
	datstatus = NumPanel(100, wxALIGN_RIGHT, "");
	//datcon->Add(datoutputbutton, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2); 
	datcon->AddSpacer(5);
	datcon->Add(datstatus, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	datfilebox->Add(GridLabel(40, "Data file"), 0, wxALIGN_CENTRE|wxALL, 2);
	datfilebox->Add(datfiletag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	//datbuttons->Add(datloadbutton, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2); 
	datfilebox->Add(datbuttons, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	wxRadioButton *sec = new wxRadioButton(panel, ID_s, "s", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	wxRadioButton *msec  = new wxRadioButton(panel, ID_ms, "ms");
	datfilebox->AddSpacer(10);
	datfilebox->Add(sec, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	datfilebox->Add(msec, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	datbox->Add(datfilebox, 0);
	//datbox->AddStretchSpacer(5);
	if(ostype == Mac) datbox->AddStretchSpacer(5); else datbox->AddSpacer(2);
	datbox->Add(datcon, 0);

	if(ostype == Windows) mainbox->AddSpacer(3);
	mainbox->Add(datbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	//mainbox->Add(datcon, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	mainbox->AddStretchSpacer();
	mainbox->Add(hbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	//mainbox->AddSpacer(5);
	mainbox->AddStretchSpacer();
	mainbox->Add(hbox2, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	mainbox->AddSpacer(5);

	panel->SetSizer(mainbox);
	panel->Layout();
	if(spikedata != NULL) SpikeDataDisp();

	snum.Printf("con x %d", paramset->con[0]->numwidth + paramset->con[0]->labelwidth);
	datstatus->SetLabel(snum);

	Connect(ID_scan, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BurstBox::OnScan));
	Connect(ID_datload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BurstBox::OnDatLoad));
	Connect(ID_datoutput, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BurstBox::OnDatOutput));
	Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(BurstBox::OnDatRadio));
	//Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(BurstBox::OnClose));
}


BurstBox::~BurstBox()
{
	delete burstparams;
	delete paramset;
	delete datburst;
	delete modburst;
	delete evoburst;
	delete blankevent;
}


void BurstBox::OnDatRadio(wxCommandEvent& event)
{
	if(event.GetId() == ID_s) units = 1000;
	if(event.GetId() == ID_ms) units = 1;
}


void BurstBox::SpikeDataDisp(SpikeDat *dispdata)
{
	if(dispdata == NULL) dispdata = spikedata;

	snum.Printf("%d", dispdata->spikecount);
	allspikes->SetLabel(snum);
	snum.Printf("%.2f", dispdata->freq);
	allfreq->SetLabel(snum);
	snum.Printf("%.2f", dispdata->meanisi);
	allisimean->SetLabel(snum);
	snum.Printf("%.2f", dispdata->isisd);
	allisisd->SetLabel(snum);
}


void BurstBox::Scan(SpikeDat *data)
{	
	spikedata = data;
	OnScan(*blankevent);
}


void BurstBox::OnScan(wxCommandEvent& WXUNUSED(event))
{
	//mainwin->SetStatus("Burst Scan");
	//for(i=0; i<paramset->numparams; i++)
	//	paramset->con[i]->numbox->GetValue().ToDouble(&((*burstparams)[paramset->con[i]->name]));
	//GetParams();

	if(spikedata == NULL && mainwin->focusdata != NULL) spikedata = mainwin->focusdata;
	if(spikedata != NULL && spikedata->burstdata == NULL) {
		spikedata->burstdata = new BurstDat();
		//spikedata->burstdata->spikedata = spikedata;
	}
	//spikedata = burstdata->spikedata;
	if(spikedata == NULL || spikedata->spikecount == 0) {
		snum.Printf("no data");
		modburst->numbursts->SetLabel(snum);
		return;
	}
	//snum.Printf("scan...");
	//numbursts->SetLabel(snum);

	if(loaddata != NULL) {
		loaddata->BurstScan(this);
		if(loaddata->burstdata->numbursts > 0) loaddata->BurstProfile();
		BurstDataDisp(loaddata, datburst);
	}

	if(moddata != NULL) {
		//if(moddata->spikecount == 0) 
		moddata->BurstScan(this);
		//if(moddata->burstdata->numbursts > 0) moddata->BurstProfile();
		BurstDataDisp(moddata, modburst);
	}

	//if(spikedata->burstdata->numbursts > 0) spikedata->BurstProfile();
	//BurstDataDisp();

	//mainwin->gpos[0].burstdata = spikedata->burstdata;
	//burstdata->spikedata = spikedata;
	mainwin->scalebox->BurstDisp(1);
}


ParamStore *BurstBox::GetParams()
{ 
	for(i=0; i<paramset->numparams; i++)
		paramset->con[i]->numbox->GetValue().ToDouble(&((*burstparams)[paramset->con[i]->name]));
	return burstparams;
}


void BurstBox::BurstDataDisp(SpikeDat *dispdata, BurstPanel *datpanel)
{
	if(dispdata == NULL) dispdata = spikedata; 
	if(datpanel == NULL) datpanel = modburst;

	// Display scan data
	snum.Printf("%d", dispdata->burstdata->numbursts);
	datpanel->numbursts->SetLabel(snum);
	snum.Printf("%.2f", dispdata->burstdata->meancount);
	datpanel->meanspikes->SetLabel(snum);
	snum.Printf("%.2f", dispdata->burstdata->meanlength);
	datpanel->meanlength->SetLabel(snum);
	snum.Printf("%.2f", dispdata->burstdata->sdlength);
	datpanel->sdlength->SetLabel(snum);
	snum.Printf("%.2f", dispdata->burstdata->meansilence);
	datpanel->meansilence->SetLabel(snum);
	snum.Printf("%.2f", dispdata->burstdata->sdsilence);
	datpanel->sdsilence->SetLabel(snum);
	snum.Printf("%.3f", dispdata->burstdata->actQ);
	datpanel->actQ->SetLabel(snum);

	datpanel->modetime->SetLabel(numtext(dispdata->burstdata->pmodetime, 3));
	datpanel->moderate->SetLabel(numtext(dispdata->burstdata->pmoderate, 3));

	snum.Printf("%d", dispdata->burstdata->intraspikes);
	datpanel->intraspikes->SetLabel(snum);
	snum.Printf("%.2f", dispdata->burstdata->freq);
	datpanel->intrafreq->SetLabel(snum);
	snum.Printf("%.2f", dispdata->burstdata->meanisi);
	datpanel->intraisimean->SetLabel(snum);
	snum.Printf("%.2f", dispdata->burstdata->isisd);
	datpanel->intraisisd->SetLabel(snum);
}


void BurstBox::OnDatLoad(wxCommandEvent& event)
{
	int s, count, maxdata, stretchdata;
	double datval, spikeint;
	//double rawdata[100000];
	datdouble rawdata;
	wxString readline, filename;

	filterthresh = 2;
	maxdata = 100000;
	stretchdata = maxdata;
	rawdata.setsize(maxdata);

	// Spike data file
	filetag = datfiletag->GetValue();
	//filename = "data/" + filetag + ".dat";
	filename = mainwin->datapath + "\\" + filetag + ".dat";
	datname = filetag;

	wxTextFile *datfile;

	datfile = new wxTextFile(filename);

	if(!datfile->Exists()) {
		//filename = "data/" + filetag + ".txt";
		filename = mainwin->datapath + "\\" + filetag + ".txt";
		datfile = new wxTextFile(filename);
		if(!datfile->Exists()) {
			datfiletag->SetValue("Not found");
			return;
		}
	}

	if(!loaddata) {
		datfiletag->SetValue("Bad Data Store");
		return;
	}

	// Dat file history
	short tagpos = datfiletag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) datfiletag->Delete(tagpos);
	datfiletag->Insert(filetag, 0);

	//redtag = "";
	//datfiletag->SetForegroundColour(blackpen);
	datfiletag->SetValue("");
	datfiletag->SetValue(filetag);

	datfile->Open();
	count = 0;
	loaddata->Clear();

	readline = datfile->GetFirstLine();
	datfiletag->SetValue(readline);
	if(readline.GetChar(0) == '\'') {
		readline = readline.AfterFirst('\'');
		datname = readline.BeforeFirst('\'');
	}
	else if(readline.GetChar(0) == '\"') {
		readline = datfile->GetNextLine();
		readline = readline.AfterFirst('\"');
		datname = readline.BeforeFirst('\"');
	}
	datfiletag->SetValue(datname);

	if(readline.ToDouble(&datval)) rawdata[count++] = datval * 1000;
	readline = datfile->GetNextLine();

	while(readline.IsEmpty() || readline.GetChar(0) == '\"' || readline.GetChar(0) == ':') 
		readline = datfile->GetNextLine();

	while(!readline.IsEmpty() && readline.GetChar(0) != '\"' && readline.GetChar(0) != ':') {
		readline.Trim();
		readline.ToDouble(&datval);
		rawdata[count++] = datval * units;
		readline = datfile->GetNextLine();
		if(count == stretchdata) {
			stretchdata = stretchdata + 1000;
			rawdata.setsize(stretchdata);
			loaddata->ReSize(stretchdata);
		}
	}
	loaddata->isis[0] = 0;
	s = 1;
	for(i=0; i<count-1; i++) {
		spikeint = rawdata[i+1] - rawdata[i];
		if(spikeint > filterthresh) loaddata->isis[s++] = spikeint;
	}

	// 

	for(i=0; i<count; i++) loaddata->times[i] = rawdata[i];                // February 2014  interval recode, need to fix filtering

	loaddata->spikecount = count;
	loaddata->start = rawdata[0];
	loaddata->neurocalc();
	//mainwin->scalebox->expdatflag = 1;
	//mainwin->scalebox->GSwitch();
	//mainwin->scalebox->GSwitch(exp_dat);
	snum.Printf("Read %d spikes", count);
	//datfiletag->SetValue(snum);
	spikedata = loaddata;
	//SpikeDataDisp(mainwin->expdata);


	SpikeDataDisp(loaddata);

	mainwin->scalebox->ratedata = 0;

	mainwin->scalebox->BurstDisp(0);
	mainwin->scalebox->GraphSwitch();

	datfile->Close();
}


void BurstBox::OnDatOutput(wxCommandEvent& event)
{
	wxString outdir = mainwin->outpath + "/output";

	loaddata->output(filetag, outdir);
	snum.Printf("%s output", filetag);
	datstatus->SetLabel(snum);
}

/*
void BurstBox::OnClose(wxCloseEvent& event)
{
//mainwin->burstbox = NULL;
mainwin->toolset.RemoveBox(setindex);
Destroy();
}
*/


/*
DataBox::DataBox(MainFrame *main, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(main, -1, title, pos, size,
wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX | wxRESIZE_BORDER)
{
mainwin = main;

panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
wxBoxSizer *mainbox = new wxBoxSizer(wxVERTICAL);

panel->SetSizer(mainbox);
panel->Layout();


//Connect(ID_scan, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BurstBox::OnScan));
Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(OnClose));
}*/

#ifdef HYPOSOUND

SoundBox::SoundBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size, SpikeDat *sdat)
	: ParamBox(mod, title, pos, size)
{
	int i;
	short stype;
	spikedata = sdat;
	soundon = 0;
	soundmutex = new wxMutex;
	//vasomod = (VasoModel)mod;

	paramset->AddNum("soundfreq", "Sound Freq", 400, 2);
	paramset->AddNum("pulsefreq", "Pulse Freq", 10, 2);
	paramset->AddNum("pulsedur", "Pulse Dur", 20, 2);
	paramset->AddNum("pulseint", "Pulse Int", 500, 0);
	paramset->AddNum("freqscale", "Freq Scale", 20, 0);
	paramset->AddNum("playspikes", "Play Spikes", 100, 0);
	paramset->AddNum("timerate", "Time Rate", 1, 1);


	SetVBox(1);

	for(i=0; i<paramset->numparams; i++) {
		vbox[0]->Add(paramset->con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);	
	}

	numspikes = NumPanel();

	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Go, "Test", 60, buttonbox);
	buttonbox->AddSpacer(10);
	AddButton(ID_spikes, "Spikes", 60, buttonbox);
	buttonbox->AddSpacer(10);
	AddButton(ID_Wave, "Wave", 60, buttonbox);
	buttonbox->AddSpacer(10);
	AddButton(ID_Stop, "Stop", 60, buttonbox);

	parambox->Add(vbox[0], 0);
	mainbox->AddSpacer(5);
	mainbox->Add(parambox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->Add(numspikes, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->Add(buttonbox, 1,  wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	panel->Layout();

	stype = mod->SoundLink(&spikedata, &wavedata);
	if(spikedata != NULL) snum.Printf("%d", spikedata->spikecount); else snum.Printf("bad %d", stype);
	numspikes->SetLabel(snum);

	Connect(ID_Go, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnGo));
	Connect(ID_Stop, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnStop));
	Connect(ID_spikes, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnSpikes));
	Connect(ID_Wave, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnWave));
}


void SoundBox::OnGo(wxCommandEvent& event)
{
	SoundTest();
}


void SoundBox::OnStop(wxCommandEvent& event)
{
	soundmutex->Lock();
	soundon = 0;
	soundmutex->Unlock();
}


void SoundBox::OnWave(wxCommandEvent& event)
{
	if(soundon) return;

	//if(spikedata == NULL && mainwin->focusdata != NULL) spikedata = mainwin->focusdata;
	//spikedata = mainwin->currvaso;
	//snum.Printf("%d", spikedata->spikecount);
	//numspikes->SetLabel(snum);
	//datdouble wavedata = (VasoModel)mod->vasodata->Ca;

	SoundGen *soundgen = new SoundGen(wavedata, GetParams(), this);
	soundgen->Create();
	soundon = 1;
	soundgen->Run(); 
}



void SoundBox::OnSpikes(wxCommandEvent& event)
{

	if(soundon) return;

	SoundGen *soundgen = new SoundGen(spikedata, GetParams(), this);
	soundgen->Create();
	soundon = 1;
	soundgen->Run(); 

	//if(spikedata == NULL && mainwin->focusdata != NULL) spikedata = mainwin->focusdata;
	//spikedata = mod->mainwin->mod->currvaso;
	/*
	if(spikedata != NULL) {
	snum.Printf("%d", spikedata->spikecount);
	numspikes->SetLabel(snum);
	SoundGen *soundgen = new SoundGen(spikedata, GetParams(), this);
	soundgen->Create();
	soundon = 1;
	soundgen->Run(); 
	}
	else {
	snum.Printf("no data");
	numspikes->SetLabel(snum);
	}
	*/

	//SpikeSonic();
}


void SoundBox::SpikeSonic()
{
	int i, s;
	int mode = 1;
	SineWave sine;
	RtWvOut *dac = NULL;

	Stk::setSampleRate(96000.0);
	Stk::showWarnings(true);
	GetParams();
	snum.Printf("%d", spikedata->spikecount);

	//snum.Printf("%d", 100);
	numspikes->SetLabel(snum);

	dac = new RtWvOut(1);
	sine.setFrequency((*modparams)["soundfreq"]);

	int msamp = 96;
	int pulseint = (*modparams)["pulseint"];
	int freqscale = (*modparams)["freqscale"];
	int freq = (*modparams)["pulsefreq"];
	int fsamp = 1000/freq;
	int pulsedur = (*modparams)["pulsedur"];
	double sdur;
	int playspikes = (*modparams)["playspikes"];

	//for(s=0; s<freq*2; s++) {
	//	for(i=0; i<(fsamp-pulsedur)*msamp; i++) dac->tick(0);
	//	for(i=0; i<pulsedur*msamp; i++) dac->tick(sine.tick());
	//}
	if(mode == 1) {
		for(s=0; s<playspikes; s++) {
			fsamp = spikedata->isis[s];
			//sine.setFrequency(freqscale*1000/fsamp);
			for(i=0; i<(fsamp-pulsedur)*msamp; i++) dac->tick(0);
			for(i=0; i<pulsedur*msamp; i++) dac->tick(sine.tick());
		}
	}

	if(mode == 2) {
		for(s=0; s<playspikes; s++) {
			fsamp = spikedata->isis[s];
			sine.setFrequency(freqscale*1000/fsamp);
			for(i=0; i<(pulseint-pulsedur)*msamp; i++) dac->tick(0);
			for(i=0; i<pulsedur*msamp; i++) dac->tick(sine.tick());
		}
	}

cleanup:
	delete dac;
}


void SoundBox::SoundTest()
{
	int i;

	Stk::setSampleRate(96000.0);
	Stk::showWarnings(true);

	GetParams();

	int nFrames = 10000;
	SineWave sine, sine2, sine3;
	Noise noise;
	RtWvOut *dac = 0;

	dac = new RtWvOut(1);

	sine.setFrequency((*modparams)["soundfreq"]);
	sine2.setFrequency(541.0);
	sine3.setFrequency(641.0);

	int s;
	int msamp = 96;
	int freq = (*modparams)["pulsefreq"];
	int fsamp = 1000/freq;
	int pulsedur = (*modparams)["pulsedur"];

	for(s=0; s<freq*2; s++) {
		for(i=0; i<(fsamp-pulsedur)*msamp; i++) dac->tick(0);
		for(i=0; i<pulsedur*msamp; i++) dac->tick(sine.tick());
	}

cleanup:
	delete dac;
}


#endif