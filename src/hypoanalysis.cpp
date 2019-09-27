

//#include "hypomodel.h"
#include <hypodat.h>
#include <hypopanels.h>



void SpikeDat::BurstProfile()
{
	int i, j, burst;
	int start, end;
	double stime;
	
	int protime, oldtime;
	FILE *ofp;
	int tailspikes = 50;
	wxString text;

	bool diagnostic = false;
	bool tailmode = false;


	if(diagnostic) {
		ofp = fopen("bprofile.txt", "w");
		diagbox->Write("BurstProfile call " + label + "\n");
	}

	stime = 0;
	oldtime = 0;
	for(i=0; i<500; i++) {
		prosum[i] = 0;
		procount[i] = 0;
		prosumtail[i] = 0;
		procounttail[i] = 0;
	}

	for(burst=1; burst<=burstdata->numbursts; burst++) {
		start = burstdata->bustore[burst].start;
		end = burstdata->bustore[burst].end;

		// Head based profile
		i = start;
		stime = 0;
		oldtime = 0;
		procount[0]++;
		if(diagnostic) fprintf(ofp, "\nburst %d  start %d  end %d\n", burst, start, end);
		
		while(i <= end && stime < 500) {
			stime = (times[i] - times[start]) / 1000; 
			if(diagnostic) fprintf(ofp, "burst %d  spike %d  stime %d\n", burst, i, stime); 
			protime = (int)stime;
			if(protime > oldtime) {
				procount[protime]++;
				oldtime = protime;
			}
			prosum[protime]++;
			i++;
		}
		
		// Tail based profile
		if(tailmode) {
			i = end;
			stime = 0;
			oldtime = 0;
			procounttail[0]++;
			while(i >= start && stime < 500) {
				stime = (times[end] - times[i]) / 1000; 
				//if(diagnostic) fprintf(ofp, "tail burst %d  stime %.2f\n", burst, stime); 
				protime = (int)stime;
				if(protime > oldtime) {
					procounttail[protime]++;
					oldtime = protime;
				}
				prosumtail[protime]++;
				i--;
			}
		}
		
	}

	for(i=0; i<500; i++) {
		if(procount[i] > 0) burstdata->profile[i+1] = (double)prosum[i] / procount[i];
		else burstdata->profile[i] = 0;
		if(procounttail[i] > 0) burstdata->tailprofile[i+1] = (double)prosumtail[i] / procounttail[i];
		else burstdata->tailprofile[i] = 0;
		if(diagnostic) fprintf(ofp, "btime %d  rate sum %.2f  count %d  mean %.2f\n", i, burstdata->profile[i], procount[i], burstdata->profile[i]);
		burstdata->bursthaz[i] = 100 * procount[i] / burstdata->numbursts;  
	}
	
	
	// Profile Smoothing for Fitting      December 2012

	int profmax = 500;
	int wincount;
	int bscount = 0;

	for(i=0; i<profmax; i++) {
		burstdata->profilesm[i] = 0;
		wincount = 0;
		for(j=i-2; j<i+3; j++) {
			if ((j > -1) & (j < profmax)) {
				burstdata->profilesm[i] += burstdata->profile[j];
				wincount++;
			}
		}
		burstdata->profilesm[i] /= wincount;
		bscount += burstdata->profilesm[i];
	}


	// find mode
	int pmode = 0;
	burstdata->pnzcount = 0;
	for(i=0; i<profmax; i++) {
		if(burstdata->profilesm[i] > burstdata->profilesm[pmode]) pmode = i;
		if(burstdata->profilesm[i] > 0) burstdata->pnzcount = i;
	}
	burstdata->pmodetime = pmode;
	burstdata->pmoderate = burstdata->profilesm[pmode];

	if(diagnostic) diagbox->Write(text.Format("modetime %.2f  moderate %.2f\n\n", burstdata->pmodetime, burstdata->pmoderate));
	

	// Burst tail analysis  (Nancy, 2006 paper)

	/*
	double tailsum, varsum;
	double logint[50000];
	//double tailmean[1000];
	//double tailstdev[1000];
	//double tailstder[1000];
	//int tailnum[1000];
	double loginterv, tpoint;
	int burstnum;

	for(i=0; i<1000; i++) {
		burstdata->tailnum[i] = 0;
		burstdata->tailmean[i] = 0;
		burstdata->tailstdev[i] = 0;
		burstdata->tailstder[i] = 0;
	}

	for(j=0; j<tailspikes; j++) {
		tailsum = 0;
		varsum = 0;
		burstnum = burstdata->numbursts;
		for(i=1; i<=burstdata->numbursts; i++) {			
			if(burstdata->bustore[i].end - j <= burstdata->bustore[i].start) {
				burstnum--;
				loginterv = 0;
			}
			else loginterv = log10(isis[burstdata->bustore[i].end - j]/1000);
			logint[i] = loginterv;
			varsum = varsum + loginterv * loginterv;
			tailsum = tailsum + loginterv;
			tpoint = times[burstdata->bustore[i].end - j+1];
			fprintf(ofp, "Spike  %d  Time %.2f  Burst Index %d  log interv %.4f Count %d\n",
				i, tpoint, j+1, loginterv, burstdata->bustore[i].count);
		}
		tailsum = tailsum / burstnum;
		varsum = (varsum - burstnum * tailsum * tailsum) / (burstnum - 1);
		burstdata->tailstdev[j+1] = sqrt(varsum);
		burstdata->tailstder[j+1] = burstdata->tailstdev[j+1] / sqrt((double)(burstnum - 1));
		burstdata->tailnum[j+1] = burstnum;
		burstdata->tailmean[tailspikes - j+1] = -tailsum;
		//burstdata->tailmean[j+1] = -tailsum;

		fprintf(ofp, "\nMean %.4f  Stdev %.4f  Bursts %d\n\n", tailsum, burstdata->tailstdev[j+1], burstnum);
		//burstdata->tailspikesum[j] = tailsum;
	}
	*/
	if(diagnostic) fclose(ofp);
}


/*
spikestep = 0;
srate.max = (int)(times[spikecount-1] + 0.5); 
for(i=0; i<=times[spikecount-1]; i++) {	     // spike rate count (1s)
fprintf(ofp, "%ds. ", i);
if(spikestep > spikecount) {
fprintf(ofp, "break spikestep %d  spikecount %d\n", spikestep, spikecount);
break;
}
while(times[spikestep] < i+1) {
fprintf(ofp, "spike %d  ", spikestep);
srate.data[i]++;
spikestep++;
if(spikestep >= spikecount) break;
}
fprintf(ofp, "srate %d\n", srate.data[i]);
}*/


/*
void SpikeDat::Clear()
{
int i;


}
*/


void BurstDat::FitAnalysis()
{
	int i;
}


