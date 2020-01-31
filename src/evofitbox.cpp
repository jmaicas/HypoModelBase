
#include "evofitbasic.h"
//#include "evofitvmn.h"
//#include <cuda.h>
//#include "vasomod.h"
#include <hypomodel.h>



EvoFitBox::EvoFitBox(Model *model, EvoChrome *chrome, const wxString& title, const wxPoint& pos, const wxSize& size, bool burstmode_set)
	: ParamBox(model, title, pos, size, "EVOFIT", 1)
{
	paramindex = 0;
	int i, gridwidth;
	int maxfit = 4;

	mod = model;
	fitchrome = chrome;
	numparams = fitchrome->numparams;

	fitdata = new FitDat();
	evofitdata = new FitDat();
	loadfitdata = new FitDat();
	fitset = new FitSet(20);
	fitconset = new FitConSet(20);
	//vasomod = mod;
	//chromepop = mod->chromepop;
	chromepopinit = false;
	//loaddata = vasomod->fitboxdata;

	moddata = NULL;
	expdata = NULL;
	loaddata = NULL;

	evodataset.resize(maxfit);

	for(i=0; i<maxfit; i++) {
		//evodataset.push_back(SpikeDat());
		//evodataset[i] = new SpikeDat();
		evodataset[i].burstdata = new BurstDat();
		evodataset[i].burstdata->spikedata = &evodataset[i];
	}

	//InitMenu();
	autoscore = true;
	burstmode = burstmode_set;

	// ParentData
	parentcount = 0;

	//SetModFlag(ID_revpots, "genlysis", "Generation Analysis", 0); 

	// Initialise fit measure
	//fitset->AddMeasure("ISImode", "ISImode", 5);
	//fitset->AddMeasure("ISImean", "ISImean", 5);
	fitset->AddMeasure("RMSFirstNBins", "RMSHistHead", 100);
	fitset->AddMeasure("RMSBinRange", "RMSBinRange", 100);
	fitset->AddMeasure("RMSHaz", "RMSHaz", 60);
	fitset->AddMeasure("RMSIoD", "RMSIoD", 20);

	if(burstmode) {
		fitset->AddMeasure("RMSBurstHead", "RMSBurstHead", 30);
		fitset->AddMeasure("BurstMode", "BurstMode", 20);
		fitset->AddMeasure("BurstLengthMean", "LengthMean", 30);
		fitset->AddMeasure("BurstLengthSD", "LengthSD", 5);
		fitset->AddMeasure("BurstSilenceMean", "SilenceMean", 15);
		fitset->AddMeasure("BurstSilenceSD", "SilenceSD", 5);
		fitset->AddMeasure("BurstIntraFreq", "BurstIntraFreq", 20);	
		fitset->AddMeasure("RMSBurstIoD", "BurstIoD", 20);	
	}
	
	// Fit measure parameters
	fitconset->AddCon("RMSHeadStart", "HeadStart", 0); 
	fitconset->AddCon("RMSHeadStop", "HeadStop", 30); 
	fitconset->AddCon("RMSBinRangeStart", "BinRangeStart", 30);
	fitconset->AddCon("RMSBinRangeFinish", "BinRangeFinish", 125);
	if(burstmode) fitconset->AddCon("RMSBurstHeadBins", "BurstHeadBins", 20);

	CreatePanels();
}


EvoFitBox::~EvoFitBox()
{
	delete modpanel;
	delete fitdatapanel;
	delete fitchecks;
	//delete evopanel;

	delete fitdata;
	delete evofitdata;
	delete loadfitdata;

	delete fitset;
	//delete fitchrome;
	delete fitconset;
}


wxFlexGridSizer *EvoFitBox::FitDataPanel(FitSet *fitset, DatPanel *datpanel)
{
	int i;
	int numwidth = 60;
	int gridwidth;

	wxFlexGridSizer *fitdatagrid = new wxFlexGridSizer(2, 3, 3);
	if(ostype == Mac) gridwidth = 45; else gridwidth = 80;

	for(i=0; i<fitset->measureCount; i++) {
		fitdatagrid->Add(GridLabel(gridwidth, fitset->measures[i].label), 0, wxALIGN_CENTRE);
		datpanel->AddDat(fitset->tags[i]);
		fitdatagrid->Add(datpanel->datset[i]);
	}

	fitdatagrid->Add(GridLabel(0, ""));
	fitdatagrid->Add(GridLabel(0, ""));

	fitdatagrid->Add(GridLabel(gridwidth, "Weighted Score"), 0, wxALIGN_CENTRE);
	datpanel->AddDat("weightscore");
	fitdatagrid->Add(datpanel->GetDat("weightscore"));

	return fitdatagrid;
}


wxFlexGridSizer *EvoFitBox::FitWeightPanel(FitSet *measureset)
{
	int i;
	int numwidth = 60;
	ParamCon *newcon;

	if(measureset == NULL) measureset = fitset;
	wxFlexGridSizer *fitweightgrid = new wxFlexGridSizer(1, 0, 2);

	activepanel->controlborder = 1;
	for(i=0; i<measureset->measureCount; i++) {
		newcon = paramset.AddNum(measureset->tags[i] + "weight", "", measureset->measures[i].weight, 2, 0, numwidth);
		diagbox->Write(text.Format("Adding fit measure param tag %s\n", measureset->tags[i] + "weight")); 
		fitweightgrid->Add(newcon);
	}
	activepanel->controlborder = 2;

	fitweightgrid->Add(GridLabel(0, ""));
	fitdatapanel->AddDat("weightsum");
	fitweightgrid->Add(fitdatapanel->GetDat("weightsum"));

	return fitweightgrid;
}


