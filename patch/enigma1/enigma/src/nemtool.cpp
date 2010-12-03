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
#include "nemtool.h"

nemTool *nemTool::instance;

nemTool *nemTool::getInstance()
{
	return instance;
}

eString nemTool::getPlugName(char *plugFile)
{
	if (!instance)
		instance=this;
	char buf[256];
	eString plugName;
	eString retName = "None";
	FILE *in = fopen(plugFile, "r");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			plugName = eString().sprintf("%s", buf);
			plugName=plugName.left(plugName.length() - 1);		
			if (plugName.left(5) == "name=")
			{	
				fclose(in);
				plugName = plugName.right(plugName.length() - 5);
				return plugName;
			}
		}
		fclose(in);
	}
	return retName;
}

void nemTool::readPortNumber(char * port)
{
	if (!instance)
		instance=this;
	char buf[256];
	if (access("/var/etc/nemesis.cfg", R_OK) == 0)
	{
		FILE *in = fopen("/var/etc/nemesis.cfg", "r");
		if(in)
		{	
			while (fgets(buf, 256, in))
			{
				if (strstr(buf ,"daemon_port="))
				{	
					fclose(in);
					char * pch;
					pch = strtok(buf,"=");
					pch = strtok(NULL,"=");
					strncpy(port,pch,strlen(pch));
					return;
				}
			}
			fclose(in);
		}	
	}
	strncpy(port,"1888",4);
	return;
}

bool nemTool::sendCmd(char *command)
{
	if (!instance)
		instance=this;
	bool ret = true;
	int sockfd, portno, n;
	char *strCmd;
	char buffer[256];

	struct sockaddr_in serv_addr;
	struct hostent *server;

	char port[6] = "";
	readPortNumber(port);
	portno = atoi(port);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)	
	{	eMessageBox msg("ERROR open socket.\nStart nemesisd daemon!", _("Error"), eMessageBox::iconError|eMessageBox::btOK); 
		msg.show(); msg.exec(); msg.hide();
		ret = false;
	}else
	{	server = gethostbyname("127.0.0.1");
		if (server == NULL) 
		{	eMessageBox msg1("The nemesisd server not respond!", _("Error"), eMessageBox::iconError|eMessageBox::btOK); 
			msg1.show(); msg1.exec(); msg1.hide();
			ret = false;
		}else
		{	bzero((char *) &serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
			serv_addr.sin_port = htons(portno);
			if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
			{	eMessageBox msg2("Connection error!", _("Error"), eMessageBox::iconError|eMessageBox::btOK); 
				msg2.show(); msg2.exec(); msg2.hide();
				ret = false;
			} else
			{	strCmd  = command;
				n = write(sockfd,strCmd,strlen(strCmd));
				if (n < 0) ret = false;
				bzero(buffer,256);
				n = read(sockfd,buffer,255);
				if (n < 0) ret = false;
			}
			close(sockfd);
		}
	}
	return ret;
}

eString nemTool::replace(eString s, eString f, eString r) 
{
	if (!instance)
		instance=this;
	unsigned int found = s.find(f);
	while(found != eString::npos) {
	s.replace(found, f.length(), r);
	found = s.find(f);
	}
	return s;
}

void nemTool::msgok(eString okstring)
{
	if (!instance)
		instance=this;
	eMessageBox msg(okstring, _("Info"), eMessageBox::iconInfo|eMessageBox::btOK); 
	msg.show(); 
	msg.exec(); 
	msg.hide();
}

void nemTool::msgfail(eString failstring)
{
	if (!instance)
		instance=this;
	eMessageBox msg(failstring, _("Error!"), eMessageBox::iconError|eMessageBox::btOK); 
	msg.show(); 
	msg.exec(); 
	msg.hide();
}

bool nemTool::exists(char *path, bool *mode=false)
{
	if (!instance)
		instance=this;
	FILE *f = fopen(path,"r");
	if (f) 
	{	fclose(f);
		return true;
	} else 
	{	if (mode) msgfail(eString().sprintf("File :%s not found!",path).c_str());
		return false;
	}
}

bool nemTool::isMultiboot()
{
	if (!instance)
		instance=this;
	bool isMb = false;
		system("touch /gtujiojih");
	if (exists("/gtujiojih"))
	{	isMb = true;
		unlink("/gtujiojih");
	}
	return isMb;
}

bool nemTool::existsdir(char *path, bool *mode=false)
{
	if (!instance)
		instance=this;
	DIR *f = opendir(path);
	if (f) 
	{	closedir(f);
		return true;
	} else 
	{	if (mode) msgfail(eString().sprintf("Folder :%s not found!",path).c_str());
		return false;
	}
}

int nemTool::getVarSpace()
{
	if (!instance)
		instance=this;
	struct statfs s;
        //struct statvfs *fpData;
        char fnPath[128];
	int ris;
	if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM7020 ||
		eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM600PVR ||
		eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM500PLUS)
		strcpy(fnPath, "/");
	else
		strcpy(fnPath, "/var");
	if((statfs(fnPath,&s)) < 0 ) {
		msgfail("Failed to stat /var");
		ris = -1;
	} else {
		ris = s.f_bfree * (s.f_bsize / 1024) - 50;
	}
	return ris;
}


int nemTool::getVarSpacePer()
{
	if (!instance)
		instance=this;
	struct statfs s;
        char fnPath[128];
	int ris;
	if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM7020 ||
		eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM600PVR ||
		eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM500PLUS)
		strcpy(fnPath, "/");
	else
		strcpy(fnPath, "/var");
	if((statfs(fnPath,&s)) < 0 ) {
		msgfail("Failed to stat /var");
		ris = 0;
	} else {
		float pp = (float)s.f_bfree / (float)s.f_blocks * 100;
		char po [50];
		sprintf(po,"%f", pp);
		ris = atoi(po);
	}
	return ris;
}

eString nemTool::readAddonsUrl()
{
	if (!instance)
		instance=this;
	char buf[256];
	eString AddonsUrl;
	system("dos2unix /var/etc/addons.url");
	FILE *in = fopen("/var/etc/addons.url", "r");
	if(in)
	{	fgets(buf, 256, in);
		AddonsUrl=eString().sprintf("%s", buf);
		AddonsUrl=AddonsUrl.left(AddonsUrl.length() - 1);		
		fclose(in);
		return AddonsUrl;
	} else
	{	AddonsUrl="http://www.edg-nemesis.tv/ADDONS/Nemesis/";
		return  AddonsUrl;
	}
}

eString nemTool::readEmuUrl()
{
	if (!instance)
		instance=this;
	char buf[256];
	eString AddonsUrl;
	system("dos2unix /var/etc/extra.url");
	FILE *in = fopen("/var/etc/extra.url", "r");
	if(in)
	{	fgets(buf, 256, in);
		AddonsUrl=eString().sprintf("%s", buf);
		AddonsUrl=AddonsUrl.left(AddonsUrl.length() - 1);		
		fclose(in);
		return AddonsUrl;
	} else return "";
}

eString nemTool::getDBVersion()
{
	eString eType  = "";	
	if (!instance)
		instance=this;
	if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM7020) eType = "7020";
	else if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM7000) eType = "7000";
	else if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM5600) eType = "5600";
	else if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM5620) eType = "5620";
	else if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM600PVR) eType = "600";
	else if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM500) eType = "500";
	else if (eSystemInfo::getInstance()->getHwType() == eSystemInfo::DM500PLUS) eType = "500PLUS";
	return eType;
}

nemTool::~nemTool()
{
	if (instance==this)
		instance=0;
}
