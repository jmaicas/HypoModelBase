
#include <hypodef.h>
#include <hypomodel.h>


#ifdef CORTMOD
#include <cortmod.h>
#endif

#ifdef OSMOMOD
#include <osmomod.h>
#endif

#ifdef IGFMOD
#include <igfmod.h>
#endif

#ifdef VASOMOD
#include <vasomod.h>
#endif

#ifdef HEATMOD
#include <heatmod.h>
#endif

#ifdef GHMOD
#include <ghmod.h>
#endif

#ifdef LYSISMOD
#include <lysismod.h>
#endif

#ifdef VMNMOD
#include <vmnmodel.h>
#endif

#ifdef AGENTMOD
#include <agentmodel.h>
#endif

#ifdef TESTMOD
#include <testmodel.h>
#endif


void HypoMain::ModInit()
{
	#ifdef LYSISMOD
	if(startmod == modLysis) {
		mod = new LysisModel(startmod, "lysismod", this);
		scaletype = 1;
	}
	modset.Add(modLysis, "Analysis Module");
	#endif
	
	#ifdef CORTMOD
	if(startmod == modCort) {
		mod = new CortModel(modCort, "Corticotroph Model", this);
		scaletype = 1;		
	}
	modrad[moddex++] = new wxRadioButton(panel, modCort, "Corticotroph");
	#endif

	#ifdef OSMOMOD
	if(startmod == modOsmo) {
		mod = new OsmoModel(modOsmo, "Osmotic System Model", this);
		scaletype = 1;		
	}
	modset.Add(modOsmo, "Osmotic System");
	#endif

	#ifdef HEATMOD
	if(startmod == modHeat) {
		mod = new HeatModel(modHeat, "Heat System Model", this);
		scaletype = modHeat;		
	}
	modset.Add(modHeat, "Heat System");
	#endif
    
	#ifdef GHMOD
 	 if(startmod == modGH) {
		mod = new GHModel(modGH, "ghmod", this);
		scaletype = 1;
	}
	modset.Add(modGH, "GH Model");
	#endif
	
	#ifdef IGFMOD
	if(startmod == modIGF) {
		mod = new IGFModel(modIGF, "IGF Model", this);
		scaletype = 0;
	}
	//moddata[moddex++].Set(modIGF, "IGF Model");
	modset.Add(modIGF, "IGF Model");
	#endif
	
	#ifdef VASOMOD
	if(startmod == modVaso) {
		mod = new VasoModel(modVaso, "vasomod", this);
		scaletype = 0;
	}
	modset.Add(modVaso, "Vasopressin");
	#endif

	#ifdef VMNMOD
	if(startmod == modVMN) {
		mod = new VMNModel(modVMN, "vmnmod", this);
		scaletype = modVMN;
	}
	modset.Add(modVMN, "VMN Network");
	#endif

	#ifdef AGENTMOD
	if(startmod == modAgent) {
		mod = new AgentModel(modAgent, "agentmod", this);
		scaletype = 1;
	}
	modset.Add(modAgent, "Agent Model");
	#endif

	#ifdef TESTMOD
	if(startmod == modTest) {
		mod = new TestModel(modTest, "Test", this);
		scaletype = 1;
	}
	modset.Add(modTest, "Test Model");
	#endif
}

