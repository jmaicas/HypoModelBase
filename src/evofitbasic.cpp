

/*
*  evofitbasic.cpp
*  
*  Created by Duncan MacGregor and Tom Clayton 
*  University of Edinburgh 2016
*  Released under MIT license, see https://opensource.org/licenses/MIT
*
*/

#include "vasomod.h"
#include "evofitbasic.h"
#include "hypodef.h"


// Quicksort 

int EvoFit::partition(vector<EvoChrome> *a, int low, int high, int pivotindex)
{
	int storeindex, i;
	double pivotvalue;

	pivotvalue = (*a)[pivotindex].fitness;
	std::swap((*a)[pivotindex], (*a)[high]);
	storeindex = low;
	for(i=low; i<high; i++) {
		if((*a)[i].fitness < pivotvalue) {
			std::swap((*a)[i], (*a)[storeindex]);
			storeindex++;
		}
	}
	std::swap((*a)[storeindex], (*a)[high]);  
	return storeindex;
}


void EvoFit::quicksort(vector<EvoChrome> *a, int low, int high)
{
	int pivot;
	wxString text;

	if (high > low) {
		pivot = partition(a, low, high, (low+high)/2);
		quicksort(a, low, pivot-1);
		quicksort(a, pivot+1, high);
	}
}


// CUDA GPU function - takes a chrome population and runs the model with each chrome generating spike times and a quad binned ISI histogram

#ifdef NOCUDA

void EvoFitGPU(float *chromepop, int gpuparams, int popsize, int blocksize, float runtime, float *Ints, float *ISIs, float *IntraFreq, float *ExtraFreq, int *ThreadData)
{
    int i;
}

#endif


#ifndef NOCUDA

extern void EvoFitGPU(float *chromepop, int gpuparams, int popsize, int blocksize, float runtime, float *Ints, float *ISIs, float *IntraFreq, float *ExtraFreq, int *ThreadData);

#endif



EvoFit::EvoFit(Model *model, EvoFitBox *fbox)
	: wxThread(wxTHREAD_JOINABLE)
{
	mod = model;
	fitbox = fbox;
	diagbox = mod->mainwin->diagbox;
}


void EvoFit::AddChromeParam(int position, double paramval)
{
	chromearray[gpuparams * position] = paramval;
	gpuparams++;
}


void EvoFit::Evaluate(int start, int popcount, double dualfit)
{
	int i, j;
	wxString text;

	ParamStore *vasoparams = fitbox->fitchrome->parambox->GetParams();

	diagbox->Write("Oxy Evaluate, generate chrome array...\n");

	for(j=0; j<popcount; j++) {
		chrome = &(*chromepop)[j + start];
		chromearray[0 * popsize + j] = chrome->Val("ire");  
		chromearray[1 * popsize + j] = chrome->Val("ire") * chrome->Val("iratio");
		chromearray[2 * popsize + j] = chrome->Val("pspmag");
		chromearray[3 * popsize + j] = log((double)2) / chrome->Val("halflifeSyn");
		chromearray[4 * popsize + j] = chrome->Val("Vthresh") - (*vasoparams)["Vrest"];
		chromearray[5 * popsize + j] = chrome->Val("kHAP");
		chromearray[6 * popsize + j] = log((double)2) / chrome->Val("halflifeHAP");
		chromearray[7 * popsize + j] = chrome->Val("kDAP");
		chromearray[8 * popsize + j] = log((double)2) / chrome->Val("halflifeDAP");
		chromearray[9 * popsize + j] = chrome->Val("kAHP");
		chromearray[10 * popsize + j] = log((double)2) / chrome->Val("halflifeAHP");
	}

	diagbox->Write("Evaluate, Chrome Array OK\n");
	diagbox->Write("Running GPU Fit " + text.Format("%.0f steps\n", runtime));

	EvoFitGPU(chromearray, gpuparams, popcount, blocksize, runtime*1000, fitbox->spikefitdata->Ints.data(), fitbox->spikefitdata->ISIs.data(), 
		fitbox->spikefitdata->IntraFreq.data(), fitbox->spikefitdata->ExtraFreq.data(), threaddata);

	diagbox->Write(text.Format("GPU Fit OK, POP %d to %d\n", start, popcount + start));

	for(i=0; i<popcount; i++) {
		for(j=0; j<512; j++) {
			fitbox->evodata->histquadsm[j] = fitbox->spikefitdata->ISIs[j + i * 512];
		}

		// IoD non-parallel code
		fitbox->evodata->spikecount = fitbox->spikefitdata->ExtraFreq[i];		// currently used to store spike count
		if(fitbox->evodata->spikecount > 16384) fitbox->evodata->spikecount = 16384;
		for(j=0; j<fitbox->evodata->spikecount; j++) fitbox->evodata->times[j] = fitbox->spikefitdata->Ints[j+32*512*i];

		fitbox->evodata->FitScoreOxy(fitbox->expdata, &(*chromepop)[i + start].fitdata, fitbox->fitset, fitbox->fitconset);
		(*chromepop)[i + start].fitness = (*chromepop)[i + start].fitdata.score;
		(*chromepop)[i + start].fithead = (*chromepop)[i + start].fitdata.RMSFirstNBins;
		(*chromepop)[i + start].fittail = (*chromepop)[i + start].fitdata.RMSBinRange;
		(*chromepop)[i + start].fithaz = (*chromepop)[i + start].fitdata.RMSHaz;
		(*chromepop)[i + start].fitIoD = (*chromepop)[i + start].fitdata.RMSIoD;
		(*chromepop)[i + start].index = i;

		if(i%32 == 0) diagbox->Write(text.Format("Chrome %d  Spikes %.0f  Score %.2f\n", i, fitbox->spikefitdata->ExtraFreq[i], (*chromepop)[i + start].fitness));
	}

	diagbox->Write(text.Format("Evaluate OK\n"));
}


