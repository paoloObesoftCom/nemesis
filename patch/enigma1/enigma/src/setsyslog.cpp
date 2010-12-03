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
#include "setsyslog.h"
#include "showfile.h"
#include "nemtool.h"

eString readSysParameter()
{
	char buf[256];
	eString syslogPar;
	system("dos2unix /var/etc/syslog.par");
	FILE *in = fopen("/var/etc/syslog.par", "r");
	if(in)
	{	fgets(buf, 256, in);
		syslogPar=eString().sprintf("%s", buf);
		syslogPar=syslogPar.left(syslogPar.length() - 1);		
		fclose(in);
		return syslogPar;
	} else 
		return " -O /tmp/log/messages";
}

eSetSyslog::eSetSyslog()
:eWindow(0)
{
	int sW = 720;
	int sH = 576;
	int winW = 410;
	int winH = 390;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
    setText(_("Debugging Manager"));

	getParameter();

	label = new eLabel(this);
	label->move(ePoint(5, 20));
	label->resize(eSize(250, 30));
	label->setText(_("Syslog message folder:"));
	label->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label->setAlign(1);

	syslogdir=new eComboBox(this, 4);
	syslogdir->move(ePoint(260, 15));
	syslogdir->resize(eSize(145, 30));
	syslogdir->loadDeco();
	syslogdir->setHelpText(_("Set syslog messages folder path"));
	new eListBoxEntryText(*syslogdir, _("/tmp/log"), (void*)0, 0);
	new eListBoxEntryText(*syslogdir, _("/var/log"), (void*)1, 0);
	new eListBoxEntryText(*syslogdir, _("/usb/log"), (void*)2, 0);
	new eListBoxEntryText(*syslogdir, _("/hdd/log"), (void*)3, 0);
	if (logpath == "/tmp/log")
		syslogdir->setCurrent(0,true);
	else if (logpath == "/var/log")
		syslogdir->setCurrent(1,true);
	else if (logpath == "/usb/log")
		syslogdir->setCurrent(2,true);
	else if (logpath == "/hdd/log")
		syslogdir->setCurrent(3,true);

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_start=new eButton(this);
	bt_start->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_start->move(ePoint(5, 60));
	bt_start->resize(eSize(100, 30));
	bt_start->setShortcut("yellow");
	bt_start->setShortcutPixmap("yellow");
	bt_start->setHelpText(_("Start / Stop Syslogd"));
	bt_start->setAlign(0);
	if (showBorder) bt_start->loadDeco();
	CONNECT(bt_start->selected, eSetSyslog::startSyslog);

	labelstatus = new eLabel(this);
	labelstatus->move(ePoint(110, 65));
	labelstatus->resize(eSize(295, 30));
	labelstatus->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelstatus->setAlign(0);

	if (nemTool::getInstance()->exists("/tmp/.syslogstarted",false))
	{	labelstatus->setText(_("Status Syslogd: started"));
		bt_start->setText(_("Stop"));
	}else
	{	labelstatus->setText(_("Status Syslogd: stopped"));
		bt_start->setText(_("Start"));
	}

    sysenigma = new eCheckbox(this);
    sysenigma->move(ePoint(5,100));
    sysenigma->resize(eSize(295,30));
	if (nemTool::getInstance()->exists("/var/etc/.syslogenigma",false))
    	sysenigma->setCheck(1);
    sysenigma->loadDeco();
    sysenigma->setText(_("Log Enigma to Syslog"));
    sysenigma->setHelpText(_("Log Enigma debug messages to Syslog"));
    CONNECT(sysenigma->checked, eSetSyslog::sysenigmaChanged);

    sysboot = new eCheckbox(this);
    sysboot->move(ePoint(5,135));
    sysboot->resize(eSize(295,30));
	if (!nemTool::getInstance()->exists("/var/etc/.dont_start_syslog",false))
	    sysboot->setCheck(1);
    sysboot->loadDeco();
    sysboot->setText(_("Enable Syslog at boot"));
    sysboot->setHelpText(_("Enable Syslog daemon at boot"));
    CONNECT(sysboot->checked, eSetSyslog::sysbootChanged);

    sysmark = new eCheckbox(this,usemark,1);
    sysmark->move(ePoint(5,170));
    sysmark->resize(eSize(295,30));
    sysmark->loadDeco();
    sysmark->setText(_("Enable MARK lines"));
    sysmark->setHelpText(_("Enable MARK lines"));
    CONNECT(sysmark->checked, eSetSyslog::sysmarkChanged);

	input[0]=new eTextInputField(this);
	input[0]->move(ePoint(305,170));
	input[0]->resize(eSize(50, 30));
	input[0]->setText(marktime.c_str());
	input[0]->setUseableChars("1234567890");
	input[0]->setHelpText(_("Enter MARK time in MIN (0 = off)"));
	input[0]->loadDeco();

	labelmin = new eLabel(this);
	labelmin->move(ePoint(360, 175));
	labelmin->resize(eSize(45, 30));
	labelmin->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelmin->setText("Min");
	labelmin->setAlign(0);
	if (!sysmark->isChecked()) 
	{	input[0]->hide();
		labelmin->hide();
	}

    sysrotate = new eCheckbox(this,userotate,1);
    sysrotate->move(ePoint(5,205));
    sysrotate->resize(eSize(295,30));
    sysrotate->loadDeco();
    sysrotate->setText(_("Enable log rotate"));
    sysrotate->setHelpText(_("Log to a circular buffer"));
    CONNECT(sysrotate->checked, eSetSyslog::sysrotateChanged);

	input[1]=new eTextInputField(this);
	input[1]->move(ePoint(305,205));
	input[1]->resize(eSize(50, 30));
	input[1]->setText(logsize.c_str());
	input[1]->setUseableChars("1234567890");
	input[1]->setHelpText(_("Enter log size in Kb"));
	input[1]->loadDeco();

	labelkb = new eLabel(this);
	labelkb->move(ePoint(360, 210));
	labelkb->resize(eSize(45, 30));
	labelkb->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelkb->setText("Kb");
	labelkb->setAlign(0);
	if (!sysrotate->isChecked()) 
	{	input[1]->hide();
		labelkb->hide();
	}

    sysremote = new eCheckbox(this,useremote,1);
    sysremote->move(ePoint(5,240));
    sysremote->resize(eSize(295,30));
    sysremote->loadDeco();
    sysremote->setText(_("Enable Remote Syslog"));
    sysremote->setHelpText(_("Enable Remote Syslog"));
    CONNECT(sysremote->checked, eSetSyslog::sysremoteChanged);

	__u32 sip=ntohl(0x0a000061); int de[4]; int tmp[4];
	if ( sscanf(remoteip.c_str(), "%d.%d.%d.%d", tmp, tmp+1, tmp+2, tmp+3) == 4 )
		sip = tmp[0]<<24 | tmp[1] << 16 | tmp[2] << 8 | tmp[3];
	else
		eDebug("couldn't parse ip(%s)", remoteip.length()?remoteip.c_str():"");

	l1=new eLabel(this);
	l1->setText("IP:");
	l1->setAlign(1);
	l1->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	l1->move(ePoint(5, 280));
	l1->resize(eSize(45, 30));

	eNumber::unpack(sip, de);
	ip=new eNumber(this, 4, 0, 255, 3, de, 0, l1, 1);
	ip->move(ePoint(55, 275));
	ip->resize(eSize(170, 30));
	ip->setFlags(eNumber::flagDrawPoints);
	ip->setHelpText(_("Enter remote IP Address (0..9, <, >)"));
	ip->loadDeco();
	CONNECT(ip->selected, eSetSyslog::fieldSelected);

	l2 = new eLabel(this);
	l2->setText(_("Port:"));
	l2->setAlign(1);
	l2->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	l2->move(ePoint(230, 280));
	l2->resize(eSize(65, 30));

	port=new eNumber(this, 1, 0, 65536, 5, 0, 0, l2, 1);
	port->move(ePoint(305, 275));
	port->resize(eSize(100, 30));
	port->setFlags(eNumber::flagDrawPoints);
	port->setHelpText(_("Enter remote port (default 514/UDP)"));
	port->setNumber(remoteport);
	port->loadDeco();
	CONNECT(port->selected, eSetSyslog::fieldSelected);
	if (!useremote)
	{	l1->hide(); l2->hide();
		ip->hide(); port->hide();
	}

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(5, winH - 75));
	bt_exit->resize(eSize(135, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setText(_("Close"));
	bt_exit->setHelpText(_("Close Debbugging"));
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	CONNECT(bt_exit->selected, eWidget::reject);

	bt_open=new eButton(this);
	bt_open->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_open->move(ePoint(140, winH - 75));
	bt_open->resize(eSize(135, 30));
	bt_open->setShortcut("green");
	bt_open->setShortcutPixmap("green");
	bt_open->setText(_("Show log"));
	bt_open->setHelpText(_("Show Syslog events"));
	bt_open->setAlign(0);
	if (showBorder) bt_open->loadDeco();
	CONNECT(bt_open->selected, eSetSyslog::openLog);

	bt_reboot=new eButton(this);
	bt_reboot->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_reboot->move(ePoint(275, winH - 75));
	bt_reboot->resize(eSize(130, 30));
	bt_reboot->setShortcut("blue");
	bt_reboot->setShortcutPixmap("blue");
	bt_reboot->setText(_("Delete"));
	bt_reboot->setHelpText(_("Delete Syslog events file"));
	bt_reboot->setAlign(0);
	if (showBorder) bt_reboot->loadDeco();
	CONNECT(bt_reboot->selected, eSetSyslog::deleteLog);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, winH - 40) );
	sbar->resize(eSize(winW, 40));
	sbar->loadDeco();	
	setFocus(syslogdir);
}

