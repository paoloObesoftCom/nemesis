 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team
 http://www.genesi-project.it/forum
 Please if you use this source, refer to Nemesis -Team

 A part of this code is based from: enigma cvs
 Special thanks a tension for this code
 ****************************************************************************************************
 */
#ifndef __autostandby_h
#define __autostandby_h

#include <lib/gui/ewindow.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/statusbar.h>

class eAutoStandbySelWindow: public eWindow
{
	eCheckbox *stand, *shut;
	eStatusBar *sbar;
	void standChanged(int);
	void shutChanged(int);
public:
	eAutoStandbySelWindow();
	~eAutoStandbySelWindow();
};

#endif
