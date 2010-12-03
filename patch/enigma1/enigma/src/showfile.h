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
#ifndef __showfile_h
#define __showfile_h

#include <stdio.h>
#include <dirent.h>

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/eskin.h>
#include <lib/gui/listbox.h>
#include <lib/gdi/font.h>

class cbpShowFile: public eWindow
{
	static cbpShowFile *instance;
	eListBox<eListBoxEntryText> *listbox;
	eButton *bt_abort;
	eTimer refreshTimer;
	Connection refreshTimerConnection;
	void Listeselected(eListBoxEntryText *item);
	void readInfo(char *eTxt);
public:
	static cbpShowFile *getInstance() { return instance; }
	void showLog(eString eTitle, char *eTxt, long iDelay);
	cbpShowFile(eString title, char *eTxt, long iDelay);
	~cbpShowFile();
};

class cpbShowDetail: public eWindow
{
	eLabel *labeldet;
	eButton *bt_abort;
public:
	cpbShowDetail(eString);
	~cpbShowDetail();
};

#endif
