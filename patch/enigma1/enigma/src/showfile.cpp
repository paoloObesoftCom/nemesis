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
#include "showfile.h"

cbpShowFile *cbpShowFile::instance;

void cbpShowFile::readInfo(char *eTxt)
{
	listbox->beginAtomic(); 
	listbox->clearList();
	char buf[512];
	FILE *f = fopen(eTxt, "r");
	if (f)
	{	int entry = 0;
		while (fgets(buf, 512, f))
			new eListBoxEntryText(listbox, buf , (void*)++entry);
		fclose(f);
	}
	listbox->endAtomic();
// 	setFocus(listbox);
}

void cbpShowFile::showLog(eString eTitle, char *eTxt, long iDelay)
{	
	if (!instance)
		instance=this;
	if (!eTxt)
		eTxt="/tmp/upd_script.out";
	if (iDelay)
		iDelay *= 1000;
	cbpShowFile dlg1(eTitle, eTxt, iDelay);
	dlg1.show(); dlg1.exec(); dlg1.hide();
}


cbpShowFile::cbpShowFile(eString title, char *eTxt, long iDelay): eWindow(1),refreshTimer(eApp)
{
	int sW = 720;
	int sH = 576;
	int winW = 596;
	int winH = 400;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(title);

	listbox = new eListBox<eListBoxEntryText>(this);
	listbox->move(ePoint(5, 5));
	listbox->resize(eSize(clientrect.width() - 20, clientrect.height() - 40));
// 	listbox->setFlags(eListBoxBase::flagNoPageMovement);
	CONNECT(listbox->selected, cbpShowFile::Listeselected);

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_abort=new eButton(this);
	bt_abort->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_abort->move(ePoint(5 , clientrect.height()-35));
	bt_abort->resize(eSize(100, 30));
	bt_abort->setShortcut("red");
	bt_abort->setShortcutPixmap("red");
	bt_abort->setText(_("Close"));
	bt_abort->setAlign(0);
	if (showBorder) bt_abort->loadDeco();
	CONNECT(bt_abort->selected, eWidget::reject);
	
	readInfo(eTxt);

	if (iDelay)
	{
		refreshTimer.start(iDelay,false);
		refreshTimerConnection = CONNECT_2_1(refreshTimer.timeout, cbpShowFile::readInfo,eTxt);
	}
}

cbpShowFile::~cbpShowFile()
{
	if (instance==this)
		instance=0;
	if ( refreshTimerConnection.connected() )
	{	
		refreshTimerConnection.disconnect();
		refreshTimer.stop();
	}
}

void cbpShowFile::Listeselected(eListBoxEntryText *item)
{
	if (item)
	{	
		hide();
		cpbShowDetail dlg(listbox->getCurrent()->getText()); 
		dlg.show(); dlg.exec(); dlg.hide();
		show();
		setFocus(listbox);
	}else
		close(0);
}

cpbShowDetail::cpbShowDetail(eString details)
{
	int sW = 720;
	int sH = 576;
	int winW = 596;
	int winH = 200;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Show details"));

	labeldet = new eLabel(this);
	labeldet->setFlags(RS_WRAP);
	labeldet->move(ePoint(5, 5));
	labeldet->resize(eSize(clientrect.width() - 10, clientrect.height() - 40));
	labeldet->setAlign(0);
	labeldet->setText(details);
	
	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_abort=new eButton(this);
	bt_abort->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_abort->move(ePoint(clientrect.width() /2 -50 , clientrect.height() - 35));
	bt_abort->resize(eSize(100, 30));
	bt_abort->setShortcut("blue");
	bt_abort->setShortcutPixmap("blue");
	bt_abort->setAlign(0);
	if (showBorder) bt_abort->loadDeco();
	bt_abort->setText(_("Close"));
	CONNECT(bt_abort->selected, eWidget::reject);
	setFocus(bt_abort);
}

cpbShowDetail::~cpbShowDetail()
{
}
