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
#include "addons.h"
#include "nemtool.h"

std::list<eAddonsItemList> eItemsList;
std::list<eAddonsFileList> eFilesList;
bool isEmu;

eString eAddons::getGroupName(int id)
{
	eString name="";
	std::list<eAddonsItemList>::iterator x;
	for(x = eItemsList.begin(); x != eItemsList.end(); ++x)
	{
		if (x->id == id) name = eString(x->item).c_str();
	}
	return name;
}

void eManualAddons::loadManList()
{
	manualList->beginAtomic(); 
	manualList->clearList();
	DIR *d = opendir("/tmp");
	while (struct dirent *e=readdir(d))
	{
		if ( strstr(e->d_name,".tbz2")) 
		{	memset(filetoinstall, 0, sizeof(filetoinstall));
			strncpy(filetoinstall, e->d_name, strlen(e->d_name));
			new eListBoxEntryText(manualList, filetoinstall, (void*)manualList->getCount());
		}
		if ( strstr(e->d_name,".tar.bz2")) 
		{	memset(filetoinstall, 0, sizeof(filetoinstall));
			strncpy(filetoinstall, e->d_name, strlen(e->d_name));
			new eListBoxEntryText(manualList, filetoinstall, (void*)manualList->getCount());
		}
	}
	closedir(d);
	manualList->endAtomic();
}

void eDeleteAddons::loadDelList()
{
	deleteList->beginAtomic(); 
	deleteList->clearList();
	DIR *d = opendir("/var/uninstall");
	while (struct dirent *e=readdir(d))
	{
		if ( strstr(e->d_name,"_remove.sh")) 
		{
			memset(filetodelete, 0, sizeof(filetodelete));
			strncpy(filetodelete, e->d_name, strlen(e->d_name)-10);
			new eListBoxEntryText(deleteList, filetodelete, (void*)deleteList->getCount());
			dellist[(int)deleteList->getCount() - 1] = e->d_name;
		}
		if ( strstr(e->d_name,"_delete.sh")) 
		{
			memset(filetodelete, 0, sizeof(filetodelete));
			strncpy(filetodelete, e->d_name, strlen(e->d_name)-10);
			new eListBoxEntryText(deleteList, filetodelete, (void*)deleteList->getCount());
			dellist[(int)deleteList->getCount() - 1] = e->d_name;
		}
	}
	closedir(d);
	deleteList->endAtomic();
}

void eDeleteAddons::delRegKey(eString delFile)
{
	char buf[256];
	eString regKey;
	FILE *in = fopen(delFile.c_str(), "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			regKey = eString().sprintf("%s", buf);
			regKey=regKey.left(regKey.length() - 1);		
			if (regKey.left(13) == "#regKeyToDel=")
			{	
				regKey = regKey.right(regKey.length() - 13);
				eConfig::getInstance()->delKey(regKey.c_str());
			}
		}
		fclose(in);
	}
}

void eAddons::removeSetting()
{
	system("rm -rf /var/etc/satellites.xml");
	system("rm -rf /var/tuxbox/config/enigma/bouque*");
	system("rm -rf /var/tuxbox/config/enigma/service*");
	system("rm -rf /var/tuxbox/config/enigma/userbouq*");
}

void eAddons::reloadSetting()
{
	eMessageBox box2(_("Reload Settings...."), _("Running.."), 0);
	box2.show();
		if (eDVB::getInstance()->settings)
		{
			eDVB::getInstance()->settings->loadServices();
			eDVB::getInstance()->settings->loadBouquets();
			eZap::getInstance()->getServiceSelector()->actualize();
			eServiceReference::loadLockedList((eZapMain::getInstance()->getEplPath()+"/services.locked").c_str());
		}	
		eZapMain::getInstance()->loadUserBouquets();
		usleep(500000);
	box2.hide();
}

void eAddons::loadFileList(int key)
{
	filelist->beginAtomic(); 
	filelist->clearList();
	std::list<eAddonsFileList>::iterator i;
	for(i = eFilesList.begin(); i != eFilesList.end(); ++i)
	{
		if (i->itemid == key)
		{ 
			labelfile->setText(getGroupName(i->itemid));
			new eListBoxEntryText(filelist, eString().sprintf(" %s (%i Kb)",i->desc.c_str(),i->size), (void *) i->id);
		}	
	}
	filelist->endAtomic();
}

