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
#include "dbset.h"
#include "nemtool.h"
#include "setpath.h"
#include <enigma_main.h>

eDbset::eDbset()
	:eSetupWindow(_("Dreambox setting"), 15, 450)
{
	int sW = 720;
	int sH = 576;
	int winW = 450;
	int winH = clientrect.height() ;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));

	int entry=0;
	CONNECT((new eListBoxEntryCheck(&list, _(" Disable Blu Panel's programs at boot"), "/Nemesis-Project/tools/emucsstartdisable",eString().sprintf("(%d) %s", ++entry, _("Disable start blu panel's programs at system boot")) ))->selected, eDbset::saveConf);
	CONNECT((new eListBoxEntryCheck(&list, _(" Can handle two services"), "/ezap/ci/handleTwoServices",eString().sprintf("(%d) %s", ++entry, _("can your CI descramble two services at the same time?")) ))->selected, eDbset::saveConf);
	CONNECT((new eListBoxEntryCheck(&list, _(" Enable lite skin on zapping"), "/Nemesis-Project/tools/liteskinzap",eString().sprintf("(%d) %s", ++entry, _("Enable / Disable lite skin on zapping")) ))->selected, eDbset::saveConf);
	if ((eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM500PLUS) || 
		(eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM500) || 
		(eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM5620))
		CONNECT((new eListBoxEntryCheck(&list, _(" Enable lite skin on INFO press"), "/Nemesis-Project/tools/liteskin",eString().sprintf("(%d) %s", ++entry, _("Enable / Disable lite skin on INFO press")) ))->selected, eDbset::saveConf);
	else
		CONNECT((new eListBoxEntryCheck(&list, _(" Enable lite skin on OK press"), "/Nemesis-Project/tools/liteskin",eString().sprintf("(%d) %s", ++entry, _("Enable / Disable lite skin on OK press")) ))->selected, eDbset::saveConf);
	if ((eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM500) || 
		(eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM500PLUS))
	{	int remotePVR = 0;
		eConfig::getInstance()->getKey("/Nemesis-Project/tools/pvrmode", remotePVR);
		if (!remotePVR)
			CONNECT((new eListBoxEntryCheck(&list, _(" Enable DM7000 remote"), "/Nemesis-Project/tools/remote7000",eString().sprintf("(%d) %s", ++entry, _("Use DM7000 remote")) ))->selected, eDbset::remoteToggle);
		int remote7000 = 0;
		eConfig::getInstance()->getKey("/Nemesis-Project/tools/remote7000", remote7000);
		if (!remote7000)
			CONNECT((new eListBoxEntryCheck(&list, _(" Enable PVR Mode"), "/Nemesis-Project/tools/pvrmode",eString().sprintf("(%d) %s", ++entry, _("Use Help key for PVR")) ))->selected, eDbset::pvrToggle);
	}
	new eListBoxEntryMenuSeparator(&list, eSkin::getActive()->queryImage("listbox.separator"), 0, true );
	CONNECT((new eListBoxEntryCheck(&list, _(" Hide ECM info in OSD"), "/Nemesis-Project/infobar/disableecm",eString().sprintf("(%d) %s", ++entry, _("Hide ECM info in infobar")) ))->selected, eDbset::saveConf);
	CONNECT((new eListBoxEntryCheck(&list, _(" Hide Network info in OSD"), "/Nemesis-Project/infobar/disablenet",eString().sprintf("(%d) %s", ++entry, _("Hide Network info in infobar")) ))->selected, eDbset::saveConf);
	CONNECT((new eListBoxEntryCheck(&list, _(" Show DreamCrypt as Irdeto in OSD"), "/Nemesis-Project/infobar/showdcasirdeto",eString().sprintf("(%d) %s", ++entry, _("Show DreamCrypt as Irdeto in OSD")) ))->selected, eDbset::saveConf);
	CONNECT((new eListBoxEntryCheck(&list, _(" Show Provider with Channel in OSD"), "/Nemesis-Project/infobar/showprowithchan",eString().sprintf("(%d) %s", ++entry, _("Show Provider name with Channel in OSD")) ))->selected, eDbset::pvToggle);
	CONNECT((new eListBoxEntryCheck(&list, _(" Show long month Date in OSD"), "/Nemesis-Project/infobar/showlongdate",eString().sprintf("(%d) %s", ++entry, _("Show long month Date in OSD")) ))->selected, eDbset::saveConf);
	CONNECT((new eListBoxEntryCheck(&list, _(" Show buttons border"), "/Nemesis-Project/tools/showborder",eString().sprintf("(%d) %s", ++entry, _("Show buttons border")) ))->selected, eDbset::saveConf);
	CONNECT((new eListBoxEntryCheck(&list, _(" Show Channell icons (Picon)"), "/Nemesis-Project/tools/showpicon",eString().sprintf("(%d) %s", ++entry, _("Show Channell icons (Picon)")) ))->selected, eDbset::piconToggle);
	CONNECT((new eListBoxEntryCheck(&list, _(" Use Picon by Channel Name"), "/Nemesis-Project/tools/picontype",eString().sprintf("(%d) %s", ++entry, _("Use Picon by Channel Name")) ))->selected, eDbset::saveConf);
	CONNECT((new eListBoxEntryCheck(&list, _(" Enable EPG Blacklist"), "/Nemesis-Project/tools/enaepgbl",eString().sprintf("(%d) %s", ++entry, _("Enable EPG Blacklist")) ))->selected, eDbset::saveConf);
}

