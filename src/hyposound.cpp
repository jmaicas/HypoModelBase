
//#include "hypomodel.h"
#include "hyposound.h"
#include "hypograph.h"

#include "SineWave.h"
#include "Noise.h"
#include "RtWvOut.h"
#include "FileWvOut.h"
#include <cstdlib>

using namespace stk;



#ifdef HYPOSOUND

SoundBox::SoundBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, SpikeDat *sdat)
	: ParamBox(model, title, pos, size)
{
	int i;
	short stype;
	spikedata = sdat;
	soundon = 0;
	soundgen = NULL;
	soundmutex = new wxMutex;
	mod = model;

	paramset.num_numwidth = 50;
	paramset.AddNum("soundfreq", "Sound Freq", 200, 2);
	paramset.AddNum("pulsefreq", "Pulse Freq", 10, 2);
	paramset.AddNum("pulsedur", "Pulse Dur", 5, 2);
	paramset.AddNum("pulseint", "Pulse Int", 500, 0);
	paramset.AddNum("freqscale", "Freq Scale", 20, 0);
	paramset.AddNum("playspikes", "Play Spikes", 0, 0);
	paramset.AddNum("volume", "Volume", 20, 1);
	paramset.AddNum("timerate", "Time Rate", 1, 1);
	paramset.AddNum("tracerate", "Trace Rate", 1000, 0);

	selfstore = true;
	tracemode = 0;

	ParamLayout(1);

	numspikes = NumPanel();

	wxBoxSizer *buttonbox = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Go, "Test", 60, buttonbox);
	buttonbox->AddSpacer(10);
	AddButton(ID_spikes, "Spikes", 60, buttonbox);
	buttonbox->AddSpacer(10);
	AddButton(ID_Wave, "Wave", 60, buttonbox);

	wxBoxSizer *buttonbox2 = new wxBoxSizer(wxHORIZONTAL);
	AddButton(ID_Data, "Data", 60, buttonbox2);
	buttonbox2->AddSpacer(10);
	AddButton(ID_Clear, "Clear", 60, buttonbox2);
	buttonbox2->AddSpacer(10);
	AddButton(ID_Stop, "Stop", 60, buttonbox2);

	SetPanel(ID_Data, mod->gridbox);

	mainbox->AddSpacer(5);
	mainbox->Add(parambox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddSpacer(5);
	mainbox->Add(numspikes, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddSpacer(5);
	mainbox->AddStretchSpacer();
	mainbox->Add(buttonbox, 0,  wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	//mainbox->AddStretchSpacer();
	mainbox->AddSpacer(5);
	mainbox->Add(buttonbox2, 0,  wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
	mainbox->AddSpacer(5);
	panel->Layout();

	stype = mod->SoundLink(&spikedata, &wavedata);
	if(spikedata != NULL) snum.Printf("%d", spikedata->spikecount); else snum.Printf("bad %d", stype);
	numspikes->SetLabel(snum);
	if(selfstore) Load();   // load self-stored tool parameter values

	Connect(ID_Go, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnGo));
	Connect(ID_Stop, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnStop));
	Connect(ID_spikes, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnSpikes));
	Connect(ID_Wave, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnWave));
	Connect(ID_Highlight, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SoundBox::OnHighlight));
	Connect(ID_Trace, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SoundBox::OnTrace));
	Connect(ID_Clear, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnClear));
	//Connect(ID_Data, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SoundBox::OnData));
}


SoundBox::~SoundBox()
{
	delete soundmutex;
}


void SoundBox::OnClear(wxCommandEvent& event)
{
	mod->graphbase->GetGraph("cell0spikes1ms")->drawX = 0;
	(*mod->graphwin)[0].Refresh();
}


void SoundBox::OnHighlight(wxCommandEvent& event)
{
	int bin = event.GetInt();

	if(tracemode == 1) (*mod->graphwin)[0].Highlight(bin);
	if(tracemode == 2) (*mod->graphwin)[0].Highlight(bin/1000, 5);
}


void SoundBox::OnTrace(wxCommandEvent& event)
{
	int timeX = event.GetInt();

	mod->graphbase->GetGraph("cell0spikes1ms")->drawX = timeX;
	(*mod->graphwin)[0].Refresh();
}


void SoundBox::DataLink(SpikeDat *sdata, datdouble *wdata)
{
	if(sdata) spikedata = sdata;
	if(wdata) wavedata = wdata;
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

	//soundgen = NULL;

	(*mod->graphwin)[0].Refresh();
}


