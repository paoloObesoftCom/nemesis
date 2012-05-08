from Screens.ChoiceBox import ChoiceBox
from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from enigma import eTimer, eDVBDB, eConsoleAppContainer, ePicLoad, eEnv
from Components.ActionMap import ActionMap
from Components.Label import Label
from Components.Sources.List import List
from Components.Sources.StaticText import StaticText
from Components.Sources.Progress import Progress
from Components.ProgressBar import ProgressBar
from Components.ScrollLabel import ScrollLabel
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmapAlphaTest
from Components.config import config
from Components.PluginComponent import plugins
from Components.Pixmap import Pixmap
from Tools.LoadPixmap import LoadPixmap
from Tools.Directories import fileExists, resolveFilename, SCOPE_PLUGINS, pathExists, createDir
from os import system, remove, listdir, chdir, getcwd, rename, path, walk
from nemesisTool import nemesisTool, restartE2, GetPluginInstallPath, getVarSpaceKb, GetSkinPath, GetSkinsPath, createSkinUninstall, GetEpgPath
from nemesisConsole import nemesisConsole
from nemesisDownloader import nemesisDownloader
from Tools import Notifications
import xml.etree.cElementTree as x

def createLink(basedir, create = False):
	ret = False
	for root, dirs, files in walk(basedir):
		for file in files:
			if file == "skin.xml" and basedir.find('/skins') != -1:
				ret = True
				if create and (root != eEnv.resolve('${datadir}/enigma2')):
					system("ln -s %s %s" % (root, eEnv.resolve('${datadir}/enigma2/')))
	return ret

t = nemesisTool()

class util:
	
	pluginIndex = -1
	pluginType = ''
	typeDownload = 'A'
	addonsName = ''
	filename = ''
	dir = ''
	size = 0
	check = 0
	
	def reloadSetting(self):
		print "Reload settings"
		self.eDVBDB = eDVBDB.getInstance()
		self.eDVBDB.reloadServicelist()
		self.eDVBDB.reloadBouquets()

	def removeSetting(self):
		print "Remove settings"
		#system("rm -f /etc/tuxbox/satellites.xml")
		system("rm -f /etc/enigma2/*.radio")
		system("rm -f /etc/enigma2/*.tv")
		system("rm -f /etc/enigma2/lamedb")
		system("rm -f /etc/enigma2/blacklist")
		system("rm -f /etc/enigma2/whitelist")
	
u = util()

class loadXml:
	
	tree_list = []
	plugin_list = []
	
	def load(self,filename):
		del self.tree_list[:]
		del self.plugin_list[:]
		tree = x.parse(filename)
		root = tree.getroot()
		c = 0
		for tag in root.getchildren(): 
			self.tree_list.append([c, tag.tag])
			c1 = 0
			for b in tree.find(tag.tag):
				self.plugin_list.append([c,tag.tag,b.find("Filename").text,b.find("Descr").text,b.find("Folder").text,b.find("Size").text,b.find("Check").text,c1])
				c1 +=1
			c +=1

loadxml = loadXml()

class loadTmpDir:
	
	tmp_list = []
	
	def load(self):
		del self.tmp_list[:]
		pkgs = listdir('/tmp')
		count = 0
		for fil in pkgs:
			if (fil.find('.ipk') != -1 or fil.find('.tbz2') != -1):
				self.tmp_list.append([count,fil])
				count +=1

loadtmpdir = loadTmpDir()

class loadUniDir:
	
	uni_list = []
	
	def load(self):
		del self.uni_list[:]
		pkgs = listdir('/usr/uninstall')
		self.uni_list.append([0,_("Remove picons")])
		self.uni_list.append([1,_("Remove EPG data files")])
		count = 2
		for fil in pkgs:
			if (fil.find('_remove.sh') != -1 or fil.find('_delete.sh') != -1):
				self.uni_list.append([count,fil])
				count +=1

loadunidir = loadUniDir()
		
xFolder = "iuregdoiuwqcdiuewq"

