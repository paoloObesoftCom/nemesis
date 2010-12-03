 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team

 Please if you use this source, refer to Nemesis -Team

 A part of this code is based from: enigma cvs
 ****************************************************************************************************
 */
#include "osdinfo.h"

osdInfo *osdInfo::instance;

osdInfo *osdInfo::getInstance()
{
	return instance;
}

eString osdInfo::getCamName()
{
	if (!instance)
		instance=this;
	char buf[256];
	eString emuName;
	eString emu;
	if (access("/var/bin/emuactive", R_OK) == 0)
	{
		FILE *in = fopen("/var/bin/emuactive", "rt");
		if(in)
		{	fgets(buf, 256, in);
			emu=eString().sprintf("%s", buf);
			fclose(in);
			FILE *in1 = fopen(eString("/var/script/" + emu + "_em.sh").c_str(), "rt");
			if(in1)
			{	while (fgets(buf, 256, in1))
				{	emuName = eString().sprintf("%s", buf);
					emuName=emuName.left(emuName.length() - 1);	
					if (emuName.left(9) == "#emuname=")
					{	fclose(in1);
						emuName = emuName.right(emuName.length() - 9);
						return emuName;
					}
				}
				fclose(in1);
			}
		}
	}
	return "None";
}

eString osdInfo::getCsName()
{
	if (!instance)
		instance=this;
	char buf[256];
	eString csName;
	eString cs;
	if (access("/var/bin/csactive", R_OK) == 0)
	{
		FILE *in = fopen("/var/bin/csactive", "rt");
		if(in)
		{	fgets(buf, 256, in);
			cs=eString().sprintf("%s", buf);
			fclose(in);
			FILE *in1 = fopen(eString("/var/script/" + cs + "_cs.sh").c_str(), "rt");
			if(in1)
			{	while (fgets(buf, 256, in1))
				{	csName = eString().sprintf("%s", buf);
					csName=csName.left(csName.length() - 1);	
					if (csName.left(9) == "#srvname=")
					{	fclose(in1);
						csName = csName.right(csName.length() - 9);
						return csName;
					}
				}
				fclose(in1);
			}
		}
	}
	return "None";
}

eString osdInfo::getEmuInfo()
{
	if (!instance)
		instance=this;

	eString retInfo = "";
	eString csInfo = getCsName();
	eString emuInfo = getCamName();

	if (emuInfo == "None" && csInfo == "None")
		retInfo = "Common Interface";
	else if (emuInfo != "None" && csInfo == "None")
		retInfo = emuInfo;
	else if (emuInfo == "None" && csInfo != "None")
		retInfo = csInfo;
	else if (emuInfo != "None" && csInfo != "None")
		retInfo = emuInfo + ":" + csInfo;

	return retInfo;
}

eString osdInfo::getInfoFileName()
{
	if (!instance)
		instance=this;
	char buf[256];
	eString fileName;
	eString emu;
	if (access("/var/bin/emuactive", R_OK) == 0)
	{
		FILE *in = fopen("/var/bin/emuactive", "rt");
		if(in)
		{	fgets(buf, 256, in);
			emu=eString().sprintf("%s", buf);
			fclose(in);
			FILE *in1 = fopen(eString("/var/script/" + emu + "_em.sh").c_str(), "rt");
			if(in1)
			{	while (fgets(buf, 256, in1))
				{	fileName = eString().sprintf("%s", buf);
					fileName=fileName.left(fileName.length() - 1);		
					if (fileName.left(13) == "#ecminfofile=")
					{	fclose(in1);
						fileName = "/tmp/" + fileName.right(fileName.length() - 13);
						return fileName;
					}
				}
				fclose(in1);
			}
		}
	}
	eString cs;
	if (access("/var/bin/csactive", R_OK) == 0)
	{
		FILE *in = fopen("/var/bin/csactive", "rt");
		if(in)
		{	fgets(buf, 256, in);
			cs=eString().sprintf("%s", buf);
			fclose(in);
			FILE *in1 = fopen(eString("/var/script/" + cs + "_cs.sh").c_str(), "rt");
			if(in1)
			{	while (fgets(buf, 256, in1))
				{	fileName = eString().sprintf("%s", buf);
					fileName=fileName.left(fileName.length() - 1);		
					if (fileName.left(13) == "#ecminfofile=")
					{	fclose(in1);
						fileName = "/tmp/" + fileName.right(fileName.length() - 13);
						return fileName;
					}
				}
				fclose(in1);
			}
		}
	}
	return "None";
}

