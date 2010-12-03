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
#include "emuinfo.h"
#include "showfile.h"
#include <enigma_plugins.h>
#include <nemtool.h>

bool eEmuInfo::loadScripts()
{
	XMLTreeParser * parser;

	parser = new XMLTreeParser("ISO-8859-1");
	char buf[2048];

	FILE *in = fopen("/var/etc/emuscript.xml", "r");

	if (in)
	{	int done;
		do 
		{	unsigned int len=fread(buf, 1, sizeof(buf), in);
			done = ( len < sizeof(buf) );
			if ( ! parser->Parse( buf, len, done ) ) 
			{	eMessageBox msg(_("Configfile parse error"), _("User Abort"), eMessageBox::iconWarning|eMessageBox::btOK);
				msg.show();     msg.exec();     msg.hide();
				delete parser;
				parser = NULL;
				return false;
			}
		} 
		while (!done);
	
		fclose(in);
	
		XMLTreeNode * root = parser->RootNode();
		if(!root)
		{	eMessageBox msg(_("Configfile parse error"), _("User Abort"), eMessageBox::iconWarning|eMessageBox::btOK);
			msg.show();     msg.exec();     msg.hide();	
			return false;
		}
		if ( strcmp( root->GetType(), "emuscripts") ) 
		{	eMessageBox msg(_("Invalid configfile"), _("User Abort"), eMessageBox::iconWarning|eMessageBox::btOK);
			msg.show();     msg.exec();     msg.hide();
			return false;
		}
		
		emuScriptsList.clear();
		EmuScriptsList thisUrl;
	
		int idcnt = 0;
	
		for(XMLTreeNode * node = root->GetChild(); node; node = node->GetNext())
		{	if(!strcmp(node->GetType(), "script"))
			{	for(XMLTreeNode * p = node->GetChild(); p; p = p->GetNext())
				{	if(!strcmp(p->GetType(), "name")) thisUrl.name = p->GetData();
					if(!strcmp(p->GetType(), "cmd")) thisUrl.cmd = p->GetData();
					if(!strcmp(p->GetType(), "info")) thisUrl.info = p->GetData();
				}
				thisUrl.id = idcnt++;
				emuScriptsList.push_back(thisUrl);
			}
		}
		delete parser;
		return true;
	}
	return false;
}

eEmuInfo::eEmuInfo()
	:eSetupWindow(_("Emulator/Cardserver Information"), 14, 350)
{
	int sW = 720;
	int sH = 576;
	int winW = 350;
	int winH = clientrect.height() ;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));

	loadScripts();
	eString plugName;
	int entry=0;
	eZapPlugins plugins(eZapPlugins::StandardPlugin);

	if ( plugins.execPluginByName("CCcaminfo.cfg",true) == "OK"
		|| plugins.execPluginByName("_CCcaminfo.cfg",true) == "OK" )
		{	plugName = nemTool::getInstance()->getPlugName("/var/tuxbox/plugins/CCcaminfo.cfg");
			if (plugName == "None") plugName = "CCCam Info";
			CONNECT_1_0((new eListBoxEntryMenu(&list, plugName.c_str(), eString().sprintf("(%d) %s", ++entry, _("Open CCcam Info")) ))->selected, eEmuInfo::execCommand,0);
		}

	if ( plugins.execPluginByName("egboxcenter.cfg",true) == "OK"
		|| plugins.execPluginByName("_egboxcenter.cfg",true) == "OK" )
		{	plugName = nemTool::getInstance()->getPlugName("/var/tuxbox/plugins/egboxcenter.cfg");
			if (plugName == "None") plugName = "G-Box Center";
			CONNECT_1_0((new eListBoxEntryMenu(&list, plugName.c_str(), eString().sprintf("(%d) %s", ++entry, _("Open G-Box Center")) ))->selected, eEmuInfo::execCommand,1);
		}

	if (entry > 0)
		new eListBoxEntryMenuSeparator(&list, eSkin::getActive()->queryImage("listbox.separator"), 0, true );

	std::list<EmuScriptsList>::iterator x;
	for(x = emuScriptsList.begin(); x != emuScriptsList.end(); ++x)
		CONNECT_1_0((new eListBoxEntryMenu(&list, x->name.c_str(), eString().sprintf("(%d) %s", ++entry, x->info.c_str()) ))->selected, eEmuInfo::execCommand,x->id + 2);
}

eEmuInfo::~eEmuInfo()
{
}

void eEmuInfo::execCommand(int key)
{
	eZapPlugins plugins(eZapPlugins::StandardPlugin);
	if (key == 0)
	{
		hide();
		if ( plugins.execPluginByName("CCcaminfo.cfg") != "OK" )
			plugins.execPluginByName("_CCcaminfo.cfg");
		
		show();
	}
	else if (key == 1)
	{
		hide();
		if ( plugins.execPluginByName("egboxcenter.cfg") != "OK" )
			plugins.execPluginByName("_egboxcenter.cfg");
		
		show();
	}else if (key > 1)
	{	char *ouputFile = "/tmp/emuinfo.out";
		std::list<EmuScriptsList>::iterator x;
		for(x = emuScriptsList.begin(); x != emuScriptsList.end(); ++x)
		{
			if (x->id == (key - 2))
			{ 
				eMessageBox box(_("Information is reading.\nPlease wait a moment.."), _("Running.."), 0);
				box.show();
					sleep(1);
					eString cmd;
					cmd.sprintf("%s > %s",x->cmd.c_str(),ouputFile);
					system(cmd.c_str());
				box.hide();
				hide();
					cbpShowFile::getInstance()->showLog(x->name, ouputFile ,false);
				show();
			}
		}
	}else
		close(0);
}
