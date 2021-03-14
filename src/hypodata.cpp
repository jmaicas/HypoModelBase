

#include "wx/wx.h"
#include <hypomodel.h>
#include "wx/grid.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//using namespace std;

#include <fcntl.h>
//#include <unistd.h>
#include <sys/types.h>
//#include <sys/mman.h>
#include <sys/stat.h>

#include <wx/numformatter.h>



SpikePanel::SpikePanel(NeuroBox *box)
	: ToolPanel(box, box->auitabpanel)
{
	int datwidth, labelwidth, buttspace;

	neurobox = box;

	selectcount = 2;
	neuroindex = 0;
	neurocount = 0;
	cellmode = 0;

	ratetag = "";
	graphwin = NULL;

	selectspikes[0].resize(100000);
	selectspikes[1].resize(100000);

	for(i=0; i<100000; i++) {
		selectspikes[0][i] = 0;
		selectspikes[1][i] = 0;
	}

	this->SetFont(neurobox->boxfont);
	wxBoxSizer *selectsizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(selectsizer);
	neurobox->activepanel = this;
	neurobox->paramset.panel = this;

	/////////////////////////////////////////////////////////
	// Neuron selection
	//
	datwidth = 50;
	labelwidth = 70;
	label = neurobox->NumPanel(labelwidth, wxALIGN_CENTRE);
	spikes = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);
	freq = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);
	selectspikecount = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);
	selectfreq = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);

	wxGridSizer *datagrid = new wxGridSizer(2, 5, 5);
	datagrid->Add(new wxStaticText(this, -1, "Name"), 0, wxALIGN_CENTRE);
	datagrid->Add(label);
	datagrid->Add(new wxStaticText(this, -1, "Spikes"), 0, wxALIGN_CENTRE);
	datagrid->Add(spikes);
	datagrid->Add(new wxStaticText(this, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(freq);
	datagrid->Add(new wxStaticText(this, -1, "Select Spikes"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(selectspikecount);
	datagrid->Add(new wxStaticText(this, -1, "Select Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(selectfreq);

	// need to fix this for multiple use
	filtercheck = neurobox->SetBoxCheck(ID_filtercheck, "cellfilter", "Filter", false);
	
	datneuron = new wxTextCtrl(this, ID_Neuron, "---", wxDefaultPosition, wxSize(50, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE|wxTE_PROCESS_ENTER);
	if(neurobox->ostype == Mac) 
		datspin = new wxSpinButton(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	else 
		datspin = new wxSpinButton(this, wxID_ANY, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	datspin->SetRange(-1000000, 1000000);

	wxBoxSizer *datbox = new wxBoxSizer(wxHORIZONTAL);
	datbox->Add(datspin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	datbox->AddSpacer(5);

	wxBoxSizer *cellbox = new wxBoxSizer(wxHORIZONTAL);
	cellbox->Add(new wxStaticText(this, wxID_ANY, "Neuron"), 1, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	cellbox->Add(datneuron, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, this, "");
	databox->AddSpacer(2);
	databox->Add(cellbox, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 5);
	databox->AddSpacer(5);
	databox->Add(datbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
	databox->AddSpacer(5);
	databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	databox->AddSpacer(5);
	databox->Add(filtercheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);

	/////////////////////////////////////////////////////////
	// Spike selection
	//
	fromcon = neurobox->paramset.AddNum("from", "From", 0, 0, 30); 
	tocon = neurobox->paramset.AddNum("to", "To", 100, 0, 20); 

	wxBoxSizer *selectpanelbox = new wxBoxSizer(wxVERTICAL);
	selectbox[0] = new wxStaticBoxSizer(wxHORIZONTAL, this, "Selection 1");
	selectbox[1] = new wxStaticBoxSizer(wxHORIZONTAL, this, "Selection 2");
	wxBoxSizer *fromtobox = new wxBoxSizer(wxHORIZONTAL);

	fromtobox->Add(fromcon, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	fromtobox->Add(tocon, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);

	buttspace = 20;
	for(i=0; i<selectcount; i++) {
		addbutton[i] = ToggleButton(100 + i, "Add", 40, selectbox[i]);	
		selectbox[i]->AddSpacer(buttspace);
		subbutton[i] = ToggleButton(200 + i, "Sub", 40, selectbox[i]);	
		selectbox[i]->AddSpacer(buttspace);
		neurobox->AddButton(300 + i, "Clear", 40, selectbox[i]);
		selectbox[i]->AddSpacer(buttspace);
		neurobox->AddButton(400 + i, "Invert", 40, selectbox[i]);
		selectmode[i] = 1;
	}

	currselect = 0;
	addbutton[currselect]->SetValue(true);

	selectpanelbox->Add(fromtobox, 0, wxALIGN_CENTRE_HORIZONTAL);
	selectpanelbox->AddSpacer(15);
	selectpanelbox->Add(selectbox[0], 0);
	selectpanelbox->AddSpacer(10);
	selectpanelbox->Add(selectbox[1], 0);
	
	wxBoxSizer *colbox2 = new wxBoxSizer(wxHORIZONTAL); 
	colbox2->AddStretchSpacer();
	colbox2->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	colbox2->AddSpacer(30);
	//colbox2->AddStretchSpacer();
	colbox2->Add(selectpanelbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	selectsizer->AddStretchSpacer();
	selectsizer->Add(colbox2, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	selectsizer->AddStretchSpacer();
	this->Layout();

	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SpikePanel::OnEnter));
	Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(SpikePanel::OnNext));
	Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(SpikePanel::OnPrev));
	Connect(300, 305, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SpikePanel::OnClear));
	Connect(400, 405, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SpikePanel::OnInvert));
}


void SpikePanel::SetData(SpikeDat *dataneuron, std::vector<NeuroDat>*dataneurons)
{
	currneuron = dataneuron;
	neurons = dataneurons;

	currneuron->SelectInit();
	currneuron->dispmodemax = 2;
	currneuron->diagbox = neurobox->diagbox;
}


void SpikePanel::PanelData(NeuroDat *data)
{
	if(!data) data = &(*neurons)[neuroindex];
	if(data->netflag) snum = "sum";
	else snum = numstring(neuroindex, 0);
	datneuron->SetLabel(snum);

	label->SetLabel(data->name);
	spikes->SetLabel(snum.Format("%d", data->spikecount));
	freq->SetLabel(snum.Format("%.2f", data->freq));
	selectspikecount->SetLabel(snum.Format("%d", currneuron->selectdata->intraspikes));
	selectfreq->SetLabel(snum.Format("%.2f", currneuron->selectdata->freq));
}


void SpikePanel::OnPrev(wxSpinEvent& WXUNUSED(event))
{
	if(!neurocount) return;
	if(neuroindex > 0) neuroindex--;
	else neuroindex = neurocount-1;

	// Store rate X-axis position
	graphwin = neurobox->mod->GetGraphWin(ratetag);
	if(graphwin) currneuron->neurodata->xscrollpos = graphwin->xscrollpos;

	// Store select grids
	for(int i=0; i<selectcount; i++) {
		currneuron->selectdata->spikes = selectspikes[i].data();
		currneuron->SelectScan(i);  // store current cell's select grid to NeuroDat
	}
	NeuroData();
}


void SpikePanel::OnNext(wxSpinEvent& WXUNUSED(event))
{
	NextCell();
}


void SpikePanel::NextCell()
{
	if(!neurocount) return;
	if(neuroindex < neurocount-1) neuroindex++;
	else neuroindex = 0;

	//neurobox->mainwin->graphwin[0]->SetScroll(0); 

	// Store rate X-axis position
	graphwin = neurobox->mod->GetGraphWin(ratetag);
	if(graphwin) currneuron->neurodata->xscrollpos = graphwin->xscrollpos;

	// Store select grids
	for(int i=0; i<selectcount; i++) {
		currneuron->selectdata->spikes = selectspikes[i].data();
		currneuron->SelectScan(i);  // store current cell's select grid to NeuroDat
	}

	NeuroData();
}


void SpikePanel::OnEnter(wxCommandEvent& event)
{
	int id = event.GetId();
	long data;

	// Enter pressed for neuron selection
	if(id == ID_Neuron) {
		datneuron->GetValue().ToLong(&data);
		if(data >= 0 && data < neurocount) {
			neuroindex = data;
			NeuroData();
		}
		return;
	}
	else NeuroData();
}


void SpikePanel::NeuroData(bool dispupdate)
{	
	ParamStore *calcparams = neurobox->GetParams();

	currneuron->normscale = (*calcparams)["normscale"];
	currneuron->binoffset = (*calcparams)["binoffset"];

	currneuron->neurocalc(&(*neurons)[neuroindex]);
	currneuron->id = neuroindex;
	currneuron->name = (*neurons)[neuroindex].name;

#ifdef HYPOSOUND
	if(mainwin->soundbox) mainwin->soundbox->DataLink(currneuron);
#endif

	for(i=0; i<selectcount; i++) {
		currneuron->selectdata->spikes = selectspikes[i].data();
		currneuron->SelectSpikes(i);  // store current cell's select grid to NeuroDat
	}
	currneuron->selectdata->spikes = selectspikes[currselect].data();
	AnalyseSelection();

	if(cellmode) {
		if(neurobox->burstbox) {
			neurobox->diagbox->Write(text.Format("SpikePanel NeuroData() numselects %d\n", currneuron->neurodata->numselects[currselect]));
			neurobox->burstbox->ExpDataScan(currneuron);
			//burstbox->SetExpGrid();
		}
		neurobox->mod->SpikeDataSwitch(currneuron);   // Switch evo fitting data source
	}

	// Restore rate plot scroll position
	if(graphwin) graphwin->ScrollX(currneuron->neurodata->xscrollpos);

	if(dispupdate) {
		PanelData();
		mainwin->scalebox->GraphUpdate();	
	}

	neurobox->SetCheck(filtercheck, (*neurons)[neuroindex].filter);

	neurobox->diagbox->Write(text.Format("NeuroData cell %d gridcol %d\n", neuroindex, (*neurons)[neuroindex].gridcol));
	if(cellmode) {
		neurobox->gridbox->textgrid[0]->SelectCol((*neurons)[neuroindex].gridcol);
		neurobox->gridbox->textgrid[0]->MakeCellVisible(0, (*neurons)[neuroindex].gridcol);
	}

	//mod->DataOutput();
}


// OnToggle, from ToolBox, used here for Add/Sub toggle buttons
void SpikePanel::OnToggle(wxCommandEvent& event)      
{
	int sel, type, i;
	wxString text;
	int id = event.GetId();

	type = id / 100;
	sel = id % 100;

	AddSubToggle(sel, type);
	SelectUpdate();
}


// AddSubToggle clears the Add and Sub toggle buttons for each select and sets the specified button
void SpikePanel::AddSubToggle(int sel, int type)      
{
	int i;
	wxString text;

	for(i=0; i<selectcount; i++) {
		addbutton[i]->SetValue(false);
		subbutton[i]->SetValue(false);
	}

	if(type == 1) addbutton[sel]->SetValue(true);
	if(type == 2) subbutton[sel]->SetValue(true);

	selectmode[sel] = type;	
	currselect = sel;

	neurobox->diagbox->Write(text.Format("\nSelect Button type %d  selection %d\n", type, sel));
}


// OnClick, from ToolBox, used here for detecting clicks in select control panels to switch between selects
void SpikePanel::OnClick(wxPoint pos)
{
	wxString text;
	bool select = false;

	wxRect selrect1 = wxRect(selectbox[0]->GetPosition(), selectbox[0]->GetSize());
	wxRect selrect2 = wxRect(selectbox[1]->GetPosition(), selectbox[1]->GetSize());

	if(selrect1.Contains(pos) || selrect2.Contains(pos)) select = true;
	if(selrect1.Contains(pos)) currselect = 0;
	if(selrect2.Contains(pos)) currselect = 1;

	AddSubToggle(currselect, 1);
	SelectUpdate();      

	if(select) neurobox->diagbox->Write(text.Format("Neuro Box Click x %d y %d  Select %d\n", pos.x, pos.y, currselect));
	mainwin->scalebox->BurstDisp(1);
}


void SpikePanel::SetSelectRange(double from, double to)
{
	fromcon->SetValue(from);
	tocon->SetValue(to);

	if(selectmode[currselect] == 1) SelectAdd();
	if(selectmode[currselect] == 2) SelectSub();
}


void SpikePanel::SelectAdd()
{
	int sfrom, sto;
	wxString text;

	if(diagnostic) neurobox->diagbox->Write(text.Format("Add %d\n", currselect));

	//ParamStore *selectparams = neurobox->GetParams();
	//sfrom = (*selectparams)["from"] * 1000;         // Convert from s to ms
	//sto = (*selectparams)["to"] * 1000;
	sfrom = fromcon->GetValue() * 1000;         // Convert from s to ms
	sto = tocon->GetValue() * 1000;

	//diagbox->Write(text.Format("spike count %d\n", numspikes));

	for(i=0; i<currneuron->spikecount; i++) {
		if(currneuron->times[i] > sfrom && currneuron->times[i] < sto) {               // should this be <= and >= ?														
			selectspikes[currselect][i] = currselect + 1;		
		}	
	}

	SelectUpdate();
}


void SpikePanel::SelectSub()
{
	int sfrom, sto;

	//ParamStore *selectparams = neurobox->GetParams();
	//sfrom = (int)(*selectparams)["from"] * 1000;
	//sto = (int)(*selectparams)["to"] * 1000;
	sfrom = fromcon->GetValue() * 1000;         // Convert from s to ms
	sto = tocon->GetValue() * 1000;
	
	for(i=0; i<currneuron->spikecount; i++) {
		if(currneuron->times[i] > sfrom && currneuron->times[i] < sto) {
			selectspikes[currselect][i] = 0;
		}
	}

	SelectUpdate();
}


void SpikePanel::SelectUpdate()
{
	if(!currneuron->spikecount) return;      // use spikecount to check for spike data

	currneuron->selectdata->spikes = selectspikes[currselect].data();
	if(!currneuron->colourdata) {
		currneuron->ColourSwitch(2);
		mainwin->scalebox->ratedata = 2;
		mainwin->scalebox->databutton->SetLabel("Select");
	}

	/*
	// Switch to select plots if not in burst mode
	if(!(*mainwin->scalebox->gflags)["burstmode"]) {
	(*mainwin->scalebox->gflags)["selectmode"] = 1;
	mainwin->scalebox->GraphSwitch();
	}
	*/

	//diagbox->textbox->AppendText(text.Format("sub%d from %d to %d\n", currselect, sfrom, sto));

	AnalyseSelection();
	if(cellmode && neurobox->burstbox) neurobox->burstbox->ExpDataScan(currneuron);
	mainwin->scalebox->GraphUpdate();
}


/*
// OnAdd, out of use since switch to toggle button
void NeuroBox::OnAdd(wxCommandEvent& event)
{
	currselect = event.GetId() - 100;
	SelectAdd();

	diagbox->Write(text.Format("\nAdd Button selection %d\n", currselect));
}


// OnSub, out of use since switch to toggle button
void NeuroBox::OnSub(wxCommandEvent& event)
{
	currselect = event.GetId() - 200;
	SelectSub();

	diagbox->Write(text.Format("\nSub Button selection %d\n", currselect));
}
*/


void SpikePanel::OnInvert(wxCommandEvent& event)
{
	int sel = event.GetId() - 400;

	for(i=0; i<currneuron->spikecount; i++) selectspikes[sel][i] = (sel + 1) - selectspikes[sel][i];

	currselect = sel;
	AddSubToggle(currselect, 1);
	SelectUpdate();
}


void SpikePanel::OnClear(wxCommandEvent& event)
{
	int sel = event.GetId() - 300;

	for(i=0; i<currneuron->spikecount; i++) selectspikes[sel][i] = 0;

	currselect = sel;
	AddSubToggle(currselect, 1);
	SelectUpdate();
}


void SpikePanel::AnalyseSelection()
{
	int i, selspike;
	int numspikes, scount;
	int sfrom, sto;
	double isi, timepoint;
	double intracount, intratime;
	wxString text;

	if(!currneuron->selectdata) currneuron->selectdata = new BurstDat();
	currneuron->selectdata->times = currneuron->times.data();

	//currneuron->SelectScan();
	currneuron->SelectScan(currselect);

	currneuron->selectdata->IntraBurstAnalysis();
	if(currneuron->neurodata->Selected()) {
		currneuron->SelectFitAnalysis();
		currneuron->IoDfit = currneuron->selectdata->IoDdata.data.data();
		if(diagnostic) neurobox->diagbox->Write("NeuroBox select fit mode\n");
	}
	else {
		currneuron->IoDfit = currneuron->IoDdata.data.data();
		if(diagnostic) neurobox->diagbox->Write("NeuroBox basic fit mode\n");
	}

	//mod->SelectBurst(selectdata[currselect]);

	PanelData();

	if(diagnostic) neurobox->diagbox->Write(text.Format("\nSelect analyse %d spikes %.2fHz time %.2f test %d\n", 
		currneuron->selectdata->intraspikes, currneuron->selectdata->freq, currneuron->selectdata->intratime, currneuron->selectdata->test));

	//if(currselect == 0) mod->burstbox->BurstDataDisp(mod->spikedata, mod->burstbox->modburst);
	//if(currselect == 1) mod->burstbox->BurstDataDisp(mod->spikedata, mod->burstbox->evoburst);
}


NeuroDat *SpikePanel::GetCell(wxString name) 
{
	for(i=0; i<neurocount; i++) if((*neurons)[i].name == name) return &(*neurons)[i];
	return NULL;
}


void SpikePanel::SelectStore()
{
	int cellindex, select;
	wxString filepath;
	wxString filedir, filesuffix;
	wxString text;
	TextFile selectfile;
	NeuroDat *cell;

	if(currneuron->spikecount) {
		for(select=0; select<selectcount; select++) {
			currneuron->selectdata->spikes = selectspikes[select].data();
			currneuron->SelectScan(select); 
		}
	}

	filedir = neurobox->mod->GetPath() + "/Tools";
	if(!wxDirExists(filedir)) wxMkdir(filedir);
	filesuffix = "-select.dat";

	// Select data file
	filepath = neurobox->selectstoretag->StoreTag(filedir, filesuffix);
	if(filepath.IsEmpty()) return;

	selectfile.New(filepath);
	for(cellindex=0; cellindex<neurocount; cellindex++) {
		cell = &(*neurons)[cellindex];
		for(select=0; select<selectcount; select++) {
			if(diagnostic) neurobox->diagbox->Write(text.Format("SelectStore cell %d select %d numselects %d\n", cellindex, select, cell->numselects[select]));
			for(i=1; i<=cell->numselects[select]; i++) {
				text.Printf("dat %s  cel %d  sel %d  index %d  sta %d  end %d", 
					cell->name, cellindex, select, i, cell->selectstore[select][i].start, cell->selectstore[select][i].end);	
				selectfile.WriteLine(text);
			}
		}
	}
	selectfile.Close();
}


void SpikePanel::SelectLoad()
{
	wxString filedir, filepath;
	wxString filesuffix;
	TextFile selectfile;
	wxString readline;
	int index, start, end;
	int cellindex, select;
	wxString name;
	bool diagnostic = true;
	NeuroDat *cell;

	if(currneuron->selectdata) currneuron->selectdata->spikes = selectspikes[currselect].data();

	for(i=0; i<neurocount; i++) {
		for(select=0; select<selectcount; select++) (*neurons)[i].numselects[select] = 0;
	}

	filedir = neurobox->mod->GetPath() + "/Tools";
	filesuffix = "-select.dat";
	filepath = neurobox->selectstoretag->LoadTag(filedir, filesuffix);
	if(filepath.IsEmpty()) return;

	if(diagnostic) neurobox->diagbox->Write("SelectLoad " + filepath + "\n");

	selectfile.Open(filepath);
	readline = selectfile.ReadLine();

	// Updated to reference by data tag string instead of index - October 2020

	while(!readline.IsEmpty()) {
		name = "";
		if(readline.GetChar(0) == 'd') name = ParseString(&readline, 't');
		cellindex = ParseLong(&readline, 'l');
		if(diagnostic) neurobox->diagbox->Write(text.Format("SelectLoad name %s  index %d\n", name, cellindex));
		if(!name.IsEmpty()) cell = GetCell(name);   // new file format
		else cell = &(*neurons)[cellindex];           // old file format or blank name
		if(!cell) {
			if(diagnostic) neurobox->diagbox->Write("NULL cell\n");
			readline = selectfile.ReadLine();	
			continue;
		}
		if(diagnostic) neurobox->diagbox->Write(text.Format("index check readline %s check %d\n", readline, ParseCheck(&readline, "sel")));
		if(ParseCheck(&readline, "sel")) select = ParseLong(&readline, 'l');
		else select = currselect;
		index = ParseLong(&readline, 'x');
		cell->numselects[select]++;
		cell->selectstore[select][index].start = ParseLong(&readline, 'a');
		cell->selectstore[select][index].end = ParseLong(&readline, 'd');
		if(diagnostic) neurobox->diagbox->Write(text.Format("SelectLoad  cell %d  index %d  start %d  end %d\n", 
			cellindex, index, cell->selectstore[select][index].start, cell->selectstore[select][index].end)); 
		if(selectfile.End()) break;
		readline = selectfile.ReadLine();	
	}

	if(currneuron->spikecount) {
		for(i=0; i<selectcount; i++) {
			currneuron->selectdata->spikes = selectspikes[i].data();
			currneuron->SelectSpikes(i);  // store current cell's select grid to NeuroDat
		}
		currneuron->selectdata->spikes = selectspikes[currselect].data();
		AnalyseSelection();
		if(!currneuron->colourdata) {
			currneuron->ColourSwitch(2);
			mainwin->scalebox->ratedata = 2;
			mainwin->scalebox->databutton->SetLabel("Select");
		}
	}

	mainwin->scalebox->GraphUpdate();
	selectfile.Close();	
}



NeuroBox::NeuroBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ParamBox(model, title, pos, size, "cellbox", 1, false)
	//ParamBox(model, title, pos, size, "outbox", 0, 1)
{
	int datwidth, labelwidth;

	mod = model;
	diagbox = mod->diagbox;
	//cellcount = 0;
	paramindex = 0;
	textgrid = NULL;
	burstbox = NULL;
	cellpanel = NULL;
	modpanel = NULL;

	selfstore = true;

	//selectcount = 2;
	//neuroindex = 0;
	
	//selectspikes[0].resize(100000);
	//selectspikes[1].resize(100000);
	spikeselectLink = false;

	//for(i=0; i<100000; i++) {
	//	selectspikes[0][i] = 0;
	//	selectspikes[1][i] = 0;
	//}

	long notestyle = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS;
	//wxAuiNotebook *tabpanel = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notestyle);
	 auitabpanel = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notestyle);

	 // Analysis parameters

	ToolPanel *analysispanel = new ToolPanel(this, auitabpanel);
	//ToolPanel *analysispanel = new ToolPanel(tabpanel, this, wxPoint(-1, -1), wxSize(-1, -1));
	analysispanel->SetFont(boxfont);
	wxBoxSizer *analysisbox = new wxBoxSizer(wxVERTICAL);
	analysispanel->SetSizer(analysisbox);
	activepanel = analysispanel;
	paramset.panel = analysispanel;

	wxBoxSizer *histparambox = new wxBoxSizer(wxVERTICAL);
	paramset.AddNum("normscale", "Norm Scale", 10000, 0, 70, 50);
	paramset.AddNum("histrange", "Hist Range", 1000, 0, 70, 50);
	paramset.AddNum("filterthresh", "ISI Filter", 5, 0, 70, 50);
	paramset.AddNum("binoffset", "30s Bin Offset", 0, 2, 70, 50);
	//paramset.AddNum("binsize", "Bin Size", 5, 0, 70, 50);
	PanelParamLayout(histparambox);

	wxBoxSizer *colbox = new wxBoxSizer(wxHORIZONTAL); 
	//colbox->AddStretchSpacer();
	//colbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//colbox->AddSpacer(10);
	colbox->Add(histparambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//colbox->AddStretchSpacer();

	analysisbox->AddStretchSpacer();
	analysisbox->Add(colbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	analysisbox->AddStretchSpacer();
	analysispanel->Layout();


	// Spike data panel (including selection)

	cellpanel = new SpikePanel(this);
	cellpanel->cellmode = true;
	cellpanel->ratetag = "cellspikes";

	//modpanel = new SpikePanel(this);

	// Data Loading - currently for batch loading 

	ToolPanel *loadpanel = new ToolPanel(this, auitabpanel);
	loadpanel->SetFont(boxfont);
	wxBoxSizer *loadbox = new wxBoxSizer(wxVERTICAL);
	loadpanel->SetSizer(loadbox);
	activepanel = loadpanel;
	paramset.panel = loadpanel;

	wxBoxSizer *datapathbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddText("datapath", "Data Path", "", 60, 250);
	datapathbox->Add(paramset.GetCon("datapath"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	AddButton(ID_PathBrowse, "Browse", 60, datapathbox);

	wxBoxSizer *datatagbox = new wxBoxSizer(wxHORIZONTAL);
	neurodatatag = new TagBox(mainwin, activepanel, ID_Select, "", wxDefaultPosition, wxSize(150, -1), "neurodatatag", mod->path);
	datatagbox->Add(neurodatatag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	if(ostype == Mac) {
		AddButton(ID_Load, "Load", 60, datatagbox);
		AddButton(ID_FileBrowse, "Browse", 60, datatagbox);
	}
	else {
		AddButton(ID_Load, "Load", 40, datatagbox);
		datatagbox->AddSpacer(2);
		AddButton(ID_FileBrowse, "Browse", 50, datatagbox);
	}

	loadbox->AddStretchSpacer();
	loadbox->Add(datapathbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	loadbox->AddSpacer(10);
	loadbox->Add(datatagbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	loadbox->AddStretchSpacer();
	loadpanel->Layout();


	//////////////////////////////////////////////////
	// Main Structure

	auitabpanel->Freeze();
	auitabpanel->AddPage(cellpanel, "Cell Select" , true);
	//if(modpanel) auitabpanel->AddPage(modpanel, "Mod Select" , false);
	auitabpanel->AddPage(analysispanel, "Analysis" , false);
	auitabpanel->AddPage(loadpanel, "Loading" , false);
	auitabpanel->Thaw();

	ToolPanel *storepanel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer *storebox = new wxBoxSizer(wxVERTICAL);
	storepanel->SetSizer(storebox);

	activepanel = storepanel;
	selectstoretag = new TagBox(mainwin, activepanel, ID_Select, "", wxDefaultPosition, wxSize(150, -1), "selectstoretag", mod->path);
	wxBoxSizer *selectstorebox = selectstoretag->TagCon(this, ID_selectstore, ID_selectload);

	//selectstorebox->Add(selectstoretag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//wxBoxSizer *selectbuttons = new wxBoxSizer(wxHORIZONTAL);
	//AddButton(ID_selectstore, "Store", 40, selectbuttons, 2);
	//AddButton(ID_selectload, "Load", 40, selectbuttons, 2);
	//selectstorebox->Add(selectbuttons, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	//wxBoxSizer *filterbox = new wxBoxSizer(wxHORIZONTAL);
	selectstorebox->AddSpacer(20);
	AddButton(ID_filter, "Grid Remove", 70, selectstorebox);
	selectstorebox->AddSpacer(5);
	AddButton(ID_keep, "Grid Keep", 60, selectstorebox);

	//filterbox->AddSpacer(20);
	//filterbox->Add(selectstorebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//filterbox->Add(selectstorebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	
	storebox->Add(selectstorebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	storepanel->Layout();

	winman->AddPane(auitabpanel, wxAuiPaneInfo().Name("tabpane").CentrePane().PaneBorder(false));
	winman->AddPane(storepanel, wxAuiPaneInfo().Name("storepane").Bottom().CaptionVisible(false).BestSize(-1, 30).PaneBorder(false));
	winman->Update();

	if(selfstore) Load();   // load self-stored tool parameter values

	
	Connect(ID_Load, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnLoadData));

	//Connect(100, 105, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnAdd));        // old code from before Add and Sub made toggle buttons
	//Connect(200, 205, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnSub));

	Connect(ID_filter, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnGridFilter));
	Connect(ID_keep, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnGridFilter));

	Connect(ID_PathBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnBrowse));
	Connect(ID_FileBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnBrowse));

	Connect(ID_selectstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnSelectStore));
	Connect(ID_selectload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnSelectLoad));
}


void NeuroBox::AddModSpikePanel(SpikeDat *spikedata, std::vector<NeuroDat>*dataneurons)
{
	modpanel = new SpikePanel(this);
	modpanel->SetData(spikedata, dataneurons);

	auitabpanel->Freeze();
	auitabpanel->InsertPage(1, modpanel, "Mod Select" , false);
	auitabpanel->Thaw();
	winman->Update();
}


void NeuroBox::OnSelectStore(wxCommandEvent& event)
{
	cellpanel->SelectStore();
}


void NeuroBox::OnSelectLoad(wxCommandEvent& event)
{
	cellpanel->SelectLoad();
}


void NeuroBox::OnBoxCheck(wxCommandEvent &event)
{
	int id = event.GetId();
	wxString checktag = checkrefs->GetRef(id);

	if((*modflags)[checktag] == 0) (*modflags)[checktag] = 1;
	else (*modflags)[checktag] = 0;

	diagbox->Write(text.Format("NeuroBox OnBoxCheck id %d\n", id));

	// Set cell for filtering
	if(checktag == "cellfilter") {
		(*cellpanel->neurons)[cellpanel->neuroindex].filter = (*modflags)[checktag];
	}
}


void NeuroBox::OnGridFilter(wxCommandEvent &event)
{
	int filtermode = 0;

	diagbox->Write("Grid filter clicked\n");

	if(event.GetId() == ID_keep) filtermode = 1;
	gridbox->NeuroGridFilter(filtermode);
	gridbox->NeuroScan();
}


void NeuroBox::Analysis()           // Specific to generating histogram and hazard grid output - currently out of use
{
	int i, j;
	int binsize = 5;
	int timerange = 1000;
	int gridmax = 500;
	int histcount;
	wxString text;

	ParamStore *calcparams = GetParams();
	cellpanel->currneuron->normscale = (*calcparams)["normscale"];
	timerange = (*calcparams)["histrange"];
	//binsize = (*calcparams)["binsize"];
	if(timerange < 0) timerange = 0;
	histcount = timerange / binsize;
	if(histcount > gridmax) histcount = gridmax;

	/*
	for(j=0; j<histcount; j++) {
		gridbox->histgrid->SetCell(j + 1, 0, text.Format("%d", j * 5));
		gridbox->hazgrid->SetCell(j + 1, 0, text.Format("%d", j * 5));
	}

	for(i=0; i<cellcount; i++) {
		//diagbox->Write(text.Format("Scanning cell %d %s ", i, celldata[i].name));
		databox->histgrid->SetCell(0, i, celldata[i].name);
		databox->hazgrid->SetCell(0, i, celldata[i].name);
		//diagbox->Write("Calling neurocalc\n");
		viewcell[0].neurocalc(&(celldata[i]));
		//diagbox->Write("OK\n"); 
		for(j=0; j<histcount; j++) {
			databox->histgrid->SetCell(j + 1, i + 1, text.Format("%.4f", viewcell[0].hist5norm[j]));
			databox->hazgrid->SetCell(j + 1, i + 1, text.Format("%.4f", viewcell[0].haz5[j]));
		}
	}
	*/
	//databox->notebook->SetPageText(1, text.Format("%dms Histograms", binsize));
	//databox->notebook->SetPageText(2, text.Format("%dms Hazards", binsize));
}


void NeuroBox::OnBrowse(wxCommandEvent& event)
{
	if(event.GetId() == ID_PathBrowse) {
		wxDirDialog *dirdialog = new wxDirDialog(this, "Choose a directory", paramset.GetCon("datapath")->GetLabel(), 0, wxDefaultPosition);
		if(dirdialog->ShowModal() == wxID_OK) paramset.GetCon("datapath")->SetValue(dirdialog->GetPath()); 
	}

	if(event.GetId() == ID_OutputBrowse) {
		wxDirDialog *dirdialog = new wxDirDialog(this, "Choose a directory", paramset.GetCon("outpath")->GetLabel(), 0, wxDefaultPosition);
		if(dirdialog->ShowModal() == wxID_OK) paramset.GetCon("outpath")->SetValue(dirdialog->GetPath()); 
	}

	if(event.GetId() == ID_FileBrowse) {
		wxFileDialog *filedialog = new wxFileDialog(this, "Choose a file", neurodatatag->GetLabel(), "", "", 0, wxDefaultPosition);
		if(filedialog->ShowModal() == wxID_OK) neurodatatag->SetValue(filedialog->GetFilename()); 
	}
}


void NeuroBox::OnLoadData(wxCommandEvent& event)
{
	wxString filetag, filepath, text;
	short tagpos;
	FileDat *file;

	filetag = neurodatatag->GetValue();
	filepath = paramset.GetCon("datapath")->GetString();
	//filepath = "C:/Data/VMN";
	FileDat newfile = FileDat(filetag, filepath);

	tagpos = neurodatatag->FindString(filetag);

	file = mainwin->filebase->Add(FileDat(filetag, filepath));

	if(tagpos != wxNOT_FOUND) neurodatatag->Delete(tagpos);

	//file = new FileDat(filetag, filepath);
	//mainwin->diagbox->Write(text.Format("Entry name %s path %s\n", filetag, filepath));
	//file = new FileDat(filetag, filepath);
	//mainwin->diagbox->Write(text.Format("FileDat name %s path %s\n", file->name, file->path));

	neurodatatag->Insert(filetag, 0, file);
	neurodatatag->SetValue(filetag);

	//if(mainwin->diagbox) mainwin->diagbox->Write("filetag " + filetag);
	//if(mainwin->diagbox) mainwin->diagbox->Write("\nOn Load\n");
	//LoadData(file);
	LoadDataList(file);

	//mod->filebase->newentry = false;
}

/*
void NeuroBox::PanelData(NeuroDat *data)
{
	if(!data) data = &(*cells)[neuroindex];
	if(data->netflag) snum = "sum";
	else snum = numstring(neuroindex, 0);
	datneuron->SetLabel(snum);

	label->SetLabel(data->name);
	spikes->SetLabel(snum.Format("%d", data->spikecount));
	freq->SetLabel(snum.Format("%.2f", data->freq));
	//mean->SetLabel(snum.Format("%.1f", data->meanisi));
	//sd->SetLabel(snum.Format("%.2f", data->isivar));
	selspikes->SetLabel(snum.Format("%d", currcell->selectdata->intraspikes));
	selfreq->SetLabel(snum.Format("%.2f", currcell->selectdata->freq));
}
*/



/*
void NeuroBox::OnPrev(wxSpinEvent& WXUNUSED(event))
{
	if(!cellcount) return;
	if(neuroindex > 0) neuroindex--;
	else neuroindex = cellcount-1;

	for(i=0; i<selectcount; i++) {
		currcell->selectdata->spikes = selectspikes[i].data();
		currcell->SelectScan(i);  // store current cell's select grid to NeuroDat
	}
	NeuroData();
}


void NeuroBox::OnNext(wxSpinEvent& WXUNUSED(event))
{
	if(!cellcount) return;
	if(neuroindex < cellcount-1) neuroindex++;
	else neuroindex = 0;

	for(i=0; i<selectcount; i++) {
		currcell->selectdata->spikes = selectspikes[i].data();
		currcell->SelectScan(i);  // store current cell's select grid to NeuroDat
	}
	NeuroData();
}


void NeuroBox::OnEnter(wxCommandEvent& event)
{
	int id = event.GetId();
	long data;

	// Enter pressed for neuron selection
	if(id == ID_Neuron) {
		datneuron->GetValue().ToLong(&data);
		if(data >= 0 && data < cellcount) {
			neuroindex = data;
			NeuroData();
		}
		return;
	}
	else NeuroData();
}
*/

// LoadDataList - batch loading of spike time data files into the grid

void NeuroBox::LoadDataList(FileDat *file)
{
	int i, row, col;
	int start, filecount;
	wxString filepath;
	wxString datstring, readline, text;
	string line, filename;
	long timeval;
	double dataval;

	if(!file) {
		mainwin->diagbox->Write("Bad file\n");
		return;
	}
	if(file->path == "") {
		mainwin->diagbox->Write("No file path\n");
		return;
	}
	if(file->name == "") {
		mainwin->diagbox->Write("No file name\n");
		return;
	}

	mainwin->diagbox->Write(text.Format("FileDat path %s name %s\n", file->path, file->name));

	row = 0;
	col = 0;
	filecount = 0;
	filename = file->String().ToStdString();

	ifstream infile(filename.c_str());
	if(infile.is_open()) {
		while(getline(infile, line)) {
			wxString readline(line);
			mainwin->diagbox->Write(readline + "\n");
			LoadNeuroData(FileDat(readline, file->path), filecount++);
			//textgrid->ParseLine(row++, col, line);
			//if(i%100000 == 0) mainwin->diagbox->Write(text.Format("Line %d\n", i)); 
		}
		//mainwin->diagbox->Write(line + "\n");
		infile.close();
	}
	else {
		mainwin->diagbox->Write("\nUnable to open file\n"); 
		mainwin->diagbox->Write(text.Format("filename %s\n", filename));
	}
	mainwin->diagbox->Write("\nFile OK\n"); 
}


// LoadNeuroData - load data from spike time data file into the grid

void NeuroBox::LoadNeuroData(FileDat file, int col)
{
	int row;
	int start;
	wxString filepath;
	wxString datstring, readline, text;
	string line, filename;
	long timeval;
	double dataval;
	int linecount = 0;
	int datacount = 0;

	if(file.path == "") {
		mainwin->diagbox->Write("No file path\n");
		return;
	}
	if(file.name == "") {
		mainwin->diagbox->Write("No file name\n");
		return;
	}

	mainwin->diagbox->Write(text.Format("FileDat path %s name %s\n", file.path, file.name));

	row = 0;
	//col = 0;
	filename = file.String().ToStdString();

	ifstream infile(filename.c_str());

	if(!infile.is_open()) {
		filename = filename + ".txt";
		infile.open(filename.c_str());
	}

	/*
	readline = datfile->GetFirstLine();
	datfiletag->SetValue(readline);
	if(readline.GetChar(0) == '\'') {
		readline = readline.AfterFirst('\'');
		datname = readline.BeforeFirst('\'');
	}
	else if(readline.GetChar(0) == '\"') {
		readline = datfile->GetNextLine();
		readline = readline.AfterFirst('\"');
		datname = readline.BeforeFirst('\"');
	}
	datfiletag->SetValue(datname);

	if(readline.ToDouble(&datval)) rawdata[count++] = datval * 1000;        // rawdata in ms
	readline = datfile->GetNextLine();

	while(readline.IsEmpty() || readline.GetChar(0) == '\"' || readline.GetChar(0) == ':') 
		readline = datfile->GetNextLine();

	while(!readline.IsEmpty() && readline.GetChar(0) != '\"' && readline.GetChar(0) != ':') {
		readline.Trim();
		readline.ToDouble(&datval);
		rawdata[count++] = datval * units;
		readline = datfile->GetNextLine();
		if(count == stretchdata) {
			stretchdata = stretchdata + 1000;
			rawdata.setsize(stretchdata);
			loaddata->ReSize(stretchdata);
		}
	}*/

	if(infile.is_open()) {
		while(getline(infile, line)) {
			linecount++;
			//wxString readline(line);
			//mainwin->diagbox->Write(readline + "\n");
			//LoadNeuroData(FileDat(readline, file->path));
			if(line == ":") {
				mainwin->diagbox->Write("Dumping :\n");
				continue;
			}
			if(line.front() == '\'') {
				mainwin->diagbox->Write("Label detect\n");
				textgrid->ParseLabel(row++, col, line);
				continue;
			}
			if(line.front() == '\"') {
				if(linecount == 2) {
					mainwin->diagbox->Write("Label detect\n");
					textgrid->ParseLabel(row++, col, line);
				}
				continue;
			}
			if(line.empty()) {
				if(datacount) break;
				else continue;
			}
			textgrid->ParseLine(row++, col, line);
			datacount++;
			//if(i%100000 == 0) mainwin->diagbox->Write(text.Format("Line %d\n", i)); 
		}
		//mainwin->diagbox->Write(line + "\n");
		infile.close();
	}
	else {
		mainwin->diagbox->Write("\nUnable to open file\n"); 
		mainwin->diagbox->Write(text.Format("filename %s\n", filename));
	}
	mainwin->diagbox->Write(text.Format("\nFile OK, datacount = %d\n", datacount)); 
}


/*
void NeuroBox::SetCell(int cellindex, GraphDat* graph)
{
	neuroindex = cellindex;
	//(*graph).gname.Printf("n%d", cellindex);
	(*graph).gname = (*cells)[cellindex].name;
	NeuroData(false);
}
*/


int NeuroBox::GetCellIndex()
{
	return cellpanel->neuroindex;
}


void NeuroBox::DataSelect(double from, double to)
{
	cellpanel->SetSelectRange(from, to);
}


void NeuroBox::SetGraph(GraphWindow3 *graphwin)
{
	int i, type;
	wxString tag;
	double pval;
	GraphDat *graph;

	//if(newgraphwin) graphwin = newgraphwin;            // default newgraphwin=NULL for updating panel without changing graph window
	graph = graphwin->dispset[0]->plot[0];

	if(graph->gname.Contains("Cell")) {
		auitabpanel->SetSelection(0);
	}
	if(graph->gname.Contains("Model")) {	
		auitabpanel->SetSelection(1);
	}
}



GridBox::GridBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size, int rows, int cols, bool bmode, bool vmode)
	: ParamBox(model, title, pos, size, "gridbox", 0, 1)
{
	int gridrows, gridcols;
	wxBoxSizer *vdubox;
	mod = model;
	numgrids = 0;
    
    undomode = true;

	redtag = "";
	gridrows = rows;
	gridcols = cols;
	bookmode = bmode;
	vdumode = vmode;
	delete parambox;

	startshift = true;
	notebook = NULL;
	vdu = NULL;
	gauge = NULL;

	textdata.resize(1000);
	//textdatagrid.max = 1000;
	textdatagrid.grow = 10;

	numdata.resize(10000);
	//numdatagrid.max = 10000;
	numdatagrid.grow = 100;

	datagrid = NULL;
	outputgrid = NULL;
	paramgrid = NULL;
	layoutgrid = NULL;

	//InitMenu();
	//SetModFlag(ID_FileIO, "ioflag", "IO Mode", 0); 

	diagbox = mod->diagbox;

	if(vdumode) {
		vdu = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1), wxBORDER_RAISED|wxTE_MULTILINE);
		vdu->SetFont(confont);
		vdu->SetForegroundColour(wxColour(0,255,0)); // set text color
		vdu->SetBackgroundColour(wxColour(0,0,0)); // set text back color

		gauge = new wxGauge(panel, wxID_ANY, 10);
		vdubox = new wxBoxSizer(wxVERTICAL);
		vdubox->Add(vdu, 1, wxEXPAND);
	}

	if(bookmode) {
		notebook = new wxNotebook(panel, -1, wxPoint(-1,-1), wxSize(-1, 400), wxNB_TOP);
		datagrid = AddGrid("Data", wxSize(gridrows, gridcols));
		outputgrid = AddGrid("Output", wxSize(gridrows, gridcols));
		paramgrid = AddGrid("Params", wxSize(20, 20));
		layoutgrid = AddGrid("Layout", wxSize(20, 20));
	}
	else AddGrid("", wxSize(gridrows, gridcols));

	currgrid = textgrid[0];
	//textgrid[0]->diagbox = diagbox;

	//for(i=0; i<gridrows; i++) textgrid->SetRowSize(i, 25);
	//for(i=0; i<gridcols; i++) textgrid->SetColSize(i, 60);
	//textgrid[0]->SetDefaultRowSize(20, true);
	//textgrid[0]->SetDefaultColSize(60, true);
	//textgrid->SetRowLabelSize(80); 
	//textgrid[0]->SetRowLabelSize(50); 
	//textgrid[0]->vdu = NULL;
	//textgrid[0]->gauge = NULL;

	wxBoxSizer *controlbox = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *storebox = StoreBox();

	buttonbox = new wxBoxSizer(wxHORIZONTAL);
	buttonbox->Add(storebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	AddButton(ID_Undo, "Undo", 40, buttonbox);
	buttonbox->AddSpacer(2);
	AddButton(ID_Copy, "Copy", 40, buttonbox);

	wxBoxSizer *leftbox = new wxBoxSizer(wxVERTICAL);
	leftbox->Add(buttonbox, 0);  
	if(vdumode) leftbox->Add(gauge, 0, wxEXPAND);

	//controlbox->Add(storebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->AddSpacer(10);
	//controlbox->Add(buttonbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->Add(leftbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	controlbox->AddSpacer(10);
	//controlbox->Add(vdu, 100, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//controlbox->Add(displaybox, 100, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	if(vdumode) controlbox->Add(vdubox, 100, wxEXPAND);
	controlbox->AddSpacer(10);

	if(bookmode) mainbox->Add(notebook, 1, wxEXPAND);
	else mainbox->Add(textgrid[0], 1, wxEXPAND);
	mainbox->Add(controlbox, 0);
	mainbox->AddSpacer(2);

	//GridDefault();

	//histmode = 1;
	//HistLoad();
	//paramstoretag->SetLabel(initparams);

	panel->Layout();

	//textgrid[0]->vdu = vdu;
	//textgrid[0]->gauge = gauge;
	//textgrid[0]->gridbox = this;

	Connect(ID_paramstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnGridStore));
	Connect(ID_paramload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnGridLoad));
	Connect(ID_Undo, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnUndo));
	Connect(ID_Copy, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnCopy));
	Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(GridBox::OnRightClick));
	Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(GridBox::OnCellChange));
	Connect(wxEVT_NOTEBOOK_PAGE_CHANGED, wxBookCtrlEventHandler(GridBox::OnGridSelect));
};


