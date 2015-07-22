

#include <hypobase.h>
//#include "hypomods.h"
//#include "hypopanels.h"
#include "wx/utils.h"
//#include "osmopanels.h"
//#include "hypograph.h"
#include <hypotools.h>



//IMPLEMENT_APP(HypoApp)

//wxCommandEvent *blankevent;



// Main window base class

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	ostype = GetSystem();
	mainwin = this;
	statusbar = CreateStatusBar();
	diagbox = NULL;                // Protect diagbox self reference for toolpanel

    diagbox = new DiagBox(this, "Diagnostic", wxPoint(0, 0), wxSize(400, 500));
	diagbox->Write("Diagnostic Box OK\n\n");

	graphbox = NULL;

	colourpen[0].Set("#000000");       // 0 black
	colourpen[1].Set("#F50000");       // 1 red
	colourpen[2].Set("#00F500");       // 2 green
	colourpen[3].Set("#0000F5");       // 3 blue
	colourpen[4].Set("#F5F500");       // 4 yellow
	colourpen[5].Set("#F500F5");       // 5 purple
	colourpen[6].Set("#FF8080");       // 6 light red
	colourpen[7].Set("#80FF80");       // 7 light green
	colourpen[8].Set("#8080FF");       // 8 light blue
	colourpen[9].Set("#000000");       // 9 custom

	toolset = new ToolSet();
	toolset->AddBox(diagbox, true);
}


MainFrame::~MainFrame()
{
	delete toolset;
}


void MainFrame::MainLoad()
{
	long numdat;
	int i, check, boxindex;

	wxString filename, filepath;
	wxString readline, numstring;

	TextFile infile;
	wxPoint pos;
    wxSize size;

	//filepath = GetPath();
	filepath = "Init//";

	// Box Load
	filename = "mainbox.ini";

	check = infile.Open(filepath + filename);
	if(!check) return;
	readline = infile.ReadLine();
	//tofp.WriteLine(readline);
	while(!readline.IsEmpty()) {
		numstring = readline.BeforeFirst(' ');
		numstring.ToLong(&numdat);
		boxindex = numdat;
		if(boxindex >= toolset->numtools) break;

		pos.x = ReadNextData(&readline);
		pos.y = ReadNextData(&readline);
		size.x = ReadNextData(&readline);
		size.y = ReadNextData(&readline);
		if(toolset->box[boxindex]->servant) toolset->box[boxindex]->visible = (bool)ReadNextData(&readline);
		else toolset->box[boxindex]->visible = true;

		if(pos.x >= -5000 && pos.x < 5000 && pos.y >= -5000 && pos.y < 5000) toolset->box[boxindex]->mpos = pos;
		if(size.x >= 50 && size.x < 2000 && size.y >= 50 && size.y < 2000) toolset->box[boxindex]->boxsize = size;

		readline = infile.ReadLine();          // Read next line
		//tofp.WriteLine(readline);
	}
	infile.Close();

	for(i=0; i<toolset->numtools; i++) {
		toolset->box[i]->ReSize();
		toolset->box[i]->Show(toolset->box[i]->visible);
	}
}


void MainFrame::MainStore()
{
	int i;
	wxString filename, filepath;
	wxString outline, text;

	TextFile outfile, opfile;

	//filepath = GetPath();
	filepath = "Init//";

	// box store
	filename = "mainbox.ini";
	outfile.New(filepath + filename);
	
	for(i=0; i<toolset->numtools; i++)
		if(toolset->box[i]) {
			outfile.WriteLine(text.Format("%d %d %d %d %d %d", i, 
			toolset->box[i]->mpos.x, toolset->box[i]->mpos.y, toolset->box[i]->boxsize.x, toolset->box[i]->boxsize.y, toolset->box[i]->IsVisible()));
		}
	outfile.Close();
}


long ReadNextData(wxString *readline)
{
	long numdat;
	wxString numstring;

	*readline = readline->AfterFirst(' ');
	numstring = readline->BeforeFirst(' ');
	numstring.ToLong(&numdat);

	return numdat;
}


wxString numtext(double number, int places)
{
	wxString text, format;
	
	format.Printf("%%.%df", places);
	text.Printf(format, number);

	return text;
}


wxString numchar(int number)
{
	char text[10];
	wxString newtext;
	
	text[0] = number + 64;
	
	newtext.Printf("%s", text);
	return newtext;
}


int numplaces(double range)
{
	int places = 0;
	
	if(range <= 100) places = 1;
	if(range <= 10) places = 2;	
	if(range <= 1) places = 3;
	if(range <= 0.1) places = 4;	
	
	return places;
}


wxString numstring(double number, int places=0)
{
	wxString snum;
	
	if(places == 1) snum.Printf("%.1f", number);
	else if(places == 2) snum.Printf("%.2f", number);
	else if(places == 3) snum.Printf("%.3f", number);
	else if(places == 4) snum.Printf("%.4f", number);
	else if(places == 5) snum.Printf("%.5f", number);
	else if(places == 6) snum.Printf("%.6f", number);
	else if(places == 7) snum.Printf("%.7f", number);
	else if(places == 8) snum.Printf("%.8f", number);
	else if(places == 9) snum.Printf("%.9f", number);
	else snum.Printf("%.0f", number);
	
	return snum;
}


