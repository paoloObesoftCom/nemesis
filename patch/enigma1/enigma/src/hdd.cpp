#ifndef DISABLE_HDD
#ifndef DISABLE_FILE
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
#include "hdd.h"
#include "nemtool.h"

static int nemGetCapacity(int dev)
{
	int c='a'+dev;
	
	FILE *f=fopen(eString().sprintf("/proc/ide/hd%c/capacity", c).c_str(), "r");
	if (!f)
		return -1;
	int capacity=-1;
	fscanf(f, "%d", &capacity);
	fclose(f);
	return capacity;
}

static eString nemGetModel(int dev)
{
	int c='a'+dev;
	char line[1024];

	FILE *f=fopen(eString().sprintf("/proc/ide/hd%c/model", c).c_str(), "r");
	if (!f)
		return "";
	*line=0;
	fgets(line, 1024, f);
	fclose(f);
	if (!*line)
		return "";
	line[strlen(line)-1]=0;
	return line;
}

int nemFreeDiskspace(int dev, eString mp="")
{

	eDebug("Dev: %d", dev);
	FILE *f=fopen("/proc/mounts", "rb");
	if (!f)
		return -1;
	eString path;
	int host=dev/4;
	int bus=!!(dev&2);
	int target=!!(dev&1);
	path.sprintf("/dev/ide/host%d/bus%d/target%d/lun0/", host, bus, target);

	eDebug("Path: %s", path.c_str());

	while (1)
	{
		char line[1024];
		if (!fgets(line, 1024, f))
			break;
		if (!strncmp(line, path.c_str(), path.size()))
		{
			eString mountpoint=line;
			mountpoint=mountpoint.mid(mountpoint.find(' ')+1);
			mountpoint=mountpoint.left(mountpoint.find(' '));
			eDebug("Mountpoint: %s", mountpoint.c_str());
			if ( mp && mountpoint != mp )
				return -1;
			struct statfs s;
			int free;
			if (statfs(mountpoint.c_str(), &s)<0)
				free=-1;
			else
				free=s.f_bfree/1000*s.f_bsize/1000;
			fclose(f);
			return free;
		}
	}
	fclose(f);
	return -1;
}

static int nemNumPartitions(int dev)
{
	eString path;
	int host=dev/4;
	int bus=!!(dev&2);
	int target=!!(dev&1);

	path.sprintf("ls /dev/ide/host%d/bus%d/target%d/lun0/ > /tmp/tmp.out", host, bus, target);
	system( path.c_str() );

	FILE *f=fopen("/tmp/tmp.out", "rb");
	if (!f)
	{
		eDebug("fopen failed");
		return -1;
	}

	int numpart=-1;		// account for "disc"
	while (1)
	{
		char line[1024];
		if (!fgets(line, 1024, f))
			break;
		if ( !strncmp(line, "disc", 4) )
			numpart++;
		if ( !strncmp(line, "part", 4) )
			numpart++;
	}
	fclose(f);
	system("rm /tmp/tmp.out");
	return numpart;
}

eHddSetup::eHddSetup()
: eListBoxWindow<eListBoxEntryText>(_("Harddisk Setup"), 5, 420)
{
	nr=0;
	
	move(ePoint(150, 136));
	
	for (int host=0; host<2; host++)
		for (int bus=0; bus<2; bus++)
			for (int target=0; target<2; target++)
			{
				int num=target+bus*2+host*4;
				
				int c='a'+num;
				
							// check for presence
				char line[1024];
				int ok=1;
				FILE *f=fopen(eString().sprintf("/proc/ide/hd%c/media", c).c_str(), "r");
				if (!f)
					continue;
				if ((!fgets(line, 1024, f)) || strcmp(line, "disk\n"))
					ok=0;
				fclose(f);

				if (ok)
				{
					int capacity=nemGetCapacity(num);
					if (capacity < 0)
						continue;
						
					capacity=capacity/1000*512/1000;

					eString sharddisks;
					sharddisks=nemGetModel(num);
					sharddisks+=" (";
					if (c&1)
						sharddisks+="master";
					else
						sharddisks+="slave";
					if (capacity)
						sharddisks+=eString().sprintf(", %d.%03d GB", capacity/1024, capacity%1024);
					sharddisks+=")";
					
					nr++;
					
					new eListBoxEntryText(&list, sharddisks, (void*)num);
				}
	}
	
	CONNECT(list.selected, eHddSetup::selectedHarddisk);
}

void eHddSetup::selectedHarddisk(eListBoxEntryText *t)
{
	if ((!t) || (((int)t->getKey())==-1))
	{
		close(0);
		return;
	}
	int dev=(int)t->getKey();
	
	eHddMenu menu(dev);
	
	hide();
	menu.show();
	menu.exec();
	menu.hide();
	show();
}

