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
#include "userscript.h"
#include "showfile.h"
#include "nemtool.h"
#include "escriptexecute.h"


eString eScriptWindow::getScriptParameter(eString script,eString par)
{
	char buf[256];
	eString fileName, ePar;
	par += "=";
	fileName = eString("/var/bin/" + script + "_user.sh").c_str();
		
	FILE *in = fopen(fileName.c_str(), "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			ePar = eString().sprintf("%s", buf);
			ePar=ePar.left(ePar.length() - 1);		
			if (ePar.left(par.length()) == par)
			{	fclose(in);
				return ePar.right(ePar.length() - par.length());
			}
		}
		fclose(in);
	}
	return "nf";
}

void eScriptWindow::readScriptParameter(eString script)
{
	useCccd = 0; confirmExec = 0; showLog = 1;
	char buf[256];
	eString fileName, ePar;
	fileName = eString("/var/bin/" + script + "_user.sh").c_str();
		
	FILE *in = fopen(fileName.c_str(), "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			ePar = eString().sprintf("%s", buf);
			ePar=ePar.left(ePar.length() - 1);		
			if (ePar.left(8) == "usecccd=")
				if (ePar.right(ePar.length() - 8) == "true") useCccd = 1;
				else useCccd = 0;
			if (ePar.left(8) == "confirm=")
				if (ePar.right(ePar.length() - 8) == "true") confirmExec = 1;
				else confirmExec = 0;
			if (ePar.left(8) == "showlog=")
				if (ePar.right(ePar.length() - 8) == "true") showLog = 1;
				else showLog = 0;
		}
		fclose(in);
	}
}

bool eScriptWindow::loadScriptList()
{
	bool empty=true;
	char scripttoexecute[100];
	scriptList->beginAtomic(); 
	scriptList->clearList();
	eString scriptName;
	int entry=0;
	DIR *d = opendir("/var/bin");
	while (struct dirent *e=readdir(d))
	{
		if ( strstr(e->d_name,"_user.sh")) 
		{	memset(scripttoexecute, 0, sizeof(scripttoexecute));
			strncpy(scripttoexecute, e->d_name, strlen(e->d_name) - 8);
			scriptName = getScriptParameter(scripttoexecute,"name");
			scriptName = nemTool::getInstance()->replace(scriptName,"'","");
			//scriptName = replace(scriptName,"""","");
			if (scriptName == "nf") new eListBoxEntryText(scriptList, scripttoexecute, (void*)++entry);
			else new eListBoxEntryText(scriptList, scriptName, (void*)++entry);
			scriptlist[entry]=scripttoexecute;
			empty = false;
		}
	}
	closedir(d);
	if (scriptList->getCount())
		scriptList->sort();
	scriptList->endAtomic();
	return empty;
}

