

/*
*  evofitscore.cpp
*  
*  Created by Duncan MacGregor and Tom Clayton 
*  University of Edinburgh 2019
*  Released under MIT license, see https://opensource.org/licenses/MIT
*
*/


#include "hypodat.h"
#include "hypopanels.h"
#include <cmath>


void SpikeDat::FitScoreOxy(SpikeDat *testdata, FitDat *fitdat, FitSet *fitset, FitConSet *conset)
{
	int i;
	int histmax = 512;
	double maxcount;

	// SpikeDat class contains spike count, spike times, and ISI histogram - testdata is the experimental data
	//	
	// FitDat class stores total and individual fit measure scores
	//
	// FitSet class stores fit measures and weights - allows parameterised set of fit measures
	//
	// testdata contains spike data for fitting, pre-analysed to generate quad bin histogram, hazard, and IoD range

	ParamStore *fitparams = new ParamStore();

	//(*fitparams)["RMSHeadStart"] = 0;
	//(*fitparams)["RMSHeadStop"] = 30;
	//(*fitparams)["RMSBinRangeStart"] = 30;
	//(*fitparams)["RMSBinRangeFinish"] = 125;

	(*fitparams)["RMSHeadStart"] = conset->GetCon("RMSHeadStart").value;
	(*fitparams)["RMSHeadStop"] = conset->GetCon("RMSHeadStop").value;
	(*fitparams)["RMSBinRangeStart"] = conset->GetCon("RMSBinRangeStart").value;
	(*fitparams)["RMSBinRangeFinish"] = conset->GetCon("RMSBinRangeFinish").value;

	if(spikecount < 10) {
		fitdat->scores["RMSFirstNBins"] = -1;
		fitdat->scores["RMSBinRange"] = -1;
		fitdat->scores["RMSHaz"] = -1; 
		fitdat->scores["RMSIoD"] = -1;
		fitdat->score = 1000;
		return;
	}

	// Spike Data Processing - generate quadritic binned hazard from GPU generated quad histogram
	double initbinwidth = 1;
	double bininc = 0.05;
	double width, histsum;
	double hazremain, hazmin;


	// Histogram mode
	maxcount = 0;
	for(i=0; i<histmax; i++) {
		if(histquadsm[i] > maxcount) {
			maxcount = histquadsm[i];
			histquadmode = i;
		}
	}

	// Quad bin x
	for (i=0; i<histmax; i++) histquadx[i] = i * initbinwidth + (i * (i - 1) / 2) * bininc;

	// Linearise
	histsum = 0;
	for(i=0; i<histmax-1; i++) {
		width = (histquadx[i+1] - histquadx[i]);
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

	
	// ISI Histogram First N Bins RMS
	double RMSError;
	double Error;
	double CutOff;
	double Big, Small;

	if(histquadsm[histquadmode] > testdata->histquadsm[testdata->histquadmode])
		CutOff = histquadsm[histquadmode] * 0.1;
	else CutOff = testdata->histquadsm[testdata->histquadmode] * 0.1;

	// The RMS error must be flexible to take account of bins with very small values
	// If a bin has a value of below 0.01 (1% of all events) then it is not normalised  

	RMSError = 0;
	for(i=(*fitparams)["RMSHeadStart"]; i<(*fitparams)["RMSHeadStop"]; i++) {               
		if(testdata->histquadsm[i] > histquadsm[i]) {
			Big = testdata->histquadsm[i];
			Small = histquadsm[i];
		}
		else {
			Big = histquadsm[i];
			Small = testdata->histquadsm[i];
		}

		//If both are less than the cutoff then use alternate rules
		if (Big < CutOff) Error = (Big - Small) * 100.0;                    
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	RMSError /= ((*fitparams)["RMSHeadStop"] - (*fitparams)["RMSHeadStart"]);
	RMSError = sqrt(RMSError);

	fitdat->RMSFirstNBins = RMSError;
	fitdat->scores["RMSFirstNBins"] = RMSError;


	// ISI Histogram Bin Range RMS
	RMSError = 0;
	for(i=(*fitparams)["RMSBinRangeStart"]; i<(*fitparams)["RMSBinRangeFinish"]; i++) {
		if(testdata->histquadsm[i] > histquadsm[i]) {
			Big = testdata->histquadsm[i];
			Small = histquadsm[i];
		}
		else {
			Big = histquadsm[i];
			Small = testdata->histquadsm[i];
		}

		//If both are less than the cutoff then use alternate rules
		if (Big < CutOff) Error = (Big - Small) * 100.0;
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	RMSError /= ((*fitparams)["RMSBinRangeFinish"] - (*fitparams)["RMSBinRangeStart"]);
	RMSError = sqrt(RMSError);

	fitdat->RMSBinRange = RMSError;
	fitdat->scores["RMSBinRange"] = RMSError;

	double errcalc;

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
		if (Big < CutOff) Error = (Big - Small) * 100.0;
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	errcalc = RMSError;
	RMSError /= MinHazSize;
	RMSError = sqrt(RMSError);

	//Extra error is added for % difference between bin counts
	RMSError += ((double)(MaxHazSize - MinHazSize)) / ((double)MinHazSize)*100;

	fitdat->RMSHaz = RMSError;
	fitdat->scores["RMSHaz"] = RMSError; 
	

	// IoD range RMS
	int IoDcount = 7;
	IoDdata[0] = dispcalc(500);       // index of dispersion (IoD) function dispcalc(binsize in ms)
	IoDdata[1] = dispcalc(1000);      // measures variability in spike rate
	IoDdata[2] = dispcalc(2000);      // purely random spiking produces IoD = 1
	IoDdata[3] = dispcalc(4000);
	IoDdata[4] = dispcalc(6000);
	IoDdata[5] = dispcalc(8000);
	IoDdata[6] = dispcalc(10000);

	RMSError = 0;
	for(i=0; i<IoDcount; i++) {
		if(testdata->IoDdata[i] > IoDdata[i]) {
			Big = testdata->IoDdata[i];
			Small = IoDdata[i];
		}
		else {
			Big = IoDdata[i];
			Small = testdata->IoDdata[i];
		}
		Error = (Big - Small) / Big * 100;
		RMSError += Error * Error;
	}
	fitdat->RMSIoD = RMSError / 100;
	fitdat->scores["RMSIoD"] = RMSError / 100;


	// Weighted Sum
	fitdat->score = 0;
	fitdat->weightsum = 0;
	wxString tag;

	for(i=0; i<fitset->measureCount; i++) {
		tag = fitset->tags[i];
		fitdat->score += fitset->measures[i].weight * fitdat->scores[tag];
		fitdat->weightsum += fitset->measures[i].weight;
	}
	fitdat->score = fitdat->score / fitdat->weightsum;
	
	delete fitparams;
}


void SpikeDat::quadhist()           // generates from ISIs stored in isis[] a quadratic binned ISI histogram and hazard function
{
	int i;

	// Quad Binned Histogram                      

	double initbinwidth = 1;
	double bininc = 0.05;
	double d_fInitBinWidth = 0.001;
	double d_fBinIncrAmount = 0.00005;
	double b = initbinwidth - bininc / 2;
	double BSquared = b*b;
	double FourA = 2*bininc;
	double TwoA = bininc;

	//This equation defines an ever increasing bin width                       
	//Even though it is quantised, it will increase by bininc               
	//every bin.

	for(i=1; i<spikecount; i++) {
		float fBinNo = (-b + sqrt(BSquared + FourA * isis[i]))/TwoA;
		int iBinNo = fBinNo;
		histquad[iBinNo]++;
		if(iBinNo > histquad.max) histquad.max = iBinNo;
	}

	//Smooth and Scale ISI Histogram

	double NumBinnedEvents = 0;
	double Div;
	int histmax = 512;
	//double htemp[1000];
	double maxcount = 0;
	double width, histsum, hazremain;
	double hazmin = 0.01;

	histquadsm.max = histmax;

	for(i=0; i<histmax; i++) {
		Div = 0;
		histquadsm[i] = 0;
		for (int j = i - 2; j < i + 3; j++) {
			if ((j > -1) & (j < 1000)) {
				histquadsm[i] += histquad[j];
				Div++;
			}
		}
		histquadsm[i] /= Div;
		NumBinnedEvents += histquadsm[i];
	}

	// normalise
	for(i=0; i<histmax; i++) histquadsm[i] = histquadsm[i] / NumBinnedEvents;


	// Mode for fitness
	maxcount = 0;
	for(i=0; i<histmax; i++) {
		if(histquadsm[i] > maxcount) {
			maxcount = histquadsm[i];
			histquadmode = i;
		}
	}

	// quad bin x
	for (i=0; i<histmax; i++) histquadx[i] = i * initbinwidth + (i * (i - 1) / 2) * bininc;

	// linearise
	histsum = 0;
	for(i=0; i<histmax-1; i++) {
		width = (histquadx[i+1] - histquadx[i]);         
		histquadlin[i] = histquadsm[i] / width;
		histsum += histquadlin[i];
	}
	for(i=0; i<histmax; i++) histquadlin[i] = histquadlin[i] / histsum;

	// Quad Hazard

	hazremain = 1;
	for(i=0; i<histmax; i++) {
		hazquad[i] = histquadlin[i] / hazremain;
		if(hazremain < hazmin) {
			hazquad[i] = 0;
			hazquadbins = i;
			break;
		}
		hazremain -= histquadlin[i];
	}
}


// Duplicated from IoDcalc purely to make simpler code set for publication (Trystan PLoS One 2017)

double SpikeDat::dispcalc(int binsize)
{
	int i;
	int maxbin = 10000;
	int spikerate[10000];
	int laststep;
	double mean, variance, dispersion = 0;
	double timeshift = 0;
	bool filediag = false;
	FILE *ofp;
	char diagname[20];
	sprintf(diagname, "normalIoD%d.txt", binsize);

	if(filediag) ofp = fopen(diagname, "w");

	//if(times[0] > 1000) timeshift = times[0] - 1000;        // for data where recording starts at non-zero time point

	// calculate spike rate for binsize
	for(i=0; i<maxbin; i++) spikerate[i] = 0;
	for(i=0; i<spikecount; i++) {
		if((times[i] - timeshift) / binsize < maxbin) spikerate[(int)((times[i] - timeshift) + 0.5) / binsize]++;
		if(i%10 == 0 && filediag) fprintf(ofp, "spike %d  times[i] %.2f  index %d\n", i, times[i], (int)((times[i] - timeshift) + 0.5) / binsize);
	}
	laststep = ((int)(times[spikecount - 1] - timeshift) / binsize) - 4;
	if(laststep > maxbin) laststep = maxbin;

	// calculate index of dispersion
	mean = 0;
	variance = 0;
	for(i=0; i<laststep; i++) mean = mean +  spikerate[i];	//mean
	mean = mean / laststep;
	for(i=0; i<laststep; i++) variance += (mean - spikerate[i]) * (mean - spikerate[i]);	// variance
	variance = variance / laststep;
	dispersion = variance / mean;		// dispersion

	if(filediag) fclose(ofp);
	return dispersion;
}


// Calculates binned spike rates and index of dispersion for selection or burst scan filtered spike time data 

double BurstDat::dispcalcburst(int binsize, int maxint)
{
	int i, bindex;          // bin index
	int maxbin = 10000;
	//int spikerate[10000];
	int laststep = 0;
	double mean, variance, dispersion = 0;
	double timeshift = 0;
	double burstshift;
	double burstend;
	int spikecount = spikedata->spikecount;
	int burstindex;
	//wxString text;
	bool shiftflag = true;
	bool filediag = true;
	bool selecton;
	FILE *ofp;
	char diagname[20];
	sprintf(diagname, "burstIoD%d.txt", binsize);
	wxString text;

	if(filediag) ofp = fopen(diagname, "w");

	spikedata->diagbox->Write(text.Format("dispcalcburst %d, select mode %d\n", binsize, selectmode));

	//if(times[0] > 1000) timeshift = times[0] - 1000;        // for data where recording starts at non-zero time point

	burstshift = 0;
	burstend = 0;
	burstindex = 1;
	selecton = false;

	// calculate spike rate for binsize
	for(i=0; i<maxbin; i++) spikerate[i] = 0;
	for(i=0; i<spikecount; i++) {
		if(!selectmode) {
			if(!spikes[i]) continue; 
			if(shiftflag && i == bustore[burstindex].start) burstshift += times[i] - burstend;
			if(i == bustore[burstindex].end) {
				burstend = times[i];
				burstindex++;
			}
		}
		else {
			if(!spikes[i] && selecton) {
				selecton = false;
				burstend = times[i-1]; 
			}
			if(spikes[i] && !selecton) {
				selecton = true;
				burstshift += times[i] - burstend;
			}
			if(!spikes[i]) continue; 
		}
		bindex = (int)((times[i] - burstshift + 0.5) / binsize);
		//bindex = round((times[i] - burstshift) / binsize);
		//if((times[i] - burstshift) / binsize < maxbin) spikerate[(int)((times[i] - burstshift) + 0.5) / binsize]++;
		if(bindex < maxbin) spikerate[bindex]++;
		if(i%10 == 0 && filediag) fprintf(ofp, "spike %d  spikes[i] %d  selecton %d  burst %d  times[i] %.2f  burstshift %.2f  index %d\n", 
			i, spikes[i], selecton, burstindex, times[i], burstshift, bindex);
		//spikedata->diagbox->Write(text.Format("spike %d  spikes[i] %d  times[i] %.2f  timeshift %.2f  burstshift %.2f  index %d\n", 
		//	i, spikes[i], times[i], timeshift, burstshift, (int)((times[i] - timeshift - burstshift) + 0.5) / binsize));
		//fflush(ofp);
		laststep = bindex;
	}
	//laststep = ((int)(times[spikecount - 1] - burstshift) / binsize) - 4;
	if(laststep > maxbin) laststep = maxbin;


	// calculate index of dispersion
	if(!laststep) {                    // in case of no burst identified spikes in data
		if(filediag) fclose(ofp);
		return 0;
	}
	mean = 0;
	variance = 0;
	for(i=0; i<laststep; i++) mean = mean + spikerate[i];	//mean
	mean = mean / laststep;
	for(i=0; i<laststep; i++) variance += (mean - spikerate[i]) * (mean - spikerate[i]);	// variance
	variance = variance / laststep;
	dispersion = variance / mean;		// dispersion

	if(filediag) fclose(ofp);
	return dispersion;
}


void BurstDat::IoDrange()
{
	//wxString text;

	//if(spikedata->diagbox) spikedata->diagbox->Write(text.Format("\nIoDrange %d bursts\n", numbursts));

	if(selectmode || numbursts) {
		IoDdata[0] = dispcalcburst(500, maxint);
		IoDdata[1] = dispcalcburst(1000, maxint);
		IoDdata[2] = dispcalcburst(2000, maxint);
		IoDdata[3] = dispcalcburst(4000, maxint);
		IoDdata[4] = dispcalcburst(6000, maxint);
		IoDdata[5] = dispcalcburst(8000, maxint);
		IoDdata[6] = dispcalcburst(10000, maxint);
	}
	else {
		IoDdata[0] = spikedata->dispcalc(500);
		IoDdata[1] = spikedata->dispcalc(1000);
		IoDdata[2] = spikedata->dispcalc(2000);
		IoDdata[3] = spikedata->dispcalc(4000);
		IoDdata[4] = spikedata->dispcalc(6000);
		IoDdata[5] = spikedata->dispcalc(8000);
		IoDdata[6] = spikedata->dispcalc(10000);
	}

	//if(spikedata->diagbox) spikedata->diagbox->Write(text.Format("IoDrange %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", 
	//	IoDdata[0], IoDdata[1], IoDdata[2], IoDdata[3], IoDdata[4], IoDdata[5], IoDdata[6]));

	IoDdataX[0] = 5;            // x positions for displaying IoD range as bar chart
	IoDdataX[1] = 15;
	IoDdataX[2] = 25;
	IoDdataX[3] = 35;
	IoDdataX[4] = 45;
	IoDdataX[5] = 55;
	IoDdataX[6] = 65;
}