void BurstDat::IntraBurstAnalysis()
{
	int i;
	//int numspikes, 
	int intracount;
	int bindex;
	bool scandiag;

	TextFile outfile;
	wxString filename, filetag, text;

	scandiag = false;

	if(scandiag) outfile.New("intradat.txt");

	//spikedata->diagbox->Write("Intra Burst Analysis\n");

	// Intraburst Re-Analysis  

	int hazdex = 0;
	int binsize = 5;
	int intcount = 0;
	double mean = 0;
	double variance = 0;
	double inttime = 0;
	bindex = 1;

	for(i=0; i<10000; i++) {
		hist1[i] = 0;
		hist5[i] = 0;
		hist1norm[i] = 0;
		hist5norm[i] = 0;
		haz1[i] = 0;
		haz5[i] = 0;
		//haz5norm[i] = 0;
	}

	//if(diagbox) diagbox->Write(text.Format("selectmode %d\n", burstdata->selectmode));

	for(i=0; i<spikedata->spikecount-1; i++) {
		if(spikes[i] > 0 && spikedata->isis[i] < spikedata->maxspikes) {
			if(!selectmode && i == bustore[bindex].end) {
				//fprintf(ofp, "New burst %d, time total %.2f\n", bindex, inttime);
				bindex++;
				if(scandiag) outfile.WriteLine(text.Format("i %d burstend", i));
			}
			else {
				intcount++;
				if(hist1.max < (int)spikedata->isis[i]) hist1.max = (int)spikedata->isis[i];
				if(hist1.data.size() < hist1.max + 1) hist1.data.resize(hist1.max + 1);
				hist1[(int)spikedata->isis[i]]++;	
				inttime = inttime + spikedata->isis[i];
				//if(spikedata->isis[i] > maxint) fprintf(ofp, "burst %d index %d isi %.2f total %.2f\n", 
				//	bindex-1, i, spikedata->isis[i], inttime);  
				mean = mean + spikedata->isis[i];
				if(scandiag) outfile.WriteLine(text.Format("i %d isi %.4f", i, spikedata->isis[i]));
				variance = variance + spikedata->isis[i] * spikedata->isis[i];
			}
		}
	}

	//mean = mean / (scount - burstdata->numbursts);
	//variance = variance / (scount - burstdata->numbursts);
	mean = mean / intcount;
	variance = variance / intcount;

	intraspikes = intcount + numbursts;
	if(intcount > 0) {
		isisd = sqrt(variance - mean * mean);
		freq = 1000/mean;
		meanisi = mean;
	}
	else {
		isisd = 0;
		freq = 0;
		meanisi = 0;
	}

	burstdisp = 1;
	maxtime = times[spikedata->spikecount-1];

	if(scandiag) for(i=0; i<10; i++) outfile.WriteLine(text.Format("spike %d  Burst time %.2f\n", i, times[i]));


	// Hazard

	double hazcount = 0;
	//double haznorm = 10/freq;
	haz1.max = hist1.max;
	if(haz1.data.size() < haz1.max + 1) haz1.data.resize(haz1.max + 1);

	for(i=0; i<=hist1.max; i++) {
		haz1.data[i] = hist1.data[i] / (intraspikes - hazcount); 	
		hazcount = hazcount + hist1.data[i];
	}

	for(i=0; i<=hist1.max; i++) {
		//haz5[i/binsize] = haz5[i/binsize] + haz[i] * 100;	          // Nancy sheet
		if(i/binsize > haz5.max) haz5.max = i/binsize;
		if(haz5.data.size() < haz5.max + 1)	haz5.data.resize(haz5.max + 1);
		haz5.data[i/binsize] = haz5.data[i/binsize] + haz1.data[i]; // * haznorm;	
	}

	for(i=0; i<=hist1.max; i++) {
		//hist[i] = hist[i] / norm;
		if(i/binsize > hist5.max) hist5.max = i/binsize;
		if(hist5.data.size() < hist5.max + 1) hist5.data.resize(hist5.max + 1);
		hist5.data[i/binsize] = hist5.data[i/binsize] + hist1.data[i];		
	}

	// Normalised histograms
	for(i=0; i<=hist1.max; i++) {
		hist1norm[i] = spikedata->normscale * hist1[i] / intcount;
		hist5norm[i] = spikedata->normscale * hist5[i] / intcount;
	}


	// Index of Dispersion

	IoDrange();

	/*
	IoDdata[0] = IoDcalc(500, intraspikes, &times);
	IoDdata[1] = IoDcalc(1000, intraspikes, &times);
	IoDdata[2] = IoDcalc(2000, intraspikes, &times);
	IoDdata[3] = IoDcalc(4000, intraspikes, &times);
	IoDdata[4] = IoDcalc(6000, intraspikes, &times);
	IoDdata[5] = IoDcalc(8000, intraspikes, &times);
	IoDdata[6] = IoDcalc(10000, intraspikes, &times);

	IoDdataX[0] = 5;            // x positions for displaying IoD range as bar chart
	IoDdataX[1] = 15;
	IoDdataX[2] = 25;
	IoDdataX[3] = 35;
	IoDdataX[4] = 45;
	IoDdataX[5] = 55;
	IoDdataX[6] = 65;*/

	if(scandiag) outfile.Close();
}


void SpikeDat::IntraSelectAnalysis()
{
	int i;
	//int numspikes, 
	int intracount;
	int bindex;
	bool scandiag;

	TextFile outfile;
	wxString filename, filetag, text;

	scandiag = false;

	if(scandiag) outfile.New("intradat.txt");

	// Intraburst Re-Analysis  

	int hazdex = 0;
	int binsize = 5;
	int intcount = 0;
	double mean = 0;
	double variance = 0;
	double inttime = 0;
	bindex = 1;

	for(i=0; i<10000; i++) {
		burstdata->hist1[i] = 0;
		burstdata->hist5[i] = 0;
		burstdata->haz1[i] = 0;
		burstdata->haz5[i] = 0;
	}

	for(i=1; i<spikecount; i++) {
		if(burstdata->spikes[i] > 0 && isis[i] < maxspikes) {
			if(i == burstdata->bustore[bindex].start) {
				//fprintf(ofp, "New burst %d, time total %.2f\n", bindex, inttime);
				bindex++;
				if(scandiag) outfile.WriteLine(text.Format("i %d burststart", i));
			}
			else {
				intcount++;
				if(burstdata->hist1.max < (int)isis[i]) burstdata->hist1.max = (int)isis[i];
				if(burstdata->hist1.data.size() < burstdata->hist1.max + 1) burstdata->hist1.data.resize(burstdata->hist1.max + 1);
				burstdata->hist1[(int)isis[i]]++;	
				inttime = inttime + isis[i];
				//if(spikedata->isis[i] > maxint) fprintf(ofp, "burst %d index %d isi %.2f total %.2f\n", 
				//	bindex-1, i, spikedata->isis[i], inttime);  
				mean = mean + (double)(isis[i]);
				if(scandiag) outfile.WriteLine(text.Format("i %d isi %.4f", i, isis[i]));
				variance = variance + isis[i] * isis[i];
			}
		}
	}

	//mean = mean / (scount - burstdata->numbursts);
	//variance = variance / (scount - burstdata->numbursts);
	mean = mean / intcount;
	variance = variance / intcount;

	burstdata->intraspikes = intcount + 1;
	if(intcount > 0) {
		burstdata->isisd = sqrt(variance - mean * mean);
		burstdata->freq = 1000/mean;
		burstdata->meanisi = mean;
	}
	else {
		burstdata->isisd = 0;
		burstdata->freq = 0;
		burstdata->meanisi = 0;
	}

	//burstdata->intraspikes = intracount;
	//burstdata->intraspikes = scount;
	//burstdata->isisd = sqrt(variance - mean * mean);
	//burstdata->freq = 1000/mean;
	//burstdata->meanisi = mean;
	//fprintf(ofp, "\nIntra burst time total = %.2f\n", inttime);
	burstdata->burstdisp = 1;
	burstdata->times = times;
	burstdata->maxtime = times[spikecount-1];
	//burstbox->BurstDataDisp();
	//for(i=0; i<10; i++) fprintf(ofp, "Burst time %.2f\n", burstdata->times[i]);
	if(scandiag) for(i=0; i<10; i++) outfile.WriteLine(text.Format("spike %d  Burst time %.2f\n", i, burstdata->times[i]));
	//fprintf(ofp, "maxtime %.2f\n", burstdata->maxtime);
	//for(i=0; i<spikecount; i++) burstdata->times[i] = times[i];

	// Hazard

	double hazcount = 0;
	//double haznorm = 10/freq;
	burstdata->haz1.max = burstdata->hist1.max;
	if(burstdata->haz1.data.size() < burstdata->haz1.max + 1) burstdata->haz1.data.resize(burstdata->haz1.max + 1);

	for(i=0; i<=burstdata->hist1.max; i++) {
		burstdata->haz1.data[i] = burstdata->hist1.data[i] / (burstdata->intraspikes - hazcount); 	
		hazcount = hazcount + burstdata->hist1.data[i];
	}

	for(i=0; i<=burstdata->hist1.max; i++) {
		//haz5[i/binsize] = haz5[i/binsize] + haz[i] * 100;	          // Nancy sheet
		if(i/binsize > burstdata->haz5.max) burstdata->haz5.max = i/binsize;
		if(burstdata->haz5.data.size() < burstdata->haz5.max + 1)	burstdata->haz5.data.resize(burstdata->haz5.max + 1);
		burstdata->haz5.data[i/binsize] = burstdata->haz5.data[i/binsize] + burstdata->haz1.data[i]; // * haznorm;	
	}

	for(i=0; i<=burstdata->hist1.max; i++) {
		//hist[i] = hist[i] / norm;
		if(i/binsize > burstdata->hist5.max) burstdata->hist5.max = i/binsize;
		if(burstdata->hist5.data.size() < burstdata->hist5.max + 1)	burstdata->hist5.data.resize(burstdata->hist5.max + 1);
		burstdata->hist5.data[i/binsize] = burstdata->hist5.data[i/binsize] + burstdata->hist1.data[i];		
	}

	if(scandiag) outfile.Close();
}