wxFlexGridSizer *EvoFitBox::FitConPanel(FitConSet *conset)
{
	int i;
	int labelwidth = 80;
	int numwidth = 50;
	ParamCon *newcon;

	if(conset == NULL) conset = fitconset;
	wxFlexGridSizer *fitcongrid = new wxFlexGridSizer(1, 0, 2);

	activepanel->controlborder = 1;
	for(i=0; i<conset->count; i++) {
		newcon = paramset.AddNum(conset->tags[i], conset->cons[i].label, conset->cons[i].value, 0, labelwidth, numwidth);
		fitcongrid->Add(newcon);
	}
	activepanel->controlborder = 2;

	return fitcongrid;
}


wxFlexGridSizer *EvoFitBox::FitScorePanel(FitPanel *datpanel)
{
	int numwidth = 60;
	int gridwidth;

	wxFlexGridSizer *fitdatagrid = new wxFlexGridSizer(2, 3, 3);
	if(ostype == Mac) gridwidth = 45; else gridwidth = 80;

	fitdatagrid->Add(GridLabel(gridwidth, "Hist FirstN RMS"), 0, wxALIGN_CENTRE);
	datpanel->histfirstrms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->histfirstrms);

	fitdatagrid->Add(GridLabel(gridwidth, "Hist Range RMS"), 0, wxALIGN_CENTRE);
	datpanel->histrangerms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->histrangerms);

	fitdatagrid->Add(GridLabel(gridwidth, "Hazard RMS"), 0, wxALIGN_CENTRE);
	datpanel->hazrms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->hazrms);

	fitdatagrid->Add(GridLabel(gridwidth, "IoD RMS"), 0, wxALIGN_CENTRE);
	datpanel->iodrms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->iodrms);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst FirstN RMS"), 0, wxALIGN_CENTRE);
	datpanel->burstfirstrms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstfirstrms);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Mode"), 0, wxALIGN_CENTRE);
	datpanel->burstmode = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstmode);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Length Mean"), 0, wxALIGN_CENTRE);
	datpanel->burstlengthmean = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstlengthmean);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Length SD"), 0, wxALIGN_CENTRE);
	datpanel->burstlengthsd = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstlengthsd);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Silence Mean"), 0, wxALIGN_CENTRE);
	datpanel->burstsilencemean = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstsilencemean);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Silence SD"), 0, wxALIGN_CENTRE);
	datpanel->burstsilencesd = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstsilencesd);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Intra Freq"), 0, wxALIGN_CENTRE);
	datpanel->burstintrafreq = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstintrafreq);

	fitdatagrid->Add(GridLabel(0, ""));
	fitdatagrid->Add(GridLabel(0, ""));

	fitdatagrid->Add(GridLabel(gridwidth, "Weighted Score"), 0, wxALIGN_CENTRE);
	datpanel->weightscore = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->weightscore);

	return fitdatagrid;
}


