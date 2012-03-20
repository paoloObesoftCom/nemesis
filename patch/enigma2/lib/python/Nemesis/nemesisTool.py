from Components.MenuList import MenuList
from Tools.Directories import fileExists
from Tools.Directories import resolveFilename, SCOPE_CURRENT_SKIN
from Tools.HardwareInfo import HardwareInfo
from enigma import eListboxPythonMultiContent, eListbox, gFont, getDesktop, eEnv
from Components.config import config, ConfigFile
from os import system, statvfs, path as os_path, remove as os_remove, walk
import os, sys, base64 as getID

configfile = ConfigFile()

PICONSEARCHPATH = []
PLUGININSTALLPATH = []
EPGPATH = []
SKINPATH = []
SWAPDEVICE = []
DEVICEINFO = []

def getVarSpace():
	free = -1
	try:
		s = statvfs("/")
	except OSError:
		return free
	free = s.f_bfree/1024 * s.f_bsize/1024
	return s.f_bfree/1024 * (s.f_bsize / 1024)	

def getVarSpaceKb():
	try:
		s = statvfs("/")
	except OSError:
		return 0,0
	return float(s.f_bfree * (s.f_bsize / 1024)), float(s.f_blocks * (s.f_bsize / 1024))

def getDeviceKb(dev):
	try:
		s = statvfs(dev)
	except OSError:
		return 0,0
	return float(s.f_bavail * (s.f_bsize / 1024)), float(s.f_blocks * (s.f_bsize / 1024))

def checkDevice(device):
	system('touch %s/test.test' % device)
	if os_path.isfile('%s/test.test' % device):
		os_remove('%s/test.test' % device)
		return True
	return False

def initNemesisTool():
	from Components.Harddisk import harddiskmanager
	print "[ENUMERATE DEVICE] enumerate mounted devices..."
	del PICONSEARCHPATH[:]
	del PLUGININSTALLPATH[:]
	del EPGPATH[:]
	del SWAPDEVICE[:]
	del SKINPATH[:]
	hddPartition = []
	flashSpace = getVarSpaceKb()
	for partition in harddiskmanager.getMountedPartitions():
		if (partition.mountpoint != '/') \
				and str(partition.mountpoint).find('/autofs') == -1 \
				and (partition.mountpoint != ''):
			if not partition.device:
				if not checkDevice(partition.mountpoint):
					continue
			description = partition.description
			if partition.device:
				try:
					physdev = os_path.realpath('/sys/block/' + partition.device[:3] + '/device')[4:]
					description = harddiskmanager.getUserfriendlyDeviceName(partition.device[:3], physdev)
				except:
					description = partition.description
			if str(partition.mountpoint).find("/media/net") >= 0:
				description = _("Network Resource %s") % partition.description
			if description.find("SATA") >= 0 or description.find("Hard disk") >= 0:
				hddPartition.append((description, partition.mountpoint))
			else:
				if partition.device:
					SWAPDEVICE.append((partition.mountpoint,"%s (%s)" % (description, partition.mountpoint)))
					SKINPATH.append(("%s (%s)" % (description, "%s/skins" % partition.mountpoint),"%s/skins" % partition.mountpoint))
				PICONSEARCHPATH.append(partition.mountpoint + '/%s/')
				PLUGININSTALLPATH.append(("%s (%s)" % (description, partition.mountpoint),partition.mountpoint))
				EPGPATH.append(("%s (%s)" % (description, partition.mountpoint),partition.mountpoint))
	PICONSEARCHPATH.append(eEnv.resolve('${datadir}/%s/'))
	PICONSEARCHPATH.append(eEnv.resolve('${datadir}/enigma2/%s/'))
	SKINPATH.append((_("%s - [ Free: %d KB ]") % (_('Internal flash (unsuggested)'),int(flashSpace[0]) ),eEnv.resolve('${datadir}/enigma2')))
	PLUGININSTALLPATH.append((_("%s - [ Free: %d KB ]") % (_('Internal flash (unsuggested)'),int(flashSpace[0]) ),eEnv.resolve('${datadir}/enigma2')))
	EPGPATH.append((_('Temporary Folder (/tmp)'),'/tmp'))

	from Plugins.SystemPlugins.NetworkBrowser.AutoMount import iAutoMount
	for partition in iAutoMount.getMountsList():
		entryExist = False
		path = "/media/net/%s" % partition
		description = _("Network Resource %s") % partition
		if os_path.ismount(path) \
				and os_path.exists(path) \
				and checkDevice(path):
			for item in EPGPATH:
				if item[1] == path:
					entryExist = True
			if not entryExist:
					EPGPATH.append(("%s (%s)" % (description, path),path))
			for item in PICONSEARCHPATH:
				if item == path:
					entryExist = True
			if not entryExist:
				PICONSEARCHPATH.append(path + '/%s/')
			for item in PLUGININSTALLPATH:
				if item[1] == path:
					entryExist = True
			if not entryExist:
				PLUGININSTALLPATH.append(("%s (%s)" % (description, path),path))

	for item in hddPartition:
		if config.nemesis.usepiconinhdd.value:
			PICONSEARCHPATH.append(item[1] + '/%s/')
		if config.nemesis.usehddforexternal.value:
			SWAPDEVICE.append((item[1],"%s (%s)" % (item[0], item[1])))
			SKINPATH.append(("%s (%s)" % (item[0], "%s/skins" % item[1]),"%s/skins" % item[1]))
			EPGPATH.append(("%s (%s)" % (item[0], item[1]),item[1]))
			PLUGININSTALLPATH.append(("%s (%s)" % (item[0], item[1]),item[1]))
	
	for item in PLUGININSTALLPATH:
		print "[PLUGININSTALLPATH] %s, %s" % (item[0], item[1])

	for item in PICONSEARCHPATH:
		print "[PICONSEARCHPATH] %s" % (item)

	for item in SWAPDEVICE:
		print "[SWAPDEVICE] %s, %s" % (item[0], item[1])

	for item in SKINPATH:
		print "[SKINPATH] %s, %s" % (item[0], item[1])

	for item in EPGPATH:
		print "[EPGPATH] %s, %s" % (item[0], item[1])