void SpikeDat::BurstScanFit(ParamStore *burstparams)
{
	int i, bstart;
	int numspikes;
	int burston, silenceon;
	int bcheck, bindex, bcount;
	int maxint, minspikes, maxspikes;
	int startspike, endspike;
	double isi;
	double tstart, tend;
	double stime, scheck;
	double btime, tpoint;
	double intracount, intratime, intravar;
	double burstfreq;
	double silencetime, silencevar;
	bool scandiag;

	// Parameter transfer
	//ParamStore *burstparams = burstbox->GetParams();
	maxint = (int)(*burstparams)["maxint"];
	minspikes = (int)(*burstparams)["minspikes"];
	maxspikes = (int)(*burstparams)["maxspikes"];
	startspike = (int)(*burstparams)["startspike"];
	endspike = (int)(*burstparams)["endspike"];

	// Initialise scan
	burston = 0;
	silenceon = 0;
	bcheck = 0;
	bindex = 0;
	bcount = 0;
	tpoint = 0;
	btime = 0;
	bstart = 0;

	numspikes = spikecount;


	// Burst Scan (Based on Nancy's algorithm)
	for(i=0; i<numspikes-1; i++) {

		//isi = isis[i]; 
		isi = times[i+1] - times[i];
		tpoint = times[i];

		if(tpoint < startspike) continue;

		if(endspike && tpoint > endspike) continue;

		if(!burston) {
			if(isi < maxint)
				bstart = i;
			btime = 0;
			bcount = 1;
			burston = 1;
			bcheck = 0;
		}

		if(burston) {
			if(isi > maxint) {
				burston = 0;
				if(bcheck) {
					if(!maxspikes || bcount <= maxspikes) { 
						bindex++;		
						burstdata->bustore[bindex].start = bstart;
						burstdata->bustore[bindex].end = i;
						burstdata->bustore[bindex].count = bcount;
						burstdata->bustore[bindex].time = btime;					
						tstart = times[bstart];
						tend = times[i];
					}
					bcheck = 0;
					silenceon = 1;
					stime = isi;
					scheck = 0;			                                 
				}
			}
			if(isi <= maxint) {
				btime = btime + isi;
				bcount++;
				if(!bcheck && bcount >= minspikes) bcheck = 1;
			}
		}
	}
	burstdata->numbursts = bindex;


	// Burst Analysis
	if(burstdata->numbursts == 0) {
		burstdata->intraspikes = 0;
		burstdata->isisd = 0;
		burstdata->freq = 0;
		burstdata->meanisi = 0;
		burstdata->meancount = 0;
		burstdata->meantime = 0;
		burstdata->meanlength = 0;
		burstdata->meansilence = 0;
		burstdata->sdlength = 0;
		burstdata->sdsilence = 0;
		burstdata->burstdisp = 0;
		burstdata->maxint = maxint;
		burstdata->IoDrange();
		return;
	}

	bindex = 1;
	int silence = 0;

	for(i=0; i<numspikes; i++) burstdata->spikes[i] = 0;

	for(i=0; i<numspikes; i++) {
		if(i == burstdata->bustore[bindex].start) silence = 1;
		burstdata->spikes[i] = silence * bindex;
		if(i == burstdata->bustore[bindex].end) {
			silence = 0;
			bindex++;
		}
		if(bindex > burstdata->numbursts) break;   
	}

	// Mean burst spikes and time
	intracount = 0;
	intratime = 0;
	silencetime = 0;
	intravar = 0;
	silencevar = 0;
	for(i=1; i<=burstdata->numbursts; i++) {
		intracount = intracount + burstdata->bustore[i].count;
		intratime = intratime + burstdata->bustore[i].time;
		if(i < burstdata->numbursts) {
			silencetime += (times[burstdata->bustore[i+1].start] - times[burstdata->bustore[i].end]) / 1000;
		}
	}
	burstdata->meancount = (double)intracount / burstdata->numbursts;
	burstdata->meantime = (double)intratime / burstdata->numbursts;
	burstdata->meanlength = burstdata->meantime / 1000;
	burstdata->meansilence = (double)silencetime / burstdata->numbursts-1;
	burstdata->freq = intracount / (intratime / 1000);
	burstdata->actQ = intratime / times[spikecount-1];   // looks odd, correct?  17/9/19
	if(burstdata->numbursts == 0) burstdata->actQ = 1;

	for(i=1; i<burstdata->numbursts; i++) {
		intravar += pow((burstdata->bustore[i].time - burstdata->meantime) / 1000, 2);
		if(i < burstdata->numbursts) {
			silencetime = (times[burstdata->bustore[i+1].start] - times[burstdata->bustore[i].end]) / 1000;
			silencevar += pow(silencetime - burstdata->meansilence, 2);
		}
	}

	intravar = intravar / burstdata->numbursts;
	silencevar = silencevar / burstdata->numbursts-1;
	burstdata->sdlength = sqrt(intravar);
	burstdata->sdsilence = sqrt(silencevar);

	burstdata->times = times;

	burstdata->maxint = maxint;
	burstdata->IoDrange();

	// Index of Dispersion

	//burstdata->maxint = maxint;
	//burstdata->IoDrange();
}


void SpikeDat::BurstScan(BurstBox *burstbox)
{
	int i, bstart;
	int numspikes;
	int burston, silenceon;
	int bcheck, bindex, bcount;
	int maxint, minspikes, maxspikes;
	int startspike, endspike;
	double isi;
	double tstart, tend;
	double stime, scheck;
	double btime, tpoint;
	double intracount, intratime, intravar;
	double burstfreq;
	double silencetime, silencevar;
	bool scandiag;

	scandiag = false;

	FILE *ofp = NULL;

	// Parameter transfer
	ParamStore *burstparams = burstbox->GetParams();
	maxint = (int)(*burstparams)["maxint"];
	minspikes = (int)(*burstparams)["minspikes"];
	maxspikes = (int)(*burstparams)["maxspikes"];
	startspike = (int)(*burstparams)["startspike"];
	endspike = (int)(*burstparams)["endspike"];

	// Initialise scan
	burston = 0;
	silenceon = 0;
	bcheck = 0;
	bindex = 0;
	bcount = 0;
	tpoint = 0;
	btime = 0;
	bstart = 0;

	numspikes = spikecount;

	if(numspikes == 0) scandiag = false;

	if(scandiag) {
		ofp = fopen("burstscan.txt", "w");
		fprintf(ofp, "scan data %d spikes\n", numspikes);
		fprintf(ofp, "maxint %d  minspikes %d   maxspikes %d  startspike %d   endspike %d\n\n",
			maxint, minspikes, maxspikes, startspike, endspike);
	}


	// Burst Scan (Based on Nancy's algorithm)
	for(i=0; i<numspikes-1; i++) {

		isi = isis[i];        
		tpoint = times[i];

		if(tpoint < startspike) continue;

		if(endspike && tpoint > endspike) continue;

		if(!burston) {
			if(isi < maxint)
				bstart = i;
			btime = 0;
			bcount = 1;
			burston = 1;
			bcheck = 0;
		}

		if(burston) {
			if(isi > maxint) {
				burston = 0;
				if(bcheck) {
					if(!maxspikes || bcount <= maxspikes) { 
						bindex++;		
						burstdata->bustore[bindex].start = bstart;
						burstdata->bustore[bindex].end = i;
						burstdata->bustore[bindex].count = bcount;
						burstdata->bustore[bindex].time = btime;					
						tstart = times[bstart];
						tend = times[i];
						if(scandiag) {
							fprintf(ofp, "\nBurst %d  start %.2f end %.2f count %d time %.2f\n", 
								bindex, tstart, tend, bcount, btime);
							fprintf(ofp, "Burst %d  start sp %d end sp %d count %d\n", 
								bindex, bstart, i, bcount); 
						}
					}
					bcheck = 0;
					silenceon = 1;
					stime = isi;
					scheck = 0;			                                 
				}
			}
			if(isi <= maxint) {
				btime = btime + isi;
				bcount++;
				if(!bcheck && bcount >= minspikes) bcheck = 1;
			}
		}

		//fprintf(ofp, "%d %.2f %.2f B %d  S %d  BC %d  Btime %.2f Bcount %d   ", i, tpoint, isi, burston, silenceon, bcheck, btime, bcount);
		//if(isi > 1000) fprintf(ofp, "ISI");
		//fprintf(ofp, "\n");
	}
	burstdata->numbursts = bindex;



	// Burst Analysis

	if(burstdata->numbursts == 0) {
		burstdata->intraspikes = 0;
		burstdata->isisd = 0;
		burstdata->freq = 0;
		burstdata->meanisi = 0;
		burstdata->meancount = 0;
		burstdata->meantime = 0;
		burstdata->meanlength = 0;
		burstdata->burstdisp = 0;
		//burstbox->BurstDataDisp();
		if(scandiag) fclose(ofp);
		return;
	}

	bindex = 1;
	int silence = 0;

	for(i=0; i<numspikes; i++) burstdata->spikes[i] = 0;

	for(i=0; i<numspikes; i++) {
		if(i == burstdata->bustore[bindex].start) silence = 1;
		burstdata->spikes[i] = silence * bindex;
		if(i == burstdata->bustore[bindex].end) {
			silence = 0;
			bindex++;
		}
		if(bindex > burstdata->numbursts) break;   
		if(scandiag) fprintf(ofp, "Spike %d  Burst ID  %d\n", i, burstdata->spikes[i]);
	}

	// Mean burst spikes and time
	intracount = 0;
	intratime = 0;
	silencetime = 0;
	intravar = 0;
	silencevar = 0;
	for(i=1; i<=burstdata->numbursts; i++) {
		intracount = intracount + burstdata->bustore[i].count;
		intratime = intratime + burstdata->bustore[i].time;
		//intravar += pow((burstdata->bustore[i].time - burstdata->meantime) / 1000, 2);          // OMG Idiot!
		if(i < burstdata->numbursts) {
			silencetime += (times[burstdata->bustore[i+1].start] - times[burstdata->bustore[i].end]) / 1000;
			//silencevar += pow(silencetime - burstdata->meansilence, 2);                                       // Ditto!
		}
		if(scandiag) fprintf(ofp, "Burst %d Silence %.2f\n", i, times[burstdata->bustore[i+1].start] - times[burstdata->bustore[i].end]);
	}
	burstdata->meancount = (double)intracount / burstdata->numbursts;
	burstdata->meantime = (double)intratime / burstdata->numbursts;
	burstdata->meanlength = burstdata->meantime / 1000;
	burstdata->meansilence = (double)silencetime / burstdata->numbursts-1;
	burstfreq = intracount / (intratime / 1000);
	burstdata->actQ = intratime / times[spikecount-1];  
	if(burstdata->numbursts == 0) burstdata->actQ = 1;

	if(scandiag) fprintf(ofp, "\ntotal time %.2f  burst time %.2f\n\n", intratime, times[spikecount-1]);


	for(i=1; i<burstdata->numbursts; i++) {
		intravar += pow((burstdata->bustore[i].time - burstdata->meantime) / 1000, 2);
		if(i < burstdata->numbursts) {
			silencetime = (times[burstdata->bustore[i+1].start] - times[burstdata->bustore[i].end]) / 1000;
			silencevar += pow(silencetime - burstdata->meansilence, 2);
			if(scandiag) fprintf(ofp, "Burst %d  Silence Var %.2f  Square %.2f\n", i, (silencetime - burstdata->meansilence), pow(silencetime - burstdata->meansilence, 2));
		}
	}

	intravar = intravar / burstdata->numbursts;
	silencevar = silencevar / burstdata->numbursts-1;
	burstdata->sdlength = sqrt(intravar);
	burstdata->sdsilence = sqrt(silencevar);

	if(scandiag) {
		fprintf(ofp, "\n\n");
		fprintf(ofp, "Total burst spikes : %.0f\n", intracount);
		fprintf(ofp, "Total burst time : %.2f ms\n", intratime);
		fprintf(ofp, "Mean burst spikes : %.2f\n", burstdata->meancount);
		fprintf(ofp, "Mean burst time : %.2f\n", burstdata->meantime);
		fprintf(ofp, "Mean silence time : %.2f\n", burstdata->meansilence);
		fprintf(ofp, "Silence time SD: %.2f\n", burstdata->sdsilence);
		fprintf(ofp, "Intra burst rate : %.2f\n\n", burstfreq);
	}

	burstdata->times = times;
	burstdata->maxint = maxint;
	burstdata->IntraBurstAnalysis();

	if(ofp) fclose(ofp);
}