void eScriptWindow::loadButton(bool empty)
{
	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);
	if (empty)
	{	
		scriptList->hide();
		label = new eLabel(this);
		label->setFlags(RS_WRAP);
   		label->setText(_("No Users Scripts Found!\n\nYou can place xxxxx_user.sh /var/bin"));
   		label->move(ePoint(5, 5));
   		label->resize(eSize(clientrect.width()-10, clientrect.height()-85));
		bt_abort=new eButton(this);
		bt_abort->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		bt_abort->move(ePoint(clientrect.width() - 115 , clientrect.height()-80));
		bt_abort->resize(eSize(110, 30));
		bt_abort->setShortcut("red");
		bt_abort->setShortcutPixmap("red");
		bt_abort->setText(_("Close"));
		bt_abort->setHelpText(_("Exit user scripts"));
		bt_abort->setAlign(0);
		if (showBorder) bt_abort->loadDeco();
		CONNECT(bt_abort->selected, eWidget::reject);
	} else
	{
		system("chmod 755 /var/bin/*_user.sh");
		usecccd = new eCheckbox(this,0,1);
		usecccd->setText(_("Run in kernel mode"));
		usecccd->setHelpText(_("Run script in linux mode"));
		usecccd->move(ePoint(5, clientrect.height() - 180));
		usecccd->resize(eSize(clientrect.width() - 10,30));
//		CONNECT(usecccd->checked, eScriptWindow::usecccdStateChanged);

		confirmexec = new eCheckbox(this,0,1);
		confirmexec->setText(_("Confirm execution"));
		confirmexec->setHelpText(_("Confirm script execution"));
		confirmexec->move(ePoint(5, clientrect.height() - 145));
		confirmexec->resize(eSize(clientrect.width() - 10,30));

		showlog = new eCheckbox(this,1,1);
		showlog->setText(_("Show output window"));
		showlog->setHelpText(_("Show result output window"));
		showlog->move(ePoint(5, clientrect.height() - 110));
		showlog->resize(eSize(clientrect.width() - 10,30));
	
		int showBorder = 0;
		eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);
		int btnsize = (clientrect.width() - 4) / 4;

		bt_exit=new eButton(this);
		bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		bt_exit->move(ePoint(2, clientrect.height() - 75));
		bt_exit->setShortcut("red");
		bt_exit->setShortcutPixmap("red");
		bt_exit->resize(eSize(btnsize, 30));
		bt_exit->setText(_("Close"));
		bt_exit->setAlign(0);
		if (showBorder) bt_exit->loadDeco();
		CONNECT(bt_exit->selected, eWidget::reject);

		bt_save=new eButton(this);
		bt_save->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		bt_save->move(ePoint(btnsize  + 2, clientrect.height()-75));
		bt_save->resize(eSize(btnsize, 30));
		bt_save->setShortcut("green");
		bt_save->setShortcutPixmap("green");
		bt_save->setText(_("Save"));
		bt_save->setHelpText(_("Save selected user script"));
		bt_save->setAlign(0);
		if (showBorder) bt_save->loadDeco();
		CONNECT(bt_save->selected, eScriptWindow::saveScript);

		bt_exec=new eButton(this);
		bt_exec->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		bt_exec->move(ePoint(btnsize * 2  + 2, clientrect.height()-75));
		bt_exec->resize(eSize(btnsize, 30));
		bt_exec->setShortcut("yellow");
		bt_exec->setShortcutPixmap("yellow");
		bt_exec->setText(_("Execute"));
		bt_exec->setHelpText(_("Execute selected user script"));
		bt_exec->setAlign(0);
		if (showBorder) bt_exec->loadDeco();
		CONNECT(bt_exec->selected, eScriptWindow::executeScript);

		bt_del=new eButton(this);
		bt_del->setFont(eSkin::getActive()->queryFont("eStatusBar"));
		bt_del->move(ePoint(btnsize * 3  + 2, clientrect.height()-75));
		bt_del->resize(eSize(btnsize, 30));
		bt_del->setShortcut("blue");
		bt_del->setShortcutPixmap("blue");
		bt_del->setText(_("Delete"));
		bt_del->setHelpText(_("Delete user scripts"));
		bt_del->setAlign(0);
		if (showBorder) bt_del->loadDeco();
		CONNECT(bt_del->selected, eScriptWindow::deleteScript);
		setCheckBox();
	}
}

eScriptWindow::eScriptWindow()
{
	int sW = 720;
	int sH = 576;
	int winW = 404;
	int winH = 385;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Manage User's Scripts"));
	scriptList = new eListBox<eListBoxEntryText>(this);
	scriptList->setFlags(eListBoxBase::flagNoPageMovement);
	scriptList->setName("scriptList");
	scriptList->setHelpText(_("Choose user scripts"));
	scriptList->move(ePoint(5, 5));
	scriptList->resize(eSize(winW - 10,winH - 185));
	scriptList->loadDeco();	
	loadButton(loadScriptList());
	CONNECT(scriptList->selected, eScriptWindow::Listeselected);
	CONNECT(scriptList->selchanged, eScriptWindow::Listeselchanged);
	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, winH - 40) );
	sbar->resize(eSize(winW, 40));
	sbar->loadDeco();	
}

eScriptWindow::~eScriptWindow()
{
}

int eScriptWindow::runScript(eString script,eString Title,int w, int h)
{
	hide();
	eScriptExecute dlg(script,Title,w,h);
	dlg.show();
	int ret = dlg.exec();
	dlg.hide();
	show();
	return ret; 
}

