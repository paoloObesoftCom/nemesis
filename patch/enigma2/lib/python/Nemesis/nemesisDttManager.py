from Screens.Screen import Screen
from Screens.Standby import TryQuitMainloop
from Screens.Ipkg import Ipkg
from Components.Sources.List import List
from Components.Ipkg import IpkgComponent
from Components.Label import Label
from Components.ActionMap import ActionMap
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmapAlphaTest
from Components.Sources.StaticText import StaticText
from Components.config import config
from Tools.LoadPixmap import LoadPixmap
from Tools.Directories import fileExists
from nemesisTool import GetSkinPath, restartE2, getVarSpaceKb
from nemesisConsole import nemesisConsole
from nemesisTunerList import tList
from enigma import eTimer
from os import path as os_path, listdir, unlink
import os

def readFile(myFile):
	try:
		mounts = open(myFile)
	except IOError:
		return 'None'
	
	line = mounts.readlines()
	mounts.close()
	return line[0].strip()

def checkDev():
	deviceDir = "/sys/bus/usb/devices/"
	devDir = listdir(deviceDir)
	myDevices = []
	for x in devDir[:]:
		idv = 'None'
		idp = 'None'
		if os_path.isdir(deviceDir + x):
			devFiles = listdir(deviceDir + x)
			for y in devFiles[:]:
				if os_path.isfile(deviceDir + x + "/" + y):
					if y == "idVendor":
						idv = readFile(deviceDir + x + "/" + y)
					if y == "idProduct":
						idp = readFile(deviceDir + x + "/" + y)
			myDevices.append("%s:%s" % (idv,idp))
	
	if myDevices:
		devicesList = []
		for myDevice in myDevices:
			for myTuner in tList.tunerList:
				if myDevice == myTuner[0]:
					devicesList.append(myTuner)
		if devicesList:
			return devicesList
		else:
			return None
	else:
		return None

class manageDttDevice(Screen):
	__module__ = __name__

	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self.devList = checkDev()
		self.devstatus = {}
		self.needInstall = False
		self.updating = True
		self['list'] = List(self.list)
		self["key_red"] = Label(_("Exit"))
		self["key_green"] = Label("")
		self['conn'] = StaticText("")
		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			'green': self.modulesInstall,
			'red': self.exit,
			'back': self.exit
		})
		self._restartE2 = restartE2(session)
		self.cmdList = []
		self.ipkg = IpkgComponent()
		self.ipkg.addCallback(self.ipkgCallback)
		self.timeoutTimer = eTimer()
		self.timeoutTimer.callback.append(self.doTimeoutTimer)
		self.onLayoutFinish.append(self.updateList)
		self.onShown.append(self.setWindowTitle)
	
	def exit(self):
		if not self.ipkg.isRunning():
			self.close()
		
	def setWindowTitle(self):
		self.setTitle(_("Manage DVB-T/C Adapter"))
		
	def KeyOk(self):
		if self.devList:
			sel = self["list"].getCurrent()[0]
			if (sel):
				self.devstatus[sel] = not self.devstatus[sel]
				self.saveStatus()
				self.updateList()
				if self.devstatus[sel]:
					msg = _('Enigma2 will be now hard restarted to enable DVB/T.please wait\nDo You want restart now?')
					self._restartE2.go(msg, True)
				else:
					msg = _('Dreambox will be now rebooted to disable DVB/T.\nDo You want reboot the box now?')
					self._restartE2.go(msg, True, True)
		else:
			self.close()

	def readStatus(self):
		try:
			if self.devList:
				for x in self.devList:
					self.devstatus[x[1]] = False
				if fileExists('/etc/dtt.devices'):
					loadedDevices = open('/etc/dtt.devices', 'r')
					for d in loadedDevices.readlines():
						for dev in self.devList:
							if dev[1] == d[:-1]:
								self.devstatus[dev[1]] = True
					loadedDevices.close()
			else:
				if fileExists('/etc/dtt.devices'):
					unlink("/etc/dtt.devices")
				return None
		except IOError:
			return None
	
	def saveStatus(self):
		out = open('/etc/dtt.devices', 'w')
		for dev in self.devList:
			if self.devstatus.get(dev[1]):
				out.write(dev[1] + '\n')
		out.close()
	
	def updateList(self):
		del self.list[:]
		if fileExists('/usr/script/loaddttmodules.sh'):
			self['conn'].text = ''
			self["key_green"].setText(_("Remove Driver"))
			self.readStatus()
			self.needInstall = False
			skin_path = GetSkinPath()
			if self.devList:
				for dev in self.devList:
					res = [dev[1]]
					png = LoadPixmap({ True:skin_path + 'menu/menu_on.png',False:skin_path + 'menu/menu_off.png' }[self.devstatus.get(dev[1])])
					self.list.append((dev[1], dev[2], png))
			self['list'].setList(self.list)
		else:
			self.needInstall = True
			self["key_green"].setText(_("Install Driver"))
			self.showInstMess()

	def showInstMess(self):
		diskSpace = getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		strMess = _("Modules for support\nUSB DVB-T/C adapter\nare not installed!\n\nPress green button\nto install it.")
		self['conn'].text = ("%s\n\nFree: %d kB (%d%%)" % (strMess, int(diskSpace[0]), percFree))

	def modulesInstall(self):
		if self.needInstall:
			if int(getVarSpaceKb()[0]) < 2500:
				self['conn'].setText(_('Not enough space!\nPlease delete addons before install new.'))
				return
			self['conn'].setText(_("Connetting to addons server.\nPlease wait..."))
			self.updating = True
			self.timeoutTimer.start(config.nemesis.ipkg.updateTimeout.value * 1000)
			self.ipkg.startCmd(IpkgComponent.CMD_UPDATE)
		else:
			self.cmdList = []
			self.cmdList.append((IpkgComponent.CMD_REMOVE, { "package": "dreambox-tuner-usb" }))
			if len(self.cmdList):
				self.session.openWithCallback(self.pakageRemoved, Ipkg, cmdList = self.cmdList)
			
	def doTimeoutTimer(self):
		self['conn'].text = _("Server connection timeout!\nPlease try again later.")
		self.ipkg.stop()

	def pakageRemoved(self):
		self.updateList()
		if fileExists('/etc/dtt.devices'):
			unlink("/etc/dtt.devices")
		msg = _('Dreambox will be now rebooted to disable DVB/T.\nDo You want reboot the box now?')
		self._restartE2.go(msg, True, True)

	def ipkgCallback(self, event, param):
		if event == IpkgComponent.EVENT_ERROR:
			self.updating = False
			self['conn'].text = (_("Server not found!\nPlease check internet connection."))
		elif event == IpkgComponent.EVENT_DONE:
			if self.timeoutTimer.isActive():
				self.timeoutTimer.stop()
			if self.updating:
				self.cmdList = []
				self.cmdList.append((IpkgComponent.CMD_INSTALL, { "package": "dreambox-tuner-usb" }))
				if len(self.cmdList):
					self.session.openWithCallback(self.updateList, Ipkg, cmdList = self.cmdList)
		pass
