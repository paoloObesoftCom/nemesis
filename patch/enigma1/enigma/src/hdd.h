#ifndef DISABLE_HDD
#ifndef DISABLE_FILE
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
#ifndef __hdd_h
#define __hdd_h

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/eskin.h>
#include <lib/gui/emessage.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/textinput.h>
#include <lib/gui/listbox.h>
#include <sys/vfs.h>

class eButton;
class eStatusBar;

class eHddSetup: public eListBoxWindow<eListBoxEntryText>
{
	int nr;
	void selectedHarddisk(eListBoxEntryText *sel);
public:
	eHddSetup();
	int getNr() const { return nr; }
};

class eHddMenu: public eWindow
{
	eButton *bt_set, *bt_start, *bt_standby, *bt_abort;
	eTextInputField *sleepTime, *silent;
	eLabel *status, *model, *capacity, *bus, *lfs, *label[6];
	eStatusBar *sbar;
	int dev;
	int numpart;
	void readStatus();
	void setStandby();
	void startHdd();
	void setParam();
public:
	eHddMenu(int dev);
};

#endif

#endif // DISABLE_FILE
#endif // DISABLE_HDD
