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
#include "np.h"
#include "deamon.h"
#include "showfile.h"
#include "addons.h"
#include "sysinfo.h"
#include "extra.h"
#include "dbset.h"
#include "nemtool.h"
#include "utility.h"

eString eCbpMain::readEmuName(eString emu)
{
	char buf[256];
	eString emuName;
	FILE *in = fopen(eString("/var/script/" + emu + "_em.sh").c_str(), "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			emuName = eString().sprintf("%s", buf);
			emuName=emuName.left(emuName.length() - 1);		
			if (emuName.left(9) == "#emuname=")
			{	
				fclose(in);
				emuName = emuName.right(emuName.length() - 9);
				return emuName;
			}
		}
		fclose(in);
	}
	return emu;
}

eString eCbpMain::readEmuActive()
{
	char buf[256];
	eString emu;
	FILE *in = fopen("/var/bin/emuactive", "r");
	if(in)
	{	fgets(buf, 256, in);
		emu=eString().sprintf("%s", buf);
		fclose(in);
		return emu;
	} else return "None";
}

void eCbpMain::loadEmuList()
{
	char emuactive[100];
	emuactive[0]=0;

	emulistbox->beginAtomic(); 
	emulistbox->clearList();
	FILE *fp = fopen("/var/bin/emuactive","r");
	if (fp) {
		memset(emuactive, 0, sizeof(emuactive));
		fgets(emuactive, 200, fp);
		fclose(fp);
		int a=0;
		for (a=0; a<strlen(emuactive); a++) if (emuactive[a]==(char)0x0a) emuactive[a]=0;
	}

	if ((eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM500) || 
		(eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM500PLUS))
	{	new eListBoxEntryText(emulistbox, "None", (void*)0, 2);
	} else
		new eListBoxEntryText(emulistbox, "Common Interface", (void*)0, 2);
	emulist[0]="None";
	eListBoxEntryText *lblast, *lbselected=0;

	DIR *d = opendir("/var/script");
	while (struct dirent *e=readdir(d))
	{
		if ( strstr(e->d_name,"_em.sh")) 
		{
			memset(emuname, 0, sizeof(emuname));
			strncpy(emuname, e->d_name, strlen(e->d_name)-6);
			lblast = new eListBoxEntryText(emulistbox, readEmuName(emuname), (void*)emulistbox->getCount(), 2);
			emulist[(int)emulistbox->getCount() - 1]=emuname;
			if ( strcmp(emuactive, emuname)==0 ) lbselected=lblast;
		}
	}
	closedir(d);
	if (emulistbox->getCount())
		emulistbox->sort();	
	if (lbselected) emulistbox->setCurrent( lbselected, false );
	emulistbox->endAtomic();
	
}

void eCbpMain::loadSrvList()
{
	char srvactive[20];
	srvactive[0]=0;

	serverlistbox->beginAtomic(); 
	serverlistbox->clearList();
	FILE *fp = fopen("/var/bin/csactive","r");
	if (fp) {
		memset(srvactive, 0, sizeof(srvactive));
		fgets(srvactive, 200, fp);
		fclose(fp);
		int a=0;
		for (a=0; a<strlen(srvactive); a++) if (srvactive[a]==(char)0x0a) srvactive[a]=0;
	}

	new eListBoxEntryText(serverlistbox, "None", (void*)0, 2);
	eListBoxEntryText *lblast, *lbselected=0;
	srvlist[0]="None";

	DIR *d = opendir("/var/script");
	if (d) {
		while (struct dirent *e=readdir(d))
		{
			if ( strstr(e->d_name,"_cs.sh")) 
			{
				memset(srvname, 0, sizeof(srvname));
				strncpy(srvname, e->d_name, strlen(e->d_name)-6);
				lblast = new eListBoxEntryText(serverlistbox, readSrvName(srvname), (void*)serverlistbox->getCount(), 2);
				srvlist[(int)serverlistbox->getCount() - 1]=srvname;
				if ( strcmp(srvactive, srvname)==0 ) lbselected=lblast;
			}
		}
		closedir(d);
	}
	if (serverlistbox->getCount())
		serverlistbox->sort();
	if (lbselected) serverlistbox->setCurrent( lbselected, false );
	serverlistbox->endAtomic();
}

