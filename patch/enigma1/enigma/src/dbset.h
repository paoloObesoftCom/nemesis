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
#ifndef __dbset_h
#define __dbset_h

#include <setup_window.h>

class eDbset: public eSetupWindow
{
	void remoteToggle(bool newState);
	void pvrToggle(bool newState);
	void pvToggle(bool newState);
	void saveConf(bool newState);
	void piconToggle(bool newState);
public:
	eDbset();
	~eDbset();
};

#endif
