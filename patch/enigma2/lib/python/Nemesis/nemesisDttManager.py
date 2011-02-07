from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from Screens.Standby import TryQuitMainloop
from Screens.Ipkg import Ipkg
from Components.Sources.List import List
from Components.Ipkg import IpkgComponent
from Components.Label import Label
from Components.ActionMap import ActionMap
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmapAlphaTest
from Components.Sources.StaticText import StaticText
from Tools.LoadPixmap import LoadPixmap
from Tools.Directories import fileExists
from nemesisTool import GetSkinPath
from nemesisConsole import nemesisConsole
from nemesisTunerList import tList
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
	
	skin = """
		<screen position="80,95" size="560,430">
			<widget source="conn" render=Label position="10,10" size="540,340" font="Regular;20" halign="center" transparent="1" />
			<widget source="list" render="Listbox" position="10,10" size="540,340" scrollbarMode="showOnDemand">
				<convert type="TemplatedMultiContent">
					{"template": [
							MultiContentEntryText(pos = (0, 0), size = (400, 30), font=0, flags = RT_HALIGN_LEFT | RT_HALIGN_LEFT, text = 1),
							MultiContentEntryPixmapAlphaTest(pos=(405, 6), size=(80, 23), png=png),
						],
					"fonts": [gFont("Regular", 20)],
					"itemHeight": 30
					}
				</convert>
			</widget>
			<widget name="key_red" position="0,400" size="280,20" zPosition="1" font="Regular;22" valign="center" foregroundColor="#0064c7" backgroundColor="#9f1313" transparent="1" />
			<widget name="key_yellow" position="400,510" size="280,20" zPosition="1" font="Regular;22" valign="center" foregroundColor="#bab329" backgroundColor="#9f1313" transparent="1" />
		</screen>"""

	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self.devList = checkDev()
		self.devstatus = {}
		self.needInstall = False
		self.list_updating = True
		self['list'] = List(self.list)
		self["key_red"] = Label(_("Exit"))
		self["key_yellow"] = Label(_("Install"))
		self["key_yellow"].hide()
		self['conn'] = StaticText("")
		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			'yellow': self.modulesInstall,
			'red': self.close,
			'back': self.close
		})
		self.cmdList = []
		self.ipkg = IpkgComponent()
		self.ipkg.addCallback(self.ipkgCallback)
		self.onLayoutFinish.append(self.updateList)
		self.onShown.append(self.setWindowTitle)
	
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
					box = self.session.openWithCallback(self.restartEnigma2, MessageBox, msg , MessageBox.TYPE_YESNO)
					box.setTitle(_('Restart Enigma2'))
				else:
					msg = _('Dreambox will be now rebooted to disable DVB/T.\nDo You want reboot the box now?')
					box = self.session.openWithCallback(self.rebootDream, MessageBox, msg , MessageBox.TYPE_YESNO)
					box.setTitle(_('Reboot Dreambox'))
		else:
			self.close()

	def restartEnigma2(self, answer):
		if (answer is True):
			self.session.open(TryQuitMainloop, 3)
	
	def rebootDream(self, answer):
		if (answer is True):
			self.session.open(TryQuitMainloop, 2)

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
			self["key_yellow"].hide()
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
			self["key_yellow"].show()
			self['conn'].text = (_("Modules for support\nUSB DVB-T/C adapter\nare not installed!\n\nPress yellow button\nto install it."))

	def modulesInstall(self):
		if self.needInstall:
			self['conn'].setText(_("Connetting to addons server.\nPlease wait..."))
			self.list_updating = True
			self.ipkg.startCmd(IpkgComponent.CMD_UPDATE)
			
	def runUpgrade(self, result):
		if result:
			self.session.openWithCallback(self.updateList, Ipkg, cmdList = self.cmdList)
		else:
			self['conn'].text = (_("Modules for support\nUSB DVB-T/C adapter\nare not installed!\n\nPress yellow button\nto install it."))
			
	def ipkgCallback(self, event, param):
		if event == IpkgComponent.EVENT_ERROR:
			self.list_updating = False
			self['conn'].text = (_("Server not found!\nPlease check internet connection."))
		elif event == IpkgComponent.EVENT_DONE:
			if self.list_updating:
				self.cmdList = []
				self.cmdList.append((IpkgComponent.CMD_INSTALL, { "package": "dreambox-tuner-usb" }))
				if len(self.cmdList):
					self.session.openWithCallback(self.runUpgrade, MessageBox, _("Do you want to install the support\npackages, for DVB-T/C adapters?") + _("\nAfter pressing OK, please wait!"), MessageBox.TYPE_YESNO )
		pass