eString osdInfo::getVideoInfo()
{
	if (!instance)
		instance=this;
	eString vformat="n/a";
	FILE *bitstream=0;
	
	if (Decoder::current.vpid!=-1)
		bitstream=fopen("/proc/bus/bitstream", "rt");
	if (bitstream)
	{
		char buffer[100];
		int xres=0, yres=0, aspect=0, framerate=0;
		while (fgets(buffer, 100, bitstream))
		{
			if (!strncmp(buffer, "H_SIZE:  ", 9))
				xres=atoi(buffer+9);
			if (!strncmp(buffer, "V_SIZE:  ", 9))
				yres=atoi(buffer+9);
			if (!strncmp(buffer, "A_RATIO: ", 9))
				aspect=atoi(buffer+9);
			if (!strncmp(buffer, "F_RATE: ", 8))
				framerate=atoi(buffer+8);
		}
		fclose(bitstream);
		vformat.sprintf("%dx%d ", xres, yres);
		switch (aspect)
		{
		case 1:
			vformat+="square"; break;
		case 2:
			vformat+="4:3"; break;
		case 3:
			vformat+="16:9"; break;
		case 4:
			vformat+="20:9"; break;
		}
		switch(framerate)
		{
		case 1:
			vformat+=", 23.976 fps";
			break;
		case 2:
			vformat+=", 24 fps";
			break;
		case 3:
			vformat+=", 25 fps";
			break;
		case 4:
			vformat+=", 29.97 fps";
			break;
		case 5:
			vformat+=", 30 fps";
			break;
		case 6:
			vformat+=", 50 fps";
			break;
		case 7:
			vformat+=", 59.94 fps";
			break;
		case 8:
			vformat+=", 80 fps";
		}
	}
	return vformat;
}

eString osdInfo::replace(eString s, eString f, eString r) 
{
	if (!instance)
		instance=this;
	unsigned int flen = f.length();
	if (flen > 0)
	{
		unsigned int found = s.find(f);
		unsigned int rlen = r.length();
		while(found != eString::npos)
		{
			s.replace(found, flen, r);
			found = s.find(f, found + rlen);
		}
	}
	return s;
}

// eString osdInfo::replace(eString s, eString f, eString r) 
// {
// 	if (!instance)
// 		instance=this;
// 	unsigned int found = s.find(f);
// 	while(found != eString::npos) {
// 		s.replace(found, f.length(), r);
// 		found = s.find(f);
// 	}
// 	return s;
// }
// 

int osdInfo::getCodifiche()
{
	if (!instance)
		instance=this;
	int intCod = 0;
	eDVBServiceController *sapi = eDVB::getInstance()->getServiceAPI();
	if(sapi)
	{
		std::set<int>& calist = sapi->usedCASystems;
		for (std::set<int>::iterator i(calist.begin()); i != calist.end(); ++i)
		{
			eString caname = eStreaminfo::getInstance()->getCAName(*i, 1);
			if(strstr(caname.c_str(),"Viacces")) intCod += 1;
			if(strstr(caname.c_str(),"Videoguard")) intCod += 2;
			if(strstr(caname.c_str(),"Seca")) intCod += 4;
			if(strstr(caname.c_str(),"Irdeto")) intCod += 8;
			if(strstr(caname.c_str(),"Cryptoworks")) intCod += 16;
			if(strstr(caname.c_str(),"Betacrypt")) intCod += 32;
			if(strstr(caname.c_str(),"Kudelski")) intCod += 64;
			if(strstr(caname.c_str(),"Conax")) intCod += 128;
			if(strstr(caname.c_str(),"DreamCrypt")) intCod += 256;
		}
	}
	return intCod;
}

