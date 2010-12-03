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
#include "inadyn.h"
#include "showfile.h"
#include "nemtool.h"

eInadyn *eInadyn::instance;

void eInadyn::getParameter()
{
	isStarted = 0;
	iUser = "xxxxxx@gmail.com";
	iPwd = "xxxpass";
	iDomain = "xxxxxx.dyndns.org";
	iPeriod = 60;
	iDyn = "dyndns@dyndns.org";
	iUseLog = 1;
	char *fileName = "/var/etc/inadyn.conf";
	eString ePar;
	char buf[256];
	FILE *in = fopen(fileName, "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			ePar = eString().sprintf("%s", buf);
			ePar = ePar.left(ePar.length() - 1);		
			if (ePar.left(10) == "isstarted=") isStarted = atoi(ePar.right(ePar.length() -10).c_str());
			if (ePar.left(5) == "user=") iUser = ePar.right(ePar.length() -5);
			if (ePar.left(9) == "password=") iPwd = ePar.right(ePar.length() -9);
			if (ePar.left(6) == "alias=") iDomain = ePar.right(ePar.length() -6);
			if (ePar.left(7) == "period=") iPeriod = atoi(ePar.right(ePar.length() -7).c_str()) / 1000;
			if (ePar.left(8) == "service=") iDyn = ePar.right(ePar.length() -8);
			if (ePar.left(7) == "uselog=") iUseLog = atoi(ePar.right(ePar.length() -7).c_str());
		}
		fclose(in);
	}
}

void eInadyn::setParameter()
{
	iUseLog=0;
	if (useLog->isChecked())
		iUseLog=1;
	iUser = input[0]->getText().c_str();
	iPwd = input[1]->getText().c_str();
	iDomain = input[2]->getText().c_str();
	iPeriod = atoi(input[3]->getText().c_str()) * 1000;
	iDyn = serviceType->getCurrent()->getText().c_str();
	char *fileName = "/var/etc/inadyn.conf";
	char *fileParam = "/var/etc/inadynpar.conf";
	eString ePar;
	if (nemTool::getInstance()->exists(fileName,false)) unlink(fileName);
	FILE *in = fopen(fileName, "w");
	if(in)
	{	
		fprintf (in, "isstarted=%i\n", isStarted);
		fprintf (in, "user=%s\n", iUser.c_str());
		fprintf (in, "password=%s\n", iPwd.c_str());
		fprintf (in, "alias=%s\n", iDomain.c_str());
		fprintf (in, "period=%i\n", iPeriod);
		fprintf (in, "service=%s\n", iDyn.c_str());
		fprintf (in, "uselog=%i\n", iUseLog);
		fclose(in);
	}

	if (nemTool::getInstance()->exists(fileParam,false)) unlink(fileParam);
	eString parameter = eString().sprintf("--username %s --password %s --alias %s --update_period %i",iUser.c_str(),iPwd.c_str(),iDomain.c_str(),iPeriod).c_str(); 
	parameter += eString().sprintf(" --dyndns_system %s", iDyn.c_str()).c_str();
	if(iUseLog)
		parameter += " --log_file /var/log/inadyn.log";
	FILE *in1 = fopen(fileParam, "w");
	if(in1)
	{	
		fprintf (in1, "%s\n", parameter.c_str());
		fclose(in1);
	}
}