eSetSyslog::~eSetSyslog()
{
	setParameter();
}

void eSetSyslog::fieldSelected(int *number)
{
	focusNext(eWidget::focusDirNext);
}

void eSetSyslog::getParameter()
{
	userotate = 0;
	logsize = "32";
	usemark = 0;
	marktime = "20";
	useremote = 0;
	remoteip = "192.168.0.1";
	remoteport = 514;
	logpath = "/tmp/log";
	char *fileName = "/var/etc/syslog.config";

	eString ePar;
	char buf[256];
	FILE *in = fopen(fileName, "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			ePar = eString().sprintf("%s", buf);
			ePar = ePar.left(ePar.length() - 1);		
			if (ePar.left(10) == "userotate=") userotate = atoi(ePar.right(ePar.length() -10).c_str());
			if (ePar.left(8) == "logsize=") logsize = ePar.right(ePar.length() -8);
			if (ePar.left(8) == "usemark=") usemark = atoi(ePar.right(ePar.length() -8).c_str());
			if (ePar.left(9) == "marktime=") marktime = ePar.right(ePar.length() -9);
			if (ePar.left(10) == "useremote=") useremote = atoi(ePar.right(ePar.length() -10).c_str());
			if (ePar.left(9) == "remoteip=") remoteip = ePar.right(ePar.length() -9);
			if (ePar.left(11) == "remoteport=") remoteport = atoi(ePar.right(ePar.length() -11).c_str());
			if (ePar.left(8) == "logpath=") logpath = ePar.right(ePar.length() -8);
		}
		fclose(in);
	}
}

