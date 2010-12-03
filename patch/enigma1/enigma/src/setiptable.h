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
#ifndef __setiptable_h
#define __setiptable_h

#include <lib/gui/enumber.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/emessage.h>
#include <lib/gui/textinput.h>
#include <enigma.h>

class eSetIptable: public eWindow
{
	eCheckbox *sysboot, *enalan;
	eLabel *labelstatus;
	eStatusBar *sbar;
	eButton *bt_exit, *bt_status, *bt_start, *bt_setting;
	void startIptable();
	void openSetting();
	void showStatus();
	void enalanChanged(int);
	void sysbootChanged(int);
	void rebootDb();
public:
	eSetIptable();
	~eSetIptable();
};

class eSetIptableSetup: public eWindow
{
	eString eUserIp[500];
	int iUserNum;
	eListBox<eListBoxEntryText> *userlistbox;
	eButton *bt_exit, *bt_add, *bt_delete;
	void addUser();
	void deleteUser();
	bool checkIP();
public:
	void loadUserList();
	eSetIptableSetup();
	~eSetIptableSetup();
};

class eSetIptableAdd: public eWindow
{
	eNumber *ip;
	eCheckbox *useip, *usename;
	eLabel *labelip, *labelcomment;
	eStatusBar *sbar;
	eTextInputField *input[2];
	eButton *bt_exit, *bt_add;
	void fieldSelected(int *number);
	void addUser();
	void useipChanged(int);
	void usenameChanged(int);
	bool checkRule(eString rule);
public:
	eSetIptableAdd();
	~eSetIptableAdd();
};

#endif


