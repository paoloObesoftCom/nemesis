#ifndef DISABLE_MOVEVAR
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
#include "movvar.h"
#include "nemtool.h"

eString eMovvar::getVarDir()
{
	char buf[256];
	eString varDir;
	FILE *in = fopen("/var/etc/.vardir", "r");
	if(in)
	{	fgets(buf, 256, in);
		varDir=eString().sprintf("%s", buf);
		varDir=varDir.left(varDir.length() - 1);		
		fclose(in);
		return varDir;
	} else
	{	varDir="None";
		return  varDir;
	}
}

void eMovvar::setVarDir(eString movDir)
{
	eString cmd;
	cmd = eString().sprintf("echo %s/Nemesis/var > /var/etc/.vardir",movDir.c_str());
	system(cmd.c_str());
}

void eMovvar::loadPartList()
{

	movList->beginAtomic(); 
	movList->clearList();
	if (!getVarStatus())
	{	eString movDir = getVarDir();
		if (strstr(movDir.c_str(),"/hdd/Nemesis/var"))
			new eListBoxEntryText(movList, _(" Restore /var from HDD to flash"), (void *)3,0,_("Restore /var to flash"));
		else if (strstr(movDir.c_str(),"/cf/Nemesis/var"))
			new eListBoxEntryText(movList, _(" Restore /var from CF to flash"), (void *)3,0,_("Restore /var to flash"));
		else if (strstr(movDir.c_str(),"/usb/Nemesis/var"))
			new eListBoxEntryText(movList, _(" Restore /var from USB to flash"), (void *)3,0,_("Restore /var to flash"));
	} else
	{	if (HDD) new eListBoxEntryText(movList, _(" Move /var on HD drive"), (void *)0, 0,_("Move /var to HDD"));
		if (CF) new eListBoxEntryText(movList, _(" Move /var on CF card"), (void *)1, 0,_("Move /var to CF"));
		if (USB) new eListBoxEntryText(movList, _(" Move /var on USB stick"), (void *)2, 0,_("Move /var to USB"));
	}
	movList->endAtomic();
}

bool eMovvar::getVarStatus()
{
	bool isLocal = false;
	char *fileName = "/proc/mounts";
	eString ePar;
	char buf[256];
	FILE *in = fopen(fileName, "rt");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			ePar = eString().sprintf("%s", buf);
			ePar = ePar.left(ePar.length() - 1);		
			if (strstr(ePar.c_str(), "/dev/mtdblock/1")) isLocal = true;
		}
		fclose(in);
	}
	return isLocal;
}


bool eMovvar::readPartitions()
{
	HDD = false; CF = false; USB = false;
	char *fileName = "/proc/mounts";
	eString ePar;
	char buf[256];
	FILE *in = fopen(fileName, "rt");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			ePar = eString().sprintf("%s", buf);
			ePar = ePar.left(ePar.length() - 1);		
			if (strstr(ePar.c_str(), " /hdd")) HDD = true;
			if (strstr(ePar.c_str(), " /cf"))  CF = true;
			if (strstr(ePar.c_str(), " /usb")) USB = true;
		}
		fclose(in);
	}
	return (HDD || CF || USB);
}

