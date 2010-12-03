#include "epgui.h"

extern int errno;

eConfigurationDlg::eConfigurationDlg()
{
	int sW = 720;
	int sH = 576;
	int winH = 350;
	int winW = 500;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText("Configuration Dialog");

	int posL=20;
	int posI=15;

	lbEpgFile_help=new eLabel(this);	
	lbEpgFile_help->move(ePoint(15, posL));
	lbEpgFile_help->resize(eSize(170,30));
	lbEpgFile_help->setText(ReadINIValue(INIKEYLB_CONFEPGFILENAME,"Epg File Name:"));

	lbEpgFile=new eTextInputField(this);
	lbEpgFile->move(ePoint(180, posI));
	lbEpgFile->resize(eSize(winW - 185,30));
	lbEpgFile->loadDeco();
	lbEpgFile->setNextCharTimeout(atoi(ReadINIValue(INIKEY_EDITCHARTIMEOUT,"1000")));
	lbEpgFile->setMaxChars(256);

	lbStartOffsetTimer_help=new eLabel(this);	
	lbStartOffsetTimer_help->move(ePoint(15, posL += 35));
	lbStartOffsetTimer_help->resize(eSize(170, 30));
	lbStartOffsetTimer_help->setText(ReadINIValue(INIKEYLB_CONFSTARTOFFSET,"Start Offset Timer:"));

	lbStartOffsetTimer=new eTextInputField(this);
	lbStartOffsetTimer->move(ePoint(180, posI += 35));
	lbStartOffsetTimer->resize(eSize(winW - 345,30)); // leave space for both buttons
	lbStartOffsetTimer->loadDeco();
	lbStartOffsetTimer->setNextCharTimeout(atoi(ReadINIValue(INIKEY_EDITCHARTIMEOUT,"1000")));
	lbStartOffsetTimer->setMaxChars(5);
	lbStartOffsetTimer->setUseableChars("-0123456789");

	lbEndOffsetTimer_help=new eLabel(this);	
	lbEndOffsetTimer_help->move(ePoint(15, posL += 35));
	lbEndOffsetTimer_help->resize(eSize(170,30));
	lbEndOffsetTimer_help->setText(ReadINIValue(INIKEYLB_CONFENDOFFSET,"End Offset Timer"));

	lbEndOffsetTimer=new eTextInputField(this);
	lbEndOffsetTimer->move(ePoint(180, posI += 35));;
	lbEndOffsetTimer->resize(eSize(winW - 345,30));
	lbEndOffsetTimer->loadDeco();
	lbEndOffsetTimer->setNextCharTimeout(atoi(ReadINIValue(INIKEY_EDITCHARTIMEOUT,"1000")));
	lbEndOffsetTimer->setMaxChars(5);
	lbEndOffsetTimer->setUseableChars("-0123456789");

	lbGridViewItems_help=new eLabel(this);	
	lbGridViewItems_help->move(ePoint(15, posL += 35));
	lbGridViewItems_help->resize(eSize(170,30));
	lbGridViewItems_help->setText(ReadINIValue(INIKEYLB_CONFGRIDVIEWITEMS,"Grid View Items:"));

	lbGridViewItems=new eTextInputField(this);
	lbGridViewItems->move(ePoint(180, posI += 35));;
	lbGridViewItems->resize(eSize(winW - 345,30));
	lbGridViewItems->loadDeco();
	lbGridViewItems->setNextCharTimeout(atoi(ReadINIValue(INIKEY_EDITCHARTIMEOUT,"1000")));
	lbGridViewItems->setMaxChars(2);
	lbGridViewItems->setUseableChars("0123456789");

	lbViewOrder_help=new eLabel(this);	
	lbViewOrder_help->move(ePoint(15, posL += 35));
	lbViewOrder_help->resize(eSize(170,30));
	lbViewOrder_help->setText(ReadINIValue(INIKEYLB_CONFVIEWORDER,"View order:"));

	lbViewOrder=new eTextInputField(this);
	lbViewOrder->move(ePoint(180, posI += 35));;
	lbViewOrder->resize(eSize(winW - 345,30));
	lbViewOrder->loadDeco();
	lbViewOrder->setNextCharTimeout(atoi(ReadINIValue(INIKEY_EDITCHARTIMEOUT,"1000")));
	lbViewOrder->setMaxChars(4);
	lbViewOrder->setUseableChars("0123");

	cbFocusEpg=new eCheckbox(this);
	cbFocusEpg->move(ePoint(15, posI += 45));;
	cbFocusEpg->resize(eSize(winW-30,30));
	cbFocusEpg->setText(ReadINIValue(INIKEYLB_CONFFOCUSPEG,"Focus EPG"));
	cbFocusEpg->loadDeco();

	cbShowPastEventAll=new eCheckbox(this);
	cbShowPastEventAll->move(ePoint(15, posI += 35));;
	cbShowPastEventAll->resize(eSize(winW-30,30));
	cbShowPastEventAll->setText(ReadINIValue(INIKEYLB_CONFSHOWEVENTALL,"Show past event all"));
	cbShowPastEventAll->loadDeco();

	cbShowPastEventChan=new eCheckbox(this);
	cbShowPastEventChan->move(ePoint(15, posI += 35));;
	cbShowPastEventChan->resize(eSize(winW-30,30));
	cbShowPastEventChan->setText(ReadINIValue(INIKEYLB_CONFSHOWEVENTCHAN,"Show past event chan"));
	cbShowPastEventChan->loadDeco();

	btEpgOnCf=new eButton(this);
	btEpgOnCf->move(ePoint(winW - 160,50));
	btEpgOnCf->resize(eSize(150, 30));
	btEpgOnCf->setShortcut("1");
	btEpgOnCf->setShortcutPixmap("1");
	btEpgOnCf->setText(ReadINIValue(INIKEYLB_EPGONCF,"Epg On CF"));
	btEpgOnCf->setAlign(0);
	CONNECT(btEpgOnCf->selected, eConfigurationDlg::OnEpgOnCf);

	btEpgOnUsb=new eButton(this);
	btEpgOnUsb->move(ePoint(winW - 160,85));
	btEpgOnUsb->resize(eSize(150, 30));
	btEpgOnUsb->setShortcut("2");
	btEpgOnUsb->setShortcutPixmap("2");
	btEpgOnUsb->setText(ReadINIValue(INIKEYLB_EPGONUSB,"Epg On USB"));
	btEpgOnUsb->setAlign(0);
	CONNECT(btEpgOnUsb->selected, eConfigurationDlg::OnEpgOnUsb);

	btEpgOnHd=new eButton(this);
	btEpgOnHd->move(ePoint(winW - 160,120));
	btEpgOnHd->resize(eSize(150, 30));            
	btEpgOnHd->setShortcut("3");
	btEpgOnHd->setShortcutPixmap("3");
	btEpgOnHd->setText(ReadINIValue(INIKEYLB_EPGONHD,"Epg On HD"));
	btEpgOnHd->setAlign(0);
	CONNECT(btEpgOnHd->selected, eConfigurationDlg::OnEpgOnHd);

	btEpgOnTmp=new eButton(this);
	btEpgOnTmp->move(ePoint(winW - 160,155));
	btEpgOnTmp->resize(eSize(150, 30));            
	btEpgOnTmp->setShortcut("4");
	btEpgOnTmp->setShortcutPixmap("4");
	btEpgOnTmp->setText(ReadINIValue(INIKEYLB_EPGONTMP,"Epg On /tmp"));
	btEpgOnTmp->setAlign(0);
	CONNECT(btEpgOnTmp->selected, eConfigurationDlg::OnEpgOnTmp);

	btExit=new eButton(this);
	btExit->move(ePoint(15,winH - 35));
	btExit->resize(eSize(100, 30));
	btExit->setShortcut("red");
	btExit->setShortcutPixmap("red");
	btExit->setText(ReadINIValue(INIKEYLB_EXIT,"Exit"));
	btExit->setAlign(0);
	CONNECT(btExit->selected, eWidget::reject);

	btSaveOptions=new eButton(this);
	btSaveOptions->move(ePoint(120,winH - 35));
	btSaveOptions->resize(eSize(100, 30));
	btSaveOptions->setShortcut("green");
	btSaveOptions->setShortcutPixmap("green");
	btSaveOptions->setText(ReadINIValue(INIKEYLB_SAVEOPTIONS,"Save"));
	btSaveOptions->setAlign(0);
	CONNECT(btSaveOptions->selected, eConfigurationDlg::OnSaveOptions);

	btReloadIni=new eButton(this);
	btReloadIni->move(ePoint(225,winH - 35));
	btReloadIni->resize(eSize(200, 30));
	btReloadIni->setShortcut("blue");
	btReloadIni->setShortcutPixmap("blue");
	btReloadIni->setText(ReadINIValue(INIKEYLB_RELOADOPTIONS,"Reload INI"));
	btReloadIni->setAlign(0);
	CONNECT(btReloadIni->selected, eConfigurationDlg::OnReloadIni);

	ReloadFromINI();
}