bool eAddons::loadAddons(eString addonsfile)
{
	XMLTreeParser * parser;

	parser = new XMLTreeParser("ISO-8859-1");
	char buf[2048];

	FILE *in = fopen(addonsfile.c_str(), "r");

	int done;
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
	if ( strcmp( root->GetType(), "addons") ) 
	{	eMessageBox msg(_("Invalid configfile"), _("User Abort"), eMessageBox::iconWarning|eMessageBox::btOK);
		msg.show();     msg.exec();     msg.hide();
		return false;
	}
	
	int idcnt = 0;
	int idcnt1 = 0;

	eItemsList.clear();
	eFilesList.clear();
	eAddonsFileList thisFile;
	eAddonsItemList thisAddon;


	for(XMLTreeNode * node = root->GetChild(); node; node = node->GetNext())
	{	for(XMLTreeNode * a = node->GetChild(); a; a = a->GetNext())
		{	if(!strcmp(a->GetType(), "files"))
			{	for(XMLTreeNode * p = a->GetChild(); p; p = p->GetNext())
				{	if(!strcmp(p->GetType(), "filename")) thisFile.filename = p->GetData();
					if(!strcmp(p->GetType(), "desc")) thisFile.desc = p->GetData();
					if(!strcmp(p->GetType(), "dir")) thisFile.dir = p->GetData();
					if(!strcmp(p->GetType(), "size")) thisFile.size = atol(p->GetData());
					if(!strcmp(p->GetType(), "check")) thisFile.check = atol(p->GetData());
				}
				thisFile.id = idcnt1++;
				thisFile.itemid = idcnt;	
				eFilesList.push_back(thisFile);
			}
		}
		thisAddon.id=idcnt++;
		thisAddon.item=node->GetType();
		eItemsList.push_back(thisAddon);
	}
	delete parser;
	return true;
}

void eAddons::serverAddonsConnect()
{
	isEmu = false;
	eString url=nemTool::getInstance()->readAddonsUrl();
	eMessageBox eMessage(_("Connetting to addons server.\nPlease wait..."), _("Connetting..."), 0);
	eMessage.show();
		int useProxy = 0;
		eConfig::getInstance()->getKey("/Nemesis-Project/proxy/isactive", useProxy);
		if (useProxy) system("/var/etc/proxy.sh");
		nemTool::getInstance()->sendCmd((char *) eString("wget " + url + "/addons" + nemTool::getInstance()->getDBVersion() + ".xml -O /tmp/addons.xml").c_str());
		sleep(1);
	eMessage.hide();
	if (nemTool::getInstance()->exists("/tmp/addons.xml",false))
	{	if (loadAddons("/tmp/addons.xml"))
		{	eAddons dlg; 
			dlg.show(); dlg.exec(); dlg.hide();
		}
		unlink("/tmp/addons.xml");
	} else	nemTool::getInstance()->msgfail(_("Server not found!\nPlease check internet connection."));
}

void eAddons::serverEmuConnect()
{
	isEmu = true;
	eString url=nemTool::getInstance()->readEmuUrl();
	eMessageBox eMessage(eString().sprintf(_("Connetting to server: %s"), url.c_str()), _("Connetting..."), 0);
	eMessage.show();
		int useProxy = 0;
		eConfig::getInstance()->getKey("/Nemesis-Project/proxy/isactive", useProxy);
		if (useProxy) system("/var/etc/proxy.sh");
		nemTool::getInstance()->sendCmd((char *) eString("wget " + url + "extra.xml -O /tmp/extra.xml").c_str());
		sleep(1);
	eMessage.hide();
	if (nemTool::getInstance()->exists("/tmp/extra.xml",false))
	{	if (loadAddons("/tmp/extra.xml"))
		{	eAddons dlg; 
			dlg.show(); dlg.exec(); dlg.hide();
		}
		unlink("/tmp/extra.xml");
	} else	nemTool::getInstance()->msgfail(_("Server not found!\nPlease check internet connection."));
}

