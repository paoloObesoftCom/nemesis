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
#ifndef __emuinfo_h
#define __emuinfo_h

#include <stdio.h>
#include <lib/gui/emessage.h>
#include <setup_window.h>

struct EmuScriptsList {
	int id;
	eString name;
	eString cmd;
	eString info;
};

class eEmuInfo: public eSetupWindow
{
	std::list<EmuScriptsList> emuScriptsList;
	bool loadScripts();
	void execCommand(int);
public:
	eEmuInfo();
	~eEmuInfo();
};

#endif
