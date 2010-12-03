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
#include "sysinfo.h"
#include "showfile.h"

bool eSysInfo::loadScripts()
{
	XMLTreeParser * parser;

	parser = new XMLTreeParser("ISO-8859-1");
	char buf[2048];

	FILE *in = fopen("/var/etc/script.xml", "rt");

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
		if ( strcmp( root->GetType(), "scripts") ) 
		{	eMessageBox msg(_("Invalid configfile"), _("User Abort"), eMessageBox::iconWarning|eMessageBox::btOK);
			msg.show();     msg.exec();     msg.hide();
			return false;
		}
		
		scriptsList.clear();
		ScriptsList thisUrl;
	
		int idcnt = 0;
	
		for(XMLTreeNode * node = root->GetChild(); node; node = node->GetNext())
		{	if(!strcmp(node->GetType(), "script"))
			{	for(XMLTreeNode * p = node->GetChild(); p; p = p->GetNext())
				{	if(!strcmp(p->GetType(), "name")) thisUrl.name = p->GetData();
					if(!strcmp(p->GetType(), "cmd")) thisUrl.cmd = p->GetData();
					if(!strcmp(p->GetType(), "info")) thisUrl.info = p->GetData();
				}
				thisUrl.id = idcnt++;
				scriptsList.push_back(thisUrl);
			}
		}
		delete parser;
		return true;
	}
	return false;
}

eSysInfo::eSysInfo()
	:eSetupWindow(_("System information"), 14, 350)
{
	int sW = 720;
	int sH = 576;
	int winW = 350;
	int winH = clientrect.height() ;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	loadScripts();
	int entry=0;
	std::list<ScriptsList>::iterator x;
	for(x = scriptsList.begin(); x != scriptsList.end(); ++x)
		CONNECT_1_0((new eListBoxEntryMenu(&list, x->name.c_str(), eString().sprintf("(%d) %s", ++entry, x->info.c_str()) ))->selected, eSysInfo::scriptExec,x->id);
}

eSysInfo::~eSysInfo()
{
}

void eSysInfo::scriptExec(int key)
{
	char *ouputFile = "/tmp/sysinfo.out";
	std::list<ScriptsList>::iterator i;
	for(i = scriptsList.begin(); i != scriptsList.end(); ++i)
	{
		if (i->id == key)
		{ 
			eMessageBox box(_("Information is reading.\nPlease wait a moment.."), _("Running.."), 0);
			box.show();
				sleep(1);
				eString cmd;
				cmd.sprintf("%s > %s",i->cmd.c_str(),ouputFile);
				system(cmd.c_str());
			box.hide();
			hide();
				cbpShowFile::getInstance()->showLog(i->name, ouputFile ,false);
			show();
		}
	}
}