class NAddons(Screen):
	__module__ = __name__

	FREESPACENEEDUPGRADE = 4000
	CANUPGRADE = False

	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self["title"] = Label(_("Addons Manager"))
		self['list'] = List(self.list)
		self['conn'] = StaticText("")
		self['spaceused'] = ProgressBar()
		self["key_red"] = StaticText(_("Exit"))
		self["key_green"] = Label(_('OK'))
		self.installFile = False
		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)
		self.containerExtra = eConsoleAppContainer()
		self.containerExtra.appClosed.append(self.runFinishedExtra)
		self.reloadTimer = eTimer()
		self.reloadTimer.timeout.get().append(self.relSetting)
			
		self.linkAddons = t.readAddonsUrl()
		self.fileAddons = self.linkAddons[1]
		self.linkAddons = self.linkAddons[0]
		self.linkExtra = t.readExtraUrl()

		isPluginManager = False
		if fileExists(resolveFilename(SCOPE_PLUGINS, "SystemPlugins/SoftwareManager/plugin.pyo")):
			isPluginManager = True

		self.MenuList = [
			('NAddons',_('Download Addons'),'icons/get.png',True),
			('NExtra',_('Download Extra'),'icons/get.png',fileExists('/etc/extra.url')),
			('NManual',_('Manual Package Install'),'icons/manual.png',True),
			('NRemove',_('Remove Addons'),'icons/remove.png',True),
			('NReload',_('Reload Settings'),'icons/enigma.png',True),
			('NExtension',_('Manage extensions'),'icons/extensions.png',isPluginManager),
			('NPacket',_('Packet management'),'icons/package.png',isPluginManager),
			('NUpdate',_('Software update'),'icons/update.png',isPluginManager)
			]
		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			'green': self.KeyOk,
			"red": self.cancel,
			'back': self.cancel
		}, -1)
		self.onLayoutFinish.append(self.updateList)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		diskSpace = getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self.setTitle("%s - Free: %d kB (%d%%)" % ( _("Addons Manager"), int(diskSpace[0]), percFree))
		self["spaceused"].setValue(percUsed)

	def KeyOk(self):
		if self.installFile:
			self.downloading()
			self.runUpgrade(True)
			return
		if self.container.running() or self.containerExtra.running():
			return
		self['conn'].text = ('')
		sel = self["list"].getCurrent()[0]
		if (sel == "NAddons"):
			self["key_red"].text = _("Cancel")
			self['conn'].text = (_("Connetting to addons server.\nPlease wait..."))
			u.typeDownload = 'A'
			cmd = "%swget %s%s -O /tmp/addons.xml" % ({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value],self.linkAddons, self.fileAddons)
			self.container.execute(cmd)
		elif (sel == "NExtra"):
			self["key_red"].text = _("Cancel")
			self['conn'].text = (_("Connetting to addons server.\nPlease wait..."))
			u.typeDownload = 'E'
			if self.linkExtra != None:
				cmd = "%swget %s%s/tmp.tmp -O /tmp/tmp.tmp" % ({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value],self.linkExtra, xFolder)
				self.containerExtra.execute(cmd)
			else:
				self['conn'].text = (_("Server not found!\nPlease check internet connection."))
		elif (sel == "NManual"):
			self.session.open(RManual)
		elif (sel == "NRemove"):
			self.session.open(RRemove)
		elif (sel == "NExtension"):
			try:
				from Plugins.SystemPlugins.SoftwareManager.plugin import PluginManager
			except ImportError:
				self.session.open(MessageBox, _("The Softwaremanagement extension is not installed!\nPlease install it."), type = MessageBox.TYPE_INFO,timeout = 10 )
			else:
				self.session.open(PluginManager, GetSkinPath())
		elif (sel == "NPacket"):
			try:
				from Plugins.SystemPlugins.SoftwareManager.plugin import PacketManager
			except ImportError:
				self.session.open(MessageBox, _("The Softwaremanagement extension is not installed!\nPlease install it."), type = MessageBox.TYPE_INFO,timeout = 10 )
			else:
				self.session.open(PacketManager, GetSkinPath())
		elif (sel == "NUpdate"):
			if (not self.CANUPGRADE) and (not fileExists("/etc/.testmode")):
				self['conn'].text = _('No Upgrade available!\nYour decoder is up to date.')
				return
			if int(getVarSpaceKb()[0]) < self.FREESPACENEEDUPGRADE:
				self['conn'].text = _('Not enough free space on flash to perform Upgrade!\nUpgrade require at least %d kB free on Flash.\nPlease remove some addons or skins before upgrade.') % self.FREESPACENEEDUPGRADE
				return
			self.downloading("Upgrading")
		elif (sel == "NReload"):
			self['conn'].text = _("Reload settings, please wait...")
			self.reloadTimer.start(250, True)

	def downloading(self, state=""):
		if state == "Upgrading":	
			self['conn'].text = _("Do you want to update your Dreambox?") + _("\nAfter pressing Yes, please wait!")
			self["key_red"].text = _("No")
			self["key_green"].setText(_("Yes"))
			self.installFile = True
		else:
			self['conn'].text = ''
			self["key_red"].text = _("Exit")
			self["key_green"].setText(_('OK'))
			self.installFile = False

	def relSetting(self):
		u.reloadSetting()
		self['conn'].text = _("Settings reloaded succesfully!")

	def runUpgrade(self, result):
		if result:
			try:
				from Plugins.SystemPlugins.SoftwareManager.plugin import UpdatePlugin
			except ImportError:
				self['conn'].text = _("The Softwaremanagement extension is not installed!\nPlease install it.")
			else:
				self.session.open(UpdatePlugin, GetSkinPath())

	def runFinishedExtra(self, retval):
		if fileExists('/tmp/tmp.tmp'):
			try:
				f = open("/tmp/tmp.tmp", "r")
				line = f.readline() [:-1]
				f.close()
				cmd = "%swget %s%s/%s -O /tmp/addons.xml" % ({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value], self.linkExtra, xFolder, line)
				self.container.execute(cmd)
			except:
				self['conn'].text = (_("Server not found!\nPlease check internet connection."))
				self["key_red"].text = _("Exit")
			remove('/tmp/tmp.tmp')
		else:
			self['conn'].text = (_("Server not found!\nPlease check internet connection."))
			self["key_red"].text = _("Exit")

	def runFinished(self, retval):
		if fileExists('/tmp/addons.xml'):
			try:
				loadxml.load('/tmp/addons.xml')
				remove('/tmp/addons.xml')
				self['conn'].text = ('')
				self.session.open(RAddons)
			except:
				self['conn'].text = (_("File xml is not correctly formatted!."))
		else:
			self['conn'].text = (_("Server not found!\nPlease check internet connection."))
		self["key_red"].text = _("Exit")
	
	def cancel(self):
		self["key_red"].text = _("Exit")
		if self.installFile:
			self.downloading()
			return
		if not self.container.running() and not self.containerExtra.running():
			del self.container.appClosed[:]
			del self.container
			del self.containerExtra.appClosed[:]
			del self.containerExtra
			self.close()
		else:
			if self.container.running():
				self.container.kill()
			if self.containerExtra.running():
				self.containerExtra.kill()
			if fileExists('/tmp/addons.xml'):
				remove('/tmp/addons.xml')
			if fileExists('/tmp/tmp.tmp'):
				remove('/tmp/tmp.tmp')
			self['conn'].text = (_('Process Killed by user\nServer Not Connected!'))
	
	def updateList(self):
		del self.list[:]
		skin_path = GetSkinPath()
		for i in self.MenuList:
			if i[3]:
				self.list.append((i[0], i[1], LoadPixmap(skin_path + i[2])))
		self['list'].setList(self.list)

