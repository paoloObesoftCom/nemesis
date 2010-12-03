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
#ifndef __deamon_h
#define __deamon_h

#include <stdio.h>
#include <dirent.h>
#include <setup_window.h>
#include <lib/gui/emessage.h>

struct DeamonList {
	int id;
	eString command;
	eString binaName;
	eString fullBinaName;
};

class eDeamon: public eSetupWindow
{
	std::list<DeamonList> deamonItem;
	char deamon[100];
	eString readDeamonBin(eString bin);
	eString readDeamonName(eString name);
	eString readDeamonPath(eString path);
	void startStop(bool newState, int key);
public:
	eDeamon();
	~eDeamon();
};

#endif
