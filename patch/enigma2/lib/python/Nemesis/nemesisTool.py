from Components.MenuList import MenuList
from Tools.Directories import fileExists
from Tools.Directories import resolveFilename, SCOPE_CURRENT_SKIN
from Tools.HardwareInfo import HardwareInfo
from enigma import eListboxPythonMultiContent, eListbox, gFont, getDesktop
from Components.config import config
from os import system, statvfs
import os, sys, base64 as getID
from Components.config import ConfigFile

configfile = ConfigFile()

def parse_ecm(filename):
	addr = caid =  pid =  provid =  port = reader = protocol = ""
	source =  ecmtime =  hops = 0
	try:
		file = open (filename)
		for line in file.readlines():
			line = line.strip()
			if line.find('CaID') >= 0:
				x = line.split(' ')
				caid = x[x.index('CaID')+1].split(',')[0].strip()
			elif line.find('caid') >= 0:
				x = line.split(':',1)
				caid = x[1].strip()
			if line.find('pid:') >= 0:
				x = line.split(':',1)
				pid = x[1].strip()
			elif line.find('pid') >= 0:
				x = line.split(' ')
				pid = x[x.index('pid')+1].strip()
			if line.find('prov:') >= 0:
				x = line.split(':',1)
				provid = x[1].strip().split(',')[0]
			elif line.find('provid:') >= 0:
				x = line.split(':',1)
				provid = x[1].strip()
			if line.find('msec') >= 0:
				x = line.split(' ',1)
				ecmtime = int(x[0].strip())
			elif line.find('response:') >= 0:
				x = line.split(':',1)
				try:
					ecmtime = int(float(x[1].strip()))
				except:
					ecmtime = 0
			elif line.find('ecm time:') >= 0:
				x = line.split(':',1)
				if x[1].strip() == "nan":
					ecmtime = 0
				else:
					try:
						ecmtime = int(float(x[1].strip()) * 1000)
					except:
						y = x[1].strip().split(' ',1)
						ecmtime = int(float(y[0].strip()))
			if line.find('hops:') >= 0:
				x = line.split(':',1)
				hops = int(x[1].strip())
			if line.find('reader:') >= 0:
				x = line.split(':',1)
				reader = x[1].strip()
			if line.find('protocol:') >= 0:
				x = line.split(':',1)
				protocol = x[1].strip()
			if line.find('using:') >= 0:
				x = line.split(':',1)
				if (x[1].strip() == "emu"):
					source = 1
				elif (x[1].strip() == "net") or (x[1].strip() == "newcamd") or (x[1].strip() == "CCcam-s2s") or (x[1].strip() == "gbox"):
					source = 2
			elif line.find('source:') >= 0:
				x = line.split(':')
				if x[1].strip() == "emu":
					source = 1
				elif x[1].find("net") >= 0:
					source = 2
					port = x[2].strip().split(")")[0]
					addr = x[1].split(' ')[4]
				elif x[1].strip() == "newcamd":
					source = 2
			elif line.find('address:') >= 0:
				x = line.split(':')
				if x[1].strip() != '':
					if x[1].find("/dev/sci0") >= 0:
						source = 4
					elif x[1].find("/dev/sci1") >= 0:
						source = 5
					elif x[1].find("local") >= 0:
						source = 1
					else:
						try:
							addr = x[1].strip()
							port = x[2].strip()
						except:
							addr = x[1].strip()
			elif line.find('from:') >= 0:
				if line.find("local") >= 0:
					source = 3
				else:
					source = 2
					x = line.split(':')
					addr = x[1].strip()
			elif line.find('slot-1') >= 0:
				source = 4
				if HardwareInfo().get_device_name() == 'dm800se':
					source = 5
			elif line.find('slot-2') >= 0:
				source = 5
				if HardwareInfo().get_device_name() == 'dm800se':
					source = 4
			elif line.find('decode:') >= 0:
				if line.find('Internal') >= 0:
					source = 1
				elif line.find('Network') >= 0:
					source = 2
				elif line.find('/dev/sci0') >= 0:
					source = 4
					if HardwareInfo().get_device_name() == 'dm800se':
						source = 5
				elif line.find('/dev/sci1') >= 0:
					source = 5
					if HardwareInfo().get_device_name() == 'dm800se':
						source = 4
				else:
					source = 2
					x = line.split(':')
					if x[1].strip() != '':
						try:
							addr = x[1].strip()
							port = x[2].strip()
						except:
							addr = x[1].strip()
		file.close()
		return caid, pid, provid, ecmtime, source, addr, port, hops, reader, protocol
	except:
		return 0
	