class	RAddons(Screen):
	__module__ = __name__

	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self.wtitle = {'A': _('Download Addons'),'E': _('Download Extra')}[u.typeDownload]
		self['list'] = List(self.list)
		self["title"] = Label(self.wtitle)
		self["key_red"] = Label(_("Exit"))
		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			"red": self.close,
			'back': self.close
		})
		self.onLayoutFinish.append(self.loadData)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(self.wtitle)

	def KeyOk(self):
		u.pluginType = self["list"].getCurrent()[0]
		u.pluginIndex = self['list'].getIndex() 
		self.session.open(RAddonsDown)
	
	def loadData(self):
		del self.list[:]
		for tag in loadxml.tree_list: 
			self.list.append((tag [1], tag [1]))
		self['list'].setList(self.list)

class	RAddonsDown(Screen):
	__module__ = __name__

	EVENT_DONE = 10
	EVENT_KILLED = 5

	def __init__(self, session):
		Screen.__init__(self, session)
		self.IS_SKIN_SECTION = False
		self.HAS_PLUG_PREVIEW = False
		self.url = ''
		self.filename = ''
		self.dstfilename = ''
		self.download = None
		self.installFile = False
		self.installingFile = False
		self.list = []
		self['list'] = List(self.list)
		self['conn'] = StaticText(_("Loading elements.\nPlease wait..."))
		self["key_red"] = StaticText(_("Exit"))
		self["key_green"] = Label(_("Download"))
		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)
		self['type'] = StaticText('')
		self["progressbar"] = Progress()
		self["progressbar"].range = 1000
		self['spaceused'] = Progress()
		self['spaceusedtext'] = StaticText()
		self.pathToInstall = ''
		self.movingTimer = eTimer()
		self.movingTimer.callback.append(self.doMovingAddon)
		self.activity = 0
		self.activityTimer = eTimer()
		self.activityTimer.callback.append(self.doActivityTimer)
		self._restartE2 = restartE2(session)

		self.linkAddons = t.readAddonsUrl()[0]
		self.linkExtra = t.readExtraUrl()

		if u.pluginType.find('Skin') >= 0 \
				or u.pluginType.find('Bootlogo') >= 0 \
				or u.pluginType.find('Picon') >= 0:
			self.skinName = "RAddonsDownSkin"
			self.IS_SKIN_SECTION = True
			self["preview"] = Pixmap()
			self["preview"].hide()
			self.checkPreviewContainer = eConsoleAppContainer()
			self.checkPreviewContainer.appClosed.append(self.fetchFinished)
			self["list"].onSelectionChanged.append(self.checkPreview)
			self['actions'] = ActionMap(['WizardActions','ColorActions'],
			{
				'ok': self.openPreview,
				"green": self.KeyOk,
				"red": self.cancel,
				'back': self.cancel
			})
		else:
			self['actions'] = ActionMap(['WizardActions','ColorActions'],
			{
				'ok': self.KeyOk,
				"green": self.KeyOk,
				"red": self.cancel,
				'back': self.cancel
			})

		self.onLayoutFinish.append(self.__onLayoutFinish)
		self.onShown.append(self.setWindowTitle)

	def __onLayoutFinish(self):
		self.loadPlugin()
		if self.IS_SKIN_SECTION:
			self.checkPreview()

	def setWindowTitle(self):
		self.setTitle(_("Download ") + str(u.pluginType))
		diskSpace = getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self["spaceusedtext"].text = _("Free space: %d kB (%d%%)") % (int(diskSpace[0]), percFree)
		self["spaceused"].setValue(percUsed)

	def doActivityTimer(self):
		self.activity += 50
		if self.activity == 1000:
			self.activity = 0
		self["progressbar"].value = self.activity

	def loadPlugin(self):
		self["progressbar"].value = 0
		del self.list[:]
		for tag in loadxml.plugin_list: 
			if tag [0] == u.pluginIndex:
				self.list.append((tag [2], tag [3], tag [4], tag [5] , tag [6]))
		self['list'].setList(self.list)
		if not self.IS_SKIN_SECTION:
			self['conn'].text = _('Elements Loaded!.\nPlease select one to install.')

	def checkPreview(self):
		self.HAS_PLUG_PREVIEW = False
		if self['list'].count() > 0:
			if self.stopLoadPreview():
				self.activityTimer.start(100, False)
				self["preview"].hide()
				self['conn'].text = _('Loading Preview....\nPlease Wait..')
				self.getAddonsPar()
				system("rm -f /tmp/skin_preview.png")
				url = "%s%s/%s.png" % (self.linkAddons, u.dir, u.filename)
				cmd = "%swget %s -O /tmp/skin_preview.png" % ({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value],url)
				self.checkPreviewContainer.execute(cmd)
			
	def stopLoadPreview(self):
		if self.IS_SKIN_SECTION:
			if self.checkPreviewContainer.running():
				self.checkPreviewContainer.kill()
				self.HAS_PLUG_PREVIEW = False
			if self.activityTimer.isActive():
				self.activityTimer.stop()
			self.activity = 0
		return True
		
	def fetchFinished(self, retval):
		if self.activityTimer.isActive():
			self.activityTimer.stop()
			self.activity = 0
		if path.exists("/tmp/skin_preview.png") and self.IS_SKIN_SECTION:
			self.picload = ePicLoad()
			self.picload.PictureData.get().append(self.previewShow)
			self.picload.setPara((self["preview"].instance.size().width(), self["preview"].instance.size().height(), 1, 1, False, 1, "#00000000"))
			self.picload.startDecode("/tmp/skin_preview.png")	
		else:
			self["progressbar"].value = 0
			self['conn'].text = _('Elements Loaded!.\nPlease select one to install.')

	def previewShow(self, picInfo=None):
		ptr = self.picload.getData()
		if ptr != None:
			self["preview"].instance.setPixmap(ptr.__deref__())
			self['conn'].text = _('The selected Plugin has a preview.\nPress OK button to show it on full screen.')
			self.HAS_PLUG_PREVIEW = True
			self["preview"].show()
		else:
			self['conn'].text = _('Elements Loaded!.\nPlease select one to install.')
		self["progressbar"].value = 0
		del self.picload

	def openPreview(self):
		if self.HAS_PLUG_PREVIEW:
			self.session.open(openPreviewScreen, "/tmp/skin_preview.png")

	def KeyOk(self):
		if self['list'].count() == 0:
			self.__Close()
		if self.installingFile:
			return
		if self.installFile:
			self.installAddons(True)
			return
		if self.download == None:
			if self.stopLoadPreview():
				self.downloadAddons()
			return

	def downloadAddons(self):
		self.getAddonsPar()
		if int(u.size) > int(getVarSpaceKb()[0]) and int(u.check) != 0:
			self["conn"].text = _('Not enough space!\nPlease delete addons before install new.')
			return
		self["key_red"].text = _("Cancel")
		url = "%s%s/%s" % ({'E':self.linkExtra,'A':self.linkAddons}[u.typeDownload], u.dir, u.filename)
		if not pathExists("/tmp/.-"):
			createDir("/tmp/.-")
		if config.proxy.isactive.value:
			cmd = "/var/etc/proxy.sh && wget %s -O /tmp/.-/.-" % url
			self.session.openWithCallback(self.executedScript, nemesisConsole, cmd, _('Download: ') + u.filename)
		else:
			self.fileDownload(url, "/tmp/.-/", u.filename)

	def fileDownload(self, url, folder, filename):
		from Tools.Downloader import downloadWithProgress
		self.url = url
		self.filename = filename
		self.dstfilename = "%s.-" % folder
		print "[download] downloading file..."
		self.download = downloadWithProgress(self.url, self.dstfilename)
		self.download.addProgress(self.progress)
		self.download.start().addCallback(self.finished).addErrback(self.failed)
		self["conn"].text = _("Downloading file:\n%s ...") % (self.filename)

	def progress(self, recvbytes, totalbytes):
		if self.download:
			self["progressbar"].value = int(1000*recvbytes/float(totalbytes))
			self["type"].text = "Downloading: %d of %d kBytes (%.2f%%)" % (recvbytes/1024, totalbytes/1024, 100*recvbytes/float(totalbytes))

	def failed(self, failure_instance=None, error_message=""):
		if error_message == "" and failure_instance is not None:
			error_message = failure_instance.getErrorMessage()
		print "[Download_failed] " + error_message
		self["conn"].text = _("Download file %s\nfailed!") % (self.filename)
		self["type"].text = error_message
		self.downloading()

	def finished(self, string = ""):
		self["key_red"].text = _("Exit")
		if self.download:
			print "[Download_finished] " + str(string)
			self["type"].text = _("Download file %s\nfinished!") % (self.filename)
			self.executedScript(self.EVENT_DONE)

	def removeFile(self):
		if fileExists('/tmp/.-/.-'):
			remove("/tmp/.-/.-")
			return
		if fileExists('/tmp/.-/p.ipk'):
			remove("/tmp/.-/p.ipk")

	def cancel(self):
		if self.installFile:
			self.downloading()
			return
		if self.download:
			self.download.stop()
			self.downloading()
			self.executedScript(self.EVENT_KILLED)
			return
		if self.container.running():
			self.container.kill()
			self.removeFile()
			self.downloading()
			self['conn'].text = _('Process Killed by user.\nAddon not installed correctly!')
			return
		self.__Close()

	def __Close(self):
		if self.download:
			self.download.stop()
		if self.container.running():
			self.container.kill()
			del self.container.appClosed[:]
			del self.container
		if self.IS_SKIN_SECTION and self.checkPreviewContainer.running():
			self.checkPreviewContainer.kill()
			del self.checkPreviewContainer.appClosed[:]
			del self.checkPreviewContainer
		if self.activityTimer.isActive():
			self.activityTimer.stop()
		if self.movingTimer.isActive():
			self.movingTimer.stop()
		self.close()
		
	def executedScript(self, *answer):
		if answer[0] == self.EVENT_DONE:
			if fileExists('/tmp/.-/.-'):
				self.downloading("Install")
			else:
				self['conn'].text = _('File: %s not found!\nPlease check your internet connection.') % u.filename
		elif answer[0] == self.EVENT_KILLED:
			self['conn'].text = _('Process Killed by user!\nAddon not downloaded.')
	
	def downloading(self, state=""):
		if state == "Install":	
			self.installFile = True
			self['conn'].text = _('Do you want install the addon: %s?') % u.addonsName
			self["key_red"].text = _("No")
			self["key_green"].setText(_("Yes"))
		elif state == "Installing":
			self.installFile = False
			self.installingFile = True
			self["key_red"].text = _("Cancel")
			self["key_green"].setText("")
			self["progressbar"].value = 0
			self['conn'].text = ''
		else:
			self.installingFile = False
			self.installFile = False
			if self.activityTimer.isActive():
				self.activityTimer.stop()
			self["progressbar"].value = 0
			self.activity = 0
			self["key_red"].text = _("Exit")
			self["key_green"].setText(_("Download"))
			self["progressbar"].value = 0
			self['conn'].text = _('Elements Loaded!.\nPlease select one to install.')
			self['type'].text = ''
		if self.download:
			self.download = None

	def installAddons(self, answer):
		if (answer is True):
			self.downloading("Installing")
			self['conn'].text = _('Installing addons.\nPlease Wait...')
			if (u.filename.find('.ipk') != -1):
				self.activityTimer.start(100, False)
				self['type'].text = _('Install')
				args = {True: '--force-overwrite --force-defaults ',False: ''}[config.nemesis.ipkg.overwriteUpgrade.value]
				args = {True: '--force-reinstall --force-defaults ',False: ''}[config.nemesis.ipkg.forceReInstall.value]
				rename("/tmp/.-/.-", "/tmp/.-/p.ipk")
				self.container.execute("ipkg " + args + "install /tmp/.-/p.ipk")
			elif (u.filename.find('.tbz2') != -1):
				self.activityTimer.start(100, False)
				self['type'].text = _('Installing')
				if u.pluginType.find('Settings') >= 0:
					self['conn'].text = _("Remove old Settings\nPlease wait...")
					u.removeSetting()	
				if pathExists("/tmp/install"):
					system("rm -rf /tmp/install")
				self.container.execute("tar -jxvf /tmp/.-/.- -C /")
			else:
				self.downloading()
				self['conn'].text = _('File: %s\nis not a valid package!') % u.filename
	
	def runFinished(self, retval):
		self.removeFile()
		if pathExists("/tmp/install"):
			msg = _("Please select a destination media,\nto install %s package.") % u.filename
			if createLink("/tmp/install/skins"):
				installPath = GetSkinsPath()
			else:
				installPath = GetPluginInstallPath()
			if installPath:
				self.pathToInstall = ''
				self.session.openWithCallback(self.runIstallation, ChoiceBox, title=msg, list = installPath)
			else:
				self.downloading()
				self['conn'].text = _("No device found to installing addon!")
			return
		plugins.readPluginList(resolveFilename(SCOPE_PLUGINS))
		self.downloading()
		if u.pluginType.find('Settings') >= 0:
			self['conn'].text = _("Reload new Settings\nPlease wait...")
			u.reloadSetting()
		self['conn'].text = _("Addon installed succesfully!")
		if fileExists('/tmp/.restartE2'):
			remove("/tmp/.restartE2")
			msg = _("Enigma2 will be now hard restarted to complete package installation.") + "\n" + _("Do You want restart enigma2 now?")
			self._restartE2.go(msg)

	def runIstallation(self, answer):
		if answer:
			answer = answer and answer[1]
			if answer == eEnv.resolve('${datadir}/enigma2') and int(u.size) > int(getVarSpaceKb()[0]):
				self["conn"].text = _('Not enough space!\nPlease delete addons before install new.')
			else:
				self.pathToInstall = answer
				self.movingTimer.start(100, True)
		else:
			if pathExists("/tmp/install"):
				system("rm -rf /tmp/install")
			self.downloading()

	def doMovingAddon(self):
		if self.movingTimer.isActive():
			self.movingTimer.stop()
		self['conn'].text = _("Moving addon on %s, please wait...") % self.pathToInstall
		if createLink("/tmp/install/skins"):
			if not pathExists(self.pathToInstall) and (self.pathToInstall != eEnv.resolve('${datadir}/enigma2')):
				system('mkdir %s' % self.pathToInstall)
			system("cp -r /tmp/install/skins/* %s/" % self.pathToInstall)
			createLink(self.pathToInstall, True)
			createSkinUninstall(u.filename,self.pathToInstall)
		else:
			system("cp -r /tmp/install/* %s/" % self.pathToInstall)
		plugins.readPluginList(resolveFilename(SCOPE_PLUGINS))
		if pathExists("/tmp/install"):
			system("rm -rf /tmp/install")
		self.downloading()
		self['conn'].text = _("Addon installed succesfully!")
			
	def getAddonsPar(self):
		u.filename  = self['list'].getCurrent()[0] 
		u.addonsName  = self['list'].getCurrent()[1] 
		u.dir  = self['list'].getCurrent()[2] 
		u.size  = self['list'].getCurrent()[3] 
		u.check  = self['list'].getCurrent()[4] 