void eHddMenu::readStatus()
{
	if (!(dev & 1))
		bus->setText("master");
	else
		bus->setText("slave");

	eString mod=nemGetModel(dev);
	setText(mod);
	model->setText(mod);
	int cap=nemGetCapacity(dev)/1000*512/1000;
	
	if (cap != -1)
		capacity->setText(eString().sprintf("%d.%03d GB", cap/1024, cap%1024));
		
	numpart=nemNumPartitions(dev);
	int fds;
	
	if (numpart == -1)
		status->setText(_("(error reading information)"));
	else if (!numpart)
		status->setText(_("uninitialized - format it to use!"));
	else if ((fds=nemFreeDiskspace(dev)) != -1)
		status->setText(eString().sprintf(_("in use, %d.%03d GB (~%d minutes) free"), fds/1024, fds%1024, fds/33 ));
	else
		status->setText(_("initialized, but unknown filesystem"));

	int hddTimeout = 120;
	int timeToSet = 120;
	eConfig::getInstance()->getKey("/extras/hdparm-s", hddTimeout);
	if (hddTimeout <= 240) 
		timeToSet = hddTimeout / 12;
	else if (hddTimeout == 252) 
		timeToSet = 21;
	else if (hddTimeout == 241) 
		timeToSet = 30;
	else if (hddTimeout == 242) 
		timeToSet = 60;
	sleepTime->setText(eString().sprintf("%i",timeToSet).c_str());

	int hddSilent = 128;
	eConfig::getInstance()->getKey("/extras/hdparm-m", hddSilent);
	silent->setText(eString().sprintf("%i",hddSilent).c_str());

}

eHddMenu::eHddMenu(int dev): dev(dev)
{
	int sW = 720;
	int sH = 576;
	int winW = 504;
	int winH = 290 ;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));

	label[0] = new eLabel(this);
	label[0]->move(ePoint(5, 5));	label[0]->resize(eSize(100, 30));
	label[0]->setText(_("Status:"));	label[0]->setAlign(1);
	status = new eLabel(this);
	status->move(ePoint(110, 5));
	status->resize(eSize(385, 30));

	label[1] = new eLabel(this);
	label[1]->move(ePoint(5, 40));	label[1]->resize(eSize(100, 30));
	label[1]->setText(_("Model:"));	label[1]->setAlign(1);
	model = new eLabel(this);
	model->move(ePoint(110, 40));
	model->resize(eSize(385, 30));

	label[2] = new eLabel(this);
	label[2]->move(ePoint(5, 75));	label[2]->resize(eSize(100, 30));
	label[2]->setText(_("Capacity:"));	label[2]->setAlign(1);
	capacity = new eLabel(this);
	capacity->move(ePoint(110, 75));
	capacity->resize(eSize(385, 30));
	
	label[3] = new eLabel(this);
	label[3]->move(ePoint(5, 110));	label[3]->resize(eSize(100, 30));
	label[3]->setText(_("Bus type:"));	label[3]->setAlign(1);
	bus = new eLabel(this);
	bus->move(ePoint(110, 110));
	bus->resize(eSize(385, 30));

	label[4] = new eLabel(this);
	label[4]->move(ePoint(5, 145));		label[4]->resize(eSize(200, 30));
	label[4]->setText(_("Sleep timeout:"));	label[4]->setAlign(1);

	sleepTime=new eTextInputField(this);
	sleepTime->move(ePoint(210, 145));
	sleepTime->resize(eSize(60, 30));
	sleepTime->setUseableChars("1234567890");
	sleepTime->setMaxChars(2);
	sleepTime->setHelpText(_("Press OK to enter setup timeout"));
	sleepTime->setEditHelpText(_("Set timeout 0-60 min"));
	sleepTime->loadDeco();

	label[5] = new eLabel(this);
	label[5]->move(ePoint(5, 180));		label[5]->resize(eSize(200, 30));
	label[5]->setText(_("Acoustics:"));	label[5]->setAlign(1);

	silent=new eTextInputField(this);
	silent->move(ePoint(210, 180));
	silent->resize(eSize(60, 30));
	silent->setUseableChars("1234567890");
	silent->setMaxChars(3);
	silent->setHelpText(_("Press OK to enter setup acoustics"));
	silent->setEditHelpText(_("Set acoustics 0-254, 128: quiet, 254: fast"));
	silent->loadDeco();

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);
	int btnsize = (winW -4) / 4;

	bt_abort=new eButton(this);
	bt_abort->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_abort->move(ePoint(2 , clientrect.height()-65));
	bt_abort->resize(eSize(btnsize, 30));
	bt_abort->setShortcut("red");
	bt_abort->setShortcutPixmap("red");
	bt_abort->setHelpText(_("Close Harddisk panel"));
	bt_abort->setText(_("Close"));
	bt_abort->setAlign(0);
	if (showBorder) bt_abort->loadDeco();

	bt_set=new eButton(this);
	bt_set->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_set->move(ePoint(btnsize  + 2, clientrect.height()-65));
	bt_set->resize(eSize(btnsize, 30));
	bt_set->setShortcut("green");
	bt_set->setShortcutPixmap("green");
	bt_set->setHelpText(_("Save and set Harddisk parameter"));
	bt_set->setText(_("Set"));
	bt_set->setAlign(0);
	if (showBorder) bt_set->loadDeco();

	bt_start=new eButton(this);
	bt_start->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_start->move(ePoint(btnsize * 2  + 2, clientrect.height()-65));
	bt_start->resize(eSize(btnsize, 30));
	bt_start->setShortcut("yellow");
	bt_start->setShortcutPixmap("yellow");
	bt_start->setHelpText(_("Start Harddisk"));
	bt_start->setText(_("Start HDD"));
	bt_start->setAlign(0);
	if (showBorder) bt_start->loadDeco();

	bt_standby=new eButton(this);
	bt_standby->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_standby->move(ePoint(btnsize * 3  + 2, clientrect.height()-65));
	bt_standby->resize(eSize(btnsize, 30));
	bt_standby->setShortcut("blue");
	bt_standby->setShortcutPixmap("blue");
	bt_standby->setHelpText(_("Stop Harddisk"));
	bt_standby->setText(_("Stop HDD"));
	bt_standby->setAlign(0);
	if (showBorder) bt_standby->loadDeco();

	readStatus();

	CONNECT(bt_set->selected, eHddMenu::setParam);
	CONNECT(bt_start->selected, eHddMenu::startHdd);
	CONNECT(bt_standby->selected, eHddMenu::setStandby);
	CONNECT(bt_abort->selected, eWidget::reject);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, clientrect.height() - 30) );
	sbar->resize(eSize(clientrect.width(), 30));
	sbar->loadDeco();	

}

