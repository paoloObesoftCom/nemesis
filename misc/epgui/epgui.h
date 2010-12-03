#define NOZAP
#define RUDREAM

#include <plugin.h>
#include <stdio.h>
#include <enigma_main.h>
#include <timer.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/emessage.h>
#include <lib/gui/listbox.h>

#include <lib/gui/echeckbox.h>
#include <lib/gui/combobox.h>

#include <lib/gui/textinput.h>

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <lib/dvb/edvb.h>

/************************************************************************/
/*                        INI KEY DEFINES                               */
/************************************************************************/ 

#define OUTLOG "stdout"
#define SEPARATOR '|'
#define INI "/var/tuxbox/config/epgui.conf"
#define DUMMYFILE "/tmp/epgui.tmp"

#define INILANG "/var/tuxbox/config/epgui_lang.conf"

#define DEFFILENAME "/tmp/epgui.dat"
#define CURRENTTIMETRESHOLD 0
#define INIKEY_FILENAME "epgFileName"

#define INIKEY_SAVETIMEOFFSET "epgSaveTimeOffset"
#define INIKEY_STARTOFFSETTIMER "startOffsetTmr"
#define INIKEY_ENDOFFSETTIMER "endOffsetTmr"
#define INIKEY_EPGOFFSET "localOffset"
#define INIKEY_DAYLIGHTSAVE "dayLightSaving"

#define INIKEYLB_PREVVIEW "lbPrevView"
#define INIKEYLB_DOWNLOADEPG "lbDownloadEPG"
#define INIKEYLB_UPDATEEPG "lbUpdateEPG"
#define INIKEYLB_UPDATEEPGLM "lbUpdateEPGLowMem"
#define INIKEYLB_CONFIGUREPLUGIN "lbConfigurePlugin"
#define INIKEYLB_EXIT "lbExit"
#define INIKEYLB_TEST1 "lbGoToChannel"
#define INIKEYLB_GENERE "lbGenere"
#define INIKEYLB_DURATA "lbDuration"
#define INIKEYLB_RECORDIT "lbRecordIt"
#define INIKEYLB_NORECORDIT "lbNoRecordIt"
#define INIKEYLB_PROGDONE "lbTimerProgrammed"
#define INIKEYLB_NEXTVIEW "lbNextView"
#define INIKEYLB_READINGTO "lbReadingTimeOffset"
#define INIKEYLB_READINGTITLES "lbReadingTitles"
#define INIKEYLB_READINGTHEMES "lbReadingThemes"
#define INIKEYLB_READINGSUMMARIES "lbReadingSummaries"
#define INIKEYLB_READINGCN "lbReadingChannelNames"
#define INIKEYLB_PLEASEWAIT "lbPleaseWait"
#define INIKEYLB_UTILS "lbUtils"
#define INIKEYLB_SEARCH "lbSearch"
#define INIKEYLB_TITLEFILTER "lbTitleFilter"
#define INIKEYLB_GENEREFILTER "lbGenereFilter"
#define INIKEYLB_DESCFILTER "lbDescFilter"
#define INIKEYLB_ALLFILTER "lbFTFilter"
#define INIKEYLB_NOFILTER "lbDisableFilter"
#define INIKEYLB_FILTERKEY "lbFilterString"
#define INIKEYLB_LBCHANNELVIEW "lbChannelView"
#define INIKEYLB_LBALLVIEW "lbAllView"
#define INIKEYLB_LBTHEMEVIEW "lbThemeView"
#define INIKEYLB_LBGRIDVIEW "lbGridView"
#define INIKEYLB_LBHELP "lbHelp"
#define INIKEYLB_DOM "lbSun"
#define INIKEYLB_LUN "lbMon"
#define INIKEYLB_MAR "lbTue"
#define INIKEYLB_MER "lbWed"
#define INIKEYLB_GIO "lbThu"
#define INIKEYLB_VEN "lbFri"
#define INIKEYLB_SAB "lbSat"
#define INIKEYLB_SAVEOPTIONS "lbSaveOptions"
#define INIKEYLB_RELOADOPTIONS "lbReloadOptions"
#define INIKEYLB_CONFEPGFILENAME "lbConfEpgFilename"
#define INIKEYLB_CONFSTARTOFFSET "lbConfStartOffset"
#define INIKEYLB_CONFENDOFFSET "lbConfEndOffset"
#define INIKEYLB_CONFGRIDVIEWITEMS "lbConfGridViewItems"
#define INIKEYLB_CONFVIEWORDER "lbConfViewOrder"
#define INIKEYLB_CONFFOCUSPEG "lbConfFocusEpg"
#define INIKEYLB_CONFSHOWEVENTALL "lbConfShowEventAll"
#define INIKEYLB_CONFSHOWEVENTCHAN "lbConfShowEventChan"
#define INIKEYLB_EPGONUSB "lbEpgOnUSB"
#define INIKEYLB_EPGONHD "lbEpgOnHD"
#define INIKEYLB_EPGONCF "lbEpgOnCf"
#define INIKEYLB_EPGONTMP "lbEpgOnTmp"

