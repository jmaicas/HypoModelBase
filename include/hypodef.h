

#ifndef HYPODEF_H
#define HYPODEF_H

//#define CORTMOD 1
//#define OSMOMOD 1
#define IGFMOD 1
#define VASOMOD 1
//#define HEATMOD 1
#define GHMOD 1
#define LYSISMOD 1
#define VMNMOD 1
#define AGENTMOD 1
#define TESTMOD 1


enum {
	Blank = 0,
	modIGF = 1,
	modVaso = 2,
	modVMN = 3,
	modCort = 4,
	modOsmo = 5,
	modHeat = 6,
  modGH = 7,
	modLysis = 8,
	modAgent = 9,
	modTest = 10
};

#endif