def createProxy():
	urlS = "http://"
	urlS += config.proxy.user.value.strip()
	urlS += ":"
	urlS += config.proxy.password.value.strip()
	urlS += "@"
	urlS += config.proxy.server.value.strip()
	urlS += ":"
	urlS += str(config.proxy.port.value)
	urlS += "\n"
	out = open('/var/etc/proxy.sh', 'w')
	out.write('#!/bin/sh\n')
	out.write('export http_proxy=' + urlS)
	out.close()
	system("chmod 755 /var/etc/proxy.sh")

def createInadynConf():
	out = open('/etc/inadyn.conf', 'w')
	out.write('username ' + config.inadyn.user.value.strip() + '\n')
	out.write('password ' + config.inadyn.password.value.strip() + '\n')
	out.write('alias ' + config.inadyn.alias.value.strip() + '\n')
	out.write('update_period_sec ' + str(config.inadyn.period.value) + '\n')
	out.write('dyndns_system ' + config.inadyn.system.value.strip() + '\n')
	out.write('log_file ' + config.inadyn.log.value.strip() +'/inadyn.log\n')
	out.write('background\n')
	out.close()

def createIpupdateConf():
	out = open('/etc/ipupdate.conf', 'w')
	out.write('service-type=' + config.ipupdate.system.value.strip() + '\n')
	out.write('user=' + config.ipupdate.user.value.strip() + ':' + config.ipupdate.password.value.strip() + '\n')
	out.write('host=' + config.ipupdate.alias.value.strip() + '\n')
	out.write('server=' + config.ipupdate.server.value.strip() + '\n')
	out.write('interface=eth0\n')
	out.write('quiet\n')
	out.write('period=' + str(config.ipupdate.period.value) + '\n')
	out.close()

def getUsrID(a):
		return getID.b64decode('b%s=' % a)
	
def isE232():
		if fileExists('/etc/.enigma_nemesis'):
			return False
		return True

def GetSkinPath():
	cur_skin_path = resolveFilename(SCOPE_CURRENT_SKIN, '')
	if (cur_skin_path == '/usr/share/enigma2/'):
		cur_skin_path = '/usr/share/enigma2/skin_default/'
	return cur_skin_path

def cleanServiceHistoryList(slist):
	if config.nemesis.enableclean.value:
		slist.history = [ ]
		slist.history_pos = 0
	else:
		slist.recallPrevService()
	return config.nemesis.zapafterclean.value and config.nemesis.enableclean.value

class ListboxE1(MenuList):
	__module__ = __name__

	def __init__(self, list, enableWrapAround = False):
		MenuList.__init__(self, list, enableWrapAround, eListboxPythonMultiContent)
		self.l.setFont(0, gFont('Regular', 20))
		self.l.setFont(1, gFont('Regular', 22))
		self.l.setItemHeight(36)

class ListboxE2(MenuList):
	__module__ = __name__

	def __init__(self, list, enableWrapAround = False):
		MenuList.__init__(self, list, enableWrapAround, eListboxPythonMultiContent)
		self.l.setFont(0, gFont('Regular', 20))
		self.l.setFont(1, gFont('Regular', 18))
		self.l.setItemHeight(60)

class ListboxE3(MenuList):
	__module__ = __name__

	def __init__(self, list, enableWrapAround = False):
		MenuList.__init__(self, list, enableWrapAround, eListboxPythonMultiContent)
		self.l.setFont(0, gFont('Regular', 20))
		self.l.setFont(1, gFont('Regular', 20))
		self.l.setItemHeight(30)

class ListboxE4(MenuList):
	__module__ = __name__

	def __init__(self, list, enableWrapAround = False):
		MenuList.__init__(self, list, enableWrapAround, eListboxPythonMultiContent)
		self.l.setFont(0, gFont('Regular', 18))
		self.l.setFont(1, gFont('Regular', 18))
		self.l.setItemHeight(25)