eString eCbpMain::readSrvActive()
{
	char buf[256];
	eString srv;
	FILE *in = fopen("/var/bin/csactive", "r");
	if(in)
	{	fgets(buf, 256, in);
		srv=eString().sprintf("%s", buf);
		fclose(in);
		return srv;
	} else return "None";
}

eString eCbpMain::readSrvName(eString srv)
{
	char buf[256];
	eString srvName;
	FILE *in = fopen(eString("/var/script/" + srv + "_cs.sh").c_str(), "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			srvName = eString().sprintf("%s", buf);
			srvName=srvName.left(srvName.length() - 1);		
			if (srvName.left(9) == "#srvname=")
			{	
				fclose(in);
				srvName = srvName.right(srvName.length() - 9);
				return srvName;
			}
		}
		fclose(in);
	}
	return srv;
}

bool eCbpMain::readNews()
{
	eMessageBox box(_("Download EDG-Nemesis news..."), _("Running.."),0);
	box.show();
		eString url=nemTool::getInstance()->readAddonsUrl();
		bool ret = nemTool::getInstance()->sendCmd((char *) eString("wget " + url + "/info.txt -O /tmp/upd_script.out").c_str());
		usleep(300000);
	box.hide();
	return ret;
}

eCbpMain::eCbpMain()//: eWindow(1)
{
	cmove(ePoint((sW - mainW) / 2,((sH - mainH) / 2) + 15));
	cresize(eSize(mainW, mainH));
	setText((_("EDG-Nemesis Panel "VDF)));

	emulistbox= new eListBox< eListBoxEntryText >(this);
	emulistbox->setFlags(eListBoxBase::flagNoUpDownMovement);
	emulistbox->setName("emulistbox");
	emulistbox->setHelpText(_("Choose emulator"));
	emulistbox->move(ePoint(25, 10));
	emulistbox->resize(eSize (mainW - 50, 35));
	emulistbox->loadDeco();
	CONNECT(emulistbox->selected, eCbpMain::emulistbox_selected);
	CONNECT(emulistbox->selchanged, eCbpMain::emulistbox_changed);

	lb1=new eLabel(this);
	lb1->move(ePoint(5,15));
	lb1->resize(eSize(15,35));
	lb1->setText("<");

	lb2=new eLabel(this);
	lb2->move(ePoint(mainW - 15, 15));
	lb2->resize(eSize(15,35));
	lb2->setText(">");

	serverlistbox=new eListBox<eListBoxEntryText>(this);
	serverlistbox->setFlags(eListBoxBase::flagNoUpDownMovement);
	serverlistbox->setName("serverlistbox");
	serverlistbox->setHelpText(_("Choose cardserver"));
	serverlistbox->move(ePoint(25, 55));
	serverlistbox->resize(eSize (mainW - 50, 35));
	serverlistbox->loadDeco();
	CONNECT(serverlistbox->selected, eCbpMain::serverlistbox_selected);
	CONNECT(serverlistbox->selchanged, eCbpMain::serverlistbox_changed);

	lb3=new eLabel(this);
	lb3->move(ePoint(5,60));
	lb3->resize(eSize(15,35));
	lb3->setText("<");

	lb4=new eLabel(this);
	lb4->move(ePoint(mainW - 15, 60));
	lb4->resize(eSize(15,35));
	lb4->setText(">");

	label=new eLabel(this);
	label->move(ePoint(5,100));
	label->resize(eSize(mainW - 10,30));
	label->setText(_("Press OK to start/restart EMU or CS"));
	label->setAlign(2);

	textlist=new eListBox<eListBoxEntryText>(this,0,0);
	textlist->move(ePoint(5, 135));
	textlist->resize(eSize(clientrect.width()-10, 30));
	new eListBoxEntryTextSeparator(textlist, eSkin::getActive()->queryImage("listbox.separator"), 0, true );

	eString eFunction[NUM_FUNC - 4];
	eString eHelpText[NUM_FUNC - 4];
	eFunction[0] = _(" System information");	eHelpText[0] = _("Open system info panel");
	eFunction[1] = _(" EDG-Nemesis News");	eHelpText[1] = _("Show EDG-Nemesis Group news");
	eFunction[2] = _(" Start - Stop programs");	eHelpText[2] = _("Start - Stop external programs");
	if ( eConfig::getInstance()->getParentalPin() )
	{	if ( eConfig::getInstance()->pLockActive() )
		{	eFunction[3] = _(" Unlock parental control");
		}
		else
		{	eFunction[3] = _(" Lock parental control");
		}
		eHelpText[3] = _("Lock,unlock (parental control)");
	} else
	{	eFunction[3] = _(" Set parental control");
		eHelpText[3] = _("Set parental control");
	} 
	eFunction[4] = _(" About EDG-Nemesis");	eHelpText[4] = _("About EDG-Nemesis");

	int bgColor = eSkin::getActive()->queryColor("global.normal.background");
	int btPos = 135;

	for(int i=0; i<NUM_FUNC-4; i++)
	{
		bt_func[i]=new eButton(this);
		bt_func[i]->move(ePoint(10, btPos += 25));
		bt_func[i]->resize(eSize(mainW - 20, 25));
		bt_func[i]->setText(eFunction[i].c_str());
		bt_func[i]->setShortcut(eString().sprintf("%d",i+1));
		bt_func[i]->setShortcutPixmap(eString().sprintf("%d",i+1));
		bt_func[i]->setAlign(0);
		bt_func[i]->setHelpText(eHelpText[i].c_str());
		bt_func[i]->setProperty("backgroundColor", eString().sprintf("%i",bgColor).c_str());
		CONNECT_1_0(bt_func[i]->selected, eCbpMain::Listeselected, i);
	}

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);
	int btnsize = (mainW -4) / 4;

	bt_func[5]=new eButton(this,0,0);
	bt_func[5]->move(ePoint(2, mainH - 70));
	bt_func[5]->resize(eSize(btnsize, 35));
	bt_func[5]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_func[5]->setProperty("shortcut", "red");
	bt_func[5]->setText(_("Utility"));
	bt_func[5]->setHelpText(_("Open utility panel"));
	bt_func[5]->setAlign(0);
	if (showBorder) bt_func[5]->loadDeco();
	CONNECT_1_0(bt_func[5]->selected, eCbpMain::Listeselected, 5);

	bt_func[6]=new eButton(this,0,0);
	bt_func[6]->move(ePoint(btnsize + 2, mainH - 70));
	bt_func[6]->resize(eSize(btnsize, 35));
	bt_func[6]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_func[6]->setProperty("shortcut", "green");
	bt_func[6]->setText(_("Extra"));
	bt_func[6]->setHelpText(_("Open Extra tools panel"));
	bt_func[6]->setAlign(0);
	if (showBorder) bt_func[6]->loadDeco();
	CONNECT_1_0(bt_func[6]->selected, eCbpMain::Listeselected, 6);

	bt_func[7]=new eButton(this,0,0);
	bt_func[7]->move(ePoint(btnsize * 2  + 2, mainH - 70));
	bt_func[7]->resize(eSize(btnsize, 35));
	bt_func[7]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_func[7]->setProperty("shortcut", "yellow");
	bt_func[7]->setText(_("Setting"));
	bt_func[7]->setHelpText(_("Open setup Dreambox Panel"));
	bt_func[7]->setAlign(0);
	if (showBorder) bt_func[7]->loadDeco();
	CONNECT_1_0(bt_func[7]->selected, eCbpMain::Listeselected, 7);

	bt_func[8]=new eButton(this,0,0);
	bt_func[8]->move(ePoint(btnsize * 3 + 2, mainH - 70));
	bt_func[8]->resize(eSize(btnsize, 35));
	bt_func[8]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_func[8]->setProperty("shortcut", "blue");
	bt_func[8]->setText(_("Addons"));
	bt_func[8]->setHelpText(_("Open Addons Panel"));
	bt_func[8]->setAlign(0);
	if (showBorder) bt_func[8]->loadDeco();
	CONNECT_1_0(bt_func[8]->selected, eCbpMain::Listeselected, 8);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, mainH - 30) );
	sbar->resize(eSize(mainW, 30));
	sbar->loadDeco();	
	loadEmuList();
	setArrowEmu();	
	loadSrvList();
	setArrowSrv();	

}