eInadyn::eInadyn()
{
	int sW = 720;
	int sH = 576;
	int winW = 444;
	int winH = 330 ;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Setting Inadyn"));

	if (!instance)
		instance=this;
	

	int labelWidth = 115;
	int textWidth = clientrect.width() - labelWidth -15 ;

	getParameter();	

	int posL=20;
	int posI=15;
	
	label[0]=new eLabel(this);
	label[0]->move(ePoint(5, posL));
	label[0]->resize(eSize(labelWidth, 25));
	label[0]->setText(_("Username:"));
	label[0]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[0]->setAlign(1);
	input[0]=new eTextInputField(this);
	input[0]->move(ePoint(label[0]->getSize().width() +10 , posI));
	input[0]->resize(eSize(textWidth, 30));
//	input[0]->setUseableChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890@._-");
	input[0]->setText(iUser.c_str());
	input[0]->setHelpText(_("Set user name"));
	input[0]->loadDeco();

	label[1]=new eLabel(this);
	label[1]->move(ePoint(5, posL += 35));
	label[1]->resize(eSize(labelWidth, 25));
	label[1]->setText("Password:");
	label[1]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[1]->setAlign(1);
	input[1]=new eTextInputField(this);
	input[1]->move(ePoint(label[1]->getSize().width() +10 , posI +=35));
	input[1]->resize(eSize(textWidth, 30));
//	input[1]->setUseableChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890._-");
	input[1]->setText(iPwd.c_str());
	input[1]->setHelpText(_("Set password"));
	input[1]->loadDeco();

	label[2]=new eLabel(this);
	label[2]->move(ePoint(5,  posL += 35));
	label[2]->resize(eSize(labelWidth, 25));
	label[2]->setAlign(1);
	label[2]->setText("Alias:");
	label[2]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	input[2]=new eTextInputField(this);
	input[2]->move(ePoint(label[2]->getSize().width() +10 , posI +=35));
	input[2]->resize(eSize(textWidth, 30));
//	input[2]->setUseableChars("abcdefghijklmnopqrstuvwxyz1234567890.");
	input[2]->setText(iDomain.c_str());
	input[2]->setHelpText(_("Set domain name to update"));
	input[2]->loadDeco();

	label[3]=new eLabel(this);
	label[3]->move(ePoint(5,  posL += 35));
	label[3]->resize(eSize(labelWidth, 25));
	label[3]->setText(_("Period:"));
	label[3]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[3]->setAlign(1);
	input[3]=new eTextInputField(this);
	input[3]->move(ePoint(label[3]->getSize().width() +10 , posI +=35));
	input[3]->resize(eSize(textWidth, 30));
	input[3]->setUseableChars("1234567890");
	input[3]->setText(eString().sprintf("%i",iPeriod).c_str());
	input[3]->setHelpText(_("Set update period time in sec"));
	input[3]->loadDeco();

	label[4]=new eLabel(this);
	label[4]->move(ePoint(5, posL += 35));
	label[4]->resize(eSize(labelWidth, 25));
	label[4]->setText(_("System:"));
	label[4]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[4]->setAlign(1);

	serviceType=new eComboBox(this, 6);
	serviceType->move(ePoint(label[4]->getSize().width() +10 , posI += 35));
	serviceType->resize(eSize(textWidth, 30));
	serviceType->loadDeco();
	new eListBoxEntryText(*serviceType, _("dyndns@dyndns.org"), (void*)0, 0);
	new eListBoxEntryText(*serviceType, _("statdns@dyndns.org"), (void*)1, 0);
	new eListBoxEntryText(*serviceType, _("custom@dyndns.org"), (void*)2, 0);
	new eListBoxEntryText(*serviceType, _("default@freedns.afraid.org"), (void*)3, 0);
	new eListBoxEntryText(*serviceType, _("default@zoneedit.com"), (void*)4, 0);
	new eListBoxEntryText(*serviceType, _("default@no-ip.com"), (void*)5, 0);
	serviceType->setHelpText(_("Set dyndns system type"));
	if (iDyn == "dyndns@dyndns.org")
		serviceType->setCurrent(0,true);
	else if (iDyn == "statdns@dyndns.org")
		serviceType->setCurrent(1,true);
	else if (iDyn == "custom@dyndns.org")
		serviceType->setCurrent(2,true);
	else if (iDyn == "default@freedns.afraid.org")
		serviceType->setCurrent(3,true);
	else if (iDyn == "default@zoneedit.com")
		serviceType->setCurrent(4,true);
	else if (iDyn == "default@no-ip.com")
		serviceType->setCurrent(5,true);

	useLog = new eCheckbox(this,iUseLog,1);
	useLog->setText(_("Enable log file"));
	useLog->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	useLog->move(ePoint(5, posI += 35));
	useLog->resize(eSize(clientrect.width()- 20,30));
	useLog->setHelpText(_("Enable / Disable log file"));

	labelstatus = new eLabel(this);
	labelstatus->move(ePoint(5, posI += 40));
	labelstatus->resize(eSize(clientrect.width()- 20,30));
	labelstatus->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelstatus->setAlign(0);

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);
	int btnsize = (clientrect.width() -4) / 4;

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(2 , clientrect.height()-65));
	bt_exit->resize(eSize(btnsize, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setText(_("Close"));
	bt_exit->setHelpText(_("Close inadyn manager"));
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	CONNECT(bt_exit->selected, eWidget::reject);

	bt_save=new eButton(this);
	bt_save->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_save->move(ePoint(btnsize + 2, clientrect.height()-65));
	bt_save->resize(eSize(btnsize, 30));
	bt_save->setShortcut("green");
	bt_save->setShortcutPixmap("green");
	bt_save->setText(_("Save"));
	bt_save->setHelpText(_("Save parameter"));
	bt_save->setAlign(0);
	if (showBorder) bt_save->loadDeco();
	CONNECT(bt_save->selected, eInadyn::save);

	bt_startStop=new eButton(this);
	bt_startStop->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_startStop->move(ePoint(btnsize * 2 + 2, clientrect.height()-65));
	bt_startStop->resize(eSize(btnsize, 30));
	bt_startStop->setShortcut("yellow");
	bt_startStop->setShortcutPixmap("yellow");
	bt_startStop->setAlign(0);
	if (showBorder) bt_startStop->loadDeco();
	if (isStarted) 
	{	labelstatus->setText(_("Status Inadyn: started"));
		bt_startStop->setText(_("Stop"));
		bt_startStop->setHelpText(_("Stop Inadyn"));
	}else
	{	labelstatus->setText(_("Status Inadyn: stopped"));
		bt_startStop->setText(_("Start"));
		bt_startStop->setHelpText(_("Start inadyn"));
	}
	CONNECT(bt_startStop->selected, eInadyn::startStop);

	bt_log=new eButton(this);
	bt_log->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_log->move(ePoint(btnsize * 3 + 2, clientrect.height()-65));
	bt_log->resize(eSize(btnsize, 30));
	bt_log->setShortcut("blue");
	bt_log->setShortcutPixmap("blue");
	bt_log->setText(_("Log"));
	bt_log->setHelpText(_("Show log file"));
	bt_log->hide();
	bt_log->setAlign(0);
	if (showBorder) bt_log->loadDeco();
	if (isStarted && iUseLog) bt_log->show();
	CONNECT(bt_log->selected, eInadyn::showLog);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, clientrect.height() - 30) );
	sbar->resize(eSize(clientrect.width(), 30));
	sbar->loadDeco();	

}

