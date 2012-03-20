from Screens.Screen import Screen
from Screens.ServiceInfo import ServiceInfo
from Screens.About import About
from Components.ActionMap import ActionMap
from Components.Label import Label
from Components.Pixmap import Pixmap
from Components.ProgressBar import ProgressBar
from Components.ScrollLabel import ScrollLabel
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmapAlphaTest
from Components.Sources.List import List
from Tools.LoadPixmap import LoadPixmap
from Tools.Directories import fileExists
from os import system, remove
from nemesisTool import GetSkinPath, getDeviceInfo
from enigma import eConsoleAppContainer
from Components.PluginComponent import plugins
from Components.PluginList import *
from Plugins.Plugin import PluginDescriptor
import thread


def getUnit(val):
	if val >= 1073741824:
		return '%.1f%s' % (float(val)/1073741824,' TB')
	if val >= 1048576:
		return '%.1f%s' % (float(val)/1048576,' GB')
	return '%.1f%s' % (float(val)/1024,' MB')

def getSize(a,b,c):
	return getUnit(a),getUnit(b),getUnit(c)

class NInfo(Screen):
	__module__ = __name__

	def getPluginsExt(self):
		plist = []
		pluginlist = plugins.getPlugins(PluginDescriptor.WHERE_EXTENSIONSMENU)
		for plugin in pluginlist:
			plist.append(PluginEntryComponent(plugin))
		for p in plist:
			if (p[0].name.find("CCcam Info") != -1):
				self.cccmaplugyn = p[0]
			if (p[0].name.find("Gbox Suite") != -1):
				self.gboxplugyn = p[0]
			if (p[0].name.find("OscamInfo") != -1):
				self.oscamplugyn = p[0]
		
	def getPluginsMenu(self):
		plist = []
		pluginlist = plugins.getPlugins(PluginDescriptor.WHERE_PLUGINMENU)
		for plugin in pluginlist:
			plist.append(PluginEntryComponent(plugin))
		for p in plist:
			if (p[0].name.find("CCcam Info") != -1):
				self.cccmaplugyn = p[0]
			if (p[0].name.find("Gbox Suite") != -1):
				self.gboxplugyn = p[0]
			if (p[0].name.find("OscamInfo") != -1):
				self.oscamplugyn = p[0]

	def __init__(self, session):
		Screen.__init__(self, session)
		self.c = eConsoleAppContainer()
		self.c.execute('free > /tmp/info_mem.tmp && ps > /tmp/ninfo.tmp')
		self.list = []
		self['list'] = List(self.list)
		self["key_red"] = Label(_("Exit"))
		self["title"] = Label(_("System Information"))
		self.cccmaplugyn = self.gboxplugyn = self.oscamplugyn = ''
		self.getPluginsExt()
		self.getPluginsMenu()
		self.infoMenuList = [
			('GboxInfo',_('Gbox Suite'),'icons/cams.png',self.gboxplugyn),
			('CInfo',_('CCcam Info'),'icons/cams.png',self.cccmaplugyn),
			('OInfo',_('Oscam Info'),'icons/cams.png',self.oscamplugyn),
			('DInfo',_('Show Device Status'),'icons/space.png',True),
			('PInfo',_('Show Active Process'),'icons/process.png',True),
			('SInfo',_('Show Service Info'),'icons/service.png',True),
			('EInfo',_('Show Enigma Settings'),'icons/enigma.png',True),
			('About',_('Nemesis Firmware Info'),'icons/about.png',True)
			]
		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			"red": self.__onClose,
			'back': self.__onClose
		})
		self.onLayoutFinish.append(self.updateList)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_("System Information"))
	
	def updateList(self):
		from nemesisTool import initDeviceInfo
		thread.start_new_thread(initDeviceInfo, ())
		self.list = []
		skin_path = GetSkinPath()
		for i in self.infoMenuList:
			if i[3]:
				self.list.append((i[0], i[1], LoadPixmap(skin_path + i[2])))
		self['list'].setList(self.list)

	def KeyOk(self):
		self.sel = self["list"].getCurrent()[0]
		if (self.sel == "GboxInfo"):
			if self.gboxplugyn:
				self.gboxplugyn(session=self.session, servicelist=self)
		elif (self.sel == "CInfo"):
			if self.cccmaplugyn:
				self.cccmaplugyn(session=self.session, servicelist=self)
		elif (self.sel == "OInfo"):
			if self.oscamplugyn:
				self.oscamplugyn(session=self.session, servicelist=self)
		elif (self.sel == "DInfo"):
				self.session.open(showDevSpaceInfo)
		elif (self.sel == "PInfo"):
				self.session.open(showRunProcessInfo)
		elif (self.sel == "SInfo"):
				self.session.open(ServiceInfo)
		elif (self.sel == "EInfo"):
				self.session.open(showEnigmaInfo)
		elif (self.sel == "About"):
				self.session.open(About)
				
	def __onClose(self):
		if fileExists('/tmp/ninfo.tmp'):
			remove('/tmp/ninfo.tmp')
		if fileExists('/tmp/info_mem.tmp'):
			remove('/tmp/info_mem.tmp')
		del self.c
		self.close()

