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
#include "epgmanager.h"
#include "nemtool.h"
#include "escriptexecute.h"

int muteStatus;
int oldmhwepg;
eString mvDir[5];
eString mvStorageDir;

eEpgManager *eEpgManager::instance;

bool eEpgManager::checkLoader()
{
	eString openTvEpgLoader = "/var/bin/weekepglogtv";
	if (nemTool::getInstance()->exists((char *) openTvEpgLoader.c_str(),false))
		return true;
	else
		return false;
}

eString eEpgManager::readEmuActive()
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

void eEpgManager::start_emu()
{
	eString emuToStart = readEmuActive();
	if (emuToStart != "None")
		nemTool::getInstance()->sendCmd((char *) eString("/var/script/" + emuToStart + "_em.sh start").c_str());
}

void eEpgManager::stop_emu()
{
	eString emuToStop = readEmuActive();
	if (emuToStop != "None")
		nemTool::getInstance()->sendCmd((char *) eString("/var/script/" + emuToStop + "_em.sh stop").c_str());
}

void eEpgManager::deleteEpgCache()
{
	char *cachepath;
	if (eConfig::getInstance()->getKey("/extras/epgcachepath", cachepath))
		cachepath = strdup("/hdd");
	eString cachefilename, cachefilenamemd5;
	cachefilename.sprintf("%s/epg.dat",cachepath);
	cachefilenamemd5.sprintf("%s/epg.dat.md5",cachepath);
	free(cachepath);
	eDebug("Delete EPG cache %s",cachefilename.c_str());
	unlink(cachefilename.c_str());
	eDebug("Delete EPG md5 %s",cachefilenamemd5.c_str());
	unlink(cachefilenamemd5.c_str());
}

