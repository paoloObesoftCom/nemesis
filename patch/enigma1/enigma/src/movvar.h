#ifndef DISABLE_MOVEVAR
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
#ifndef __movvar_h
#define __movvar_h

#include <fstream>

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/listbox.h>
#include <lib/gui/emessage.h>
#include <lib/gui/statusbar.h>
#include <lib/gdi/font.h>
#include <lib/gui/echeckbox.h>

#include <enigma.h>

using namespace std;

class eMovvar: public eWindow
{
	eButton *bt_exit;
	eLabel *label, *status;
	eStatusBar *sbar;
	eListBox<eListBoxEntryText> *movList;
	eCheckbox *cpVar;
	void movSelected(eListBoxEntryText *item);
	bool HDD, CF ,USB;
	bool readPartitions();
	void loadPartList();
	void setVarDir(eString varDir);
	eString getVarDir();	
	void removeVar();
	void moveVar(eString movDir);
	void setStatus(const eString &string);
	void getStatus();
	bool getVarStatus();

public:
	eMovvar();
	~eMovvar();
};

#endif
#endif // DISABLE_MOVEVAR
