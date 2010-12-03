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
#ifndef __userscript_h
#define __userscript_h

#include <dirent.h>

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/listbox.h>
#include <lib/gui/emessage.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/echeckbox.h>
#include <lib/gdi/font.h>
#include <lib/system/info.h>

class eScriptWindow: public eWindow
{
	eString scriptlist[100];
	int useCccd, confirmExec, showLog;
	eListBox<eListBoxEntryText> *scriptList;
	eButton *bt_exec,*bt_abort, *bt_save, *bt_del, *bt_exit;
	eLabel *label;
	eCheckbox *showlog, *confirmexec, *usecccd;
	eStatusBar *sbar;
	void Listeselected(eListBoxEntryText *item);
	void Listeselchanged(eListBoxEntryText *item);
	bool loadScriptList();
	void loadButton(bool empty);
	void executeScript();
	void saveScript();
	void setCheckBox();
	void deleteScript();
	int runScript(eString script, eString Title="Execute Script",int wsize=500, int hsize=370);
	void readScriptParameter(eString script);
	eString getScriptParameter(eString script, eString par);
	eString setScriptParameter(eString script, eString par, eString value);
public:
	eScriptWindow();
	~eScriptWindow();
};

#endif