eConfigurationDlg::~eConfigurationDlg()
{
}

void eConfigurationDlg::OnSaveOptions()
{
	eString sDummy;
	char buffer[5];

	sDummy = lbEpgFile->getText(); 
	SaveINIValue(INIKEY_FILENAME, sDummy);

	sDummy = lbStartOffsetTimer->getText();
	SaveINIValue(INIKEY_STARTOFFSETTIMER, sDummy);

	sDummy = lbEndOffsetTimer->getText();
	SaveINIValue(INIKEY_ENDOFFSETTIMER, sDummy);

	sDummy = lbGridViewItems->getText();
	SaveINIValue(INIKEY_GRIDELEMENTS, sDummy);

	sDummy = lbViewOrder->getText();
	SaveINIValue(INIKEY_VIEWORDER, sDummy);

	sprintf(buffer,"%d",cbFocusEpg->isChecked());
	SaveINIValue(INIKEY_FOCUSEPG,buffer);
	
	sprintf(buffer,"%d",cbShowPastEventAll->isChecked());
	SaveINIValue(INIKEY_SHOWPASTALL,buffer);

	sprintf(buffer,"%d",cbShowPastEventChan->isChecked());
	SaveINIValue(INIKEY_SHOWPASTCHANNEL,buffer);

	eWidget::accept();
}

void eConfigurationDlg::OnExit()
{
}

void eConfigurationDlg::ReloadFromINI()
{
	lbEpgFile->setText(ReadINIValue(INIKEY_FILENAME, DEFFILENAME));
	lbStartOffsetTimer->setText(ReadINIValue(INIKEY_STARTOFFSETTIMER, "0"));
	lbEndOffsetTimer->setText(ReadINIValue(INIKEY_ENDOFFSETTIMER, "0"));
	lbGridViewItems->setText(ReadINIValue(INIKEY_GRIDELEMENTS, "2"));
	lbViewOrder->setText(ReadINIValue(INIKEY_VIEWORDER, DEFAULT_VIEW_ORDER));
	cbFocusEpg->setCheck(atoi(ReadINIValue(INIKEY_FOCUSEPG,"1")));
	cbShowPastEventAll->setCheck(atoi(ReadINIValue(INIKEY_SHOWPASTALL,"1")));
	cbShowPastEventChan->setCheck(atoi(ReadINIValue(INIKEY_SHOWPASTCHANNEL,"1")));
}

void eConfigurationDlg::OnReloadIni()
{
	ReloadFromINI();
}

void eConfigurationDlg::FillComboboxes()
{
	AddListboxElement((eListBox<eListBoxEntryText>*)cbDaySeparator,"Pitum0", (void *)0);
	AddListboxElement((eListBox<eListBoxEntryText>*)cbDaySeparator,"Pitum1", (void *)1);
	AddListboxElement((eListBox<eListBoxEntryText>*)cbDaySeparator,"Pitum2", (void *)2);
	AddListboxElement((eListBox<eListBoxEntryText>*)cbDaySeparator,"Pitum3", (void *)3);
}

void eConfigurationDlg::OnEpgOnUsb()
{
	lbEpgFile->setText("/mnt/usb/epgui.dat");
}

void eConfigurationDlg::OnEpgOnHd()
{
	lbEpgFile->setText("/hdd/epgui.dat");
}

void eConfigurationDlg::OnEpgOnCf()
{
	lbEpgFile->setText("/mnt/cf/epgui.dat");
}

void eConfigurationDlg::OnEpgOnTmp()
{
	lbEpgFile->setText("/tmp/epgui.dat");
}

void eChannelListDialog::OnPrevViewMode()
{
	current_display_mode--;
	if(current_display_mode<0)
		for(current_display_mode=0; display_mode_order[current_display_mode+1]!=-1;current_display_mode++);
	display_mode = (displayMode)display_mode_order[current_display_mode];
	RefreshWindowMode();
	FocusListbox();
}

eSearch::eSearch()
{
	int sW = 720;
	int sH = 576;
	int winH = 310;
	int winW = 450;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText("Search");

	lb_status=new eLabel(this);	
	lb_status->move(ePoint(10, 10));
	lb_status->resize(eSize(winW  - 20, 30)); // leave space for both buttons
	lb_status->setText(ReadINIValue(INIKEYLB_FILTERKEY,"Filter key:"));

	lb_search=new eTextInputField(this);
	lb_search->move(ePoint(10, 40));
	lb_search->resize(eSize(winW - 20, 30)); // leave space for both buttons
	lb_search->setText("");
	lb_search->loadDeco();
	lb_search->setNextCharTimeout(atoi(ReadINIValue(INIKEY_EDITCHARTIMEOUT,"1000")));
	lb_search->setMaxChars(128);
	lb_search->setUseableChars("abcdefghilmnopqrstuvzxykwj0123456789 ");

	bt_searchTitle=new eButton(this);
	bt_searchTitle->move(ePoint(10,90));
	bt_searchTitle->resize(eSize(200, 30));
	bt_searchTitle->setShortcut("red");
	bt_searchTitle->setShortcutPixmap("red");
	bt_searchTitle->setText(ReadINIValue(INIKEYLB_TITLEFILTER,"Filter By Title"));
	bt_searchTitle->setAlign(0);
	CONNECT(bt_searchTitle->selected, eSearch::SearchTitle);

	bt_searchDescriptions=new eButton(this);
	bt_searchDescriptions->move(ePoint(10,130));
	bt_searchDescriptions->resize(eSize(200, 30));
	bt_searchDescriptions->setShortcut("green");
	bt_searchDescriptions->setShortcutPixmap("green");
	bt_searchDescriptions->setText(ReadINIValue(INIKEYLB_DESCFILTER,"Filter By Description"));
	bt_searchDescriptions->setAlign(0);
	CONNECT(bt_searchDescriptions->selected, eSearch::SearchDescriptions);

	bt_searchGenere=new eButton(this);
	bt_searchGenere->move(ePoint(10,170));
	bt_searchGenere->resize(eSize(200, 30));
	bt_searchGenere->setShortcut("yellow");
	bt_searchGenere->setShortcutPixmap("yellow");
	bt_searchGenere->setText(ReadINIValue(INIKEYLB_GENEREFILTER,"Filter by type"));
	bt_searchGenere->setAlign(0);
	CONNECT(bt_searchGenere->selected, eSearch::SearchGenere);

	bt_searchAll=new eButton(this);
	bt_searchAll->move(ePoint(10,210));
	bt_searchAll->resize(eSize(200, 30));
	bt_searchAll->setShortcut("blue");
	bt_searchAll->setShortcutPixmap("blue");
	bt_searchAll->setText(ReadINIValue(INIKEYLB_ALLFILTER,"Full text filter"));
	bt_searchAll->setAlign(0);
	CONNECT(bt_searchAll->selected, eSearch::SearchAll);

	bt_no_search=new eButton(this);
	bt_no_search->move(ePoint(10,250));
	bt_no_search->resize(eSize(200, 30));
	bt_no_search->setShortcut("0");
	bt_no_search->setShortcutPixmap("0");
	bt_no_search->setAlign(0);
	bt_no_search->setText(ReadINIValue(INIKEYLB_NOFILTER,"Disable filter"));
	CONNECT(bt_no_search->selected, eSearch::NoSearch);

}

eSearch::~eSearch()
{
}

void eSearch::NoSearch(void)
{
 	mSearchKey = "";
	mSearchType = no_search;
	eWidget::reject();
}

void eSearch::SearchTitle(void)
{
 	mSearchKey = lb_search->getText();
	mSearchType = etitle;
	eWidget::reject();
}

void eSearch::SearchGenere(void)
{
	mSearchKey = lb_search->getText();
	mSearchType = genere;
	eWidget::reject();
}

void eSearch::SearchDescriptions(void)
{
	mSearchKey = lb_search->getText();
	mSearchType = description;
	eWidget::reject();
}

void eSearch::SearchAll(void)
{
	mSearchKey = lb_search->getText();
	mSearchType = all;
	eWidget::reject();
}

void eSearch::SetpSearchKey(eString *ps)
{
	pSearchKey = ps;
	lb_search->setText(*ps);
}

void eSearch::SetpSearchMode(searchType *ps)
{
	pSearchType = ps;
}

