 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team
 http://www.genesi-project.it/forum
 Please if you use this source, refer to Nemesis -Team

 A part of this code is based from: enigma cvs
 ****************************************************************************************************
 */
#ifndef __osdinfo_h
#define __osdinfo_h

#include <dirent.h>
#include <stdio.h>
#include <string.h> 
#include <streaminfo.h>
#include <lib/base/estring.h>
#include <lib/dvb/dvbservice.h>
#include <lib/dvb/servicedvb.h>
#include <lib/dvb/decoder.h>
#include <enigma_dyn_utils.h>

class osdInfo: public eApplication, public Object
{
	static osdInfo *instance;
public:
	static osdInfo *getInstance();
	eString getCamName();
	eString getCsName();
	eString getEmuInfo();
	eString getInfoFileName();
	eString getVideoInfo();
	eString replace(eString s, eString f, eString r);
	int getCodifiche();
	void getData(char * caid, char * pid, char * provid, int * source,char * addr,char * port, int * ecmtime, int * hops, char * reader);
	void getLNBData(int * freq, int * sr, char * pol, char * fec, char * proname);
	void getFormattedData(char * data);
	void getFormattedDataShort(char * data);

	osdInfo();
	~osdInfo();
};

#endif