void GridBox::OnGridSelect(wxBookCtrlEvent& event) {
	wxString text; 
	int newpage;

	newpage = event.GetSelection();
	//diagbox->Write(text.Format("OnGridSelect %d\n", newpage));
	currgrid = textgrid[newpage];
}


// AddGrid() in (now default) notebook mode adds a new TextGrid and wxNotebook page
// initialises grid and links to output controls

TextGrid *GridBox::AddGrid(wxString label, wxSize size) {

	// Initialise
	if(notebook) {
		textgrid[numgrids] = new TextGrid(notebook, size);
		notebook->AddPage(textgrid[numgrids], label);
		gridindex.Add(label);
	}
	else textgrid[numgrids] = new TextGrid(panel, size);

	// Set Links
	textgrid[numgrids]->diagbox = diagbox;
	textgrid[numgrids]->vdu = vdu;
	textgrid[numgrids]->gauge = gauge;
	textgrid[numgrids]->gridbox = this;

	// Format
	textgrid[numgrids]->SetDefaultRowSize(20, true);
	textgrid[numgrids]->SetDefaultColSize(60, true);
	textgrid[numgrids]->SetRowLabelSize(50); 

	return textgrid[numgrids++];
}


void GridBox::PlotButton()
{
	buttonbox->AddSpacer(2);
	AddButton(ID_Plot, "Plot", 40, buttonbox);
	Connect(ID_Plot, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnPlot));
}


