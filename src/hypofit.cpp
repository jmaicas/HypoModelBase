

#include "hypodat.h"
#include "hypopanels.h"
#include <cmath>



void SpikeDat::FitScore(SpikeDat *testdata, FitDat *fitdat)
{
	int i;
	int maxcount, histmax = 512;
	wxString text;
	ParamStore *fitweights, *fitcon;
	TextFile ofp;

	ofp.New("fitscore-diag.txt");

	fitweights = new ParamStore();
	(*fitweights)["ISImode"] = 5;
	(*fitweights)["ISImean"] = 5;
	(*fitweights)["RMSFirstNBins"] = 100;
	(*fitweights)["RMSBinRange"] = 100;
	(*fitweights)["RMSHaz"] = 60;

	(*fitweights)["RMSFirstNBinsBurst"] = 30;
	(*fitweights)["burstmode"] = 20;
	(*fitweights)["burstlengthmean"] = 30;
	(*fitweights)["burstlengthsd"] = 5;
	(*fitweights)["burstsilencemean"] = 15;
	(*fitweights)["burstsilencesd"] = 5;
	(*fitweights)["burstintrafreq"] = 20;


	fitcon = new ParamStore();
	(*fitcon)["RMSFirstNBins"] = 30;
	(*fitcon)["RMSBinRangeStart"] = 30;
	(*fitcon)["RMSBinRangeFinish"] = 125;
	(*fitcon)["HazMin"];
	(*fitcon)["RMSFirstNBinsBurst"] = 20;
	
	double chisum = 0;
	int bincount = 300;
	int sumcount = 0;

	/*
	for(i=0; i<bincount; i++) {
		if(haz5[i] > 0) chisum += pow((testdata->haz5[i] - haz5[i]), 2) / haz5[i];
		ofp.WriteLine(text.Format("bin %d  mod %.4f  exp %.4f  chisum %.4f", i, haz5[i], testdata->haz5[i], chisum)); 
	}
	fitdat->haz5chisq = (double)chisum / bincount;
	//return chiscore;
	ofp.WriteLine(text.Format("haz5chi %.6f", fitdat->haz5chisq)); 
	*/


	
	// Spike Data Processing //////////////////////////////////////////////////////////////////  Duplicate of neurocalc processing for GPU generated data

	double initbinwidth = 1;
	double bininc = 0.05;
	double width, histsum;
	double hazremain, hazmin;


	// Mode for fitness
	maxcount = 0;
	for(i=0; i<histmax; i++) 
		if(histquadsm[i] > maxcount) {
			maxcount = histquadsm[i];
			histquadmode = i;
		}

	// quad bin x
	for (i=0; i<histmax; i++) histquadx[i] = i * initbinwidth + (i * (i - 1) / 2) * bininc;

	
	// linearise
	histsum = 0;
	for(i=0; i<histmax-1; i++) {
		width = (histquadx[i+1] - histquadx[i]);          //     / 0.001;
		histquadlin[i] = histquadsm[i] / width;
		histsum += histquadlin[i];
	}
	for(i=0; i<histmax-1; i++) histquadlin[i] = histquadlin[i] / histsum;


	// Quad Hazard

	hazmin = 0.01;
	hazremain = 1;
	for(i=0; i<histmax-1; i++) {
		hazquad[i] = histquadlin[i] / hazremain;
		if(hazremain < hazmin) {
			hazquad[i] = 0;
			hazquadbins = i;
			break;
		}
		hazremain -= histquadlin[i];
	}


	// Burst Mode
	int pmode = 0;
	burstdata->pnzcount = 0;
	for(i=0; i<histmax; i++) {
		if(burstdata->profilesm[i] > burstdata->profilesm[pmode]) pmode = i;
		if(burstdata->profilesm[i] > 0) burstdata->pnzcount = i;
	}
	burstdata->pmodetime = pmode;
	burstdata->pmoderate = burstdata->profilesm[pmode];

	
	////////////////////////////////////////////////////////////////////////////////////


	// Tom Fit Re-engineering

	fitdat->score = 0;
	fitdat->weightsum = 0;

	//fitdat->ISImode = (abs(histquadmode - testdata->histquadmode) / testdata->histquadmode) * 100.0;
	//fitdat->score += fitdat->ISImode * (*fitweights)["ISImode"];                                        // not used in current Tom config

	//fitdat->ISImean = (abs(meanisi - testdata->meanisi) / testdata->meanisi) * 100.0;
	//fitdat->score += fitdat->ISImean * (*fitweights)["ISImean"];                                         // not used in current Tom config


	// ISI Histogram First N Bins RMS

	double RMSError;
  double Error;
	double CutOff;
	double Big, Small;

	if(histquadsm[histquadmode] > testdata->histquadsm[testdata->histquadmode])
		CutOff = histquadsm[histquadmode] * 0.1;
	else CutOff = testdata->histquadsm[testdata->histquadmode] * 0.1;


	RMSError = 0;
	for(i=0; i<(*fitcon)["RMSFirstNBins"]; i++) {
		//The RMS error must be flexible to take account of bins with very small values
		//If a bin has a value of below 0.01 (1% of all events) then it is not normalised                   // Tom Code, probably out of date description
		if(testdata->histquadsm[i] > histquadsm[i]) {
			Big = testdata->histquadsm[i];
			Small = histquadsm[i];
		}
		else {
			Big = histquadsm[i];
			Small = testdata->histquadsm[i];
		}

		//If both are less than the cutoff then use alternate rules
		if (Big < CutOff) Error = Big - Small * 100.0;
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	RMSError /= (*fitcon)["RMSFirstNBins"];
	RMSError = sqrt(RMSError);

	fitdat->RMSFirstNBins = RMSError;
	fitdat->score += fitdat->RMSFirstNBins * (*fitweights)["RMSFirstNBins"];


	// ISI Histogram Bin Range RMS

	RMSError = 0;
	for(i=(*fitcon)["RMSBinRangeStart"]; i<(*fitcon)["RMSBinRangeFinish"]; i++) {
		if(testdata->histquadsm[i] > histquadsm[i]) {
			Big = testdata->histquadsm[i];
			Small = histquadsm[i];
		}
		else {
			Big = histquadsm[i];
			Small = testdata->histquadsm[i];
		}

		//If both are less than the cutoff then use alternate rules
		if (Big < CutOff) Error = Big - Small * 100.0;
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	RMSError /= ((*fitcon)["RMSBinRangeFinish"] - (*fitcon)["RMSBinRangeStart"]);
	RMSError = sqrt(RMSError);

	fitdat->RMSBinRange = RMSError;
	fitdat->score += fitdat->RMSBinRange * (*fitweights)["RMSBinRange"];


	// Hazard RMS

	RMSError = 0;
	int MinHazSize, MaxHazSize;
 
  if(hazquadbins > testdata->hazquadbins) {
		MinHazSize = testdata->hazquadbins;
    MaxHazSize = hazquadbins;
  }
  else {
		MinHazSize = hazquadbins;
		MaxHazSize = testdata->hazquadbins;
  }

	CutOff = 0.01;

	for(i=0; i<MinHazSize; i++) {
		if(testdata->hazquad[i] > hazquad[i]) {
			Big = testdata->hazquad[i];
			Small = hazquad[i];
		}
		else {
			Big = hazquad[i];
			Small = testdata->hazquad[i];
		}

		//If both are less than the cutoff then use alternate rules
		if (Big < CutOff) Error = Big - Small * 100.0;
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	RMSError /= MinHazSize;
	RMSError = sqrt(RMSError);

	 //Extra error is added for % difference between bin counts
  RMSError += ((double)(MaxHazSize - MinHazSize)) / ((double)MinHazSize)*100;

	fitdat->RMSHaz = RMSError;
	fitdat->score += fitdat->RMSHaz * (*fitweights)["RMSHaz"];


	//// Burst Profile Analysis

	// First N Bins RMS

	RMSError = 0;

  if(burstdata->pnzcount < (*fitcon)["RMSFirstNBinsBurst"]) burstdata->pnzcount = (*fitcon)["RMSFirstNBinsBurst"] + 10;

	double AvSum = 0;
  for (i = (*fitcon)["RMSFirstNBinsBurst"]; i < testdata->burstdata->pnzcount; i++) AvSum += testdata->burstdata->profilesm[i];
	AvSum /= testdata->burstdata->pnzcount - (*fitcon)["RMSFirstNBinsBurst"];

	CutOff = AvSum / 5.0;

	double testrate, datarate;

	for(i=0; i<(*fitcon)["RMSFirstNBinsBurst"]; i++)
	{
		//The RMS error must be flexible to take account of bins with very small values
		//If a bin has a value of below 0.01 (1% of all events) then it is not normalised
		
		testrate = testdata->burstdata->profilesm[i] - AvSum;
		datarate = burstdata->profilesm[i] - AvSum;

		if (testrate > datarate) { Big = testrate; Small = datarate; }
		else { Big = datarate; Small = testrate; }

		//If both are less than the cutoff then use alternate rules
		if ((Big < CutOff) && (Big > -CutOff)) Error = (Big - Small) * 20.0;
	  else Error = abs((Big - Small) / Big) * 100.0;
	
		RMSError += Error * Error;
	}

	RMSError /= (*fitcon)["RMSFirstNBinsBurst"];
	RMSError = sqrt(RMSError);

	fitdat->RMSFirstNBinsBurst = RMSError;
	fitdat->score += fitdat->RMSFirstNBinsBurst * (*fitweights)["RMSFirstNBinsBurst"];


	// Burst Mode

	fitdat->burstmode = (abs(testdata->burstdata->pmoderate - burstdata->pmoderate) / testdata->burstdata->pmoderate) * 50.0;
  fitdat->burstmode += (abs(testdata->burstdata->pmodetime - burstdata->pmodetime) / testdata->burstdata->pmodetime) * 50.0;
	fitdat->score += fitdat->burstmode * (*fitweights)["burstmode"];

	// Burst Stats

	fitdat->burstlengthmean = (abs(testdata->burstdata->meanlength - burstdata->meanlength) / testdata->burstdata->meanlength) * 100.0;
	fitdat->score += fitdat->burstlengthmean * (*fitweights)["burstlengthmean"];

	ofp.WriteLine(text.Format("burst mod %.6f  exp %.6f  score %.6f", burstdata->meanlength, testdata->burstdata->meanlength, fitdat->burstlengthmean)); 
	
	fitdat->burstlengthsd = (abs(testdata->burstdata->sdlength - burstdata->sdlength) / testdata->burstdata->sdlength) * 100.0;
	fitdat->score += fitdat->burstlengthsd * (*fitweights)["burstlengthsd"];

	ofp.WriteLine(text.Format("burstsd mod %.6f  exp %.6f  score %.6f", burstdata->sdlength, testdata->burstdata->sdlength, fitdat->burstlengthsd)); 

	fitdat->burstsilencemean = (abs(testdata->burstdata->meansilence - burstdata->meansilence) / testdata->burstdata->meansilence) * 100.0;
	fitdat->score += fitdat->burstsilencemean * (*fitweights)["burstsilencemean"];

	ofp.WriteLine(text.Format("silence mod %.6f  exp %.6f  score %.6f", burstdata->meansilence, testdata->burstdata->meansilence, fitdat->burstsilencemean)); 

	fitdat->burstsilencesd = (abs(testdata->burstdata->sdsilence - burstdata->sdsilence) / testdata->burstdata->sdsilence) * 100.0;
	fitdat->score += fitdat->burstsilencesd * (*fitweights)["burstsilencesd"];

	fitdat->burstintrafreq = (abs(testdata->burstdata->freq - burstdata->freq) / testdata->burstdata->freq) * 100.0;
	fitdat->score += fitdat->burstintrafreq * (*fitweights)["burstintrafreq"];

	ofp.WriteLine(text.Format("intra %.6f  exp %.6f  score %.6f", burstdata->meansilence, testdata->burstdata->meansilence, fitdat->burstsilencemean)); 

	//MoF.Fitness = (Math.Abs(ExpBurst.MeanExtraBurstNoise - TgtBurst.MeanExtraBurstNoise) / TgtBurst.MeanExtraBurstNoise) * 100.0;


	// Burst Hazard   -- no known function


	// Sum weights

	fitdat->weightsum += (*fitweights)["RMSFirstNBins"];
	fitdat->weightsum += (*fitweights)["RMSBinsRange"];
	fitdat->weightsum += (*fitweights)["RMSHaz"];

	fitdat->weightsum += (*fitweights)["RMSFirstNBinsBurst"];
	fitdat->weightsum += (*fitweights)["burstmode"];
	fitdat->weightsum += (*fitweights)["burstlengthmean"];
	fitdat->weightsum += (*fitweights)["burstlengthsd"];
	fitdat->weightsum += (*fitweights)["burstsilencemean"];
	fitdat->weightsum += (*fitweights)["burstsilencesd"];
	fitdat->weightsum += (*fitweights)["burstintrafreq"];

	fitdat->score = fitdat->score / fitdat->weightsum;

	
	ofp.Close();
}