void EvoFit::InitPop()
{
	int i, j;
	int size;
	wxString text, tag;

	/*
	// Initialise fit chrome template 'fitchrome'
	fitchrome = new EvoChrome();
	fitchrome->AddParam("ire0", 600, 50, 5000, true, "Syn Rate");
	fitchrome->AddParam("iratio0", 0.5, 0, 1, false, "Iratio");
	fitchrome->AddParam("pspmag", 3, 0.1, 10, false, "PSP mag");
	fitchrome->AddParam("halflifeSyn", 8, 2, 20, false, "Syn HL");
	fitchrome->AddParam("Vthresh", -48, -60, -40, false, "V Thresh");
	fitchrome->AddParam("kHAP", 60, 10, 100, true, "HAP k");
	fitchrome->AddParam("halflifeHAP", 7, 2, 50, true, "HAP HL");
	fitchrome->AddParam("kDAP", 0, 0, 5, false, "DAP k");
	fitchrome->AddParam("halflifeDAP", 150, 50, 500, false, "DAP HL");
	fitchrome->AddParam("kAHP", 0.5, 0, 5, true, "AHP k");
	fitchrome->AddParam("halflifeAHP", 500, 50, 1500, true, "AHP HL");*/
	
	fitchrome = fitbox->fitchrome;
	chromeparams = fitchrome->numparams;

	size = popsize + parentrange;
	if(fitbox->chromepopinit) chromepop->clear();
	fitbox->chromepopinit = true;

	// Population chrome vector
	for(i=0; i<size; i++) {
		chromepop->push_back(EvoChrome());
		for(j=0; j<chromeparams; j++) {
			tag = fitchrome->tags[j];
			(*chromepop)[i].diagbox = mod->diagbox;
			//(*chromepop)[i].AddParam(tag, fitchrome->cons[j], &fitchrome->GetParam(tag));
            (*chromepop)[i].AddParam(tag, fitchrome->cons[j], &fitchrome->params[j]);
		}
	}

	// Multirun result chrome vector
	if(multirun) {
		chromeresult->clear();	
		for(i=0; i<numruns; i++) {
			chromeresult->push_back(EvoChrome());
			for(j=0; j<chromeparams; j++) {
				tag = fitchrome->tags[j];
				(*chromeresult)[i].diagbox = mod->diagbox;
				//(*chromeresult)[i].AddParam(tag, fitchrome->cons[j], &fitchrome->GetParam(tag));
                (*chromeresult)[i].AddParam(tag, fitchrome->cons[j], &fitchrome->params[j]);
			}
		}
	}
}