void GridBox::ParamButton()
{
	buttonbox->AddSpacer(2);
	AddButton(ID_ParamScan, "Param", 40, buttonbox);
	Connect(ID_ParamScan, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnParamScan));
}


void GridBox::NeuroButton()
{
    int buttwidth;
    if(ostype == Mac) buttwidth = 45;
    else buttwidth = 40;
	buttonbox->AddSpacer(2);
	AddButton(ID_Neuron, "Neuro", buttwidth, buttonbox);
	Connect(ID_Neuron, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnNeuroScan));
}


void GridBox::OnParamMode(wxCommandEvent& event)
{
	WriteVDU("param mode");
	buttonbox->AddSpacer(2);
	AddButton(ID_ParamScan, "Params", 40, buttonbox);
	Connect(ID_ParamScan, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnParamScan));
}


void GridBox::OnParamScan(wxCommandEvent& event)
{
	int paramindex;

	paramindex = gridindex.GetIndex("Params");
	if(textgrid[paramindex]->selectrow == 0) {
		textgrid[paramindex]->selectrow = 2;
		textgrid[paramindex]->GoToCell(2, 0);
	}


	// On first press if Params grid is hidden bring to front
	if(bookmode && notebook->GetSelection() != paramindex) {
		notebook->SetSelection(gridindex.GetIndex("Params"));
		textgrid[paramindex]->SelectRow(textgrid[paramindex]->selectrow);
		return;
	}

	if(!textgrid[paramindex]->IsSelection()) textgrid[paramindex]->SelectRow(textgrid[paramindex]->selectrow);

	WriteVDU("Param Scan\n");
	mod->ParamScan();
}