void SpikeDat::MeanSpikeForm(datdouble V, int timerange, int filter, int substeps)
{
	int i, j, numspikes;
	//int timerange = 2000;        // max 200   // why? March 2018
	wxString text;
	int maxtime = 1000000;

	for(i=0; i<5000; i++) meanV[i] = 0;
	numspikes = 0;

	//diagbox->Write(text.Format("MeanSpikeForm\nspikecount %d\n", spikecount));

	for(i=0; i<spikecount; i++) {
		if(times[i] < timerange) continue;
		if(filter) {
			if(i > 0 && (times[i] - times[i-1] < timerange/2)) continue;
			if(i < spikecount-1  && (times[i+1] - times[i] < timerange/2)) continue;
		}
		if(times[i] > maxtime) break;
		for(j=0; j<timerange*substeps; j++) meanV[j] += V[(times[i] - timerange/2) * substeps + j];
		numspikes++;
	}

	//diagbox->Write(text.Format("MeanSpikeForm\nspikecount %d  filter %d  timerange %d\n", spikecount, numspikes, timerange));

	for(i=0; i<5000; i++) meanV[i] = meanV[i] / numspikes;
}


void SpikeDat::output(wxString filetag, wxString path) 
{
	int i;
	int numsteps, substeps, sample;
	double normfactor;
	FILE *ofp;
	//char filename[100];
	wxString text, filename;
	TextFile outfile;

	normfactor = 1/freq * 10000;

	if(!wxDirExists(path)) wxMkdir(path);

	path += "\\";

	outfile.New(filename.Format(path + "%s-hist1.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.0f", i, hist1.data[i]));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-hist1norm.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.4f", i, hist1.data[i] * 10000 / spikecount));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-hist5norm.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.4f", i*5, hist5.data[i] * 10000 / spikecount));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-hist5timenorm.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.0f", i*5, hist5.data[i] * 10000 / (times[spikecount-1] - times[0])));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-hist5timefreqnorm.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.0f", i*5, hist5.data[i] * normfactor / (times[spikecount-1] - times[0])));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-hist5.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.0f", i*5, hist5.data[i]));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-haz1.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.6f", i, haz1.data[i]));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-haz5.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.6f", i*5, haz5.data[i]));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-hist5allnorm.txt", filetag));
	for(i=0; i<=2000; i++)
		outfile.WriteLine(text.Format("%d  %.2f  %.2f  %.2f", i*5, hist5.data[i], 
		hist5.data[i] * 10000 / (times[spikecount-1] - times[0]),
		hist5.data[i] * 10000 / spikecount));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-srate1.txt", filetag));
	for(i=0; i<=30000; i++)   
		outfile.WriteLine(text.Format("%d  %d", i, srate1.data[i]));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-srate.txt", filetag));
	for(i=0; i<=10000; i++)   
		outfile.WriteLine(text.Format("%d  %d", i, srate.data[i]));
	outfile.Close();

	outfile.New(filename.Format(path + "%s-times.txt", filetag));
	for(i=0; i<=spikecount; i++)   
		outfile.WriteLine(text.Format("%.5f", times[i]));
	outfile.Close();

	if(burstdata != NULL) {
		outfile.New(filename.Format(path + "%s-headprofile.txt", filetag));
		for(i=0; i<=100; i++)   
			outfile.WriteLine(text.Format("%d  %.4f", i, burstdata->profile.data[i]));
		outfile.Close();

		outfile.New(filename.Format(path + "%s-tailprofile.txt", filetag));
		for(i=0; i<=100; i++)   
			outfile.WriteLine(text.Format("%d  %.4f", i, burstdata->tailprofile.data[i]));
		outfile.Close();
	}
}


void AnaDat::autocalc(SpikeDat *data)
{
	int i;
	double autobin = 10;
	double autowindow = 4000;
	double windowtime = autowindow / 1000;
	int windowstart = 0;
	int windowmark = 0;
	int winindex;
	double timediff;

	spikedata = data;
	FILE *ofp;
	ofp = fopen("autocalc.txt", "w");

	for(i=0; i<10000; i++) autocorr[i] = 0;

	fprintf(ofp, "Spike Count %d\n", spikedata->spikecount);

	for(i=0; i<spikedata->spikecount; i++) {
		if(spikedata->times[i] < windowtime || spikedata->times[spikedata->spikecount-1] - spikedata->times[windowmark] < windowtime) continue;
		while(spikedata->times[i] - spikedata->times[windowstart] > windowtime) windowstart++;
		//fprintf(ofp, "Spike %d time %.4f windowstart %d time %.4f\n", i, spikedata->times[i], windowstart, spikedata->times[windowstart]);
		windowmark = windowstart;
		while(spikedata->times[windowmark] - spikedata->times[i] < windowtime) {
			timediff = (spikedata->times[windowmark] - spikedata->times[i]) + windowtime;
			winindex = (int)(timediff*1000/autobin);
			if(windowmark != i) autocorr[winindex]++;
			windowmark++;
		}
	}
	fclose(ofp);
}