void *EvoFit::Entry()
{
	int i, j;
	wxString text;
	int runindex;

	genmon = false;
	gpuparams = 11;
	runtime = 1000;

	// Read GA parameters

	ParamStore *evoparams = fitbox->GetParams();
	ParamStore *evoflags = fitbox->modflags;

	runtime = (float)(*evoparams)["evosteps"];
	popsize = (int)(*evoparams)["evopop"];
	parentrange = (int)(*evoparams)["parentrange"];
	blocksize = (int)(*evoparams)["blocksize"];
	generations = (int)(*evoparams)["evogens"];
	mutateprob = (*evoparams)["evomutprob"];
	numruns = (int)(*evoparams)["numruns"];

	/*runtime = 1000;
	popsize = 128;
	parentrange = 32;
	blocksize = 32;
	generations = 20;
	mutateprob = 0.05;
	numruns = 1;*/

	multirun = (*fitbox->modflags)["multirun"];
	genlysis = (*fitbox->modflags)["genlysis"];

	evoseed = (*evoparams)["evoseed"];
	seedgen = (*evoflags)["seedgen"];
	if(seedgen) {
		evoseed = (unsigned)(time(NULL));
		fitbox->paramset.GetCon("evoseed")->SetValue(evoseed);
	}
	init_mrand(evoseed);

	chromepop = &(fitbox->chromepop);
	chromeresult = &(fitbox->chromeresult);

	// Allocate or clear population storage
	InitPop();

	// Allocate chrome array for GPU transfer
	chromearray = new float[popsize * gpuparams];

	// Allocate fit data storage arrays for GPU output
	if(fitbox->spikefitdata->chromecount < popsize) {
		//fitbox->spikefitdata->Ints = new float[popsize * 512 * 32];
		fitbox->spikefitdata->Ints.resize(popsize * 512 * 32);
		fitbox->spikefitdata->ISIs.resize(popsize * 512);
		fitbox->spikefitdata->IntraFreq.resize(popsize);
		fitbox->spikefitdata->ExtraFreq.resize(popsize);
		fitbox->spikefitdata->chromecount = popsize;
	}

	fitbox->spikefitdata->extracount = true;

	threaddata = new int[popsize * 4];

	mod->mainwin->SetStatus("Fit Storage OK");

	if(!multirun) Evolve();          
	else {
		for(i=0; i<numruns; i++) {
			fitbox->runstatus->SetLabel(text.Format("%d", i+1));
			Evolve();
			(*chromeresult)[i] = (*chromepop)[0];
			(*chromeresult)[i].index = i;
			diagbox->Write(text.Format("\nRun %d Best chrome fitness %.2f\n\n", i, (*chromeresult)[i].fitness));
		}	
		TextFile resofp;
		wxString filetag = fitbox->fitchrome->parambox->paramstoretag->GetValue();
		resofp.New(text.Format("chrome results %s.txt", filetag));
		quicksort(chromeresult, 0, numruns-1);
		for(i=0; i<numruns; i++) (*chromeresult)[i].Output(&resofp, 1); 
		resofp.Close();
	}

	// Analysis Output
	if(genlysis) {
		TextFile genofp;
		wxString outline;

		genofp.New(text.Format("genlysis results.txt"));
		for(i=1; i<=generations; i++) {
			outline.Printf("Gen\t%d\t", i);
			for(j=0; j<chromeparams; j++) 
				if((*chromepop)[0].params[j].adapt) 	
					outline += text.Format("P%d:\t%.6f\t", j, fitbox->paramsd[i][j] / fitbox->parammean[i][j]);	
			outline += text.Format("Fit\t%.6f\t%.6f\t", fitbox->fitmean[i], fitbox->fitsd[i]);
			genofp.WriteLine(outline);
		}
		genofp.Close();
	}

	diagbox->Write("GA Complete OK\n");

	fitbox->ChromeData();

	return NULL;
}