eChannelListDialog::eChannelListDialog(): eWindow(1)
{
	unsigned int d=0;
	current_sel_program = 0;
	current_channel = 0;
	ProgramDescription = NULL;

	ProgRecord = false;
	mSearchKey = "";
	mSearchType = no_search;

	ThemesListBoxInit = false;
	GridListBoxInit = false;	
	UpdatingListboxes = false;
	AllListBoxInit = false;
	FillChannelListInit = false;
	search_dirty_flag = false;

	int sW = 720;
	int sH = 576;
	int winH = 410;
	int winW = 480;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText("EPG Viewer - Version: 2.8");

	lb_status=new eLabel(this);
	lb_status->move(ePoint(10, 5));
	lb_status->resize(eSize(winW - 20, 20)); // leave space for both buttons
	lb_status->loadDeco();
	lb_status->setText("*********PITUM*********");

	ChanListbox=new eListBox<eListBoxEntryText>(this);
	ChanListbox->move(ePoint(10, 30));
	ChanListbox->resize(eSize(clientrect.width()-20, clientrect.height()-100));
	ChanListbox->loadDeco();
	CONNECT(ChanListbox->selected, eChannelListDialog::OnChannelListSelected);
	CONNECT(ChanListbox->selchanged, eChannelListDialog::OnChannelListChange);

	GridListbox=new eListBox<eListBoxEntryText>(this);
	GridListbox->move(ePoint(10, 30));
	GridListbox->resize(eSize(clientrect.width()-20, clientrect.height()-100));
	GridListbox->loadDeco();
	GridListbox->hide();

	AllProgListbox=new eListBox<eListBoxEntryText>(this);
	AllProgListbox->move(ePoint(10, 30));
	AllProgListbox->resize(eSize(clientrect.width()-20, clientrect.height()-100));
	AllProgListbox->loadDeco();
	AllProgListbox->hide();

	ProgByThemeListbox=new eListBox<eListBoxEntryText>(this);
	ProgByThemeListbox->move(ePoint(10, 30));
	ProgByThemeListbox->resize(eSize(clientrect.width()-20, clientrect.height()-100));
	ProgByThemeListbox->loadDeco();
	ProgByThemeListbox->hide();

	bt_hidden1=new eButton(this);
	bt_hidden1->move(ePoint(clientrect.width()-150, clientrect.height()-80));
	bt_hidden1->resize(eSize(1, 1));
	bt_hidden1->setShortcut("1");
	bt_hidden1->setText(".");
	CONNECT(bt_hidden1->selected, eChannelListDialog::OnView1);
	bt_hidden2=new eButton(this);
	bt_hidden2->move(ePoint(clientrect.width()-150, clientrect.height()-80));
	bt_hidden2->resize(eSize(1, 1));
	bt_hidden2->setShortcut("2");
	bt_hidden2->setText(".");
	CONNECT(bt_hidden2->selected, eChannelListDialog::OnView2);
	bt_hidden3=new eButton(this);
	bt_hidden3->move(ePoint(clientrect.width()-150, clientrect.height()-80));
	bt_hidden3->resize(eSize(1, 1));
	bt_hidden3->setShortcut("3");
	bt_hidden3->setText(".");
	CONNECT(bt_hidden3->selected, eChannelListDialog::OnView3);
	bt_hidden4=new eButton(this);
	bt_hidden4->move(ePoint(clientrect.width()-150, clientrect.height()-80));
	bt_hidden4->resize(eSize(1, 1));
	bt_hidden4->setShortcut("4");
	bt_hidden4->setText(".");
	CONNECT(bt_hidden4->selected, eChannelListDialog::OnView4);

	bt_hidden_searchT=new eButton(this);
	bt_hidden_searchT->move(ePoint(clientrect.width()-150, clientrect.height()-80));
	bt_hidden_searchT->resize(eSize(1, 1));
	bt_hidden_searchT->setShortcut("7");
	bt_hidden_searchT->setText(".");
	CONNECT(bt_hidden_searchT->selected, eChannelListDialog::openTimer);

	bt_hidden_zapit=new eButton(this);
	bt_hidden_zapit->move(ePoint(clientrect.width()-150, clientrect.height()-80));
	bt_hidden_zapit->resize(eSize(1, 1));
	bt_hidden_zapit->setShortcut("9");
	bt_hidden_zapit->setText(".");
	CONNECT(bt_hidden_zapit->selected, eChannelListDialog::OnZapIt);

	bt_prevview=new eButton(this);
	bt_prevview->move(ePoint(5, clientrect.height()- 65 ));
	bt_prevview->resize(eSize(120, 30));
	bt_prevview->setShortcut("red");
	bt_prevview->setShortcutPixmap("red");
	bt_prevview->setText(ReadINIValue(INIKEYLB_PREVVIEW, "<< View"));
	bt_prevview->setAlign(0);
	CONNECT(bt_prevview->selected, eChannelListDialog::OnPrevViewMode);

	bt_modeepg=new eButton(this);
	bt_modeepg->move(ePoint(125, clientrect.height()- 65));
	bt_modeepg->resize(eSize(120, 30));
	bt_modeepg->setShortcut("green");
	bt_modeepg->setShortcutPixmap("green");
	bt_modeepg->setText(ReadINIValue(INIKEYLB_NEXTVIEW,"View >>"));
	bt_modeepg->setAlign(0);
	CONNECT(bt_modeepg->selected, eChannelListDialog::NextViewMode);

	bt_search=new eButton(this);
	bt_search->move(ePoint(clientrect.width()-235, clientrect.height()- 65));
	bt_search->resize(eSize(100, 30));
	bt_search->setShortcut("yellow");
	bt_search->setShortcutPixmap("yellow");
	bt_search->setText(ReadINIValue(INIKEYLB_SEARCH,"Filter"));
	bt_search->setAlign(0);
	CONNECT(bt_search->selected, eChannelListDialog::OnSearchIt);
		
	bt_utils=new eButton(this);
	bt_utils->move(ePoint(clientrect.width()-135, clientrect.height()- 65));
	bt_utils->resize(eSize(130, 30));
	bt_utils->setShortcut("blue");
	bt_utils->setShortcutPixmap("blue");
	bt_utils->setText(ReadINIValue(INIKEYLB_UTILS,"Configure"));
	bt_utils->setAlign(0);
	CONNECT(bt_utils->selected, eChannelListDialog::OnShowUtils);

	lb_help=new eLabel(this);
	lb_help->move(ePoint(10, clientrect.height()- 30));
	lb_help->resize(eSize(clientrect.width() - 20, 30)); // leave space for both buttons
	lb_help->loadDeco();
	lb_help->setText(ReadINIValue(INIKEYLB_LBHELP,"Press 9 to zap channel, 7 to open Timer List"));

	CONNECT(GridListbox->selected, eChannelListDialog::OnGridListSelected);
	CONNECT(GridListbox->selchanged, eChannelListDialog::OnGridListChange);
	CONNECT(AllProgListbox->selected, eChannelListDialog::OnAllListSelected);
	CONNECT(AllProgListbox->selchanged, eChannelListDialog::OnAllListChange);
	CONNECT(ProgByThemeListbox->selected, eChannelListDialog::OnThemeListSelected);
	CONNECT(ProgByThemeListbox->selchanged, eChannelListDialog::OnThemeListChange);
	
	setFocus(ChanListbox);

	counter=0;
	now = GetTransponderTime();
	now += atoi(ReadINIValue(INIKEY_EPGOFFSET,"0"));
	now += atoi(ReadINIValue(INIKEY_DAYLIGHTSAVE,"0"))*3600;
		
	ProgramsCount = ReadEpgFile();

	char sDummy[MAXTXT];
	strcpy(sDummy,ReadINIValue(INIKEY_VIEWORDER,DEFAULT_VIEW_ORDER));

	current_display_mode = 0;
	for(d=0; d<strlen(sDummy);d++)
		display_mode_order[d]=sDummy[d]-'0';
	display_mode_order[d]=-1;
	display_mode = (displayMode) display_mode_order[0];
}

eChannelListDialog::~eChannelListDialog()
{
	if(ProgramDescription)
		free(ProgramDescription);
}

void eChannelListDialog::openTimer()
{
	hide();
		eTimerListView setup;
		setup.show();
		setup.exec();
		setup.hide();
	show();
	FocusListbox();
}

void eChannelListDialog::OnChannelListChange(eListBoxEntryText *item)
{
	if(UpdatingListboxes)
		return;
	counter=0;
	if(item)
		current_channel = (int) item->getKey();
}

void eChannelListDialog::OnZapIt(void)
{
	if(!ZapIT(current_channel, ProgramDescription))
		myerror("What's this channel? ;))");
	FocusListbox();
}

void eChannelListDialog::OnSearchIt(void)
{
	int result;
	hide();
		eSearch dlg;
		dlg.SetpSearchKey(&mSearchKey);
		dlg.SetpSearchMode(&mSearchType);
		dlg.show();
		result = dlg.exec();
		if(result) // sono uscito premendo LAME
		{
			dlg.hide();
			show();
			FocusListbox();
			return;
		}
		dlg.hide();
	show();
	RefreshWindowMode();
	search_dirty_flag = true;
}


void eChannelListDialog::OnShowUtils(void)
{
	hide();
		eConfigurationDlg dlg;
		dlg.show();
		dlg.exec();
		dlg.hide();
	show();
	FocusListbox();
}

bool eChannelListDialog::GetProgRecord(void)
{
	return ProgRecord;
}