void EvoFitBox::CreatePanels()
{
	int datwidth = 60;
	int numwidth = 50;
	int gridwidth = 65;
	ParamCon *newcon;
	wxBoxSizer *fitsetbox;

	if(ostype == Mac) labelwidth = 30; else labelwidth = 50;

	long notestyle = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS;
	wxAuiNotebook *tabpanel = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notestyle);
	wxAuiNotebook *tabpanel2 = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notestyle);

	// Chrome Panel ////////////////////

	chromeindex = 0;

	ToolPanel *chromepanel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
	chromepanel->SetFont(boxfont);
	wxBoxSizer *chromesizer = new wxBoxSizer(wxVERTICAL);
	chromepanel->SetSizer(chromesizer);
	activepanel = chromepanel;

	chromedex = NumPanel(datwidth, wxALIGN_RIGHT, "0");
	chromefit = NumPanel(datwidth, wxALIGN_RIGHT, "0");
	lengthmean = NumPanel(datwidth, wxALIGN_RIGHT, "0");
	silencemean = NumPanel(datwidth, wxALIGN_RIGHT, "0");
	intrafreq = NumPanel(datwidth, wxALIGN_RIGHT, "0");
	spikes = NumPanel(datwidth, wxALIGN_RIGHT, "0");

	wxGridSizer *chromedatagrid = new wxGridSizer(2, 3, 3);
	chromedatagrid->Add(TextLabel("Index"), 0, wxALIGN_CENTRE);
	chromedatagrid->Add(chromedex);
	chromedatagrid->Add(TextLabel("Length Mean"), 0, wxALIGN_CENTRE);
	chromedatagrid->Add(lengthmean);
	chromedatagrid->Add(TextLabel("Silence Mean"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	chromedatagrid->Add(silencemean);
	chromedatagrid->Add(TextLabel("Intra Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	chromedatagrid->Add(intrafreq);
	chromedatagrid->Add(TextLabel("Spikes"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	chromedatagrid->Add(spikes);
	chromedatagrid->Add(TextLabel("Fitness"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	chromedatagrid->Add(chromefit);

	wxStaticBoxSizer *chromestatbox = new wxStaticBoxSizer(wxVERTICAL, chromepanel, "");
	chromestatbox->Add(TextLabel("Chrome"), 0, wxALIGN_CENTRE);
	datchrome = new wxStaticText(chromepanel, -1, "---", wxDefaultPosition, wxSize(80, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE);
	datchrome->SetFont(confont);
	if(ostype == Mac) 
		chromespin = new wxSpinButton(chromepanel, ID_chromespin, wxDefaultPosition, wxSize(40, 25), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	else chromespin = new wxSpinButton(chromepanel, ID_chromespin, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	chromespin->SetRange(-1000000, 1000000);
	wxBoxSizer *chromedatbox = new wxBoxSizer(wxHORIZONTAL);
	chromedatbox->Add(chromespin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	chromedatbox->AddSpacer(5);
	chromestatbox->Add(datchrome, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	chromestatbox->Add(chromedatbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
	chromestatbox->AddSpacer(5);
	chromestatbox->Add(chromedatagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	chromesizer->AddStretchSpacer();
	chromesizer->Add(chromestatbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	chromesizer->AddStretchSpacer();

	chromepanel->Layout();

	/////////////////////////////
	//Data Panel

	ToolPanel *datapanel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
	datapanel->SetFont(boxfont);
	wxBoxSizer *datasizer = new wxBoxSizer(wxVERTICAL);
	datapanel->SetSizer(datasizer);

	activepanel = datapanel;
	//paramset.panel = activepanel;

	wxBoxSizer *datfilebox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *datbuttons = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *datcon = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *datbox = new wxBoxSizer(wxVERTICAL);

	//datfiletag = TextInput(100, 20, "n0");
	datfiletag = TextInputCombo(100, 20, "", "fitload", mod->GetPath());
	datstatus = NumPanel(100, wxALIGN_RIGHT, "");

	if(ostype == Mac) {
		AddButton(ID_datload, "Load", 60, datbuttons, 2);
		AddButton(ID_datoutput, "Output Data", 100, datcon, 2);
	}
	else {
		AddButton(ID_datload, "Load", 40, datbuttons, 2);
		AddButton(ID_datoutput, "Output Data", 80, datcon, 2);
	}

	datcon->AddSpacer(5);
	datcon->Add(datstatus, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	datfilebox->Add(GridLabel(40, "Data file"), 0, wxALIGN_CENTRE|wxALL, 2);
	datfilebox->Add(datfiletag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	datfilebox->Add(datbuttons, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	datbox->Add(datfilebox, 0);
	if(ostype == Mac) datbox->AddStretchSpacer(5); else datbox->AddSpacer(2);
	datbox->Add(datcon, 0);


	// Exp spike data panel              December 2019
	expspikes = NumPanel(numwidth);
	expfreq = NumPanel(numwidth);
	expbursts = NumPanel(numwidth);
	
	if(ostype == Mac) gridwidth = 45; else gridwidth = 30;
	wxFlexGridSizer *spikedatagrid = new wxFlexGridSizer(2, 3, 3);
	spikedatagrid->Add(GridLabel(gridwidth, "Spikes"), 0, wxALIGN_CENTRE);
	spikedatagrid->Add(expspikes);
	spikedatagrid->Add(GridLabel(gridwidth, "Freq"), 0, wxALIGN_CENTRE);
	spikedatagrid->Add(expfreq);
	spikedatagrid->Add(GridLabel(gridwidth, "Bursts"), 0, wxALIGN_CENTRE);
	spikedatagrid->Add(expbursts);
	
	wxStaticBoxSizer *spikedatabox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "Exp Data");
	if(ostype == Windows) spikedatabox->AddSpacer(5);
	spikedatabox->Add(spikedatagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	if(ostype == Windows) datasizer->AddStretchSpacer(5);
	datasizer->Add(datbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	datasizer->AddStretchSpacer(5);
	datasizer->Add(spikedatabox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	datasizer->AddStretchSpacer(5);
	datapanel->Layout();


	////////////////////////////////
	// Fitness Panel

	//wxPanel *measurepanel = new wxPanel(this, wxID_ANY);
	ToolPanel *fitnesspanel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
	fitnesspanel->SetFont(boxfont);
	wxBoxSizer *fitnessSizer = new wxBoxSizer(wxVERTICAL);
	fitnesspanel->SetSizer(fitnessSizer);

	activepanel = fitnesspanel;
	paramset.panel = activepanel;
	labelwidth = 50;
	numwidth = 80;

	fitdatapanel = new DatPanel(this, 20);

	modpanel = new FitPanel();
	//wxFlexGridSizer *fitdatagrid = FitDataPanel(modpanel);
	wxFlexGridSizer *fitdatagrid = FitDataPanel(fitset, fitdatapanel);
	wxStaticBoxSizer *modfitdatabox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "Fitness Measure");
	//wxStaticBoxSizer *chromestatbox = new wxStaticBoxSizer(wxVERTICAL, chromepanel, "");
	if(ostype == Windows) modfitdatabox->AddSpacer(5);
	modfitdatabox->Add(fitdatagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	modfitdatabox->AddSpacer(10);
	wxBoxSizer *fitbuttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_fitscore, "Score", 50, fitbuttonbox);
	AddButton(ID_loadfitscore, "LScore", 50, fitbuttonbox);
	modfitdatabox->Add(fitbuttonbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);

	wxStaticBoxSizer *fitweightbox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "Weights");
	if(ostype == Windows) fitweightbox->AddSpacer(3);
	fitweightbox->Add(FitWeightPanel(), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	wxBoxSizer *fitweightbox2 = new wxBoxSizer(wxVERTICAL);
	fitweightbox2->Add(fitweightbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	fitweightbox2->AddSpacer(5);
	burstToggle = ToggleButton(ID_Burst, "Burst", 40, fitweightbox2);
	//fitweightbox2->Add(burstToggle, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);

	/*
	evopanel = new FitPanel();
	wxFlexGridSizer *evofitgrid = FitDataPanel(evopanel);
	wxStaticBoxSizer *evofitdatabox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "Fitness Measure");
	if(ostype == Windows) evofitdatabox->AddSpacer(5);
	evofitdatabox->Add(evofitgrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	evofitdatabox->AddSpacer(10);
	AddButton(ID_evofitscore, "Score", 50, evofitdatabox);*/

	wxBoxSizer *fitdatabox = new wxBoxSizer(wxHORIZONTAL);
	fitdatabox->Add(modfitdatabox, 0);
	fitdatabox->Add(fitweightbox2, 0);
	//fitdatabox->AddSpacer(10);
	//fitdatabox->Add(evofitdatabox, 1);

	wxStaticBoxSizer *fitconbox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "Fitness Controls");
	if(ostype == Windows) fitconbox->AddSpacer(3);
	fitconbox->Add(FitConPanel(fitconset), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);

	paramindex = paramset.numparams;

	//short measurenum = paramset.numparams;

	fitnessSizer->AddStretchSpacer();
	fitnessSizer->Add(fitdatabox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	fitnessSizer->AddStretchSpacer();
	fitnessSizer->Add(fitconbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	fitnessSizer->AddStretchSpacer();

	fitnesspanel->Layout();


	// GPU Panel ///////////////////////

	ToolPanel* gpupanel = new ToolPanel(this, tabpanel);
	gpupanel->SetFont(boxfont);
	wxBoxSizer* gpusizer = new wxBoxSizer(wxVERTICAL);
	gpupanel->SetSizer(gpusizer);

	activepanel = gpupanel;
	paramset.panel = gpupanel;
	labelwidth = 50;
	numwidth = 45;

	

	paramset.AddNum("gpudevice", "Device", 0, 0, labelwidth, numwidth);
	
	wxStaticBoxSizer* devicebox = new wxStaticBoxSizer(wxVERTICAL, gpupanel, "GPU Device");
	PanelParamLayout(devicebox);

	gpusizer->AddSpacer(10);
	gpusizer->Add(devicebox, 0, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL | wxALL, 0);
	gpupanel->Layout();


	//////////////////////////////
	// Evolve Panel


	ToolPanel *evopanel = new ToolPanel(this, tabpanel);
	evopanel->SetFont(boxfont);
	wxBoxSizer *evosizer = new wxBoxSizer(wxVERTICAL);
	evopanel->SetSizer(evosizer);

	activepanel = evopanel;
	paramset.panel = evopanel;

	labelwidth = 50;
	numwidth = 45;

	paramset.AddNum("evosteps", "Steps", 1000, 0, labelwidth, numwidth); 
	paramset.AddNum("evopop", "Pop", 32, 0, labelwidth, numwidth); 
	paramset.AddNum("parentrange", "Parents", 8, 0, labelwidth, numwidth); 
	paramset.AddNum("evogens", "Gens", 0, 0, labelwidth, numwidth); 
	paramset.AddNum("evomutprob", "Mutation", 0.05, 3, labelwidth, numwidth); 
	paramset.AddNum("blocksize", "Block N", 32, 0, labelwidth, numwidth); 
	paramset.AddNum("numruns", "Num Runs", 1, 0, labelwidth, numwidth); 
	paramset.AddNum("fitmode", "Fit Mode", 0, 0, labelwidth, numwidth); 
	paramset.AddNum("runindex", "Run Index", 1, 0, labelwidth, numwidth); 
	paramset.AddNum("chromediag", "Diag", 32, 0, labelwidth, numwidth); 
	paramset.AddNum("scorethreads", "Score Para", 0, 0, labelwidth, numwidth); 
	//short evonum = paramset.numparams;

	wxStaticBoxSizer *inputrangebox = new wxStaticBoxSizer(wxVERTICAL, evopanel, "Evo Gen");
	PanelParamLayout(inputrangebox);
	//for(i=0; i<evonum; i++) {
	//	inputrangebox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	//artspikebox->AddSpacer(2);
	//}

	wxBoxSizer *evorunbox = new wxBoxSizer(wxHORIZONTAL);
	evorunbox->Add(TextLabel("Run"), 1, wxALIGN_CENTRE);
	runstatus = NumPanel(40, wxALIGN_CENTRE, "---"); 
	evorunbox->AddSpacer(10);
	evorunbox->Add(runstatus, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	inputrangebox->AddSpacer(10);
	inputrangebox->Add(evorunbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	inputrangebox->AddSpacer(10);

	wxBoxSizer *evobuttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Run, "Run", 50, evobuttonbox);
	evobuttonbox->AddSpacer(10);
	AddButton(ID_MultiRun, "RunX", 50, evobuttonbox);

	inputrangebox->Add(evobuttonbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	wxBoxSizer *seedbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddNum("evoseed", "", 0, 0, 0, 80);
	paramset.GetCon("evoseed")->SetMinMax(0, 1000000000000);
	seedbox->Add(paramset.GetCon("evoseed"), 0, wxALIGN_CENTRE_VERTICAL);
	seedcheck = SetModCheck(ID_seedcheck, "seedgen", "Seed", true);
	seedbox->Add(seedcheck, 0, wxALIGN_CENTRE_VERTICAL|wxLEFT, 5);
	paramindex = paramset.numparams;

	// Ramp controls - disabled 19/6/18, can't even remember what this was for, left over from proto box copy?

	/*
	paramset.AddNum("rampbase", "Base", 200, 0, labelwidth, numwidth); 
	paramset.AddNum("rampstart", "Start", 200, 0, labelwidth, numwidth); 
	paramset.AddNum("rampstop", "Stop", 300, 0, labelwidth, numwidth); 
	paramset.AddNum("rampinit", "Initial", 200, 0, labelwidth, numwidth); 
	paramset.AddNum("rampstep", "1s Step", 0.1, 2, labelwidth, numwidth); 
	//short rampnum = paramset.numparams;	

	wxStaticBoxSizer *rampbox = new wxStaticBoxSizer(wxVERTICAL, evopanel, "Input Ramp");
	//for(i=evonum; i<rampnum; i++) {
	//	rampbox->Add(paramset.con[i], 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	//artspikebox->AddSpacer(2);
	//}
	PanelParamLayout(rampbox);
	rampbox->AddSpacer(10);
	AddButton(ID_EvoRunRamp, "Run", 50, rampbox);*/

	wxBoxSizer *inputparambox = new wxBoxSizer(wxHORIZONTAL);
	inputparambox->Add(inputrangebox, 0, wxALL, 5);
	//inputparambox->AddStretchSpacer();
	//inputparambox->Add(rampbox, 0, wxALL, 5);
	//leftbox->Add(artspikebox, 0);
	//leftbox->AddSpacer(20);
	evosizer->AddSpacer(10);
	//inputsizer->Add(inputparambox, 0);
	evosizer->Add(inputparambox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//inputsizer->AddStretchSpacer(10);
	evosizer->Add(seedbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	evopanel->Layout();


	// Parameter Panel /////////////////

	ToolPanel *parampanel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
	parampanel->SetFont(boxfont);
	wxBoxSizer *paramsizer = new wxBoxSizer(wxVERTICAL);
	parampanel->SetSizer(paramsizer);
	activepanel = parampanel;
	paramset.panel = parampanel;

	labelwidth = 30;
	numwidth = 60;

	wxStaticBoxSizer *fitparambox = new wxStaticBoxSizer(wxVERTICAL, parampanel, "Fit Parameters");

	fitchecks = new wxCheckBox*[numparams];
	wxFlexGridSizer *fitparamgrid = new wxFlexGridSizer(4, 1, 3);

	int gridlabelwidth = 60;

	fitparamgrid->Add(GridLabel(gridlabelwidth, "Tag"), 0, wxALIGN_CENTRE);
	fitparamgrid->Add(GridLabel(30, "Min"), 0, wxALIGN_CENTRE);
	fitparamgrid->Add(GridLabel(30, "Max"), 0, wxALIGN_CENTRE);
	fitparamgrid->Add(GridLabel(30, "Fit"), 0, wxALIGN_CENTRE);

	for(i=0; i<numparams; i++) {
		fitparamgrid->Add(GridLabel(gridlabelwidth, fitchrome->params[i].label), 0, wxALIGN_CENTRE);
		newcon = paramset.AddNum(fitchrome->tags[i] + "min", "", fitchrome->params[i].min, fitchrome->params[i].places, 0, numwidth);
		fitparamgrid->Add(newcon);
		newcon = paramset.AddNum(fitchrome->tags[i] + "max", "", fitchrome->params[i].max, fitchrome->params[i].places, 0, numwidth);
		fitparamgrid->Add(newcon);
		fitchecks[i] = SetModCheck(1000 + i, fitchrome->tags[i], "", fitchrome->params[i].adapt);
		fitparamgrid->Add(fitchecks[i], 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	}

	fitparambox->Add(fitparamgrid, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);

	paramindex = paramset.numparams;
	//PanelParamLayout(fitparambox);

	paramsizer->AddStretchSpacer();
	paramsizer->Add(fitparambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	paramsizer->AddStretchSpacer();
	parampanel->Layout();


	// Fit Weights Panel /////////////////

	/*ToolPanel *weightpanel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
	weightpanel->SetFont(boxfont);
	wxBoxSizer *weightsizer = new wxBoxSizer(wxVERTICAL);
	weightpanel->SetSizer(weightsizer);
	activepanel = weightpanel;*/

	//////////////////////////////////////////////////

	tabpanel->Freeze();
	tabpanel->AddPage(fitnesspanel, "Fit Measure" , true);
	tabpanel->AddPage(datapanel, "Data" , false);
	//tabpanel->AddPage(chromepanel, "Vids" , false);
	tabpanel->AddPage(evopanel, "Evolve" , true);
	tabpanel->AddPage(parampanel, "Parameters" , false);
	tabpanel->AddPage(gpupanel, "GPU" , false);
	tabpanel->Thaw();

	tabpanel2->Freeze();
	tabpanel2->AddPage(chromepanel, "Chromes" , false);
	tabpanel2->Thaw();

	winman->AddPane(tabpanel, wxAuiPaneInfo().Name("tabpane").CentrePane().BestSize(-1, 400).PaneBorder(false));
	winman->AddPane(tabpanel2, wxAuiPaneInfo().Name("tabpane2").CentrePane().BestSize(-1, 200).PaneBorder(false));
	//winman->AddPane(storepanel, wxAuiPaneInfo().Name("storepane").Bottom().CaptionVisible(false).BestSize(-1, 60).PaneBorder(false));
	//winman->AddPane(storepanel, wxAuiPaneInfo().Name("storepane").CaptionVisible(false));

	winman->GetPane("tabpane").dock_proportion = 70;
	winman->GetPane("tabpane2").dock_proportion = 30;

	winman->Update();

	Connect(ID_fitscore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EvoFitBox::OnFitScore));
	Connect(ID_loadfitscore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EvoFitBox::OnFitScore));
	Connect(ID_Run, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EvoFitBox::OnEvoRun));
	Connect(ID_MultiRun, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EvoFitBox::OnEvoRun));
	Connect(ID_datload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EvoFitBox::OnDataLoad));
	//Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(VasoNewSecBox::OnRun));
	//Connect(wxEVT_SCROLL_THUMBTRACK, wxSpinEventHandler(VasoNewSecBox::OnSpin));
	Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(EvoFitBox::OnSpinNext));
	Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(EvoFitBox::OnSpinPrev));
	Connect(ID_paramstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EvoFitBox::OnParamStore));
	Connect(ID_paramload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EvoFitBox::OnParamLoad));
}


void EvoFitBox::OnToggle(wxCommandEvent& event)
{
	if(event.GetId() == ID_Burst) {
		burstfitmode = 1 - burstfitmode;
		diagbox->Write("burst fit toggle\n");
	}
}


void EvoFitBox::FitChromeUpdate()
{
	wxString tag;
	//ParamStore *evoparams = GetParams();
	ParamStore *modparams = fitchrome->parambox->GetParams();

	for(i=0; i<numparams; i++) {
		tag = fitchrome->tags[i];
		//fitchrome->params[i].value = (*modparams)[tag];
		fitchrome->params[i].value = fitchrome->cons[i]->GetValue();
		fitchrome->params[i].min = paramset.GetValue(tag + "min"); 
		fitchrome->params[i].max = paramset.GetValue(tag + "max"); 
		//fitchrome->params[i].adapt = (*modflags)[i + 1000];
		fitchrome->params[i].adapt = fitchecks[i]->GetValue();
	}
}


void EvoFitBox::FitControlUpdate()
{
	wxString tag;

	for(i=0; i<fitconset->count; i++) {
		tag = fitconset->tags[i];	
		fitconset->cons[i].value = paramset.GetValue(tag); 
		mod->diagbox->Write(text.Format("Get fit control param tag %s value %.0f\n", tag, fitconset->cons[i].value)); 
	}
}


void EvoFitBox::FitWeightUpdate()
{
	wxString tag;

	for(i=0; i<fitset->measureCount; i++) {
		tag = fitset->tags[i];	
		fitset->measures[i].weight = paramset.GetValue(tag + "weight"); 
		//mod->diagbox->Write(text.Format("Get fit measure param tag %s\n", tag + "weight")); 
	}
}


void EvoFitBox::ExpDataDisp()
{
	if(expdata == NULL) return;

	snum.Printf("%d", expdata->spikecount);
	expspikes->SetLabel(snum);
	snum.Printf("%.2f", expdata->freq);
	expfreq->SetLabel(snum);
	snum.Printf("%d", expdata->burstdata->numbursts);
	expbursts->SetLabel(snum);
}


void EvoFitBox::ModFitScore()
{
	moddata->FitScore(expdata, fitdata, fitset, fitconset, burstbox->GetParams());
	FitDispNew();
}
	

void EvoFitBox::OnFitScore(wxCommandEvent& event)
{
	int id;
	int highstart, highstop;

	FitWeightUpdate();
	FitControlUpdate();

	// Display hist head parameter positions on quad hist plot
	ParamStore *fitparams = GetParams();
	highstart = fitconset->GetCon("RMSHeadStart").value;   // (*fitparams)["RMSHeadRangeStart"];
	highstop = fitconset->GetCon("RMSHeadStop").value;    // (*fitparams)["RMSHeadRangeStop"];
	mod->graphbase->GetGraph("exphistquadsmooth")->highstart = highstart;
	mod->graphbase->GetGraph("exphistquadsmooth")->highstop = highstop;
	mod->mainwin->GraphUpdate();

	if(event.GetId() == ID_fitscore) {
		moddata->FitScore(expdata, fitdata, fitset, fitconset, burstbox->GetParams());
		//FitDisp();
		FitDispNew();
	}

	if(event.GetId() == ID_evofitscore) {
		evodata->FitScore(expdata, evofitdata, fitset, fitconset);
		FitDisp(evofitdata, evopanel);
	}

	if(event.GetId() == ID_loadfitscore) {
		loaddata->FitScore(expdata, loadfitdata, fitset, fitconset);
		FitDispNew(loadfitdata);
	}
}


void EvoFitBox::FitDispNew(FitDat *data, DatPanel *datapanel)
{
	int i;
	int places = 4;
	wxString tag;
	
	if(!data) data = fitdata;
	if(!datapanel) datapanel = fitdatapanel;

	//diagbox->Write(text.Format("\nFitDisp %d measures\n", fitset->measureCount));

	for(i=0; i<fitset->measureCount; i++) {
		tag = fitset->tags[i];
		datapanel->datset[i]->SetLabel(numtext(data->scores[tag], places));
		//diagbox->Write(text.Format("Measure %s  Score %.4f\n", tag, data->scores[tag]));
	}

	datapanel->GetDat("weightscore")->SetLabel(numtext(data->score, places));
	datapanel->GetDat("weightsum")->SetLabel(numtext(data->weightsum, places));
}


void EvoFitBox::FitDisp(FitDat *data, FitPanel *datpanel)       // out of use since move to ParamStore based FitDat
{
	int places = 4;

	if(!data) data = fitdata;
	if(!datpanel) datpanel = modpanel;

	//hazchisquare->SetLabel(numtext(data->haz5chisq, 8));
	datpanel->histfirstrms->SetLabel(numtext(data->RMSFirstNBins, places));
	datpanel->histrangerms->SetLabel(numtext(data->RMSBinRange, places));
	datpanel->hazrms->SetLabel(numtext(data->RMSHaz, places));
	datpanel->iodrms->SetLabel(numtext(data->RMSIoD, places));
	//datpanel->iodrms->SetLabel(numtext(100, places));

	datpanel->burstfirstrms->SetLabel(numtext(data->RMSBurstHead, places));
	datpanel->burstmode->SetLabel(numtext(data->burstmode, places));
	datpanel->burstlengthmean->SetLabel(numtext(data->burstlengthmean, places));
	datpanel->burstlengthsd->SetLabel(numtext(data->burstlengthsd, places));
	datpanel->burstsilencemean->SetLabel(numtext(data->burstsilencemean, places));
	datpanel->burstsilencesd->SetLabel(numtext(data->burstsilencesd, places));
	datpanel->burstintrafreq->SetLabel(numtext(data->burstintrafreq, places));

	datpanel->weightscore->SetLabel(numtext(data->score, places));
}


void EvoFitBox::OnEvoRun(wxCommandEvent &event) 
{
	wxString tag;
	int i, j, size;
	int highstop;

	FitChromeUpdate();
	FitWeightUpdate();
	FitControlUpdate();

	//highstop = fitconset->GetCon("RMSFirstNBins").value;
	//mod->graphbase->GetGraph("exphistquadsmooth")->highstart = 0;
	//mod->graphbase->GetGraph("exphistquadsmooth")->highstop = highstop;
	//mod->graphbase->GetGraph("evohistquadsmooth")->highstart = 0;
	//mod->graphbase->GetGraph("evohistquadsmooth")->highstop = highstop;
	//mod->mainwin->GraphUpdate();

	(*modflags)["multirun"] = false;
	(*modflags)["genlysis"] = false;

	// Allocate and initialise chrome pop

	ParamStore *params = GetParams();
	popsize = (int)(*params)["evopop"];
	parentrange = (int)(*params)["parentrange"];
	rescount = (int)(*params)["numruns"];
	size = popsize + parentrange;
	if(chromepopinit) chromepop.clear();
	chromepopinit = true;


	/*
	// Population chrome vector
	for(i=0; i<size; i++) {
		chromepop.push_back(VasoChrome(numparams));
		for(j=0; j<numparams; j++) {
			tag = fitchrome->tags[j];
			chromepop[i].diagbox = mod->diagbox;
			chromepop[i].AddParam(tag, fitchrome->GetParam(tag)); 
		}
	}

	if(event.GetId() == ID_MultiRun) {
		(*modflags)["multirun"] = true;
		chromeresult.clear();
		// Multirun result chrome vector
		for(i=0; i<rescount; i++) {
			chromeresult.push_back(VasoChrome(numparams));
			for(j=0; j<numparams; j++) {
				tag = fitchrome->tags[j];
				chromeresult[i].diagbox = mod->diagbox;
				chromeresult[i].AddParam(tag, fitchrome->GetParam(tag)); 
			}
		}
	}
	*/

	if(event.GetId() == ID_MultiRun) (*modflags)["multirun"] = true;

	//vasomod->fitthread = new VasoEvoFit(vasomod, this);
	//vasomod->fitthread->Create();
	//vasomod->fitthread->Run();
	//evothread = new EvoFitVMN(mod, this);
	//evothread->Create();
	//evothread->Run();
	mod->EvoRun();
}


void EvoFitBox::OnSpinPrev(wxSpinEvent& event)
{
	int id = event.GetId();

	if(id == ID_chromespin) OnChromePrev();
}


void EvoFitBox::OnSpinNext(wxSpinEvent& event)
{
	int id = event.GetId();

	if(id == ID_chromespin) OnChromeNext();
}


void EvoFitBox::OnChromePrev()
{
	if(!popsize) return; 
	if(chromeindex > 0) chromeindex--;
	else chromeindex = popsize - 1;
	ChromeData();
}


void EvoFitBox::OnChromeNext()
{
	if(!popsize) return; 
	if(chromeindex < popsize - 1) chromeindex++;
	else chromeindex = 0;
	ChromeData();
}


void EvoFitBox::ChromeParams(EvoChrome *chrome)
{
	int i, conref;
	//wxString tag;
	double pval;
	ParamCon *con;

	for(i=0; i<chrome->numparams; i++) {
		//tag = chrome->tags[i];
		pval = chrome->params[i].value;
		chrome->cons[i]->SetValue(pval);
		//conref = fitchrome->parambox->paramset.ref[tag];
		//fitchrome->parambox->paramset.con[conref]->SetValue(pval);
	}
}


void EvoFitBox::ChromeData()
{
	ChromeParams(&chromepop[chromeindex]);

	snum = "chrome " + numstring(chromeindex, 0);
	datchrome->SetLabel(snum);
	snum.Printf("%d", chromepop[chromeindex].index);
	chromedex->SetLabel(snum);
	snum.Printf("%.4f", chromepop[chromeindex].fitness);
	chromefit->SetLabel(snum);
	//FitDisp(vasomod->chromepop[chromeindex].fitdata);
	FitDispNew(&chromepop[chromeindex].fitdata);

	//lengthmean->SetLabel(numtext(vasomod->fitdata->BurstMean[chromeindex], 4));
	//silencemean->SetLabel(numtext(vasomod->fitdata->SilenceMean[chromeindex], 4));
	//intrafreq->SetLabel(numtext(vasomod->fitdata->IntraFreq[chromeindex], 4));

	if(spikefitdata->ExtraFreq) evodata->spikecount = spikefitdata->ExtraFreq[chromepop[chromeindex].dataindex];
	else evodata->spikecount = spikefitdata->spikecounts[chromepop[chromeindex].dataindex];
	if(evodata->spikecount > 16384) evodata->spikecount = 16384;
	// d_oInts[i + tid*16384 + bid*num_threads*16384]
	//for(i=0; i<10; i++) vasomod->diagbox->Write(text.Format("Spike %d  Time %.2f\n", i, vasomod->fitdata->Ints[i]));
	//for(i=0; i<10; i++) vasomod->diagbox->Write(text.Format("Spike %d  Time %.2f\n", i, vasomod->fitdata->Ints[i + 32*512*vasomod->chromepop[chromeindex].index])); 
	//for(i=0; i<vasomod->evodata->spikecount; i++) vasomod->evodata->isis[i] = vasomod->fitdata->Ints[i + 32*512*vasomod->chromepop[chromeindex].index];
	for(i=0; i<evodata->spikecount; i++) evodata->times[i] = spikefitdata->Ints[i + 32*512*chromepop[chromeindex].dataindex];

	evodata->neurocalc();

	//lengthmean->SetLabel(numtext(vasomod->fitdata->BurstMean[chromepop[chromeindex].index], 2));
	//silencemean->SetLabel(numtext(vasomod->fitdata->SilenceMean[chromepop[chromeindex].index], 2));
	//intrafreq->SetLabel(numtext(vasomod->fitdata->IntraFreq[vasomod->chromepop[chromeindex].index], 2));
	intrafreq->SetLabel(numtext(evodata->freq, 2));
	//spikes->SetLabel(numtext(vasomod->fitdata->ExtraFreq[vasomod->chromepop[chromeindex].index], 0));
	spikes->SetLabel(numtext(evodata->spikecount, 0));

	//vasomod->burstbox->Scan(vasomod->evodata);

	if(burstmode) {
		evodata->BurstScan(burstbox);
		if(evodata->burstdata->numbursts > 0) evodata->BurstProfile();
		burstbox->BurstDataDisp(evodata, burstbox->evoburst);
	}

	//spikefitdata->Select(chromeindex);
	mod->mainwin->GraphUpdate();
}


void EvoFitBox::OnDataLoad(wxCommandEvent& event)
{
	int s, count, maxdata, stretchdata;
	double datval, spikeint;
	datdouble rawdata;
	wxString readline, filename;
	wxString datname, filetag, text;
	bool check;
	TextFile datfile;
	int filterthresh = 2;

	int units = 1000;
	
	maxdata = 100000;
	stretchdata = maxdata;
	rawdata.setsize(maxdata);

	// Spike data file
	filetag = datfiletag->GetValue();
	filename = mainwin->datapath + "\\" + filetag + ".dat";
	//datname = filetag;

	if(!datfile.Open(filename)) {
		filename = mainwin->datapath + "\\" + filetag + ".txt";
		if(!datfile.Open(filename)) {
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

	count = 0;
	loaddata->Clear();

	readline = datfile.ReadLine();
	//datfiletag->SetValue(readline);
	if(readline.GetChar(0) == '\'') {
		readline = readline.AfterFirst('\'');
		datname = readline.BeforeFirst('\'');
	}
	else if(readline.GetChar(0) == '\"') {
		readline = datfile.ReadLine();
		readline = readline.AfterFirst('\"');
		datname = readline.BeforeFirst('\"');
	}
	//datfiletag->SetValue(datname);

	if(readline.ToDouble(&datval)) rawdata[count++] = datval * 1000;
	readline = datfile.ReadLine();

	while(readline.IsEmpty() || readline.GetChar(0) == '\"' || readline.GetChar(0) == ':') 
		readline = datfile.ReadLine();

	while(!readline.IsEmpty() && readline.GetChar(0) != '\"' && readline.GetChar(0) != ':') {
		readline.Trim();
		readline.ToDouble(&datval);
		rawdata[count++] = datval * units;
		readline = datfile.ReadLine();
		if(count == stretchdata) {
			stretchdata = stretchdata + 1000;
			rawdata.setsize(stretchdata);
			loaddata->ReSize(stretchdata);
		}
	}

	/*loaddata->isis[0] = 0;
	s = 1;
	for(i=0; i<count-1; i++) {
		spikeint = rawdata[i+1] - rawdata[i];
		if(spikeint > filterthresh) loaddata->isis[s++] = spikeint;
	}*/

	for(i=0; i<count; i++) loaddata->times[i] = rawdata[i];                // February 2014  interval recode, need to fix filtering
	loaddata->spikecount = count;
	loaddata->start = rawdata[0];
	loaddata->neurocalc();

	mod->diagbox->Write(text.Format("Data file OK, %d spikes loaded\n", count));

	datfile.Close();
}

/*
wxFlexGridSizer *VasoFitBox::FitDataPanel(FitPanel *datpanel)
{
	int numwidth = 60;
	int gridwidth;

	wxFlexGridSizer *fitdatagrid = new wxFlexGridSizer(2, 3, 3);
	if(ostype == Mac) gridwidth = 45; else gridwidth = 80;

	fitdatagrid->Add(GridLabel(gridwidth, "Hist FirstN RMS"), 0, wxALIGN_CENTRE);
	datpanel->histfirstrms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->histfirstrms);

	fitdatagrid->Add(GridLabel(gridwidth, "Hist Range RMS"), 0, wxALIGN_CENTRE);
	datpanel->histrangerms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->histrangerms);

	fitdatagrid->Add(GridLabel(gridwidth, "Hazard RMS"), 0, wxALIGN_CENTRE);
	datpanel->hazrms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->hazrms);

	fitdatagrid->Add(GridLabel(gridwidth, "IoD RMS"), 0, wxALIGN_CENTRE);
	datpanel->iodrms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->iodrms);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst FirstN RMS"), 0, wxALIGN_CENTRE);
	datpanel->burstfirstrms = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstfirstrms);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Mode"), 0, wxALIGN_CENTRE);
	datpanel->burstmode = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstmode);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Length Mean"), 0, wxALIGN_CENTRE);
	datpanel->burstlengthmean = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstlengthmean);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Length SD"), 0, wxALIGN_CENTRE);
	datpanel->burstlengthsd = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstlengthsd);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Silence Mean"), 0, wxALIGN_CENTRE);
	datpanel->burstsilencemean = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstsilencemean);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Silence SD"), 0, wxALIGN_CENTRE);
	datpanel->burstsilencesd = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstsilencesd);

	fitdatagrid->Add(GridLabel(gridwidth, "Burst Intra Freq"), 0, wxALIGN_CENTRE);
	datpanel->burstintrafreq = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->burstintrafreq);

	fitdatagrid->Add(GridLabel(0, ""));
	fitdatagrid->Add(GridLabel(0, ""));

	fitdatagrid->Add(GridLabel(gridwidth, "Weighted Score"), 0, wxALIGN_CENTRE);
	datpanel->weightscore = NumPanel(numwidth);
	fitdatagrid->Add(datpanel->weightscore);

	return fitdatagrid;
}*/