void SoundBox::OnWave(wxCommandEvent& event)
{

	//(*mod->graphwin)[0].HighlightTime(0, 500000);

	/*if(soundon) return;
	SoundGen *soundgen = new SoundGen(wavedata, GetParams(), this);
	soundgen->Create();
	soundon = 1;
	soundgen->Run();*/ 
}



void SoundBox::OnSpikes(wxCommandEvent& event)
{
	soundmutex->Lock();
	if(soundon) {
		soundmutex->Unlock();
		return;
	}
	soundmutex->Unlock();
	
	tracemode = 3;

	soundgen = new SoundGen(spikedata, GetParams(), this);
	soundgen->Create();
	soundmutex->Lock();
	soundon = 1;
	soundmutex->Unlock();
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
	double freq = (*modparams)["pulsefreq"];
	int fsamp = 1000/freq;
	int pulsedur = (*modparams)["pulsedur"];
	double volume = (*modparams)["volume"];

	for(s=0; s<freq*2; s++) {
		for(i=0; i<(fsamp-pulsedur)*msamp; i++) dac->tick(0);
		for(i=0; i<pulsedur*msamp; i++) dac->tick(sine.tick() * volume);
	}


cleanup:
	delete dac;
}



#endif



SoundGen::SoundGen(SpikeDat *data, ParamStore *p, SoundBox *box)
: wxThread(wxTHREAD_JOINABLE)
{
	spikemode = 1;
	spikedata = data;
	params = p;
	soundbox = box;
}


SoundGen::SoundGen(datdouble *data, ParamStore *p, SoundBox *box)
: wxThread(wxTHREAD_JOINABLE)
{
	spikemode = 0;
	wavedata = data;
	params = p;
	soundbox = box;
}


void *SoundGen::Entry()
{
	dac = NULL;
	Stk::setSampleRate(96000.0);
	Stk::showWarnings(true);
	dac = new RtWvOut(1);
	
	soundbox->numspikes->SetLabel("playing");
	msamp = 96;
	pulseint = (*params)["pulseint"];
	freqscale = (*params)["freqscale"];
	freq = (*params)["pulsefreq"];
	fsamp = 1000/freq;
	pulsedur = (*params)["pulsedur"];
	playspikes = (*params)["playspikes"];
	timerate = (*params)["timerate"];
	volume = (*params)["volume"];
	tracerate = (*params)["tracerate"];

	if(!playspikes) playspikes = spikedata->spikecount;
	
	if(spikedata->neurodata && spikedata->neurodata->numselects) {
		selectmode = 1;
		playspikes = spikedata->spikecount;
	}
	else selectmode = 0;
	
	//spikemode = 1;
	if(spikemode && soundbox->tracemode == 1) PlaySpikesBinTrace();   // PlaySpikes();
	if(spikemode && soundbox->tracemode == 2) PlaySpikesTrace();   // PlaySpikes();
	if(spikemode && soundbox->tracemode == 3) PlaySpikesTimeTrace();   // PlaySpikes();
	else PlayWave();
	
	//cleanup:
	delete dac;

	soundbox->soundmutex->Lock();
	soundbox->soundon = 0;
	soundbox->soundmutex->Unlock();
	soundbox->numspikes->SetLabel("stopped");
	return NULL;
}


void SoundGen::PlayWave()
{
	int i, s, numdat;
	int datsample, timerate;
	double amp, maxamp;
	wxString text;
	SineWave sine;
	//FileWvOut outfile;
	
	outfile.openFile("vwave.wav", 1, FileWrite::FILE_WAV, Stk::STK_SINT16);
	
	sine.setFrequency((*params)["soundfreq"]);
	numdat = 100000;
	
	maxamp = 500;
	datsample = 100;
	timerate = 10;

	amp = 1;
	//msamp = 96;
	//freqscale = 0.5;
	
	// File output
	
	/*
	for(s=0; s<numdat; s=s+datsample) {
		amp = (wavedata[s] + 100) / 600;
		//amp = 0.5;
		sine.setFrequency(wavedata[s]*freqscale);
		for(i=0; i<msamp; i++) outfile.tick(sine.tick()*amp);
	}*/

	
	//wxString text;                                                                                
	//TextFile textfile;                                                                          
  //textfile.New("wavedata.txt");                                                            
	//for(i=0; i<100; i++) textfile.WriteLine(text.Format("num %.4f\n", (*wavedata)[i]));      
	//textfile.Close(); 


	// Live output
	
	for(s=0; s<numdat; s++) {
		if(s%1 == 0) {
			soundbox->soundmutex->Lock();
			if(!soundbox->soundon) { 
				soundbox->soundmutex->Unlock();
				//delete dac;
				return;
			}
			soundbox->soundmutex->Unlock();
		}	
		//amp = ((*wavedata)[s] + 100) / 600;
		if(s%datsample == 0) {
			text.Printf("%d", s/datsample);
			soundbox->numspikes->SetLabel(text);
		}
		amp = 0.5;
		sine.setFrequency((*wavedata)[s]*freqscale);
		for(i=0; i<msamp*datsample/timerate; i++) dac->tick(sine.tick()*amp);
	}
	
	outfile.closeFile();
}