wxString ColourString(wxColour col, int type)
{
	wxString colstring;

	if(type == 0) return colstring.Format("%.4f %.4f %.4f", (double)col.Red()/255, (double)col.Green()/255, (double)col.Blue()/255);
	else return colstring.Format("%d %d %d", col.Red(), col.Green(), col.Blue());
}


double ParseDouble(wxString *readline, wxUniChar tag)
{
	wxString numstring;
	double numdat;

	*readline = readline->AfterFirst(tag);
	readline->Trim(false);
	numstring = readline->BeforeFirst(' ');
	numstring.ToDouble(&numdat);
	*readline = readline->AfterFirst(' ');
	return numdat;
}


long ParseLong(wxString *readline, wxUniChar tag)
{
	wxString numstring;
	long numdat;

	*readline = readline->AfterFirst(tag);
	readline->Trim(false);
	numstring = readline->BeforeFirst(' ');
	numstring.ToLong(&numdat);
	*readline = readline->AfterFirst(' ');
	return numdat;
}


wxString ParseString(wxString *readline, wxUniChar tag)
{
	wxString string;
	long numdat;

	*readline = readline->AfterFirst(tag);
	readline->Trim(false);
	string = readline->BeforeFirst(' ');
	*readline = readline->AfterFirst(' ');
	return string;
}


TextFile::TextFile()
{
	file = NULL;
	readonly = true;
}


TextFile::~TextFile()
{
	if(file != NULL) delete file;
}


bool TextFile::End()
{
	return file->Eof();
}


int TextFile::Exists(wxString name)
{
	int check = 0;

	file = new wxTextFile(name);
	if(file->Exists()) check = 1;
	delete file;
	file = NULL;
	return check;
}


void TextFile::New(wxString name)
{
	file = new wxTextFile(name);
	if(!file->Exists()) file->Create();
	file->Open();
	file->Clear();
	readonly = false;
}


int TextFile::Open(wxString name, bool read)
{
	readonly = read;
	file = new wxTextFile(name);
	if(!file->Exists()) return 0;
	file->Open();
	unread = true;
	return 1;
}


void TextFile::Flush()
{
	file->Write();
}


void TextFile::WriteLine(wxString text)
{
	file->AddLine(text);
}


void TextFile::MoveTo(double x, double y)
{
	WriteLine(txt.Format("%.2f pu %.2f pu moveto", x, y));
}


void TextFile::LineTo(double x, double y)
{
	WriteLine(txt.Format("%.2f pu %.2f pu lineto", x, y));
}


void TextFile::DrawLine(double xf, double yf, double xt, double yt)
{
	WriteLine(txt.Format("%.2f pu %.2f pu moveto", xf, yf));
	WriteLine(txt.Format("%.2f pu %.2f pu lineto", xt, yt));
}


void TextFile::DrawText(wxString text, double x, double y)
{
	WriteLine(txt.Format("%.2f pu %.2f pu moveto", x, y));
	WriteLine(txt.Format("(%s) show", text));
}


void TextFile::SetColour(wxString col)
{
	WriteLine(txt.Format("%s setrgbcolor", col)); 
}


wxString TextFile::ReadLine()
{
	if(unread) {
		unread = false;
		return file->GetFirstLine();
	}
	else return file->GetNextLine();	
}


void TextFile::Close()
{
	if(!readonly) file->Write();
	file->Close();
	delete file;
	file = NULL;
}


datint::datint()
{
	zero = 0;
}


datdouble::datdouble(wxTextCtrl *text)
{
	zero = 0;
	maxindex = 0;
	textbox = text;
}


datdouble::datdouble(int size)
{
	zero = 0;
	data.resize(size * 1.1);
	max = size;
	textbox = NULL;
	maxindex = 0;
}


double *initfarray(int size)
{
  return (double *)malloc(size * sizeof(double));
}


double gaussian(double mean, double sd)
{
	double u1, u2;
	double v1, v2;
	double s;
	double g1, g2;

	v1 = 0;
	v2 = 0;
	s = 1;
	while(s>=1) {
		u1 = mrand01();                    /* U1=[0,1] */
		u2 = mrand01();										/* U2=[0,1] */          
		v1 = 2 * u1 - 1;								/* V1=[-1,1] */
		v2 = 2 * u2 - 1;								/* V2=[-1,1] */
		s = v1 * v1 + v2 * v2;
	}	
	g1 = sqrt(-2 * log(s) / s) * v1;
	g2 = sqrt(-2 * log(s) / s) * v2;
	
	g1 = mean + sd * g1;
	g2 = mean + sd * g2;
	
	return g1;
}


double uniform(double mean, double range)
{
	double r1, r2;
	
	r1 = mrand01();
	r2 = (mean - range) + (range * 2 * r1);
	
	return r2;
}


int iuniform(int base, int range)
{
	double r1, r2;
	
	r1 = mrand01();
	r2 = base + (range+1) * r1;
	
	return (int)r2;
}


int randint(int range)
{
	double num;

	num = mrand01();
	return (int)(num * range);
}


float fast_tanh(float x) {
  float x2 = x * x;
  float a = x * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
  float b = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + x2 * 28.0f));
  return a / b;
}


double kapow(double base, int power) {
	int i;
	double num;

	num = base;
	for(i=1; i<power; i++) num = num * base; 
	return num;
}


int GetSystem()
{
	wxString oslabel = wxGetOsDescription();
	if(oslabel.StartsWith("Windows")) return 1;
	if(oslabel.StartsWith("Mac")) return 2;
	if(oslabel.StartsWith("Linux")) return 3;
	return 0;
}