eInadyn::~eInadyn()
{
	if (instance==this)
		instance=0;
}

void eInadyn::init()
{
	eInadyn dlg1;
	dlg1.show(); dlg1.exec(); dlg1.hide();
}

void eInadyn::startStop()
{
	if (isStarted) 
		eInadyn::stop();
	else
		eInadyn::start();
}

void eInadyn::stop()
{
	eMessageBox box(_("Stop inadyn in progress.\nPlease Wait..."), _("Running.."), 0);
	box.show();
		if (nemTool::getInstance()->exists("/var/log/inadyn.log",false)) unlink("/var/log/inadyn.log");
		system("touch /var/etc/.dont_start_inadyn");
		eString cmd = "killall -9 inadyn";
		nemTool::getInstance()->sendCmd((char *) cmd.c_str());
		isStarted=0;
		setParameter();
		usleep(500000);
	box.hide();
	labelstatus->setText(_("Status Inadyn: stopped"));
	bt_startStop->setText(_("Start"));
	bt_startStop->setHelpText(_("Start inadyn"));
	bt_log->hide();	
}

void eInadyn::start()
{
	isStarted=1;
	setParameter();
	if (nemTool::getInstance()->exists("/var/etc/inadynpar.conf",false))
	{
		eMessageBox box(_("Start Inadyn in progress.\nPlease Wait..."), _("Running.."), 0);
		box.show();
			unlink("/var/etc/.dont_start_inadyn");
			eString cmd = "/bin/inadyn --input_file /var/etc/inadynpar.conf &";
			nemTool::getInstance()->sendCmd((char *) cmd.c_str());
			usleep(500000);
		box.hide();
		labelstatus->setText(_("Status Inadyn: started"));
		bt_startStop->setText(_("Stop"));
		bt_startStop->setHelpText(_("Stop Inadyn"));
		if (iUseLog) 
			bt_log->show();	
	} else
		nemTool::getInstance()->msgfail(_("File '/var/etc/inadynpar.conf' don't exists.\nPlease save config first"));
}

void eInadyn::save()
{
	eMessageBox box(_("Save parameter in progress.\nPlease Wait..."), _("Running.."), 0);
	box.show();
		setParameter();
		usleep(500000);
	box.hide();
}

void eInadyn::showLog()
{
	hide();
		cbpShowFile::getInstance()->showLog(_("Show inaDyn log"),"/var/log/inadyn.log",60);
	show();
}