eMovvar::eMovvar()
{
	int sW = 720;
	int sH = 576;
	int winW = 450;
	int winH = 235;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Manage /var folder"));
	status = new eLabel(this);
	status->move(ePoint(5, 10));
	status->resize(eSize(winW - 10, 30));
	status->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	movList = new eListBox<eListBoxEntryText>(this);
	movList->move(ePoint(5, 40));
	movList->resize(eSize(clientrect.width() - 10 ,90));
	movList->setFlags(eListBoxBase::flagNoPageMovement);
	movList->loadDeco();
	bool notEmpty = readPartitions();
	if (notEmpty)
	{
		loadPartList();
		getStatus();
		CONNECT(movList->selected, eMovvar::movSelected);
		cpVar = new eCheckbox(this,1,1);
		cpVar->setText(_("Copy /var data to new folder"));
		cpVar->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		cpVar->move(ePoint(5, 135));
		cpVar->resize(eSize(winW - 10,30));
		cpVar->setHelpText(_("Enable copy /var data"));
		if (!getVarStatus()) cpVar->hide();
	} else
	{	movList->hide();
		label = new eLabel(this);
		label->setFlags(RS_WRAP);
		label->setText(_("No External device Found!"));
		label->move(ePoint(5, 5));
		label->resize(eSize(clientrect.width()-10, clientrect.height()-75));
	}

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint((winW / 2) - 50, clientrect.height()-65));
	bt_exit->resize(eSize(100, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setText(_("Close"));
	bt_exit->setHelpText(_("Close"));
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	CONNECT(bt_exit->selected, eWidget::reject);
	sbar = new eStatusBar(this);
	sbar->move(ePoint(0, clientrect.height() - 30) );
	sbar->resize(eSize(clientrect.width(), 30));
	sbar->loadDeco();	
	if (notEmpty)
		setFocus(movList);
}

eMovvar::~eMovvar()
{
}

void eMovvar::movSelected(eListBoxEntryText *item)
{
	if (item)
	{	int key = (int)item->getKey();
		eString movDir;
		if (key == 0) movDir = "/hdd";
		if (key == 1) movDir = "/cf";
		if (key == 2) movDir = "/usb";

		if (getVarStatus())
			moveVar(movDir);
		else
			removeVar();

		loadPartList();
		getStatus();
	}else
		close(0);
}

void eMovvar::moveVar(eString movDir)
{
	eString eDirn = movDir + "/Nemesis/var";

	bool eTux = nemTool::getInstance()->existsdir((char*)eString(movDir + "/Nemesis/var/tuxbox").c_str(),false);

	eMessageBox msg(eString().sprintf(_("Do you want move /var on %s?"),eDirn.c_str()), _("Moving /var folder"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
	msg.show(); int button=msg.exec(); msg.hide();
	if (button == eMessageBox::btYes)
	{
		if (eTux)
		{	eMessageBox msg(eString().sprintf(_("The folder %s is not empty!\nDo you want delete it before move /var?"),eDirn.c_str()).c_str(), _("Folder not empty"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
			msg.show(); int button=msg.exec(); msg.hide();
			if (button == eMessageBox::btYes)
			{	status->setText(_("Deleting folder.."));
				system(eString().sprintf("rm -rf %s/*",eDirn.c_str()).c_str());
				eTux = false;
				usleep(500000);
			}
		}else
		{	status->setText(_("Creating new /var folder.."));
			system(eString().sprintf("mkdir %s/Nemesis",movDir.c_str()).c_str());
			system(eString().sprintf("mkdir %s",eDirn.c_str()).c_str());
			usleep(500000);
		}

		if (nemTool::getInstance()->existsdir((char*)eDirn.c_str(),false))
		{	status->setText(_("Copy /var to new folder.."));
			system(eString().sprintf("echo %s > /var/etc/.vardir",eDirn.c_str()).c_str());
			if (!eTux)
			{	system(eString().sprintf("cp -rPf /var/* %s/",eDirn.c_str()).c_str());
			}else
			{	
				if (cpVar->isChecked())
				{	eMessageBox msg1(eString().sprintf(_("The 'Copy' option is enable.\nDo you want copy /var folder data to %s/ folder?"),eDirn.c_str()).c_str(), _("Copy data"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
					msg1.show(); int button1=msg1.exec(); msg1.hide();
					if (button1 == eMessageBox::btYes)
						system(eString().sprintf("cp -rPf /var/* %s/",eDirn.c_str()).c_str());
				}
			}
			system(eString().sprintf("echo %s > %s/etc/.vardir",eDirn.c_str(),eDirn.c_str()).c_str());
			usleep(500000);
			status->setText("Reboot dreambox..");
			usleep(500000);
			eConfig::getInstance()->setKey("/extras/wasshutdown", 1 );
			eZap::getInstance()->quit(4);
		}else
			nemTool::getInstance()->msgfail(eString().sprintf(_("Cannot create %s folder!"),eDirn.c_str()));
	}
}

void eMovvar::removeVar()
{
	eMessageBox msg(_("Do you want restore /var folder in flash?"), _("Restoring /var folder"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
	msg.show(); 
	int button=msg.exec(); 
	msg.hide();
	if (button == eMessageBox::btYes)
	{	status->setText(_("Restore /var.."));
		unlink("/var/etc/.vardir");
		system("mkdir /var/mnt/nem_temp");
		system("mount -t jffs2 /dev/mtdblock/1 /var/mnt/nem_temp");
		unlink("/var/mnt/nem_temp/etc/.vardir");
		system("umount /var/mnt/nem_temp");
		system("rmdir /var/mnt/nem_temp");
		usleep(500000);
		status->setText("Reboot dreambox..");
		usleep(500000);
		eConfig::getInstance()->setKey("/extras/wasshutdown", 1 );
		eZap::getInstance()->quit(4);
	}
}

void eMovvar::setStatus(const eString &string)
{
	status->setText(string);
}

void eMovvar::getStatus()
{
	if (getVarStatus())
		status->setText(_("/var folder is in flash!"));
	else
	{
		eString eStatus = getVarDir();
		status->setText(eString().sprintf(_("/var folder is moved in: %s"),eStatus.c_str()));
	}
	
}
#endif // DISABLE_MOVEVAR