class showRunProcessInfo(Screen):
	__module__ = __name__

	def __init__(self, session):
		Screen.__init__(self, session)
		self['titlebar'] = Label(' PID \t Uid \t Command')
		self['psinfo'] = ScrollLabel('')
		self['actions'] = ActionMap(['WizardActions',
			'DirectionActions'], {'ok': self.close,
			'back': self.close,
			'up': self['psinfo'].pageUp,
			'left': self['psinfo'].pageUp,
			'down': self['psinfo'].pageDown,
			'right': self['psinfo'].pageDown})
		
		self.onLayoutFinish.append(self.writepslist)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_("Active Process Information"))

	def writepslist(self):
		p = ''
		if fileExists('/tmp/ninfo.tmp'):
			f = open('/tmp/ninfo.tmp', 'r')
			for line in f.readlines():
				x = line.strip().split()
				if (x[0] == 'PID'):
					continue
				p += (x[0] + '\t')
				p += (x[1] + '\t')
				if len(x) > 4:
					p += x[4] + '\n'
				else:
					p += x[3] + '\n'
			f.close()
		self['psinfo'].setText(p)

class showDevSpaceInfo(Screen):
	__module__ = __name__

	def __init__(self, session):
		Screen.__init__(self, session)
		labelList = [
			('f1' , ''), ('f2' , ''),
			('l1' , ''), ('l2' , ''), ('l3' , ''), ('l4' , ''), ('l5' , ''),
			('rr2' , ''), ('rs2' , ''), ('rt1' , 'Total: '), ('rt2' , '')]
		self.progrList = ['rrpr','rspr','rtpr','flsp','p1','p2','p3','p4','p5']
		self.pixList = ['s1','s2','s3','s4','s5']
		
		for x in labelList:
			self[x[0]] = Label(x[1])
		for x in self.progrList:
			self[x] = ProgressBar()
		for x in self.pixList:
			self[x] = Pixmap()
		
		self['actions'] = ActionMap(['WizardActions'],
		{
			'ok': self.__onClose,
			'back': self.__onClose
		})
		self.onLayoutFinish.append(self.writelist)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_("Usage Device Status"))
		
	def writelist(self):
		for x in self.progrList:
			self[x].setValue(0)
		for x in self.pixList:
			self[x].hide()
		i = 1
		r = [0,0,0]
		
		for dev in getDeviceInfo():
			if dev[0] == "/":
				s = getSize(int(dev[2][1]),int(dev[2][1] - dev[2][0]),int(dev[2][0]))
				inUse = int((dev[2][1] - dev[2][0]) / dev[2][1] * 100)
				self['f1'].setText('%s: %s - In use: %d%%' % (dev[1],s[0], inUse))
				self['f2'].setText('%s: %s - Used: %s - Free: %s' % (dev[1],s[0],s[1],s[2]))
				self['flsp'].setValue(inUse)
			else:
				if i <= 5:
					try:
						s = getSize(int(dev[2][1]),int(dev[2][1] - dev[2][0]),int(dev[2][0]))
						inUse = int((dev[2][1] - dev[2][0]) / dev[2][1] * 100)
						self['s%d' % i].show()
						self['l%d' % i].setText('%s %s\nUsed: %s - Free: %s' % (dev[1],s[0],s[1],s[2]))
						self['p%d' % i].setValue(inUse)
						i += 1
					except:
						continue
					
		if fileExists('/tmp/info_mem.tmp'):
			f = open('/tmp/info_mem.tmp', 'r')
			for line in f.readlines():
				x = line.strip().split()
				if (x[0] == 'Mem:'):
					r[0] = int(int(x[2]) * 100 / int(x[1]))
					s = getSize(int(x[1]),int(x[2]),int(x[3]))
					self['rr2'].setText('Ram: %s Used: %s Free: %s Shared: %s Buf: %s' % (s[0],s[1],s[2],getUnit(int(x[4])),getUnit(int(x[5]))))
				elif (x[0] == 'Swap:'):
					if int(x[1]) > 1:
						r[1] = int(int(x[2]) * 100 / int(x[1]))
					s = getSize(int(x[1]),int(x[2]),int(x[3]))
					self['rs2'].setText('Swap: %s - Used: %s - Free: %s' % (s[0],s[1],s[2]))
				elif (x[0] == 'Total:'):
					r[2] = int(int(x[2]) * 100 / int(x[1]))
					self['rt1'].setText('Total Memory:  %s  in use:: %d%%' % (getUnit(int(x[1])), r[2]))
					s = getSize(int(x[1]),int(x[2]),int(x[3]))
					self['rt2'].setText('Total: %s - Used: %s - Free: %s' % (s[0],s[1],s[2]))
			f.close()
			self['rrpr'].setValue(r[0])
			self['rspr'].setValue(r[1])
			self['rtpr'].setValue(r[2])
				
	def __onClose(self):
		self.close()
		
class showEnigmaInfo(Screen):
	__module__ = __name__

	def __init__(self, session):
		Screen.__init__(self, session)
		self['infotext'] = ScrollLabel('')
		self['actions'] = ActionMap(['WizardActions',
			'DirectionActions'], {'ok': self.close,
			'back': self.close,
			'up': self['infotext'].pageUp,
			'left': self['infotext'].pageUp,
			'down': self['infotext'].pageDown,
			'right': self['infotext'].pageDown})
		self.onLayoutFinish.append(self.updatetext)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_("Enigma Setting Info"))

	def updatetext(self):
		strview = ''
		if fileExists('/etc/enigma2/settings'):
				f = open('/etc/enigma2/settings', 'r')
				for line in f.readlines():
					strview += line
				f.close()
				self['infotext'].setText(strview)