void eHddMenu::setParam()
{
	if(sleepTime->getText() && silent->getText())
	{ 
		int host=dev/4;
		int bus=!!(dev&2);
		int target=!!(dev&1);

		int hddTimeout = atoi(sleepTime->getText().c_str());
		int hddSilent = atoi(silent->getText().c_str());
		int timeToSet = 0;
		eString cmd, cmd1;
		if (hddTimeout >= 0 && hddTimeout <= 60) 
		{	if (hddTimeout >= 0 && hddTimeout <= 20) timeToSet = hddTimeout * 12;
			if (hddTimeout == 21) timeToSet = 252;
			if (hddTimeout > 21) timeToSet = 241;
			if (hddTimeout == 60) timeToSet = 242;
			cmd = eString().sprintf("/sbin/hdparm -S %i /dev/ide/host%d/bus%d/target%d/lun0/disc",timeToSet, host, bus, target).c_str();
		} else 
		{	nemTool::getInstance()->msgfail(_("Bad timeout parameter!"));
			return;
		}
		if (hddSilent >= 0 && hddSilent <= 254) 
		{	
			cmd1 = eString().sprintf("/sbin/hdparm -M %i /dev/ide/host%d/bus%d/target%d/lun0/disc",hddSilent, host, bus, target).c_str();
		} else 
		{	nemTool::getInstance()->msgfail(_("Bad acoustics parameter!"));
			return;
		}
		eMessageBox box(_("Setting HD parameter.\nPlease Wait..."), _("Running.."), 0);
		box.show();
			nemTool::getInstance()->sendCmd((char *) cmd.c_str());
			eConfig::getInstance()->setKey("/extras/hdparm-m", hddSilent);
			nemTool::getInstance()->sendCmd((char *) cmd1.c_str());
			system(eString().sprintf("echo  %i > /var/etc/.acoustic.hd",hddSilent).c_str());
			eConfig::getInstance()->setKey("/extras/hdparm-s", timeToSet);
			system(eString().sprintf("echo  %i > /var/etc/.timeout.hd",timeToSet).c_str());
			eConfig::getInstance()->flush();
			usleep(500000);
		box.hide();
	}else 
	{
		if (!sleepTime->getText()) { nemTool::getInstance()->msgfail(_("Set sleep timeout before!")); setFocus(sleepTime); return;}
		if (!silent->getText()) { nemTool::getInstance()->msgfail(_("Set acoustic before!")); setFocus(silent); return;}
	}

}

void eHddMenu::startHdd()
{
	int host=dev/4;
	int bus=!!(dev&2);
	int target=!!(dev&1);
	eMessageBox box(_("Start HDD.\nPlease Wait..."), _("Running.."), 0);
	box.show();
		eString cmd = eString().sprintf("/sbin/hdparm -S 0 /dev/ide/host%d/bus%d/target%d/lun0/disc", host, bus, target).c_str();
		nemTool::getInstance()->sendCmd((char *) cmd.c_str());
		usleep(500000);
	box.hide();
}

void eHddMenu::setStandby()
{
	int host=dev/4;
	int bus=!!(dev&2);
	int target=!!(dev&1);
	eMessageBox box(_("Stop HDD.\nPlease Wait..."), _("Running.."), 0);
	box.show();
		eString cmd = eString().sprintf("/bin/sync", host, bus, target).c_str();
		nemTool::getInstance()->sendCmd((char *) cmd.c_str());
		cmd = eString().sprintf("/sbin/hdparm -y /dev/ide/host%d/bus%d/target%d/lun0/disc", host, bus, target).c_str();
		nemTool::getInstance()->sendCmd((char *) cmd.c_str());
		usleep(500000);
	box.hide();
}

#endif // DISABLE_FILE
#endif // DISABLE_HDD