void SpikeDat::neurocalcBasic(NeuroDat *datneuron, ParamStore *calcparams) 
{
	int i;
	int spikestep, binsize;
	int neurodat = 0;
	//int maxspikes = 100000;
	int maxtime = 100000;
	int max1 = 1000000;
	int max100 = 100000;
	double hazcount;
	double mean, variance;
	double norm, haznorm;
	double stime;
	wxString text;
	bool spikediag = false; 
	//short freqwindow = 300;
	bool calcdiag, calcdiag2;
	FILE *ofp;

	//maxspikes = 100000;

	calcdiag = false;
	calcdiag2 = false;

	if(calcdiag) ofp = fopen("neurocalc.txt", "w");

	if(datneuron != NULL) neurodat = 1;

	for(i=0; i<10000; i++) {
		hist1[i] = 0;
		hist5[i] = 0;
		haz1[i] = 0;	
		haz5[i] = 0;	
		hist1norm[i] = 0;
		hist5norm[i] = 0;
	}

	for(i=0; i<maxtime; i++) srate[i] = 0;
	for(i=0; i<max1; i++) srate1[i] = 0;
	for(i=0; i<max100; i++) srate100[i] = 0;

	hist1.max = 0;
	hist5.max = 0;
	hist1norm.max = 0;
	hist5norm.max = 0;
	histquad.max = 0;
	haz1.max = 0;
	haz5.max = 0;
	//haz1norm.max = 0;
	haz5norm.max = 0;
	hazquad.max = 0;
	srate.max = 0;
	srate1.max = 0;
	srate100.max = 0;

	mean = 0;
	variance = 0;
	norm = 1;
	binsize = 5;

	if(neurodat) {
		spikecount = datneuron->spikecount;
		//isis = datneuron->isis;
	}
	stime = start;

	if(calcdiag) fprintf(ofp, "Neuron %d  Spike Count %d\n", neurodat, spikecount);

	// ISIs, Histogram, Freq, Variance

	isicount = spikecount - 1;
	if(neurodat) times[0] = datneuron->times[0];

	for(i=0; i<isicount; i++) {                                      // February 2014  interval recode
		if(i > maxspikes) break;
		if(neurodat) times[i+1] = datneuron->times[i+1];
		isis[i] = times[i+1] - times[i];
		if(hist1.max < (int)isis[i]) hist1.max = (int)isis[i];
		if(hist1.data.size() < hist1.max + 1) hist1.data.resize(hist1.max + 1);
		hist1[(int)isis[i]]++;	
		mean = mean + (double)(isis[i] / isicount);
		variance = isis[i] * isis[i] / isicount + variance;
		//if(spikediag && mainwin) mainwin->diagbox->Write(text.Format("i=%d time %.4f isi %.4f\n", i, times[i], isis[i]));
	}

	isisd = sqrt(variance - mean * mean);
	freq = 1000/mean;
	if(mean == 0) freq = 0;
	meanisi = mean;
	isivar = variance;

	if(calcdiag) fprintf(ofp, "Freq %.2fHz  Hist1 max %d  Hist1 500 %.2f\n\n", freq, hist1.max, hist1.data[500]);

	if(neurodat) {
		datneuron->freq = freq;
		datneuron->meanisi = mean;
		datneuron->isivar = isisd;
		datneuron->netflag = 0;
	}


	// Rate Count (1s)

	//if(calcdiag) fprintf(ofp, "Rate count  last spike at %.2fs.\n", times[spikecount-1]/1000);

	spikestep = 0;
	srate.max = (int)(times[spikecount-1]/1000 + 0.5); 
	for(i=0; i<times[spikecount-1]/1000; i++) {	     // spike rate count (1s)
		if(calcdiag) fprintf(ofp, "%ds. ", i);
		if(spikestep > spikecount) {
			//if(calcdiag) fprintf(ofp, "break spikestep %d  spikecount %d\n", spikestep, spikecount);
			break;
		}
		while(times[spikestep]/1000 < i+1) {
			if(calcdiag) fprintf(ofp, "spike %d  ", spikestep);
			if(i < maxtime) srate.data[i]++;
			spikestep++;
			if(spikestep >= spikecount) break;
		}
		//if(calcdiag) fprintf(ofp, "srate %d\n", srate.data[i]);
	}

	//if(calcdiag) fprintf(ofp, "Rate count end  spikestep %d  spikecount %d\n", spikestep, spikecount);

	srate1.max = (int)(times[spikecount-1] + 0.5);
	srate100.max = (int)(times[spikecount-1]/100 + 0.5);

	for(i=0; i<spikecount; i++) {
		if(times[i] < 1000000) srate1[(int)(times[i] + 0.5)]++;            // spike rate count (1ms)
		if(times[i]/100 < 100000) srate100[(int)(times[i]/100 + 0.5)]++;
		//if(spikediag && mainwin) mainwin->diagbox->Write(text.Format("srate1 i=%d time %.2f bin %d\n", i, times[i], (int)(times[i] + 0.5)));
	}

	//if(mainwin) mainwin->diagbox->Write(text.Format("srate1 i=%d \n", i, times[i], (int)(times[i]+0.5)));
	//if(mainwin) mainwin->diagbox->Write(text.Format("srate1 i=%d time %.2f bin %d\n", i, times[i], (int)(times[i] + 0.5)));

	if(times[spikecount] > 1000) srate1.max = srate1.data.size();


	if(calcdiag) fclose(ofp);

	if(calcdiag2) {
		ofp = fopen("net-g18.txt", "w");
		fprintf(ofp, "Numspikes = %d\n\n", spikecount);
		for(i=0; i<5000; i++)
			fprintf(ofp, "%d time %.8f int %.4f\n", i, times[i], isis[i]); 
		fclose(ofp);

		ofp = fopen("net-rate1.txt", "w");
		fprintf(ofp, "Numspikes = %d\n\n", spikecount);
		fprintf(ofp, "max = %d\n\n", srate.max);
		for(i=0; i<2000; i++)
			fprintf(ofp, "%d ms, %d spikes\n", i, srate1.data[i]); 
		fclose(ofp);
	}

	// Hazard

	hazcount = 0;
	haznorm = 5/freq;  // 10/freq
	haz1.max = hist1.max;
	if(haz1.data.size() < haz1.max + 1) haz1.data.resize(haz1.max + 1);

	for(i=0; i<=hist1.max; i++) {        // 1ms Hazard
		haz1[i] = hist1[i] / (spikecount - hazcount); //   / freq;		
		hazcount = hazcount + hist1[i];
	}

	for(i=0; i<hist1.max; i++) {      // 5ms Hazard                                               
		//haz5[i/binsize] = haz5[i/binsize] + haz[i] * 100;	          // Nancy sheet
		if(i/binsize > haz5.max) haz5.max = i/binsize;
		if(haz5.data.size() < haz5.max + 1)	haz5.data.resize(haz5.max + 1);
		haz5[i/binsize] = haz5[i/binsize] + haz1[i]; // * haznorm;	
	}

	for(i=0; i<=hist1.max; i++) {         // 5ms ISI Histogram
		if(i/binsize > hist5.max) hist5.max = i/binsize;
		if(hist5.data.size() < hist5.max + 1)	hist5.data.resize(hist5.max + 1);
		hist5[i/binsize] = hist5[i/binsize] + hist1[i];		
	}


	// Normalise Histograms
	for(i=0; i<=hist1.max; i++) {
		hist1norm[i] = normscale * hist1[i] / isicount;
		hist5norm[i] = normscale * hist5[i] / isicount;
		haz5norm[i] = haz5[i] * haznorm;
	}	
}

