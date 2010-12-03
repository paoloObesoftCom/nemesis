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
#include "setiptable.h"
#include "showfile.h"
#include "nemtool.h"

void eSetIptableSetup::loadUserList()
{
	userlistbox->beginAtomic(); 
	userlistbox->clearList();

	char buf[256];
	eString ePar;
	system("dos2unix /var/etc/firewall.users");
	FILE *in = fopen("/var/etc/firewall.users", "r");
	int iUserNum = 0;
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			ePar = eString().sprintf("%s", buf);
			ePar = ePar.left(ePar.length() - 1);		
			new eListBoxEntryText(userlistbox, ePar, (void*) iUserNum);
			eUserIp[iUserNum] = ePar;
			iUserNum++;
		}
		fclose(in);
	}else
		new eListBoxEntryText(userlistbox, "None", (void*) 0);

	userlistbox->endAtomic();
}

eSetIptable::eSetIptable()
:eWindow(0)
{
	int sW = 720;
	int sH = 576;
	int winW = 410;
	int winH = 220;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
    setText(_("Firewall Manager"));

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_start=new eButton(this);
	bt_start->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_start->move(ePoint(5, 15));
	bt_start->resize(eSize(100, 30));
	bt_start->setShortcut("yellow");
	bt_start->setShortcutPixmap("yellow");
	bt_start->setHelpText(_("Start / Stop Firewall"));
	bt_start->setAlign(0);
	if (showBorder) bt_start->loadDeco();
	CONNECT(bt_start->selected, eSetIptable::startIptable);

	labelstatus = new eLabel(this);
	labelstatus->move(ePoint(110, 20));
	labelstatus->resize(eSize(295, 30));
	labelstatus->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelstatus->setAlign(0);

	if (nemTool::getInstance()->exists("/tmp/.firewallstarted",false))
	{	labelstatus->setText(_("Status Firewall: started"));
		bt_start->setText(_("Stop"));
	}else
	{	labelstatus->setText(_("Status Firewall: stopped"));
		bt_start->setText(_("Start"));
	}

	enalan = new eCheckbox(this);
	enalan->move(ePoint(5,60));
	enalan->resize(eSize(400,30));
	if (!nemTool::getInstance()->exists("/var/etc/.dont_ena_lan",false))
		enalan->setCheck(1);
	enalan->loadDeco();
	enalan->setText(_("Permit Input LAN Traffic"));
	enalan->setHelpText(_("Permit traffic to dreambox from LAN"));
	CONNECT(enalan->checked, eSetIptable::enalanChanged);

	sysboot = new eCheckbox(this);
	sysboot->move(ePoint(5,100));
	sysboot->resize(eSize(295,30));
	if (!nemTool::getInstance()->exists("/var/etc/.dont_start_firewall",false))
		sysboot->setCheck(1);
	sysboot->loadDeco();
	sysboot->setText(_("Enable Firewall at boot"));
	sysboot->setHelpText(_("Enable Firewall at boot"));
	CONNECT(sysboot->checked, eSetIptable::sysbootChanged);

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(5, winH - 75));
	bt_exit->resize(eSize(125, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setText(_("Close"));
	bt_exit->setHelpText(_("Close Debbugging"));
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	CONNECT(bt_exit->selected, eWidget::reject);

	bt_status=new eButton(this);
	bt_status->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_status->move(ePoint(130, winH - 75));
	bt_status->resize(eSize(125, 30));
	bt_status->setShortcut("green");
	bt_status->setShortcutPixmap("green");
	bt_status->setText(_("Status"));
	bt_status->setHelpText(_("Show Firewall status"));
	bt_status->setAlign(0);
	if (showBorder) bt_status->loadDeco();
	CONNECT(bt_status->selected, eSetIptable::showStatus);

	bt_setting=new eButton(this);
	bt_setting->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_setting->move(ePoint(255, winH - 75));
	bt_setting->resize(eSize(150, 30));
	bt_setting->setShortcut("blue");
	bt_setting->setShortcutPixmap("blue");
	bt_setting->setText(_("Setup"));
	bt_setting->setHelpText(_("Access setup"));
	bt_setting->setAlign(0);
	if (showBorder) bt_setting->loadDeco();
	CONNECT(bt_setting->selected, eSetIptable::openSetting);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, winH - 40) );
	sbar->resize(eSize(winW, 40));
	sbar->loadDeco();	
}

