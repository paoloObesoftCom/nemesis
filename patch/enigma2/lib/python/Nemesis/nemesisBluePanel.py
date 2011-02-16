from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from Screens.ParentalControlSetup import ParentalControlSetup
from Screens.PluginBrowser import PluginBrowser
from Components.ActionMap import ActionMap, NumberActionMap
from Components.FileList import FileList
from Components.Label import Label
from Components.ScrollLabel import ScrollLabel
from Components.ConfigList import ConfigList
from Components.config import ConfigSelection, getConfigListEntry, ConfigNothing, KEY_LEFT, KEY_RIGHT, KEY_OK
from Components.Pixmap import Pixmap
from Components.Sources.StaticText import StaticText
from Tools.Directories import fileExists
from Tools.HardwareInfo import HardwareInfo
from nemesisTool import *
from nemesisShowPanel import nemesisShowPanel
from nemesisConsole import nemesisConsole
from nemesisInfo import NInfo
from nemesisUtility import NUtility
from nemesisSetting import NSetupSum
from nemesisAddons import NAddons
from Components.Console import Console
from Components.About import about
import os
from os import unlink
from enigma import eTimer, eDVBCI_UI, nemTool,iServiceInformation

t = nemesisTool()
tool = nemTool()

class nemesisBluePanel(Screen):
	
	skin = """
	<screen name="nemesisBluePanel" position="0,0" size="500,720" flags="wfNoBorder">
		<widget name="title" position="50,40" size="300,32" font="Regular;40" halign="center" foregroundColor="#53a9ff" backgroundColor="transpBlack" transparent="1"/>
		<widget name="info_use" position="50,110" zPosition="2" size="340,30" valign="center" halign="center" font="Regular;21" transparent="1" backgroundColor="#1f771f" shadowColor="black" shadowOffset="-1,-1" />
		<widget name="config" position="50,150" size="340,125" zPosition="2" />
		<widget name="key_blue" position="50,320" zPosition="2" size="300,30" valign="center" halign="left" font="Regular;22" transparent="1" foregroundColor="#6565ff" backgroundColor="#1f771f" shadowColor="black" shadowOffset="-1,-1" />
		<widget name="key_yellow" position="50,350" zPosition="2" size="300,30" valign="center" halign="left" font="Regular;22" transparent="1" foregroundColor="#bab329" backgroundColor="#1f771f" shadowColor="black" shadowOffset="-1,-1" />
		<widget name="key_green" position="50,380" zPosition="2" size="300,30" valign="center" halign="left" font="Regular;22" transparent="1" foregroundColor="#389416" backgroundColor="#1f771f" shadowColor="black" shadowOffset="-1,-1" />
		<widget name="key_red" position="50,410" zPosition="2" size="300,30" valign="center" halign="left" font="Regular;22" transparent="1" foregroundColor="#ff2525" backgroundColor="#9f1313" shadowColor="black" shadowOffset="-1,-1" />
		<widget name="ecmtext" position="50,460" size="340,240" font="Regular;18" zPosition="2" backgroundColor="#333333" transparent="1"/>
		<widget name="conn" position="0,360" size="540,50" font="Regular;20" halign="center" valign="center" />
	</screen>"""
	
	NEWDEVER = "2.1 (OE 1.6)"
	IMAGEVER = about.getImageVersionString()
	ENIGMAVER = about.getEnigmaVersionString()
	KERNELVER = about.getKernelVersionStringL()
	
	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self['config'] = ConfigList(self.list)
		self["title"] = Label(_("Nemesis Blue Panel"))
		self["ecmtext"] = ScrollLabel("")
		self['conn'] = Label("")
		self['conn'].hide()
		self["key_blue"] = Label(_("Addons Manager"))
		self["key_yellow"] = Label(_("System Settings"))
		self["key_green"] = Label(_("System Information"))
		self["key_red"] = Label(_("System Utility"))
		self["info_use"] = Label(_("Use arrows < > to select"))
		self["actions"] = NumberActionMap(["ColorActions", "CiSelectionActions","WizardActions","SetupActions"],
			{
				"left": self.keyLeft,
				"right": self.keyRight,
				"blue": self.naddons,
				"green": self.ninfo,
				"yellow": self.nsetting,
				"red": self.nutility,
				"ok": self.ok_pressed,
				"back": self.close
			},-1)
		self.console = Console()
		self.nemPortNumber = t.readPortNumber()
		self.ecmTimer = eTimer()
		self.ecmTimer.timeout.get().append(self.readEcmInfo)
		self.ecmTimer.start(10000, False)
		self.readEcmInfo()
		self.checkVersion()
		self.onLayoutFinish.append(self.loadEmuList)
		self.onShown.append(self.setWindowTitle)
		self.onClose.append(self.__onClose)

	def checkVersion(self):
		fetchFile = "/tmp/ver.txt"
		if fileExists("/etc/.testmode"):
			url = t.readAddonsUrl() + "ver-test.txt"
		else:
			url = t.readAddonsUrl() + "ver.txt"
		print "[BluePanel] downloading version file " + url + " to " + fetchFile
		from twisted.web import client
		from twisted.internet import reactor
		client.downloadPage(url,fetchFile).addCallback(self.fetchFinished,fetchFile).addErrback(self.fetchFailed)
	
	def fetchFinished(self, string, fileName):
		if fileExists(fileName):
			hwVersion = HardwareInfo().get_device_name()
			version = about.getSvnVersionString()
			newVer = version
			f = open(fileName, "r")
			for line in f.readlines():
				line = line.split('=')
				if line[0] == hwVersion:
					newVer = line[1][:-1]
			f.close()
			unlink(fileName)
			if int(version) < int(newVer):
				self['conn'].show()
				self['conn'].setText(_('Update is available!\nCurrent version: %s\nNew Version: %s\nPlease upgrade EDG firmware!') % (version, newVer))

	def fetchFailed(self,string):
		print "[BluePanel] fetch failed " + string.getErrorMessage()

	def setWindowTitle(self):
		self.setTitle(_("EDG-Nemesis Blue Panel"))
	
	def __onClose(self):
		if self.ecmTimer.isActive():
			self.ecmTimer.stop()

	def loadEmuList(self):
		emu = []
		crd = []
		emu.append("None")
		crd.append("None")
		self.emu_list = {}
		self.crd_list = {}
		self.emu_list["None"] = "None"
		self.crd_list["None"] = "None"
		emufilelist = FileList("/usr/script", matchingPattern = "_em.*")
		srvfilelist = FileList("/usr/script", matchingPattern = "_cs.*")
		
		for x in emufilelist.getFileList():
			if x[0][1] != True:
				emuName = t.readEmuName(x[0][0][:-6]) 
				emu.append(emuName)
				self.emu_list[emuName] = x[0][0][:-6]
		softcam = ConfigSelection(default = t.readEmuName(t.readEmuActive()), choices = emu)
		
		for x in srvfilelist.getFileList():
			if x[0][1] != True:
				srvName = t.readSrvName(x[0][0][:-6])
				crd.append(srvName)
				self.crd_list[srvName] = x[0][0][:-6]
		cardserver = ConfigSelection(default = t.readSrvName(t.readSrvActive()), choices = crd)
		
		del self.list[:]
		self.list.append(getConfigListEntry(_('SoftCams (%s) :') % str(len(emu)-1), softcam))
		self.list.append(getConfigListEntry(_('CardServers (%s) :') % str(len(crd)-1), cardserver))
		self.list.append(getConfigListEntry(_("EDG-Nemesis News"), ConfigNothing()))
		self.list.append(getConfigListEntry(_("Parental Control Setup"), ConfigNothing()))
		self.list.append(getConfigListEntry(_("About EDG-Nemesis"), ConfigNothing()))
		self['config'].list = self.list
		self['config'].l.setList(self.list)
	
	def keyLeft(self):
		self["config"].handleKey(KEY_LEFT)

	def keyRight(self):
		self["config"].handleKey(KEY_RIGHT)
	
	def ok_pressed(self):
		self.sel = self["config"].getCurrentIndex()
		if self.sel == 0:
			self.newemu = self.emu_list[self["config"].getCurrent()[1].getText()]
			self.ss_sc()
		elif self.sel == 1:
			self.newsrv = self.crd_list[self["config"].getCurrent()[1].getText()]
			self.ss_srv()
		elif self.sel == 2:
			cmd = {True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value] + "wget " + t.readAddonsUrl() + "info.txt -O /tmp/info.txt"
			self.session.openWithCallback(self.executedScript, nemesisConsole, cmd, _('Downloading EDG-Nemesis info...'))
		elif self.sel == 3:
			self.session.open(ParentalControlSetup)
		elif self.sel == 4:
			self.message = "\nEDG-Nemesis Version: " +  self.NEWDEVER
			self.message += "\n\nImage version: " + self.IMAGEVER
			self.message += "\nBuild by Gianathem"
			self.message += "\nBased on Enigma Version: " +  self.ENIGMAVER
			self.message += "\nKernel version: " + self.KERNELVER
			self.message += "\n\nFor support visit: http://edg-nemesis.tv/"
			self.mbox = self.session.open(MessageBox, self.message, MessageBox.TYPE_INFO)
			self.mbox.setTitle("About EDG-Nemesis " + self.NEWDEVER)
	
	def executedScript(self, *answer):
		self.session.open(nemesisShowPanel, "/tmp/info.txt" ,_('EDG-Nemesis Info'))
		
	def naddons(self):
		self.session.openWithCallback(self.loadEmuList, NAddons)
	
	def nsetting(self):
		self.session.openWithCallback(self.loadEmuList, NSetupSum)
		
	def ninfo(self):
		self.session.openWithCallback(self.loadEmuList, NInfo)
		
	def nutility(self):
		self.session.openWithCallback(self.loadEmuList, NUtility)
	
	def readEcmInfo(self):
		service = self.session.nav.getCurrentService()
		info = service and service.info()
		if info is not None:
			if info.getInfo(iServiceInformation.sIsCrypted):
				self["ecmtext"].setText(t.readEcmInfo())
			else:
				self["ecmtext"].setText("Free To Air")
		else:
			self["ecmtext"].setText("")
	
	def ss_sc(self):
		self.emuactive = t.readEmuActive()
		self.oldref = self.session.nav.getCurrentlyPlayingServiceReference()
		if self.emuactive != "None" and self.newemu != "None":
			if self.emuactive == self.newemu:
				mess = _("Restarting %s.") % t.readEmuName(self.emuactive)
			else:
				mess = _("Stopping %s and starting %s.") % (t.readEmuName(self.emuactive), t.readEmuName(self.newemu))
			cmd = '/var/script/%s_em.sh stop && ' % self.emuactive
			cmd += '/var/script/%s_em.sh start && ' % self.newemu
			cmd += 'echo %s > /var/bin/emuactive' % self.newemu
			self.executeCommand(mess, cmd, True, True)
			return
		if self.emuactive != "None":
			mess = _("Stopping %s.") % t.readEmuName(self.emuactive)
			cmd = '/var/script/%s_em.sh stop && ' % self.emuactive
			cmd += 'rm -f /var/bin/emuactive'
			self.executeCommand(mess, cmd)
			return
		if self.newemu != "None":
			mess = _("Starting %s.") % t.readEmuName(self.newemu)
			cmd = '/var/script/%s_em.sh start && ' % self.newemu
			cmd += 'echo %s > /var/bin/emuactive' % self.newemu
			self.executeCommand(mess, cmd, True, True)
	
	def ss_srv(self):
		self.serveractive = t.readSrvActive()
		if self.serveractive == "None" and self.newsrv == "None":
			return
		self.emuactive = t.readEmuActive()
		if self.emuactive != "None" and self.serveractive == "None":
			self.box = self.session.open( MessageBox, _("Please stop %s\nbefore start cardserver!") % t.readEmuName(self.emuactive) , MessageBox.TYPE_INFO)
			self.box.setTitle(_("Start Cardserver"))
			return
		if self.serveractive != "None" and self.newsrv != "None":
			if self.serveractive == self.newsrv:
				mess = _("Restarting %s.") % t.readSrvName(self.serveractive)
			else:
				mess = _("Stopping %s and starting %s.") % (t.readSrvName(self.serveractive), t.readSrvName(self.newsrv))
			cmd = '/var/script/%s_cs.sh stop && ' % self.serveractive
			cmd += '/var/script/%s_cs.sh start && ' % self.newsrv
			cmd += 'echo %s > /var/bin/csactive' % self.newsrv
			self.executeCommand(mess, cmd)
			return
		if self.serveractive != "None":
			mess = _("Stopping %s.") % t.readSrvName(self.serveractive)
			cmd = '/var/script/%s_cs.sh stop && ' % self.serveractive
			cmd += 'rm -f /var/bin/csactive'
			self.executeCommand(mess, cmd)
			return
		if self.newsrv != "None":
			mess = _("Starting  %s.") % t.readSrvName(self.newsrv)
			cmd = '/var/script/%s_cs.sh start && ' % self.newsrv
			cmd += 'echo %s > /var/bin/csactive' % self.newsrv
			self.executeCommand(mess, cmd)
			
	def executeCommand(self, mess, cmd, panelClose=False, playServ=False):
		self.playServ = playServ
		self.panelClose = panelClose
		self.hide()
		self.mbox = self.session.open(MessageBox, mess, MessageBox.TYPE_INFO, enable_input = False)
		self.mbox.setTitle(_("Running.."))
		if self.playServ:
			self.session.nav.stopService()
		self.console.ePopen("nemesisc '%s' '%s' '%s'" % ('127.0.0.1',self.nemPortNumber,cmd), self.commandFinished)
		
	def commandFinished(self, result, retval, extra_args):
		if self.playServ:
			self.session.nav.playService(self.oldref)
		self.mbox.close()
		if self.panelClose:
			self.close()
		else:
			self.show()

class nemesisBluePanelOpen():
	__module__ = __name__
	
	def __init__(self):
		self['nemesisBluePanelOpen'] = ActionMap(['InfobarExtensions'], {'nemesisBpanel': self.NemesisBpOpen})
	
	def NemesisBpOpen(self):
		self.session.open(nemesisBluePanel)

class nemesisPluginsPanelOpen():
	__module__ = __name__
	
	def __init__(self):
		self['nemesisPluginsPanelOpen'] = ActionMap(['InfobarTimeshiftActions','InfobarAudioSelectionActions'], {'nemesisOPlugins': self.NemesisPluginsOpen})
	
	def NemesisPluginsOpen(self):
		self.session.open(PluginBrowser)
		