void osdInfo::getFormattedData(char * data)
{
	if (!instance)
		instance=this;

	static const char *dayStr[7] = { _("Sunday"), _("Monday"), _("Tuesday"), _("Wednesday"), _("Thursday"), _("Friday"), _("Saturday") };
	static const char *dayFixStr[7] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	static const char *monthStr[12] = { _("January"), _("February"), _("March"),_("April"), _("May"), _("June"),_("July"),_("August"), _("September"), _("October"),_("November"), _("December") };
	static const char *monthFixStr[12] = { "January","February","March","April","May","June","July","August","September","October","November","December" };

	time_t c=time(0)+eDVB::getInstance()->time_difference;
	tm *t=localtime(&c);

	char bufferDay [30];
	char bufferDays [10];
	char bufferMon [30];
	char bufferYear [10];
	setlocale(LC_TIME, "C");
	strftime (bufferDay,30,"%A",t);
	strftime (bufferDays,10,", %d ",t);
	strftime (bufferMon,30,"%B",t);
	strftime (bufferYear,10," %Y",t);

	for (int i=0; i < 7;i++)
	{
		if (strcmp(bufferDay,dayFixStr[i])==0) strcpy(data,dayStr[i]);
	}
	strncat(data,bufferDays,strlen(bufferDays));
	for (int x = 0; x < 12; x++)
	{
		if (strcmp(bufferMon,monthFixStr[x])==0) strncat(data,monthStr[x],strlen(monthStr[x]));
	}
	strncat(data,bufferYear,strlen(bufferYear));
	
}