from Screens.ChoiceBox import ChoiceBox
from Tools import Notifications

class restartE2:

	list = (
		(_("Yes"), "restart"),
		(_("No"), "no_restart")
	)

	def __init__(self, session):
		self.session = session

	def go(self, msg, modeClean = False, reboot = False):
		try:
			if reboot:
				Notifications.AddNotificationWithCallback(self.rebootDB, ChoiceBox, title=msg, list = self.list)
			else:
				if modeClean:
					Notifications.AddNotificationWithCallback(self.restartEnigma2Clean, ChoiceBox, title=msg, list = self.list)
				else:	
					Notifications.AddNotificationWithCallback(self.restartEnigma2, ChoiceBox, title=msg, list = self.list)
		except:
			self.restartEnigma2(True)
			
	def restartEnigma2(self, answer):
		answer = answer and answer[1]
		if answer == "restart":
			configfile.save()
			system("touch /etc/.reboot_ok")
			system("killall -9 enigma2")
	
	def restartEnigma2Clean(self, answer):
		answer = answer and answer[1]
		if answer == "restart":
			from Screens.Standby import TryQuitMainloop
			self.session.open(TryQuitMainloop, 3)

	def rebootDB(self, answer):
		answer = answer and answer[1]
		if answer == "restart":
			from Screens.Standby import TryQuitMainloop
			self.session.open(TryQuitMainloop, 2)

class editBlacklist:

	EPGBLACKLIST = '/etc/enigma2/epg.blacklist'

	def dec2hex(self,n):    
		return "%X" % n 
		
	def hex2dec(self,s):  
		return int(s, 16)
		
	def add(self,service):
		s = service.split(":")
		serv2add = "%d,%d,%d,%d" % (self.hex2dec(s[3]),self.hex2dec(s[4]),self.hex2dec(s[5]),self.hex2dec(s[2]))
		try:
			f = open(self.EPGBLACKLIST,'a')
			f.write("%s\n" % serv2add)
			f.close()
			print "[EPGBlacklist] service %s added to Blacklist." % serv2add
		except:
			pass

	def remove(self,service):
		s = service.split(":")
		serv2remove = "%d,%d,%d,%d" % (self.hex2dec(s[3]),self.hex2dec(s[4]),self.hex2dec(s[5]),self.hex2dec(s[2]))
		try:
			f = open(self.EPGBLACKLIST, 'r')
			fileContent = f.readlines()
			f.close()
			f = open(self.EPGBLACKLIST, 'w')
			for line in fileContent:
				if line.find(serv2remove) == -1:
					f.write(line)
			f.close()
			print "[EPGBlacklist] service %s removed form Blacklist." % serv2remove
		except:
			pass
		
	def check(self,service):
		s = service.split(":")
		serv2check = "%d,%d,%d,%d" % (self.hex2dec(s[3]),self.hex2dec(s[4]),self.hex2dec(s[5]),self.hex2dec(s[2]))
		try:
			f = open(self.EPGBLACKLIST, 'r')
			for line in f.readlines():
				if line.find(serv2check) >= 0:
					f.close()
					return True
			f.close()
			return False
		except:
			return False

