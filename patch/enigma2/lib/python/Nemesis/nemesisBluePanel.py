from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from Screens.ParentalControlSetup import ParentalControlSetup
from Screens.PluginBrowser import PluginBrowser
from Components.ActionMap import ActionMap, NumberActionMap
from Components.Label import Label
from Components.ScrollLabel import ScrollLabel
from Components.ConfigList import ConfigList
from Components.config import config, ConfigSelection, getConfigListEntry, ConfigNothing, KEY_LEFT, KEY_RIGHT, KEY_OK
from Components.Sources.StaticText import StaticText
from Components.Sources.Progress import Progress
from Tools.Directories import fileExists, resolveFilename, SCOPE_PLUGINS
from Tools.HardwareInfo import HardwareInfo
from nemesisTool import getVarSpaceKb, nemesisTool
from nemesisShowPanel import nemesisShowPanel
from nemesisConsole import nemesisConsole
from nemesisInfo import NInfo
from nemesisUtility import NUtility
from nemesisSetting import NSetupSum
from nemesisAddons import NAddons
from Components.Console import Console
from Components.About import about
import os, thread
from enigma import eTimer, eDVBCI_UI, iServiceInformation, eConsoleAppContainer

t = nemesisTool()

class nemesisBluePanel(Screen):

	NEMESISVER = "2.6"
	OEVER = "1.6"
	IMAGEVER = about.getImageVersionString()
	ENIGMAVER = about.getEnigmaVersionString()
	KERNELVER = about.getKernelVersionStringL()
	SVNVERSION = about.getSvnVersionString()
	
	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self['config'] = ConfigList(self.list)
		self["ecmtext"] = ScrollLabel("")
		self['conninfo'] = StaticText('')
		self['conn'] = Label("")
		self['conn'].hide()
		self["key_blue"] = Label(_("Addons"))
		self["key_yellow"] = Label(_("Settings"))
		self["key_green"] = Label(_("Information"))
		self["key_red"] = Label(_("Utility"))
		#self["info_use"] = Label(_("Use arrows < > to select"))
		self['spaceused'] = Progress()
		self['spaceusedtext'] = StaticText()
		
		self["actions"] = NumberActionMap(["ColorActions", "CiSelectionActions","WizardActions","SetupActions"],
			{
				"left": self.keyLeft,
				"right": self.keyRight,
				"blue": self.naddons,
				"green": self.ninfo,
				"yellow": self.nsetting,
				"red": self.nutility,
				"ok": self.ok_pressed,
				"back": self.__onClose
			},-1)
	
		self.console = Console()
		self.nemPortNumber = t.readPortNumber()
		self.checkVersionTimer = eTimer()
		self.checkVersionTimer.timeout.get().append(self.checkVersion)
		self.ecmTimer = eTimer()
		self.ecmTimer.timeout.get().append(self.readEcmInfo)
		self.ecmTimer.start(10000, False)
		self.onLayoutFinish.append(self.checkdeveloperMode)
		self.onShown.append(self.setWindowTitle)
		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)
		self.checkVersionContainer = eConsoleAppContainer()
		self.checkVersionContainer.appClosed.append(self.fetchFinished)
		self.linkAddons = t.readAddonsUrl()[0]
		thread.start_new_thread(self.readEcmInfo, ())

	def checkdeveloperMode(self):
		self.loadMainList()
		if config.nemesis.skindevelopermode.value:
			self['conn'].show()
			self['conn'].setText(_('!!!! WARNING !!!!\n\nYou are using image in developer mode\nPlease disable it for best performance\nPress: Yellow->Setup Skin to disable it.'))
		else:
			self['conn'].hide()
			self['conn'].setText('')
			if config.nemesis.ipkg.upgrade.value:
				self.checkVersionTimer.start(100, True)

	def checkVersion(self):
		url = self.linkAddons + {True:'rel-test.txt',False:'rel.txt'}[fileExists("/etc/.testmode")]
		cmd = {True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value] + "wget " + url + " -O /tmp/ver.txt"
		self.checkVersionContainer.execute(cmd)

	def fetchFinished(self, retval):
		if fileExists('/tmp/ver.txt') and retval == 0:
			hwVersion = HardwareInfo().get_device_name()
			newVer = self.SVNVERSION
			minVer = "0000"
			NAddons.CANUPGRADE = False
			try:
				f = open('/tmp/ver.txt','r')
				for line in f.readlines():
					line = line.split('=')
					if line[0] == "FreeSpaceNeed":
						NAddons.FREESPACENEEDUPGRADE = int(line[1][:-1])
					if line[0] == hwVersion:
						newVer = line[1]
						minVer = line[2][:-1]
				f.close()
				os.unlink('/tmp/ver.txt')
				if int(self.SVNVERSION) < int(newVer):
					self['conn'].show()
					if int(self.SVNVERSION) < int(minVer):
						self['conn'].setText(_('New version SVN(%sr%s) is available!\nCurrent version: %sr%s\nMin Version required: %sr%s\nUpgrade not possible!') % (newVer[0:3],newVer[3], self.SVNVERSION[0:3],self.SVNVERSION[3], minVer[0:3],minVer[3]))
					else:
						NAddons.CANUPGRADE = True
						self['conn'].setText(_('New version is available!\nCurrent version: %sr%s\nNew Version: %sr%s\nPlease upgrade Nemesis firmware!') % (self.SVNVERSION[0:3],self.SVNVERSION[3], newVer[0:3],newVer[3]))
			except:
				pass

	def setWindowTitle(self):
		self.setTitle("%s - %s: %s SVN(%sr%s)" % (_("Nemesis Blue Panel"), _("Image Version"), self.NEMESISVER, self.SVNVERSION[0:3],self.SVNVERSION[3]))
		diskSpace = getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self["spaceusedtext"].text = _("Free space: %d kB (%d%%)") % (int(diskSpace[0]), percFree)
		self["spaceused"].setValue(percUsed)

	def __onClose(self):
		if self.container.running():
			self.container.kill()
			self['conninfo'].text = (_('Process Killed by user, Server Not Connected!'))
		else:
			if self.checkVersionContainer.running():
				self.checkVersionContainer.kill()
			del self.container.appClosed[:]
			del self.checkVersionContainer.appClosed[:]
			del self.container
			del self.checkVersionContainer
			if self.ecmTimer.isActive():
				self.ecmTimer.stop()
			if self.checkVersionTimer.isActive():
				self.checkVersionTimer.stop()
			if fileExists('/tmp/ver.txt'):
				os.unlink('/tmp/ver.txt')
			if fileExists('/tmp/info.txt'):
				os.unlink('/tmp/info.txt')
			from nemesisTool import initNemesisTool
			thread.start_new_thread(initNemesisTool, ())
			self.close()

	def loadMainList(self):
		emu = []
		crd = []
		emu.append("None")
		crd.append("None")
		self.emu_list = {}
		self.crd_list = {}
		self.emu_list["None"] = "None"
		self.crd_list["None"] = "None"

		for x in os.listdir("/usr/script/"):
			if x.find('_em.sh') >= 0:
				emuName = t.readEmuName(x[:-6]) 
				emu.append(emuName)
				self.emu_list[emuName] = x[:-6]
				continue
			if x.find('_cs.sh') >= 0:
				srvName = t.readSrvName(x[:-6]) 
				crd.append(srvName)
				self.crd_list[srvName] = x[:-6]
				continue

		softcam = ConfigSelection(default = t.readEmuName(t.readEmuActive()), choices = emu)
		cardserver = ConfigSelection(default = t.readSrvName(t.readSrvActive()), choices = crd)
		
		del self.list[:]
		self.list.append(getConfigListEntry(_('SoftCams (%s) :') % str(len(emu)-1), softcam))
		self.list.append(getConfigListEntry(_('CardServers (%s) :') % str(len(crd)-1), cardserver))
		self.list.append(getConfigListEntry(_("Nemesis News"), ConfigNothing()))
		self.list.append(getConfigListEntry(_("Nemesis Timeline"), ConfigNothing()))
		self.list.append(getConfigListEntry(_("Parental Control Setup"), ConfigNothing()))
		self.list.append(getConfigListEntry(_("About Nemesis"), ConfigNothing()))
		self['config'].list = self.list
		self['config'].l.setList(self.list)
	
	def keyLeft(self):
		self["config"].handleKey(KEY_LEFT)

	def keyRight(self):
		self["config"].handleKey(KEY_RIGHT)
	
	def ok_pressed(self):
		self['conninfo'].text = ('')
		if self.container.running():
			self.container.kill()
		self.sel = self["config"].getCurrentIndex()
		if self.sel == 0:
			self.newemu = self.emu_list[self["config"].getCurrent()[1].getText()]
			self.ss_sc()
		elif self.sel == 1:
			self.newsrv = self.crd_list[self["config"].getCurrent()[1].getText()]
			self.ss_srv()
		elif self.sel == 2:
			if not self.container.running():
				self['conninfo'].text = (_("Connetting to server. Please wait..."))
				cmd = "%swget %sinfo.txt -O /tmp/info.txt" % ({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value], self.linkAddons)
				self.container.execute(cmd)
		elif self.sel == 3:
			if not self.container.running():
				self['conninfo'].text = (_("Connetting to server. Please wait..."))
				cmd = "%swget %stimeline.txt -O /tmp/info.txt" % ({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value], self.linkAddons)
				self.container.execute(cmd)
		elif self.sel == 4:
			self.session.open(ParentalControlSetup)
		elif self.sel == 5:
			message = "\nNemesis Version: %s OE(%s)" % (self.NEMESISVER, self.OEVER)
			message += "\n\nImage version: " + self.IMAGEVER
			message += "\nBuild by Gianathem"
			message += "\nBased on Enigma Version: " +  self.ENIGMAVER
			message += "\nKernel version: " + self.KERNELVER
			message += "\n\nFor download visit: http://www.nemesis.tv/"
			self.session.open(nemesisShowPanel, None ,"About Nemesis ", message)
	
	def runFinished(self, retval):
		if fileExists('/tmp/info.txt') and retval == 0 :
			self['conninfo'].text = ('')
			self.session.open(nemesisShowPanel, "/tmp/info.txt" ,{2:_('Nemesis News'),3:_('Nemesis Timeline')}[self.sel])
			os.unlink('/tmp/info.txt')
		else:
			self['conninfo'].text = (_("Server not found! Please check internet connection."))

	def naddons(self):
		self.session.openWithCallback(self.loadMainList, NAddons)
	
	def nsetting(self):
		self.session.openWithCallback(self.checkdeveloperMode, NSetupSum)
		
	def ninfo(self):
		self.session.openWithCallback(self.loadMainList, NInfo)
		
	def nutility(self):
		self.session.openWithCallback(self.loadMainList, NUtility)
	
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
		try:
			self.console.ePopen("nemesisc '%s' '%s' '%s'" % ('127.0.0.1',self.nemPortNumber,cmd), self.commandFinished)
		except:
			pass
		
	def commandFinished(self, result, retval, extra_args):
		if self.playServ:
			self.session.nav.playService(self.oldref)
		self.mbox.close()
		if self.panelClose:
			self.__onClose()
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
		