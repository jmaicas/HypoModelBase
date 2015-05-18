// *************************************************************************************************
// *********************************      hypobase.h             ***********************************
//
//	Classes included:
//	1. MainFrame. Define the Main Window, its size, position, OS, header text
//	2. TextFile. Interacting with a Text File. 
//	3. datdouble. Storage a vector of double overloading operator []
//	4. datint. Storage a vector of integer overloading operator []
//	5. ParamStore. Storage of struct of parameters ?
//	6. NewToolbar. Derived from wxToolBar. Define a new Toolbar based on the standard one. 
//
// *************************************************************************************************

#ifndef HYPOBASE_H
#define HYPOBASE_H


/*
#ifdef _DEBUG
      inline void * __cdecl operator new(unsigned int size, 
                                         const char *file, int line)
      {
      };

      inline void __cdecl operator delete(void *p)
      {
      };
#endif

#ifdef _DEBUG
      #define DEBUG_NEW new(__FILE__, __LINE__)
      #else
      #define DEBUG_NEW new
      #endif
      #define new DEBUG_NEW



      #ifdef _DEBUG
      inline void * __cdecl operator new(unsigned int size,
                                         const char *file, int line)
      {
	      void *ptr = (void *)malloc(size);
	      AddTrack((DWORD)ptr, size, file, line);
	      return(ptr);
      };
      inline void __cdecl operator delete(void *p)
      {
	      RemoveTrack((DWORD)p);
	      free(p);
      };
      #endif
 */


#include "wx/wx.h"

#include "wx/spinctrl.h"
#include "wx/scrolwin.h"
#include <vector>
#include <list>
#include "mersenne.h"
#include "wx/dcbuffer.h"
#include "wx/msgqueue.h"
#include <wx/textfile.h>

//#include "hypocontrols.h"
//#include "hypograph.h"

//#include "hypopanels.h"

//#include "spinctlg.h"
//#include "hypomods2.h"

//#include "RtWvOut.h"
//#include "FileWvOut.h"
#include <cstdlib>

//#define _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAP_ALLOC_NEW
//#include <stdlib.h>
//#include <crtdbg.h>
//#include <vld.h>


//using namespace stk;


/*
typedef struct {
	      DWORD	address;
	      DWORD	size;
	      char	file[64];
	      DWORD	line;
      } ALLOC_INFO;

      typedef list<ALLOC_INFO*> AllocList;

      AllocList *allocList;

      void AddTrack(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum)
      {
	      ALLOC_INFO *info;

	      if(!allocList) {
		      allocList = new(AllocList);
	      }

	      info = new(ALLOC_INFO);
	      info->address = addr;
	      strncpy(info->file, fname, 63);
	      info->line = lnum;
	      info->size = asize;
	      allocList->insert(allocList->begin(), info);
      };

      void RemoveTrack(DWORD addr)
      {
	      AllocList::iterator i;

	      if(!allocList)
		      return;
	      for(i = allocList->begin(); i != allocList->end(); i++)
	      {
		      if((*i)->address == addr)
		      {
			      allocList->remove((*i));
			      break;
		      }
	      }
      };
*/

enum {
	spincon = 1,
	numcon = 2,
	textcon = 3
};


enum {
	Windows = 1,
	Mac = 2,
	Linux = 3
};


enum {                 // IGF = 1   Vaso = 2   VMN = 3   Cort = 4
	igf_mod = 1,
	vaso_mod = 2,
	vmn_mod = 3,
	cort_mod = 4,
	exp_dat = 5,
	blank = 6
};


enum {
	vProto = 1,
	vTom = 2,
	vABC = 3,
	vIGF = 4
};


enum {
	none = 0,
	synaptic = 1,
	antidromic = 2,
	ramp = 3,
	osmoinject = 4,
	synrate = 5,
	secpulse = 6,
	secfreq = 7,
	pulse = 8,
	range = 9
};


