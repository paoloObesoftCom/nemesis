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
#ifndef __inadyn_h
#define __inadyn_h

#include <lib/gui/ewindow.h>
#include <lib/gui/eskin.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/eprogress.h>
#include <lib/gui/emessage.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/textinput.h>
#include <lib/gui/combobox.h>

class eInadyn: public eWindow
{
	static eInadyn *instance;
	eString iUser;
	eString iPwd;
	eString iDomain;
	int iPeriod,iUseLog;
	int isStarted;
	eString iDyn;
	eLabel *label[5],*labelstatus;
	eTextInputField *input[4];
	eComboBox *serviceType;
	eButton *bt_exit, *bt_save, *bt_startStop, *bt_log;
	eCheckbox *useLog;
	eStatusBar *sbar;
	void startStop();
	void start();
	void stop();
	void save();
	void showLog();
	void getParameter();
	void setParameter();
public:
	static eInadyn *getInstance() { return instance; }
	void init();
	eInadyn();
	~eInadyn();
};

#endif