void eChannelListDialog::OnView1(void)
{
	UpdatingListboxes = true;
	display_mode = channel_view;
	RefreshWindowMode();
	UpdatingListboxes = false;
}

void eChannelListDialog::OnView2(void)
{
	UpdatingListboxes = true;
	display_mode = grid_view;
	RefreshWindowMode();
	UpdatingListboxes = false;
}

void eChannelListDialog::OnView3(void)
{
	UpdatingListboxes = true;
	display_mode = allprog_view;
	RefreshWindowMode();
	UpdatingListboxes = false;
}

void eChannelListDialog::OnView4(void)
{
	UpdatingListboxes = true;
	display_mode = themes_view;
	RefreshWindowMode();
	UpdatingListboxes = false;
}

int eChannelListDialog::ReadEpgFile(void)
{	
	FILE *f;
	long len=0, count=0;
	int iDummy;
	char buffer[MAXTXT];
	f=fopen(EPGfilename,"r");
	long PreviousPos;

	if(f!=NULL)
	{
		eWaitPlease dlgWait;
		dlgWait.SetMessage(ReadINIValue(INIKEYLB_PLEASEWAIT,"Wait please..."));
		dlgWait.show();
		MAXPROGS = 0;
// 		do 
// 		{
// 			fgets(buffer,MAXTXT, f);
// 			if(buffer[0]=='#')
// 			{
// 				switch(buffer[1])
// 				{
// 					case 'P':
// 						MAXPROGS += atoi(buffer+2);
// 						break;
// 					case 'C':
// 						MAXPROGS += atoi(buffer+2);
// 						break;
// 					default:
// 						// Unknown option
// 						break;
// 				}
// 			}
// 		}
// 		while(buffer[0]=='C');

/*		if(!MAXPROGS)
		{*/
		MAXPROGS = 0;
		while(!feof(f))
		{
			fgets(buffer,MAXTXT,f);
			switch(buffer[0])
			{
			case 'e':
				MAXPROGS++;
				break;
			case 'C':
				MAXPROGS++;
				break;
			}
		}
		fseek(f, 0, SEEK_SET);
// 		}

		iDummy=atoi(ReadINIValue(INIKEY_MAXPROGRAMSTOLOAD,"-1"));
		if(iDummy!=-1)
			if(iDummy<MAXPROGS)
				MAXPROGS = iDummy;

		ProgramDescription = (sProgramDescription *)malloc(sizeof(sProgramDescription) * (MAXPROGS + 1));
		
		if(!ProgramDescription)
		{
			myerror("Cannot allocate memory ",MAXPROGS);
			MAXPROGS = 0;
			return false;
		}
		dlgWait.SetMax(MAXPROGS);

		while(!feof(f) && count < MAXPROGS)
		{
			PreviousPos = ftell(f);
			fgets(buffer,MAXTXT,f);
			len++;
			switch(buffer[0])
			{
				case 'C':
					strcpy(ProgramDescription[count].title,strchr(StripEndl(buffer+2),' ')+1);
					ProgramDescription[count].duration=-1; // It's a channel name
					ProgramDescription[count].btime=0;
					count++;
					break;
				case 'E':
					 // Field not used!
					break;
				case 'T':
					strcpy(ProgramDescription[count].title,StripEndl(buffer+2));
					break;
				case 'X':
					ProgramDescription[count].btime = atol(StripEndl(buffer+2));
					break;
				case 'S':
					strcpy(ProgramDescription[count].genere,StripEndl(buffer+2));
					break;
				case 'L':
					ProgramDescription[count].duration = atoi(StripEndl(buffer+2));
					break;
				case 'D':
					ProgramDescription[count].DescriptionPos = PreviousPos;
					break;
				case 'e':
					count++;
					dlgWait.SetValue(count);
					break;			
			}
		}
		fclose(f);
		dlgWait.hide();

		return count;
	}
	else
	{
		char err[MAXTXT];
		sprintf(err,"Cannot open: %s",EPGfilename);
		AddListboxElement(ChanListbox, err, (void*)-1);
		return false;
	}	
}

int eChannelListDialog::FillChannelList(void)
{	
	long c=0, count=0;
	char buffer[MAXTXT], chan_name[MAXTXT];
	eListBoxEntryText *item;
	eListBoxEntryText *CurrentItem=NULL;
	ReadChanName();
	for(c=0;c<ProgramsCount;c++)
	{
		if(ProgramDescription[c].duration==-1)
		{
			sprintf(buffer,"%ld. %s",++count,ProgramDescription[c].title);
			item = AddListboxElement(ChanListbox,StripEndl(buffer), (void*)c);
			strcpy(chan_name,ReadINIValue(ProgramDescription[c].title,ProgramDescription[c].title));
			if(!strcmp(chan_name,sCurrentEnigmaChannel))
				CurrentItem = item;
		}
	}
	if(CurrentItem)
		ChanListbox->setCurrent(CurrentItem, false);
	return true;	
}


char *StripEndl(char *string)
{	
	unsigned int i;
	for(i=0;i<strlen(string);i++)
	{
		if(string[i]==10 || string[i]==13)
			string[i]=0;
	}
	return string;
}

bool ParseServiceID(char *sid_in, char *sid_out)	// Parse cur_chan id
{
	char *temp_pos;
	char ChannelTab[6][MAXTXT];

	for(int n=0;n<6;n++)
	{
		temp_pos=strchr(sid_in,SERVICESEPARATOR);
		if(temp_pos!=NULL)
		{
			strncpy(ChannelTab[n],sid_in,temp_pos - sid_in);
			ChannelTab[n][temp_pos - sid_in]=0;
			memmove(sid_in,temp_pos+1,strlen(temp_pos)+1);
		}
		else
			strcpy(ChannelTab[n],sid_in);	
	}
	
	sprintf(sid_out,"1:0:1:%s:%s:%s:%s:0:0:0:\n",
		StripZero(ChannelTab[0]),
		StripZero(ChannelTab[2]), 
		StripZero(ChannelTab[3]),
		StripZero(ChannelTab[1]));

	return true;
}

bool LookupServiceByChannel(char *channelID, char *serviceID)
{
	char chan_name[MAXTXT], cur_name[MAXTXT], cur_chanid[MAXTXT];
	FILE *f_services;

	strcpy(chan_name,ReadINIValue(channelID,channelID));

	f_services=fopen(ReadINIValue(INIKEY_SERVICESFILENAME,SERVICESFILENAME),"r");
	if(f_services==NULL)
	{
		myerror("Cannot open services file:",ReadINIValue(INIKEY_SERVICESFILENAME,SERVICESFILENAME));
		return false;
	}

	cur_name[0]=0;
	cur_chanid[0]=0;

	do 
	{
		strcpy(cur_chanid, cur_name);
		fgets(cur_name,MAXTXT, f_services);
		StripEndl(cur_name);
	} 
	while(strcmp(chan_name,cur_name) && !feof(f_services));

	if(feof(f_services))
	{
		fclose(f_services);
		return false;
	}

	fclose(f_services);
	strcpy(serviceID, cur_chanid);
	return true;
}

bool LookupChannelByService(char *channelID, char *serviceID)
{
	char chan_name[MAXTXT], cur_chanid[MAXTXT];
	FILE *f_services;

	f_services=fopen(ReadINIValue(INIKEY_SERVICESFILENAME,SERVICESFILENAME),"r");
	if(f_services==NULL)
	{
		myerror("Cannot open services file:",ReadINIValue(INIKEY_SERVICESFILENAME,SERVICESFILENAME));
		return false;
	}

	cur_chanid[0]=0;

	do 
	{
		fgets(cur_chanid,MAXTXT, f_services);
		StripEndl(cur_chanid);
	} 
	while(strcmp(cur_chanid,serviceID) && !feof(f_services));

	if(feof(f_services))
	{
		fclose(f_services);
		return false;
	}

	fgets(chan_name,MAXTXT, f_services);
	StripEndl(chan_name);

	fclose(f_services);
	return true;

}

char *StripZero(char *string)
{
	for(unsigned int i=0;i<strlen(string);i++)
	{
		if(string[0]=='0')
			memmove(string, string+1,strlen(string));
	}
	return string;
}

eWaitPlease::eWaitPlease()
{
	int sW = 720;
// 	int sH = 576;
	int winH = 60;
	int winW = 400;
	cmove(ePoint((sW-winW) / 2,100));
	cresize(eSize(winW, winH));
	setText("EPG Viewer - Version: 2.8");

	strcpy(message,"Please wait...");

	lb_status=new eLabel(this);
	lb_status->move(ePoint(20, clientrect.height()-40));
	lb_status->resize(eSize(clientrect.width()-40, 30)); // leave space for both buttons
	lb_status->setText(message);

}

eWaitPlease::~eWaitPlease()
{
}

void eWaitPlease::SetMessage(char *msg)
{
	strcpy(message,msg);
	lb_status->setText(message);
}

