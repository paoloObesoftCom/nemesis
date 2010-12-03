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
#ifndef __setsyslog_h
#define __setsyslog_h

#include <lib/gui/enumber.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/emessage.h>
#include <lib/gui/combobox.h>
#include <lib/gui/textinput.h>
#include <enigma.h>

class eSetSyslog: public eWindow
{
	eString logsize, marktime; 
	int userotate, usemark, useremote, remoteport;
	eString remoteip, logpath;
	eNumber *ip, *port;
	eTextInputField *input[2];
	eCheckbox *sysboot, *sysenigma, *sysmark, *sysrotate, *sysremote;
	eStatusBar *sbar;
	eLabel *label, *labelstatus, *labelmin, *labelkb , *l1, *l2;
	eComboBox *syslogdir;
	eButton *bt_exit, *bt_open, *bt_start, *bt_reboot;
	void fieldSelected(int *number);
	void openLog();
	void startSyslog();
	void rebootDb();
	void deleteLog();
	void sysenigmaChanged(int);
	void sysbootChanged(int);
	void sysmarkChanged(int);
	void sysrotateChanged(int);
	void sysremoteChanged(int);
	void setParameter();
	void getParameter();
public:
	eSetSyslog();
	~eSetSyslog();
};

#endif