void eScriptWindow::saveScript()
{
	eString scriptToSave = scriptlist[(int)scriptList->getCurrent()->getKey()];
	eString scriptName = getScriptParameter(scriptToSave,"name");
	scriptName = nemTool::getInstance()->replace(scriptName,"'","");
	eMessageBox eMessage(eString().sprintf(_("Save '%s' in progress..."), scriptName.c_str()), _("Running.."), 0);
	eMessage.show();
		eString euseCccd = "false";
		eString econfirmExec = "false";
		eString eshowLog = "false";
		if (usecccd->isChecked()) euseCccd = "true";
		if (confirmexec->isChecked()) econfirmExec = "true";
		if (showlog->isChecked()) eshowLog = "true";
		char buf[256];
		eString fileName, ePar;
		fileName = eString("/var/bin/" + scriptToSave + "_user.sh").c_str();
			
		FILE *in = fopen(fileName.c_str(), "r");
		if(in)
		{	
			FILE *in1 = fopen("/tmp/script.tmp", "w");
			if(in1)
			{	
				fputs ("#!/bin/sh\n", in1);
				fprintf (in1, "usecccd=%s\n", euseCccd.c_str());
				fprintf (in1, "confirm=%s\n", econfirmExec.c_str());
				fprintf (in1, "showlog=%s\n", eshowLog.c_str());
				while (fgets(buf, 256, in))
				{
					ePar = eString().sprintf("%s", buf);
					if (! (strstr(ePar.c_str(),"#!/bin/sh") || strstr(ePar.c_str(),"usecccd=")|| strstr(ePar.c_str(),"confirm=")|| strstr(ePar.c_str(),"showlog=")))
					{	fputs (ePar.c_str(), in1);
					}
				}
				fclose(in1);
			}
			fclose(in);
		}
		system(eString("cp /tmp/script.tmp " + fileName).c_str());
		unlink("/tmp/script.tmp");
		usleep(500000);
	eMessage.hide();
	
}

void eScriptWindow::executeScript()
{
	eString scriptToExecute=scriptlist[(int)scriptList->getCurrent()->getKey()];
	eString dir = "/var/bin/";
	int button;
	if (confirmexec->isChecked())
	{	eMessageBox msg(eString().sprintf(_("Do you want execute the script :\n%s?"),scriptToExecute.c_str()), _("Execute script"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
		msg.show(); 
		button=msg.exec(); 
		msg.hide();
	} else
		button = eMessageBox::btYes;

	if (button == eMessageBox::btYes)
	{	if (usecccd->isChecked())
		{	eMessageBox eMessage(eString().sprintf(_("Script %s is running..."), scriptToExecute.c_str()), _("Running.."), 0);
			eMessage.resize(eSize(400, 120));
			eMessage.show();
				nemTool::getInstance()->sendCmd((char *) eString(dir + scriptToExecute + "_user.sh > /var/tmp/upd_script.out 2>&1").c_str());
			eMessage.hide();
			if (showlog->isChecked())
			{	hide();
					cbpShowFile::getInstance()->showLog(scriptToExecute,0,false);
				show();
			}
		}else 
			runScript(eString(dir + scriptToExecute + "_user.sh"),scriptToExecute);
	}	
}
void eScriptWindow::setCheckBox()
{
	eString scriptToExecute=scriptlist[(int)scriptList->getCurrent()->getKey()];
	readScriptParameter(scriptToExecute);
	usecccd->setCheck(useCccd);
	confirmexec->setCheck(confirmExec);
	showlog->setCheck(showLog);
}

void eScriptWindow::Listeselchanged(eListBoxEntryText *item)
{
	if (item)
		setCheckBox();
}

void eScriptWindow::Listeselected(eListBoxEntryText *item)
{
	if (item)
		executeScript();
	else
		close(0);
}

void eScriptWindow::deleteScript()
{
	eString fileToDelete=scriptlist[(int)scriptList->getCurrent()->getKey()];
	if ( ! (getScriptParameter(fileToDelete,"candelete") == "false")) 
	{
		eMessageBox msg(eString().sprintf(_("Do you want delete the script:\n%s?"),fileToDelete.c_str()), _("Delete script"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
		msg.show(); 
		int button=msg.exec(); 
		msg.hide();
		if (button == eMessageBox::btYes)
		{	eMessageBox eMessage(eString().sprintf(_("Delete %s"), fileToDelete.c_str()), _("Running.."), 0);
			eMessage.resize(eSize(400, 120));
			eMessage.show();
				unlink(eString("/var/bin/" + fileToDelete + "_user.sh").c_str());
				loadScriptList();
			eMessage.hide();
		}	
	} else
		nemTool::getInstance()->msgfail(_("You cannot delete this file!"));
}
