

#include "hypodat.h"
#include "hypopanels.h"
#include <cmath>


void SpikeDat::FitScore(SpikeDat *testdata, FitDat *fitdat, FitSet *fitset, FitConSet *conset, ParamStore *burstparams)
{
	if(fitType == 1) FitScoreOxy(testdata, fitdat, fitset, conset);

	else if(fitType == 2 && burstparams != NULL) {
		ParamStore *fitparams = conset->GetParams();
		FitScoreVasoFast(testdata, fitdat, fitset);
	}

	else FitScoreBasic(testdata, fitdat, fitset, conset);
}


void SpikeDat::FitScoreVasoFast(SpikeDat *testdata, FitDat *fitdat, FitSet *fitset, int burstmode, int IoDmode)
{
	int i;
	int histmax = 512;
	double maxcount;

	//TextFile ofp;
	//bool burstmode = true;
	//bool IoDmode = false;
	bool IoDburstmode;

	if(burstmode == 1) IoDburstmode = true;


	//if(fitdiag) ofp.New("fitscorevaso-diag.txt");


	if(spikecount < 10) {
		fitdat->scores["RMSFirstNBins"] = -1;
		fitdat->scores["RMSBinRange"] = -1;
		fitdat->scores["RMSHaz"] = -1; 
		fitdat->scores["RMSIoD"] = -1;
		fitdat->score = 1000;
		return;
	}

	
	// Spike Data Processing //////////////////Duplicate of neurocalc processing for GPU generated data

	double initbinwidth = 1;
	double bininc = 0.05;
	double width, histsum;
	double hazremain, hazmin;


	// Mode for fitness
	maxcount = 0;
	for(i=0; i<histmax; i++) {	
		if(histquadsmfit[i] > maxcount) {
			maxcount = histquadsmfit[i];
			histquadmode = i;
		}
	}

	// Quad bin x
	for (i=0; i<histmax; i++) histquadxfit[i] = i * initbinwidth + (i * (i - 1) / 2) * bininc;

	// Linearise
	histsum = 0;
	for(i=0; i<histmax-1; i++) {
		width = (histquadxfit[i+1] - histquadxfit[i]);        
		histquadlinfit[i] = histquadsmfit[i] / width;
		histsum += histquadlinfit[i];
	}
	for(i=0; i<histmax-1; i++) histquadlinfit[i] = histquadlinfit[i] / histsum;

	// Quad Hazard
	hazmin = 0.01;
	hazremain = 1;
	for(i=0; i<histmax-1; i++) {
		hazquadfit[i] = histquadlinfit[i] / hazremain;
		if(hazremain < hazmin) {
			hazquadfit[i] = 0;
			hazquadbins = i;
			break;
		}
		hazremain -= histquadlinfit[i];
	}


	////////////////////////////////////////////////////////////////////////////////////


	// ISI Histogram First N Bins RMS

	double RMSError;
	double Error;
	double CutOff;
	double Big, Small;

	if(histquadsmfit[histquadmode] > testdata->histquadsmfit[testdata->histquadmode])
		CutOff = histquadsmfit[histquadmode] * 0.1;
	else CutOff = testdata->histquadsmfit[testdata->histquadmode] * 0.1;

	RMSError = 0;
	for(i=RMSHeadStart; i<RMSHeadStop; i++) {    
		if(testdata->histquadsmfit[i] > histquadsmfit[i]) {
			Big = testdata->histquadsmfit[i];
			Small = histquadsmfit[i];
		}
		else {
			Big = histquadsmfit[i];
			Small = testdata->histquadsmfit[i];
		}

		//If both are less than the cutoff then use alternate rules
		if (Big < CutOff) Error = (Big - Small) * 100.0;                 
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	RMSError /= (RMSHeadStop - RMSHeadStart);
	RMSError = sqrt(RMSError);

	fitdat->RMSFirstNBins = RMSError;
	//fitdat->scores["RMSFirstNBins"] = RMSError;
	

	// ISI Histogram Bin Range RMS
	RMSError = 0;
	for(i=RMSBinRangeStart; i<RMSBinRangeFinish; i++) {
		if(testdata->histquadsmfit[i] > histquadsmfit[i]) {
			Big = testdata->histquadsmfit[i];
			Small = histquadsmfit[i];
		}
		else {
			Big = histquadsmfit[i];
			Small = testdata->histquadsmfit[i];
		}

		//If both are less than the cutoff then use alternate rules
		if (Big < CutOff) Error = (Big - Small) * 100.0;
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	RMSError /= (RMSBinRangeFinish - RMSBinRangeStart);
	RMSError = sqrt(RMSError);

	fitdat->RMSBinRange = RMSError;
	//fitdat->scores["RMSBinRange"] = RMSError;


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
		if(testdata->hazquadfit[i] > hazquadfit[i]) {
			Big = testdata->hazquadfit[i];
			Small = hazquadfit[i];
		}
		else {
			Big = hazquadfit[i];
			Small = testdata->hazquadfit[i];
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
	//fitdat->scores["RMSHaz"] = RMSError; 


	//// Burst Profile Analysis

	if(burstmode) {

		burstdata->BurstScanFit();
		if(burstdata->numbursts > 0) burstdata->BurstProfileFit();
		else {
			for(i=0; i<histmax; i++) burstdata->profilesmfit[i] = 0; 
			burstdata->pmodetime = 0;
			burstdata->pmoderate = 0;
		}

		// Burst Mode
		/*int pmode = 0;
		burstdata->pnzcount = 0;
		for(i=0; i<histmax; i++) {
			if(burstdata->profilesm[i] > burstdata->profilesm[pmode]) pmode = i;
			if(burstdata->profilesm[i] > 0) burstdata->pnzcount = i;
		}
		burstdata->pmodetime = pmode;
		burstdata->pmoderate = burstdata->profilesm[pmode];*/

		// First N Bins RMS

		RMSError = 0;
		//int burstheadbins = (*fitparams)["RMSBurstHeadBins"];
		int burstheadbins = RMSBurstHeadBins;

		if(burstdata->pnzcount < burstheadbins) burstdata->pnzcount = burstheadbins + 10;

		double AvSum = 0;   // Profile average after burst profile head
		for (i = burstheadbins; i < testdata->burstdata->pnzcount; i++) AvSum += testdata->burstdata->profilesmfit[i];
		AvSum /= testdata->burstdata->pnzcount - burstheadbins;

		CutOff = AvSum / 5.0;

		double testrate, datarate;

		for(i=0; i<burstheadbins; i++) {
			//The RMS error must be flexible to take account of bins with very small values
			//If a bin has a value of below 0.01 (1% of all events) then it is not normalised

			testrate = testdata->burstdata->profilesm[i] - AvSum;
			datarate = burstdata->profilesmfit[i] - AvSum;

			if (testrate > datarate) { Big = testrate; Small = datarate; }
			else { Big = datarate; Small = testrate; }

			//If both are less than the cutoff then use alternate rules
			if ((Big < CutOff) && (Big > -CutOff)) Error = (Big - Small) * 20.0;
			else Error = abs((Big - Small) / Big) * 100.0;

			RMSError += Error * Error;
		}

		RMSError /= burstheadbins;
		RMSError = sqrt(RMSError);

		fitdat->RMSBurstHead = RMSError;
		//fitdat->scores["RMSBurstHead"] = RMSError;


		// Burst Mode
		fitdat->burstmode = (abs(testdata->burstdata->pmoderate - burstdata->pmoderate) / testdata->burstdata->pmoderate) * 50.0;
		fitdat->burstmode += (abs(testdata->burstdata->pmodetime - burstdata->pmodetime) / testdata->burstdata->pmodetime) * 50.0;
		//fitdat->scores["BurstMode"] = fitdat->burstmode;
	
		// Burst Stats
		fitdat->burstlengthmean = (abs(testdata->burstdata->meanlength - burstdata->meanlength) / testdata->burstdata->meanlength) * 100.0;
		//fitdat->scores["BurstLengthMean"] = fitdat->burstlengthmean;

		fitdat->burstlengthsd = (abs(testdata->burstdata->sdlength - burstdata->sdlength) / testdata->burstdata->sdlength) * 100.0;
		//fitdat->scores["BurstLengthSD"] = fitdat->burstlengthsd;
		
		fitdat->burstsilencemean = (abs(testdata->burstdata->meansilence - burstdata->meansilence) / testdata->burstdata->meansilence) * 100.0;
		//fitdat->scores["BurstSilenceMean"] = fitdat->burstsilencemean;
	
		fitdat->burstsilencesd = (abs(testdata->burstdata->sdsilence - burstdata->sdsilence) / testdata->burstdata->sdsilence) * 100.0;
		//fitdat->scores["BurstSilenceSD"] = fitdat->burstsilencesd;
	
		fitdat->burstintrafreq = (abs(testdata->burstdata->freq - burstdata->freq) / testdata->burstdata->freq) * 100.0;
		//fitdat->scores["BurstIntraFreq"] = fitdat->burstintrafreq;
	}

	int IoDcount = 7;

	if(IoDmode) {
		IoDdata[0] = dispcalc(500);
		IoDdata[1] = dispcalc(1000);
		IoDdata[2] = dispcalc(2000);
		IoDdata[3] = dispcalc(4000);
		IoDdata[4] = dispcalc(6000);
		IoDdata[5] = dispcalc(8000);
		IoDdata[6] = dispcalc(10000);

		RMSError = 0;

		for(i=0; i<IoDcount; i++) {

			//if(fitdiag) ofp.WriteLine(text.Format("IoD %d  Exp %.6f  Mod %.6f\n", i, testdata->IoDdata[i], IoDdata[i])); 

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
			//if(fitdiag) ofp.WriteLine(text.Format("Error %.6f  RMSError %.6f\n", Error, RMSError)); 
		}

		fitdat->RMSIoD = RMSError / 100;
		//fitdat->scores["RMSIoD"] = RMSError / 100;

		//if(fitdiag) ofp.WriteLine(text.Format("IoD %.6f\n", fitdat->RMSIoD)); 
	}


	//if(fitset->GetMeasure("RMSBurstIoD").weight > 0) {
	if(IoDburstmode) {

		RMSError = 0;

		for(i=0; i<IoDcount; i++) {

			//if(fitdiag) ofp.WriteLine(text.Format("IoD %d  Exp %.6f  Mod %.6f\n", i, testdata->burstdata->IoDdata[i], burstdata->IoDdata[i])); 

			if(testdata->burstdata->IoDdata[i] > burstdata->IoDdata[i]) {
				Big = testdata->burstdata->IoDdata[i];
				Small = burstdata->IoDdata[i];
			}
			else {
				Big = burstdata->IoDdata[i];
				Small = testdata->burstdata->IoDdata[i];
			}

			Error = (Big - Small) / Big * 100;
			RMSError += Error * Error;
			//if(fitdiag) ofp.WriteLine(text.Format("Error %.6f  RMSError %.6f\n", Error, RMSError)); 
		}

		fitdat->RMSBurstIoD = RMSError / 100;
		//fitdat->scores["RMSBurstIoD"] = RMSError / 100;

		//if(fitdiag) ofp.WriteLine(text.Format("Burst IoD %.6f\n", fitdat->RMSBurstIoD)); 
	}


	// Weighted Sum
	fitdat->score = 0;
	fitdat->weightsum = 0;
	//wxString tag;

	double score = 0;

	//diagbox->Write(text.Format("score %.2f  weight %.2f  fit %.2f\n", score, fitset->measures[0].weight, fitdat->RMSFirstNBins));

	score += fitset->measures[0].weight * fitdat->RMSFirstNBins;
	score += fitset->measures[1].weight * fitdat->RMSBinRange;
	score += fitset->measures[2].weight * fitdat->RMSHaz;
	score += fitset->measures[3].weight * fitdat->RMSIoD;
	score += fitset->measures[4].weight * fitdat->RMSBurstHead;
	score += fitset->measures[5].weight * fitdat->burstmode;
	score += fitset->measures[6].weight * fitdat->burstlengthmean;

	//diagbox->Write(text.Format("score %.2f  weight %.2f  fit %.2f\n", score, fitset->measures[6].weight, fitdat->burstlengthmean));

	score += fitset->measures[7].weight * fitdat->burstlengthsd;
	score += fitset->measures[8].weight * fitdat->burstsilencemean;
	score += fitset->measures[9].weight * fitdat->burstsilencesd;
	score += fitset->measures[10].weight * fitdat->burstintrafreq;
	score += fitset->measures[11].weight * fitdat->RMSBurstIoD;

	for(i=0; i<fitset->measureCount; i++) {	
		//tag = fitset->tags[i];
		//fitdat->score += fitset->measures[i].weight * fitdat->scores[fitset->tags[i]];      // check score tag and weight ordering   27/9/19
		fitdat->weightsum += fitset->measures[i].weight;
	}


	//diagbox->Write(text.Format("score sum %.2f  weight %.2f  fit %.2f\n", score, fitset->measures[6].weight, fitdat->burstlengthmean));
	
	fitdat->score = score / fitdat->weightsum;

	//if(fitdiag) ofp.Close();
}


void SpikeDat::FitScoreVaso(SpikeDat *testdata, FitDat *fitdat, FitSet *fitset, FitConSet *conset, ParamStore *burstparams)
{
	int i;
	int histmax = 512;
	double maxcount;

	wxString text;
	//ParamStore *fitweights;
	TextFile ofp;
	bool burstmode = true;
	bool IoDmode = false;

	fitdiag = false;

	if(fitdiag) ofp.New("fitscore-diag.txt");

	//diagbox->Write("FitScoreVaso...");

	ParamStore *fitparams = new ParamStore();

	(*fitparams)["RMSHeadStart"] = conset->GetCon("RMSHeadStart").value;
	(*fitparams)["RMSHeadStop"] = conset->GetCon("RMSHeadStop").value;
	(*fitparams)["RMSBinRangeStart"] = conset->GetCon("RMSBinRangeStart").value;
	(*fitparams)["RMSBinRangeFinish"] = conset->GetCon("RMSBinRangeFinish").value;
	(*fitparams)["RMSBurstHeadBins"] = conset->GetCon("RMSBurstHeadBins").value;     // default 20

	if(spikecount < 10) {
		fitdat->scores["RMSFirstNBins"] = -1;
		fitdat->scores["RMSBinRange"] = -1;
		fitdat->scores["RMSHaz"] = -1; 
		fitdat->scores["RMSIoD"] = -1;
		fitdat->score = 1000;
		return;
	}

	
	//double chisum = 0;
	//int bincount = 300;
	//int sumcount = 0;

	/*
	for(i=0; i<bincount; i++) {
	if(haz5[i] > 0) chisum += pow((testdata->haz5[i] - haz5[i]), 2) / haz5[i];
	ofp.WriteLine(text.Format("bin %d  mod %.4f  exp %.4f  chisum %.4f", i, haz5[i], testdata->haz5[i], chisum)); 
	}
	fitdat->haz5chisq = (double)chisum / bincount;
	//return chiscore;
	ofp.WriteLine(text.Format("haz5chi %.6f", fitdat->haz5chisq)); 
	*/


	// Spike Data Processing //////////////////Duplicate of neurocalc processing for GPU generated data

	double initbinwidth = 1;
	double bininc = 0.05;
	double width, histsum;
	double hazremain, hazmin;


	// Mode for fitness
	maxcount = 0;
	for(i=0; i<histmax; i++) {
		//if(i < 20) diagbox->Write(text.Format("all cell bin %d histquadsm %.8f\n", i, histquadsm[i]));
		if(histquadsm[i] > maxcount) {
			maxcount = histquadsm[i];
			histquadmode = i;
			//diagbox->Write(text.Format("new mode bin %d histquadsm %.4f\n", i, histquadsm[i]));
		}
	}

	//diagbox->Write(text.Format("histquadmode %.2f\n", histquadmode));

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
		hazquadfit[i] = histquadlin[i] / hazremain;
		if(hazremain < hazmin) {
			hazquadfit[i] = 0;
			hazquadbins = i;
			break;
		}
		hazremain -= histquadlin[i];
	}


	/*
	// Averaged burst head profile - count length and find mode

	int pmode = 0;                // mode bin of burst profile
	burstdata->pnzcount = 0;      // profile non-zero bin count
	for(i=0; i<histmax; i++) {
		if(burstdata->profilesm[i] > burstdata->profilesm[pmode]) pmode = i;
		if(burstdata->profilesm[i] > 0) burstdata->pnzcount = i;
	}
	burstdata->pmodetime = pmode;
	burstdata->pmoderate = burstdata->profilesm[pmode];*/    //  - duplicate found below  3/9/19


	////////////////////////////////////////////////////////////////////////////////////


	// ISI Histogram First N Bins RMS

	double RMSError;
	double Error;
	double CutOff;
	double Big, Small;

	if(histquadsm[histquadmode] > testdata->histquadsm[testdata->histquadmode])
		CutOff = histquadsm[histquadmode] * 0.1;
	else CutOff = testdata->histquadsm[testdata->histquadmode] * 0.1;


	//diagbox->Write(text.Format("\nRMSHead Start %.0f Stop %.0f\n", (*fitcon)["RMSHeadStart"], (*fitcon)["RMSHeadStop"]));

	// The RMS error must be flexible to take account of bins with very small values
    // If a bin has a value of below 0.01 (1% of all events) then it is not normalised    - Tom description

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

	//diagbox->Write(text.Format("\nRMSHead Error Sum %.4f\n", RMSError));

	RMSError /= ((*fitparams)["RMSHeadStop"] - (*fitparams)["RMSHeadStart"]);
	RMSError = sqrt(RMSError);

	//diagbox->Write(text.Format("\nRMSHead Error RMS %.4f\n", RMSError));

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
		if(testdata->hazquadfit[i] > hazquadfit[i]) {
			Big = testdata->hazquadfit[i];
			Small = hazquadfit[i];
		}
		else {
			Big = hazquadfit[i];
			Small = testdata->hazquadfit[i];
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

	if(fitdiag) ofp.WriteLine(text.Format("Haz RMS RMSRrror %.6f Pre SQRT %.6f", RMSError, errcalc)); 


	//// Burst Profile Analysis

	if(burstmode) {

		if(fitdiag) diagbox->Write("BurstScan...");


		burstdata->BurstScanFit();
		if(burstdata->numbursts > 0) BurstProfile();
		else {
			for(i=0; i<histmax; i++) burstdata->profilesm[i] = 0; 
		}
		

		//BurstScanFit(burstbox);
		//burstdata->IntraBurstAnalysis();
		//BurstProfile();

		if(fitdiag) diagbox->Write(text.Format("Scan OK, %d bursts", burstdata->numbursts));

		// Burst Mode
		int pmode = 0;
		burstdata->pnzcount = 0;
		for(i=0; i<histmax; i++) {
			if(burstdata->profilesm[i] > burstdata->profilesm[pmode]) pmode = i;
			if(burstdata->profilesm[i] > 0) burstdata->pnzcount = i;
		}
		burstdata->pmodetime = pmode;
		burstdata->pmoderate = burstdata->profilesm[pmode];

		// First N Bins RMS

		RMSError = 0;
		int burstheadbins = (*fitparams)["RMSBurstHeadBins"];

		if(burstdata->pnzcount < burstheadbins) burstdata->pnzcount = burstheadbins + 10;

		double AvSum = 0;   // Profile average after burst profile head
		for (i = burstheadbins; i < testdata->burstdata->pnzcount; i++) AvSum += testdata->burstdata->profilesm[i];
		AvSum /= testdata->burstdata->pnzcount - burstheadbins;

		CutOff = AvSum / 5.0;

		double testrate, datarate;

		for(i=0; i<burstheadbins; i++) {
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

		RMSError /= burstheadbins;
		RMSError = sqrt(RMSError);

		fitdat->RMSBurstHead = RMSError;
		fitdat->scores["RMSBurstHead"] = RMSError;
		//fitdat->score += fitdat->RMSFirstNBinsBurst * (*fitweights)["RMSFirstNBinsBurst"];


		// Burst Mode

		fitdat->burstmode = (abs(testdata->burstdata->pmoderate - burstdata->pmoderate) / testdata->burstdata->pmoderate) * 50.0;
		fitdat->burstmode += (abs(testdata->burstdata->pmodetime - burstdata->pmodetime) / testdata->burstdata->pmodetime) * 50.0;
		fitdat->scores["BurstMode"] = fitdat->burstmode;
		//fitdat->score += fitdat->burstmode * (*fitweights)["burstmode"];

		if(fitdiag) {
			diagbox->Write(text.Format("pmode rate time mod %.6f  exp %.6f  score %.6f\n", 
				burstdata->pmoderate, burstdata->pmodetime, testdata->burstdata->pmoderate, testdata->burstdata->pmodetime, fitdat->burstmode)); 
			//ofp.WriteLine(text.Format("mode rate time mod %.6f  exp %.6f  score %.6f", burstdata->meanlength, testdata->burstdata->meanlength, fitdat->burstlengthmean)); 
		}

		// Burst Stats

		fitdat->burstlengthmean = (abs(testdata->burstdata->meanlength - burstdata->meanlength) / testdata->burstdata->meanlength) * 100.0;
		fitdat->scores["BurstLengthMean"] = fitdat->burstlengthmean;
		//fitdat->score += fitdat->burstlengthmean * (*fitweights)["burstlengthmean"];

		if(fitdiag) {
			diagbox->Write(text.Format("burst mod %.6f  exp %.6f  score %.6f\n", burstdata->meanlength, testdata->burstdata->meanlength, fitdat->burstlengthmean)); 
			ofp.WriteLine(text.Format("burst mod %.6f  exp %.6f  score %.6f", burstdata->meanlength, testdata->burstdata->meanlength, fitdat->burstlengthmean)); 
		}

		fitdat->burstlengthsd = (abs(testdata->burstdata->sdlength - burstdata->sdlength) / testdata->burstdata->sdlength) * 100.0;
		fitdat->scores["BurstLengthSD"] = fitdat->burstlengthsd;
		//fitdat->score += fitdat->burstlengthsd * (*fitweights)["burstlengthsd"];

		if(fitdiag) {
			ofp.WriteLine(text.Format("burstsd mod %.6f  exp %.6f  score %.6f", burstdata->sdlength, testdata->burstdata->sdlength, fitdat->burstlengthsd)); 
			diagbox->Write(text.Format("burstsd mod %.6f  exp %.6f  score %.6f\n", burstdata->sdlength, testdata->burstdata->sdlength, fitdat->burstlengthsd)); 
		}

		fitdat->burstsilencemean = (abs(testdata->burstdata->meansilence - burstdata->meansilence) / testdata->burstdata->meansilence) * 100.0;
		fitdat->scores["BurstSilenceMean"] = fitdat->burstsilencemean;
		//fitdat->score += fitdat->burstsilencemean * (*fitweights)["burstsilencemean"];

		

		if(fitdiag) ofp.WriteLine(text.Format("silence mod %.6f  exp %.6f  score %.6f", burstdata->meansilence, testdata->burstdata->meansilence, fitdat->burstsilencemean)); 

		fitdat->burstsilencesd = (abs(testdata->burstdata->sdsilence - burstdata->sdsilence) / testdata->burstdata->sdsilence) * 100.0;
		fitdat->scores["BurstSilenceSD"] = fitdat->burstsilencesd;
		//fitdat->score += fitdat->burstsilencesd * (*fitweights)["burstsilencesd"];

		fitdat->burstintrafreq = (abs(testdata->burstdata->freq - burstdata->freq) / testdata->burstdata->freq) * 100.0;
		fitdat->scores["BurstIntraFreq"] = fitdat->burstintrafreq;
		//fitdat->score += fitdat->burstintrafreq * (*fitweights)["burstintrafreq"];

	    if(fitdiag) {
			ofp.WriteLine(text.Format("intra %.6f  exp %.6f  score %.6f", burstdata->freq, testdata->burstdata->freq, fitdat->burstintrafreq)); 
			diagbox->Write(text.Format("intra %.6f  exp %.6f  score %.6f\n", burstdata->freq, testdata->burstdata->freq, fitdat->burstintrafreq)); 
		}
		

		//MoF.Fitness = (Math.Abs(ExpBurst.MeanExtraBurstNoise - TgtBurst.MeanExtraBurstNoise) / TgtBurst.MeanExtraBurstNoise) * 100.0;


		// Burst Hazard   -- no known function
	}

	int IoDcount = 7;

	if(IoDmode) {
		IoDdata[0] = dispcalc(500);
		IoDdata[1] = dispcalc(1000);
		IoDdata[2] = dispcalc(2000);
		IoDdata[3] = dispcalc(4000);
		IoDdata[4] = dispcalc(8000);
		IoDdata[5] = dispcalc(6000);
		IoDdata[6] = dispcalc(10000);

		RMSError = 0;

		for(i=0; i<IoDcount; i++) {

			if(fitdiag) ofp.WriteLine(text.Format("IoD %d  Exp %.6f  Mod %.6f\n", i, testdata->IoDdata[i], IoDdata[i])); 

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
			if(fitdiag) ofp.WriteLine(text.Format("Error %.6f  RMSError %.6f\n", Error, RMSError)); 
		}

		fitdat->RMSIoD = RMSError / 100;
		fitdat->scores["RMSIoD"] = RMSError / 100;
		//fitdat->score += fitdat->RMSIoD * (*fitweights)["RMSIoD"];

		if(fitdiag) ofp.WriteLine(text.Format("IoD %.6f\n", fitdat->RMSIoD)); 
	}


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


	//// Sum weights

	//fitdat->weightsum += (*fitweights)["RMSFirstNBins"];
	//fitdat->weightsum += (*fitweights)["RMSBinsRange"];
	//fitdat->weightsum += (*fitweights)["RMSHaz"];
	//fitdat->weightsum += (*fitweights)["RMSIoD"];

	//fitdat->weightsum += (*fitweights)["RMSFirstNBinsBurst"];
	//fitdat->weightsum += (*fitweights)["burstmode"];
	//fitdat->weightsum += (*fitweights)["burstlengthmean"];
	//fitdat->weightsum += (*fitweights)["burstlengthsd"];
	//fitdat->weightsum += (*fitweights)["burstsilencemean"];
	//fitdat->weightsum += (*fitweights)["burstsilencesd"];
	//fitdat->weightsum += (*fitweights)["burstintrafreq"];

	//fitdat->score = fitdat->score / fitdat->weightsum;

	delete fitparams;

	if(fitdiag) ofp.Close();

	//diagbox->Write("OK\n\n");
}



//fitweights = new ParamStore();
	//(*fitweights)["ISImode"] = 5;
	//(*fitweights)["ISImean"] = 5;
	//(*fitweights)["RMSFirstNBins"] = 100;
	//(*fitweights)["RMSBinRange"] = 100;
	//(*fitweights)["RMSHaz"] = 60;
	//(*fitweights)["RMSIoD"] = 20;

	//// Burst fit

	//if(burstmode) {
	//	(*fitweights)["RMSFirstNBinsBurst"] = 30;
	//	(*fitweights)["burstmode"] = 20;
	//	(*fitweights)["burstlengthmean"] = 30;
	//	(*fitweights)["burstlengthsd"] = 5;
	//	(*fitweights)["burstsilencemean"] = 15;
	//	(*fitweights)["burstsilencesd"] = 5;
	//	(*fitweights)["burstintrafreq"] = 20;
	//}
	//else {																			// Non-Burst fit
	//	(*fitweights)["RMSFirstNBinsBurst"] = 0;			
	//	(*fitweights)["burstmode"] = 0;
	//	(*fitweights)["burstlengthmean"] = 0;
	//	(*fitweights)["burstlengthsd"] = 0;
	//	(*fitweights)["burstsilencemean"] = 0;
	//	(*fitweights)["burstsilencesd"] = 0;
	//	(*fitweights)["burstintrafreq"] = 0;
	//}


void SpikeDat::FitScoreBasic(SpikeDat *testdata, FitDat *fitdat, FitSet *fitset, FitConSet *conset)
{
	int i;
	int histmax = 512;
	double maxcount;
	wxString text;
	ParamStore *fitweights, *fitcon;
	TextFile ofp;
	bool burstmode = false;
	bool IoDmode = true;

	bool fitdiag = false;

	if(fitdiag) ofp.New("fitscore-diag.txt");


	//fitweights = new ParamStore();
	//(*fitweights)["ISImode"] = 5;
	//(*fitweights)["ISImean"] = 5;
	//(*fitweights)["RMSFirstNBins"] = 100;
	//(*fitweights)["RMSBinRange"] = 100;
	//(*fitweights)["RMSHaz"] = 60;
	//(*fitweights)["RMSIoD"] = 20;

	//// Burst fit

	//if(burstmode) {
	//	(*fitweights)["RMSFirstNBinsBurst"] = 30;
	//	(*fitweights)["burstmode"] = 20;
	//	(*fitweights)["burstlengthmean"] = 30;
	//	(*fitweights)["burstlengthsd"] = 5;
	//	(*fitweights)["burstsilencemean"] = 15;
	//	(*fitweights)["burstsilencesd"] = 5;
	//	(*fitweights)["burstintrafreq"] = 20;
	//}
	//else {																			// Non-Burst fit
	//	(*fitweights)["RMSFirstNBinsBurst"] = 0;			
	//	(*fitweights)["burstmode"] = 0;
	//	(*fitweights)["burstlengthmean"] = 0;
	//	(*fitweights)["burstlengthsd"] = 0;
	//	(*fitweights)["burstsilencemean"] = 0;
	//	(*fitweights)["burstsilencesd"] = 0;
	//	(*fitweights)["burstintrafreq"] = 0;
	//}


	fitcon = new ParamStore();
	//(*fitcon)["RMSFirstNBins"] = 30;
	//(*fitcon)["RMSBinRangeStart"] = 30;
	//(*fitcon)["RMSBinRangeFinish"] = 125;
	(*fitcon)["HazMin"];
	(*fitcon)["RMSFirstNBinsBurst"] = 20;

	//(*fitcon)["RMSFirstNBins"] = conset->GetCon("RMSFirstNBins").value;
	(*fitcon)["RMSHeadStart"] = conset->GetCon("RMSHeadStart").value;
	(*fitcon)["RMSHeadStop"] = conset->GetCon("RMSHeadStop").value;
	(*fitcon)["RMSBinRangeStart"] = conset->GetCon("RMSBinRangeStart").value;
	(*fitcon)["RMSBinRangeFinish"] = conset->GetCon("RMSBinRangeFinish").value;


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


	if(spikecount < 10) {
		fitdat->scores["RMSFirstNBins"] = -1;
		fitdat->scores["RMSBinRange"] = -1;
		fitdat->scores["RMSHaz"] = -1; 
		fitdat->scores["RMSIoD"] = -1;
		fitdat->score = 1000;
		return;
	}


	// Spike Data Processing //////////////////////////////////////////////////////////////////  Duplicate of neurocalc processing for GPU generated data

	double initbinwidth = 1;
	double bininc = 0.05;
	double width, histsum;
	double hazremain, hazmin;


	// Mode for fitness
	maxcount = 0;
	for(i=0; i<histmax; i++) {
		//if(i < 20) diagbox->Write(text.Format("all cell bin %d histquadsm %.8f\n", i, histquadsm[i]));
		if(histquadsm[i] > maxcount) {
			maxcount = histquadsm[i];
			histquadmode = i;
			//diagbox->Write(text.Format("new mode bin %d histquadsm %.4f\n", i, histquadsm[i]));
		}
	}

	//diagbox->Write(text.Format("histquadmode %.2f\n", histquadmode));

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
		hazquadfit[i] = histquadlin[i] / hazremain;
		if(hazremain < hazmin) {
			hazquadfit[i] = 0;
			hazquadbins = i;
			break;
		}
		hazremain -= histquadlin[i];
	}


	/*
	// Burst Mode
	int pmode = 0;
	burstdata->pnzcount = 0;
	for(i=0; i<histmax; i++) {
	if(burstdata->profilesm[i] > burstdata->profilesm[pmode]) pmode = i;
	if(burstdata->profilesm[i] > 0) burstdata->pnzcount = i;
	}
	burstdata->pmodetime = pmode;
	burstdata->pmoderate = burstdata->profilesm[pmode];*/


	////////////////////////////////////////////////////////////////////////////////////


	// Tom Fit Re-engineering

	//fitdat->score = 0;
	//fitdat->weightsum = 0;

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


	diagbox->Write(text.Format("\nRMSHead Start %.0f Stop %.0f\n", (*fitcon)["RMSHeadStart"], (*fitcon)["RMSHeadStop"]));

	RMSError = 0;
	//for(i=0; i<(*fitcon)["RMSFirstNBins"]; i++) {
	for(i=(*fitcon)["RMSHeadStart"]; i<(*fitcon)["RMSHeadStop"]; i++) {
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

		if (Big < CutOff) Error = (Big - Small) * 100.0;                     // Fix brackets 20/9/16
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	diagbox->Write(text.Format("\nRMSHead Error Sum %.4f\n", RMSError));

	//RMSError /= (*fitcon)["RMSFirstNBins"];
	RMSError /= ((*fitcon)["RMSHeadStop"] - (*fitcon)["RMSHeadStart"]);
	RMSError = sqrt(RMSError);

	diagbox->Write(text.Format("\nRMSHead Error RMS %.4f\n", RMSError));

	fitdat->RMSFirstNBins = RMSError;
	fitdat->scores["RMSFirstNBins"] = RMSError;
	//fitdat->score += fitdat->RMSFirstNBins * (*fitweights)["RMSFirstNBins"];


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
		if (Big < CutOff) Error = (Big - Small) * 100.0;
		else Error = (Big - Small) / Big * 100.0;
		RMSError += Error * Error;
	}

	RMSError /= ((*fitcon)["RMSBinRangeFinish"] - (*fitcon)["RMSBinRangeStart"]);
	RMSError = sqrt(RMSError);

	fitdat->RMSBinRange = RMSError;
	fitdat->scores["RMSBinRange"] = RMSError;
	//fitdat->score += fitdat->RMSBinRange * (*fitweights)["RMSBinRange"];


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
		if(testdata->hazquadfit[i] > hazquadfit[i]) {
			Big = testdata->hazquadfit[i];
			Small = hazquadfit[i];
		}
		else {
			Big = hazquadfit[i];
			Small = testdata->hazquadfit[i];
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
	//fitdat->score += fitdat->RMSHaz * (*fitweights)["RMSHaz"];

	if(fitdiag) ofp.WriteLine(text.Format("Haz RMS RMSRrror %.6f Pre SQRT %.6f", RMSError, errcalc)); 


	//// Burst Profile Analysis

	if(burstmode) {

		// Burst Mode
		int pmode = 0;
		burstdata->pnzcount = 0;
		for(i=0; i<histmax; i++) {
			if(burstdata->profilesm[i] > burstdata->profilesm[pmode]) pmode = i;
			if(burstdata->profilesm[i] > 0) burstdata->pnzcount = i;
		}
		burstdata->pmodetime = pmode;
		burstdata->pmoderate = burstdata->profilesm[pmode];

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

		fitdat->RMSBurstHead = RMSError;
		fitdat->score += fitdat->RMSBurstHead * (*fitweights)["RMSBurstHead"];   // fix this for new weights scheme


		// Burst Mode

		fitdat->burstmode = (abs(testdata->burstdata->pmoderate - burstdata->pmoderate) / testdata->burstdata->pmoderate) * 50.0;
		fitdat->burstmode += (abs(testdata->burstdata->pmodetime - burstdata->pmodetime) / testdata->burstdata->pmodetime) * 50.0;
		fitdat->score += fitdat->burstmode * (*fitweights)["burstmode"];

		// Burst Stats

		fitdat->burstlengthmean = (abs(testdata->burstdata->meanlength - burstdata->meanlength) / testdata->burstdata->meanlength) * 100.0;
		fitdat->score += fitdat->burstlengthmean * (*fitweights)["burstlengthmean"];

		if(fitdiag) ofp.WriteLine(text.Format("burst mod %.6f  exp %.6f  score %.6f", burstdata->meanlength, testdata->burstdata->meanlength, fitdat->burstlengthmean)); 

		fitdat->burstlengthsd = (abs(testdata->burstdata->sdlength - burstdata->sdlength) / testdata->burstdata->sdlength) * 100.0;
		fitdat->score += fitdat->burstlengthsd * (*fitweights)["burstlengthsd"];

		if(fitdiag) ofp.WriteLine(text.Format("burstsd mod %.6f  exp %.6f  score %.6f", burstdata->sdlength, testdata->burstdata->sdlength, fitdat->burstlengthsd)); 

		fitdat->burstsilencemean = (abs(testdata->burstdata->meansilence - burstdata->meansilence) / testdata->burstdata->meansilence) * 100.0;
		fitdat->score += fitdat->burstsilencemean * (*fitweights)["burstsilencemean"];

		if(fitdiag) ofp.WriteLine(text.Format("silence mod %.6f  exp %.6f  score %.6f", burstdata->meansilence, testdata->burstdata->meansilence, fitdat->burstsilencemean)); 

		fitdat->burstsilencesd = (abs(testdata->burstdata->sdsilence - burstdata->sdsilence) / testdata->burstdata->sdsilence) * 100.0;
		fitdat->score += fitdat->burstsilencesd * (*fitweights)["burstsilencesd"];

		fitdat->burstintrafreq = (abs(testdata->burstdata->freq - burstdata->freq) / testdata->burstdata->freq) * 100.0;
		fitdat->score += fitdat->burstintrafreq * (*fitweights)["burstintrafreq"];

		if(fitdiag) ofp.WriteLine(text.Format("intra %.6f  exp %.6f  score %.6f", burstdata->meansilence, testdata->burstdata->meansilence, fitdat->burstsilencemean)); 

		//MoF.Fitness = (Math.Abs(ExpBurst.MeanExtraBurstNoise - TgtBurst.MeanExtraBurstNoise) / TgtBurst.MeanExtraBurstNoise) * 100.0;


		// Burst Hazard   -- no known function
	}

	int IoDcount = 7;

	if(IoDmode) {
		IoDdata[0] = dispcalc(500);
		IoDdata[1] = dispcalc(1000);
		IoDdata[2] = dispcalc(2000);
		IoDdata[3] = dispcalc(4000);
		IoDdata[4] = dispcalc(8000);
		IoDdata[5] = dispcalc(6000);
		IoDdata[6] = dispcalc(10000);

		RMSError = 0;

		for(i=0; i<IoDcount; i++) {

			if(fitdiag) ofp.WriteLine(text.Format("IoD %d  Exp %.6f  Mod %.6f\n", i, testdata->IoDdata[i], IoDdata[i])); 

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
			if(fitdiag) ofp.WriteLine(text.Format("Error %.6f  RMSError %.6f\n", Error, RMSError)); 
		}

		fitdat->RMSIoD = RMSError / 100;
		fitdat->scores["RMSIoD"] = RMSError / 100;
		//fitdat->score += fitdat->RMSIoD * (*fitweights)["RMSIoD"];

		if(fitdiag) ofp.WriteLine(text.Format("IoD %.6f\n", fitdat->RMSIoD)); 
	}


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


	//// Sum weights

	//fitdat->weightsum += (*fitweights)["RMSFirstNBins"];
	//fitdat->weightsum += (*fitweights)["RMSBinsRange"];
	//fitdat->weightsum += (*fitweights)["RMSHaz"];
	//fitdat->weightsum += (*fitweights)["RMSIoD"];

	//fitdat->weightsum += (*fitweights)["RMSFirstNBinsBurst"];
	//fitdat->weightsum += (*fitweights)["burstmode"];
	//fitdat->weightsum += (*fitweights)["burstlengthmean"];
	//fitdat->weightsum += (*fitweights)["burstlengthsd"];
	//fitdat->weightsum += (*fitweights)["burstsilencemean"];
	//fitdat->weightsum += (*fitweights)["burstsilencesd"];
	//fitdat->weightsum += (*fitweights)["burstintrafreq"];

	//fitdat->score = fitdat->score / fitdat->weightsum;

	delete fitcon;

	if(fitdiag) ofp.Close();
}