void SoundGen::PlaySpikes()
{
	int i, s;
	double stime;
	int sbin;
	SineWave sine;
	
	outfile.openFile("vspikes.wav", 1, FileWrite::FILE_WAV, Stk::STK_SINT16);	
	
	sine.setFrequency((*params)["soundfreq"]);
	//for(s=0; s<freq*2; s++) {
	//	for(i=0; i<(fsamp-pulsedur)*msamp; i++) dac->tick(0);
	//	for(i=0; i<pulsedur*msamp; i++) dac->tick(sine.tick());
	//}

	//soundbox->mod->SoundOn();
	//(*soundbox->mod->graphwin)[0].Highlight();

	if(spikemode == 1) {
		for(s=0; s<playspikes; s++) {
			fsamp = spikedata->isis[s];
			//sine.setFrequency(freqscale*1000/fsamp);
			for(i=0; i<(fsamp-pulsedur)*msamp; i++) outfile.tick(0);
			for(i=0; i<pulsedur*msamp; i++) outfile.tick(sine.tick());
		}

		outfile.closeFile();

		// msamp = samples per ms
		// pulsedur = spike sound duration in ms
		// fsamp = sound interval (inter-spike interval)
        
        // Live Output
		for(s=0; s<playspikes; s++) {
			if(s%1 == 0) {
				soundbox->soundmutex->Lock();
				if(!soundbox->soundon) { 
					soundbox->soundmutex->Unlock();
					//delete dac;
					return;
				}
				soundbox->soundmutex->Unlock();
			}
			if(selectmode) {
				if(spikedata->selectdata->spikes[s]) fsamp = spikedata->isis[s];
				//(*soundbox->mod->graphwin)[0].Highlight();
				else continue;
			}
			else fsamp = spikedata->isis[s];
			stime = spikedata->times[s];
			sbin = floor(stime / 1000);
			(*soundbox->mod->graphwin)[0].Highlight(sbin);
			//sine.setFrequency(freqscale*1000/fsamp);
			for(i=0; i<(fsamp - pulsedur) * msamp; i++) dac->tick(0);
			for(i=0; i<pulsedur * msamp; i++) dac->tick(sine.tick());
		}
	}
	
	if(spikemode == 2) {
		for(s=0; s<playspikes; s++) {
			fsamp = spikedata->isis[s];
			sine.setFrequency(freqscale*1000/fsamp);
			for(i=0; i<(pulseint-pulsedur)*msamp; i++) dac->tick(0);
			for(i=0; i<pulsedur*msamp; i++) dac->tick(sine.tick());
		}
	}
	
	outfile.closeFile();
}


void SoundGen::PlaySpikesBinTrace()
{
	int i, s, t=-1;
	double stime;
	int sbin;
	int samprate = msamp * 1000;

	SineWave sine;

	wxCommandEvent highevent(wxEVT_COMMAND_TEXT_UPDATED, ID_Highlight);

	sine.setFrequency((*params)["soundfreq"]);

	// msamp = samples per ms
	// pulsedur = spike sound duration in ms
	// fsamp = sound interval (inter-spike interval)

	// Live Output

	for(s=0; s<playspikes; s++) {

		soundbox->soundmutex->Lock();          // check for Stop flag
		if(!soundbox->soundon) {
			soundbox->soundmutex->Unlock();
			return;
		}
		soundbox->soundmutex->Unlock();
		
		if(selectmode) {
			if(spikedata->selectdata->spikes[s]) fsamp = spikedata->isis[s];
			else continue;
		}
		else fsamp = spikedata->isis[s];
		stime = spikedata->times[s];
		sbin = floor(stime / 1000);
		if(t < 0) {
			t = stime - 1;   // initialise t on first loop
			highevent.SetInt(sbin);
			soundbox->GetEventHandler()->AddPendingEvent(highevent);
		}
		
		//sine.setFrequency(freqscale*1000/fsamp);

		for(i=0; i<pulsedur * msamp; i+=timerate) {      // spike sound
			dac->tick(sine.tick());
			if(i % msamp == 0) {
				t++;
				if(t % 1000 == 0) {
					highevent.SetInt(t/1000);
					soundbox->GetEventHandler()->AddPendingEvent(highevent);
				}
			}
		}

		for(i=0; i<(fsamp - pulsedur) * msamp; i+=timerate) {     // interspike silence
			dac->tick(0);
			if(i % msamp == 0) {
				t++;
				if(t % 1000 == 0) {
					highevent.SetInt(t/1000);
					soundbox->GetEventHandler()->AddPendingEvent(highevent);
				}
			}
		}
		
	}

	(*soundbox->mod->graphwin)[0].Refresh();
}


