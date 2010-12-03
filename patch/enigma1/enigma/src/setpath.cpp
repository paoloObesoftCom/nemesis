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
#include "setpath.h"
#include "nemtool.h"

eSetpath::eSetpath()
{
	int sW = 720;
	int sH = 576;
	int winW = 365;
	int winH = 220;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(_("Setting path"));

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	label = new eLabel(this);
	label->move(ePoint(5, 25));
	label->resize(eSize(150, 30));
	label->setText(_("PVR movie folder:"));
	label->setAlign(1);
	label->setFont(eSkin::getActive()->queryFont("eStatusBar"));

	recdir=new eTextInputField(this);
	recdir->move(ePoint(160, 20));
	recdir->resize(eSize(200, 30));
	recdir->setUseableChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_./");
	recdir->loadDeco();
	recdir->setText("/hdd/movie");
	recdir->setHelpText(_("Set PVR movie folder path"));
	char *eMovieDir = 0;
	if (!eConfig::getInstance()->getKey("/Nemesis-Project/PVR/moviedir", eMovieDir))
		recdir->setText(eMovieDir);

	label = new eLabel(this);
	label->move(ePoint(5, 60));
	label->resize(eSize(150, 30));
	label->setText(_("Picon images folder:"));
	label->setAlign(1);
	label->setFont(eSkin::getActive()->queryFont("eStatusBar"));

	picondir=new eComboBox(this, 4);
	picondir->move(ePoint(160, 55));
	picondir->resize(eSize(200, 30));
	picondir->loadDeco();
	picondir->setHelpText(_("Select Picon images path"));
	new eListBoxEntryText(*picondir, "/var/tuxbox/config/", (void*)0, 0);
	new eListBoxEntryText(*picondir, "/mnt/usb/", (void*)1, 0);
	new eListBoxEntryText(*picondir, "/mnt/hdd/", (void*)2, 0);
	new eListBoxEntryText(*picondir, "/mnt/cf/", (void*)3, 0);
	char *piconPath = "/var/tuxbox/config/";
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/piconpath", piconPath);
	eString piconpath = eString().sprintf("%s",piconPath);
	if (piconpath == "/var/tuxbox/config/")
		picondir->setCurrent(0,true);
	else if (piconpath == "/mnt/usb/")
		picondir->setCurrent(1,true);
	else if (piconpath == "/mnt/hdd/")
		picondir->setCurrent(2,true);
	else 
		picondir->setCurrent(3,true);

	bt_rec=new eButton(this);
	bt_rec->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_rec->move(ePoint(5, winH - 115));
	bt_rec->resize(eSize(355, 30));
	bt_rec->setShortcut("yellow");
	bt_rec->setShortcutPixmap("yellow");
	bt_rec->setAlign(0);
	if (showBorder) bt_rec->loadDeco();
	bt_rec->setText(_("Recover Movie list"));
	bt_rec->setHelpText(_("Recover Movie list"));
	CONNECT(bt_rec->selected, eSetpath::recoverMovieList);

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(5, winH - 75));
	bt_exit->resize(eSize(120, 30));
	bt_exit->setText(_("Close"));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setAlign(0);
	bt_exit->setHelpText(_("Close"));
	if (showBorder) bt_exit->loadDeco();
	CONNECT(bt_exit->selected, eWidget::reject);

	bt_def=new eButton(this);
	bt_def->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_def->move(ePoint(125, winH - 75));
	bt_def->resize(eSize(115, 30));
	bt_def->setShortcut("green");
	bt_def->setShortcutPixmap("green");
	bt_def->setText(_("Default"));
	bt_def->setHelpText(_("Set default parameter"));
	bt_def->setAlign(0);
	if (showBorder) bt_def->loadDeco();
	CONNECT(bt_def->selected, eSetpath::setDef);

	bt_save=new eButton(this);
	bt_save->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_save->move(ePoint(240, winH - 75));
	bt_save->resize(eSize(120, 30));
	bt_save->setShortcut("blue");
	bt_save->setShortcutPixmap("blue");
	bt_save->setText(_("Save"));
	bt_def->setHelpText(_("Save parameter"));
	bt_save->setAlign(0);
	if (showBorder) bt_save->loadDeco();
	CONNECT(bt_save->selected, eSetpath::saveDir);

	sbar = new eStatusBar(this);
	sbar->move( ePoint(0, winH - 40) );
	sbar->resize(eSize(winW, 40));
	sbar->loadDeco();	
	setFocus(recdir);
}

eSetpath::~eSetpath()
{
}