void eCbpMain::emulistbox_changed(eListBoxEntryText *item)
{
	if (item) setArrowEmu();
}

void eCbpMain::serverlistbox_changed(eListBoxEntryText *item)
{
	if (item) setArrowSrv();
}

void eCbpMain::setArrowEmu()
{
	int Count = (int) emulistbox->getCount();
	int Pos = (int) emulistbox->getCurrent()->getKey() + 1;

	if (Count == 1)
	{	lb1->hide();
		lb2->hide();
	}
	else if (Pos == 1) 
	{	lb1->hide();
		lb2->show();
	}
	else if (Count == Pos)
	{	lb1->show();
		lb2->hide();
	}
	else
	{
		lb1->show();
		lb2->show();
	}
}

void eCbpMain::setArrowSrv()
{
	int Count = (int) serverlistbox->getCount();
	int Pos = (int) serverlistbox->getCurrent()->getKey() + 1;

	if (Count == 1)
	{	lb3->hide();
		lb4->hide();
	}
	else if (Pos == 1) 
	{	lb3->hide();
		lb4->show();
	}
	else if (Count == Pos)
	{	lb3->show();
		lb4->hide();
	}
	else
	{
		lb3->show();
		lb4->show();
	}
}

extern bool checkPin( int pin, const char * text );