void EvoFit::Evolve()
{
	wxString text;
	int i, j;

	int gen, pA, pB;
	bool orient;
	EvoChrome temp, parentA, parentB;
	EvoParam newparam;
	short crossA, crossB;
	double muteA, muteB, offset;

	temp = (*chromepop)[0];

	bool diagfile = false;
	bool gpudiag = false;


	// Generate initial population
	for(i=0; i<popsize; i++) {
		(*chromepop)[i].index = i;
		(*chromepop)[i].Initialise();	
	}

	diagbox->Write("Population Generation OK\n");

	Evaluate(0, popsize, dualfit);

	quicksort(chromepop, 0, popsize-1);

	diagbox->Write("Sort OK\n");

	if(gpudiag) {
		FILE *dofp;
		dofp = fopen("gpu-diagnostic.txt", "w");
		for(i=0; i<popsize; i++) {
			fprintf(dofp, "pop %d data index %d tid %d blocksize %d bid %d  syn %.4f\n\n", i, threaddata[0 + i*4], threaddata[1 + i*4], 
				threaddata[2 + i*4], threaddata[3 + i*4], fitbox->spikefitdata->ExtraFreq[i]);
		}
		fclose(dofp);
	}

	mod->mainwin->SetStatus("GA Running");

	// Diagnostic file

	if(diagfile) {
		ofp.New("gatest.txt");
		ofp.WriteLine(text.Format("Running GA, %d parameters", chromeparams));
		ofp.WriteLine("");
		ofp.WriteLine("");
	}

	diagbox->Write(text.Format("Running GA, %d parameters, %d parents, %d gens\n\n\n", chromeparams, parentrange, generations));


	for(gen=1; gen<=generations; gen++) {
		if(diagfile) {
			ofp.WriteLine("");
			ofp.WriteLine(text.Format("generation %d", gen));
		}
		diagbox->Write(text.Format("Generation %d...\n", gen));

		// Generate new generation

		for(i=0; i<popsize; i++) {
			pA = (int)(mrand01() * parentrange); 
			pB = (int)(mrand01() * parentrange); 
			parentA = (*chromepop)[pA]; 
			parentB = (*chromepop)[pB]; 
			crossA = ((int)(mrand01() * (chromeparams - 3)) + 1);
			crossB = ((int)(mrand01() * (chromeparams - 2)) + crossA);
			if(mrand01() > 0.5) orient = false; else orient = true;

			for(j=0; j<chromeparams; j++) {
				newparam = temp.params[j];
				if(temp.params[j].adapt) {
					if(mrand01() < mutateprob) {
						newparam.Generate();            // Mutate
						if(diagfile && genmon) ofp.WriteLine("mutate");
					}
					else {                           // Crossover
						if(diagfile && genmon) ofp.WriteLine("crossover");
						if(j < crossA || j > crossB) {
							if(orient) newparam = parentA.params[j];
							else newparam = parentB.params[j];
						}
						else {
							if(orient) newparam = parentB.params[j];
							else newparam = parentA.params[j];
						}
						muteA = mrand01();
						muteB = mrand01();
						offset = (muteA - muteB) * 0.5;
						offset = offset * (parentA.params[j].value - parentB.params[j].value);
						newparam.value += offset;
						if(newparam.value < newparam.min) newparam.value = newparam.min;
						if(newparam.value > newparam.max) newparam.value = newparam.max;
					}
				}
				(*chromepop)[i + parentrange].params[j] = newparam;
			}
			(*chromepop)[i + parentrange].index = popsize * gen + i;
		}

		diagbox->Write("Generation OK, Evaluate...\n");

		Evaluate(parentrange, popsize, dualfit);

		// Insert new generation
		quicksort(chromepop, 0, popsize + parentrange - 1);
	}

	if(diagfile) ofp.Close();
}



void SpikeFitDat::Select(int index)
{
	int i;

	for(i=0; i<512; i++) {
		//ISIhist[i] = ISIs[i + index * 512];
		//ISIhist[i] = 0.5;
		//burstprof[i] = Bursts[i + index * 512];
	}
}


void SpikeFitDat::DeAllocate()
{
	//if(Ints) delete[] Ints;
	//if(ISIs) delete[] ISIs;
	//if(ExtraFreq) delete[] ExtraFreq;
	//if(IntraFreq) delete[] IntraFreq;
	//if(Bursts) delete[] Bursts;
	//if(BurstMean) delete[] BurstMean;
	//if(BurstSD) delete[] BurstSD;
	//if(SilenceMean) delete[] SilenceMean;
	//if(SilenceSD) delete[] SilenceSD;
	//if(SpikeCounts) delete[] SpikeCounts;
}


SpikeFitDat::~SpikeFitDat()
{
	DeAllocate();
}
