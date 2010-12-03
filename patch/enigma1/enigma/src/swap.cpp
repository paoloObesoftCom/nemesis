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
#include "swap.h"
#include "nemtool.h"

void eSwap::loadPartList()
{
	swapList->beginAtomic(); 
	swapList->clearList();
	eString swapFile = getConfigSwapFile();
	if (HDD)
		if (strstr(swapFile.c_str(),"/hdd"))
			new eListBoxEntryText(swapList, eString().sprintf(_(" Disable from HD (%'i Kb)"),swapSize).c_str(), (void *)0);
		else
			new eListBoxEntryText(swapList, _(" Enable swap on HD drive"), (void *)0,0,_("Enable / Disable Swap on HDD"));
	if (CF) 
		if (strstr(swapFile.c_str(),"/cf"))
			new eListBoxEntryText(swapList, eString().sprintf(_(" Disable from CF (%'i Kb)"),swapSize).c_str(), (void *)1);
		else
			new eListBoxEntryText(swapList, _(" Enable swap on CF card"), (void *)1,0,_("Enable / Disable Swap on CF"));
	if (USB)
		if (strstr(swapFile.c_str(),"/usb"))
			new eListBoxEntryText(swapList, eString().sprintf(_(" Disable from USB (%'i Kb)"),swapSize).c_str(), (void *)2);
		else
			new eListBoxEntryText(swapList, _(" Enable swap on USB stick"), (void *)2,0,_("Enable / Disable Swap on USB"));
	swapList->endAtomic();
}



bool eSwap::readPartitions()
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
			if (strstr(ePar.c_str(), " /media/hdd")) HDD = true;
			if (strstr(ePar.c_str(), " /cf"))  CF = true;
			if (strstr(ePar.c_str(), " /media/cf"))  CF = true;
			if (strstr(ePar.c_str(), " /usb")) USB = true;
			if (strstr(ePar.c_str(), " /media/usb")) USB = true;
		}
		fclose(in);
	}
	return (HDD || CF || USB);
}