eEpgManager::eEpgManager()
{
	int sW = 720;
	int sH = 576;
	int winW = 410;
	int winH = 380;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("EPG Control Center"));

	label = new eLabel(this);
	label->move(ePoint(5, 15));
	label->resize(eSize(195, 30));
	label->setText(_("EPG cache folder:"));
	label->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label->setAlign(1);

	cachedir=new eTextInputField(this);
	cachedir->move(ePoint(205, 10));
	cachedir->resize(eSize(200, 30));
	cachedir->loadDeco();
	cachedir->setText("/hdd");
	cachedir->setHelpText(_("Set EPG cache folder path"));
	char *eChacheDir = 0;
	if (!eConfig::getInstance()->getKey("/extras/epgcachepath", eChacheDir))
		cachedir->setText(eChacheDir);

	labelRef = new eLabel(this);
	labelRef->move(ePoint(5, 55));
	labelRef->resize(eSize(195, 30));
	labelRef->setText(_("Select MHW Channel:"));
	labelRef->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelRef->setAlign(1);

	loadChannelList();
	comboRef=new eComboBox(this, numMHWChannel);
	comboRef->move(ePoint(205,50));
	comboRef->resize(eSize(200, 30));
	comboRef->loadDeco();
	comboRef->setHelpText(_("Select MHW Channel to update EPG"));

	for (int i=0; i < numMHWChannel ; i++)
		new eListBoxEntryText(*comboRef, eChName[i], (void*)i, 0);

	int channelsel = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/channelsel", channelsel);

	if (numMHWChannel > 0)
		comboRef->setCurrent(channelsel,true);

	labelTime = new eLabel(this);
	labelTime->move(ePoint(5, 95));
	labelTime->resize(eSize(195, 30));
	labelTime->setText(_("EPG update timeout:"));
	labelTime->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelTime->setAlign(1);

	timeTxt=new eTextInputField(this);
	timeTxt->move(ePoint(205, 90));
	timeTxt->resize(eSize(50, 30));
	timeTxt->setUseableChars("1234567890");
	timeTxt->setMaxChars(3);
	timeTxt->loadDeco();
	timeTxt->setHelpText(_("Set timeout for epg cache download"));
	timeTxt->setText("120");
	char *eEpgTimeout = 0;
	if (!eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/epgtimeout", eEpgTimeout))
		timeTxt->setText(eEpgTimeout);

	labelmin = new eLabel(this);
	labelmin->move(ePoint(260, 95));
	labelmin->resize(eSize(45, 30));
	labelmin->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelmin->setText("Sec");
	labelmin->setAlign(0);

	int bgColor = eSkin::getActive()->queryColor("global.normal.background");
	int btPos = 130;

	int stopEmu=true;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/stopemuman", stopEmu);

	chbemu = new eCheckbox(this,stopEmu,1);
	chbemu->setText(_("Stop emu before update"));
	chbemu->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	chbemu->move(ePoint(5, btPos));
	chbemu->resize(eSize(400,30));
	chbemu->setHelpText(_("Stop emu before update"));

	int isFilter=true;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/filterapplyman", isFilter);

	chkfilter = new eCheckbox(this,isFilter,1);
	chkfilter->setText(_("Apply Channels filter"));
	chkfilter->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	chkfilter->move(ePoint(5, btPos += 25));;
	chkfilter->resize(eSize(winW - 10,30));
	chkfilter->setHelpText(_("Download only selected channels"));

	btEpg[0]=new eButton(this);
	btEpg[0]->move(ePoint(5, btPos += 40));
	btEpg[0]->resize(eSize(400, 25));
	btEpg[0]->setText(_(" Download EPG cache"));
	btEpg[0]->setHelpText(_("Download data for EPG cache now"));
	btEpg[0]->setShortcut(eString().sprintf("%d",1));
	btEpg[0]->setShortcutPixmap(eString().sprintf("%d",1));
	btEpg[0]->setAlign(0);
	btEpg[0]->setProperty("backgroundColor", eString().sprintf("%i",bgColor).c_str());
	CONNECT_1_0(btEpg[0]->selected, eEpgManager::btSelected, 0);

	eString eBinSearch[3];
	eBinSearch[0] = "/var/tuxbox/plugins/epgui.cfg";
	eBinSearch[1] = "/var/tuxbox/plugins/weekepg.cfg";
	eBinSearch[2] = "/var/tuxbox/config/mv/multiepg";

	if (checkLoader())
	{
		if (nemTool::getInstance()->exists((char *)eBinSearch[0].c_str(),false))
		{
			btEpg[1]=new eButton(this);
			btEpg[1]->move(ePoint(5, btPos += 25));
			btEpg[1]->resize(eSize(400, 25));
			btEpg[1]->setText(_(" Download EPGui data"));
			btEpg[1]->setHelpText(_("Download data for EPGui now"));
			btEpg[1]->setShortcut(eString().sprintf("%d",2));
			btEpg[1]->setShortcutPixmap(eString().sprintf("%d",2));
			btEpg[1]->setAlign(0);
			btEpg[1]->setProperty("backgroundColor", eString().sprintf("%i",bgColor).c_str());
			CONNECT_1_0(btEpg[1]->selected, eEpgManager::btSelected, 1);
		}else
			eConfig::getInstance()->delKey("/Nemesis-Project/epgManager/epgui");

		if (nemTool::getInstance()->exists((char *)eBinSearch[1].c_str(),false))
		{
			btEpg[2]=new eButton(this);
			btEpg[2]->move(ePoint(5, btPos += 25));
			btEpg[2]->resize(eSize(400, 25));
			btEpg[2]->setText(_(" Download WeekEPG data"));
			btEpg[2]->setHelpText(_("Download data for WeekEPG now"));
			btEpg[2]->setShortcut(eString().sprintf("%d",3));
			btEpg[2]->setShortcutPixmap(eString().sprintf("%d",3));
			btEpg[2]->setAlign(0);
			btEpg[2]->setProperty("backgroundColor", eString().sprintf("%i",bgColor).c_str());
			CONNECT_1_0(btEpg[2]->selected, eEpgManager::btSelected, 2);
		}else
			eConfig::getInstance()->delKey("/Nemesis-Project/epgManager/weekepg");
	}else
	{
		eConfig::getInstance()->delKey("/Nemesis-Project/epgManager/epgui");
		eConfig::getInstance()->delKey("/Nemesis-Project/epgManager/weekepg");
	}

	if (nemTool::getInstance()->exists((char *)eBinSearch[2].c_str(),false))
	{
		btEpg[3]=new eButton(this);
		btEpg[3]->move(ePoint(5, btPos += 25));
		btEpg[3]->resize(eSize(400, 25));
		btEpg[3]->setText(_(" Download MvEPG data"));
		btEpg[3]->setHelpText(_("Download data for MvEPG now"));
		btEpg[3]->setShortcut(eString().sprintf("%d",4));
		btEpg[3]->setShortcutPixmap(eString().sprintf("%d",4));
		btEpg[3]->setAlign(0);
		btEpg[3]->setProperty("backgroundColor", eString().sprintf("%i",bgColor).c_str());
		CONNECT_1_0(btEpg[3]->selected, eEpgManager::btSelected, 3);
	}

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_save=new eButton(this,0,0);
	bt_save->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_save->move(ePoint(5, winH - 75));
	bt_save->resize(eSize(80, 30));
	bt_save->setShortcut("red");
	bt_save->setShortcutPixmap("red");
	bt_save->setText(_("Save"));
	bt_save->setHelpText(_("Save parameter"));
	bt_save->setAlign(0);
	if (showBorder) bt_save->loadDeco();
	CONNECT(bt_save->selected, eEpgManager::saveParameters);

	bt_update=new eButton(this,0,0);
	bt_update->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_update->move(ePoint(85, winH - 75));
	bt_update->resize(eSize(160, 30));
	bt_update->setShortcut("green");
	bt_update->setShortcutPixmap("green");
	bt_update->setText(_("Update Channels"));
	bt_update->setAlign(0);
	if (showBorder) bt_update->loadDeco();
	CONNECT_1_0(bt_update->selected, eEpgManager::btSelected, 4);

	bt_timer=new eButton(this,0,0);
	bt_timer->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_timer->move(ePoint(245, winH - 75));
	bt_timer->resize(eSize(80, 30));
	bt_timer->setShortcut("yellow");
	bt_timer->setShortcutPixmap("yellow");
	bt_timer->setText(_("Timer"));
	bt_timer->setAlign(0);
	if (showBorder) bt_timer->loadDeco();
	CONNECT(bt_timer->selected, eEpgManager::openEpgTimer);

	bt_filter=new eButton(this,0,0);
	bt_filter->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_filter->move(ePoint(325, winH - 75));
	bt_filter->resize(eSize(80, 30));
	bt_filter->setShortcut("blue");
	bt_filter->setShortcutPixmap("blue");
	bt_filter->setText(_("Channels"));
	bt_filter->setAlign(0);
	if (showBorder) bt_filter->loadDeco();
	CONNECT(bt_filter->selected, eEpgManager::openEpgFilter);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, winH - 40) );
	sbar->resize(eSize(winW, 40));
	sbar->loadDeco();	
	setFocus(cachedir);

	if (!checkLoader())
		nemTool::getInstance()->msgfail(_("OpenTV EPG Loader plugin not found!\nPlease install it, for fully functionality.\nDownload it by Addons."));

}

eEpgManager::~eEpgManager()
{
 	if (instance==this)
		instance=0;
}

int eEpgManager::runScript(eString script,eString Title,int w,int h)
{
	hide();
	eScriptExecute dlg(script,Title,w,h);
	dlg.show();
	int ret = dlg.exec();
	dlg.hide();
	show();
	return ret; 
}

