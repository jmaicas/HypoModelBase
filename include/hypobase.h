
#ifndef HYPOBASE_H
#define HYPOBASE_H


#include "wx/wx.h"

#include "wx/spinctrl.h"
#include "wx/scrolwin.h"
#include <vector>
#include <list>
#include <mersenne.h>
#include "wx/dcbuffer.h"
#include "wx/msgqueue.h"
#include <wx/textfile.h>
//#include <hypodef.h>


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

#ifndef OSXClip
#ifndef TEACH
#ifdef _DEBUG
//#include <vld.h>
#endif
#endif
#endif


using namespace std;
//using namespace stk



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
	ID_seedcheck,
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
	ID_GraphPrint,
	ID_Print,
	ID_GraphEPS,
	ID_Scale,
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
	ID_PathBrowse,
	ID_FileBrowse,
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
	ID_net, 
	ID_norm,
	ID_clipmode,
	ID_scatter,
	ID_line,
	ID_Plot,
	ID_Load,
	ID_Browse,
	ID_Select,
	ID_Bin,
	ID_Compare,
	ID_Store,
	ID_Wheel,
	ID_Neuron,
	ID_FileIO
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
	lightblue = 8,
	custom = 9
};



//void quicksort(refstore *a, int low, int high)
wxString numstring(double, int);
int numplaces(double);
wxString numchar(int);
wxString numtext(double number, int places);
wxString ColourString(wxColour colour, int type=0);
long ParseLong(wxString *, wxUniChar);
double ParseDouble(wxString *, wxUniChar);
wxString ParseString(wxString *, wxUniChar);

int GetSystem();

class MainFrame;
class CortMod;
class BurstBox;
class BurstDat;
class InfoBox;
class ScaleBox;
class GraphBox;
class GraphWindow3;
class ToolBox;
class GraphBase;
class ModGenBox;
class DiagBox;
class ToolSet;


//DiagBox *gdiag;


class BoxOut
{
public:
	wxStaticText *status;
	DiagBox *diagbox;
	wxString name;

	BoxOut() {status = NULL; diagbox = NULL; name = "";}
	BoxOut(wxStaticText *statustext, DiagBox *diag, wxString boxname) {status = statustext; diagbox =diag; name = boxname;}
};



class MainFrame: public wxFrame
{
public:
	MainFrame *mainwin;
	
	wxFrame *frame;
	wxPoint mainpos;
	wxSize screensize;
	wxString snum, text;
	wxBoxSizer *mainsizer;
	wxStatusBar *statusbar;
	//ToolSet toolset;
	ToolSet *toolset;
  DiagBox *diagbox;
	wxColour colourpen[10];

	// Display
	ScaleBox *scalebox;
	GraphBox *graphbox;
	GraphWindow3 *graphwin[10];
    
	int basic;
	int diagnostic;
	int ostype;
    
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MainFrame();
	virtual void MainLoad();
	virtual void MainStore();
};


class sortpair{
public:
	int index;
	double value;
};


// Text File 

class TextFile{
public:
	wxTextFile *file;
	wxString txt;
	bool unread;
	bool readonly;
	bool buffmode;
    
	//TextFile(wxString name);
	TextFile();
	~TextFile();

	void New(wxString name);
	int Open(wxString name, bool read=true);
	int Exists(wxString name);
	void WriteLine(wxString);

	// Postscript Writing
	void MoveTo(double x, double y);
	void LineTo(double x, double y);
	void DrawLine(double xf, double yf, double xt, double yt);
	void DrawText(wxString, double x, double y);
	void SetColour(wxString);

	void Flush();
	wxString ReadLine();
	void Close();
	bool End();
};

long ReadNextData(wxString *readline);


// Fast File I/O

/*
string ReadFile(const char *filename)
{
  ifstream infile(filename, std::ios::in | std::ios::binary);
  if(infile) {
    string contents;
    infile.seekg(0, ios::end);
    contents.resize(infile.tellg());
    infile.seekg(0, std::ios::beg);
    infile.read(&contents[0], contents.size());
    infile.close();
    return(contents);
  }
}*/


// Storage

class datdouble{
public:
	datdouble(wxTextCtrl *text = NULL);
	datdouble(int size);
	std::vector <double> data;
	double count;
	int max, maxindex;
	double zero;
	wxString tag, mess;
	wxTextCtrl *textbox;
	bool stretchmode;
	
	double &operator [](int index) {
		if(index < 0) index = 0;
		//if(index > max) return zero;
		if(index >= data.size()) {
			if(stretchmode) data.resize(data.size() + 100);
			if(textbox && index%100 == 0) textbox->AppendText(mess.Format("%s bad access, index %d\n", tag, index));
			return zero;		
		}
		if(index > maxindex) maxindex = index;
		return data[index];
	}

	int maxdex() {
		//return data.size()-1;
		return maxindex;
	}
    
	void setsize(int size, wxTextCtrl *text = NULL, wxString dattag = "") {
		textbox = text;
		tag = dattag;
		data.resize(size * 1.1);
		max = size;
		maxindex = 0;
		stretchmode = false;
	}

	void setsize(int size, bool stretch) {
		data.resize(size * 1.1);
		max = size;
		maxindex = 0;
		stretchmode = stretch;
	}
};


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



class DatStore{
	struct DatData{
		wxString tag;
		datdouble data;
		int xscale;
	};
private:
	vector<DatData> store;

public:
	unsigned long size() {
		return store.size();
	}
	void add(wxString tag, int xscale, int size) {
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].tag == tag) return;                 
		DatData dat;
		dat.tag = tag;
		dat.xscale = xscale;
		dat.data.setsize(size);
		store.push_back(dat);
	}
	datdouble *operator[](long index) {
		if(index < 0 || index >= store.size()) return NULL;
		return &store[index].data;
	}
	datdouble *operator[](wxString tag){
		for(unsigned long i=0; i<store.size(); i++) 
			if(store[i].tag == tag) {
				return &store[i].data;
			}
		return NULL;
	}  
};


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
	ToolBox *tool;
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

	ToolBox *GetTool(int id) {
		for(i=0; i<numrefs; i++)
			if(refbase[i].id == id) return refbase[i].tool;
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

	void AddTool(int id, ToolBox *tool) {
		refbase[numrefs].id = id;
		refbase[numrefs].tool = tool;
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
float fast_tanh(float x);
double kapow(double base, int power);




#endif