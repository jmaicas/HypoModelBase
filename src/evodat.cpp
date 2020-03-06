

/*
*  evodat.cpp
*  
*  Created by Duncan MacGregor 
*  University of Edinburgh 2016
*
*/


#include "evodat.h"


// GA Parameter Genes

EvoParam::EvoParam()
{
	adapt = false;
}


EvoParam::EvoParam(double v, double mn, double mx, bool ad, wxString lab, int pl)
{
	value = v;
	min = mn;
	max = mx;
	adapt = ad;
	label = lab;
	places = pl;
}


void EvoParam::Generate()
{
	if(adapt) value = mrand01() * (max - min) + min;
}


// GA Parameter Chromosome (parameter set)

EvoChrome::EvoChrome()          
{
	unsigned int storesize = 512;
	unsigned int max = 1000;

	//fitdata = new FitDat();
	//fitdata2 = new FitDat();

	numparams = 0;
	maxparams = 50;
}


EvoChrome::~EvoChrome()
{
	//delete fitdata;
	//delete fitdata2;
}


int EvoChrome::AddParam(wxString tag, ParamCon *con, double min, double max, bool adapt, wxString label, int places)
{
	wxString text;
	//diagbox->Write(text.Format("chrome add: %s %s %.4f\n", tag, con->name, con->GetValue()));
	ref[tag] = numparams;
	tags[numparams] = tag;
	cons[numparams] = con;
	params[numparams] = EvoParam(con->GetValue(), min, max, adapt, label, places);
	numparams++;
	return 1;
}


int EvoChrome::AddParam(wxString tag, ParamCon *con, EvoParam *fp)
{
	wxString text;
	//diagbox->Write(text.Format("chrome add: %s %s %.4f %s %.4f\n", tag, con->name, con->GetValue(), fp->label, fp->value));
	ref[tag] = numparams;
	tags[numparams] = tag;
	cons[numparams] = con;
	params[numparams] = EvoParam(fp->value, fp->min, fp->max, fp->adapt, fp->label, fp->places);
	numparams++;
	return 1;
}


void EvoChrome::Initialise()
{
	for(i=0; i<numparams; i++) params[i].Generate();
}


double EvoChrome::Val(wxString tag)
{
	return params[(int)ref[tag]].value;
}


EvoParam EvoChrome::GetParam(wxString tag)
{
	return params[(int)ref[tag]];
}


void EvoChrome::Diagnostic()
{
	wxString text;

	diagbox->Write(text.Format("\nChrome %d\n", index));
	for(i=0; i<numparams; i++) diagbox->Write(text.Format("%d %s %.2f\n", i, tags[i], params[i].value));
	diagbox->Write("\n");
}

void EvoChrome::Output(wxString filename)
{
	TextFile ofp;
	wxString text;

	ofp.Open(filename);
	ofp.WriteLine(text.Format("Chrome %d", index));
	for(i=0; i<numparams; i++)
		ofp.WriteLine(text.Format("G%d %s %.2f", i, tags[i], params[i].value));
	ofp.WriteLine(text.Format("Fitness %.6f", fitness));
	ofp.WriteLine("");
	ofp.Close();
}


void EvoChrome::Output(TextFile *ofp, int mode)
{
	wxString text;
	wxString outline;

	if(mode == 0) {
		ofp->WriteLine(text.Format("Chrome %d", index));
		for(i=0; i<numparams; i++) ofp->WriteLine(text.Format("g%d %.2f", i, params[i].value));
		ofp->WriteLine(text.Format("Fitness %.10f", fitness));
		ofp->WriteLine("");
	}

	if(mode == 1) {
		outline = "";
		for(i=0; i<numparams; i++) if(params[i].adapt) outline += text.Format("%.2f\t", params[i].value);
		outline += text.Format("%.2f\t", fitness);
		//outline += text.Format("%.2f\t%.2f\t%.2f\t%.2f\t", fithead, fittail, fithaz, fithazhead, fitIoD);
		outline += text.Format("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t", fitdata.RMSFirstNBins, fitdata.RMSBinRange, fitdata.RMSHaz, fitdata.RMSHazHead, fitdata.RMSIoD);
		//outline += text.Format(" %d", index);
		ofp->WriteLine(outline);
	}

	if(mode == 2) {
		outline = "";
		for(i=0; i<numparams; i++) if(params[i].adapt) outline += numtext(params[i].value, params[i].places) + "\t";
		outline += text.Format("%.2f\t", fitness);
		//outline += text.Format("%.2f\t%.2f\t%.2f\t%.2f\t", fithead, fittail, fithaz, fitIoD);
		outline += text.Format("%.2f\t%.2f\t%.2f\t%.2f\t", fitdata.RMSFirstNBins, fitdata.RMSBinRange, fitdata.RMSHaz, fitdata.RMSBurstHead);
		outline += text.Format("%.2f\t%.2f\t%.2f\t%.2f\t", fitdata.burstmode, fitdata.burstlengthmean, fitdata.burstlengthsd, fitdata.burstsilencemean);
		outline += text.Format("%.2f\t%.2f\t%.2f", fitdata.burstsilencesd, fitdata.burstintrafreq, fitdata.RMSBurstIoD);
		//outline += text.Format(" %d", index);
		ofp->WriteLine(outline);
	}
}


wxString EvoChrome::OutLine()
{
	wxString text, outline;

	outline = "";
	//for(i=0; i<numparams; i++) if(params[i].adapt) outline += text.Format("%.3f\t", params[i].value);
	for(i=0; i<numparams; i++) outline += text.Format("%s %.5f\n", tags[i], params[i].value);
	//outline += text.Format("%.3f", fitness);
	return outline;
}