bool eEpgManager::readMvDir()
{
	char buf[256];
	char *pch;
	char mvpath[256] = "-";
	char l0[64] = "-";char l1[64] = "-";char l2[64] = "-";char l3[64] = "-";char l4[64] = "-";

	system("/var/bin/mvccat > /tmp/getepg.tmp");
	FILE *in = fopen("/tmp/getepg.tmp", "r");
	if(in)
	{
		while (fgets(buf, 256, in))
		{
			if (strstr(buf ,"STORAGEDIR="))
			{	
				pch = strtok(buf,"=");
				pch = strtok(NULL,"=");
				strncpy(mvpath,pch,strlen(pch)-1);
			}
			if (strstr(buf ,"L0="))
			{	
				pch = strtok(buf,"=");
				pch = strtok(NULL,"=");
				strncpy(l0,pch,strlen(pch)-1);
			}
			if (strstr(buf ,"L1="))
			{	
				pch = strtok(buf,"=");
				pch = strtok(NULL,"=");
				strncpy(l1,pch,strlen(pch)-1);
			}
			if (strstr(buf ,"L2="))
			{	
				pch = strtok(buf,"=");
				pch = strtok(NULL,"=");
				strncpy(l2,pch,strlen(pch)-1);
			}
			if (strstr(buf ,"L3="))
			{	
				pch = strtok(buf,"=");
				pch = strtok(NULL,"=");
				strncpy(l3,pch,strlen(pch)-1);
			}
			if (strstr(buf ,"L4="))
			{	
				pch = strtok(buf,"=");
				pch = strtok(NULL,"=");
				strncpy(l4,pch,strlen(pch)-1);
			}
		}
		fclose(in);
		mvStorageDir.sprintf("%s", mvpath);
		mvDir[0].sprintf("%s", l0);
		mvDir[1].sprintf("%s", l1);
		mvDir[2].sprintf("%s", l2);
		mvDir[3].sprintf("%s", l3);
		mvDir[4].sprintf("%s", l4);
		return true;
	}
	return false;
}

eString eEpgManager::readEpguiDir()
{
	char buf[256];
	eString epgDirName;
	FILE *in = fopen("/var/tuxbox/config/epgui.conf", "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			epgDirName = eString().sprintf("%s", buf);
			epgDirName=epgDirName.left(epgDirName.length() - 1);		
			if (epgDirName.left(12) == "epgFileName=")
			{	
				fclose(in);
				epgDirName = epgDirName.right(epgDirName.length() - 12);
				epgDirName = epgDirName.left(epgDirName.length() - 9);
				return epgDirName;
			}
		}
		fclose(in);
	}
	return "None";
}

eString eEpgManager::readWeekEPGDir()
{
	char buf[256];
	eString epgDirName;
	FILE *in = fopen("/var/tuxbox/plugins/weekepg.cfg", "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			epgDirName = eString().sprintf("%s", buf);
			epgDirName=epgDirName.left(epgDirName.length() - 1);		
			if (epgDirName.left(12) == "OfflinePath=")
			{	
				fclose(in);
				epgDirName = epgDirName.right(epgDirName.length() - 12);
				epgDirName = epgDirName.left(epgDirName.length() - 1);
				return epgDirName;
			}
		}
		fclose(in);
		return "/hdd/weekepg";
	}
	return "None";
}

void eEpgManager::loadChannelList()
{
	char buf[256];
	eString epgCh;
	numMHWChannel=0;
	FILE *in = fopen("/var/etc/mhwchannel.txt", "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			epgCh = eString().sprintf("%s", buf);
			epgCh=epgCh.left(epgCh.length() - 1);		
			if (epgCh.left(7) == "chname=")
				eChName[numMHWChannel] = epgCh.right(epgCh.length() - 7);
			if (epgCh.left(7) == "chpath=")
				eChDir[numMHWChannel] = epgCh.right(epgCh.length() - 7);
			if (epgCh.left(6) == "chref=")
			{
				eChRef[numMHWChannel] = epgCh.right(epgCh.length() - 6);
				numMHWChannel++;
			}
		}
		fclose(in);
	}else
	{
		eChName[numMHWChannel] = "Marcopolo (IT)";
		eChRef[numMHWChannel] = "1:0:1:e31:16a8:fbff:820000:0:0:0:";
		eChDir[numMHWChannel] = "mhw-it";
		numMHWChannel++;
	}
}

eString eEpgManager::getMvChRef(eString mvDir)
{
	for (int i=0; i < numMHWChannel ; i++)
	{
		if (eChDir[i] == mvDir)
			return eChRef[i];
	}
	return "None";
}