void SpikeDat::neurocalc(NeuroDat *datneuron, ParamStore *calcparams) 
{
	int i;
	int spikestep, binsize;
	int neurodat = 0;
	//int maxspikes = 100000;
	int maxtime = 100000;
	int max1 = 1000000;
	int max100 = 100000;
	double hazcount;
	double mean, variance;
	double norm, haznorm;
	double stime;
	wxString text;
	bool spikediag = false; 
	//short freqwindow = 300;
	bool calcdiag, calcdiag2;
	FILE *ofp;

	//maxspikes = 100000;

	//diagbox->Write("\nneurocalc\n");

	calcdiag = false;
	calcdiag2 = false;

	if(calcdiag) ofp = fopen("neurocalc.txt", "w");

	if(datneuron != NULL) neurodat = 1;

	for(i=0; i<10000; i++) {
		hist1[i] = 0;
		hist5[i] = 0;
		histquad[i] = 0;
		haz1[i] = 0;	
		haz5[i] = 0;	
		hazquad[i] = 0;
		hist1norm[i] = 0;
		hist5norm[i] = 0;
		//haz1norm.data[i] = 0;
		haz5norm[i] = 0;
	}

	for(i=0; i<maxtime; i++) srate[i] = 0;
	for(i=0; i<max1; i++) srate1[i] = 0;
	for(i=0; i<max100; i++) {
		srate10[i] = 0;
		srate100[i] = 0;
		srate10s[i] = 0;
	}
	for(i=0; i<10000; i++) srate100s[i] = 0;

	hist1.max = 0;
	hist5.max = 0;
	hist1norm.max = 0;
	hist5norm.max = 0;
	histquad.max = 0;
	haz1.max = 0;
	haz5.max = 0;
	//haz1norm.max = 0;
	//haz5norm.max = 0;
	hazquad.max = 0;
	srate.max = 0;
	srate1.max = 0;
	srate100.max = 0;

	mean = 0;
	variance = 0;
	norm = 1;
	binsize = 5;

	if(neurodat) {
		spikecount = datneuron->spikecount;
		//isis = datneuron->isis;
	}
	stime = start;

	if(calcdiag) fprintf(ofp, "Neuron %d  Spike Count %d\n", neurodat, spikecount);

	// ISIs, Histogram, Freq, Variance

	//times[0] = stime/1000;
	isicount = spikecount - 1;
	if(neurodat) times[0] = datneuron->times[0];

	for(i=0; i<isicount; i++) {                                      // February 2014  interval recode
		if(i > maxspikes) break;
		if(neurodat) times[i+1] = datneuron->times[i+1];
		isis[i] = times[i+1] - times[i];
		if(hist1.max < (int)isis[i]) hist1.max = (int)isis[i];
		if(hist1.data.size() < hist1.max + 1) hist1.data.resize(hist1.max + 1);
		hist1[(int)isis[i]]++;	
		mean = mean + (double)(isis[i] / isicount);
		variance = isis[i] * isis[i] / isicount + variance;
		//if(spikediag && mainwin) mainwin->diagbox->Write(text.Format("i=%d time %.4f isi %.4f\n", i, times[i], isis[i]));
		if(spikediag && diagbox) if(i<10) diagbox->Write(text.Format("i=%d time %.4f isi %.4f\n", i, times[i], isis[i]));
	}

	/*
	for(i=1; i<spikecount; i++) {
	//isis[i] = (currentset->times[i+1] - currentset->times[i]) * 1000;   // real
	if(i > maxspikes-1) break;
	if(neurodat) isis[i] = datneuron->isis[i];
	stime = stime + isis[i];
	times[i] = stime/1000;
	//index[i] = i;
	if(hist1.max < (int)isis[i]) hist1.max = (int)isis[i];
	if(hist1.data.size() < hist1.max + 1) hist1.data.resize(hist1.max + 1);
	hist1[(int)isis[i]]++;	
	mean = mean + (double)(isis[i] / spikecount);
	variance = isis[i] * isis[i] / spikecount + variance;
	}*/

	isisd = sqrt(variance - mean * mean);      // Is this correct? 10/12/17 
	if(mean == 0) freq = 0;
	else freq = 1000/mean;
	meanisi = mean;
	isivar = variance;

	//if(diagbox) diagbox->Write(text.Format("spikecount %d freq %.2f\n", spikecount, freq)); 


	// Quad Binned Histogram                      24th November 2012

	double initbinwidth = 1;
	double bininc = 0.05;
	double d_fInitBinWidth = 0.001;
	double d_fBinIncrAmount = 0.00005;
	double b = initbinwidth - bininc / 2;
	double BSquared = b*b;
	double FourA = 2*bininc;
	double TwoA = bininc;

	//This equation defines an ever increasing bin width                        Comment from Tom's code, SimulateIGF2.cs
	//Even though it is quantised, it will increase by BinSpacing               BinSpacing likely equals bininc
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
	double htemp[1000];
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
		width = (histquadx[i+1] - histquadx[i]);          //     / 0.001;
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


	if(calcdiag) fprintf(ofp, "Freq %.2fHz  Hist1 max %d  Hist1 500 %.2f\n\n", freq, hist1.max, hist1.data[500]);

	if(neurodat) {
		datneuron->freq = freq;
		datneuron->meanisi = mean;
		datneuron->isivar = isisd;
		datneuron->netflag = 0;
	}

	//if(normal)
	//  norm = (double)spikecount / 5000;	

	// Rate Count (1s)

	if(calcdiag) fprintf(ofp, "Rate count  last spike at %.2fs.\n", times[spikecount-1]/1000);

	spikestep = 0;
	srate.max = (int)(times[spikecount-1]/1000 + 0.5); 
	for(i=0; i<times[spikecount-1]/1000; i++) {	     // spike rate count (1s)
		if(calcdiag) fprintf(ofp, "%ds. ", i);
		if(spikestep > spikecount) {
			if(calcdiag) fprintf(ofp, "break spikestep %d  spikecount %d\n", spikestep, spikecount);
			break;
		}
		while(times[spikestep]/1000 < i+1) {
			if(calcdiag) fprintf(ofp, "spike %d  ", spikestep);
			if(i < maxtime) srate.data[i]++;
			spikestep++;
			if(spikestep >= spikecount) break;
		}
		if(calcdiag) fprintf(ofp, "srate %d\n", srate.data[i]);
	}

	if(calcdiag) fprintf(ofp, "Rate count end  spikestep %d  spikecount %d\n", spikestep, spikecount);

	srate1.max = (int)(times[spikecount-1] + 0.5);
	srate10.max = (int)(times[spikecount-1]/10 + 0.5);
	srate100.max = (int)(times[spikecount-1]/100 + 0.5);
	srate10s.max = (int)(times[spikecount-1]/10000 + 0.5);
	srate100s.max = (int)(times[spikecount-1]/100000 + 0.5);

	for(i=0; i<spikecount; i++) {
		if(start < 100000 && times[i] < 1000000) srate1[(int)(times[i] + 0.5)]++;            // spike rate count (1ms)
		if(start >= 100000 && times[i] < (start + 1000000)) srate1[(int)(times[i] + 0.5 - start)]++;            // spike rate count (1ms) with deferred start point
		if(times[i]/10 < 100000) srate10[(int)(times[i]/10 + 0.5)]++;
		if(times[i]/100 < 100000) srate100[(int)(times[i]/100 + 0.5)]++;
		if(times[i]/10000 < 100000) srate10s[(int)(times[i] + 0.5)/10000]++;
		if(times[i]/100000 < 100000) srate100s[(int)(times[i] + 0.5)/100000]++;
		//if(spikediag && mainwin) mainwin->diagbox->Write(text.Format("srate1 i=%d time %.2f bin %d\n", i, times[i], (int)(times[i] + 0.5)));
		//if(mainwin) mainwin->diagbox->Write(text.Format("srate10s i=%d time %.2f bin %d\n", i, times[i], (int)(times[i]+0.5)/10000));
	}

	if(neurodat) {
		//for(i=0; i<100000; i++)	srate10s[i] = datneuron->srate10[i];
		//for(i=0; i<10000; i++)	srate100s[i] = datneuron->srate100[i];            // What is this for? 20/2/17   Vaso net maybe?
	}

	//if(mainwin) mainwin->diagbox->Write(text.Format("srate1 i=%d \n", i, times[i], (int)(times[i]+0.5)));
	//if(mainwin) mainwin->diagbox->Write(text.Format("srate1 i=%d time %.2f bin %d\n", i, times[i], (int)(times[i] + 0.5)));

	if(times[spikecount] > 1000) srate1.max = srate1.data.size();


	// Freq Window                                        new 23/11/11

	if(calcdiag) fprintf(ofp, "\nFreq Window = %d\n\n", freqwindow);
	double winsum = 0;
	for(i=0; i<=times[spikecount-1]/1000; i++) {
		if(i<=freqwindow) {
			winsum += srate[i];
			winfreq[i] = 0;
		}
		else {
			winsum += srate[i] - srate[i-freqwindow];
			winfreq[i - freqwindow / 2] = winsum / freqwindow;
		}
	}

	if(calcdiag) fclose(ofp);

	if(calcdiag2) {
		ofp = fopen("net-g18.txt", "w");
		fprintf(ofp, "Numspikes = %d\n\n", spikecount);
		for(i=0; i<5000; i++)
			fprintf(ofp, "%d time %.8f int %.4f\n", i, times[i], isis[i]); 
		fclose(ofp);

		ofp = fopen("net-rate1.txt", "w");
		fprintf(ofp, "Numspikes = %d\n\n", spikecount);
		fprintf(ofp, "max = %d\n\n", srate.max);
		for(i=0; i<2000; i++)
			fprintf(ofp, "%d ms, %d spikes\n", i, srate1.data[i]); 
		fclose(ofp);
	}

	// Hazard

	hazcount = 0;
	haznorm = 10/freq;
	haz1.max = hist1.max;
	if(haz1.data.size() < haz1.max + 1) haz1.data.resize(haz1.max + 1);

	for(i=0; i<=hist1.max; i++) {        // 1ms Hazard
		haz1.data[i] = hist1.data[i] / (spikecount - hazcount); //   / freq;		
		hazcount = hazcount + hist1.data[i];
	}

	for(i=0; i<hist1.max; i++) {      // 5ms Hazard                                               
		//haz5[i/binsize] = haz5[i/binsize] + haz[i] * 100;	          // Nancy sheet
		if(i/binsize > haz5.max) haz5.max = i/binsize;
		if(haz5.data.size() < haz5.max + 1)	haz5.data.resize(haz5.max + 1);
		haz5.data[i/binsize] = haz5.data[i/binsize] + haz1.data[i]; // * haznorm;	
	}

	for(i=0; i<=hist1.max; i++) {         // 5ms ISI Histogram
		//hist[i] = hist[i] / norm;
		if(i/binsize > hist5.max) hist5.max = i/binsize;
		if(hist5.data.size() < hist5.max + 1)	hist5.data.resize(hist5.max + 1);
		hist5.data[i/binsize] = hist5.data[i/binsize] + hist1.data[i];		
	}

	// Normalise
	for(i=0; i<=hist1.max; i++) {
		hist1norm[i] = normscale * hist1[i] / isicount;
		hist5norm[i] = normscale * hist5[i] / isicount;
		haz5norm[i] = haz5[i] * haznorm;
	}


	// Index of Dispersion Code

	IoDdata[0] = dispcalc(500);
	IoDdata[1] = dispcalc(1000);
	IoDdata[2] = dispcalc(2000);
	IoDdata[3] = dispcalc(4000);
	IoDdata[4] = dispcalc(6000);
	IoDdata[5] = dispcalc(8000);
	IoDdata[6] = dispcalc(10000);              // order rearranged to match Trystan figure  March 2016   // restored Feb 2017

	IoDdataX[0] = 5;
	IoDdataX[1] = 15;
	IoDdataX[2] = 25;
	IoDdataX[3] = 35;
	IoDdataX[4] = 45;
	IoDdataX[5] = 55;
	IoDdataX[6] = 65;

	/*   Jorge IoD code

	for(i=0; i<100000; i++) {
	spikerate05.data[i] = 0;
	spikerate1.data[i] = 0;
	spikerate2.data[i] = 0;
	spikerate4.data[i] = 0;
	}

	for(i=0; i<spikecount; i++) {
	spikerate05[(int)(times[i])/500]++;  // 0.5 second BIN
	spikerate1[(int)(times[i])/1000]++;  
	spikerate2[(int)(times[i])/2000]++;  
	spikerate4[(int)(times[i])/4000]++;  
	spikerate6[(int)(times[i])/6000]++;  
	spikerate8[(int)(times[i])/8000]++;  
	spikerate10[(int)(times[i])/10000]++; // 10 seconds BIN		
	}
	double laststep05, laststep1, laststep2, laststep4, laststep6, laststep8, laststep10;
	laststep05 =  ((int)(times[spikecount-1])/500)-4;
	laststep1 =  ((int)(times[spikecount-1])/1000)-4;
	laststep2 =  ((int)(times[spikecount-1])/2000)-4;
	laststep4 =  ((int)(times[spikecount-1])/4000)-4;
	laststep6 =  ((int)(times[spikecount-1])/6000)-4;
	laststep8 =  ((int)(times[spikecount-1])/8000)-4;
	laststep10 =  ((int)(times[spikecount-1])/10000)-4;
	// **************** 0.5 sec **************************
	double mean05, variance05, temp05, dispersion05; 
	mean05 = variance05 = temp05 = dispersion05 = 0.0;
	for (int a=0; a<laststep05;  a++) mean05 = mean05 +  spikerate05[a]; //mean
	mean05 = mean05/laststep05;
	//mainwin->diagbox->Write(text.Format("En Hypoanalysis -> %.4f Number of elements-> %d \n", mean05, (int)(times[spikecount])/500-4));
	for(double a=0; a<laststep05;a++) temp05 += (mean05-spikerate05[a])*(mean05-spikerate05[a]); // variance
	variance05 = temp05/laststep05;
	//mainwin->diagbox->Write(text.Format("En Hypoanalysis Variance05-> %.4f Number of elements-> %d \n", variance05, (int)(times[spikecount])/500-4));
	dispersion05 = variance05/mean05; // dispersion
	// **************** 1 sec **************************
	double mean1, variance1, temp1, dispersion1; 
	mean1 = variance1 = temp1 = dispersion1 = 0.0;
	for (int a=0; a<laststep1;  a++) mean1 = mean1 +  spikerate1[a]; //mean
	mean1 = mean1/(laststep1);
	for(double a=0; a<laststep1;a++) temp1 += (mean1-spikerate1[a])*(mean1-spikerate1[a]); // variance
	variance1 = temp1/(laststep1);
	dispersion1 = variance1/mean1; // dispersion
	// **************** 2 sec **************************
	double mean2, variance2, temp2, dispersion2; 
	mean2 = variance2 = temp2 = dispersion2 = 0.0;
	for (int a=0; a<laststep2;  a++) mean2 = mean2 +  spikerate2[a]; //mean
	mean2 = mean2/(laststep2);
	for(double a=0; a<laststep2;a++) temp2 += (mean2-spikerate2[a])*(mean2-spikerate2[a]); // variance
	variance2 = temp2/(laststep2);
	dispersion2 = variance2/mean2; // dispersion
	// **************** 4 sec **************************
	double mean4, variance4, temp4, dispersion4; 
	mean4 = variance4 = temp4 = dispersion4 = 0.0;
	for (int a=0; a<laststep4;  a++) mean4 = mean4 +  spikerate4[a]; //mean
	mean4 = mean4/(laststep4);
	for(double a=0; a<laststep4;a++) temp4 += (mean4-spikerate4[a])*(mean4-spikerate4[a]); // variance
	variance4 = temp4/(laststep4);
	dispersion4 = variance4/mean4; // dispersion
	// **************** 6 sec **************************
	double mean6, variance6, temp6, dispersion6; 
	mean6 = variance6 = temp6 = dispersion6 = 0.0;
	for (int a=0; a<laststep6;  a++) mean6 = mean6 +  spikerate6[a]; //mean
	mean6 = mean6/(laststep6);
	for(double a=0; a<laststep6;a++) temp6 += (mean6-spikerate6[a])*(mean6-spikerate6[a]); // variance
	variance6 = temp6/(laststep6);
	dispersion6 = variance6/mean6; // dispersion
	// **************** 8 sec **************************
	double mean8, variance8, temp8, dispersion8 ; 
	mean8 = variance8 = temp8 = dispersion8 = 0.0;
	for (int a=0; a<laststep8;  a++) mean8 = mean8 +  spikerate8[a]; //mean
	mean8 = mean8/(laststep8);
	for(double a=0; a<laststep8;a++) temp8 += (mean8 - spikerate8[a])*(mean8 - spikerate8[a]); // variance
	variance8 = temp8/(laststep8);
	dispersion8 = variance8/mean8; // dispersion
	// **************** 10 sec **************************
	double mean10, variance10, temp10, dispersion10; 
	mean10 = variance10 = temp10 = dispersion10 = 0.0;
	for (int a=0; a<laststep10;  a++) mean10 = mean10 +  spikerate10[a]; //mean
	mean10 = mean10/(laststep10);
	for(double a=0; a<laststep10;a++) temp10 += (mean10 - spikerate10[a])*(mean10 - spikerate10[a]); // variance
	variance10 = temp10/(laststep10);
	dispersion10 = variance10/mean10; // dispersion
	*/

	/*
	// Array of dispersions for altogether plotting 
	for (int i=6; i<12; i++) dispersionsre[i] = dispersion05;
	for (int i=31; i<37; i++) dispersionsre[i] = dispersion1;
	for (int i=56; i<62; i++) dispersionsre[i] = dispersion2;
	for (int i=81; i<87; i++) dispersionsre[i] = dispersion4;
	for (int i=106; i<112; i++) dispersionsre[i] = dispersion6;
	for (int i=131; i<137; i++) dispersionsre[i] = dispersion8;
	for (int i=156; i<162; i++) dispersionsre[i] = dispersion10;
	// Array of Real dispersions for altogether plotting 
	for (int i=13; i<19; i++) dispersions[i] = dispersion05;
	for (int i=38; i<44; i++) dispersions[i] = dispersion1;
	for (int i=63; i<69; i++) dispersions[i] = dispersion2;
	for (int i=88; i<94; i++) dispersions[i] = dispersion4;
	for (int i=113; i<119; i++) dispersions[i] = dispersion6;
	for (int i=138; i<144; i++) dispersions[i] = dispersion8;
	for (int i=163; i<169; i++) dispersions[i] = dispersion10;
	*/
}

