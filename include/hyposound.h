


#include "wx/wx.h"
#include "hypodat.h"
#include "hypopanels.h"

#include "RtWvOut.h"
#include "FileWvOut.h"

using namespace stk;


class SoundBox : public ParamBox
{
public:
	SpikeDat *spikedata;
	datdouble *wavedata;
	//VasoModel *vasomod;
	wxStaticText *numspikes;
	wxMutex *soundmutex;
	unsigned int soundon;
	Model *mod;

	SoundBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, SpikeDat *sdat=NULL);
	void SoundTest();
	void SpikeSonic();
	void OnGo(wxCommandEvent& event);
	void OnSpikes(wxCommandEvent& event);
	void OnWave(wxCommandEvent& event);
	void OnStop(wxCommandEvent& event);
	void DataLink(SpikeDat *spikedata, datdouble *wavedata=NULL);
};


class SoundGen : public wxThread
{
public:
	SpikeDat *spikedata;
	datdouble *wavedata;
	ParamStore *params;
	SoundBox *soundbox;
	RtWvOut *dac;
	FileWvOut outfile;
	int spikemode;
	int selectmode;

	int msamp;
	int pulseint;
	double freqscale;
	int freq;
	int fsamp;
	int pulsedur;
	double sdur;
	int playspikes;
    int timerate;

	SoundGen(SpikeDat *, ParamStore *, SoundBox *);
	SoundGen(datdouble *, ParamStore *, SoundBox *);
	void PlaySpikes();
	void PlayWave();
	virtual void *Entry();
};