class nemesisTool:
	
	def readPortNumber(self):	
		try:
			f = open("/var/etc/nemesis.cfg", "r")
			for line in f.readlines():
				if line.find("daemon_port=") >= 0:
					f.close()
					return line.split("=") [1] [:-1]
			f.close()
		except:
			return "1888"
			
	def readEmuName(self,emu):	
		try:
			f = open("/var/script/" + emu + "_em.sh", "r")
			for line in f.readlines():
				if line.find("#emuname=") >= 0:
					f.close()
					return line.split("=") [1] [:-1]
			f.close()
			return emu
		except:
			return "None"
	
	def readSrvName(self,srv):	
		try:
			f = open("/var/script/" + srv + "_cs.sh", "r")
			for line in f.readlines():
				if line.find("#srvname=") >= 0:
					f.close()
					return line.split("=") [1] [:-1]
			f.close()
			return srv
		except:
			return "None"

	def getScriptName(self,script):	
		try:
			f = open("/usr/script/" + script + "_user.sh", "r")
			for line in f.readlines():
				if line.find("#scriptname=") >= 0:
					f.close()
					return line.split("=") [1] [:-1]
			f.close()
			return script
		except:
			return "None"
	
	def getScriptCanDelete(self,script):	
		try:
			f = open("/usr/script/" + script + "_user.sh", "r")
			for line in f.readlines():
				if line.find("#candelete=") >= 0:
					f.close()
					return line.split("=") [1] [:-1]
			f.close()
			return script
		except:
			return "None"
	
	def readEcmInfoFile(self,emu):	
		try:
			f = open("/var/script/" + emu + "_em.sh", "r")
			for line in f.readlines():
				if line.find("#ecminfofile=") >= 0:
					f.close()
					return "/tmp/" + line.split("=") [1] [:-1]
			f.close()
		except:
			return "/tmp/ecm.info"
	
	def readEmuActive(self):	
		try:
			f = open("/var/bin/emuactive", "r")
			line = f.readline()
			f.close()
			return line [:-1]
		except:
			return "None"

	def readSrvActive(self):	
		try:
			f = open("/var/bin/csactive", "r")
			line = f.readline()
			f.close()
			return line  [:-1]
		except:
			return "None"
	
	def readEmmInterval(self):	
		try:
			f = open("/var/etc/nemesis.cfg", "r")
			for line in f.readlines():
				if line.find("emm_interval=") >= 0:
					f.close()
					return int(line.split("=") [1] [:-1])
			f.close()
		except:
			return int(1000)
	
	def readAddonsUrl(self):	
		try:
			#os.system("dos2unix /var/etc/addons.url");
			f = open("/var/etc/addons.url", "r")
			line = f.readline()
			f.close()
			return line [:-1]
		except:
			return "http://nemesis.tv/Nemesis/"

	def readExtraUrl(self):	
		try:
			#os.system("dos2unix /var/etc/extra.url");
			f = open("/var/etc/extra.url", "r")
			line = f.readline() [:-1]
			f.close()
			return line
		except:
			return None
	
	def getVarSpace(self):
		free = -1
		try:
			s = statvfs("/")
		except OSError:
			return free
		free = s.f_bfree/1024 * s.f_bsize/1024
		return s.f_bfree/1024 * (s.f_bsize / 1024)	

	def getVarSpaceKb(self):
		try:
			s = statvfs("/")
		except OSError:
			return 0,0
		return float(s.f_bfree * (s.f_bsize / 1024)), float(s.f_blocks * (s.f_bsize / 1024))

	def readEcmInfo(self):	
		emuActive = self.readEmuActive()
		info = parse_ecm(self.readEcmInfoFile(emuActive))
		if info != 0:
			caid = info[0]
			pid = info[1]
			provid = info[2]
			ecmtime = info[3]
			source = info[4]
			addr = info[5]
			port = info[6]
			hops = info[7]
			reader = info[8]
			protocol = info[9]
			returnMsg = ''
			if provid !='':
				returnMsg += "Provider: " + provid + "\n"
			if caid !='':
				returnMsg += "Ca ID: " + caid + "\n"
			if pid !='':
				returnMsg += "Pid: " + pid + "\n"
			if source == 0:
				returnMsg += "Decode: Unsupported!\n"
			elif source == 1:
				returnMsg += "Decode: Internal\n"
			elif source == 2:
				returnMsg += "Decode: Network\n"
				if config.nemesis.shownetdet.value:
					if addr !='':
						returnMsg += "  Source: " + addr + "\n"
					if port !='':
						returnMsg += "  Port: " + port + "\n"
					if hops > 0:
						returnMsg += "  Hops: " + str(hops) + "\n"
					if protocol !='':
						returnMsg += "  Protocol: " + protocol + "\n"
					if reader !='':
						returnMsg += "  Reader: " + reader + "\n"
			elif source == 3:
				returnMsg += "Decode: " + reader + "\n"
			elif source == 4:
				returnMsg += "Decode: slot-1\n"
			elif source == 5:
				returnMsg += "Decode: slot-2\n"
			if ecmtime > 0:
				returnMsg += "ECM Time: " + str(ecmtime) + " msec\n"
			return returnMsg
		else:
			return "No Info"
