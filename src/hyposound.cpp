
//#include "hypomodel.h"
#include "hyposound.h"

#include "SineWave.h"
#include "Noise.h"
#include "RtWvOut.h"
#include "FileWvOut.h"
#include <cstdlib>

using namespace stk;


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
	//outspikes.openFile( "vspikes.wav", 1, FileWrite::FILE_WAV, Stk::STK_SINT16 );
	
	soundbox->numspikes->SetLabel("playing");
	msamp = 96;
	pulseint = (*params)["pulseint"];
	freqscale = (*params)["freqscale"];
	freq = (*params)["pulsefreq"];
	fsamp = 1000/freq;
	pulsedur = (*params)["pulsedur"];
	playspikes = (*params)["playspikes"];
	timerate = (*params)["timerate"];

	if(spikedata->neurodata->numselects) {
		selectmode = 1;
		playspikes = spikedata->spikecount;
	}
	else selectmode = 0;
	
	//spikemode = 1;
	if(spikemode) PlaySpikes();
	else PlayWave();
	
	//cleanup:
	delete dac;
	//outfile.closeFile();
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
	SineWave sine;
	
	outfile.openFile("vspikes.wav", 1, FileWrite::FILE_WAV, Stk::STK_SINT16);	
	
	sine.setFrequency((*params)["soundfreq"]);
	//for(s=0; s<freq*2; s++) {
	//	for(i=0; i<(fsamp-pulsedur)*msamp; i++) dac->tick(0);
	//	for(i=0; i<pulsedur*msamp; i++) dac->tick(sine.tick());
	//}

	soundbox->mod->SoundOn();

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
				else continue;
			}
			else fsamp = spikedata->isis[s];
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