eAddons::eAddons()	
	:http(0)
{
	int sW = 720;
	int sH = 576;
	int winW = 515;
	int winH = 415;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Download addons"));

	labelspace = new eLabel(this);
	labelspace->move(ePoint(5, 5));
	labelspace->resize(eSize(clientrect.width() - 10, 30));
	labelspace->setAlign(0);

	inUseBar = new eProgress(this);
	inUseBar->setName("inUseBar");
	inUseBar->setStart(0);
	inUseBar->setPerc(100);
	inUseBar->move(ePoint(5, 35));
	inUseBar->resize(eSize(clientrect.width() - 10 ,20));
	inUseBar->setProperty("direction", "0");
	updateFree();

	labelname = new eLabel(this);
	labelname->move(ePoint(5, 60));
	labelname->resize(eSize(190, 30));
	labelname->setText(_("Addons"));

	labelfile = new eLabel(this);
	labelfile->move(ePoint(200, 60));
	labelfile->resize(eSize(310, 30));
	labelfile->setText(_("List"));

	namelist = new eListBox<eListBoxEntryText>(this);
	namelist->move(ePoint(5, 95));
	namelist->resize(eSize(190,240 ));
	namelist->setFlags(eListBoxBase::flagNoPageMovement);
	namelist->loadDeco();
	CONNECT(namelist->selchanged, eAddons::nameChanged);

	filelist = new eListBox<eListBoxEntryText>(this);
	filelist->move(ePoint(200, 95));
	filelist->resize(eSize(310,240 ));
	filelist->setFlags(eListBoxBase::flagNoPageMovement);
	filelist->loadDeco();
	CONNECT(filelist->selected, eAddons::nameSelected);

	namelist->clearList();

	std::list<eAddonsItemList>::iterator x;
	for(x = eItemsList.begin(); x != eItemsList.end(); ++x)
	{
		new eListBoxEntryText(namelist, eString().sprintf(" %s",x->item.c_str()), (void *) x->id);
	}

	loadFileList(0);	

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_abort=new eButton(this);
	bt_abort->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_abort->move(ePoint(200 , clientrect.height()-75));
	bt_abort->resize(eSize(310, 30));
	bt_abort->setShortcut("yellow");
	bt_abort->setShortcutPixmap("yellow");
	bt_abort->setAlign(0);
	if (showBorder) bt_abort->loadDeco();
	bt_abort->setText(_("Abort download"));
	CONNECT(bt_abort->selected, eAddons::abortDownload);
	bt_abort->hide();

	bt_down=new eButton(this);
	bt_down->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_down->move(ePoint(200 , clientrect.height()-75));
	bt_down->resize(eSize(310, 30));
	bt_down->setShortcut("blue");
	bt_down->setShortcutPixmap("blue");
	bt_down->setAlign(0);
	if (showBorder) bt_down->loadDeco();
	bt_down->setText(_("Download addons"));
	CONNECT(bt_down->selected, eAddons::downloadFile);

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(5, clientrect.height()-75));
	bt_exit->resize(eSize(100, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	bt_exit->setText(_("Close"));
	CONNECT(bt_exit->selected, eWidget::reject);
	setFocus(namelist);

	status = new eStatusBar(this);
	status->move( ePoint(0, clientrect.height()- 40) );
	status->resize(eSize(clientrect.width(), 40));
	status->loadDeco();	
}

eAddons::~eAddons()
{
	if (http)
		delete http;
}

void eAddons::updateFree()
{
	int freeSpace=nemTool::getInstance()->getVarSpacePer();
	int freeSpaceKb=nemTool::getInstance()->getVarSpace();
	if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM7020 || eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM600PVR)
		labelspace->setText(_(eString().sprintf(_("Free space %i %% (%'i Kb)"),freeSpace,freeSpaceKb).c_str()));
	else
		labelspace->setText(_(eString().sprintf(_("Free space in /var %i %% (%'i Kb)"),freeSpace,freeSpaceKb).c_str()));
	inUseBar->setParams(0,100 - freeSpace);
	inUseBar->show();

}

void eAddons::downloadFile()
{
	std::list<eAddonsFileList>::iterator i;
	for(i = eFilesList.begin(); i != eFilesList.end(); ++i)
	{
		if (i->id == (int)filelist->getCurrent()->getKey())
		{ 
			filename = eString(i->filename).c_str();
			dir = eString(i->dir).c_str();
			eString url;
			if (isEmu)
				url = nemTool::getInstance()->readEmuUrl();
			else
				url = nemTool::getInstance()->readAddonsUrl();
			if (i->check)
			{	if (! (nemTool::getInstance()->getVarSpace() > i->size))
				{	nemTool::getInstance()->msgfail(_("Not enough space in /var!\nPlease delete addons before install new."));
					return;
				}
			}
			int useProxy = 0;
			eConfig::getInstance()->getKey("/Nemesis-Project/proxy/isactive", useProxy);
			if (useProxy)
			{	
				setStatus(_("Download addon ...."));
				eMessageBox eMessage(eString().sprintf(_("Download %s.\nPlease wait..."), filename.c_str()), _("Running.."), 0);
				eMessage.resize(eSize(400, 120));
				eMessage.show();
					system("/var/etc/proxy.sh");
					nemTool::getInstance()->sendCmd((char *) eString("wget " + url +  dir + "/" + filename + " -O /var/tmp/" + filename).c_str());
				eMessage.show();
				installAddon();
				return;
			}
			loadFile(eString(url + dir + "/" + filename).c_str(),filename);
		}
	}
}

