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
#ifndef __sysinfo_h
#define __sysinfo_h

#include <setup_window.h>
#include <lib/gui/emessage.h>

struct ScriptsList {
	int id;
	eString name;
	eString cmd;
	eString info;
};

class eSysInfo: public eSetupWindow
{
	std::list<ScriptsList> scriptsList;
	bool loadScripts();
	void scriptExec(int);
public:
	eSysInfo();
	~eSysInfo();
};

#endif