def GetPiconPath():
	return PICONSEARCHPATH

def GetSkinsPath():
	return SKINPATH

def GetPluginInstallPath():
	return PLUGININSTALLPATH

def GetEpgPath():
	return EPGPATH

def GetSwapDevice():
	return SWAPDEVICE

def initSwap():
	print "[SWAP] Activating Swapfile..."
	for path in GetSwapDevice():
		path = path[0]
		if fileExists("%s/swapfile" % path):
			system("swapon %s/swapfile" % path)
			print "[SWAP] on %s Activate" % path
			return
	print "[SWAP] No swap files found!"

def initDeviceInfo():
	from Components.Harddisk import harddiskmanager
	print "[ENUMERATE DEVICEINFO] enumerate mounted devices..."
	del DEVICEINFO[:]
	for partition in harddiskmanager.getMountedPartitions():
		print "[PARTITION] %s, %s" % (partition.description, partition.mountpoint)
		if (partition.mountpoint != '')  \
				and str(partition.mountpoint).find('/autofs') == -1:
			if not partition.device:
				if not checkDevice(partition.mountpoint):
					continue
			description = partition.description
			if partition.device:
				try:
					physdev = os_path.realpath('/sys/block/' + partition.device[:3] + '/device')[4:]
					description = harddiskmanager.getUserfriendlyDeviceName(partition.device[:3], physdev)
				except:
					description = partition.description
			if str(partition.mountpoint).find("/media/net") >= 0:
				description = _("Network Resource %s") % description
			if partition.mountpoint == "/":
				DEVICEINFO.append((partition.mountpoint,description,getVarSpaceKb()))
			else:
				DEVICEINFO.append((partition.mountpoint,description,getDeviceKb(partition.mountpoint)))

def getDeviceInfo():
	return DEVICEINFO

