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
#ifndef __utility_h
#define __utility_h

#include <setup_window.h>

class eUtility: public eSetupWindow
{
	void openSetPathPanel();
	void openSetOsdPanel();
	void openStbyPanel();
	void openDebugPanel();
	void cleanPlaylist();
	void saveDreamParameter();
public:
	eUtility();
	~eUtility();
};

#endif
