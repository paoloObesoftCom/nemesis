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
#include "deamon.h"
#include "nemtool.h"


eString eDeamon::readDeamonName(eString name)
{
	char buf[256];
	eString deamonName;
	FILE *in = fopen(eString("/var/etc/" + name).c_str(), "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			deamonName = eString().sprintf("%s", buf);
			deamonName = deamonName.left(deamonName.length() - 1);		
			if (deamonName.left(5) == "name=")
			{	
				fclose(in);
				deamonName = deamonName.right(deamonName.length() - 5);
				deamonName = nemTool::getInstance()->replace(deamonName, "'", "");
				return deamonName;
			}
		}
		fclose(in);
	}
	return name;
}


eString eDeamon::readDeamonBin(eString bin)
{
	char buf[256];
	eString binName;
	FILE *in = fopen(eString("/var/etc/" + bin).c_str(), "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			binName = eString().sprintf("%s", buf);
			binName = binName.left(binName.length() - 1);		
			if (binName.left(11) == "binaryname=")
			{	
				fclose(in);
				binName = binName.right(binName.length() - 11);
				binName = nemTool::getInstance()->replace(binName, "'", "");
				return binName;
			}
		}
		fclose(in);
	}
	return "None";
}

eString eDeamon::readDeamonPath(eString path)
{
	char buf[256];
	eString binPath;
	FILE *in = fopen(eString("/var/etc/" + path).c_str(), "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			binPath = eString().sprintf("%s", buf);
			binPath = binPath.left(binPath.length() - 1);		
			if (binPath.left(11) == "binarypath=")
			{	
				fclose(in);
				binPath = binPath.right(binPath.length() - 11);
				binPath = nemTool::getInstance()->replace(binPath, "'", "");
				return binPath;
			}
		}
		fclose(in);
	}
	return "None";
}

eDeamon::eDeamon()
	:eSetupWindow(_("Start - Stop programs"), 12, 400)
{
	int sW = 720;
	int sH = 576;
	int winW = 400;
	int winH = clientrect.height() ;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));

	DIR *d = opendir("/var/etc");
	int entry=0;

	deamonItem.clear();
	DeamonList thisDeamon;
	system("chmod 755 /var/etc/*_startup.sh");
	while (struct dirent *e=readdir(d))
	{
		if ( strstr(e->d_name,"_startup.sh")) 
		{
			memset(deamon, 0, sizeof(deamon));
			strncpy(deamon, e->d_name, strlen(e->d_name));
			eString scriptName = eString().sprintf("%s", deamon);
			eString binName = readDeamonBin(scriptName);
			eString fullbinName = readDeamonPath(scriptName);
			if ( binName != "None" && fullbinName !="None" )
			{
				fullbinName += binName;
				if (nemTool::getInstance()->exists((char*)fullbinName.c_str(),false))
				{	eString descName = readDeamonName(scriptName);
					const char* deamonName = descName.c_str();
					const char* deamonBinary = binName.c_str();
					eString listHelpText = eString().sprintf(_("(%d) Start/Stop  %s"), ++entry, deamonName);
					char listKey[100];
					strcpy(listKey,"/Nemesis-Project/tools/");
					strcat(listKey,deamonBinary);
					thisDeamon.id = entry;
					thisDeamon.command = deamon;
					thisDeamon.binaName = binName;
					thisDeamon.fullBinaName = fullbinName;
					deamonItem.push_back(thisDeamon);
					CONNECT_2_1((new eListBoxEntryCheck(&list, eString().sprintf(_("Enable %s"),deamonName).c_str(),listKey ,listHelpText ))->selected, eDeamon::startStop,entry);
				}
			}
		}
	}
	closedir(d);
}

eDeamon::~eDeamon()
{
}


void eDeamon::startStop(bool newState, int key)
{
	eString scriptName = "";
	eString disableName = "/var/etc/.dont_start_";
	eString fullBinName = "";
	std::list<DeamonList>::iterator x;
	for(x = deamonItem.begin(); x != deamonItem.end(); ++x)
	{
		if (x->id == key) 
		{	
			scriptName = eString(x->command).c_str();
			fullBinName =  x->fullBinaName;
			disableName += x->binaName;
		}
	}
	if (newState)
	{	
	eMessageBox box(eString().sprintf(_("Starting program: %s.\nPlease Wait..."),fullBinName.c_str()), _("Running.."), 0);
	box.show();
		::unlink(disableName.c_str());
		nemTool::getInstance()->sendCmd((char *) eString("/var/etc/" + scriptName + " start").c_str());
		usleep(500000);
	box.hide();	
	}
	else
	{
	eMessageBox box1(eString().sprintf(_("Stopping program: %s.\nPlease Wait..."),fullBinName.c_str()), _("Running.."), 0);
	box1.show();
		nemTool::getInstance()->sendCmd((char *) eString("/var/etc/" + scriptName + " stop").c_str());
		eString cmd = "touch ";
		cmd += disableName.c_str();
		system(cmd.c_str());
		usleep(500000);
	box1.hide();	
	}
	eConfig::getInstance()->flush();
}