void eCbpMain::Listeselected(int key)
{
	if (key == 0)
	{	//Show System info
		hide();
		eSysInfo dlg; 
		dlg.show(); dlg.exec(); dlg.hide();
		show();
	}
	else
	if (key == 1)
	{	//download news
		hide();
		if (readNews()) cbpShowFile::getInstance()->showLog(_("EDG-Nemesis News"),0,false);
		show();
	}
	else
	if (key == 2)
	{	//Show Deamon Manager
		hide();
		eDeamon dlg; 
		dlg.show(); dlg.exec(); dlg.hide();
		show();
	}
	else
	if (key == 3)
	{	//Lock / unlock parental control
		if ( eConfig::getInstance()->getParentalPin() )
		{
			if ( eConfig::getInstance()->pLockActive() )
			{	if ( !checkPin( eConfig::getInstance()->getParentalPin(), _("parental")) )
					return;
			}
			eString sRef;
			if (eServiceInterface::getInstance()->service)
				sRef = eServiceInterface::getInstance()->service.toString();
			eConfig::getInstance()->locked^=1;
			if ( eServiceInterface::getInstance()->service.isLocked() && eConfig::getInstance()->locked )
				eServiceInterface::getInstance()->stop();
			close(0);
			eZapMain::getInstance()->playService(sRef, eZapMain::psSetMode|eZapMain::psDontAdd);
		} else
		{
			hide();
				eParentalSetup setup;
				setup.show();
				setup.exec();
				setup.hide();
			show();
			if ( eConfig::getInstance()->getParentalPin() )
			{	if ( eConfig::getInstance()->pLockActive() )
					bt_func[key]->setText(_(" Unlock parental control"));
				else
					bt_func[key]->setText(_(" Lock parental control"));
				bt_func[key]->setHelpText(_("Lock,unlock (parental control)"));
			}
		}
	}
	else
	if (key == 4)
	{	//Show About
		hide();
		eString message = "EDG-Nemesis Version "NEMESISVER"\nBased on cvs from "CVSDATE" (Fixed by Gianathem)\n(c) "NEMESISVERDATE" by EDG-Nemesis Group\n\nhttp://www.edg-nemesis.tv/";
		eMessageBox msg(_(message.c_str()), _("About EDG-Nemesis "NEMESISVER), eMessageBox::iconInfo|eMessageBox::btOK); 
		msg.show(); msg.exec(); msg.hide();
		show();
	}
	else
	if (key == 5)
	{	//open utility panel
		hide();
		eUtility dlg; 
		dlg.show(); dlg.exec(); dlg.hide();
		show();
	}
	else
	if (key == 6)
	{	//open extra tools panel
		hide();
		eExtra dlg; 
		dlg.show(); dlg.exec(); dlg.hide();
		show();
	}
	else
	if (key == 7)
	{	//open setup panel
		hide();
		eDbset dlg; 
		dlg.show(); dlg.exec(); dlg.hide();
		show();
	}
	else
	if (key == 8)
	{	//open addons panel
		hide();
		eChooseAddons dlg;
		dlg.show(); dlg.exec(); dlg.hide();
		loadSrvList();
		setArrowSrv();
		loadEmuList();
		setArrowSrv();
		show();
	}
	if (key <= 3) setFocus(bt_func[key]);
}	