void GridBox::OnPlot(wxCommandEvent& event)
{
	//mod->RangePlot(textgrid);
	WriteVDU("Plot\n");
	//diagbox->Write("param scan\n");
}


void GridBox::OnUndo(wxCommandEvent& event)
{
	currgrid->Undo();
}


void GridBox::OnButton(wxCommandEvent& event)
{
	wxString text;

	mainwin->diagbox->Write("Button: Neuro data scan\n");
}


void GridBox::OnCopy(wxCommandEvent& event)
{
	currgrid->Copy();
}


void GridBox::OnCellChange(wxGridEvent& event)
{
	int col = event.GetCol();

	mainwin->diagbox->Write(text.Format("grid cell change col %d\n", col));
}


void GridBox::OnRightClick(wxMouseEvent& event)
{
	//int id = event.GetId();
	//wxWindow *pos = FindWindowById(id, toolpanel);
	//wxPoint point = this->GetPosition();
	currgrid->SetCellValue(10, 0, "right");

	wxPoint pos = event.GetPosition();
	//wxSize size = this->GetSize();
	//menuPlot->Check(1000 + gpos->GetFront()->gindex, true);
	//PopupMenu(rightmenu, pos.x + 20, pos.y);
}


void GridBox::HistLoad()
{
	wxString filename, readline;
	TextFile infile;

	// Output Grid History
	filename =  "outgridhist.ini";
	if(infile.Open(filename)) {
		readline = infile.ReadLine();
		//tofp.WriteLine(readline);
		while(!readline.IsEmpty()) {
			readline = readline.AfterFirst(' ');
			readline.Trim();
			initparams = readline;
			paramstoretag->Insert(initparams, 0);
			readline = infile.ReadLine();
			//tofp.WriteLine(readline);
		}
		infile.Close();	
	}
	paramstoretag->SetValue(initparams);
}