eSwap::eSwap()
{
	int sW = 720;
	int sH = 576;
	int winW = 400;
	int winH = 270;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Setting Swap file"));
	status = new eLabel(this);
	status->move(ePoint(5, 10));
	status->resize(eSize(winW - 10, 30));
	status->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	swapList = new eListBox<eListBoxEntryText>(this);
	swapList->move(ePoint(5, 40));
	swapList->resize(eSize(clientrect.width() - 10 ,120));
	swapList->setFlags(eListBoxBase::flagNoPageMovement);
	swapList->loadDeco();
	bool notEmpty = readPartitions();

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	if (notEmpty)
	{
		loadPartList();
		getStatus();
		CONNECT(swapList->selected, eSwap::swapSelected);
		labelspace = new eLabel(this);
		labelspace->move(ePoint(5, 170));
		labelspace->resize(eSize(270, 30));
		labelspace->setText(_("Select swap file size:"));
		labelspace->setAlign(1);
		labelspace->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		combo_size=new eComboBox(this, 4);
		combo_size->move(ePoint(280,165));
		combo_size->resize(eSize(115, 30));
		new eListBoxEntryText(*combo_size, _("16 Mb"), (void*)0, 0);
		new eListBoxEntryText(*combo_size, _("32 Mb"), (void*)1, 0);
		new eListBoxEntryText(*combo_size, _("64 Mb"), (void*)2, 0);
		new eListBoxEntryText(*combo_size, _("128 Mb"), (void*)3, 0);
		combo_size->setCurrent(1,true);
		CONNECT(combo_size->selchanged, eSwap::selectSize);
		combo_size->loadDeco();
		bt_delete=new eButton(this);
		bt_delete->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		bt_delete->move(ePoint((clientrect.width()-105), clientrect.height()-65));
		bt_delete->resize(eSize(100, 30));
		bt_delete->setShortcut("blue");
		bt_delete->setShortcutPixmap("blue");
		bt_delete->setText(_("Delete"));
		if (showBorder) bt_delete->loadDeco();
		CONNECT(bt_delete->selected, eSwap::deleteSwap);
	} else
	{	swapList->hide();
		label = new eLabel(this);
		label->setFlags(RS_WRAP);
   		label->setText(_("No External device Found!"));
   		label->move(ePoint(5, 5));
   		label->resize(eSize(clientrect.width()-10, clientrect.height()-85));
	}
	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(5, clientrect.height()-65));
	bt_exit->resize(eSize(100, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setText(_("Close"));
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	CONNECT(bt_exit->selected, eWidget::reject);
	sbar = new eStatusBar(this);
	sbar->move(ePoint(0, clientrect.height() - 30) );
	sbar->resize(eSize(clientrect.width(), 30));
	sbar->loadDeco();	
	if (notEmpty)
		setFocus(swapList);
}


eSwap::~eSwap()
{
}

void eSwap::selectSize(eListBoxEntryText *le)
{
	setFocus(swapList);
}

void eSwap::deleteSwap()
{
	eMessageBox msg(_("Do you want delete all SWAP device?"), _("Delete SWAP device"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
	msg.show(); 
	int button=msg.exec(); 
	msg.hide();
	if (button == eMessageBox::btYes)
	{	
		eString swapName = getConfigSwapFile();
		if ( swapName != "None" ) deactivateSwapFile(swapName);
		setStatus(_("Delete all SWAP device. Please wait.."));
		if (nemTool::getInstance()->exists("/mnt/hdd/swapfile",false) && HDD) system("rm -f /mnt/hdd/swapfile");
		if (nemTool::getInstance()->exists("/mnt/cf/swapfile",false) && CF) system("rm -f /mnt/cf/swapfile");
		if (nemTool::getInstance()->exists("/mnt/usb/swapfile",false) && USB) system("rm -f /mnt/usb/swapfile");
		if (nemTool::getInstance()->exists("/media/hdd/swapfile",false) && HDD) system("rm -f /media/hdd/swapfile");
		if (nemTool::getInstance()->exists("/media/cf/swapfile",false) && CF) system("rm -f /media/cf/swapfile");
		if (nemTool::getInstance()->exists("/media/usb/swapfile",false) && USB) system("rm -f /media/usb/swapfile");
		usleep(500000);
		loadPartList();
		getStatus();
	}
}


void eSwap::swapSelected(eListBoxEntryText *item)
{
	if (item)
	{	int key = (int)item->getKey();
		eString swapFile;
		if (key == 0) swapFile = "/mnt/hdd/swapfile";
		if (key == 1) swapFile = "/mnt/cf/swapfile";
		if (key == 2) swapFile = "/mnt/usb/swapfile";
		if (getConfigSwapFile() == swapFile)
		{
			setSwapFile(0, swapFile);
		}else
		{
			setSwapFile(1, swapFile);
		}
		loadPartList();
		getStatus();
	}else
		close(0);
}


eString eSwap::readFile(eString filename)
{
	eString result;
	eString line;
	ifstream infile(filename.c_str());
	if (infile)
		while (getline(infile, line, '\n'))
			result += line + "\n";
	return result;
}

eString eSwap::getConfigSwapFile(void)
{
	eString result;
	eString procswaps = readFile("/proc/swaps");
	eString th1, th2, th3, th4, th5;
	eString td1, td2, td3, td4, td5;
	swapSize = 0;
	std::stringstream tmp;
	tmp.str(procswaps);
	tmp >> th1 >> th2 >> th3 >> th4 >> th5 >> td1 >> td2 >> td3 >> td4 >> td5;
	if (td1 != "")
	{
		swapSize = atol(td3.c_str());
		result = td1;
	}
	else
		result = "None";
	return result;
}

void eSwap::deactivateSwapFile(eString swapFile)
{
	setStatus(_("Deactivating SWAP file. Please wait.."));
	eString cmd;
	cmd = "swapoff " + swapFile;
	system(cmd.c_str());
	usleep(500000);
}

void eSwap::activateSwapFile(eString swapFile)
{
	eString procswaps = readFile("/proc/swaps");
	eString th1, th2, th3, th4, th5;
	eString td1, td2, td3, td4, td5;
	std::stringstream tmp;
	tmp.str(procswaps);
	tmp >> th1 >> th2 >> th3 >> th4 >> th5 >> td1 >> td2 >> td3 >> td4 >> td5;
	if ((td1 != "") && (td1 != swapFile))
		deactivateSwapFile(td1);
	if ((td1 == "") || (td1 != swapFile))
	{
		if ( createSwapFile(swapFile) )
		{
			setStatus(_("Formatting SWAP file. Please wait.."));
			eString cmd = "mkswap " + swapFile;
			system(cmd.c_str());
			usleep(500000);
			setStatus(_("Activating SWAP file. Please wait.."));
			cmd = "swapon " + swapFile;
			system(cmd.c_str());
			usleep(500000);
		}
	}
}

void eSwap::setSwapFile(int nextswapfile, eString swapFile)
{
	eConfig::getInstance()->setKey("/extras/swapfile", nextswapfile);
	if (nextswapfile == 1)
	{
		eConfig::getInstance()->setKey("/extras/swapfilename", swapFile.c_str());
		activateSwapFile(swapFile);
	}
	else
		deactivateSwapFile(swapFile);
}

bool eSwap::createSwapFile(eString swapFile)
{
	int key = (int)combo_size->getCurrent()->getKey();
	eString eSwapSize;
	if (key == 0) eSwapSize = "16000";
	if (key == 1) eSwapSize = "32000";
	if (key == 2) eSwapSize = "64000";
	if (key == 3) eSwapSize = "128000";
	if (access(swapFile.c_str(), W_OK) != 0)
	{
		eMessageBox msg(eString().sprintf(_("SWAP device '%s' not found!\nYou need restart dreambox to create it.\nDo you want restart now?"),swapFile.c_str()), _("Create SWAP device"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
		msg.show(); 
		int button=msg.exec(); 
		msg.hide();
		if (button == eMessageBox::btYes)
		{
			system(eString("echo '#!/bin/sh' > /var/etc/.swapcreate.sh").c_str());
			system(eString("echo 'dd if=/dev/zero of=" + swapFile + " bs=1024 count=" + eSwapSize + "' >> /var/etc/.swapcreate.sh").c_str());
			system(eString("echo 'mkswap " + swapFile + "' >> /var/etc/.swapcreate.sh").c_str());
			system("chmod 755 /var/etc/.swapcreate.sh");
			eConfig::getInstance()->setKey("/extras/wasshutdown", 1 );
			eZap::getInstance()->quit(4);
		}
		return false;
	}
	return true;
}

void eSwap::setStatus(const eString &string)
{
	status->setText(string);
}

void eSwap::getStatus()
{
	eString eStatus = getConfigSwapFile();
	if ( eStatus == "None" )
		status->setText(_("No swap file active."));
	else
		status->setText(eString().sprintf(_("Active in: %s"),eStatus.c_str()));

}

#endif // DISABLE_FILE
#endif // DISABLE_HDD