def initEpg():
	epgFile = config.misc.epgcache_filename.value
	print "[EPG] Restoring EPG data..."
	if fileExists(epgFile):
		print "[EPG] %s found!" % epgFile
		return True
	elif fileExists("%s.save" % epgFile):
		print "[EPG] %s.save found!" % epgFile
		system("cp %s.save %s" % (epgFile,epgFile))
		print "[EPG] %s.save restored on %s!" % (epgFile,epgFile)
		return True
	else:
		for path in GetEpgPath():
			path = path[1]
			if fileExists("%s/epg.dat" % path):
				system("cp %s/epg.dat %s" % (path,epgFile))
				print "[EPG] epg.dat found on %s copied on %s" % (path,epgFile)
				return True
			else:
				if fileExists("%s/epg.dat.save" % path):
					print "[EPG] restoring %s/epg.dat.save..." % path
					system("cp %s/epg.dat.save %s" % (path,epgFile))
					print "[EPG] %s/epg.dat.save restored on %s!" % (path,epgFile)
					return True
				if fileExists("%s/ext.epg.dat" % path):
					print "[EPG] restoring %s/ext.epg.dat..." % path
					system("cp %s/ext.epg.dat %s" % (path,epgFile))
					print "[EPG] %sext.epg.dat restored on %s!" % (path,epgFile)
					return True
	print "[EPG] No EPG data found!"
	return False

def initializeEpg():
	epgFile = config.misc.epgcache_filename.value
	if config.nemepg.loadonboot.value:
		initEpg()
	else:
		if fileExists(epgFile):
			print "[EPG] Remove %s" % epgFile
			os_remove('rm -f %s' % epgFile)

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
	
def createSkinUninstall(fileName, installPath):
	dirname = ''
	for root, dirs, files in walk("/tmp/install/skins"):
		for file in files:
			if file == "skin.xml":
				root = root.split('/')
				dirname = root[len(root) - 1]
				break
	linkPath = '%s%s' % (eEnv.resolve('${datadir}/enigma2/'),dirname)
	skinPath = '%s/%s' % (installPath,dirname)
	entry = "#!/bin/sh\nrm -rf %s\nrm -f %s\nrm -f $0\nkillall -9 enigma2\nexit 0\n" % \
		( skinPath, \
			linkPath )
	open('/usr/uninstall/%s_remove.sh' % fileName[:-5], 'w').write(entry)
	system('chmod 755 /usr/uninstall/%s_remove.sh' % fileName[:-5])

def createProxy():
	entry = "#!/bin/sh\nexport http_proxy=http://%s:%s@%s:%d\n\n" % \
		( config.proxy.user.value.strip(), \
			config.proxy.password.value.strip(), \
			config.proxy.server.value.strip(), \
			config.proxy.port.value )
	open('/var/etc/proxy.sh', 'w').write(entry)
	system("chmod 755 /var/etc/proxy.sh")

def createInadynConf():
	entry = 'username %s\npassword %s\nalias %s\nupdate_period_sec %d\ndyndns_system %s\nlog_file %s/inadyn.log\nbackground\n' % \
			( config.inadyn.user.value.strip(), \
				config.inadyn.password.value.strip(), \
				config.inadyn.alias.value.strip(), \
				config.inadyn.period.value, \
				config.inadyn.system.value.strip(), \
				config.inadyn.log.value.strip() )
	open('/etc/inadyn.conf', 'w').write(entry)

def createIpupdateConf():
	entry = 'service-type=%s\nuser=%s:%s\nhost=%s\nserver=%s\ninterface=eth0\nquiet\nperiod=%d\n' % \
			( config.ipupdate.system.value.strip(), \
				config.ipupdate.user.value.strip(), \
				config.ipupdate.password.value.strip(), \
				config.ipupdate.alias.value.strip(), \
				config.ipupdate.server.value.strip(), \
				config.ipupdate.period.value )
	open('/etc/ipupdate.conf', 'w').write(entry)

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
		line = []
		try:
			#os.system("dos2unix /var/etc/addons.url");
			f = open("/var/etc/addons.url", "r")
			line.append(f.readline()[:-1])
			line.append(f.readline()[:-1])
			f.close()
			return line
		except:
			return "http://nemesis.tv/Nemesis/", "addonsE2_26.xml"

	def readExtraUrl(self):	
		try:
			#os.system("dos2unix /var/etc/extra.url");
			f = open("/var/etc/extra.url", "r")
			line = f.readline() [:-1]
			f.close()
			return line
		except:
			return None

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