#define DVBDEVICE "dvbDevice"
#define DAYSEPARATOR "daySeparator"
#define INIKEY_CHANNELLISTX "channelListPosX"
#define INIKEY_CHANNELLISTY "channelListPosY"
#define INIKEY_CHANNELLISTSX "channelListSizeX"
#define INIKEY_CHANNELLISTSY "channelListSizeY"
#define INIKEY_CHANDETAILX "channelDetailPosX"
#define INIKEY_CHANDETAILY "channelDetailPosY"
#define INIKEY_CHANDETAILSX "channelDetailSizeX"
#define INIKEY_CHANDETAILSY "channelDetailSizeY"
#define INIKEY_EPGSELLISTX  "epgSelPosX"
#define INIKEY_EPGSELLISTY  "epgSelPosY"
#define INIKEY_EPGSELLISTSX  "epgSelSizeX"
#define INIKEY_EPGSELLISTSY  "epgSelSizeY"
#define INIKEY_MULTIEPG "multiEpg"
#define INIKEY_FOCUSEPG "focusEpg"
#define INIKEY_MULTIEPGLB "multiEpgLabel"
#define INIKEY_MULTIEPGFN "multiEpgFile"
#define INIKEY_SERVICESFILENAME "ServiceFileName"
#define INIKEY_TIMERFILENAME "TimerFileName"
#define INIKEY_STARTENIGMA "startEnigmaCmd"
#define INIKEY_NOSUMMARIES "noSummaries"
#define INIKEY_NOTHEMES "noThemes"
#define INIKEY_DATEFORMAT "dateDisplayFormat"
#define INIKEY_WAITPLEASEX "pleaseWaitPosX"
#define INIKEY_WAITPLEASEY "pleaseWaitPosY"
#define INIKEY_WAITPLEASESX "pleaseWaitPosSX"
#define INIKEY_WAITPLEASESY "pleaseWaitPosSY"
#define INIKEY_EPGADDRESS "epgAddress"
#define INIKEY_EPGADDRESSLM "epgAddressLM"
#define INIKEY_EDITCHARTIMEOUT "nextCharTimeout"
#define INIKEY_SHOWPASTALL "showPastEvent_all"
#define INIKEY_SHOWPASTCHANNEL "showPastEvent_chan"
#define INIKEY_SHOWFWDALL "showEventN_all"
#define INIKEY_SHOWFWDCHANNEL "showEventN_chan"
#define INIKEY_VIEWORDER "viewOrder"
#define INIKEY_GRIDELEMENTS "gridViewItems"
#define INIKEY_MAXPROGSTODOWNLOAD "maxMhwProgs"
#define INIKEY_MAXPROGRAMSTOLOAD "maxProgsToLoad"
#define INIKEY_HTTPUSER "HTTPuser"
#define INIKEY_HTTPPASSWORD "HTTPpassword"

#define AUTOSAVE_FILTERKEY "asFilterKey"
#define AUTOSAVE_FILTERTYPE "asFilterType"

#define DEFAULT_MAXMHWPROGS "100000"
#define SERVICESFILENAME "/var/tuxbox/config/enigma/services"
#define TIMERFILENAME "/var/tuxbox/config/enigma/timer.epl"
#define SERVICESEPARATOR ':'

#define DEFAULT_VIEW_ORDER "0123"
#define BYESTRING "EPGui Viewer - By Luca Spallarossa (www.spallared.com) - Freeware\nModified by Gianathem\nFor support and suggestions visit forum at:\nhttp://www.dreamboxonline.com"

#define MAXTXT 1024
#define MAXTITLELEN 100
#define MAXGENERELEN 100

