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
#ifndef __swap_h
#define __swap_h

#include <fstream>

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/listbox.h>
#include <lib/gui/combobox.h>
#include <lib/gui/emessage.h>
#include <lib/gui/statusbar.h>
#include <lib/gdi/font.h>

#include <enigma.h>

using namespace std;

class eSwap: public eWindow
{
	eButton *bt_exit, *bt_delete;
	eLabel *label, *labelspace, *status;
	eStatusBar *sbar;
	eListBox<eListBoxEntryText> *swapList;
	eComboBox *combo_size;
	void swapSelected(eListBoxEntryText *item);
	long swapSize;
	bool HDD, CF ,USB;
	void selectSize(eListBoxEntryText *le);
	bool readPartitions();
	void loadPartList();
	eString readFile(eString filename);
	eString getConfigSwapFile(void);
	void deleteSwap();
	void deactivateSwapFile(eString swapFile);
	void activateSwapFile(eString swapFile);
	void setSwapFile(int nextswapfile, eString swapFile);
	bool createSwapFile(eString swapFile);
	void setStatus(const eString &string);
	void getStatus();

public:
	eSwap();
	~eSwap();
};

#endif

#endif // DISABLE_FILE
#endif // DISABLE_HDD
