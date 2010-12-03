 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team
 http://www.genesi-project.it/forum
 Please if you use this source, refer to Nemesis -Team

 A part of this code is based from: apps/tuxbox/plugins/enigma/script/script.h
 ****************************************************************************************************
 */
#ifndef __ecminfo_h
#define __ecminfo_h

#include <stdio.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/eskin.h>
#include <lib/gdi/font.h>
#include <lib/gui/elabel.h>
#include <lib/gui/eprogress.h>
#include <lib/gui/emessage.h>
#include <lib/gui/guiactions.h>
#include <lib/system/info.h>

#define sW 720		//larghezza in pixels
#define sH 576		//larghezza in pixels

class ecmInfo: public eWindow
{
	eLabel *label;
	eWidget *visible;
	eProgress *scrollbar;
	eButton *bt_abort,*bt_pid,*bt_ecm,*bt_emm;
	int pageHeight;
	int total;
	int eventHandler(const eWidgetEvent &event);
	void updateScrollbar();
	void toggleWordwrap(int state);
	void readInfo();
	void readPidInfo();
	void readEmmInfo();
	void setPid();
	void setEcm();
	void setEmm();
	eTimer refreshTimer;
	Connection refreshTimerConnection;

public:
	ecmInfo();
	~ecmInfo();
};
#endif