eSetIptable::~eSetIptable()
{
}

void eSetIptable::enalanChanged( int checked )
{
	if (nemTool::getInstance()->exists("/tmp/.firewallstarted",false))
	{	enalan->setCheck(!checked);
		nemTool::getInstance()->msgfail(_("Firewall is running!\nPlease stop it before change setting."));
		return;
	}
	if (checked)
			system("rm -f /var/etc/.dont_ena_lan");
		else
			system("touch /var/etc/.dont_ena_lan");
}

void eSetIptable::sysbootChanged( int checked )
{
	if (checked)
	{	system("rm -f /var/etc/.dont_start_firewall");
		rebootDb();
	}
	else
		system("touch /var/etc/.dont_start_firewall");
}

void eSetIptable::startIptable()
{
	if (nemTool::getInstance()->exists("/tmp/.firewallstarted",false))
	{	eMessageBox box(_("Starting Firewall.\nPlease Wait..."), _("Running.."), 0);
		box.show();
			eString cmd = "/var/script/firewall.sh stop";
			nemTool::getInstance()->sendCmd((char *) cmd.c_str());
			system("rm -f /tmp/.firewallstarted");
			usleep(500000);
		box.hide();
		labelstatus->setText(_("Status Firewall: stopped"));
		bt_start->setText(_("Start"));
	}else
	{	eMessageBox box1(_("Starting Firewall.\nPlease Wait..."), _("Stopping.."), 0);
		box1.show();
			eString cmd = "/var/script/firewall.sh start";
			nemTool::getInstance()->sendCmd((char *) cmd.c_str());
			system("touch /tmp/.firewallstarted");
			usleep(500000);
		box1.hide();
		labelstatus->setText(_("Status Firewall: started"));
		bt_start->setText(_("Stop"));
	}
}

void eSetIptable::openSetting()
{
	hide();
		eSetIptableSetup dlg;
		dlg.show();
		dlg.exec();
		dlg.hide();
	show();
}

void eSetIptable::showStatus()
{
	eString cmd = "/var/script/firewall.sh status > /tmp/status.log";
	nemTool::getInstance()->sendCmd((char *) cmd.c_str());
	cbpShowFile::getInstance()->showLog(_("Show Firewall Status"),"/tmp/status.log",0);
}