void eAddons::installAddon()
{
	if (nemTool::getInstance()->exists((char *) eString("/tmp/" + filename).c_str(),false))
	{
		eMessageBox msg(eString().sprintf(_("Do you want install the addon:\n%s ?"), filename.c_str()), _("Install Addon"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
		msg.show(); int button=msg.exec(); msg.hide();
		if (button == eMessageBox::btYes)
		{	if (dir == "Settings")
				removeSetting();
			eMessageBox eMessage(eString().sprintf(_("Install %s ..."), filename.c_str()), _("Running.."), 0);
			eMessage.resize(eSize(400, 120));
			eMessage.show();
				nemTool::getInstance()->sendCmd((char *) eString("tar -jxvf /tmp/" + filename + " -C /").c_str());
				system("/bin/sync");
			eMessage.hide();
			if (dir == "Settings")
				reloadSetting();
			setStatus(_("Addon installed."));
		} else
			setStatus(_("Istallation aborted by user!."));
		system((char *) eString("rm -f /tmp/" + filename).c_str());
	}else
		nemTool::getInstance()->msgfail(_("Addon not found!\nPlease check internet connection."));
}

void eAddons::nameSelected(eListBoxEntryText *item)
{
	if (item)
	{	downloadFile();	
	}else
		setFocus(namelist);
}

void eAddons::nameChanged(eListBoxEntryText *item)
{
	if (item) eAddons::loadFileList((int)item->getKey());	
	
}

void eAddons::loadFile(eString url, eString fileName)
{
	tempPath = "/var/tmp/" + fileName ;
	current_url=url;
	int error;
	if (http)
		delete http;
	bt_abort->show();
	bt_down->hide();
	setStatus(_("Download addon ...."));
	http=eHTTPConnection::doRequest(url.c_str(), eApp, &error);
	if (!http)
	{
		fileTransferDone(error);
	} else
	{
		CONNECT(http->transferDone, eAddons::fileTransferDone);
		CONNECT(http->createDataSource, eAddons::createImageDataSink);
		http->local_header["User-Agent"]="PLBOT";
		http->start();
	}
}

void eAddons::setStatus(const eString &string)
{
	status->setText(string);
}

void eAddons::setError(int err)
{
	eString errmsg;
	switch (err)
	{
	case 0:
		if (http && http->code != 200)
			errmsg="error: server replied " + eString().setNum(http->code) + " " + http->code_descr;
		break;
	case -2:
		errmsg="Can't resolve hostname!";
		break;
	case -3:
		errmsg="Can't connect! (check network settings)";
		break;
	default:
		errmsg.sprintf("unknown error %d", err);
	}
	setStatus(errmsg);
	if (errmsg.length())
	{
		if (current_url.length())
			nemTool::getInstance()->msgfail(errmsg+="\n(URL: " + current_url + ")");
	}
}


eHTTPDataSource *eAddons::createImageDataSink(eHTTPConnection *conn)
{
	file = new eHTTPDownload(conn, (char *)tempPath.c_str());
	lasttime=0;
	CONNECT(file->inUseBar, eAddons::downloadProgress);
	return file;
}


void eAddons::downloadProgress(int received, int total)
{
	if ((time(0) == lasttime) && (received != total))
		return;
	lasttime=time(0);
	if (total > 0)
	{
		eString pt;
		int perc=received*100/total;
		pt.sprintf(_("Downloading addon: %d/%d kb (%d%%)"), received/1024, total/1024, perc);
		inUseBar->setPerc(perc);
		labelspace->setText(pt);
	} else
	{
		eString pt;
		pt.sprintf(_("Addon downloaded: %d kb"), received/1024);
		inUseBar->setPerc(0);
		labelspace->setText(pt);
	}
}

void eAddons::abortDownload()
{
	if (http)
	{
		delete http;
		http=0;
	}
	setStatus(_("Download aborted."));
	bt_abort->hide();
	bt_down->show();
	updateFree();
	setFocus(namelist);
}

void eAddons::fileTransferDone(int err)
{
	bt_abort->hide();
	bt_down->show();
	if (err || !http || http->code != 200)
	{	setError(err);
	} else
	{
		setStatus(_("File downloaded correctly!"));
		installAddon();
	}
	http=0;
	updateFree();
	setFocus(namelist);
}

eChooseAddons::eChooseAddons()
		:eSetupWindow(_("Nemesis download panel"), 8, 320)
{
	bool hadEmu = nemTool::getInstance()->exists("/var/etc/extra.url", false);
	int sW = 720;
	int sH = 576;
	int winW = 320;
	int winH = clientrect.height() ;

	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));

	int entry=0;
	CONNECT((new eListBoxEntryMenu(&list, _(" Download addons "), eString().sprintf("(%d) %s", ++entry, _("Download addons from internet")) ))->selected, eChooseAddons::openAddonsDownload);
	if (hadEmu) CONNECT((new eListBoxEntryMenu(&list, _(" Download extra"), eString().sprintf("(%d) %s", ++entry, _("Download extra from internet")) ))->selected, eChooseAddons::openEmuDownload);
	CONNECT((new eListBoxEntryMenu(&list, _(" Delete addons"), eString().sprintf("(%d) %s", ++entry, _("Delete addons")) ))->selected, eChooseAddons::openDelete);
	CONNECT((new eListBoxEntryMenu(&list, _(" Manual install"), eString().sprintf("(%d) %s", ++entry, _("Manual install addons")) ))->selected, eChooseAddons::openManual);
	new eListBoxEntryMenuSeparator(&list, eSkin::getActive()->queryImage("listbox.separator"), 0, true );
	CONNECT((new eListBoxEntryMenu(&list, _(" Setting internet parameter"), eString().sprintf("(%d) %s", ++entry, _("Setting URL and proxy server")) ))->selected, eChooseAddons::openProxy);
}