#define MAXTHEMES 128

#define MAX_CHANNELS 500
#define MAX_PROGRAMS 100000

#define MAX_SECTION_BUFFER 4096
#define DISPLAYMODES 6

/************************************************************************/
/*                       STRUCTURES AND ENUMS DEFINITION                */
/************************************************************************/

#define bcdtoint(i) ((((i & 0xf0) >> 4) * 10) + (i & 0x0f))

struct sProgramDescription 
{
	char title[MAXTITLELEN];
	char genere[MAXGENERELEN];
	int duration;
	int DescriptionPos;
	time_t btime;
};

struct mThread
{
	char *answer;
	char command[MAXTXT];
	bool completed;
};

struct mTimer
{
	eLabel *lb_status;
	bool exitloop;
};

struct chan_info 
{
	time_t time;
	int idx;
};

char day_names[8][15];
char EPGfilename[MAXTXT];
long MAXPROGS;
mThread mThreadOptions;
char sCurrentEnigmaChannel[MAXTXT];

enum searchType { no_search=0, genere=1, etitle=2, description=3, all=4 };
enum displayMode { channel_view=0, grid_view=1, allprog_view=2, themes_view=3};

extern "C" int plugin_exec( PluginParam *par );

eString mSearchKey;
searchType mSearchType;
char *ReadINIValue(char *vname);
char *ReadINIValue(char *KeyName, char *DefaultValue);
char *StripZero(char *string);
void *ShowTime(void *arg);
time_t GetTransponderTime(void);
bool ShallDisplay(sProgramDescription *pd, char *mcSearchKey, searchType SearchType);
void myerror(char *errortxt);
void myerror(char *errortxt, int d);
void myerror(char *errortxt, char *s);
char *StripEndl(char *string);
bool SearchSubstring(char *str, char *substr);
char *GetDescription(long pos);
int FormatEPGElement(sProgramDescription *pd, char *element);
eListBoxEntryText *AddListboxElement(eListBox<eListBoxEntryText> *lb, char *txt, void *key);
bool CheckOverlap(int start, int duration);
int DoEPGRecord(int channel, int ProgNumber, sProgramDescription *ProgramDescription, int timeZone);
void ShowProgramDetail(int progID, int channel, sProgramDescription *ProgramDescription, int timeZone);
bool LookupServiceByChannel(char *channelID, char *serviceID);
bool LookupChannelByService(char *channelID, char *serviceID);
bool ParseServiceID(char *sid_in, char *sid_out);
int SaveINIValue(char *KeyName, char *Value);
int SaveINIValue(char *KeyName, eString Value);
int SaveINIValue(char *KeyName, int Value);
int ZapIT(int channel, sProgramDescription *pd);
int FindChannelFromProg(int prog_num, sProgramDescription *pd);
char *HttpParse(char *str);
int ReadChanName();

class eChannelEpgDetail: public eWindow
{
private:
	int timeZone;
	long ProgramsCount;
	int counter;
	searchType mSearchMode;
	char mcSearchKey[MAXTXT];
	long channel;
	eListBox<eListBoxEntryText> *listbox;
	eButton *bt_abort, *bt_ok, *bt_zap;
	sProgramDescription *ProgramDescription;
	int useDaySeparator;
	bool ProgRecord;
	int current_sel_program;
	time_t now;
	void selectedItem(eListBoxEntryText *item);
	void selectionChanged(eListBoxEntryText *item);

public:
	void OnZap();
	void SetProgramsCount(long pc);
	void SetProgramDescription(sProgramDescription *pd);
	void SetTime(time_t mNow);
	void SetSearchKey(eString Key);
	void SetSearchMode(searchType mSm);
	int GetProgRecord(void);
	void SetChannel(long channel_pos);
	int FillChannelEPG();
	eChannelEpgDetail();
	~eChannelEpgDetail();
};

class eWaitPlease: public eWindow
{
	eLabel *lb_status;

public:
	void SetValue(int v);
	void SetMax(int m);
	eWaitPlease();
	// the destructor.
	~eWaitPlease();
	void SetMessage(char *msg);
	
private:
	int maxvalue;
	char message[MAXTXT];
};

class eConfigurationDlg: public eWindow
{
public:
	void OnEpgOnHd();
	void OnEpgOnCf();
	void OnEpgOnUsb(void);
	void OnEpgOnTmp(void);
	void OnReloadIni(void);
	void OnExit(void);
	void OnSaveOptions(void);