class openPreviewScreen(Screen):

	skin = """
	<screen position="0,0" size="1280,720"  backgroundColor="transparent" flags="wfNoBorder">
		<widget name="Preview" position="0,0" size="1280,720" alphatest="on"/>
	</screen>"""

	def __init__(self, session, pngpath):
		Screen.__init__(self, session)
		
		self["Preview"] = Pixmap()
		self.pngpath = pngpath
		self["actions"] = ActionMap(["WizardActions", "ColorActions"],
			{
			'ok': self.close,
			'back': self.close,
			'red': self.close
			}, -1)

		self.onLayoutFinish.append(self.layoutFinished)
		
	def layoutFinished(self):
		self["Preview"].instance.setPixmapFromFile(self.pngpath)

class	RManual(Screen):
	__module__ = __name__

	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self['list'] = List(self.list)
		self['conn'] = StaticText("")
		self["title"] = Label(_("Manual Installation"))
		self["key_red"] = StaticText(_("Exit"))
		self["key_green"] = Label(_("Install"))
		self["key_yellow"] = Label(_("Reload /tmp"))
		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)

		self['type'] = StaticText()
		self["progressbar"] = Progress()
		self["progressbar"].range = 100
		self['spaceused'] = Progress()
		self['spaceusedtext'] = StaticText()
		self.pathToInstall = ''
		self.movingTimer = eTimer()
		self.movingTimer.callback.append(self.doMovingAddon)
		self.activity = 0
		self.activityTimer = eTimer()
		self.activityTimer.callback.append(self.doActivityTimer)

		self._restartE2 = restartE2(session)
		self.installFile = False

		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			'green': self.KeyOk,
			"yellow": self.readTmp,
			"red": self.cancel,
			'back': self.cancel
		})

		self.onLayoutFinish.append(self.readTmp)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_("Manual Installation"))
		diskSpace = getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self["spaceusedtext"].text = _("Free space: %d kB (%d%%)") % (int(diskSpace[0]), percFree)
		self["spaceused"].setValue(percUsed)

	def doActivityTimer(self):
		self.activity += 5
		if self.activity == 100:
			self.activity = 0
		self["progressbar"].value = self.activity

	def readTmp(self):
		self["progressbar"].value = 0
		if not self.container.running() and not self.installFile:
			del self.list[:]
			loadtmpdir.load()
			if len(loadtmpdir.tmp_list) > 0:
				for fil in loadtmpdir.tmp_list: 
					self.list.append((fil[1], fil[1]))
			else:	
				self['conn'].text = (_("Put your plugin xxx.tbz2 or xxx.ipk\nvia FTP in /tmp."))
			self['list'].setList(self.list)
	
	def KeyOk(self):
		if self['list'].count() == 0:
			self.close()
		if not self.container.running() and not self.installFile:
			u.filename  = self['list'].getCurrent()[0] 
			self.downloading("Install")
			return
		if self.installFile:
			self.installAddons(True)
			return
	
	def installAddons(self, answer):
		if (answer is True):
			self['conn'].text = (_('Installing: %s.\nPlease wait...') % u.filename)
			if (u.filename.find('.ipk') != -1):
				self.downloading("Installing")
				self.activityTimer.start(100, False)
				args = {True: '--force-overwrite --force-defaults ',False: ''}[config.nemesis.ipkg.overwriteUpgrade.value]
				args = {True: '--force-reinstall --force-defaults ',False: ''}[config.nemesis.ipkg.forceReInstall.value]
				self.container.execute("ipkg " + args + "install /tmp/" + u.filename)
			elif (u.filename.find('.tbz2') != -1):
				self.downloading("Installing")
				self.activityTimer.start(100, False)
				if pathExists("/tmp/install"):
					system("rm -rf /tmp/install")
				self.container.execute("tar -jxvf /tmp/" + u.filename + " -C /")
			else:
				self.downloading()
				self['conn'].text = (_('File: %s\nis not a valid package!') % u.filename)

	def runFinished(self, retval):
		remove("/tmp/" + u.filename);
		self.readTmp()
		if pathExists("/tmp/install"):
			msg = _("Please select a destination media,\nto install %s package.") % u.filename
			if createLink("/tmp/install/skins"):
				installPath = GetSkinsPath()
			else:
				installPath = GetPluginInstallPath()
			if installPath:
				self.pathToInstall = ''
				self.session.openWithCallback(self.runIstallation, ChoiceBox, title=msg, list = installPath)
			else:
				self.downloading()
				self['conn'].text = _("No device found to installing addon!")
			return
		plugins.readPluginList(resolveFilename(SCOPE_PLUGINS))
		self.downloading()
		self['conn'].text = (_("Addon: %s\ninstalled succesfully!") % u.filename)
		if fileExists('/tmp/.restartE2'):
			remove('/tmp/.restartE2')
			msg = 'Enigma2 will be now hard restarted to complete package installation.\nDo You want restart enigma2 now?'
			self._restartE2.go(msg)

	def runIstallation(self, answer):
		if answer:
			answer = answer and answer[1]
			self.pathToInstall = answer
			self.movingTimer.start(100, True)
		else:
			if pathExists("/tmp/install"):
				system("rm -rf /tmp/install")
			self.downloading()

	def doMovingAddon(self):
		if self.movingTimer.isActive():
			self.movingTimer.stop()
		self['conn'].text = _("Moving addon on %s, please wait...") % self.pathToInstall
		if createLink("/tmp/install/skins"):
			if not pathExists(self.pathToInstall) and (self.pathToInstall != eEnv.resolve('${datadir}/enigma2')):
				system('mkdir %s' % self.pathToInstall)
			system("cp -r /tmp/install/skins/* %s/" % self.pathToInstall)
			createLink(self.pathToInstall, True)
			createSkinUninstall('Skin_%s' % u.filename,self.pathToInstall)
		else:
			system("cp -r /tmp/install/* %s/" % self.pathToInstall)
		plugins.readPluginList(resolveFilename(SCOPE_PLUGINS))
		self['conn'].text = _("Addon installed succesfully!")
		if pathExists("/tmp/install"):
			system("rm -rf /tmp/install")
		self.downloading()
			
	def downloading(self, state=""):
		if state == "Install":	
			self['conn'].text = _('Do you want install the addon: %s?') % u.filename
			self["key_red"].text = _("No")
			self["key_green"].setText(_("Yes"))
			self.installFile = True
		elif state == "Installing":
			self['type'].text = _("Installing")
			self["key_red"].text = _("Cancel")
			self["key_green"].setText('')
			self.installFile = False
		else:
			if self.activityTimer.isActive():
				self.activityTimer.stop()
				self["progressbar"].value = 0
				self.activity = 0
			self['type'].text = ''
			self["key_red"].setText(_("Exit"))
			self["key_green"].setText(_("Install"))
			self.installFile = False

	def cancel(self):
		if self.movingTimer.isActive():
			self.movingTimer.stop()
		if self.activityTimer.isActive():
			self.activityTimer.stop()
			self["progressbar"].value = 0
		if self.installFile:
			self.downloading()
			return
		if not self.container.running():
			del self.container.appClosed[:]
			del self.container
			self.close()
		else:
			self.container.kill()
			self['conn'].text = (_('Process Killed by user.\nAddon not installed correctly!'))
	
