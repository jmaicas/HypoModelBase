


#include "hyponeuro.h"
#include "hypodata.h"


NeuroMod::NeuroMod(int type, wxString name, HypoMain *main)
	: Model(type, name, main)
{
	evodata = NULL;
	evodata2 = NULL;
	fitbox = NULL;
}


NeuroMod::~NeuroMod()
{
	if(evodata) delete evodata;
}


void NeuroMod::IoDGraph(datdouble *IoDdata, datdouble *IoDdataX, wxString label, wxString tag, int colour, int barshift, wxString settag)
{
	graphbase->Add(GraphDat(IoDdata, 0, 70, 0, 2, label, 9, 1, colour), tag, settag);
	graphbase->GetGraph(tag)->gdatax = IoDdataX;
	graphbase->GetGraph(tag)->xcount = 7;   // 7  // 5 for Trystan data
	graphbase->GetGraph(tag)->synchx = false;
	graphbase->GetGraph(tag)->barshift  = barshift;
}


void NeuroMod::EvoGraphs()
{
	evodata->PlotSet(graphbase, "Evo ", purple, 1, "evo");

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


void NeuroMod::DataSelect(double from, double to)
{
	neurobox->cellpanel->SetSelectRange(from, to);
}


/*void NeuroMod::ExpDataSwitch(SpikeDat *data)
{
	if(fitbox) fitbox->expdata = data;
}*/


/*void NeuroMod::SpikeBox(int modmode)
{
int boxwidth, boxheight;

//wxString tag;

//SetStatusText("Burst Box");
//burstdata->spikedata = vasodata->spikedat;
if(ostype == Mac) {
boxwidth = 285;
boxheight = 380;
}
else {
boxwidth = 425;
boxheight = 500;
}

diagbox->Write(text.Format("Spike box init type %d\n", modmode));


if(modmode == 2) burstbox = new BurstBox(mod, "Spike Data Load and Analysis", wxPoint(0, 500), wxSize(boxwidth, boxheight), 0, "Selected");

if(modmode == 1) burstbox = new BurstBox(mod, "Spike Data", wxPoint(0, 500), wxSize(boxwidth, boxheight), 0, "Selected", false, 0);
//mainpos = GetPosition();

//burstbox = new BurstBox(this, "Analysis", wxPoint(320, 485), wxSize(330, 430), 0, "Selected");
burstbox->loaddata = expdata;

diagbox->Write(text.Format("SpikeModule modmode %d\n", modmode));

if(!expdata->graphs) {
SpikeModule(mod);
if(!modmode) scalebox->GraphSwitch();
}

diagbox->Write(text.Format("SpikeModule OK\n"));

mod->modtools.AddBox(burstbox, true);

//toolset->AddBox(burstbox);
burstbox->Show(true);
*/