/*
double SpikeDat::dispcalc(int binsize)
{
int i;
int maxbin = 10000;
int spikerate[10000];
int laststep;
double mean, variance, dispersion = 0;
double timeshift = 0;

if(times[0] > 1000) timeshift = times[0] - 1000;        // for data where recording starts at non-zero time point

// calculate spike rate for binsize
for(i=0; i<maxbin; i++) spikerate[i] = 0;
for(i=0; i<spikecount; i++) if((times[i] - timeshift) / binsize < maxbin) spikerate[(int)((times[i] - timeshift) + 0.5) / binsize]++;
laststep = ((int)(times[spikecount - 1] - timeshift)/ binsize) - 4;
if(laststep > maxbin) laststep = maxbin;

// calculate index of dispersion
mean = 0;
variance = 0;
for(i=0; i<laststep; i++) mean = mean +  spikerate[i];	//mean
mean = mean / laststep;
for(i=0; i<laststep; i++) variance += (mean - spikerate[i]) * (mean - spikerate[i]);	// variance
variance = variance / laststep;
dispersion = variance / mean;		// dispersion

return dispersion;
}*/



// IoDDat class not currently in use

IoDDat::IoDDat(int bsize, SpikeDat *sdata)
{
	binsize = bsize;
	spikedata = sdata;
	srate = new int(100000);
}


void IoDDat::ratecalc()
{
	int i;

	for(i=0; i<spikedata->spikecount; i++) {
		if(spikedata->times[i] / binsize < 100000) srate[(int)(spikedata->times[i] + 0.5) / binsize]++;
	}
	laststep = ((int)(spikedata->times[spikedata->spikecount - 1])/ binsize) - 4;
}


double IoDDat::dispcalc()
{
	int i;

	mean = 0;
	variance = 0;

	ratecalc();

	for(i=0; i<laststep; i++) mean = mean +  srate[i];	//mean
	mean = mean / laststep;
	for(i=0; i<laststep; i++) variance += (mean - srate[i]) * (mean - srate[i]);	// variance
	variance = variance / laststep;
	dispersion = variance / mean;		// dispersion

	return dispersion;
}


void SpikeDat::ISIanalysis()
{
	int i;
}


void SpikeDat::inputsim(double halflife)
{
	int i;
	double memtau;
	double v, epsph, nepsp;
	double syninput;
	double vrest = -62;
	double ae = 0.04;
	double ve = 38;

	halflife = 7.5;
	memtau = 1 / (log((double)2) / halflife);

	v = vrest;
	//memtau = 3.6067;

	for(i=0; i<1000000; i++) {
		epsph = ae * (ve - v);
		nepsp = srate1.data[i];
		syninput = epsph * nepsp;
		//v = v - (v - vrest) * 0.1 + syninput;
		v = v - (v - vrest) / memtau + syninput;
		synsim.data[i] = v;
	}
}


