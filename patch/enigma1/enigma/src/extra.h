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
#ifndef __extra_h
#define __extra_h

#include <setup_window.h>

class eExtra: public eSetupWindow
{
	void openInadynPanel();
#ifndef DISABLE_HDD
#ifndef DISABLE_FILE
	void openSwapPanel();
	void hddSetup();
#endif // DISABLE_FILE
#endif // DISABLE_HDD
	void openUserPanel();
	void openMovVarPanel();
	void openEpgPanel();
	void openFirewallPanel();
public:
	eExtra();
	~eExtra();
};

#endif
