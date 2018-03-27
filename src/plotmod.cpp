/*
 *  plotmod.cpp
 *  HypoModel
 *
 *  Created by Duncan MacGregor.
 *
 */


#include "plotmodel.h"


PlotMod::PlotMod(PlotModel *plotmodel)
: ModThread(plotmodel->modbox, plotmodel->mainwin) 
{
	mod = plotmodel;
	plotbox = mod->plotbox;
	plotdata = mod->plotdata;	
	mainwin = mod->mainwin;
	scalebox = mod->mainwin->scalebox;
}


void *PlotMod::Entry()
{
	if(mainwin->diagnostic) mainwin->SetStatusText("running PlotMod");
	
	initialise();
	
	if(mainwin->diagnostic) mainwin->SetStatusText("PlotMod Initialised");
	
	if(randomflag) init_mrand(0);
	else init_mrand((unsigned)(time(NULL)));
	
	plotmodel();
	
	scalebox->SetMod(mod);
	scalebox->GraphUpdate();

	return NULL;
}
		
		
void PlotMod::initialise()
{
	ParamStore *plotparams = plotbox->GetParams();

	//runtime = 1000;
	runtime = int((*plotparams)["runtime"]);
	waterloss = (*plotparams)["waterloss"];
}
	

void PlotMod::plotmodel()
{
	int i;

	// Initialise variables
	water = 2000;
	salt = 50;
	osmo = water/salt;
	heat = 10;

  plotdata->water[0] = water;
	plotdata->salt[0] = salt;
	plotdata->osmo[0] = osmo;
  plotdata->heat[0] = heat;
	plotbox->countmark = 0;

	// Run model loop
	for(i=1; i<=runtime; i++) {
		if(i%100 == 0) plotbox->SetCount(i*100/runtime);     // Update run progress % in model panel

		water = water - (water * waterloss);
		//salt = salt;
		osmo = water/salt;
		//heat = heat;

		// Record model variables
		plotdata->water[i] = water;
	  plotdata->salt[i] = salt;
	  plotdata->osmo[i] = osmo;
		plotdata->heat[i] = heat;
	}
}
		
		