void GridBox::TestGrid()
{
	int i;
	wxString text;

	for(i=0; i<1000; i++) currgrid->SetCellValue(i, 0, text.Format("%.1f", i*0.1 + 10));
}


void GridBox::GridDefault()
{
	currgrid->SetCellValue(0, 0, "date");
	currgrid->SetCellValue(1, 0, "breath rhy");
	currgrid->SetCellValue(2, 0, "odour");
	currgrid->SetCellValue(3, 0, "bedding");
	currgrid->SetCellValue(4, 0, "val");
	currgrid->SetCellValue(5, 0, "hex");
	currgrid->SetCellValue(6, 0, "air");
	currgrid->SetCellValue(7, 0, "other");

	currgrid->SetCellValue(9, 0, "phasic");
	currgrid->SetCellValue(10, 0, "other");
	currgrid->SetCellValue(11, 0, "vas");

	currgrid->SetCellValue(13, 0, "data");
	currgrid->SetCellValue(14, 0, "spikes");
	currgrid->SetCellValue(15, 0, "freq");
	currgrid->SetCellValue(16, 0, "mean isi");
	currgrid->SetCellValue(17, 0, "isi SD");
}


void GridBox::OnGridStore(wxCommandEvent& event)
{
	//GridStore();
	GridStoreAll();
}


void GridBox::OnGridLoad(wxCommandEvent& event)
{
	if(undomode) currgrid->CopyUndo();
	//GridLoad();
	//int ioflag = (*modflags)["ioflag"];

	/*
	GridLoad *gridload = new GridLoad(this);  
	gridload->Create();
	gridload->Run();
	gridload->Wait();
	delete gridload;
	*/

	//int ioflag = true;
	//if(ioflag) GridLoadFast();
	//else GridLoad();
	GridLoadAll();

	//textgrid->AutoSizeColumns(false);
}


void GridBox::ColumnSelect(int col)
{
	wxString text;

	WriteVDU(text.Format("Column Select %d\n", col));

	mod->GridColumn(col);
}


void GridBox::RowSelect(int row)
{
	wxString text;

	WriteVDU(text.Format("Row Select %d\n", row));

	mod->GridRow(row);
}


int GridBox::ColumnData(int col, datdouble *data)
{
	int row, count;
	double value;
	wxString celltext;

	count = 0;
	for(row=0; row<currgrid->GetNumberRows(); row++) {
		celltext = currgrid->GetCellValue(row, col);
		if(celltext != "") {
			celltext.ToDouble(&value);
			count++;
		}
		else value = 0;
		(*data)[row] = value;
	}
	return count;
}