char *ReadINIValue(char *vname)
{
	FILE *f;
	char buffer[MAXTXT];
	static char result[256];
	
	result[0]=0;

	if(vname[0]=='l' && vname[1]=='b')
	{
		f=fopen(INILANG,"r");
		if(f==NULL)
			f=fopen(INI,"r");
	}
	else
		f=fopen(INI,"r");
	
	if(f==NULL)
		return result;
	
	while(!feof(f))
	{
        if(fgets(buffer,MAXTXT,f)==NULL)
		{
			fclose(f);
			return result;
		}
		if(strlen(buffer))
		{
			if(buffer[0]!='#')
			{
				char key[256];
				strcpy(key,buffer);
				if(strchr(buffer,'=')!=NULL)
				{
					*(strchr(key,'='))=0;
					if(!strcmp(key,vname))
					{
						strcpy(result, strchr(buffer,'=')+1);
						StripEndl(result);
					}
				}	
			}
		}
	}
	
	fclose(f);
	
	return result;
}

eChannelEpgDetail::eChannelEpgDetail(): eWindow(1)
{
	int sW = 720;
	int sH = 576;
	int winH = 405;
	int winW = 550;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
 	setText("");

	listbox=new eListBox<eListBoxEntryText>(this);
	listbox->move(ePoint(10, 10));
	listbox->resize(eSize(clientrect.width()-20, clientrect.height()- 40));
	listbox->loadDeco();	

	bt_ok=new eButton(this);
	bt_ok->move(ePoint(5, clientrect.height()-35));
	bt_ok->resize(eSize(100, 30));
	bt_ok->setShortcut("red");
	bt_ok->setShortcutPixmap("red");
	bt_ok->setText(ReadINIValue(INIKEYLB_EXIT,"Exit"));
 	CONNECT(bt_ok->selected, eWidget::accept);

	bt_zap=new eButton(this);
	bt_zap->move(ePoint(1,1));
	bt_zap->resize(eSize(1, 1));
	bt_zap->setShortcut("9");
	bt_zap->setShortcutPixmap("9");
	bt_zap->setText("");
 	CONNECT(bt_zap->selected, eChannelEpgDetail::OnZap);
	
 	CONNECT(listbox->selected, eChannelEpgDetail::selectedItem);
 	CONNECT(listbox->selchanged, eChannelEpgDetail::selectionChanged);
//  	setFocus(listbox);
	counter=0;
	channel = 0;
	
	useDaySeparator = atoi(ReadINIValue(DAYSEPARATOR,"1"));
 	ProgRecord = false;
	current_sel_program = 0;
	ProgramsCount = 0;
}

eChannelEpgDetail::~eChannelEpgDetail()
{
}

void myerror(char *errortxt)
{
	eMessageBox msg(errortxt, "EPGui 2.8", eMessageBox::iconWarning|eMessageBox::btOK);
	msg.show(); msg.exec(); msg.hide();
}

void myerror(char *errortxt, int d)
{
	char errorstring[MAXTXT];
	sprintf(errorstring,"%s%d",errortxt, d);
	eMessageBox msg(errorstring, "EPGui 2.8", eMessageBox::iconWarning|eMessageBox::btOK);
	msg.show(); msg.exec(); msg.hide();
}

void myerror(char *errortxt, char *s)
{
	char errorstring[MAXTXT];
	sprintf(errorstring,"%s%s",errortxt, s);
	eMessageBox msg(errorstring, "EPGui 2.8", eMessageBox::iconWarning|eMessageBox::btOK);
	msg.show(); msg.exec(); msg.hide();
}

int SaveINIValue(char *KeyName, eString Value)
{
	unsigned int cs;
	char sDummy[MAXTXT];
	for(cs=0;cs<Value.length(); cs++)
		sDummy[cs]=Value[cs];
	sDummy[cs]=0;
	return SaveINIValue(KeyName,sDummy);	
}

int SaveINIValue(char *KeyName, int Value)
{
	char sDummy[MAXTXT];
	sprintf(sDummy,"%d",Value);
	return SaveINIValue(KeyName,sDummy);
}

int SaveINIValue(char *KeyName, char *Value)
{
	FILE *f_in, *f_out;
	char key[256];
	char buffer[MAXTXT];
	char *pos;
	bool written=false;

	f_in=fopen(INI,"r");
	if(!f_in)
	{
		myerror("Cannot open file: ",INI);
		return false;
	}

	f_out=fopen(DUMMYFILE,"w");

	if(!f_out)
	{
		myerror("Cannot open file: ",DUMMYFILE);
		fclose(f_in);
		return false;
	}

	while(!feof(f_in))
	{
		fgets(buffer,MAXTXT,f_in);
		StripEndl(buffer);
		if(strlen(buffer))
		{
			strcpy(key,buffer);
			if((pos=strchr(key,'=')))
			{	
				*pos=0; // Tagliamo dall' =
				if(strcmp(key, KeyName))
					fprintf(f_out,"%s\n",buffer); // Copiamo invariato
				else
				{
					fprintf(f_out,"%s=%s\n",key,Value);
					written=true;
				}
			}
			else
				fprintf(f_out,"%s\n",buffer);
		}
		strcpy(buffer,"");

	}

	if(!written)
		fprintf(f_out,"%s=%s\n",KeyName,Value);

	fclose(f_in);
	fclose(f_out);
	sprintf(buffer,"mv %s %s",DUMMYFILE,INI);
	system(buffer);
	sprintf(buffer,"rm %s",DUMMYFILE);
	system(buffer);

	return true;
}

char *ReadINIValue(char *KeyName, char *DefaultValue)
{
	static char result[MAXTXT];
	char *cDummy;
	cDummy = ReadINIValue(KeyName);
	if(strlen(cDummy))
		strcpy(result,cDummy);
	else
		strcpy(result,DefaultValue);
	return result; // We'll never be here... but i hate build warnings ;)
}

int plugin_exec( PluginParam *par )
{
	strcpy(EPGfilename,"");
	strcpy(day_names[0],ReadINIValue(INIKEYLB_DOM,"Dom"));
	strcpy(day_names[1],ReadINIValue(INIKEYLB_LUN,"Lun"));
	strcpy(day_names[2],ReadINIValue(INIKEYLB_MAR,"Mar"));
	strcpy(day_names[3],ReadINIValue(INIKEYLB_MER,"Mer"));
	strcpy(day_names[4],ReadINIValue(INIKEYLB_GIO,"Gio"));
	strcpy(day_names[5],ReadINIValue(INIKEYLB_VEN,"Ven"));
	strcpy(day_names[6],ReadINIValue(INIKEYLB_SAB,"Sab"));
	MAXPROGS = 0;

	strcpy(EPGfilename,ReadINIValue(INIKEY_FILENAME, DEFFILENAME));

	int result=0;
	eChannelListDialog dlg;
 	dlg.InitDialog();
	dlg.show();
	result=dlg.exec();
	dlg.hide();
	
// 	eMessageBox msg(BYESTRING, "EPGUi 2.8", eMessageBox::iconWarning|eMessageBox::btOK);
// 	msg.show(); msg.exec(); msg.hide();

	return 0;
}

char *GetDescription(long pos)
{
	FILE *f;
	char buffer[MAXTXT];

	f=fopen(EPGfilename,"r");
	if(f==NULL)
		return ".";

	fseek(f,pos,SEEK_SET);
	fgets(buffer, MAXTXT, f);

	fclose(f);

	return StripEndl(buffer+2);
}

time_t GetTransponderTime(void)
{
	time_t t=time(0)+eDVB::getInstance()->time_difference;
	tm *c=gmtime(&t);
	t=mktime(c);	
	return t - atoi(ReadINIValue(INIKEY_DAYLIGHTSAVE,"0"))*3600;
}

bool ShallDisplay(sProgramDescription *pd, eString *mSearchKey, searchType mSearchMode)
{
	char mcSearchKey[MAXTXT];
	unsigned int cs;
	for(cs=0;cs<mSearchKey->length(); cs++)
		mcSearchKey[cs]=(*mSearchKey)[cs];
	mcSearchKey[cs]=0;
	return ShallDisplay(pd, mcSearchKey, mSearchMode);
}

bool ShallDisplay(sProgramDescription *pd, char *mcSearchKey, searchType mSearchMode)
{
	bool DisplayIt = false;

	switch(mSearchMode)
	{
		case genere:
			DisplayIt = SearchSubstring(pd->genere, mcSearchKey);
			break;
		case etitle:
			DisplayIt = SearchSubstring(pd->title, mcSearchKey);
			break;
		case description:
			DisplayIt = SearchSubstring(GetDescription(pd->DescriptionPos), mcSearchKey);
			break;
		case all:
			DisplayIt = SearchSubstring(pd->genere, mcSearchKey);
			DisplayIt |= SearchSubstring(pd->title, mcSearchKey);
			DisplayIt |= SearchSubstring(GetDescription(pd->DescriptionPos), mcSearchKey);
			break;
		case no_search:
			DisplayIt = true;
			break;
	}
	return DisplayIt;
}