class	RRemove(Screen):
	__module__ = __name__

	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self.deleteCmd = []
		self['list'] = List(self.list)
		self['conn'] = StaticText("")
		self["title"] = Label(_("Remove Addons"))
		self["key_red"] = StaticText(_("Exit"))
		self["key_green"] = Label(_("Remove"))
		self['type'] = StaticText()
		self["progressbar"] = Progress()
		self["progressbar"].range = 100
		self['spaceused'] = Progress()
		self['spaceusedtext'] = StaticText()

		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)
		self.activity = 0
		self.activityTimer = eTimer()
		self.activityTimer.callback.append(self.doActivityTimer)
		self.deleteTimer = eTimer()
		self.deleteTimer.callback.append(self.doDeleteTimer)

		self._restartE2 = restartE2(session)
		self.installFile = False

		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'green': self.KeyOk,
			'ok': self.KeyOk,
			"red": self.cancel,
			'back': self.cancel
		})

		self.onLayoutFinish.append(self.readTmp)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_("Remove Addons"))
		diskSpace = getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self["spaceusedtext"].text = _("Free space: %d kB (%d%%)") % (int(diskSpace[0]), percFree)
		self["spaceused"].setValue(percUsed)

	def doActivityTimer(self):
		self.activity += 5
		if self.activity == 100:
			self.activity = 0
		self["progressbar"].value = self.activity

	def fillList(self):
		del self.list[:]
		for fil in self.deleteCmd: 
			self.list.append((fil, fil))
		self['list'].setList(self.list)

	def readTmp(self):
		self["progressbar"].value = 0
		if not self.container.running() and not self.installFile:
			loadunidir.load()
			del self.list[:]
			for fil in loadunidir.uni_list: 
				if fil[0] < 2:
					self.list.append((fil[0], fil[1]))
				else:
					self.list.append((fil[1], fil[1] [:-10]))
			self['list'].setList(self.list)
	
	def KeyOk(self):
		if self.installFile:
			self.removeAddons()
			return
		if not self.container.running() and not self.installFile:
			if self['list'].getIndex() == 0:
				self.removePicon()
				return
			elif self['list'].getIndex() == 1:
				self.removeEpg()
				return
			elif self['list'].getIndex() >= 2:
				u.filename = self['list'].getCurrent()[0]
				self.downloading("Remove")
				return

	def removeAddons(self):
		if u.filename == "RemovePicon" or u.filename == "RemoveEpg":
			self['conn'].text = (_('Removing: %s.\nPlease wait...') % self['list'].getCurrent()[1])
			self["key_red"].text = ''
			self.deleteTimer.start(200,True)
		else:
			self.downloading("Removing")
			self.activityTimer.start(100, False)
			self['conn'].text = (_('Removing: %s.\nPlease wait...') % u.filename[:-10])
			self.container.execute("/usr/uninstall/" + u.filename)

	def removePicon(self):
		del self.deleteCmd[:]
		for dev in GetPluginInstallPath():
			for file in listdir(dev[1]):
				if file in ('picon', 'picon_oled', 'piconlcd', 'piconSys', 'piconProv', 'piconSat'):
					self.deleteCmd.append("%s/%s" % (dev[1], file))
		if self.deleteCmd:
			u.filename = "RemovePicon"
			self.downloading("Picon")
			self.fillList()
		else:
			if self.installFile:
				self.installFile = False
				self.readTmp()
			self.downloading()
			self['conn'].text = _('Nothing to uninstall!')
			
	def removeEpg(self):
		del self.deleteCmd[:]
		for dev in GetEpgPath():
			for file in sorted(listdir(dev[1]), reverse=True):
				if file in ('epg.dat','epg.dat.save','ext.epg.dat','crossepg'):
					if file == 'crossepg':
						if fileExists("%s/%s/ext.epg.dat" % (dev[1],file)):
							self.deleteCmd.append("%s/%s/ext.epg.dat" % (dev[1],file))
					else:
						self.deleteCmd.append("%s/%s" % (dev[1],file))
		if self.deleteCmd:
			u.filename = "RemoveEpg"
			self.downloading("Epg")
			self.fillList()
		else:
			if self.installFile:
				self.installFile = False
				self.readTmp()
			self.downloading()
			self['conn'].text = _('Nothing to uninstall!')

	def doDeleteTimer(self):
		itemToRemove =  self['list'].getCurrent()[1]
		system('rm -rf %s' % itemToRemove)
		self['type'].text = _('%s\nis removed succesfully.') % itemToRemove
		if u.filename == "RemovePicon":
			self.removePicon()
		else:
			self.removeEpg()

	def runFinished(self, retval):
		plugins.readPluginList(resolveFilename(SCOPE_PLUGINS))
		self.readTmp()
		self['conn'].text = (_('Addons: %s\nremoved succesfully.') % u.filename[:-10])
		self.downloading()
		if fileExists('/tmp/.restartE2'):
			remove('/tmp/.restartE2')
			msg = 'Enigma2 will be now hard restarted to complete package installation.\nDo You want restart enigma2 now?'
			self._restartE2.go(msg, True)

	def downloading(self, state=""):
		if state == "Picon":
			self['conn'].text = _('Remove selected picons')
			self["key_red"].text = _("Cancel")
			self["key_green"].setText(_("Remove"))
			self.installFile = True
		elif state == "Epg":
			self['conn'].text = _('Remove selected EPG data file')
			self["key_red"].text = _("Cancel")
			self["key_green"].setText(_("Remove"))
			self.installFile = True
		elif state == "Remove":
			self['conn'].text = _('Do you want remove the addon: %s?') % u.filename[:-10]
			self["key_red"].text = _("No")
			self["key_green"].setText(_("Yes"))
			self.installFile = True
		elif state == "Removing":
			self['type'].text = _("Removing")
			self["key_red"].text = _("Cancel")
			self["key_green"].setText("")
			self.installFile = False
		else:
			if self.activityTimer.isActive():
				self.activityTimer.stop()
				self["progressbar"].value = 0
				self.activity = 0
			self['type'].text = ''
			self["progressbar"].value = 0
			self["key_red"].text = _("Exit")
			self["key_green"].setText(_("Remove"))
			self.installFile = False

	def cancel(self):
		if self.activityTimer.isActive():
			self.activityTimer.stop()
			self["progressbar"].value = 0
		if self.installFile:
			self.downloading()
			self['conn'].text = ''
			self.readTmp()
			return
		if not self.container.running():
			del self.container.appClosed[:]
			del self.container
			self.close()
		else:
			self.container.kill()
			self['conn'].text = (_('Process Killed by user.\nAddon not removed completly!'))