void eCbpMain::bt_start_emu()
{
	eString emuActive = readEmuActive();
	if ((emulistbox->getCurrent()->getText() == "None" || emulistbox->getCurrent()->getText() == "Common Interface") && emuActive == "None")
		return;
	eString newEmu = emulist[(int)emulistbox->getCurrent()->getKey()];
	hide();
	FILE *fp = fopen("/var/bin/emuactive","w");
	if (fp) {
		fputs(newEmu.c_str(), fp);
		fclose(fp);
	}
	int serviceFlags = eServiceInterface::getInstance()->getService()->getFlags();
	int isCrypted = 0;
	if ( serviceFlags & eServiceHandler::flagIsScrambled ) isCrypted = 1;
	eString sRef;
	if (eServiceInterface::getInstance()->service)
		sRef = eServiceInterface::getInstance()->service.toString();
	if ( isCrypted )
		Decoder::Flush();
	if (emuActive != "None")
	{ 
		eMessageBox box1(eString().sprintf(_("Stopping %s"), readEmuName(emuActive).c_str()), _("Running.."), 0);
		box1.resize(eSize(400, 120));
		box1.show();
			nemTool::getInstance()->sendCmd((char *) eString("/var/script/" + emuActive + "_em.sh stop").c_str());
		box1.hide();
	}
	if (newEmu != "None")
	{ 
		eMessageBox box2(eString().sprintf(_("Starting %s"), readEmuName(newEmu).c_str()), _("Running.."), 0);
		box2.resize(eSize(400, 120));
		box2.show();
			nemTool::getInstance()->sendCmd((char *) eString("/var/script/" + newEmu + "_em.sh start").c_str());
		box2.hide();	
	}
	if ( isCrypted )
		eZapMain::getInstance()->playService(sRef, eZapMain::psSetMode|eZapMain::psDontAdd);
	close(0);
	return;
}

void eCbpMain::emulistbox_selected(eListBoxEntryText *item)
{
	bt_start_emu();
}	

void eCbpMain::serverlistbox_selected(eListBoxEntryText *item)
{
	bt_start_srv();

}	

void eCbpMain::bt_start_srv()
{
	eString srvActive=readSrvActive();
	if (serverlistbox->getCurrent()->getText() == "None" && srvActive == "None")
		return;
	eString emuActive = readEmuActive();
	eString emuName = readEmuName(emuActive);
	eString newSrv = srvlist[(int)serverlistbox->getCurrent()->getKey()];
	if (emuActive != "None" && srvActive == "None")
	{	nemTool::getInstance()->msgfail(eString().sprintf(_("The emulator '%s' is started!\nPlease stop it before start cardserver."),emuName.c_str()));	
		return;
	}
	int isRestart = false;
	if (srvActive == newSrv)
		isRestart = true;
	hide();
	FILE *fp = fopen("/var/bin/csactive","w");
	if (fp) {
		fputs(newSrv.c_str(), fp);
		fclose(fp);
	}
	if (srvActive != "None")
	{ 
		eMessageBox box1(eString().sprintf(_("Stopping %s"), readSrvName(srvActive).c_str()), _("Running.."), 0);
		box1.resize(eSize(400, 120));
		box1.show();
			nemTool::getInstance()->sendCmd((char *) eString("/var/script/" + srvActive + "_cs.sh stop").c_str());
		box1.hide();
	}
	if (newSrv != "None")
	{ 
		eMessageBox box2(eString().sprintf(_("Starting %s"), readSrvName(newSrv).c_str()), _("Running.."), 0);
		box2.resize(eSize(400, 120));
		box2.show();
			nemTool::getInstance()->sendCmd((char *) eString("/var/script/" + newSrv + "_cs.sh start").c_str());
		box2.hide();	
	}
	if (isRestart)
		close(0);
	show();
	return;
}

eCbpMain::~eCbpMain()
{
}