void osdInfo::getFormattedDataShort(char * data)
{
	if (!instance)
		instance=this;

	static const char *dayStr[7] = { _("Sunday"), _("Monday"), _("Tuesday"), _("Wednesday"), _("Thursday"), _("Friday"), _("Saturday") };
	static const char *dayFixStr[7] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	static const char *monthStr[12] = { _("Jan"), _("Feb"), _("Mar"),_("Apr"), _("may"), _("Jun"),_("Jul"),_("Aug"), _("Sep"), _("Oct"),_("Nov"), _("Dec") };
	static const char *monthFixStr[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov", "Dec" };

	time_t c=time(0)+eDVB::getInstance()->time_difference;
	tm *t=localtime(&c);

	char bufferDay [30];
	char bufferDays [10];
	char bufferMon [30];
	char bufferYear [10];
	setlocale(LC_TIME, "C");
	strftime (bufferDay,30,"%A",t);
	strftime (bufferDays,10,", %d ",t);
	strftime (bufferMon,30,"%b",t);
	strftime (bufferYear,10," %Y",t);

	for (int i=0; i < 7;i++)
	{
		if (strcmp(bufferDay,dayFixStr[i])==0) strcpy(data,dayStr[i]);
	}
	strncat(data,bufferDays,strlen(bufferDays));
	for (int x = 0; x < 12; x++)
	{
		if (strcmp(bufferMon,monthFixStr[x])==0) strncat(data,monthStr[x],strlen(monthStr[x]));
	}
	strncat(data,bufferYear,strlen(bufferYear));
	
}

void osdInfo::getData(char * caid, char * pid, char * provid, int * source,char * addr, char * port, int * ecmtime, int * hops, char * reader)
{
	if (!instance)
		instance=this;
	strncpy(addr,"None",4);
	char buf[256];
	char buf1[256];
	char *pch;
	char *pch1;
	eString ecmFileName = getInfoFileName();

	if (access(ecmFileName.c_str(), R_OK) == 0)
	{
		FILE *in = fopen(ecmFileName.c_str(), "rt");
		if(in)
		{	while (fgets(buf, 256, in) != NULL)
			{
				strcpy(buf1,buf);
				if (strstr(buf ,"CaID"))
				{	
					pch = strtok(buf1,",");
					pch = strtok(pch," ");
					while ( strcmp(pch ,"CaID"))
					{	pch = strtok(NULL," ");
					}
					pch = strtok(NULL," ");
					if (pch != NULL)
						strncpy(caid,pch,strlen(pch));
				}
				else if (strstr(buf ,"caid:"))
				{	
					pch = strtok(buf,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
						strncpy(caid,pch,strlen(pch)-1);
				}
				strcpy(buf1,buf);
				if (strstr(buf ,"reader:"))
				{	
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
						strncpy(reader,pch,strlen(pch)-1);
				}
				strcpy(buf1,buf);
				if (strstr(buf ,"pid:"))
				{	
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
						strncpy(pid,pch,strlen(pch)-1);
				}
				else if (strstr(buf ,"pid"))
				{	
					pch = strtok(buf,",");
					pch = strtok(NULL,",");
					if (pch != NULL)
					{
						pch = strtok(pch," ");
						pch = strtok(NULL," ");
						if (pch != NULL)
							strncpy(pid,pch,strlen(pch));
					}
				}
				strcpy(buf1,buf);
				if (strstr(buf ,"provid:"))
				{	
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
						strncpy(provid,pch,strlen(pch)-1);
				}
				else if (strstr(buf ,"prov:"))
				{	
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
						strncpy(provid,pch,strlen(pch)-1);
				}
				strcpy(buf1,buf);
				if (strstr(buf,"ecm time:"))
				{	
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch,".");
					if (pch != NULL)
					{	if (strstr(pch,"ms"))
							*ecmtime = atoi(pch);
						else
						{	*ecmtime = atoi(pch) * 1000;
							pch = strtok(NULL,".");
							if (pch != NULL)
								*ecmtime += atoi(pch);
						}
					}
				}
				else if (strstr(buf,"msec"))
				{	
					pch = strtok(buf1,"--");
					pch = strtok(pch," ");
					if (pch != NULL)
						*ecmtime = atoi(pch);
				}
				strcpy(buf1,buf);
				if (strstr(buf,"hops:"))
				{	
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					if (pch != NULL)
						*hops = atoi(pch);
				}
				strcpy(buf1,buf);
				if (strstr(buf,"slot-1"))
					*source = 4;
				else if (strstr(buf,"slot-2"))
					*source = 5;
				else if (strstr(buf,"/dev/sci0"))
					*source = 4;
				else if (strstr(buf,"/dev/sci1"))
					*source = 5;
				else if (strstr(buf,"using:"))
				{
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
					{
						char net[4] = "net"; char newcamd[8] = "newcamd"; char emu[4] = "emu"; char cccam[10] = "CCcam-s2s"; char gbox[5] = "gbox";
						if (strncmp(pch,gbox,4) == 0) *source = 2;
						if (strncmp(pch,cccam,9) == 0) *source = 2;
						if (strncmp(pch,net,3) == 0) *source = 2;
						if (strncmp(pch,newcamd,7) == 0) *source = 2;
						if (strncmp(pch,emu,3) == 0) *source = 1;
					}
				}
				else if (strstr(buf,"source:"))
				{	
					*source = 1;
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
					{
						char net[4] = "net"; char newcamd[8] = "newcamd";
						if (strncmp(pch,net,3) == 0)
						{
							*source = 2;
							strcpy(buf1,buf);
							pch = strtok(buf1,":");
							pch = strtok(NULL,":");
							if (strchr(pch,'('))
							{	char *a;
								a = pch;
								pch = strtok(NULL,":");
								pch = strtok(pch,")");
								a = strtok(a,"(");
								a = strtok(NULL," ");
								a = strtok(NULL," ");
								a = strtok(NULL," ");
								strncpy(addr,a,strlen(a));
								strncat(port,pch,strlen(pch));
							}
						}
						if (strncmp(pch,newcamd,7) == 0) 
							*source = 2;
					}
				}
				else if (strstr(buf,"address:"))
				{	pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
					{
						char local[6] = "local";
						if (strncmp(pch,local,5) == 0)
							*source = 1;
						else 
						{	strcpy(buf1,buf);
							char *a;
							pch = strtok(buf1,":");
							pch = strtok(NULL,":");
							a = pch;
							pch = strtok(NULL,":");
							if (pch != NULL)
							{	
								a = strtok(a," ");
								if (a != NULL)
								{	
									strncpy(addr,a,strlen(a));
									strncat(port,pch,strlen(pch)-1);
									*source = 2;
								}
							}
						}
					}
				}
				else if (strstr(buf,"from:"))
				{	if (strstr(buf,"local")) 
						*source = 3;
					else 
					{	*source = 2;
						pch = strtok(buf1,":");
						pch = strtok(NULL,":");
						pch = strtok(pch," ");
						if (pch != NULL) strncpy(addr,pch,strlen(pch)-1);
					}
				}	
				else if (strstr(buf,"decode:"))
				{	*source = 1;
					char Network[8] = "Network";
					char Internal[9] = "Internal";
					pch = strtok(buf1,":");
					pch = strtok(NULL,":");
					pch = strtok(pch," ");
					if (pch != NULL)
					{	if (strncmp(pch,Network,7) == 0) *source = 2;
						else if (strncmp(pch,Internal,8) == 0) *source = 1;
						else
						{*source = 2;
							pch1 = strtok(buf,":");
							pch1 = strtok(NULL,":");
							pch1 = strtok(NULL,":");
							if (pch1 != NULL)
							{	strncpy(addr,pch,strlen(pch));
								strncpy(port,pch1,strlen(pch1)-1);
							}else strncpy(addr,pch,strlen(pch)-1);
						}
					}
				}
			}
			fclose(in);
		}
	}
	return;
}

void osdInfo::getLNBData(int * freq, int * sr, char * pol, char * fec, char * proname)
{
	eDVBServiceController *sapi = eDVB::getInstance()->getServiceAPI();
	if(sapi)
	{
		eServiceDVB *service=eDVB::getInstance()->settings->getTransponders()->searchService(sapi->service);
		if (service)
			strcpy(proname,filter_string(service->service_provider).c_str());
		eTransponder *tp = sapi->transponder;
		if (tp)
		{
			int tpData = 0;
			for (std::list<eLNB>::iterator it( eTransponderList::getInstance()->getLNBs().begin());
				tpData == 0 && it != eTransponderList::getInstance()->getLNBs().end(); it++)
				for (ePtrList<eSatellite>::iterator s ( it->getSatelliteList().begin());
					s != it->getSatelliteList().end(); s++)
					if (s->getOrbitalPosition() == tp->satellite.orbital_position) 
					{	tpData = 1;
						break;
					}
			if (tpData == 1)
			{
				*freq = tp->satellite.frequency / 1000;

				*sr = tp->satellite.symbol_rate / 1000;

				strcpy(pol,(tp->satellite.polarisation ? "V" : "H"));


				switch (tp->satellite.fec)
				{
					case 0: strcpy(fec, "Auto"); break;
					case 1: strcpy(fec, "1/2") ; break;
					case 2: strcpy(fec, "2/3") ; break;
					case 3: strcpy(fec, "3/4") ; break;
					case 4: strcpy(fec, "5/6") ; break;
					case 5: strcpy(fec, "7/8") ; break;
					case 6: strcpy(fec, "8/9") ; break;
				}
			}
		}
	}
}

osdInfo::~osdInfo()
{
	if (instance==this)
		instance=0;
}