enum {
	ID_Quit = 5,
	ID_About,
	ID_Output,
	ID_OK,
	ID_Params,
	ID_IGFMod,
	ID_VMNMod,
	ID_VasoMod,
	ID_CortMod,
  ID_GHMod,
	ID_LysisMod,
	ID_AgentMod,
	ID_Run,
	ID_Reset,
	ID_BurstBox,
	ID_Info, 
	ID_Burst,
	ID_Sec,
	ID_autorun,
	ID_revpots,
	ID_histhaz1,
	ID_histhaz2, 
	ID_binres1,
	ID_binres2,
	ID_overlay,
	ID_position,
	ID_sum,
	ID_spikes,
	ID_rateres,
	ID_pararun,
	ID_vasomodv1,
	ID_vasomodv2,
	ID_vasomodv3,
	ID_simple,
	ID_chloride,
	ID_voltcl,
	ID_kir,
	ID_kir2,
	ID_acurrent,
	ID_acurrent2,
	ID_bk,
	ID_bknear,
	ID_bkfar,
	ID_nsna,
	ID_scan,
	ID_allburst, 
	ID_osmo,
	ID_netcheck,
	ID_cellcheck,
	ID_unicheck,
	ID_netstore,
	ID_netload,
	ID_output,
	ID_memnoise,
	ID_IGFPanel,
	ID_VMHPanel,
	ID_VasoPanel,
	ID_CortPanel,
	ID_BlankPanel,
	ID_OsmoPanel,
	ID_HeatPanel,
	ID_Options, 
	ID_synqueue,
	ID_paramstore,
	ID_paramload,
	ID_pin,
	ID_arun, 
	ID_tool,
	ID_params,
	ID_model,
	ID_controls,
	ID_MAIN_TOOLBAR,
	ID_loaddata,
	ID_datstore,
	ID_datload,
	ID_datoutput,
	ID_Icap,
	ID_Idrift,
	ID_Display,
	ID_actI,
	ID_DAPcap,
	ID_IA,
	ID_vsyn,
	ID_runtime,
	ID_DAP,
	ID_DAP2,
	ID_Dyno,
	ID_vDAP,
	ID_vIKleak,
	ID_Iratio,
	ID_artspikes,
	ID_profile,
	ID_Protocol,
	ID_Signal,
	ID_Ramp,
	ID_Pulse,
	ID_Range,
	ID_antispikes,
	ID_secretion,
	ID_AHPact,
	ID_Random,
	ID_expdat,
	ID_DendMod,
	ID_NeuroGen,
	ID_Generate,
	ID_Parallel,
	ID_protorun,
	ID_Sound,
	ID_Graph,
	ID_GraphAdd,
	ID_GraphRemove,
	ID_Go,
	ID_Stop, 
	ID_Wave,
	ID_OsmoMod,
	ID_Zero,
	ID_inputgen,
	ID_Input,
	ID_data,
	ID_intern,
	ID_dendmode,
	ID_inputramp,
	ID_osmoinput,
	ID_neurospin,
	ID_bundlespin,
	ID_dynostore,
	ID_inject,
	ID_HeatMod,
	ID_recep,
	ID_vasoinject,
	ID_Sync,
  ID_gload,
  ID_gstore,
	ID_DataBrowse,
	ID_OutputBrowse,
	ID_ParamBrowse,
	ID_ModBrowse,
	ID_absref,
	ID_SelectAll,
	ID_Copy,
	ID_Paste,
	ID_Undo,
	ID_Bold,
	ID_Italic,
	ID_Cut,
	ID_Default,
	ID_ModGen,
	ID_Diag,
	ID_s,
	ID_ms,
	ID_net
};


enum {
	black = 0,
	red = 1,
	green = 2,
	blue = 3,
	yellow = 4,
	purple = 5,
	lightred = 6,
	lightgreen = 7,
	lightblue = 8
};



//void quicksort(refstore *a, int low, int high)
wxString numstring(double, int);
int numplaces(double);
wxString numchar(int);
wxString numtext(double number, int places);

int GetSystem();

class MainFrame;
class CortMod;
class BurstBox;
class BurstDat;
class InfoBox;
class ScaleBox;
class ToolBox;
class GraphBase;
class ModGenBox;
class DiagBox;
class ToolSet;



//	1. MainFrame. Define the Main Window, its size, position, OS, header text
class MainFrame: public wxFrame
{
public:
	MainFrame *mainwin;
	
	wxFrame *frame;
	wxPoint mainpos;
	wxSize screensize;
	wxString snum, text;
	wxBoxSizer *mainsizer;
	//ToolSet toolset;
	ToolSet *toolset;
  DiagBox *diagbox;
    
	int basic;
	int diagnostic;
	int ostype;
    
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	virtual void MainLoad();
	virtual void MainStore();
};


class sortpair{
public:
	int index;
	double value;
};


//	2. TextFile. Interacting with a Text File. 
class TextFile{
public:
	wxTextFile *file;
	bool unread;
	bool readonly;
    
	//TextFile(wxString name);
	TextFile();
	~TextFile();

