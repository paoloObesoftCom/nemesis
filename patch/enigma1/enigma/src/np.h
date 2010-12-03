 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team
 http://www.genesi-project.it/forum
 Please if you use this source, refer to Nemesis -Team

 A part of this code is based from: enigma cvs, EasyEmu
 
 Special tanks for EasyEmu author: BlackCannon
 ****************************************************************************************************
 */
#ifndef __np_h
#define __np_h

#include <dirent.h>
#include <parentallock.h>

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/listbox.h>
#include <lib/gui/emessage.h>
#include <lib/system/info.h>
#include <lib/dvb/decoder.h>

#include <enigma_main.h>

#define VDF "v. 5.0"
#define NEMESISVER "5.0"
#define NEMESISVERDATE "01-10-2010"
#define CVSDATE "01-10-2010"
#define NUM_FUNC 9
#define sW 720		//larghezza in pixels
#define sH 576		//larghezza in pixels
#define mainW 404	//larghezza in pixels
#define mainH 365	//altezza in pixels

class eCbpMain: public eWindow
{
	char emuname[100];
	eString emulist[100];
	eString emuStarted;
	
	char srvname[20];
	eString srvlist[20];
	eString srvStarted;
	eButton *bt_func[NUM_FUNC];
	eLabel *label,*lb1, *lb2, *lb3, *lb4;
	eStatusBar *sbar;
	eListBox<eListBoxEntryText> *emulistbox;
	eListBox<eListBoxEntryText> *serverlistbox;
	eListBox<eListBoxEntryText> *textlist;
	void Listeselected(int key);
	void emulistbox_selected(eListBoxEntryText *item);
	void emulistbox_changed(eListBoxEntryText *item);
	void serverlistbox_selected(eListBoxEntryText *item);
	void serverlistbox_changed(eListBoxEntryText *item);
	void bt_start_emu();
	void bt_start_srv();
	void loadEmuList();
	void loadSrvList();
	void setArrowEmu();
	void setArrowSrv();
	eString readEmuName(eString emu);
	eString readEmuActive();
	eString readSrvName(eString emu);
	eString readSrvActive();
	bool readNews();
public:
	eCbpMain();
	~eCbpMain();
};

#endif