void eEpgManager::btSelected(int key)
{
	if (key == 0)
	{	if (saveTimeout())
		{	oldmhwepg=0;
			eConfig::getInstance()->getKey("/extras/mhwepg", oldmhwepg);
			if (!oldmhwepg)
				eConfig::getInstance()->setKey("/extras/mhwepg", 1);
		}else
			return;
	}

	hide();

	if (key == 3)
	{
		if (readMvDir());
		{
			if (strcmp(mvStorageDir.c_str() ,"-") != 0)
			{	
				for (int i = 0; i <= 4; i++)
				{	
					eString mhwItDir = mvStorageDir + "/" + mvDir[i];
					if (mvDir[i] == "mhw-it" && checkLoader())
					{	
						eString epgFile = mhwItDir + "/epg.dat";
						eString epguiFile = readEpguiDir();
						if (epguiFile != "None") 
							epguiFile = epguiFile + "epgui.dat";
						if (nemTool::getInstance()->exists((char *) epguiFile.c_str(),false))
						{	
							eMessageBox msg(_("EPGui data file found\nDo you want use it, and create a symbolic link?"), _("Info"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btCancel);
							msg.show(); int button=msg.exec(); msg.hide();
							if (button == eMessageBox::btYes)
							{  
								if (!nemTool::getInstance()->existsdir((char *) mvStorageDir.c_str(),false)) 
									system(eString("mkdir " + mvStorageDir).c_str());
								if (!nemTool::getInstance()->existsdir((char *) mhwItDir.c_str(),false))
									system(eString("mkdir " + mhwItDir).c_str());

								system(eString("echo rm -f " + mhwItDir + "/epg.* > /tmp/script.sh").c_str());
								system(eString("echo ln -s " + epguiFile + " " + mhwItDir + "/epg.dat  >> /tmp/script.sh").c_str());
								system("echo 'echo Creating index for MVEpg....' >> /tmp/script.sh ");
								system(eString("echo /var/tuxbox/config/mv/epgidx " + epgFile + " >> /tmp/script.sh").c_str());
								system("chmod 755 /tmp/script.sh");
								runScript("/tmp/script.sh",_("Download Data for MvEPG"));
								unlink("/tmp/script.sh");

								show();
								return;
							}
							else if (button == eMessageBox::btCancel)
							{  
								show();
								return;
							}
						}
					}
				}
			}
		}	
	}
	else if (key == 4)
	{
		eMessageBox box4(_("Do you want refresh the channel list\nfrom Satellite data?"),_("Update"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
		box4.show();int ret = box4.exec();box4.hide();
		if (ret != eMessageBox::btYes) 
		{
			show();
			return;
		}
	}

	//Store current channel
	if (eServiceInterface::getInstance()->service)
		sRefCur = eString(eServiceInterface::getInstance()->service.toString()).upper();

	//Get Epg Channel from combobox
	sRefEpg = eString(eChRef[(int)comboRef->getCurrent()->getKey()]).upper();

	if (sRefCur!=sRefEpg)
		eZapMain::getInstance()->playService(sRefEpg, eZapMain::psNoUser|eZapMain::psSetMode);

	numOfTestChannel = 1;
	epgZapTimer=new eTimer(eApp);
	epgZapTimer->start(600,false); 
	CONNECT_1_0(epgZapTimer->timeout,eEpgManager::zapTimeout,key);
}

void eEpgManager::zapTimeout(int key)
{
	numOfTestChannel++;
	eFrontend *fe = eFrontend::getInstance();
	int status = fe->Status();
	bool lock = status & FE_HAS_LOCK;
	if (lock || (numOfTestChannel >= 30) )
	{	
		epgZapTimer->stop();
		delete epgZapTimer;
		epgZapTimer=0;
		if (numOfTestChannel >= 30)
		{	//return to stored channel
			if (sRefCur!=sRefEpg)
				eZapMain::getInstance()->playService(sRefCur, eZapMain::psNoUser|eZapMain::psSetMode);
			show();
			return;
		}
		updateEPG(key);
	}
}

void eEpgManager::updateEPG(int key)
{
	if (key == 0)
	{	
		eMessageBox box(_("Download Data for EPG cache...\nPlease wait.\nPress Cancel to abort."), 
						_("Running.."),eMessageBox::btCancel|eMessageBox::iconInfo, eMessageBox::btOK, atoi(timeTxt->getText().c_str()));
		box.show(); int msgRet = box.exec(); box.hide();

		if (!oldmhwepg)
			eConfig::getInstance()->setKey("/extras/mhwepg", 0);

		if (msgRet == eMessageBox::btOK)
		{
			eMessageBox box3(eString().sprintf(_("Saving EPG cache in %s/epg.dat...\nPlease wait."),cachedir->getText().c_str()).c_str(),
						_("Running.."), eMessageBox::iconInfo);
			box3.show();
				eEPGCache::getInstance()->messages.send(eEPGCache::Message(eEPGCache::Message::save));
				sleep(1);
			box3.hide();
		}
	}
	else if (key == 1)
	{
		if (chbemu->isChecked()) stop_emu();
		getEpguiEpgData(true);
		if (chbemu->isChecked()) start_emu();
	}
	else if (key == 2)
	{
		if (chbemu->isChecked()) stop_emu();
		getWeekEpgData(true);
		if (chbemu->isChecked()) start_emu();
	}
	else if (key == 3)
	{
		if (chbemu->isChecked()) stop_emu();
		getMvEpgData(false,true);
		if (chbemu->isChecked()) start_emu();
	}

	else if (key == 4)
	{
		if (chbemu->isChecked()) stop_emu();
		updateChannellist();
		if (chbemu->isChecked()) start_emu();
	}

	//return to stored channel
	if (sRefCur!=sRefEpg)
		eZapMain::getInstance()->playService(sRefCur, eZapMain::psNoUser|eZapMain::psSetMode);

	show();
}

void eEpgManager::updateEpgEvent()
{
	if (!instance)
		instance=this;

	int stopEmu = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/stopemu", stopEmu);
	if (stopEmu)
		stop_emu();

	muteStatus = eAVSwitch::getInstance()->getMute();
	if (!muteStatus)
		eAVSwitch::getInstance()->toggleMute();

	oldmhwepg=0;
	eConfig::getInstance()->getKey("/extras/mhwepg", oldmhwepg);

	int epguiToExecute = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/epgui", epguiToExecute);
	if (epguiToExecute)
		getEpguiEpgData(false);

	int weekepgToExecute = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/weekepg", weekepgToExecute);
	if (weekepgToExecute)
		getWeekEpgData(false);

	int mvToExecute = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/mvepg", mvToExecute);
	if (mvToExecute)
		getMvEpgData(epguiToExecute,false);

	if (!oldmhwepg)
		eConfig::getInstance()->setKey("/extras/mhwepg", 1);
}

void eEpgManager::FinishedEpgEvent()
{
	if (!instance)
		instance=this;

	if (!oldmhwepg)
		eConfig::getInstance()->setKey("/extras/mhwepg", 0);

	int toExecute = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/mhwepg", toExecute);
	if (toExecute)
		eEPGCache::getInstance()->messages.send(eEPGCache::Message(eEPGCache::Message::save));

	int stopEmu = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/stopemu", stopEmu);
	if (stopEmu)
		start_emu();
	
	if (!muteStatus)
		eAVSwitch::getInstance()->toggleMute();

}

void eEpgManager::getEpguiEpgData(bool manual)
{
	int applyFilter = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/applyfilter", applyFilter);
	eString appFilter = "";
	if (manual)
	{	if (chkfilter->isChecked())
			 appFilter = " -f";
	}else
	{	if (applyFilter)
			 appFilter = " -f";
	}	

	if (checkLoader())
	{
		eString epguiDir = readEpguiDir();
		if (epguiDir != "None")
		{
			int mhwepg=0;
			eConfig::getInstance()->getKey("/extras/mhwepg", mhwepg);
			if (mhwepg)
				eConfig::getInstance()->setKey("/extras/mhwepg", 0);
			
			if (manual)
			{	eString epgFile = epguiDir + "epg.dat";
				system(eString("echo /var/bin/weekepglogtv -e " + epguiDir + appFilter + " > /tmp/script.sh").c_str());
				system(eString("echo mv " + epgFile + " " + epguiDir + "epgui.dat >> /tmp/script.sh").c_str());
				system("chmod 755 /tmp/script.sh");
				runScript("/tmp/script.sh",_("Download Data for EPGui"));
				unlink("/tmp/script.sh");
         } else
			{	eString mvEpgFile = epguiDir + "epg.dat";
				nemTool::getInstance()->sendCmd((char *) eString("/var/bin/weekepglogtv -e " + epguiDir + appFilter).c_str());
				if (nemTool::getInstance()->exists((char *) mvEpgFile.c_str(),false))
					nemTool::getInstance()->sendCmd((char *) eString("mv " + mvEpgFile + " " + epguiDir + "epgui.dat").c_str());
			}

			if (mhwepg)
				eConfig::getInstance()->setKey("/extras/mhwepg", 1);
		}
	}
}

void eEpgManager::getWeekEpgData(bool manual)
{
	int applyFilter = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/applyfilter", applyFilter);
	eString appFilter = "";
	if (manual)
	{	if (chkfilter->isChecked())
			 appFilter = " -f";
	}else
	{	if (applyFilter)
			 appFilter = " -f";
	}	

	if (checkLoader())
	{
		eString weekepgDir = readWeekEPGDir();
		if (weekepgDir != "None")
		{
			int mhwepg=0;
			eConfig::getInstance()->getKey("/extras/mhwepg", mhwepg);
			if (mhwepg)
				eConfig::getInstance()->setKey("/extras/mhwepg", 0);

			if (!nemTool::getInstance()->existsdir((char *) weekepgDir.c_str(),false))
				system(eString("mkdir " + weekepgDir).c_str());

			if (manual)
				runScript(eString("/var/bin/weekepglogtv -w "+ weekepgDir + "/" + appFilter),_("Download Data for WeekEPG"));
         else
				nemTool::getInstance()->sendCmd((char *) eString("/var/bin/weekepglogtv -w "+ weekepgDir + "/" + appFilter).c_str());

			if (mhwepg)
				eConfig::getInstance()->setKey("/extras/mhwepg", 1);
		}
	}
}

void eEpgManager::getMvEpgData(bool epguiExecuted, bool manual)
{
	int mhwepg=0;
	eConfig::getInstance()->getKey("/extras/mhwepg", mhwepg);
	if (mhwepg)
		eConfig::getInstance()->setKey("/extras/mhwepg", 0);

	int applyFilter = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/applyfilter", applyFilter);
	eString appFilter = "";
	if (manual)
	{	if (chkfilter->isChecked())
			 appFilter = " -f";
	}else
	{	if (applyFilter)
			 appFilter = " -f";
	}	

	if (readMvDir())
	{
		if (strcmp(mvStorageDir.c_str() ,"-") != 0)
		{
			if (!nemTool::getInstance()->existsdir((char *) mvStorageDir.c_str(),false))
				system(eString("mkdir " + mvStorageDir).c_str());
			for (int i = 0; i <= 4; i++)
			{	
				eString mhwItDir = mvStorageDir + "/" + mvDir[i];
				if (mvDir[i] == "mhw-it" && checkLoader())
				{
					eString epguiFile = readEpguiDir() + "epgui.dat";
					eString mvEpgFile = mhwItDir + "/epg.dat";

					if (!nemTool::getInstance()->existsdir((char *) mhwItDir.c_str(),false))
						system(eString("mkdir " + mhwItDir).c_str());

					if (manual)
					{
						system(eString("echo rm -f " + mhwItDir + "/epg.* > /tmp/script.sh").c_str());
						system(eString("echo /var/bin/weekepglogtv -e " + mhwItDir + "/" + appFilter + " >> /tmp/script.sh").c_str());
						system("echo 'echo Creating index for MVEpg....' >> /tmp/script.sh ");
						system(eString("echo /var/tuxbox/config/mv/epgidx " + mhwItDir + "/epg.dat >> /tmp/script.sh").c_str());
						system("chmod 755 /tmp/script.sh");
						runScript("/tmp/script.sh",_("Download Data for MvEPG"));
						unlink("/tmp/script.sh");
					}else
					{
						if (!epguiExecuted)
							nemTool::getInstance()->sendCmd((char *) eString("/var/bin/weekepglogtv -e " + mhwItDir + "/" + appFilter).c_str());
						system(eString("rm -f " + mhwItDir + "/epg.*").c_str());
						if (nemTool::getInstance()->exists((char *) epguiFile.c_str(),false))
							system(eString("ln -s " + epguiFile + " " + mhwItDir + "/epg.dat").c_str());
						else
							nemTool::getInstance()->sendCmd((char *) eString("/var/bin/weekepglogtv -e " + mhwItDir + "/" + appFilter).c_str());
						if (nemTool::getInstance()->exists((char *) mvEpgFile.c_str(),false))
							nemTool::getInstance()->sendCmd((char *) eString("/var/tuxbox/config/mv/epgidx " + mhwItDir + "/epg.dat").c_str());
					}
				}
				else if (mvDir[i] == "mhw-nl" || mvDir[i] == "mhw-es" || mvDir[i] == "mhw-fr" ||
						mvDir[i] == "otv-fr" || mvDir[i] == "mhw-pl")
				{
					if (!nemTool::getInstance()->existsdir((char *) mhwItDir.c_str(),false))
						system(eString("mkdir " + mhwItDir).c_str());
					nemTool::getInstance()->sendCmd((char *) eString("/var/tuxbox/config/mv/multiepg -l -p  " + mhwItDir).c_str());
					nemTool::getInstance()->sendCmd((char *) eString("/var/tuxbox/config/mv/epgidx " + mhwItDir + "/epg.dat").c_str());
				}
			}
		}
	}
	
	if (mhwepg)
		eConfig::getInstance()->setKey("/extras/mhwepg", 1);
}

void eEpgManager::updateChannellist()
{
	if (checkLoader())
	{
		int mhwepg=0;
		eConfig::getInstance()->getKey("/extras/mhwepg", mhwepg);
		if (mhwepg)	eConfig::getInstance()->setKey("/extras/mhwepg", 0);
		runScript("/var/bin/weekepglogtv -ch",_("Update channel list"));
		if (mhwepg)	eConfig::getInstance()->setKey("/extras/mhwepg", 1);
	}
}

void eEpgManager::saveParameters()
{
	if (numMHWChannel > 0)
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/channelsel", (int)comboRef->getCurrent()->getKey());
	if (saveTimeout()) saveDir();
}

bool eEpgManager::saveTimeout()
{
	if(timeTxt->getText())
	{ 
		int iTime = atoi(timeTxt->getText().c_str());
		if (iTime < 60 || iTime > 240)
		{
			nemTool::getInstance()->msgfail(_("Incorret Timeout valor!\nPlease enter a valor betwenn 60 and 240 second.")); 
			setFocus(timeTxt); 
			return false;
		}
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/epgtimeout", timeTxt->getText().c_str());
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/filterapplyman", chkfilter->isChecked());
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/stopemuman", chbemu->isChecked());
		eConfig::getInstance()->flush();
	}else 
	{
		nemTool::getInstance()->msgfail(_("You do enter a valid timeout!")); 
		setFocus(timeTxt); 
		return false;
	}
	return true;
}

void eEpgManager::saveDir()
{
	if(cachedir->getText())
	{ 
		eMessageBox box(_("Saving folder parameter.\nPlease Wait..."), _("Running.."), 0);
		box.show();
			bool folderOK;
			folderOK = nemTool::getInstance()->existsdir((char*) cachedir->getText().c_str(),false);
			usleep(700000);
		box.hide();
		if (!folderOK)
		{
			eMessageBox msg(_(eString().sprintf(_("The folder '%s' does not exist.\nDo you want continue and save anyway?"),cachedir->getText().c_str()).c_str()), _("Folder error"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
			msg.show(); 
			int button=msg.exec(); 
			msg.hide();
			if (button == eMessageBox::btNo)
			{
				setFocus(cachedir); 
				return;
			}
		}
		eConfig::getInstance()->setKey("/extras/epgcachepath", cachedir->getText().c_str());
		eConfig::getInstance()->flush();
	}else 
	{
		nemTool::getInstance()->msgfail(_("You do enter a valid path!")); 
		setFocus(cachedir); 
		return;
	}
}

void eEpgManager::openEpgTimer()
{
	hide();
	eEpgManagerTimer dlg(eChRef[(int)comboRef->getCurrent()->getKey()],comboRef->getText());
	dlg.show();
	dlg.exec();
	dlg.hide();
	show();
}

void eEpgManager::openEpgFilter()
{
	hide();
	eEpgManagerFilter dlg; 
	dlg.show();
	dlg.exec();
	dlg.hide();
	show();
}

eEpgManagerTimer::eEpgManagerTimer(eServiceReference ref,eString ch)
{
	int sW = 720;
	int sH = 576;
	int winW = 350;
	int winH = 310;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Setting daily EPG Timer"));

	label = new eLabel(this);
	label->move(ePoint(5, 10));
	label->resize(eSize(340, 30));
	label->setText(eString().sprintf(_("Selected MHW Channel: %s"),ch.c_str()));
	label->setFont(eSkin::getActive()->queryFont("eStatusBar"));

	eString eFunction[6];
	eString eHelpText[4];
	eString eBinSearch[3];
	eFunction[0] = _("Enable update EPG cache");	eHelpText[0] = _("Enable / Disable timer update EPG cache");
	eFunction[1] = _("Enable update EPGui");	eHelpText[1] = _("Enable / Disable timer update EPGui");
	eFunction[2] = _("Enable update WeekEPG");	eHelpText[2] = _("Enable / Disable timer update WeekEPG");
	eFunction[3] = _("Enable update MvEPG");	eHelpText[3] = _("Enable / Disable timer update MvEPG");
	eFunction[4] = _("Stop emu before update");
	eFunction[5] = _("Apply Channels filter");
	eBinSearch[0] = "/var/tuxbox/plugins/epgui.cfg";
	eBinSearch[1] = "/var/tuxbox/plugins/weekepg.cfg";
	eBinSearch[2] = "/var/tuxbox/config/mv/multiepg";

	int cbPos = 50;

	int isChecked = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/stopemu", isChecked);
	chepg[4] = new eCheckbox(this,isChecked,1);
	chepg[4]->setText(eFunction[4].c_str());
	chepg[4]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	chepg[4]->move(ePoint(5, cbPos));
	chepg[4]->resize(eSize(340,25));
	chepg[4]->setHelpText(eFunction[4].c_str());
	CONNECT_1_0(chepg[4]->checked, eEpgManagerTimer::cbSelected,4);

	isChecked = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/applyfilter", isChecked);
	chepg[5] = new eCheckbox(this,isChecked,1);
	chepg[5]->setText(eFunction[5].c_str());
	chepg[5]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	chepg[5]->move(ePoint(5, cbPos += 27));
	chepg[5]->resize(eSize(340,25));
	chepg[5]->setHelpText(eFunction[5].c_str());
	CONNECT_1_0(chepg[5]->checked, eEpgManagerTimer::cbSelected,5);

	isChecked = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/mhwepg", isChecked);
	chepg[0] = new eCheckbox(this,isChecked,1);
	chepg[0]->setText(eFunction[0].c_str());
	chepg[0]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	chepg[0]->move(ePoint(5, cbPos += 40));
	chepg[0]->resize(eSize(340,25));
	chepg[0]->setHelpText(eHelpText[0].c_str());
	CONNECT_1_0(chepg[0]->checked, eEpgManagerTimer::cbSelected,0);

	if (nemTool::getInstance()->exists((char *)eBinSearch[0].c_str(),false))
	{	isChecked = 0;
		eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/epgui", isChecked);
		chepg[1] = new eCheckbox(this,isChecked,1);
		chepg[1]->setText(eFunction[1].c_str());
		chepg[1]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		chepg[1]->move(ePoint(5, cbPos += 27));
		chepg[1]->resize(eSize(340,25));
		chepg[1]->setHelpText(eHelpText[1].c_str());
		CONNECT_1_0(chepg[1]->checked, eEpgManagerTimer::cbSelected,1);
	}

	if (nemTool::getInstance()->exists((char *)eBinSearch[1].c_str(),false))
	{	isChecked = 0;
		eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/weekepg", isChecked);
		chepg[2] = new eCheckbox(this,isChecked,1);
		chepg[2]->setText(eFunction[2].c_str());
		chepg[2]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		chepg[2]->move(ePoint(5, cbPos += 27));
		chepg[2]->resize(eSize(340,25));
		chepg[2]->setHelpText(eHelpText[2].c_str());
		CONNECT_1_0(chepg[2]->checked, eEpgManagerTimer::cbSelected,2);
	}

	if (nemTool::getInstance()->exists((char *)eBinSearch[2].c_str(),false))
	{	isChecked = 0;
		eConfig::getInstance()->getKey("/Nemesis-Project/epgManager/mvepg", isChecked);
		chepg[3] = new eCheckbox(this,isChecked,1);
		chepg[3]->setText(eFunction[3].c_str());
		chepg[3]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		chepg[3]->move(ePoint(5, cbPos += 27));
		chepg[3]->resize(eSize(340,25));
		chepg[3]->setHelpText(eHelpText[3].c_str());
		CONNECT_1_0(chepg[3]->checked, eEpgManagerTimer::cbSelected,3);
	}

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(5, winH - 75));
	bt_exit->resize(eSize(100, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setText(_("Close"));
	bt_exit->setHelpText(_("Close"));
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	CONNECT(bt_exit->selected, eWidget::reject);

	bt_add=new eButton(this);
	bt_add->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_add->move(ePoint(105, winH - 75));
	bt_add->resize(eSize(140, 30));
	bt_add->setShortcut("yellow");
	bt_add->setShortcutPixmap("yellow");
	bt_add->setText(_("Create event"));
	bt_add->setHelpText(_("Create EPG Download event to Timerlist"));
	bt_add->setAlign(0);
	if (showBorder) bt_add->loadDeco();
	CONNECT_1_0(bt_add->selected,  eEpgManagerTimer::addTimerEvent,ref);

	bt_timer=new eButton(this);
	bt_timer->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_timer->move(ePoint(245, winH - 75));
	bt_timer->resize(eSize(100, 30));
	bt_timer->setShortcut("blue");
	bt_timer->setShortcutPixmap("blue");
	bt_timer->setText(_("Timer"));
	bt_timer->setHelpText(_("Open Timer panel"));
	bt_timer->setAlign(0);
	if (showBorder) bt_timer->loadDeco();
	CONNECT(bt_timer->selected, eEpgManagerTimer::openTimer);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, winH - 40) );
	sbar->resize(eSize(winW, 40));
	sbar->loadDeco();	
}

eEpgManagerTimer::~eEpgManagerTimer()
{
}

void eEpgManagerTimer::cbSelected(int state, int key)
{
	if (key == 0)
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/mhwepg", state);
	else if (key == 1)
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/epgui", state);
	else if (key == 2)
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/weekepg", state);
	else if (key == 3)
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/mvepg", state);
	else if (key == 4)
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/stopemu", state);
	else if (key == 5)
		eConfig::getInstance()->setKey("/Nemesis-Project/epgManager/applyfilter", state);
}

void eEpgManagerTimer::openTimer()
{
	hide();
	eTimerListView setup;
#ifndef DISABLE_LCD
	setup.setLCD(LCDTitle, LCDElement);
#endif
	setup.show();
	setup.exec();
	setup.hide();
	show();
}

void eEpgManagerTimer::addTimerEvent(eServiceReference ref)
{
	hide();
		ref.descr = "/Automatic EPG Download";
		int appType=0;
		appType = ePlaylistEntry::epgDown;
		appType |= ePlaylistEntry::doGoSleep;
		appType |= ePlaylistEntry::isRepeating;
		appType |= ePlaylistEntry::Su;
		appType |= ePlaylistEntry::Mo;
		appType |= ePlaylistEntry::Tue;
		appType |= ePlaylistEntry::Wed;
		appType |= ePlaylistEntry::Thu;
		appType |= ePlaylistEntry::Fr;
		appType |= ePlaylistEntry::Sa;

		EITEvent e;
		e.start_time = 14400;
		e.duration = 600;

		eTimerEditView  v(e, appType, ref);
		v.show();
		v.exec();
		v.hide();
	show();
}

eListBoxEntryMck::eListBoxEntryMck ( eListBox<eListBoxEntryMck>* lb, const char* txt, void* key, int align, const eString& hlptxt, int keytype )
		:eListBoxEntryText ( ( eListBox<eListBoxEntryText>* ) lb, txt, key, align, hlptxt, keytype ),
		pm ( eSkin::getActive()->queryImage ( "eListBoxEntryCheck" ) ),
		checked ( 0 )
{
	selectable=1;
	if ( listbox )
		CONNECT ( listbox->selected, eListBoxEntryMck::LBSelected );
}

void eListBoxEntryMck::setCheck ( bool ncheck )
{
	if ( checked != ncheck )
	{
		checked=ncheck;
		listbox->invalidateCurrent();
	}
}

void eListBoxEntryMck::LBSelected ( eListBoxEntry* t )
{
	if ( t == this )
		setCheck ( checked^1 );
}

const eString& eListBoxEntryMck::redraw ( gPainter *rc, const eRect& rect, gColor coActiveB, gColor coActiveF, gColor coNormalB, gColor coNormalF, int state )
{
	bool b;
	if ( ( b = ( state == 2 ) ) )
		state = 0;
	eListBoxEntryText::redraw ( rc, rect, coActiveB, coActiveF, coNormalB, coNormalF, state );
	if ( pm && checked )
	{
		eRect right = rect;
		right.setLeft ( rect.right() - ( pm->x + 10 ) );
		rc->clip ( right );
		eSize psize = pm->getSize(),
		              esize = rect.size();
		int yOffs = rect.top() + ( ( esize.height() - psize.height() ) / 2 );
		rc->blit ( *pm, ePoint ( right.left(), yOffs ), right, gPixmap::blitAlphaTest );
		rc->clippop();
	}
	return text;
}

int numChSelected;

eEpgManagerFilter::eEpgManagerFilter() :eWindow()
{
	int sW = 720;
	int sH = 576;
	int winW = 400;
	int winH = 370;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));

	list = new eListBox<eListBoxEntryMck>(this);
	list->move(ePoint(5, 10));
	list->resize(eSize(winW - 10,winH - 40));
	CONNECT(list->selected, eEpgManagerFilter::changeStatus);

	fileName = "/var/etc/channels.it";
	readChannelList();
	numChSelected = 0;

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_up=new eButton(this,0,0);
	bt_up->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_up->move(ePoint(5, winH - 35));
	bt_up->resize(eSize(295, 30));
	bt_up->setShortcut("red");
	bt_up->setShortcutPixmap("red");
	bt_up->setText(_("Enable move Channel"));
	bt_up->setAlign(0);
	if (showBorder) bt_up->loadDeco();
	CONNECT(bt_up->selected, eEpgManagerFilter::toggleMove);

	bt_channel=new eButton(this,0,0);
	bt_channel->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_channel->move(ePoint(300, winH - 35));
	bt_channel->resize(eSize(95, 30));
	bt_channel->setShortcut("blue");
	bt_channel->setShortcutPixmap("blue");
	bt_channel->setText(_("SKY UK"));
	bt_channel->setAlign(0);
	if (showBorder) bt_channel->loadDeco();
	CONNECT(bt_channel->selected, eEpgManagerFilter::toggleChannels);

	updateLabel();
	reordering = 0;
}

eEpgManagerFilter::~eEpgManagerFilter()
{
	saveChannelList();
}

struct countChecked
{
	countChecked()	{	}

	bool operator()(eListBoxEntryMck& s)
	{
		if (s.getCheck()) 
			numChSelected++;
		return 0;
	}
};

void eEpgManagerFilter::updateLabel()
{
	int numChannel = list->getCount();
	list->forEachEntry(countChecked());
 	if (bt_channel->getText() == "SKY UK")
		setText(eString().sprintf(_("%d channel in list SKY-IT, %d selected."),numChannel,numChSelected));
 	else
		setText(eString().sprintf(_("%d channel in list SKY-UK, %d selected."),numChannel,numChSelected));
}

void eEpgManagerFilter::toggleMove()
{
	if (!reordering)
	{
		bt_up->setText(_("Disable move Channel"));
		selectedBackColor = list->getActiveBackColor();
		reordering = 1;
	}
	else
	{
		reordering= 0;
		bt_up->setText(_("Enable move Channel"));
		list->setActiveColor(selectedBackColor,gColor(0));
		list->setMoveMode(0);
	}
}

void eEpgManagerFilter::toggleChannels()
{
	if (bt_channel->getText() == "SKY UK")
	{
		bt_channel->setText(_("SKY IT"));
		fileName = "/var/etc/channels.uk";
	}
	else
	{
		bt_channel->setText(_("SKY UK"));
		fileName = "/var/etc/channels.it";
	}
	readChannelList();
	numChSelected = 0;
	updateLabel();
}

void eEpgManagerFilter::changeStatus(eListBoxEntryMck *item)
{
	switch (reordering)
	{
		case 0:
			if (item->getCheck())
				numChSelected = -1;
			else
				numChSelected = 1;
			updateLabel();
			return;
		case 1:
			list->setMoveMode(1);
			list->setActiveColor(eSkin::getActive()->queryColor("eServiceSelector.entrySelectedToMove"),gColor(0));
			reordering = 2;	
			item->setCheck(!item->getCheck());
			return;
		case 2:
			list->setMoveMode(0);
			list->setActiveColor(selectedBackColor,gColor(0));
			reordering = 1;	
			item->setCheck(!item->getCheck());
			return;
	}
}

void eEpgManagerFilter::readChannelList()
{
	list->beginAtomic(); 
	list->clearList();
	list->hide();
	eListBoxEntryMck *lblast;
	char buf[256];
	FILE *in = fopen(fileName, "r");
	if(in)
	{	
		eString e,f;
		int a,b;
		unsigned int pos;
		while (fgets(buf, 256, in))
		{
			f.sprintf("%s", buf);
			sscanf(buf,"%d;%d",&a,&b);
			e.sprintf("%d;%d;",a,b);
			f = f.right(f.length() - e.length());
			pos = f.find(";");
			lblast=new eListBoxEntryMck(list, f.left(pos).c_str(),(void*)b,0,f.right(f.length() - pos - 1));
			if (a == 2) a = 0;
 			lblast->setCheck(a);
		}
		fclose(in);
	}
	list->show();
	list->endAtomic(); 
}

struct saveChPosition
{
	FILE *in;
	saveChPosition(FILE *in):in(in)	{	}

	bool operator()(eListBoxEntryMck& s)
	{
		fprintf (in, "%d;%d;%s;%s", s.getCheck(),(int)s.getKey(),s.getText().c_str(),s.getHelpText().c_str());
		return 0;
	}
};

void eEpgManagerFilter::saveChannelList()
{
	if (list->getCount())
	{
		FILE *in = fopen(fileName, "w");
		if(in)
			list->forEachEntry(saveChPosition(in));
		fclose(in);
	}
}