void GridBox::GridStoreAll()
{
	TextFile ofp;
	int gridindex, row, col;
	wxString celltext, text, filename, filetag, filepath;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	std::vector <Index> columnindex;  // stores list of columns (by index) containing data
	int storeversion = 1;   //  initial multi grid store 14/12/20

	columnindex.resize(numgrids);

	filepath = mod->GetPath() + "/Grids";
	if(!wxDirExists(filepath)) wxMkdir(filepath);

	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	if(ofp.Exists(filename) && redtag != filetag) {
		paramstoretag->SetForegroundColour(redpen);
		paramstoretag->SetValue("");
		paramstoretag->SetValue(filetag);
		redtag = filetag;
		return;
	}

	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	sfilename = filename.ToStdString();
	ofstream outfile(sfilename.c_str());

	if(!outfile.is_open()) {
		paramstoretag->SetValue("File error");
		return;
	}

	WriteVDU("Writing file...");

	text.Printf("gsv %d\n", storeversion);
	outfile << text.ToStdString();
	text.Printf("num %d\n", numgrids);
	outfile << text.ToStdString();

	for(gridindex=0; gridindex<numgrids; gridindex++) {
		text.Printf("g %d r %d c %d\n", gridindex, textgrid[gridindex]->GetNumberRows(), textgrid[gridindex]->GetNumberCols());
		outfile << text.ToStdString();
	}

	for(gridindex=0; gridindex<numgrids; gridindex++) {
		for(row=0; row<textgrid[gridindex]->GetNumberRows(); row++) {
			if(gauge) gauge->SetValue(100 * (row + 1) / textgrid[gridindex]->GetNumberRows());
			for(col=0; col<textgrid[gridindex]->GetNumberCols(); col++) {
				celltext = textgrid[gridindex]->GetCellValue(row, col);
				celltext.Trim();                                                                     
				if(!celltext.IsEmpty()) {
					text.Printf("%d %d %d %s\n", gridindex, row, col, celltext);
					columnindex[gridindex].Add(col);
					outfile << text.ToStdString();
				}
			}
		}
	}

	outfile.close();
	if(gauge) gauge->SetValue(0);
	WriteVDU("OK\n");

	filename = filepath + "/" + filetag + "-gridsize.txt";
	ofp.New(filename);
	for(gridindex=0; gridindex<numgrids; gridindex++) {
		for(i=0; i<columnindex[gridindex].count; i++) {
			col = columnindex[gridindex].list[i];
			ofp.WriteLine(text.Format("grid %d col %d %d", gridindex, col, textgrid[gridindex]->GetColSize(col)));
		}
	}
	ofp.Close();
}


void GridBox::GridStore()
{
	TextFile ofp;
	int row, col;
	wxString celltext, text, filename, filetag, filepath;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	Index columnindex;

	/*
	filetag = gstag->GetValue();
	filename = "graph-" + filetag + ".dat";

	// Tag history
	short tagpos = gstag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) gstag->Delete(tagpos);
	gstag->Insert(filetag, 0);

	// Check and warn existing file
	wxTextFile checkfile(filepath + "/" + filename);*/

	filepath = mod->GetPath() + "/Grids";
	if(!wxDirExists(filepath)) wxMkdir(filepath);

	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	if(ofp.Exists(filename) && redtag != filetag) {
		paramstoretag->SetForegroundColour(redpen);
		paramstoretag->SetValue("");
		paramstoretag->SetValue(filetag);
		redtag = filetag;
		return;
	}

	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	//ofp.New(filename);
	sfilename = filename.ToStdString();
	ofstream outfile(sfilename.c_str());

	if(!outfile.is_open()) {
		paramstoretag->SetValue("File error");
		return;
	}

	WriteVDU("Writing file...");

	for(row=0; row<currgrid->GetNumberRows(); row++) {
		if(gauge) gauge->SetValue(100 * (row + 1) / currgrid->GetNumberRows());
		for(col=0; col<currgrid->GetNumberCols(); col++) {
			celltext = currgrid->GetCellValue(row, col);
			celltext.Trim();                                                                     // Fixes odd line endings in pasted data  23/4/15
			//if(!celltext.IsEmpty()) ofp.WriteLine(text.Format("%d %d %s", row, col, celltext));
			if(!celltext.IsEmpty()) {
				text.Printf("%d %d %s\n", row, col, celltext);
				columnindex.Add(col);
				outfile << text.ToStdString();
			}
		}
	}

	outfile.close();
	if(gauge) gauge->SetValue(0);
	WriteVDU("OK\n");

    filename = filepath + "/" + filetag + "-gridsize.txt";
	ofp.New(filename);
	for(i=0; i<columnindex.count; i++) {
		col = columnindex.list[i];
		ofp.WriteLine(text.Format("col %d %d", col, currgrid->GetColSize(col)));
	}
	ofp.Close();
}


void GridBox::GridLoadAll()
{
	TextFile ifp;
	int row, col, width;
	long numdat;
	double cellnum;
	wxString text, filetag, filepath, filename;
	wxString vertag, celldata;
	wxString datstring, readline;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	int numlines, linecount, cellcount;
	int numrows, numcols;
	int newnumgrids;
	int storeversion = 0;
	int gindex;  // grid index

	
	filepath = mod->GetPath() + "/Grids";
	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	if(!ifp.Exists(filename)) {
		paramstoretag->SetValue("Not found");
		return;
	}
	
	sfilename = filename.ToStdString();
	ifstream readfile(sfilename.c_str());

	// Param file history
	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	//textdatagrid.Clear();
	//numdatagrid.Clear();

	WriteVDU("Reading file header...");

	numlines = count(istreambuf_iterator<char>(readfile), istreambuf_iterator<char>(), '\n');
	if(!numlines) {
		WriteVDU("File empty\n");
		return;
	}
	readfile.clear();
	linecount = 0;

	// Read file to stream
	string contents;
	readfile.seekg(0, readfile.end);
	contents.resize(readfile.tellg());
	readfile.seekg(0, readfile.beg);
	readfile.read(&contents[0], contents.size());
	readfile.close();
	istringstream infile(contents);

	// Read and check file version
	getline(infile, line);
	readline = StringConvert(line);
	vertag = readline.BeforeFirst(' ');
	if(vertag != "gsv") {
		diagbox->Write("GridLoadAll file version not found, trying old GridLoad\n");
		GridLoad();
		return;
	}
	storeversion = ParseLong(&readline, 'v');

	// Read and set number of grids
	getline(infile, line);
	readline = StringConvert(line);
	newnumgrids = ParseLong(&readline, 'm');
	if(newnumgrids > numgrids) {
		// code to add new grids
		numgrids = newnumgrids;
	}
	diagbox->Write(text.Format("GridLoadAll  numgrids %d  newnumgrids %d\n", numgrids, newnumgrids));

	// Read and set grid sizes
	for(i=0; i<newnumgrids; i++) {                 // use newnumgrids to match number of grids to load from file
		getline(infile, line);
		wxString readline(line);
		gindex = ParseLong(&readline, 'g');
		numrows = ParseLong(&readline, 'r');
		numcols = ParseLong(&readline, 'c');
		if(numrows > textgrid[gindex]->GetNumberRows()) textgrid[gindex]->AppendRows(numrows - textgrid[gindex]->GetNumberRows());
		if(numcols > textgrid[gindex]->GetNumberCols()) textgrid[gindex]->AppendRows(numcols - textgrid[gindex]->GetNumberCols());
		textgrid[gindex]->ClearGrid();
	}

	WriteVDU("OK\n");

	// Read cells
	cellcount = 0;
	WriteVDU("Reading file data...");

	cellcount = 0;
	while(getline(infile, line)) {
		//diagbox->Write(text.Format(" line length %d first %d\n", (int)line.length(), (char)line[0]));
		wxString readline(line);

		if(readline.IsEmpty() || !readline[0]) break;

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		gindex = numdat;
		readline = readline.AfterFirst(' ');

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		row = numdat;
		readline = readline.AfterFirst(' ');

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		col = numdat;
		readline = readline.AfterFirst(' ');

		readline.Trim();
		celldata = readline;
		textgrid[gindex]->SetCell(row, col, celldata);
		cellcount++;
		linecount++;
		if(gauge) gauge->SetValue(100 * linecount / numlines);
	}

	WriteVDU("OK\n");
	if(gauge) gauge->SetValue(0);

	// Read column sizes file
	filename = filepath + "/" + filetag + "-gridsize.txt";
	if(!ifp.Open(filename)) return;

	readline = ifp.ReadLine();
	while(!readline.IsEmpty()) {
		gindex = ParseLong(&readline, 'd');
		col = ParseLong(&readline, 'l');
		width = ParseLong(&readline);
		//WriteVDU(text.Format("gindex %d col %d %d\n", gindex, col, width));
		textgrid[gindex]->SetColSize(col, width);
		if(ifp.End()) break;
		readline = ifp.ReadLine();
	}

	ifp.Close();
}

/*
void GridBox::GridLoad()            // Replaced by GridLoadFast()
{
	TextFile ifp;
	int i, row, col;
	long numdat;
	double cellnum;
	wxString text, filetag, filename, filepath, celldata;
	wxString datstring;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	int numlines, linecount, cellcount;

	filepath = mod->GetPath() + "/Grids";
	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	

	sfilename = filename.ToStdString();
	ifstream infile(sfilename.c_str());
	if(!infile.is_open()) {
		paramstoretag->SetValue("Not found");
		return;
	}

	// Param file history
	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	currgrid->ClearGrid();

	WriteVDU("Reading file...");


	numlines = count(istreambuf_iterator<char>(infile), istreambuf_iterator<char>(), '\n');
	infile.clear();
	infile.seekg(0, ios::beg);
	linecount = 0;
	cellcount = 0;

	//readline = ifp.ReadLine();
	while(getline(infile, line)) {
		wxString readline(line);
		//diagbox->Write(readline + "\n");
		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		row = numdat;
		readline = readline.AfterFirst(' ');

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		col = numdat;
		readline = readline.AfterFirst(' ');

		readline.Trim();
		celldata = readline;

		currgrid->SetCell(row, col, celldata);
		cellcount++;
		//diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
		//readline = ifp.ReadLine();
		//diagbox->Write("Read " + readline + "\n");
		linecount++;
		if(gauge) gauge->SetValue(100 * linecount / numlines);
	}
	infile.close();

	diagbox->Write("OK\n");
	//WriteVDU("OK\n");
	WriteVDU(text.Format("OK, %d cells\n", cellcount));
	if(gauge) gauge->SetValue(0);
}
*/