bool SearchSubstring(char *str, char *substr)
{
	if(strlen(str)<strlen(substr))
		return false;
	for(unsigned int n=0;n<((strlen(str)-strlen(substr))+1);n++)
	{
		if(!strncasecmp(str+n,substr,strlen(substr)))
			return true;
	}
	return false;
}

void eChannelListDialog::NextViewMode()
{
	current_display_mode++;
	if(display_mode_order[current_display_mode]==-1)
		current_display_mode=0;
	display_mode = (displayMode)display_mode_order[current_display_mode];
	RefreshWindowMode();
	FocusListbox();
}

void eChannelListDialog::RefreshWindowMode()
{
	eWaitPlease dlgWait;
	dlgWait.SetMessage(ReadINIValue(INIKEYLB_PLEASEWAIT,"Wait please..."));
	dlgWait.show();
	dlgWait.SetMax(ProgramsCount);

	switch(display_mode)
	{
		case channel_view:
			if(!FillChannelListInit)
			{
				FillChannelListInit=true;
				FillChannelList();
			}
			GridListbox->hide();
			AllProgListbox->hide();
			ProgByThemeListbox->hide();

			ChanListbox->show();
			setFocus(ChanListbox);
			break;
		case grid_view:
			if(!GridListBoxInit)
			{
				FillGridEPG(now);
				GridListBoxInit=true;
			}
			AllProgListbox->hide();
			ProgByThemeListbox->hide();
			ChanListbox->hide();

			GridListbox->show();
			setFocus(GridListbox);
			break;
		case allprog_view:
			if(search_dirty_flag && AllListBoxInit)
			{
				AllProgListbox->clearList();
				AllListBoxInit=false;
			}
			if(!AllListBoxInit)
			{
				FillAllProgs(now);
				AllListBoxInit=true;
			}
			GridListbox->hide();
			ProgByThemeListbox->hide();
			ChanListbox->hide();

			AllProgListbox->show();
			setFocus(AllProgListbox);
			break;
		case themes_view:
			if(!ThemesListBoxInit)
			{
				ThemesListBoxInit = true;
				FillThemes();
			}
			GridListbox->hide();
			AllProgListbox->hide();
			ChanListbox->hide();

			ProgByThemeListbox->show();
			setFocus(ProgByThemeListbox);
			break;
	}
	dlgWait.hide();
}

eListBoxEntryText *AddListboxElement(eListBox<eListBoxEntryText> *lb, char *txt, void *key)
{
	eListBoxEntryText *item;
	eString *st;
	st=new eString;
	*st = txt;
	item = new eListBoxEntryText(lb,*st,key,0 /* align */);
	return item;
}

void ShowProgramDetail(int progID, int channel, sProgramDescription *ProgramDescription, int timeZone)
{
		
		if(progID<MAXPROGS && progID!=-1 && ProgramDescription[progID].duration!=-1)
		{
			int current_start, current_end;
			char sDesc[MAXTXT], tGenere[MAXTXT], tDurata[MAXTXT];
			strcpy(tGenere,ReadINIValue(INIKEYLB_GENERE,"Genere"));
			strcpy(tDurata,ReadINIValue(INIKEYLB_DURATA,"Durata"));

			current_start = (int)ProgramDescription[progID].btime
									+ atoi(ReadINIValue(INIKEY_STARTOFFSETTIMER,"0"))*60;
			current_end = (int)ProgramDescription[progID].duration*60
									+ abs(atoi(ReadINIValue(INIKEY_ENDOFFSETTIMER,"0"))*60)
									+ atoi(ReadINIValue(INIKEY_STARTOFFSETTIMER,"0"))*60;

			if(CheckOverlap(current_start, current_end) || channel == -1) // Se channel = -1 allora proveniamo dalla grid view... da sistemare
			{
				sprintf(sDesc,"%s:%s\n%s:%d'\n\n%s\n\n%s",
					tGenere,
					ProgramDescription[progID].genere,
					tDurata,
					ProgramDescription[progID].duration,
					GetDescription(ProgramDescription[progID].DescriptionPos),
					ReadINIValue(INIKEYLB_NORECORDIT,"Registrazione non possibile a causa di altri eventi gia' programmati!"));
				eMessageBox msg(sDesc, ProgramDescription[progID].title, eMessageBox::iconInfo|eMessageBox::btOK);
				msg.show(); 
				msg.exec();
				msg.hide();
			}
			else
			{
				sprintf(sDesc,"%s:%s\n%s:%d'\n\n%s\n\n%s",
					tGenere,
					ProgramDescription[progID].genere,
					tDurata,
					ProgramDescription[progID].duration,
					GetDescription(ProgramDescription[progID].DescriptionPos),
					ReadINIValue(INIKEYLB_RECORDIT,"Vuoi programmare la registrazione di questo programma?"));
				eMessageBox msg(sDesc, ProgramDescription[progID].title, eMessageBox::iconInfo|eMessageBox::btYes|eMessageBox::btNo);
				msg.show(); 
				if(eMessageBox::btYes == msg.exec())
				{
					if(DoEPGRecord(channel, progID, ProgramDescription, timeZone))
						myerror(ReadINIValue(INIKEYLB_PROGDONE,"Programmazione effettuata"));
				}
				msg.hide();
			}
		}
}

int eChannelListDialog::FillThemes()
{
	char Themes[MAXTHEMES][MAXTXT];
	int ThemeCount=0;
	int n;
	bool AlreadyPresent=false;

	for(int c=0;c<ProgramsCount; c++)
	{
		AlreadyPresent=false;
		if(ProgramDescription[c].duration!=-1)
		{
			//controlliamo se questo theme e' stato gia' inserito
			for(n=0; n<ThemeCount;n++)
			{
				if(!strcmp(ProgramDescription[c].genere, Themes[n]))
					AlreadyPresent = true;
			}
			// ... se non lo e' stato allora andiamo ad inserirlo
			if(!AlreadyPresent)
			{
				if(ThemeCount<MAXTHEMES)
				{
					strcpy(Themes[ThemeCount++],ProgramDescription[c].genere);
					AddListboxElement(ProgByThemeListbox, ProgramDescription[c].genere,0);
				}
			}
		}
	}

return true;
}

void eChannelListDialog::OnChannelListSelected(eListBoxEntryText *item)
{
	if(UpdatingListboxes)
		return;
	counter=0;
	if (item)
	{
		unsigned int cs;
		eString title;
		char sDummy[MAXTXT];
		title = item->getText();

		for(cs=0;cs<title.length(); cs++)
			sDummy[cs]=title[cs];
		sDummy[cs]=0;

		eChannelEpgDetail dlgDetail;
		dlgDetail.SetChannel((int)item->getKey());
		dlgDetail.SetTime(now);
		dlgDetail.SetSearchKey(mSearchKey);
		dlgDetail.SetSearchMode(mSearchType);
		dlgDetail.SetProgramsCount(ProgramsCount);
		dlgDetail.SetProgramDescription(ProgramDescription);
		dlgDetail.FillChannelEPG();
		hide();
			dlgDetail.show();
			dlgDetail.exec();
			dlgDetail.hide();		
		show();
		FocusListbox();
	} 
}
void eChannelListDialog::OnGridListSelected(eListBoxEntryText *item)
{
	if(UpdatingListboxes)
		return;
	if(item)
		ShowProgramDetail((int)item->getKey(), FindChannelFromProg((int)item->getKey(),ProgramDescription), ProgramDescription, 0);
}

void eChannelListDialog::OnGridListChange(eListBoxEntryText *item)
{
	if(UpdatingListboxes)
		return;
	if(item)
	{
		current_channel=FindChannelFromProg((int)item->getKey(), ProgramDescription);
		current_sel_program=(int)item->getKey();
	}
}

void eChannelListDialog::OnThemeListSelected(eListBoxEntryText *item)
{
	if(UpdatingListboxes)
		return;

	if(item)
	{
		mSearchKey = item->getText();
		mSearchType = genere;
		search_dirty_flag = true;
		FocusListbox();
	}
}

void eChannelListDialog::OnThemeListChange(eListBoxEntryText *item)
{
	if(UpdatingListboxes)
		return;
}

void eChannelListDialog::OnAllListSelected(eListBoxEntryText *item)
{
	if(UpdatingListboxes)
		return;
	if(item)
		ShowProgramDetail((int)item->getKey(), FindChannelFromProg((int)item->getKey(),ProgramDescription), ProgramDescription, 0);
}

void eChannelListDialog::OnAllListChange(eListBoxEntryText *item)
{
	if(UpdatingListboxes)
		return;
	if(item)
	{
		current_channel=FindChannelFromProg((int)item->getKey(), ProgramDescription);
		current_sel_program=(int)item->getKey();
	}
}