eDbset::~eDbset()
{
}

void eDbset::saveConf(bool newState)
{
	eConfig::getInstance()->flush();
}

void eDbset::remoteToggle(bool newState)
{
	eString cmd;
	if (eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM500)
	{	if (nemTool::getInstance()->exists("/var/tuxbox/config/enigma/resources/rcdm5xxx.xml",false))
		{	cmd = "rm -f /var/tuxbox/config/enigma/resources/rcdm5xxx.xml";
			system(cmd.c_str());
		}
		if (newState)
		{	eConfig::getInstance()->setKey("/Nemesis-Project/tools/pvrmode", 0);
			if (nemTool::getInstance()->exists("/share/tuxbox/enigma/resources/rcdm5xxx.xml.7000",false))
			{	cmd = "ln -s  /share/tuxbox/enigma/resources/rcdm5xxx.xml.7000 /var/tuxbox/config/enigma/resources/rcdm5xxx.xml";
				system(cmd.c_str());
			}
		}
	}else
	{	system("rm -f /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml");
		system("ln -f /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml.orig /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml");
		if (newState)
		{	eConfig::getInstance()->setKey("/Nemesis-Project/tools/pvrmode", 0);
			if (nemTool::getInstance()->exists("/usr/share/tuxbox/enigma/resources/rcdm5xxx.xml.7000",false))
			{	system("rm -f /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml");
				system("ln -f /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml.7000 /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml");
			}
		}
	}
	eMessageBox msg(_("You have to restart enigma to apply the new remote setting\nRestart now?"), _("Remote changed"), eMessageBox::btYes|eMessageBox::btNo|eMessageBox::iconQuestion, eMessageBox::btYes );
	msg.show(); int button=msg.exec(); msg.hide();
	if (button == eMessageBox::btYes)
		eApp->quit(2);
}

void eDbset::piconToggle(bool newState)
{
	if (newState)
	{	eMessageBox msg(_("The Picon function is actvated!\nYou do configure the Picon image folder.\nDo you want configure it now?"), 
						_("Picon"), eMessageBox::btYes|eMessageBox::btNo|eMessageBox::iconQuestion, eMessageBox::btYes );
		msg.show(); int button=msg.exec(); msg.hide();
		if (button == eMessageBox::btYes)
		{	
			hide();	
				eSetpath dlg; 
				dlg.show(); dlg.exec(); dlg.hide();
			show();
		}
	}
}

void eDbset::pvrToggle(bool newState)
{
	eString cmd;
	if (eSystemInfo::getInstance()->getHwType() ==  eSystemInfo::DM500)
	{	if (nemTool::getInstance()->exists("/var/tuxbox/config/enigma/resources/rcdm5xxx.xml",false))
		{	cmd = "rm -f /var/tuxbox/config/enigma/resources/rcdm5xxx.xml";
			system(cmd.c_str());
		}
		if (newState)
		{	eConfig::getInstance()->setKey("/Nemesis-Project/tools/remote7000", 0);
			if (nemTool::getInstance()->exists("/share/tuxbox/enigma/resources/rcdm5xxx.xml.PVR",false))
			{	cmd = "ln -s  /share/tuxbox/enigma/resources/rcdm5xxx.xml.PVR /var/tuxbox/config/enigma/resources/rcdm5xxx.xml";
				system(cmd.c_str());
			}
		}
	}else
	{	system("rm -f /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml");
		system("ln -f /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml.orig /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml");
		if (newState)
		{	eConfig::getInstance()->setKey("/Nemesis-Project/tools/remote7000", 0);
			if (nemTool::getInstance()->exists("/usr/share/tuxbox/enigma/resources/rcdm5xxx.xml.PVR",false))
			{	system("rm -f /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml");
				system("ln -f /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml.PVR /usr/share/tuxbox/enigma/resources/rcdm5xxx.xml");
			}
		}
	}
	eMessageBox msg(_("You have to restart enigma to apply the new remote setting\nRestart now?"), _("Remote changed"), eMessageBox::btYes|eMessageBox::btNo|eMessageBox::iconQuestion, eMessageBox::btYes );
	msg.show(); int button=msg.exec(); msg.hide();
	if (button == eMessageBox::btYes)
		eApp->quit(2);
}

void eDbset::pvToggle(bool newState)
{
	eString sRef;
	if (eServiceInterface::getInstance()->service)
		sRef = eServiceInterface::getInstance()->service.toString();
	eConfig::getInstance()->flush();
	eZapMain::getInstance()->playService(sRef, eZapMain::psSetMode|eZapMain::psDontAdd);

}