void eSetSyslog::setParameter()
{
	usemark=0;
	if (sysmark->isChecked())
		usemark=1;
	userotate=0;
	if (sysrotate->isChecked())
		userotate=1;
	useremote=0;
	if (sysremote->isChecked())
		useremote=1;

	int eIP[4];
	for (int i=0; i<4; i++)
		eIP[i]=ip->getNumber(i);

	system(eString("mkdir " + syslogdir->getCurrent()->getText()).c_str());
	
	marktime = input[0]->getText().c_str();
	logsize = input[1]->getText().c_str();
	remoteip.sprintf("%d.%d.%d.%d", eIP[0], eIP[1], eIP[2], eIP[3]);
	remoteport = port->getNumber();

	logpath = syslogdir->getCurrent()->getText().c_str();

	char *fileName = "/var/etc/syslog.config";
	char *fileParam = "/var/etc/syslog.par";
	eString ePar;
	if (nemTool::getInstance()->exists(fileName,false)) unlink(fileName);

	FILE *in = fopen(fileName, "w");
	if(in)
	{	
		fprintf (in, "userotate=%i\n", userotate);
		fprintf (in, "logsize=%s\n", logsize.c_str());
		fprintf (in, "usemark=%i\n", usemark);
		fprintf (in, "marktime=%s\n", marktime.c_str());
		fprintf (in, "useremote=%i\n", useremote);
		fprintf (in, "remoteip=%s\n", remoteip.c_str());
		fprintf (in, "remoteport=%i\n", remoteport);
		fprintf (in, "logpath=%s\n", logpath.c_str());
		fclose(in);
	}

	if (nemTool::getInstance()->exists(fileParam,false))
		unlink(fileParam);

	eString parameter = eString().sprintf(" -O %s/messages",logpath.c_str()).c_str(); 
	if (userotate)
		parameter += eString().sprintf(" -C %s", logsize.c_str()).c_str();
	if (usemark)
		parameter += eString().sprintf(" -m %s", marktime.c_str()).c_str();
	if (useremote)
		parameter += eString().sprintf(" -L -R %s:%i", remoteip.c_str(),remoteport).c_str();

	FILE *in1 = fopen(fileParam, "w");
	if(in1)
	{	
		fprintf (in1, "%s\n", parameter.c_str());
		fclose(in1);
	}
}