void eChannelListDialog::FillAllProgs(time_t now)
{
	char item[MAXTXT*4];
	int cache_ShowPastEvent;
	int cache_EventToDisplay;
	int event_count=0;

	cache_ShowPastEvent = atoi(ReadINIValue(INIKEY_SHOWPASTALL,"1"));
	cache_EventToDisplay = atoi(ReadINIValue(INIKEY_SHOWFWDALL,"9999"));

	for(int c=0; c<ProgramsCount; c++)
	{
		if(ProgramDescription[c].duration==-1) // Abbiamo un canale
		{
			AddListboxElement(AllProgListbox, ProgramDescription[c].title,(void *)c);
			event_count=0;
		}
		else
		{
			if(cache_ShowPastEvent || ProgramDescription[c+1].btime>now)
			{
				if(ShallDisplay((ProgramDescription+c), &mSearchKey, mSearchType))
				{
					if(event_count<cache_EventToDisplay)
					{
						FormatEPGElement((ProgramDescription+c), item);
						AddListboxElement(AllProgListbox, item, 0);
						event_count++;
					}
				}
			}

		}
	}
}

char *eChannelListDialog::RefreshSearchMode()
{
	unsigned int cs=0;
	char sDummy[MAXTXT];
	static char dDummy[MAXTXT];
	// Show selected filter
	
	for(;cs<mSearchKey.length(); cs++)
		sDummy[cs]=mSearchKey[cs];
	sDummy[cs]=0;
	
	switch(mSearchType)
	{
		case no_search:
			strcpy(dDummy,"No filter selected");
			break;
		
		case etitle:
			strcpy(dDummy, "Title:");
			break;
		
		case genere:
			strcpy(dDummy, "Kind:");
			break;
		
		case description:
			strcpy(dDummy, "Description:");
			break;
			
		case all:
			strcpy(dDummy, "Full text:");
			break;
	}
	if(mSearchType!=no_search)
		strcat(dDummy,sDummy);
	
	if(strlen(sDummy))
		return (dDummy);
	else
		return NULL;
}

bool CheckOverlap(int start, int duration)
{
	bool result=false;
	bool check;
	FILE *f_timer;

	char buffer[MAXTXT];
	int read_btime;
	int read_duration;

	f_timer = fopen(ReadINIValue(INIKEY_TIMERFILENAME,TIMERFILENAME),"rb");
	
	if(f_timer==NULL)
	{
		myerror("Cannot open timer file:",ReadINIValue(INIKEY_TIMERFILENAME,TIMERFILENAME));
		return false;
	}

	check=false;
	while(!feof(f_timer))
	{
		fgets(buffer,MAXTXT,f_timer);
		if(!strncmp(buffer,"#TIME_BEGIN",strlen("#TIME_BEGIN")))
		{
			sscanf(buffer+strlen("#TIME_BEGIN")+1,"%d",&read_btime);
		}
		if(!strncmp(buffer,"#DURATION",strlen("#DURATION")))
		{
			sscanf(buffer+strlen("#DURATION")+1,"%d",&read_duration);
			check=true;
		}

		if(check)
		{
			check=false;
			if(start>read_btime && start<(read_btime+read_duration))
				result=true;
			if((start+duration)>read_btime && (start+duration)<(read_btime+read_duration))
				result=true;
			if(start<read_btime && ((start+duration)>(read_btime+read_duration)))
				result=true;
			if(start==read_btime && ((start+duration)==(read_btime+read_duration)))
				result=true;
		}
	}
	fclose(f_timer);
	return result;
}
void eChannelEpgDetail::SetSearchKey(eString Key)
{		
	char sDummy[MAXTXT];
	unsigned int cs;
	
	for(cs=0;cs<Key.length(); cs++)
	sDummy[cs]=Key[cs];
	sDummy[cs]=0;
	strcpy(mcSearchKey, sDummy);
}

void eChannelEpgDetail::SetSearchMode(searchType mSm)
{		
	mSearchMode = mSm;
}

void eChannelEpgDetail::SetChannel(long channel_num)
{
	channel = channel_num;	
}

void eChannelEpgDetail::selectionChanged(eListBoxEntryText *item)
{
	counter=0;
	char buffer[MAXTXT];
	struct tm *prgtime;
	time_t tDummy;

	if((int)item->getKey()==-1)
		setText("No channel detail!");
	else
	{
		current_sel_program = (int)item->getKey();
		tDummy = ProgramDescription[current_sel_program].btime;
		tDummy += atoi(ReadINIValue(INIKEY_EPGOFFSET,"0"));
		tDummy += atoi(ReadINIValue(INIKEY_DAYLIGHTSAVE,"0"))*3600;
		prgtime=gmtime(&tDummy);
		sprintf(buffer,"%s - %s %02d/%02d",ProgramDescription[channel].title, day_names[prgtime->tm_wday],prgtime->tm_mday,prgtime->tm_mon+1);
		setText(buffer);
	}
}

void eChannelEpgDetail::selectedItem(eListBoxEntryText *item)
{
	if (item) 
		ShowProgramDetail((int)item->getKey(), channel, ProgramDescription, 0);
	else
		reject();
}

int DoEPGRecord(int channel, int ProgNumber, sProgramDescription *ProgramDescription, int timeZone)
{ 
	char temp_chan_name[MAXTXT];
	char cur_name[MAXTXT];
	char cur_chanid[MAXTXT];
	char new_charid[MAXTXT];
	char sDummy[MAXTXT];
	char DummyEventName[MAXTXT];

	int current_start;
	int current_end;

	current_start = (int)ProgramDescription[ProgNumber].btime
								+ atoi(ReadINIValue(INIKEY_STARTOFFSETTIMER,"0"))*60;
	current_end = (int)ProgramDescription[ProgNumber].duration*60 
								+ abs(atoi(ReadINIValue(INIKEY_STARTOFFSETTIMER,"0"))*60) 
								+ atoi(ReadINIValue(INIKEY_ENDOFFSETTIMER,"0"))*60;

	cur_name[0]=0;
	cur_chanid[0]=0;

	strcpy(temp_chan_name, ProgramDescription[channel].title); // LK2do... orrore!

	if(!LookupServiceByChannel(temp_chan_name,cur_chanid))
	{
		myerror("Cannot find channel: ", temp_chan_name);
		return false;
	}

	ParseServiceID(cur_chanid, new_charid);

	struct tm *btm;
	btm = gmtime(&(ProgramDescription[ProgNumber].btime));
	sprintf(DummyEventName,"%s (%02d/%02d/%04d)",ProgramDescription[ProgNumber].title,btm->tm_mday,btm->tm_mon+1,btm->tm_year+1900);
	
	StripEndl(new_charid);
	HttpParse(new_charid);

	char sUser[MAXTXT];
	char sPwd[MAXTXT];
	strcpy(sUser,ReadINIValue(INIKEY_HTTPUSER,"root"));
	strcpy(sPwd, ReadINIValue(INIKEY_HTTPPASSWORD,"dreambox"));
	sprintf(sDummy,"wget -O /dev/null \"http://%s:%s@localhost/addTimerEvent?ref=%s&ID=%d&start=%d&duration=%d&channel=%s&descr=%s\" &",
		sUser,
		sPwd,
		new_charid,
		0, // ID
		current_start,
		current_end,
		HttpParse(temp_chan_name),
		HttpParse(DummyEventName));

	system(sDummy);

	return true;
}

char *HttpParse(char *str)
{
	char sDummy[MAXTXT];
	unsigned int i_src;
	int l;

	strcpy(sDummy, str);
	strcpy(str,"");

	for(i_src=0;i_src<=strlen(sDummy);i_src++)
	{
		switch(sDummy[i_src])
		{
			case ' ':
				strcat(str,"%20");
				break;
			case ':':
				strcat(str,"%3A");
				break;
			case '(':
				strcat(str,"%28");
				break;
			case ')':
				strcat(str,"%29");
				break;
			case '/':
				strcat(str,"%2F");
				break;
			default:
				l=strlen(str);
				str[l]=sDummy[i_src];
				str[l+1]=0;
				break;
		}
	}
	return str;
}

int eChannelEpgDetail::GetProgRecord(void)
{
	return ProgRecord;
}