int SpikeDat::GraphSet(GraphBase *graphbase, wxString tag, int colour, int light, wxString reftag, wxString btag)
{
	int setindex;
	int shift;
	wxString text;

	if(light) shift = 5;
	else shift = 0;

	//graphbase->NewSet(tag, reftag);
	//diagbox->Write(text.Format("\nGraphSet tag %s test %d\n", btag, burstdata->test));

	//if(diagbox) diagbox->Write(text.Format("GraphSet entered... tag %s\n", tag));

	setindex = graphbase->Add(GraphDat(&srate, 0, 500, 0, 20, tag + "Spike Rate 1s", this, 1, red + shift), reftag + "rate1s", reftag);
	graphbase->Add(GraphDat(&srate100s, 0, 500, 0, 2000, tag + "Spike Rate 100s", this, 100, red + shift), reftag + "rate100s", reftag);
	graphbase->Add(GraphDat(&srate10s, 0, 500, 0, 200, tag + "Spike Rate 10s", this, 10, red + shift), reftag + "rate10s", reftag);
	graphbase->Add(GraphDat(&srate100, 0, 50, 0, 20, tag + "Spike Rate 100ms", this, 0.1, red + shift), reftag + "rate100ms", reftag);
	graphbase->Add(GraphDat(&srate10, 0, 5, 0, 20, tag + "Spike Rate 10ms", this, 0.1, red + shift), reftag + "rate10ms", reftag);
	graphbase->Add(GraphDat(&srate1, 0, 0.5, 0, 3, tag + "Spikes 1ms", this, 0.001, red + shift), reftag + "spikes1ms", reftag);
	graphbase->Add(GraphDat(&hist1, 0, 500, 0, 100, tag + "Hist 1ms", 1, 1, colour + shift), reftag + "hist1ms", reftag);
	graphbase->Add(GraphDat(&hist5, 0, 500, 0, 500, tag + "Hist 5ms", 1, 5, colour + shift), reftag + "hist5ms", reftag);
	graphbase->Add(GraphDat(&haz1, 0, 500, 0, 0.04, tag + "Haz 1ms", 1, 1, colour + shift), reftag + "haz1ms", reftag);
	graphbase->Add(GraphDat(&haz5, 0, 500, 0, 0.2, tag + "Haz 5ms", 1, 5, colour + shift), reftag + "haz5ms", reftag);
	graphbase->Add(GraphDat(&winfreq, 0, 500, 0, 20, tag + "Win Freq", 4, 1, green + shift), reftag + "winfreq", reftag);

	//if(diagbox) diagbox->Write(text.Format("GraphSet burst... tag %s\n", btag));

	if(burstdata) {
		graphbase->Add(GraphDat(&burstdata->hist1, 0, 500, 0, 100, btag + tag + "Hist 1ms", 1, 1, colour + shift), reftag + "bursthist1ms", reftag);
		graphbase->Add(GraphDat(&burstdata->hist5, 0, 500, 0, 500, btag + tag + "Hist 5ms", 1, 5, colour + shift), reftag + "bursthist5ms", reftag);
		graphbase->Add(GraphDat(&burstdata->haz1, 0, 500, 0, 0.04, btag + tag + "Haz 1ms", 1, 1, colour + shift), reftag + "bursthaz1ms", reftag);
		graphbase->Add(GraphDat(&burstdata->haz5, 0, 500, 0, 0.2, btag + tag + "Haz 5ms", 1, 5, colour + shift), reftag + "bursthaz5ms", reftag);
		graphbase->Add(GraphDat(&burstdata->profile, 0, 250, 0, 20, tag + "Burst Head Profile", 4, 1, colour + shift), reftag + "-burst-headprofile", reftag);
		graphbase->Add(GraphDat(&burstdata->tailprofile, 0, 250, 0, 20, tag + "Burst Tail Profile", 4, 1, colour + shift), reftag + "-burst-tailprofile", reftag);
		graphbase->Add(GraphDat(&burstdata->profilesm, 0, 250, 0, 20, tag + "Burst Profile Smooth", 4, 1, colour + shift), reftag + "-burst-headprofilesm", reftag);
		graphbase->Add(GraphDat(&burstdata->bursthaz, 0, 250, 0, 200, tag + "Burst Haz", 1, 1, colour + shift), reftag + "-bursthaz", reftag);
	}

	//if(diagbox) diagbox->Write(text.Format("GraphSet stage 1 OK\n"));

	//graphbase->Add(GraphDat(&histquad, 0, 125, 0, 500, tag + "ISI Histogram Quad", 1, 1, green + shift), reftag + "histquad", reftag);                // old colours 9/2/17
	//graphbase->Add(GraphDat(&histquadsm, 0, 125, 0, 0.1, tag + "ISI Histogram Quad Smooth", 1, 1, blue + shift), reftag + "histquadsmooth", reftag);
	//graphbase->Add(GraphDat(&histquadlin, 0, 125, 0, 0.1, tag + "ISI Histogram Quad Linear", 1, 1, red + shift), reftag + "histquadlinear", reftag);
	//graphbase->Add(GraphDat(&hazquad, 0, 125, 0, 0.1, tag + "Hazard Quad", 1, 1, green + shift), reftag + "hazquad", reftag);

	graphbase->Add(GraphDat(&histquad, 0, 125, 0, 500, tag + "ISI Histogram Quad", 1, 1, colour + shift), reftag + "histquad", reftag);
	graphbase->Add(GraphDat(&histquadsm, 0, 125, 0, 0.1, tag + "ISI Histogram Quad Smooth", 1, 1, colour + shift), reftag + "histquadsmooth", reftag);
	graphbase->Add(GraphDat(&histquadlin, 0, 125, 0, 0.1, tag + "ISI Histogram Quad Linear", 1, 1, colour + shift), reftag + "histquadlinear", reftag);
	graphbase->Add(GraphDat(&hazquad, 0, 125, 0, 0.1, tag + "Hazard Quad", 1, 1, colour + shift), reftag + "hazquad", reftag);

	graphbase->Add(GraphDat(&hist1norm, 0, 500, 0, 100, tag + "Norm Hist 1ms", 1, 1, colour + shift), reftag + "normhist1ms", reftag);
	graphbase->Add(GraphDat(&hist5norm, 0, 500, 0, 500, tag + "Norm Hist 5ms", 1, 5, colour + shift), reftag + "normhist5ms", reftag);
	graphbase->Add(GraphDat(&burstdata->hist1norm, 0, 500, 0, 100, btag + tag + "Norm Hist 1ms", 1, 1, colour + shift), reftag + "burstnormhist1ms", reftag);
	graphbase->Add(GraphDat(&burstdata->hist5norm, 0, 500, 0, 500, btag + tag + "Norm Hist 5ms", 1, 5, colour + shift), reftag + "burstnormhist5ms", reftag);

	graphbase->Add(GraphDat(&haz5norm, 0, 500, 0, 0.2, tag + "Norm Haz 5ms", 1, 5, colour + shift), reftag + "normhaz5ms", reftag);

	(*graphbase)[reftag + "rate1s"]->synchx = false;
	(*graphbase)[reftag + "spikes1ms"]->synchx = false;
	(*graphbase)[reftag + "hist1ms"]->synchx = false;
	(*graphbase)[reftag + "hist5ms"]->synchx = false;
	(*graphbase)[reftag + "haz1ms"]->synchx = false;
	(*graphbase)[reftag + "haz5ms"]->synchx = false;
	(*graphbase)[reftag + "normhist1ms"]->synchx = false;
	(*graphbase)[reftag + "normhist5ms"]->synchx = false;

	graphindex = setindex;
	//graphbase->datdex[datset] = setindex; 
	graphs = true;

	//if(diagbox) diagbox->Write(text.Format("GraphSet Finished\n"));

	return setindex;
}


int SpikeDat::GraphSetLysis(GraphBase *graphbase, wxString tag, int colour, int light, wxString reftag, wxString btag)
{
	int setindex;
	int shift;

	if(light) shift = 5;
	else shift = 0;

	setindex = graphbase->Add(GraphDat(&srate, 0, 500, 0, 20, tag + "Spike Rate 1s", this, 1, red + shift), reftag + "rate1s");
	graphbase->Add(GraphDat(&srate100, 0, 50, 0, 20, tag + "Spikes 100ms", this, 0.1, red + shift), reftag + "rate100ms");
	graphbase->Add(GraphDat(&srate1, 0, 0.5, 0, 3, tag + "Spikes 1ms", this, 0.001, red + shift), reftag + "spikes1ms");
	graphbase->Add(GraphDat(&hist1, 0, 500, 0, 100, tag + "ISI Histogram 1ms", 1, 1, colour + shift), reftag + "hist1ms");
	graphbase->Add(GraphDat(&hist5, 0, 500, 0, 500, tag + "ISI Histogram 5ms", 1, 5, colour + shift), reftag + "hist5ms");
	graphbase->Add(GraphDat(&haz1, 0, 500, 0, 0.04, tag + "Hazard 1ms", 1, 1, colour + shift), reftag + "haz1ms");
	graphbase->Add(GraphDat(&haz5, 0, 500, 0, 0.2, tag + "Hazard 5ms", 1, 5, colour + shift), reftag + "haz5ms");
	graphbase->Add(GraphDat(&burstdata->hist1, 0, 500, 0, 100, btag + tag + "Hist 1ms", 1, 1, colour + shift), reftag + "bursthist1ms");
	graphbase->Add(GraphDat(&burstdata->hist5, 0, 500, 0, 500, btag + tag + "Hist 5ms", 1, 5, colour + shift), reftag + "bursthist5ms");
	graphbase->Add(GraphDat(&burstdata->haz1, 0, 500, 0, 0.04, btag + tag + "Haz 1ms", 1, 1, colour + shift), reftag + "bursthaz1ms");
	graphbase->Add(GraphDat(&burstdata->haz5, 0, 500, 0, 0.2, btag + tag + "Haz 5ms", 1, 5, colour + shift), reftag + "bursthaz5ms");

	graphindex = setindex;
	//graphbase->datdex[datset] = setindex; 
	graphs = true;

	return setindex;
}