void eSetSyslog::sysenigmaChanged( int checked )
{
	if (checked)
	{	system("touch /var/etc/.syslogenigma");
		enableOutputSyslog = 1;
	}else
	{	system("rm -f /var/etc/.syslogenigma");
		enableOutputSyslog = 0;
	}
}

void eSetSyslog::sysmarkChanged( int checked )
{
	if (nemTool::getInstance()->exists("/tmp/.syslogstarted",false))
	{	sysmark->setCheck(!checked);
		nemTool::getInstance()->msgfail(_("Syslogd is running!\nPlease stop it before change setting."));
		return;
	}
	else
	{	if (checked)
		{	input[0]->show();
			labelmin->show();
		}else
		{	input[0]->hide();
			labelmin->hide();
		}
	}
}

void eSetSyslog::sysrotateChanged( int checked )
{
	if (nemTool::getInstance()->exists("/tmp/.syslogstarted",false))
	{	sysrotate->setCheck(!checked);
		nemTool::getInstance()->msgfail(_("Syslogd is running!\nPlease stop it before change setting."));
		return;
	}
	else
	{	if (checked)
		{	input[1]->show();
			labelkb->show();
		}else
		{	input[1]->hide();
			labelkb->hide();
		}
	}
}

void eSetSyslog::sysremoteChanged( int checked )
{
	if (nemTool::getInstance()->exists("/tmp/.syslogstarted",false))
	{	sysremote->setCheck(!checked);
		nemTool::getInstance()->msgfail(_("Syslogd is running!\nPlease stop it before change setting."));
		return;
	} else
	{	
		if (checked)
		{	l1->show(); l2->show();
			ip->show(); port->show();
		}else
		{	l1->hide(); l2->hide();
			ip->hide(); port->hide();
		}
	}
}

void eSetSyslog::sysbootChanged( int checked )
{
	if (checked)
	{	system("rm -f /var/etc/.dont_start_syslog");
		rebootDb();
	}
	else
		system("touch /var/etc/.dont_start_syslog");
}


void eSetSyslog::openLog()
{
	eString sysLogFile = syslogdir->getCurrent()->getText().c_str();
	eString errMsg = eString().sprintf(_("Syslog file '%s' not found!"),sysLogFile.c_str());
	sysLogFile += "/messages";
	if (nemTool::getInstance()->exists((char*) sysLogFile.c_str(),false))
	{	hide();
			cbpShowFile::getInstance()->showLog(_("Show Syslog events"),(char*) sysLogFile.c_str(),60);
		show();
	}else
			nemTool::getInstance()->msgfail(errMsg);
}

void eSetSyslog::deleteLog()
{
	eString sysLogFile = syslogdir->getCurrent()->getText().c_str();
	sysLogFile += "/messages";
	eString logMsg = eString().sprintf(_("Do you want delete Syslog file:\n'%s'?"),sysLogFile.c_str());
	eString errMsg = eString().sprintf(_("Syslog file '%s' not found!"),sysLogFile.c_str());
	eMessageBox msg(logMsg, _("Delete"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
	msg.show(); int button=msg.exec(); msg.hide();
	if (button == eMessageBox::btYes)
	{	if (nemTool::getInstance()->exists((char*) sysLogFile.c_str(),false))
			system(eString("rm -f  " + sysLogFile).c_str());
		else
			nemTool::getInstance()->msgfail(errMsg);
	}
}

void eSetSyslog::startSyslog()
{
	if (nemTool::getInstance()->exists("/tmp/.syslogstarted",false))
	{	system("killall -9 syslogd");
		system("rm -f /tmp/.syslogstarted");
		labelstatus->setText(_("Status Syslogd: stopped"));
		bt_start->setText(_("Start"));
	}else
	{	setParameter();
		eString cmd = "/sbin/syslogd" + readSysParameter();
		nemTool::getInstance()->sendCmd((char *) cmd.c_str());
		system("touch /tmp/.syslogstarted");
		bt_start->setText(_("Stop"));
		labelstatus->setText(_("Status Syslogd: started"));
	}
}

void eSetSyslog::rebootDb()
{
	setParameter();
	eMessageBox msg(_("Do you want reboot the dreambox now?"), 
                    _("Reboot dreambox"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
	msg.show(); 
	int button=msg.exec(); 
	msg.hide();
	if (button == eMessageBox::btYes)
		eZap::getInstance()->quit(4);
}