void GridBox::SetNumCell(int row, int col, double data)
{
	if(numdatagrid.count == numdata.size()) numdata.resize(numdata.size() + numdatagrid.grow);
	if(row > numdatagrid.rowmax) numdatagrid.rowmax = row;
	if(col > numdatagrid.colmax) numdatagrid.colmax = col;

	numdata[numdatagrid.count].row = row;
	numdata[numdatagrid.count].col = col;
	numdata[numdatagrid.count].data = data;

	numdatagrid.count++;
}


void GridBox::SetTextCell(int row, int col, wxString data)
{
	if(textdatagrid.count == textdata.size()) textdata.resize(textdata.size() + textdatagrid.grow);
	if(row > textdatagrid.rowmax) textdatagrid.rowmax = row;
	if(col > textdatagrid.colmax) textdatagrid.colmax = col;

	textdata[textdatagrid.count].row = row;
	textdata[textdatagrid.count].col = col;
	textdata[textdatagrid.count].data = data;

	textdatagrid.count++;
}




// Experimental file access thread for improving GUI performance during load     December 2020 - not currently in use

GridLoadThread::GridLoadThread(GridBox *gbox)
	: wxThread(wxTHREAD_JOINABLE)
{
	gridbox = gbox;
}


void *GridLoadThread::Entry()
{
	wxString text;
	TextFile ifp;
	int i;
	int row, col, width;
	long numdat;
	double cellnum;
	wxString filetag, filepath, filename, celldata;
	wxString datstring, readline;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	int numlines, linecount, cellcount;


	filepath = gridbox->mod->GetPath() + "/Grids";
	filetag = gridbox->paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	if(!ifp.Exists(filename)) {
		filename = filetag + "-grid.txt";            // Backwards compatibility, try old grid file location
		if(!ifp.Exists(filename)) {
			gridbox->paramstoretag->SetValue("Not found");
			return NULL;
		}
	}
	sfilename = filename.ToStdString();
	ifstream readfile(sfilename.c_str());

	// Param file history
	short tagpos = gridbox->paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) gridbox->paramstoretag->Delete(tagpos);
	gridbox->paramstoretag->Insert(filetag, 0);

	gridbox->redtag = "";
	gridbox->paramstoretag->SetForegroundColour(blackpen);
	gridbox->paramstoretag->SetValue("");
	gridbox->paramstoretag->SetValue(filetag);

	gridbox->currgrid->ClearGrid();
	gridbox->textdatagrid.Clear();
	gridbox->numdatagrid.Clear();

	gridbox->WriteVDU("Reading file...");

	numlines = count(istreambuf_iterator<char>(readfile), istreambuf_iterator<char>(), '\n');
	if(!numlines) {
		gridbox->WriteVDU("File empty\n");
		return NULL;
	}
	readfile.clear();
	//readfile.seekg(0, ios::beg);
	linecount = 0;

	//string filetext = (ReadFile(filename.c_str()));
	//istringstream filetext(ReadFile(filename.c_str()));

	//ifstream infile(filename, std::ios::in | std::ios::binary);

	/*
	string contents;
	readfile.seekg(0, ios::end);
	contents.resize(readfile.tellg());
	readfile.seekg(0, ios::beg);
	readfile.read(&contents[0], contents.size());
	readfile.close();
	istringstream infile(contents);
	*/

	string contents;
	readfile.seekg(0, readfile.end);
	contents.resize(readfile.tellg());
	readfile.seekg(0, readfile.beg);
	readfile.read(&contents[0], contents.size());
	readfile.close();
	istringstream infile(contents);

	/*
	diagbox->Write("Contents codes:\n");
	for(i=0; i<contents.length(); i++) {
	diagbox->Write(text.Format("%d ", contents[i]));
	if(contents[i] == '\n') diagbox->Write("\n");
	}
	diagbox->Write("EOF Contents\n");

	diagbox->Write(text.Format("File length %d size %d numlines %d :\n%s\n", (int)contents.length(), (int)contents.size(), numlines, contents));
	*/

	cellcount = 0;

	//readline = ifp.ReadLine();
	while(getline(infile, line)) {
		//diagbox->Write(text.Format(" line length %d first %d\n", (int)line.length(), (char)line[0]));
		wxString readline(line);
		/*
		diagbox->Write("Line codes:");
		for(i=0; i<readline.Len(); i++) diagbox->Write(text.Format("%d ", readline[i]));
		diagbox->Write("\n");
		*/
		//readline.Trim(false);
		//readline.Trim();
		//diagbox->Write("readline " + readline);
		if(readline.IsEmpty() || !readline[0]) break;
		//else diagbox->Write(text.Format(" length %d ", (int)readline.Len()));
		//diagbox->Write(readline + "\n");
		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		row = numdat;
		readline = readline.AfterFirst(' ');

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		col = numdat;
		readline = readline.AfterFirst(' ');

		readline.Trim();
		celldata = readline;

		if(celldata.ToDouble(&cellnum)) gridbox->SetNumCell(row, col, cellnum);
		else gridbox->SetTextCell(row, col, celldata);

		//currgrid->SetCell(row, col, celldata);

		//diagbox->Write(text.Format(" setcell %d %d %s\n", row, col, cell));
		cellcount++;
		//diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
		//readline = ifp.ReadLine();
		//diagbox->Write("Read " + readline + "\n");
		linecount++;
		//if(gauge && (linecount % (numlines / 10)) == 0) {
		//	//diagbox->Write(text.Format("Gauge %d%%\n", 100 * linecount / numlines));
		//	gauge->SetValue(100 * linecount / numlines);
		//}
		if(gridbox->gauge) gridbox->gauge->SetValue(100 * linecount / numlines);
	}


	// Transfer cell data to display grid


	int numrows, rowmax;
	int numcols, colmax;

	rowmax = gridbox->numdatagrid.rowmax;
	if(gridbox->textdatagrid.rowmax > rowmax) rowmax = gridbox->textdatagrid.rowmax;
	colmax = gridbox->numdatagrid.colmax;
	if(gridbox->textdatagrid.colmax > colmax) colmax = gridbox->textdatagrid.colmax;

	//if(gauge) gauge->SetValue(100);
	//diagbox->Write(text.Format("\nlinecount %d  numlines %d\n", linecount, numlines));

	numrows = gridbox->currgrid->GetNumberRows();
	numcols = gridbox->currgrid->GetNumberCols();
	if(rowmax > numrows) gridbox->currgrid->AppendRows(rowmax - numrows);
	if(colmax > numcols) gridbox->currgrid->AppendCols(colmax - numcols);

	int numcount, textcount;
	numcount = gridbox->numdatagrid.count;
	textcount =	gridbox->textdatagrid.count;

	for(i=0; i<numcount; i++) gridbox->currgrid->SetCell(gridbox->numdata[i].row, gridbox->numdata[i].col, text.Format("%.6f", gridbox->numdata[i].data));
	for(i=0; i<textcount; i++) gridbox->currgrid->SetCell(gridbox->textdata[i].row, gridbox->textdata[i].col, gridbox->textdata[i].data);


	//infile.close();
	//diagbox->Write("OK\n");
	//WriteVDU(text.Format("OK, %d grid cells\n", cellcount));
	gridbox->WriteVDU("OK\n");
	if(gridbox->gauge) gridbox->gauge->SetValue(0);

	if(!ifp.Open(filetag + "-gridsize.txt")) return NULL;

	readline = ifp.ReadLine();
	while(!readline.IsEmpty()) {
		col = ParseLong(&readline, 'l');
		width = ParseLong(&readline, 0);
		//WriteVDU(text.Format("col %d %d\n", col, width));
		gridbox->currgrid->SetColSize(col, width);
		if(ifp.End()) break;
		readline = ifp.ReadLine();
	}

	ifp.Close();

	return NULL;
}


void GridBox::GridLoad()
{
	TextFile ifp;
	int row, col, width;
	long numdat;
	double cellnum;
	wxString text, filetag, filepath, filename, celldata;
	wxString datstring, readline;
	wxColour redpen("#dd0000"), blackpen("#000000");
	string line, sfilename;
	int numlines, linecount, cellcount;
	int numrows, rowmax;
	int numcols, colmax;


	filepath = mod->GetPath() + "/Grids";
	filetag = paramstoretag->GetValue();
	filename = filepath + "/" + filetag + "-grid.txt";

	if(!ifp.Exists(filename)) {
		filename = filetag + "-grid.txt";            // Backwards compatibility, try old grid file location
		if(!ifp.Exists(filename)) {
			paramstoretag->SetValue("Not found");
			return;
		}
	}
	sfilename = filename.ToStdString();
	ifstream readfile(sfilename.c_str());

	// Param file history
	short tagpos = paramstoretag->FindString(filetag);
	if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
	paramstoretag->Insert(filetag, 0);

	redtag = "";
	paramstoretag->SetForegroundColour(blackpen);
	paramstoretag->SetValue("");
	paramstoretag->SetValue(filetag);

	currgrid->ClearGrid();
	textdatagrid.Clear();
	numdatagrid.Clear();

	WriteVDU("Reading file...");

	numlines = count(istreambuf_iterator<char>(readfile), istreambuf_iterator<char>(), '\n');
	if(!numlines) {
		WriteVDU("File empty\n");
		return;
	}
	readfile.clear();
	//readfile.seekg(0, ios::beg);
	linecount = 0;

	//string filetext = (ReadFile(filename.c_str()));
	//istringstream filetext(ReadFile(filename.c_str()));

	//ifstream infile(filename, std::ios::in | std::ios::binary);

	string contents;
	readfile.seekg(0, readfile.end);
	contents.resize(readfile.tellg());
	readfile.seekg(0, readfile.beg);
	readfile.read(&contents[0], contents.size());
	readfile.close();
	istringstream infile(contents);

	/*
	diagbox->Write("Contents codes:\n");
	for(i=0; i<contents.length(); i++) {
	diagbox->Write(text.Format("%d ", contents[i]));
	if(contents[i] == '\n') diagbox->Write("\n");
	}
	diagbox->Write("EOF Contents\n");

	diagbox->Write(text.Format("File length %d size %d numlines %d :\n%s\n", (int)contents.length(), (int)contents.size(), numlines, contents));
	*/

	cellcount = 0;

	//readline = ifp.ReadLine();
	while(getline(infile, line)) {
		//diagbox->Write(text.Format(" line length %d first %d\n", (int)line.length(), (char)line[0]));
		wxString readline(line);
		
		if(readline.IsEmpty() || !readline[0]) break;

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		row = numdat;
		readline = readline.AfterFirst(' ');

		datstring = readline.BeforeFirst(' ');
		datstring.ToLong(&numdat);
		col = numdat;
		readline = readline.AfterFirst(' ');

		readline.Trim();
		celldata = readline;

		if(ostype != Mac) {
			if(celldata.ToDouble(&cellnum)) SetNumCell(row, col, cellnum);
			else SetTextCell(row, col, celldata);
		}
		else currgrid->SetCell(row, col, celldata);

		//diagbox->Write(text.Format(" setcell %d %d %s\n", row, col, cell));
		cellcount++;
		
		linecount++;
		//if(gauge && (linecount % (numlines / 10)) == 0) {
		//	//diagbox->Write(text.Format("Gauge %d%%\n", 100 * linecount / numlines));
		//	gauge->SetValue(100 * linecount / numlines);
		//}
		if(gauge) gauge->SetValue(100 * linecount / numlines);
	}


	// Transfer cell data to display grid

	
	if(ostype != Mac) {
		rowmax = numdatagrid.rowmax;
		if(textdatagrid.rowmax > rowmax) rowmax = textdatagrid.rowmax;
		colmax = numdatagrid.colmax;
		if(textdatagrid.colmax > colmax) colmax = textdatagrid.colmax;

		numrows = currgrid->GetNumberRows();
		numcols = currgrid->GetNumberCols();
		if(rowmax > numrows) currgrid->AppendRows(rowmax - numrows);
		if(colmax > numcols) currgrid->AppendCols(colmax - numcols);

		for(i=0; i<numdatagrid.count; i++) currgrid->SetCell(numdata[i].row, numdata[i].col, text.Format("%.6f", numdata[i].data));
		for(i=0; i<textdatagrid.count; i++) currgrid->SetCell(textdata[i].row, textdata[i].col, textdata[i].data);
	}
	

	//infile.close();
	//diagbox->Write("OK\n");
	//WriteVDU(text.Format("OK, %d grid cells\n", cellcount));
	WriteVDU("OK\n");
	if(gauge) gauge->SetValue(0);

	if(!ifp.Open(filetag + "-gridsize.txt")) return;

	readline = ifp.ReadLine();
	while(!readline.IsEmpty()) {
		col = ParseLong(&readline, 'l');
		width = ParseLong(&readline, 0);
		//WriteVDU(text.Format("col %d %d\n", col, width));
		currgrid->SetColSize(col, width);
		if(ifp.End()) break;
		readline = ifp.ReadLine();
	}

	ifp.Close();
}