int eChannelEpgDetail::FillChannelEPG()
{
	bool exitloop=false;
	char item[MAXTXT*4];
	long c=1;
	struct tm *stime;
	eListBoxEntry *lbItem=NULL, *lbItem_temp;
	time_t tDummy;
	int curr_day=-1;
	int cache_ShowPastEvent;
	long event_count=0;
	int cache_EventToDisplay;
	bool doFocusEPG;

	doFocusEPG = atoi(ReadINIValue(INIKEY_FOCUSEPG,"1"));

	if(now<CURRENTTIMETRESHOLD)
		doFocusEPG = false;

	cache_EventToDisplay = atoi(ReadINIValue(INIKEY_SHOWFWDCHANNEL,"9999"));

	eWaitPlease dlgWait;
	dlgWait.SetMessage(ReadINIValue(INIKEYLB_PLEASEWAIT,"Wait please..."));
	dlgWait.show();

	cache_ShowPastEvent = atoi(ReadINIValue(INIKEY_SHOWPASTCHANNEL,"1"));
	
	while(!exitloop)
	{
		tDummy = ProgramDescription[c+channel].btime;
		tDummy += atoi(ReadINIValue(INIKEY_EPGOFFSET,"0"));
		tDummy += atoi(ReadINIValue(INIKEY_DAYLIGHTSAVE,"0"))*3600;
		stime=gmtime(&tDummy);

		if(event_count<cache_EventToDisplay)
		{
			if(useDaySeparator)
			{
				// Put separator if day changed
				if(curr_day!=stime->tm_mday && curr_day!=-1)
				{
					curr_day = stime->tm_mday;
					if(useDaySeparator==2)
						AddListboxElement(listbox," ",(void *)-1);
					sprintf(item, "**** %s, %02d/%02d ****",day_names[stime->tm_wday],stime->tm_mday, stime->tm_mon+1);
					AddListboxElement(listbox, item, (void *)-1);
					if(useDaySeparator==3)
						AddListboxElement(listbox," ",(void *)-1);
				}
				else if(curr_day==-1) // se e' il primo giro
					curr_day = stime->tm_mday;
			}

			if(cache_ShowPastEvent || ProgramDescription[c+channel+1].btime>now)
			{
				if(ShallDisplay(ProgramDescription+c+channel,mcSearchKey, mSearchMode))
				{
					FormatEPGElement(ProgramDescription+c+channel, item);
					lbItem_temp = AddListboxElement(listbox, item, (void *) (c+channel));
					if(doFocusEPG)
					{
						// Se la fine dell'evento appena inserito (start time + durata) e' maggiore di now allora questo e' l'evento corrente
						if((ProgramDescription[c+channel].btime<now))// && (ProgramDescription[c+channel].btime+ProgramDescription[c+channel].duration*60)>now) 
							lbItem=lbItem_temp;
					}
					event_count++;
				}
			}
		}
		c++;
		if(ProgramDescription[c+channel].duration==-1 || c+channel>ProgramsCount-1)
			exitloop=true;
	}

	if(lbItem)
		listbox->setCurrent(lbItem, false);	

	dlgWait.hide();
	
	return 1;
}

void eChannelEpgDetail::SetTime(time_t mNow)
{
	now = mNow;
}

int eChannelListDialog::FillGridEPG(time_t now)                     
{
	int c, iDummy;
	bool channel_done=false;
	char item[MAXTXT*4];
	int elements_to_print;

	elements_to_print = atoi(ReadINIValue(INIKEY_GRIDELEMENTS, "2"));

	for(c=0; c<ProgramsCount; c++)
	{
		if(ProgramDescription[c].duration==-1) // E' un canale
		{
			channel_done = false;
			AddListboxElement(GridListbox, " ",(void *)-1);
			AddListboxElement(GridListbox, ProgramDescription[c].title,(void *)c);
		}
		else
		{
			if(!channel_done)
			{
				iDummy=elements_to_print+c;
				if(ProgramDescription[c].btime>now)
				{
					for(;c<iDummy;c++)
					{
						FormatEPGElement((ProgramDescription+c-1),item);
						AddListboxElement(GridListbox, item, (void *) (c-1));
					}
					channel_done = true;
				}

			}
		}
	}
	return true;
}

int FormatEPGElement(sProgramDescription *pd, char *element)
{
	struct tm *stime;
	time_t tDummy;
	tDummy = pd->btime;
	tDummy += atoi(ReadINIValue(INIKEY_EPGOFFSET,"0"));
	tDummy += atoi(ReadINIValue(INIKEY_DAYLIGHTSAVE,"0"))*3600;

	if(pd->duration==-1) // Se si tratta di un nome di canale
	{
		strcpy(element,".");
		return false;
	}

	stime=gmtime(&tDummy);

	switch(atoi(ReadINIValue(INIKEY_DATEFORMAT,"0")))
		{
			case 0:
				sprintf(element,"%02d:%02d %c %s (%d')",
					stime->tm_hour,
					stime->tm_min,
					SEPARATOR,pd->title,
					pd->duration);
				break;

			case 1:
				sprintf(element,"%02d/%02d, %02d:%02d %c %s (%d')",
						stime->tm_mday,
						stime->tm_mon+1,
						stime->tm_hour,
						stime->tm_min,
						SEPARATOR,pd->title,
						pd->duration);
				break;

			case 2:
				sprintf(element,"%02d/%02d, %02d:%02d %c %s (%d')",
						stime->tm_mon+1,
						stime->tm_mday,
						stime->tm_hour,
						stime->tm_min,
						SEPARATOR,pd->title,
						pd->duration);
				break;

			case 3:
				sprintf(element,"%s %02d/%02d, %02d:%02d %c %s (%d')",
						day_names[stime->tm_wday],
						stime->tm_mon+1,
						stime->tm_mday,
						stime->tm_hour,
						stime->tm_min,
						SEPARATOR,pd->title,
						pd->duration);
				break;

			default:
				sprintf(element,"invalid date format in ini file");
				break;

	}
				
	return true;
}

void eChannelEpgDetail::SetProgramsCount(long pc)
{
	ProgramsCount = pc;
}

void eChannelEpgDetail::SetProgramDescription(sProgramDescription *pd)
{
	ProgramDescription = pd;
}

void eWaitPlease::SetMax(int m)
{
	maxvalue = m;
}

void eWaitPlease::SetValue(int v)
{
	static int old_p=-1; // Aggiorniamo solo se necessaio visto che le visualizzazioni portano via un sacco di tempo
	int new_p;

	new_p = (int)(((float)(v*100))/(float)maxvalue);

	if(old_p==new_p)
	{
		return;
	}
	old_p = new_p;
	char sDummy[MAXTXT];
	sprintf(sDummy,"%s (%d%%)",message, new_p);	
	lb_status->setText(sDummy);
	return;
}

void eChannelListDialog::InitDialog()
{
	UpdatingListboxes = true;
	FillChannelList();
	FillChannelListInit = true;
	RefreshWindowMode();
	FocusListbox();
	UpdatingListboxes = false;
}

void eChannelListDialog::FocusListbox()
{	
	char view_name[MAXTXT];
	char *search;

	switch(display_mode)
	{
		case channel_view:
			setFocus(ChanListbox);
			strcpy(view_name,ReadINIValue(INIKEYLB_LBCHANNELVIEW,"Channel View"));
			break;
		case grid_view:
			setFocus(GridListbox);
			strcpy(view_name,ReadINIValue(INIKEYLB_LBGRIDVIEW,"Grid View"));
			break;
		case allprog_view:
			setFocus(AllProgListbox);
			strcpy(view_name,ReadINIValue(INIKEYLB_LBALLVIEW,"All view"));
			break;
		case themes_view:
			setFocus(ProgByThemeListbox);
			strcpy(view_name,ReadINIValue(INIKEYLB_LBTHEMEVIEW,"Theme view"));
			break;
	}
	
	search = RefreshSearchMode();

	if(search)
	{
		strcat(view_name,", ");
		strcat(view_name,search);
	}
	lb_status->setText(view_name);

}

int ZapIT(int channel, sProgramDescription *pd)
{
	char sID[MAXTXT], zID[MAXTXT], c_temp[MAXTXT];
	strcpy(c_temp,pd[channel].title);
	if(!LookupServiceByChannel(pd[channel].title, sID))
		return false;
	ParseServiceID(sID, zID);
	eString sRefEpg = eString().sprintf("%s",StripEndl(zID));
	eZapMain::getInstance()->playService(sRefEpg.upper(), eZapMain::psNoUser|eZapMain::psSetMode);
	return true;
}

void eChannelEpgDetail::OnZap()
{
	if(!ZapIT(channel, ProgramDescription))
		myerror("What's this channel? ;))");
	setFocus(listbox);
}

int FindChannelFromProg(int prog_num, sProgramDescription *pd)
{
	int channel=-1;
	while(channel==-1 && prog_num>0)
	{
		if(pd[prog_num].duration==-1)
			channel=prog_num;
		else
			prog_num--;
	}

	// LKtemp
	if(prog_num==0 && channel==-1)
		channel=1;
	
	return channel;
}

int ReadChanName()
{
	eString name;
	eServiceHandler *sapi=eServiceInterface::getInstance()->getService();
	if (sapi)
	{	
		eServiceReference &ref=eServiceInterface::getInstance()->service;
		if (ref)
		{	
			eService *service = eServiceInterface::getInstance()->addRef(ref);
			if (service)
			{	
				if (ref.type == eServiceReference::idDVB)
					name = service->service_name;
				if (name.length())
				{	strcpy(sCurrentEnigmaChannel,(char*)name.c_str());
					StripEndl(sCurrentEnigmaChannel);
				}
				eServiceInterface::getInstance()->removeRef(ref);
			}
		}
	}
	return true;
}
