from Screens.Screen import Screen
from Components.ActionMap import NumberActionMap, ActionMap
from Components.config import config, ConfigNothing, ConfigFile
from Components.SystemInfo import SystemInfo
from Components.ConfigList import ConfigListScreen
from Components.Label import Label
from Components.Pixmap import Pixmap
from Components.Sources.List import List
from Components.Sources.StaticText import StaticText
from nemesisTool import GetSkinPath, createProxy, createInadynConf, createIpupdateConf, restartE2
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmapAlphaTest
from Tools.LoadPixmap import LoadPixmap
from enigma import eTimer
from os import system

import xml.etree.cElementTree

setupfile = file('/usr/share/enigma2/nemesis.xml', 'r')
setupdom = xml.etree.cElementTree.parse(setupfile)
setupfile.close()
configfile = ConfigFile()

class NSetupSum(Screen):

	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self["title"] = Label(_("System Settings"))
		self["key_red"] = Label(_("Exit"))
		self['conn'] = StaticText("")
		skin_path = GetSkinPath()
		self._restartE2 = restartE2(session)

		xmldata = setupdom.getroot()
		for x in xmldata.findall("setup"):
			requires = x.get("requires")
			if requires and not SystemInfo.get(requires, False):
				continue;
			self.list.append((x.get("key"), _(x.get("title", "").encode("UTF-8")), LoadPixmap(skin_path + 'icons/setup.png')))
			
		self['list'] = List(self.list)
		self["actions"] = ActionMap(['WizardActions','ColorActions'],
		{
			"ok": self.okbuttonClick,
			"red": self.close,
			'back': self.close
		})
		self.activityTimer = eTimer()
		self.onShown.append(self.setWindowTitle)
		self.RCStatus = config.usage.remote_control_setup.value
		self.E2Status = config.usage.set_e2_start_config.value
		self.PiconStatus = config.nemesis.piconlcd.value

	def setWindowTitle(self):
		self.setTitle(_("System Settings"))

	def okbuttonClick(self):
		self.indice = self["list"].getCurrent()[0]
		self.session.openWithCallback(self.saveConfig, NSetup, self.indice)
	
	def saveConfig(self, *ret):
		if ret:
			self['conn'].text = (_('Saving Setting.\nPlease wait...'))
			self.activityTimer.timeout.get().append(self.saveConf(ret[0]))
			self.activityTimer.start(100, False)
		
	def saveConf(self, ret):
		self.activityTimer.stop()
		if ret == "proxy":
			createProxy()
		elif ret == "inadyn":
			createInadynConf()
		elif ret == "ipupdate":
			createIpupdateConf()
		configfile.save()
		self['conn'].text = ('')
		if (self.RCStatus != config.usage.remote_control_setup.value 
				or self.E2Status != config.usage.set_e2_start_config.value 
				or self.PiconStatus != config.nemesis.piconlcd.value):
			msg = _("Please restart Enigma to apply the new configuration.") + "\n" + _("Do You want restart enigma2 now?")
			self._restartE2.go(msg, True)

class NSetup(ConfigListScreen, Screen):

	ALLOW_SUSPEND = True

	def refill(self, list):
		xmldata = setupdom.getroot()
		for x in xmldata.findall("setup"):
			if x.get("key") != self.setup:
				continue
			self.addItems(list, x);
			self.setup_title = _(x.get("title", "").encode("UTF-8"))

	def __init__(self, session, setup):
		Screen.__init__(self, session)

		self.onChangedEntry = [ ]

		self.setup_title = ""
		self.setup = setup
		list = []
		self.refill(list)
		self["oktext"] = Label(_("OK"))
		self["canceltext"] = Label(_("Exit"))
		
		self["actions"] = NumberActionMap(["SetupActions"], 
			{
				"cancel": self.keyCancel,
				"save": self.okPress,
			}, -2)

		ConfigListScreen.__init__(self, list, session = session, on_change = self.changedEntry)

		self.changedEntry()
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_(self.setup_title))
	
	def changedEntry(self):
		for x in self.onChangedEntry:
			x()

	def getCurrentEntry(self):
		return self["config"].getCurrent()[0]

	def getCurrentValue(self):
		return str(self["config"].getCurrent()[1].getText())
	
	def addItems(self, list, parentNode):
		for x in parentNode:
			if x.tag == 'item':
				item_level = int(x.get("level", 0))
				item_text = _(x.get("text", "??").encode("UTF-8"))
				item = eval(x.text or "");
				if item == "":
					continue
				if not isinstance(item, ConfigNothing):
					list.append( (item_text, item) )
	
	def okPress(self):
		self.saveAll()
		self.close(self.setup)