/*
void GridBox::OnCellChange(wxGridEvent& event)
{
	int col = event.GetCol();

	diagbox->Write(text.Format("plot grid cell change col %d\n", col));

	plotbox->DataEdit(col);
}


void GridBox::ColumnSelect(int col)
{
	GridBox::ColumnSelect(col);

	plotbox->SetColumn(col);
}*/



void GridBox::NeuroGridFilter(int mode)
{
	int i;
	int col;
	int newcol[1000];
	int numcols, newnumcols;

	// mode = 0   remove selected cells
	// mode = 1   keep selected cells
	
	numcols = currgrid->GetNumberCols();

	for(i=0; i<numcols; i++) colflag[i] = 0;

	for(i=0; i<neurobox->cellpanel->neurocount; i++) {
		col = (*celldata)[i].gridcol;
		if(mode == 1) colflag[col] = 1 - (*celldata)[i].filter;
		else colflag[col] = (*celldata)[i].filter;
	}

	// Mark columns with their replacement
	col = 0;
	for(i=0; i<currgrid->GetNumberCols(); i++) {
		if(!colflag[i]) {
			newcol[col] = i;
			col++;
		}
	}
	newnumcols = col - 1;

	// Copy over replacement columns
	for(i=0; i<newnumcols; i++) currgrid->CopyColumn(newcol[i], i);

	// Delete remaining copied columns
	currgrid->DeleteCols(newnumcols, numcols - newnumcols);
}


void GridBox::OnNeuroScan(wxCommandEvent& event)
{
	NeuroScan();
}


void GridBox::NeuroScan()
{
	int col, row;
	int spikecount, cellcount;
	wxString text, celltext;
	double cellval;
	int view = 0;
	int filterthresh;
	double spikeint, spiketime;
	double spikestart, startthresh;
	wxNumberFormatter numform;
	wxString typetext;

	diagbox->Write("Neuro data scan\n");
	//mod->cellbox->datneuron->SetLabel("OK");
	WriteVDU("Neural data scan...");

	ParamStore *params = neurobox->GetParams();
	filterthresh = (*params)["filterthresh"];
	spikestart = 0;
	startthresh = 10;

	cellcount = 0;
	col = 0;
	celltext = currgrid->GetCell(0, 0);
	celltext.Trim();

	while(!celltext.IsEmpty()) {
		celltext = currgrid->GetCell(0, col);
		celltext.Trim();
		if(celldata->size() <= cellcount) celldata->resize(cellcount + 10);
		//diagbox->Write(text.Format("cellcount %d  cell data size %d\n", cellcount, (int)mod->celldata.size()));
		(*celldata)[cellcount].name = celltext;
		celltext = currgrid->GetCell(1, col);
		celltext.Trim();
		spikecount = 0;
		row = 1;

		// Specific to data with type label, reject non-vasopressin types
		typetext = currgrid->GetCell(3, col);
		/*if(typetext.Contains("OT") || typetext.Contains("NR")) {                           // replace with parameterised string filtering  13/2/20
			//diagbox->Write(text.Format("col %d typetext %s rejected\n", col, typetext));
			(*celldata)[cellcount].filter = 1;
		}
		else {
			//diagbox->Write(text.Format("col %d typetext %s accepted\n", col, typetext));
			(*celldata)[cellcount].filter = 0;
		}*/

		(*celldata)[cellcount].filter = 0;


		// Skip non-spike time rows
		while (!numform.FromString(celltext, &cellval)) {
			//diagbox->Write(text.Format("col %d row %d %s\n", col, row, celltext));
			row++;
			celltext = currgrid->GetCell(row, col);
			celltext.Trim();
		}

		// Read and filter spike time data
		while(!celltext.IsEmpty()) {
			celltext.ToDouble(&cellval);
			if(spikecount == 0 && cellval > startthresh) spikestart = floor(cellval);     // shift spike times when there's a long initial silent period
			spiketime = (cellval - spikestart) * 1000;
			if(spikecount > 0) {       
				spikeint = spiketime - (*celldata)[cellcount].times[spikecount-1];
				//if(spikecount < 10) diagbox->Write(text.Format("col %d spikeint %.2f filter %d\n", col, spikeint, filterthresh));
			}
			if(spikecount == 0 || spikeint > filterthresh) {
				(*celldata)[cellcount].times[spikecount] = spiketime;
				spikecount++;
			}
			row++;
			celltext = currgrid->GetCell(row, col);
			celltext.Trim();
		}

		// Record spike count and initialise next column
		(*celldata)[cellcount].spikecount = spikecount;
		(*celldata)[cellcount].gridcol = col;
		cellcount++;
		col++;
		celltext = currgrid->GetCell(0, col);
		celltext.Trim();
	}

	if (!cellcount) {
		diagbox->Write("Neuro scan: NO DATA\n");
		//mod->cellbox->datneuron->SetLabel("NO DATA");
	}
	else {
		diagbox->Write(text.Format("Neuro scan: %d cells read OK\n", cellcount));
		//mod->cellcount = cellcount;
		if(neurobox->cellpanel->neuroindex > cellcount) neurobox->cellpanel->neuroindex = 0;
		neurobox->cellpanel->neurocount = cellcount;
		//diagbox->Write(text.Format("Neuro analysis...."));
		//neurobox->Analysis();
		//diagbox->Write(text.Format("OK\n"));
		diagbox->Write(text.Format("Neuro data...."));
		neurobox->cellpanel->NeuroData();
		diagbox->Write(text.Format("OK\n"));
	}
	WriteVDU("OK\n");
}



/*NeuroBox::NeuroBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size)
: ParamBox(mod, title, pos, size, "cellbox")
{
int datwidth;

diagbox = mod->diagbox;
cellcount = 0;
paramindex = 0;


notebook = new wxNotebook(panel, -1, wxPoint(-1,-1), wxSize(-1, 400), wxNB_TOP);


ToolPanel *analysispanel = new ToolPanel(mainwin, notebook);
analysispanel->SetFont(boxfont);
wxBoxSizer *analysisbox = new wxBoxSizer(wxVERTICAL);
analysispanel->SetSizer(analysisbox);
activepanel = analysispanel;
paramset.panel = analysispanel;


notebook->AddPage(analysispanel, "Analysis");

//chromesizer->AddStretchSpacer();
//chromesizer->Add(chromestatbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
//chromesizer->AddStretchSpacer();


wxBoxSizer *histparambox = new wxBoxSizer(wxVERTICAL);
paramset.AddNum("normscale", "Norm Scale", 10000, 0, 70, 50);
paramset.AddNum("histrange", "Hist Range", 1000, 0, 70, 50);
//paramset.AddNum("binsize", "Bin Size", 5, 0, 70, 50);
PanelParamLayout(histparambox);


datwidth = 50;
spikes = NumPanel(datwidth, wxALIGN_RIGHT);
mean = NumPanel(datwidth, wxALIGN_RIGHT);
freq = NumPanel(datwidth, wxALIGN_RIGHT);
sd = NumPanel(datwidth, wxALIGN_RIGHT);

wxGridSizer *datagrid = new wxGridSizer(2, 5, 5);
datagrid->Add(new wxStaticText(activepanel, -1, "Spikes"), 0, wxALIGN_CENTRE);
datagrid->Add(spikes);
datagrid->Add(new wxStaticText(activepanel, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(freq);
datagrid->Add(new wxStaticText(activepanel, -1, "Mean"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(mean);
datagrid->Add(new wxStaticText(activepanel, -1, "Std Dev"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(sd);

datneuron = new wxTextCtrl(activepanel, ID_Neuron, "---", wxDefaultPosition, wxSize(50, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE|wxTE_PROCESS_ENTER);
datspin = new wxSpinButton(activepanel, wxID_ANY, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
wxBoxSizer *datbox = new wxBoxSizer(wxHORIZONTAL);
datbox->Add(datspin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
datbox->AddSpacer(5);

wxBoxSizer *neurobox = new wxBoxSizer(wxHORIZONTAL);
neurobox->Add(new wxStaticText(activepanel, wxID_ANY, "Neuron"), 1, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
neurobox->Add(datneuron, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "");
databox->AddSpacer(2);
databox->Add(neurobox, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 5);
databox->AddSpacer(5);
databox->Add(datbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
databox->AddSpacer(5);
databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

wxBoxSizer *colbox = new wxBoxSizer(wxHORIZONTAL); 
//colbox->AddStretchSpacer();
colbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
colbox->AddSpacer(10);
colbox->Add(histparambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
//colbox->AddStretchSpacer();



analysisbox->AddStretchSpacer();
analysisbox->Add(colbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
analysisbox->AddStretchSpacer();

analysispanel->Layout();

mainbox->Add(notebook, 1, wxEXPAND);

//panel->Layout();


Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(NeuroBox::OnEnter));
Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(NeuroBox::OnNext));
Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(NeuroBox::OnPrev));
}*/