void eSetIptable::rebootDb()
{
	eMessageBox msg(_("Do you want reboot the dreambox now?"), 
                    _("Reboot dreambox"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
	msg.show(); 
	int button=msg.exec(); 
	msg.hide();
	if (button == eMessageBox::btYes)
		eZap::getInstance()->quit(4);
}

eSetIptableSetup::eSetIptableSetup()
:eWindow(0)
{
	int sW = 720;
	int sH = 576;
	int winW = 410;
	int winH = 320;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
    setText(_("Access setup"));
	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	userlistbox= new eListBox< eListBoxEntryText >(this);
	userlistbox->setFlags(eListBoxBase::flagNoPageMovement);
	userlistbox->setName("userlistbox");
	userlistbox->move(ePoint(5, 5));
	userlistbox->resize(eSize (400, 275));
	userlistbox->loadDeco();

	bt_exit=new eButton(this,0,0);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(5, winH - 35));
	bt_exit->resize(eSize(135, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setText(_("Close"));
	bt_exit->setHelpText(_("Close"));
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	CONNECT(bt_exit->selected, eWidget::reject);

	bt_add=new eButton(this,0,0);
	bt_add->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_add->move(ePoint(140, winH - 35));
	bt_add->resize(eSize(135, 30));
	bt_add->setShortcut("yellow");
	bt_add->setShortcutPixmap("yellow");
	bt_add->setText(_("Add"));
	bt_add->setHelpText(_("Add user"));
	bt_add->setAlign(0);
	if (showBorder) bt_add->loadDeco();
	CONNECT(bt_add->selected, eSetIptableSetup::addUser);

	bt_delete=new eButton(this,0,0);
	bt_delete->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_delete->move(ePoint(275, winH - 35));
	bt_delete->resize(eSize(130, 30));
	bt_delete->setShortcut("blue");
	bt_delete->setShortcutPixmap("blue");
	bt_delete->setText(_("Delete"));
	bt_delete->setHelpText(_("Delete User"));
	bt_delete->setAlign(0);
	if (showBorder) bt_delete->loadDeco();
	CONNECT(bt_delete->selected, eSetIptableSetup::deleteUser);
	loadUserList();
}

eSetIptableSetup::~eSetIptableSetup()
{
}

void eSetIptableSetup::deleteUser()
{
	if (nemTool::getInstance()->exists("/tmp/.firewallstarted",false))
	{	nemTool::getInstance()->msgfail(_("Firewall is running!\nPlease stop it before change setting."));
		return;
	}
	if (userlistbox->getCount())
	{	eString userToRemove = userlistbox->getCurrent()->getText();
		eMessageBox msg(eString().sprintf(_("Do you want delete the access:\n'%s'?"), userToRemove.c_str()).c_str(), 
						_("Delete"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
		msg.show(); int button=msg.exec(); msg.hide();
		if (button == eMessageBox::btYes)
		{	eMessageBox eMessage(eString().sprintf(_("Remove access entry:'%s'\nPlease Wait..."), userToRemove.c_str()), 
								_("Running.."), 0);
			eMessage.show();
				char buf[512];
				eString ePar;
				system("touch /tmp/firewall.users.tmp");
				FILE *in = fopen("/var/etc/firewall.users", "r");
				if(in)
				{	while (fgets(buf, 512, in))
					{	ePar = eString().sprintf("%s", buf);
						ePar = ePar.left(ePar.length() - 1);		
						if (ePar != userToRemove)
							system(eString().sprintf("echo '%s' >> /tmp/firewall.users.tmp",ePar.c_str()).c_str());
					}
					fclose(in);
					system(eString("cp /tmp/firewall.users.tmp /var/etc/firewall.users").c_str());
					unlink("/tmp/firewall.users.tmp");
				}
				usleep(500000);
			eMessage.hide();
			loadUserList();
		}
	}
}

void eSetIptableSetup::addUser()
{
	if (nemTool::getInstance()->exists("/tmp/.firewallstarted",false))
	{	nemTool::getInstance()->msgfail(_("Firewall is running!\nPlease stop it before change setting."));
		return;
	}
	hide();
		eSetIptableAdd dlg;
		dlg.show();
		dlg.exec();
		dlg.hide();
	show();
	loadUserList();
}

eSetIptableAdd::eSetIptableAdd()
:eWindow(0)
{
	int sW = 720;
	int sH = 576;
	int winW = 370;
	int winH = 240;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));

    setText(_("Add Access"));
	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	useip = new eCheckbox(this,1,1);
	useip->move(ePoint(5,10));
	useip->resize(eSize(360,30));
	useip->loadDeco();
	useip->setText(_("Use IP address"));
	useip->setHelpText(_("Use IP address"));
	CONNECT(useip->checked, eSetIptableAdd::useipChanged);

	usename = new eCheckbox(this,0,1);
	usename->move(ePoint(5,45));
	usename->resize(eSize(360,30));
	usename->loadDeco();
	usename->setText(_("Use Domain name"));
	usename->setHelpText(_("Use Domain name"));
	CONNECT(usename->checked, eSetIptableAdd::usenameChanged);

	labelip = new eLabel(this);
	labelip->move(ePoint(5, 90));
	labelip->resize(eSize(125, 30));
	labelip->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelip->setText("IP Address:");
	labelip->setAlign(1);

	__u32 sip=ntohl(0x0a000061); int de[4];

	eNumber::unpack(sip, de);
	ip=new eNumber(this, 4, 0, 255, 3, de, 0, labelip, 1);
	ip->move(ePoint(135,85));
	ip->resize(eSize(170, 30));
	ip->setFlags(eNumber::flagDrawPoints);
	ip->setHelpText(_("Enter remote IP Address (0..9, <, >)"));
	ip->loadDeco();
	CONNECT(ip->selected, eSetIptableAdd::fieldSelected);

	input[0]=new eTextInputField(this);
	input[0]->move(ePoint(135,85));
	input[0]->resize(eSize(230, 30));
	input[0]->loadDeco();
	input[0]->setUseableChars("abcdefghijklmnopqrstuvwxyz1234567890-_./");
	input[0]->setHelpText(_("Enter a domain name (Ex: www.google.com)"));
	input[0]->hide();

	labelcomment = new eLabel(this);
	labelcomment->move(ePoint(5, 130));
	labelcomment->resize(eSize(125, 30));
	labelcomment->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	labelcomment->setText(_("Comment:"));
	labelcomment->setAlign(1);

	input[1]=new eTextInputField(this);
	input[1]->move(ePoint(135,125));
	input[1]->resize(eSize(230, 30));
	input[1]->setHelpText(_("Enter a comment (Def = FW Rule)"));
	input[1]->loadDeco();

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
	bt_add->move(ePoint(215, winH - 75));
	bt_add->resize(eSize(150, 30));
	bt_add->setShortcut("blue");
	bt_add->setShortcutPixmap("blue");
	bt_add->setText(_("Add"));
	bt_add->setHelpText(_("Add user"));
	bt_add->setAlign(0);
	if (showBorder) bt_add->loadDeco();
	CONNECT(bt_add->selected, eSetIptableAdd::addUser);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, winH - 40) );
	sbar->resize(eSize(winW, 40));
	sbar->loadDeco();	

}

eSetIptableAdd::~eSetIptableAdd()
{
}

void eSetIptableAdd::fieldSelected(int *number)
{
	focusNext(eWidget::focusDirNext);
}

void eSetIptableAdd::useipChanged( int checked )
{
	if (checked)
	{	usename->setCheck(0);
		labelip->setText(_("IP Address:"));
		input[0]->hide();
		ip->show();
	}else
	{	usename->setCheck(1);
		labelip->setText(_("Domain name:"));
		input[0]->show();
		ip->hide();
	}
}

void eSetIptableAdd::usenameChanged( int checked )
{
	if (checked)
	{	useip->setCheck(0);
		labelip->setText(_("Domain name:"));
		input[0]->show();
		ip->hide();
	}else
	{	useip->setCheck(1);
		labelip->setText(_("IP Address:"));
		input[0]->hide();
		ip->show();
	}
}

void eSetIptableAdd::addUser()
{
	eString cmd;
	eString comment = "FW Rule";
	eString parameter;
	if (useip->isChecked())
	{	if (input[1]->getText()) comment = input[1]->getText().c_str();
		int eIP[4];
		for (int i=0; i<4; i++)
			eIP[i]=ip->getNumber(i);
		parameter.sprintf("%d.%d.%d.%d # %s", eIP[0], eIP[1], eIP[2], eIP[3],comment.c_str());
		cmd.sprintf("echo '%s' >> /var/etc/firewall.users",parameter.c_str());
		nemTool::getInstance()->sendCmd((char *) cmd.c_str());
	} else if (input[0]->getText())
	{	if (input[1]->getText())
			comment = input[1]->getText().c_str();
		eString domain = input[0]->getText().c_str();
		parameter.sprintf("%s # %s", domain.c_str(),comment.c_str());
		cmd.sprintf("echo '%s # %s' >> /var/etc/firewall.users",domain.c_str(),comment.c_str());
		nemTool::getInstance()->sendCmd((char *) cmd.c_str());
	} else
	{	setFocus(input[0]);
		nemTool::getInstance()->msgfail(_("You do enter a valid domain name!"));
		return;
	}
	if (checkRule(parameter))
		nemTool::getInstance()->msgok(eString().sprintf(_("New Access '%s'\nadded succefully!"),parameter.c_str()));
	else
		nemTool::getInstance()->msgfail(eString().sprintf(_("Access '%s'\nnot added!"),parameter.c_str()));
}

bool eSetIptableAdd::checkRule(eString rule)
{
	bool ret = false;
	char buf[512];
	eString ePar;
	FILE *in = fopen("/var/etc/firewall.users", "r");
	if(in)
	{	while (fgets(buf, 512, in))
		{	ePar = eString().sprintf("%s", buf);
			ePar.removeChars('\n');
			if (ePar == rule)
				ret = true;
		}
		fclose(in);
	}
	return ret;
}
