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
#ifndef __addons_h
#define __addons_h

#include <stdio.h>
#include <dirent.h>

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/eprogress.h>
#include <lib/gui/listbox.h>
#include <lib/gui/emessage.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/textinput.h>
#include <lib/system/httpd.h>
#include <lib/dvb/edvb.h>
#include <setup_window.h>

#include <enigma_main.h>
#include <enigma.h>

struct eAddonsItemList {
	int id;
	eString item;
};

struct eAddonsFileList {
	int id;
	int itemid;
	eString filename;
	eString desc;
	eString dir;
	int size;
	int check;
};

class eHTTPDownload: public eHTTPDataSource
{
	int received;
	int total;
	int fd;
	eString filename;
public:
	eHTTPDownload(eHTTPConnection *c, const char *filename);
	Signal2<void,int,int> inUseBar; // received, total (-1 for unknown)
	~eHTTPDownload();
	void haveData(void *data, int len);
};

class eAddons: public eWindow
{
	eLabel *labelfile;
	eString tempPath, dir, current_url, filename;
	int lasttime;
	eHTTPConnection *http;
	eHTTPDataSource *createImageDataSink(eHTTPConnection *conn);
	eHTTPDownload *file;
	void loadFile(eString url, eString fileName);
	void setError(int error);
	void abortDownload();
	void fileTransferDone(int err);
	void downloadProgress(int received, int total);
	void setStatus(const eString &string);
	void downloadFile();
	eLabel *labelname, *labelspace;
	eButton *bt_abort, *bt_exit, *bt_down;
	eProgress *inUseBar;
	eStatusBar *status;
	eListBox<eListBoxEntryText> *namelist;
	eListBox<eListBoxEntryText> *filelist;
	void nameSelected(eListBoxEntryText *item);
	void nameChanged(eListBoxEntryText *item);
	void loadFileList(int key);
	eString getGroupName(int id);
	void removeSetting();
	void reloadSetting();
	bool loadAddons(eString file);
	void updateFree();
	void installAddon();
public:
	void serverAddonsConnect();
	void serverEmuConnect();
	eAddons();
	~eAddons();
};

class eChooseAddons: public eSetupWindow
{
	void openAddonsDownload();
	void openEmuDownload();
	void openDelete();
	void openManual();
	void openProxy();
public:
	eChooseAddons();
	~eChooseAddons();
};

class eDeleteAddons: public eWindow
{
	char filetodelete[100];
	eLabel *labelspace;
	eButton *bt_abort;
	eProgress *inUseBar;
	eString dellist[100];
	eListBox<eListBoxEntryText> *deleteList;
	void Listeselected(eListBoxEntryText *item);
	void loadDelList();
	void updateFree();
	void delRegKey(eString);
public:
	eDeleteAddons();
	~eDeleteAddons();
};

class eManualAddons: public eWindow
{
	char filetoinstall[100];
	eLabel *labelspace, *labelinfo;
	eButton *bt_abort, *bt_refresh;
	eProgress *inUseBar;
	eListBox<eListBoxEntryText> *manualList;
	void Listeselected(eListBoxEntryText *item);
	void loadManList();
	void updateFree();
public:
	eManualAddons();
	~eManualAddons();
};

class eProxySetting: public eWindow
{
	eTextInputField *url;
	eLabel *labelurl;
	eTextInputField *input[4];
	eLabel *label[4];
	eCheckbox *useProxy;
	eButton *bt_exit, *bt_save, *bt_def, *bt_clear;
	void saveProxySetting();
	void clearProxySetting();
	void setUrlDefault();
	eStatusBar *sbar;

public:
	eProxySetting();
	~eProxySetting();
};

#endif