void eChooseAddons::openAddonsDownload()
{
	hide();
	eAddons dlg; 
	dlg.serverAddonsConnect();
	show();
}

void eChooseAddons::openEmuDownload()
{
	hide();
	eAddons dlg; 
	dlg.serverEmuConnect();
	show();
}

void eChooseAddons::openDelete()
{
	hide();
	eDeleteAddons dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eChooseAddons::openManual()
{
	hide();
	eManualAddons dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

void eChooseAddons::openProxy()
{
	hide();
	eProxySetting dlg; 
	dlg.show(); dlg.exec(); dlg.hide();
	show();
}

eChooseAddons::~eChooseAddons()
{
}

eDeleteAddons::eDeleteAddons()
{
	int sW = 720;
	int sH = 576;
	int winW = 400;
	int winH = 360;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Delete addons"));

	labelspace = new eLabel(this);
	labelspace->move(ePoint(5, 5));
	labelspace->resize(eSize(clientrect.width() - 10, 30));
	labelspace->setAlign(0);

	inUseBar = new eProgress(this);
	inUseBar->setName("inUseBar");
	inUseBar->setStart(0);
	inUseBar->setPerc(100);
	inUseBar->move(ePoint(5, 35));
	inUseBar->resize(eSize(clientrect.width() - 10 ,20));
	inUseBar->setProperty("direction", "0");
	updateFree();

	deleteList = new eListBox<eListBoxEntryText>(this);
	deleteList->move(ePoint(5, 60));
	deleteList->resize(eSize(clientrect.width() - 10 ,clientrect.height() - 100));
	deleteList->setFlags(eListBoxBase::flagNoPageMovement);
	deleteList->loadDeco();	
	CONNECT(deleteList->selected, eDeleteAddons::Listeselected);
	loadDelList();

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_abort=new eButton(this);
	bt_abort->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_abort->move(ePoint(clientrect.width() /2 -50 , clientrect.height() - 35));
	bt_abort->resize(eSize(100, 30));
	bt_abort->setShortcut("red");
	bt_abort->setShortcutPixmap("red");
	bt_abort->setAlign(0);
	if (showBorder) bt_abort->loadDeco();
	bt_abort->setText(_("Close"));
	CONNECT(bt_abort->selected, eWidget::reject);
	setFocus(deleteList);
}

eDeleteAddons::~eDeleteAddons()
{
}

void eDeleteAddons::updateFree()
{
	int freeSpace=nemTool::getInstance()->getVarSpacePer();
	int freeSpaceKb=nemTool::getInstance()->getVarSpace();
	if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM7020 || eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM600PVR)
		labelspace->setText(_(eString().sprintf(_("Free space %i %% (%'i Kb)"),freeSpace,freeSpaceKb).c_str()));
	else
		labelspace->setText(_(eString().sprintf(_("Free space in /var %i %% (%'i Kb)"),freeSpace,freeSpaceKb).c_str()));
	inUseBar->setParams(0,100 - freeSpace);
	inUseBar->show();
}

void eDeleteAddons::Listeselected(eListBoxEntryText *item)
{
	if (item)
	{	eString fileToDelete = dellist[(int)item->getKey()];
		eString fileName = deleteList->getCurrent()->getText().c_str();
		eMessageBox msg(eString().sprintf(_("Do you want delete the addon:\n%s ?"), fileName.c_str()), _("Delete Addon"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
		msg.show(); 
		int button=msg.exec(); 
		msg.hide();
		if (button == eMessageBox::btYes)
		{	eMessageBox eMessage(eString().sprintf(_("Delete %s"), fileName.c_str()), _("Running.."), 0);
			eMessage.resize(eSize(400, 120));
			eMessage.show();
				delRegKey("/var/uninstall/" + fileToDelete);
				system(eString("chmod 755 /var/uninstall/" + fileToDelete).c_str());
				system(eString("/var/uninstall/" + fileToDelete).c_str());
				system("/bin/sync");
				eConfig::getInstance()->flush();
				loadDelList();
			eMessage.hide();
		}	
		updateFree();
	}else
		close(0);
}

eManualAddons::eManualAddons()
{
	int sW = 720;
	int sH = 576;
	int winW = 400;
	int winH = 380;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Install addons manually"));

	labelspace = new eLabel(this);
	labelspace->move(ePoint(5, 5));
	labelspace->resize(eSize(clientrect.width() - 10, 30));
	labelspace->setAlign(0);

	inUseBar = new eProgress(this);
	inUseBar->setName("inUseBar");
	inUseBar->setStart(0);
	inUseBar->setPerc(100);
	inUseBar->move(ePoint(5, 35));
	inUseBar->resize(eSize(clientrect.width() - 10 ,20));
	inUseBar->setProperty("direction", "0");
	updateFree();

	manualList = new eListBox<eListBoxEntryText>(this);
	manualList->move(ePoint(5, 60));
	manualList->resize(eSize(clientrect.width() - 10 ,clientrect.height() - 135));
	manualList->setFlags(eListBoxBase::flagNoPageMovement);
	manualList->loadDeco();	
	CONNECT(manualList->selected, eManualAddons::Listeselected);
	loadManList();

	labelinfo = new eLabel(this);
	labelinfo->move(ePoint(5, clientrect.height() - 70));
	labelinfo->resize(eSize(clientrect.width() - 10, 30));
	labelinfo->setAlign(0);
	labelinfo->setText(_("Put your plugin (xxx.tbz2) via FTP in /tmp."));

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_abort=new eButton(this);
	bt_abort->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_abort->move(ePoint(5 , clientrect.height()-35));
	bt_abort->resize(eSize(100, 30));
	bt_abort->setShortcut("red");
	bt_abort->setShortcutPixmap("red");
	bt_abort->setAlign(0);
	if (showBorder) bt_abort->loadDeco();
	bt_abort->setText(_("Close"));
	CONNECT(bt_abort->selected, eWidget::reject);

	bt_refresh=new eButton(this);
	bt_refresh->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_refresh->move(ePoint(clientrect.width() - 205 , clientrect.height()-35));
	bt_refresh->resize(eSize(200, 30));
	bt_refresh->setShortcut("yellow");
	bt_refresh->setShortcutPixmap("yellow");
	bt_refresh->setAlign(0);
	if (showBorder) bt_refresh->loadDeco();
	bt_refresh->setText(_("Read again /tmp"));
	CONNECT(bt_refresh->selected, eManualAddons::loadManList);

	setFocus(manualList);
}

eManualAddons::~eManualAddons()
{
}

void eManualAddons::updateFree()
{
	int freeSpace=nemTool::getInstance()->getVarSpacePer();
	int freeSpaceKb=nemTool::getInstance()->getVarSpace();
	if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM7020 || eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM600PVR)
		labelspace->setText(_(eString().sprintf(_("Free space %i %% (%'i Kb)"),freeSpace,freeSpaceKb).c_str()));
	else
		labelspace->setText(_(eString().sprintf(_("Free space in /var %i %% (%'i Kb)"),freeSpace,freeSpaceKb).c_str()));
	inUseBar->setParams(0,100 - freeSpace);
	inUseBar->show();
}

void eManualAddons::Listeselected(eListBoxEntryText *item)
{
	if (item)
	{	
		eString fileToInstall=manualList->getCurrent()->getText().c_str();
		eMessageBox msg(eString().sprintf(_("Do you want install the addon:\n%s ?"), fileToInstall.c_str()), _("Install Addon"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
		msg.show(); 
		int button=msg.exec(); 
		msg.hide();
		if (button == eMessageBox::btYes)
		{	eMessageBox eMessage(eString().sprintf(_("Install %s ..."), fileToInstall.c_str()), _("Running.."), 0);
			eMessage.resize(eSize(400, 120));
			eMessage.show();
				nemTool::getInstance()->sendCmd((char *) eString("tar -jxvf /tmp/" + fileToInstall + " -C /").c_str());
				system((char *) eString("rm -f /tmp/" + fileToInstall).c_str());
				system("/bin/sync");
				loadManList();
			eMessage.hide();
		}	
		updateFree();
	}else
		close(0);
}

eProxySetting::eProxySetting()
{
	int sW = 720;
	int sH = 576;
	int winW = 504;
	int winH = 330;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText((_("Setting internet parameter")));

	char *ctemp = 0;
	int isActive = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/proxy/isactive", isActive);
	int labelWidth = 110;
	int textWidth = clientrect.width() - labelWidth -15 ;

	int posL=130;
	int posI=5;

	labelurl = new eLabel(this);
	labelurl->move(ePoint(5, posI + 10));
	labelurl->resize(eSize(winW - 10, 30));
	labelurl->setText(_("Extra server address to connect"));
	labelurl->setFont(eSkin::getActive()->queryFont("eStatusBar"));

	url=new eTextInputField(this);
	url->move(ePoint(5, posI += 35));
	url->resize(eSize(winW - 10, 30));
	url->setUseableChars("abcdefghijklmnopqrstuvwxyz1234567890-_./");
	url->loadDeco();
	url->setText(nemTool::getInstance()->readEmuUrl());
	url->setHelpText(_("Enter extra address"));

	useProxy = new eCheckbox(this,isActive,1);
	useProxy->setText(_("Use Proxy Server"));
	useProxy->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	useProxy->move(ePoint(15, posI += 45));
	useProxy->resize(eSize(clientrect.width()- 20,30));
	useProxy->setHelpText(_("Enable / Disable proxy"));

	label[0] = new eLabel(this);
	label[0]->move(ePoint(5, posL));
	label[0]->resize(eSize(labelWidth, 25));
	label[0]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[0]->setText(_("Proxy server:"));
	label[0]->setAlign(1);

	input[0]=new eTextInputField(this);
	input[0]->move(ePoint(label[0]->getSize().width() +10 , posI +=35));
	input[0]->resize(eSize(textWidth, 30));
	if (!eConfig::getInstance()->getKey("/Nemesis-Project/proxy/server", ctemp))
	{	input[0]->setText(ctemp);
		free(ctemp);
	}
	input[0]->setHelpText(_("Set Proxy server"));
	input[0]->loadDeco();

	label[1] = new eLabel(this);
	label[1]->move(ePoint(5, posL += 35));
	label[1]->resize(eSize(labelWidth, 25));
	label[1]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[1]->setText(_("Proxy port:"));
	label[1]->setAlign(1);

	input[1]=new eTextInputField(this);
	input[1]->move(ePoint(label[1]->getSize().width() +10 , posI +=35));
	input[1]->resize(eSize(textWidth, 30));
	if (!eConfig::getInstance()->getKey("/Nemesis-Project/proxy/port", ctemp)) 
	{	input[1]->setText(ctemp);
		free(ctemp);
	}
	input[1]->setUseableChars("1234567890");
	input[1]->setHelpText(_("Set Proxy port"));
	input[1]->loadDeco();

	label[2] = new eLabel(this);
	label[2]->move(ePoint(5, posL += 35));
	label[2]->resize(eSize(labelWidth, 25));
	label[2]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[2]->setText(_("User name:"));
	label[2]->setAlign(1);

	input[2]=new eTextInputField(this);
	input[2]->move(ePoint(label[2]->getSize().width() +10 , posI +=35));
	input[2]->resize(eSize(textWidth, 30));
	if (!eConfig::getInstance()->getKey("/Nemesis-Project/proxy/user", ctemp)) 
	{	input[2]->setText(ctemp);
		free(ctemp);
	}
	input[2]->setHelpText(_("Set User name"));
	input[2]->loadDeco();

	label[3] = new eLabel(this);
	label[3]->move(ePoint(5, posL += 35));
	label[3]->resize(eSize(labelWidth, 25));
	label[3]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[3]->setText(_("Password:"));
	label[3]->setAlign(1);

	input[3]=new eTextInputField(this);
	input[3]->move(ePoint(label[2]->getSize().width() +10 , posI +=35));
	input[3]->resize(eSize(textWidth, 30));
	if (!eConfig::getInstance()->getKey("/Nemesis-Project/proxy/password", ctemp)) 
	{	input[3]->setText(ctemp);
		free(ctemp);
	}
	input[3]->setHelpText(_("Set password"));
	input[3]->loadDeco();

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);
	int btnsize = (winW -4) / 4;

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(2, winH - 65));
	bt_exit->resize(eSize(btnsize, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	bt_exit->setText(_("Close"));
	CONNECT(bt_exit->selected, eWidget::reject);

	bt_def=new eButton(this);
	bt_def->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_def->move(ePoint(btnsize, winH - 65));
	bt_def->resize(eSize(btnsize + 2, 30));
	bt_def->setShortcut("green");
	bt_def->setShortcutPixmap("green");
	bt_def->setAlign(0);
	if (showBorder) bt_def->loadDeco();
	bt_def->setText(_("Default"));
	CONNECT(bt_def->selected, eProxySetting::setUrlDefault);

	bt_clear=new eButton(this);
	bt_clear->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_clear->move(ePoint(btnsize * 2 + 2, winH - 65));
	bt_clear->resize(eSize(btnsize, 30));
	bt_clear->setShortcut("yellow");
	bt_clear->setShortcutPixmap("yellow");
	bt_clear->setText(_("Clear"));
	bt_clear->setAlign(0);
	if (showBorder) bt_clear->loadDeco();
	CONNECT(bt_clear->selected, eProxySetting::clearProxySetting);

	bt_save=new eButton(this);
	bt_save->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_save->move(ePoint(btnsize * 3 + 2, winH - 65));
	bt_save->resize(eSize(btnsize, 30));
	bt_save->setShortcut("blue");
	bt_save->setShortcutPixmap("blue");
	bt_save->setText(_("Save"));
	bt_save->setAlign(0);
	if (showBorder) bt_save->loadDeco();
	CONNECT(bt_save->selected, eProxySetting::saveProxySetting);
	
	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, clientrect.height() - 30) );
	sbar->resize(eSize(clientrect.width(), 30));
	sbar->loadDeco();	
}

eProxySetting::~eProxySetting()
{
}

void eProxySetting::saveProxySetting()
{
	if(url->getText() && url->getText() != "http://" )
	{ 
		eString urlS = url->getText().c_str();
		system(eString("echo " + urlS + " > /var/etc/extra.url").c_str());
	}else 
	{
		nemTool::getInstance()->msgfail(_("You do enter a a valid URL!"));
		setFocus(url);
		return;
	}

	if (useProxy->isChecked())
	{
		if(input[0]->getText() && input[1]->getText())
		{ 
			eConfig::getInstance()->setKey("/Nemesis-Project/proxy/isactive", useProxy->isChecked());
			eConfig::getInstance()->setKey("/Nemesis-Project/proxy/server", input[0]->getText().c_str());
			eConfig::getInstance()->setKey("/Nemesis-Project/proxy/port", input[1]->getText().c_str());
			eString urlS;
			urlS += "\"http://";
			if(input[2]->getText() && input[3]->getText())
			{
				eConfig::getInstance()->setKey("/Nemesis-Project/proxy/user", input[2]->getText().c_str());
				eConfig::getInstance()->setKey("/Nemesis-Project/proxy/password", input[3]->getText().c_str());
				urlS += input[2]->getText().c_str();
				urlS += ":";
				urlS += input[3]->getText().c_str();
				urlS += "@";
			}else
			{
				eConfig::getInstance()->delKey("/Nemesis-Project/proxy/user");
				eConfig::getInstance()->delKey("/Nemesis-Project/proxy/password");
			}
			urlS += input[0]->getText().c_str();
			urlS += ":";
			urlS += input[1]->getText().c_str();
			urlS += "\" ";
			system(eString("echo '#!/bin/sh' > /var/etc/proxy.sh").c_str());
			system(eString("echo 'export http_proxy=" + urlS + "' >> /var/etc/proxy.sh").c_str());
			system("chmod 755 /var/etc/proxy.sh");
		}else 
		{
			if (!input[0]->getText()) { nemTool::getInstance()->msgfail(_("You do enter a proxy server!")); setFocus(input[0]); return;}
			if (!input[1]->getText()) { nemTool::getInstance()->msgfail(_("You do enter a proxy port!")); setFocus(input[1]); return;}
		}
	}else
	{
		eConfig::getInstance()->setKey("/Nemesis-Project/proxy/isactive", useProxy->isChecked());
		system("rm -f /var/etc/proxy.sh");
	}
	eConfig::getInstance()->flush();
	close(0);
}

void eProxySetting::clearProxySetting()
{
	eMessageBox msg(_("Do you want delete all proxy setting ?"), _("Delete proxy setting"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
	msg.show(); 
	int button=msg.exec(); 
	msg.hide();
	if (button == eMessageBox::btYes)
	{	
		input[0]->setText(0);
		input[1]->setText(0);
		input[2]->setText(0);
		input[3]->setText(0);
		useProxy->setCheck(0);
		eConfig::getInstance()->delKey("/Nemesis-Project/proxy/isactive");
		eConfig::getInstance()->delKey("/Nemesis-Project/proxy/server");
		eConfig::getInstance()->delKey("/Nemesis-Project/proxy/port");
		eConfig::getInstance()->delKey("/Nemesis-Project/proxy/user");
		eConfig::getInstance()->delKey("/Nemesis-Project/proxy/password");
		eConfig::getInstance()->flush();
		system("rm -f /var/etc/proxy.sh");
		setFocus(useProxy);
	}	
}

void eProxySetting::setUrlDefault()
{
	url->setText("http://");
}
