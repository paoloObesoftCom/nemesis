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
#include "utility.h"
#include "nemtool.h"
#include "epgmanager.h"
#include "autostandby.h"
#include "setpath.h"
#include "setsyslog.h"
#include "setosdpos.h"
#include <enigma_main.h>

eUtility::eUtility()
	:eSetupWindow(_("Utility"), 10, 400)
{
	int sW = 720;
	int sH = 576;
	int winW = 400;
	int winH = clientrect.height() ;

	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));

	int entry=0;
	CONNECT((new eListBoxEntryMenu(&list, _(" Setting skin position"), eString().sprintf("(%d) %s", ++entry, _("Setting OSD skin position")) ))->selected, eUtility::openSetOsdPanel);
	CONNECT((new eListBoxEntryMenu(&list, _(" Clear zapping playlist"), eString().sprintf("(%d) %s", ++entry, _("Clear channel history playlist")) ))->selected, eUtility::cleanPlaylist );
	CONNECT((new eListBoxEntryMenu(&list, _(" AutoStandby Manager"), eString().sprintf("(%d) %s", ++entry, _("Open AutoStandby panel")) ))->selected, eUtility::openStbyPanel);
	CONNECT((new eListBoxEntryMenu(&list, _(" Setting path"), eString().sprintf("(%d) %s", ++entry, _("Open Setting path panel")) ))->selected, eUtility::openSetPathPanel);
	CONNECT((new eListBoxEntryMenu(&list, _(" Debugging Manager"), eString().sprintf("(%d) %s", ++entry, _("Open Debugging panel")) ))->selected, eUtility::openDebugPanel);
	new eListBoxEntryMenuSeparator(&list, eSkin::getActive()->queryImage("listbox.separator"), 0, true );
	CONNECT((new eListBoxEntryMenu(&list, _(" Save dreambox conf"), eString().sprintf("(%d) %s", ++entry, _("Save configuration")) ))->selected, eUtility::saveDreamParameter );
}

eUtility::~eUtility()
{
}

void eUtility::openStbyPanel()
{
	hide();	
	eAutoStandbySelWindow dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eUtility::openDebugPanel()
{
	hide();	
	eSetSyslog dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eUtility::openSetOsdPanel()
{
	hide();	
	eOsdPosition dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eUtility::openSetPathPanel()
{
	hide();	
	eSetpath dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eUtility::cleanPlaylist()
{
	eMessageBox msg(_("Do you want delete the playlist.epl file too?"), 
					_("Clear playlist"), eMessageBox::btYes|eMessageBox::btNo|eMessageBox::iconQuestion, eMessageBox::btNo );
	msg.show(); int button=msg.exec(); msg.hide();
	eMessageBox box(_("Cleaning in progress.\nPlease Wait..."), _("Running.."), 0);
	box.show();
		if (button == eMessageBox::btYes)
			eZapMain::getInstance()->cleanPlaylist(true);
		else
			eZapMain::getInstance()->cleanPlaylist(false);
		usleep(500000);
	box.hide();	
}

void eUtility::saveDreamParameter()
{
	eMessageBox box(_("Save in progress.\nPlease Wait..."), _("Running.."), 0);
	box.show();
		eConfig::getInstance()->flush();
		usleep(500000);
	box.hide();	
}
