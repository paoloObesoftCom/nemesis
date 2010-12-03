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
#include "extra.h"
#include "userscript.h"
#include "nemtool.h"
#include "movvar.h"
#include "epgmanager.h"
#include "hdd.h"
#include "swap.h"
#include "inadyn.h"
#include "setiptable.h"
#include <enigma_main.h>

eExtra::eExtra()
	:eSetupWindow(_("Extra tools"), 10, 400)
{
	int sW = 720;
	int sH = 576;
	int winW = 400;
	int winH = clientrect.height() ;

	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));

	int entry=0;
	CONNECT((new eListBoxEntryMenu(&list, _(" Setting Inadyn (DynDNS)"), eString().sprintf("(%d) %s", ++entry, _("Open Inadyn panel")) ))->selected, eExtra::openInadynPanel);
#ifndef DISABLE_HDD
#ifndef DISABLE_FILE
	CONNECT((new eListBoxEntryMenu(&list, _(" Setting SWAP file"), eString().sprintf("(%d) %s", ++entry, _("Open swap file panel")) ))->selected, eExtra::openSwapPanel);
	CONNECT((new eListBoxEntryMenu(&list, _(" Setting Harddisk parameter"), eString().sprintf("(%d) %s", ++entry, _("Open Harddisk manager")) ))->selected, eExtra::hddSetup );
#endif // DISABLE_FILE
#endif // DISABLE_HDD
	CONNECT((new eListBoxEntryMenu(&list, _(" Manage User's Scripts"), eString().sprintf("(%d) %s", ++entry, _("Open user sript panel")) ))->selected, eExtra::openUserPanel);
#ifndef DISABLE_MOVEVAR
	CONNECT((new eListBoxEntryMenu(&list, _(" Manage /var folder"), eString().sprintf("(%d) %s", ++entry, _("Open manage /var panel")) ))->selected, eExtra::openMovVarPanel);
#endif // DISABLE_MOVEVAR
	new eListBoxEntryMenuSeparator(&list, eSkin::getActive()->queryImage("listbox.separator"), 0, true );
	CONNECT((new eListBoxEntryMenu(&list, _(" EPG Control Center"), eString().sprintf("(%d) %s", ++entry, _("Open EPG control center panel")) ))->selected, eExtra::openEpgPanel);
	CONNECT((new eListBoxEntryMenu(&list, _(" Firewall Manager"), eString().sprintf("(%d) %s", ++entry, _("Open Firewall panel")) ))->selected, eExtra::openFirewallPanel);
}

eExtra::~eExtra()
{
}

void eExtra::openUserPanel()
{
	hide();	
	eScriptWindow dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eExtra::openEpgPanel()
{
	hide();	
	eEpgManager dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

#ifndef DISABLE_MOVEVAR
void eExtra::openMovVarPanel()
{
	hide();	
	eMovvar dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}
#endif // DISABLE_MOVEVAR

void eExtra::openFirewallPanel()
{
	hide();	
	eSetIptable dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eExtra::openInadynPanel()
{
	hide();	
	eInadyn::getInstance()->init();
	show();
}

#ifndef DISABLE_HDD
#ifndef DISABLE_FILE
void eExtra::openSwapPanel()
{
	hide();	
	eSwap dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eExtra::hddSetup()
{
	hide();
	eHddSetup setup;
	if (!setup.getNr())
	{
		eMessageBox msg(_("sorry, no harddisks found!"), _("Harddisk setup..."));
		msg.show();
		msg.exec();
		msg.hide();
	} else
	{
		setup.show();
		setup.exec();
		setup.hide();
	}
	show();
}
#endif // DISABLE_FILE
#endif // DISABLE_HDD