void eSetpath::saveDir()
{
	if(recdir->getText())
	{ 
		eMessageBox box(_("Saving folder parameter.\nPlease Wait..."), _("Running.."), 0);
		box.show();
			bool folderOK;
			folderOK = nemTool::getInstance()->existsdir((char*) recdir->getText().c_str(),false);
			usleep(700000);
		box.hide();
		if (!folderOK)
		{
			eMessageBox msg(_(eString().sprintf(_("The folder '%s' does not exist.\nDo you want continue and save anyway?"),recdir->getText().c_str()).c_str()), _("Folder error"), eMessageBox::iconQuestion|eMessageBox::btYes|eMessageBox::btNo,eMessageBox::btNo);
			msg.show(); 
			int button=msg.exec(); 
			msg.hide();
			if (button == eMessageBox::btNo)
			{
				setFocus(recdir); 
				return;
			}
		}
		eConfig::getInstance()->setKey("/Nemesis-Project/tools/piconpath", picondir->getCurrent()->getText().c_str());
		eConfig::getInstance()->setKey("/Nemesis-Project/PVR/moviedir", recdir->getText().c_str());
		eConfig::getInstance()->flush();
		close(0);
	}else 
	{
		nemTool::getInstance()->msgfail(_("You do enter a valid path!")); 
		setFocus(recdir); 
		return;
	}


}

void eSetpath::setDef()
{
	recdir->setText("/hdd/movie");
	picondir->setCurrent(0,true);
}

void eSetpath::recoverMovieList()
{
	eMessageBox box(_("Recover movie list.\nPlease Wait..."), _("Running.."), 0);
	box.show();
		bool recoverOK;
		recoverOK = recover_movies();
		usleep(700000);
	box.hide();
}

bool eSetpath::recover_movies()
{
	eString filen;
	int i;
	bool result = false;

	eZapMain::getInstance()->loadRecordings();
	ePlaylist *recordings = eZapMain::getInstance()->getRecordings();
	std::list<ePlaylistEntry>& rec_list = recordings->getList();
	struct dirent **namelist;

	char *eMoviePath = 0;
	int n;
	if (!eConfig::getInstance()->getKey("/Nemesis-Project/PVR/moviedir", eMoviePath))
		n = scandir(eMoviePath, &namelist, 0, alphasort);
	else	
		n = scandir("/hdd/movie", &namelist, 0, alphasort);

	if (n > 0)
	{
		// There will be 2 passes through recordings list:
		// 1) Delete entries from list that are not on disk.
		// 2) Add entries to list that do not exist yet.
	
		// Pass 1
		std::list<ePlaylistEntry>::iterator it(rec_list.begin());
		std::list<ePlaylistEntry>::iterator it_next;
		while (it != rec_list.end()) 
		{
			bool valid_file = false;
			// For every file in /hdd/movie
			for (i = 0; i < n; i++)
			{
				filen = namelist[i]->d_name;
				// For every valid file
				if ((filen.length() >= 3) &&
				(filen.substr(filen.length()-3, 3).compare(".ts") == 0) &&
					(it->service.path.length() >= 11) &&
					!it->service.path.substr(11,it->service.path.length()-11).compare(filen))
				{
					valid_file = true;
					break;
				}
			}

			(it_next = it)++;
			if (!valid_file)
				rec_list.erase(it);
			else
			{
				// Trim descr
				if (it->service.descr.find_last_not_of(' ') != eString::npos) 
					it->service.descr = it->service.descr.substr(0, it->service.descr.find_last_not_of(' ') + 1);
			}
			it = it_next;
		}
		
		// Pass 2
		for (i = 0; i < n; i++)
		{
			filen = namelist[i]->d_name;
			// For every .ts file
			if ((filen.length() >= 3) &&
				(filen.substr(filen.length()-3, 3).compare(".ts") == 0))
			{
				// Check if file is in the list.
				bool file_in_list = false;
				for (std::list<ePlaylistEntry>::iterator it(rec_list.begin()); it != rec_list.end(); ++it)
				{
					if ((it->service.path.length() >= 11) &&
						!it->service.path.substr(11,it->service.path.length()-11).compare(filen))
					{
						file_in_list = true;
						break;
					}
				}
				if (!file_in_list)	// Add file to list.
				{
					eMoviePath = 0;
					if (!eConfig::getInstance()->getKey("/Nemesis-Project/PVR/moviedir", eMoviePath))
					{	eServicePath path(eString().sprintf("1:0:1:0:0:0:000000:0:0:0:%s/", eMoviePath) + filen);
						rec_list.push_back(path);
					}else	
					{	eServicePath path("1:0:1:0:0:0:000000:0:0:0:/hdd/movie/" + filen);
						rec_list.push_back(path);
					}
					rec_list.back().type = 16385;
					rec_list.back().service.descr = filen.substr(0, filen.find(".ts"));
					eMoviePath = 0;
					if (!eConfig::getInstance()->getKey("/Nemesis-Project/PVR/moviedir", eMoviePath))
						rec_list.back().service.path = eString().sprintf("%s/",eMoviePath) + filen;
					else	
						rec_list.back().service.path = "/hdd/movie/" + filen;
			}
			}
			free(namelist[i]);
		}
		rec_list.sort();
		result = true;
	}
	
	recordings->save();
	free(namelist);

	return result;
}