	void New(wxString name);
	int Open(wxString name, bool read=true);
	int Exists(wxString name);
	void WriteLine(wxString);
	void Flush();
	wxString ReadLine();
	void Close();
	bool End();
};

long ReadNextData(wxString *readline);


//	3. datdouble. Storage a vector of double overloading operator []
class datdouble{
public:
	datdouble(wxTextCtrl *text = NULL);
	datdouble(int size);  //// OJO AQUI, ESTABA EN INTEGER. 
	std::vector <double> data;
	double count;
	int max;
	double zero;
	wxString tag, mess;
	wxTextCtrl *textbox;
	
	double &operator [](int index) {
		if(index < 0) index = 0;
		//if(index > max) return zero;
		if(index >= data.size()) {
			if(textbox && index%100 == 0) textbox->AppendText(mess.Format("%s bad access, index %d\n", tag, index));
			return zero;		
		}
		return data[index];
	}
    
	void setsize(int size, wxTextCtrl *text = NULL, wxString dattag = "") {
		textbox = text;
		tag = dattag;
		data.resize(size * 1.1);
		max = size;
	}

	//~datdouble() {
	//	delete data;
	//}
};

//	4. datint. Storage a vector of integer overloading operator []
class datint{
public:
	datint();
	std::vector<int> data;
	int count;
	int max;
	int zero;
	
	int &operator [](int index){
		if(index < 0) index = 0;
		if(index >= data.size()) return zero;
		return data[index];
	}
    
	void setsize(int size) {
		data.resize(size * 1.1);
		max = size;
	}
};


double *initfarray(int size);

//	5. ParamStore. Storage of struct of parameters ?
class ParamStore{
	struct ParamData{
		wxString indexName;
		double data;
	};
private:
	std::vector<ParamData> store;
	
public:
	unsigned long size() {
		return store.size();
	}
	void add(wxString indexName, double data){
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].indexName == indexName) {
				store[i].data = data;
				return;
			}                  
		ParamData param;
		param.indexName = indexName;
		param.data = data;
		store.push_back(param);
	}
	double &operator [](long index){
		if(index < 0) index=0;
		return store[index].data;
	}
	double &operator [](wxString indexName){
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].indexName == indexName) {
				return store[i].data;
			}
		long idx = store.size();
		ParamData param;
		param.indexName = indexName;
		store.push_back(param);  
		return store[idx].data;             
	} 
	int check(wxString indexName) {
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].indexName == indexName) return 1;
		return 0;
	}
};


class RefEntry{
public:
	int id;
	wxString label;
	wxControl *box;
};


class RefStore{
	int i;
	
public:
	int numrefs;
	RefEntry refbase[100];
	
	RefStore() {
		numrefs = 0;
	};
	
	wxString GetRef(int id) {
		for(i=0; i<numrefs; i++)
			if(refbase[i].id == id) return refbase[i].label;
		return "";
	};
    
	wxControl *GetBox(int id) {
		for(i=0; i<numrefs; i++)
			if(refbase[i].id == id) return refbase[i].box;
		return NULL;
	};
	
	int GetID(wxString label) {
		for(i=0; i<numrefs; i++)
			if(refbase[i].label == label) return refbase[i].id;
		return 0;
	};
	
	void AddRef(int id, wxString label, wxControl *box = NULL) {
		refbase[numrefs].id = id;
		refbase[numrefs].label = label;
		refbase[numrefs].box = box;
		numrefs++;
	};
};


class ParamRef{
public:
	int numparams;
	wxString paramname[100];
	double scale[100];
	int prop[100];
	
	ParamRef() {
		numparams = 0;
	};
	void AddRef(wxString pname, double pscale, int pprop=0) {
		paramname[numparams] = pname;
		scale[numparams] = pscale;
		prop[numparams] = pprop;
		numparams++;
	};
};



// Interface

//	6. NewToolbar. Derived from wxToolBar. Define a new Toolbar based on the standard one. 
class NewToolBar: public wxToolBar
{
public:
	NewToolBar(wxWindow *parent,
			   wxWindowID id,
			   const wxPoint& pos = wxDefaultPosition,
			   const wxSize& size = wxDefaultSize,
			   long style = wxNO_BORDER | wxTB_HORIZONTAL,
			   const wxString& name = wxToolBarNameStr);
	bool Realize();
	void AdjustToolBitmapSize();
};



// Processing

double gaussian(double mean, double sd);
double uniform(double mean, double range);
int iuniform(int base, int range);
int randint(int range);




#endif