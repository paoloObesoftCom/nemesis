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
#ifndef __nemtool_h
#define __nemtool_h

#include <dirent.h>
#include <sys/vfs.h>
#include <stdio.h>
#include <string.h> 
#include <lib/gui/emessage.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <lib/system/info.h>

class nemTool: public eApplication, public Object
{
	static nemTool *instance;

public:
	static nemTool *getInstance();

	void readPortNumber(char * port);
	bool sendCmd(char *command);
	eString replace(eString s, eString f, eString r);
	void msgfail(eString okstring);
	void msgok(eString okstring);
	bool exists(char *path, bool *mode);
	bool existsdir(char *path, bool *mode);
	int getVarSpace();
	int getVarSpacePer();
	bool isMultiboot();
	eString getDBVersion();
	eString getPlugName(char*);
	eString readAddonsUrl();
	eString readEmuUrl();
	nemTool();
	~nemTool();
};

#endif