void SoundGen::PlaySpikesTrace()
{
	int i, s, t=-1;
	double stime;
	int sbin;
	int samprate = msamp * 1000;

	SineWave sine;

	wxCommandEvent highevent(wxEVT_COMMAND_TEXT_UPDATED, ID_Highlight);

	sine.setFrequency((*params)["soundfreq"]);

	// msamp = samples per ms
	// pulsedur = spike sound duration in ms
	// fsamp = sound interval (inter-spike interval)

	// Live Output

	for(s=0; s<playspikes; s++) {

		soundbox->soundmutex->Lock();          // check for Stop flag
		if(!soundbox->soundon) {
			soundbox->soundmutex->Unlock();
			return;
		}
		soundbox->soundmutex->Unlock();

		if(selectmode) {
			if(spikedata->selectdata->spikes[s]) fsamp = spikedata->isis[s];
			else continue;
		}
		else fsamp = spikedata->isis[s];
		stime = spikedata->times[s];
		sbin = floor(stime / 1000);
		
		//sine.setFrequency(freqscale*1000/fsamp);

		highevent.SetInt(stime);
		soundbox->GetEventHandler()->AddPendingEvent(highevent);

		for(i=0; i<pulsedur * msamp; i+=timerate) {      // spike sound
			dac->tick(sine.tick() * volume);
		}

		for(i=0; i<(fsamp - pulsedur) * msamp; i+=timerate) {     // interspike silence
			dac->tick(0);
		}

	}

	if(soundbox->mod->ostype != Mac) (*soundbox->mod->graphwin)[0].Refresh();
}


void SoundGen::PlaySpikesTimeTrace()
{
	int i, s, t=-1;
	double stime;
	int sbin;
	int samprate = msamp * 1000;

	SineWave sine;

	//wxCommandEvent highevent(wxEVT_COMMAND_TEXT_UPDATED, ID_Highlight);
	wxCommandEvent traceevent(wxEVT_COMMAND_TEXT_UPDATED, ID_Trace);

	sine.setFrequency((*params)["soundfreq"]);

	// msamp = samples per ms
	// pulsedur = spike sound duration in ms
	// fsamp = sound interval (inter-spike interval)

	// Live Output

	for(s=0; s<playspikes; s++) {

		soundbox->soundmutex->Lock();          // check for Stop flag
		if(!soundbox->soundon) {
			soundbox->soundmutex->Unlock();
			return;
		}
		soundbox->soundmutex->Unlock();

		if(selectmode) {
			if(spikedata->selectdata->spikes[s]) fsamp = spikedata->isis[s];
			else continue;
		}
		else fsamp = spikedata->isis[s];
		stime = spikedata->times[s];
		sbin = floor(stime / 1000);
		if(t < 0) {
			t = stime - 1;   // initialise t on first loop
			traceevent.SetInt(sbin * 1000);
			soundbox->GetEventHandler()->AddPendingEvent(traceevent);
		}

		//sine.setFrequency(freqscale*1000/fsamp);

		for(i=0; i<pulsedur * msamp; i+=timerate) {      // spike sound
			dac->tick(sine.tick());
			if(i % msamp == 0) {
				t++;
				if(t % tracerate == 0) {
					traceevent.SetInt(t);
					soundbox->GetEventHandler()->AddPendingEvent(traceevent);
				}
			}
		}

		for(i=0; i<(fsamp - pulsedur) * msamp; i+=timerate) {     // interspike silence
			dac->tick(0);
			if(i % msamp == 0) {
				t++;
				if(t % tracerate == 0) {
					traceevent.SetInt(t);
					soundbox->GetEventHandler()->AddPendingEvent(traceevent);
				}
			}
		}
	}


	soundbox->mod->graphbase->GetGraph("cell0spikes1ms")->drawX = 100;
	soundbox->mod->graphbase->GetGraph("cell0spikes1ms")->axistrace = 0;
	(*soundbox->mod->graphwin)[0].Refresh();
}



