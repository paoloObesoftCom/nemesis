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
#ifndef __epgmanager_h
#define __epgmanager_h

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/listbox.h>
#include <lib/gui/textinput.h>
#include <lib/gui/emessage.h>
#include <lib/gui/eskin.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/combobox.h>
#include <lib/base/ebase.h>
#include <lib/gui/guiactions.h>
#include <lib/dvb/serviceplaylist.h>
#include <enigma_standby.h>
#include <enigma.h>
#include <enigma_main.h>
#include <lib/driver/eavswitch.h>
#include <timer.h>
#include <lib/dvb/decoder.h>
#include <lib/dvb/epgcache.h>
#include <lib/dvb/frontend.h>
#include <lib/base/ebase.h>
#include <enigma_plugins.h>

class eEpgManager: public eWindow
{
	int numMHWChannel;
	int numOfTestChannel;
	eString sRefCur;
	eString sRefEpg;
	eString eChName[200];
	eString eChRef[200];
	eString eChDir[200];
	eTimer *epgZapTimer;
	static eEpgManager *instance;
	eCheckbox *chbemu, *chkfilter;
	eComboBox *comboRef;
	eLabel *label, *labelRef, *labelTime, *labelmin;
	eButton *bt_update, *bt_save, *bt_timer, *bt_add, *btEpg[4], *bt_filter;
	eTextInputField *cachedir, *timeTxt;
	eStatusBar *sbar;
	void updateEPG(int key);
	void saveParameters();
	bool saveTimeout();
	void saveDir();
	void getMvEpgData(bool epgExecuted=false,bool manual=false);
	void getEpguiEpgData(bool manual=false);
	void getWeekEpgData(bool manual=false);
	void openEpgTimer();
	void btSelected(int key);
	void zapTimeout(int key);
	void deleteEpgCache();
	eString readEpguiDir();
	eString getMvChRef(eString mvDir);
	bool readMvDir();
	eString readWeekEPGDir();
	void loadChannelList();
	void addTimerEvent();
	eString readEmuActive();
	void start_emu();
	void stop_emu();
	void openEpgFilter();
	void updateChannellist();
	bool checkLoader();
	int runScript(eString script,eString Title="Execute Script",int wsize=420, int hsize=310);
public:
	static eEpgManager *getInstance() { return instance; }
	void updateEpgEvent();
	void FinishedEpgEvent();
	eEpgManager();
	~eEpgManager();
};

class eEpgManagerTimer: public eWindow
{
	eCheckbox *chepg[6];
	eLabel *label;
	eButton *bt_exit,*bt_timer, *bt_add;
	eStatusBar *sbar;
	void cbSelected(int state, int key);
	void openTimer();
	void addTimerEvent(eServiceReference ref);
public:
	eEpgManagerTimer(eServiceReference ref,eString ch);
	~eEpgManagerTimer();
};

class eListBoxEntryMck: public eListBoxEntryText
{
		gPixmap *pm;
		bool checked;
		void LBSelected ( eListBoxEntry* t );
		const eString& redraw ( gPainter *rc, const eRect& rect, gColor coActiveB, gColor coActiveF, gColor coNormalB, gColor coNormalF, int state );
	public:
		eListBoxEntryMck ( eListBox<eListBoxEntryMck>* lb, const char* txt, void* key=0, int align=0, const eString& hlptxt="", int keytype = value );
		bool getCheck()
		{
			return checked;
		}
		void setCheck ( bool ncheck );
};

class eEpgManagerFilter: public eWindow
{
	char *fileName;
	int reordering;
	gColor selectedBackColor;
	eListBox<eListBoxEntryMck> *list;
	eButton *bt_up, *bt_channel;
	void changeStatus(eListBoxEntryMck *item);
	void readChannelList();
	void saveChannelList();
	void toggleMove();
	void updateLabel();
	void toggleChannels();
public:
	eEpgManagerFilter();
	~eEpgManagerFilter();
};


#endif