	eConfigurationDlg();
	// the destructor.
	~eConfigurationDlg();
	
private:
	void FillComboboxes();
	void ReloadFromINI();
	eCheckbox *cbFocusEpg;
	eCheckbox *cbShowPastEventAll;
	eCheckbox *cbShowPastEventChan;
	eComboBox *cbDaySeparator;
	eButton *btSaveOptions;
	eButton *btReloadIni;
	eButton *btExit;
	eButton *btEpgOnHd;
	eButton *btEpgOnUsb;
	eButton *btEpgOnCf;
	eButton *btEpgOnTmp;
	eTextInputField *lbEpgFile;
	eTextInputField *lbStartOffsetTimer;
	eTextInputField *lbEndOffsetTimer;
	eTextInputField *lbGridViewItems;
	eTextInputField *lbViewOrder;
	eTextInputField *lbShowNumEventAll;
	eTextInputField *lbShowNumEventChan;
	eLabel *lbEpgFile_help;
	eLabel *lbStartOffsetTimer_help;
	eLabel *lbEndOffsetTimer_help;
	eLabel *lbGridViewItems_help;
	eLabel *lbViewOrder_help;
	eLabel *lbShowNumEventAll_help;
	eLabel *lbShowNumEventChan_help;
	
};

class eSearch: public eWindow
{
	eTextInputField *lb_search;
	eButton *bt_searchTitle;
	eButton *bt_searchGenere;
	eButton *bt_searchDescriptions;
	eButton *bt_searchAll;
	eButton *bt_no_search;
	eLabel *lb_status;

public:
	eSearch();
	// the destructor.
	~eSearch();
	void SetpSearchKey(eString *ps);
	void SetpSearchMode(searchType *ps);
	
private:
	enum searchType *pSearchType;
	eString *pSearchKey;
	void SearchTitle(void);
	void SearchGenere(void);
	void SearchDescriptions(void);
	void SearchAll(void);
	void NoSearch(void);
};

class eChannelListDialog: public eWindow
{
	int counter;
	eListBox<eListBoxEntryText> *ChanListbox;
	eListBox<eListBoxEntryText> *GridListbox;
	eListBox<eListBoxEntryText> *AllProgListbox;
	eListBox<eListBoxEntryText> *ProgByThemeListbox;
	eButton *bt_prevview, *bt_utils, *bt_modeepg, *bt_search;
	eButton *bt_hidden1,*bt_hidden2,*bt_hidden3,*bt_hidden4, *bt_hidden_searchT, *bt_hidden_searchG, *bt_hidden_zapit;
	void OnChannelListSelected(eListBoxEntryText *item);
	void OnChannelListChange(eListBoxEntryText *item);
	void OnGridListSelected(eListBoxEntryText *item);
	void OnGridListChange(eListBoxEntryText *item);
	void OnThemeListSelected(eListBoxEntryText *item);
	void OnThemeListChange(eListBoxEntryText *item);
	void OnAllListSelected(eListBoxEntryText *item);
	void OnAllListChange(eListBoxEntryText *item);
	void OnView1(void);
	void OnView2(void);
	void OnView3(void);
	void OnView4(void);
	void openTimer();
	void OnZapIt(void);
	bool ProgRecord;
	void OnShowUtils(void);
	void OnSearchIt(void);
	eLabel *lb_status, *lb_help;
	displayMode display_mode;
	int current_sel_program;
	int current_channel;
	int ProgramsCount;
	int FillGridEPG(time_t now);
	sProgramDescription *ProgramDescription;
	int ReadEpgFile(void);
	int current_display_mode;
	int display_mode_order[DISPLAYMODES];

public:
	void OnPrevViewMode();
	void InitDialog();
	bool GetProgRecord(void);
	int FillChannelList(void);
	eChannelListDialog();
	~eChannelListDialog();

private:
	void FocusListbox();
	char *RefreshSearchMode();
	void FillAllProgs(time_t now);
	bool UpdatingListboxes;
	int FillThemes(void);
	void RefreshWindowMode(void);
	void NextViewMode(void);
	time_t now;
	bool ThemesListBoxInit;
	bool GridListBoxInit;
	bool AllListBoxInit;
	bool FillChannelListInit;
	bool search_dirty_flag;
};
