from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from enigma import eTimer, eDVBDB, eConsoleAppContainer, ePicLoad
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
from os import system, remove, listdir, chdir, getcwd, rename, path
from nemesisTool import nemesisTool, GetSkinPath, restartE2
from nemesisConsole import nemesisConsole
from nemesisDownloader import nemesisDownloader
from Tools import Notifications
import xml.etree.cElementTree as x

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
		count = 0
		for fil in pkgs:
			if (fil.find('_remove.sh') != -1 or fil.find('_delete.sh') != -1):
				self.uni_list.append([count,fil])
				count +=1

loadunidir = loadUniDir()
		
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
		self["key_red"] = Label(_("Cancel"))
		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)
		self.containerExtra = eConsoleAppContainer()
		self.containerExtra.appClosed.append(self.runFinishedExtra)
		self.reloadTimer = eTimer()
		self.reloadTimer.timeout.get().append(self.relSetting)
			
		self.linkAddons = t.readAddonsUrl()
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
			"red": self.cancel,
			'back': self.cancel
		})
		self.onLayoutFinish.append(self.updateList)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		diskSpace = t.getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self.setTitle("%s - Free: %d kB (%d%%)" % ( _("Addons Manager"), int(diskSpace[0]), percFree))
		self["spaceused"].setValue(percUsed)

	def KeyOk(self):
		self['conn'].text = ('')
		if not self.container.running():
			sel = self["list"].getCurrent()[0]
			if (sel == "NAddons"):
				self['conn'].text = (_("Connetting to addons server.\nPlease wait..."))
				u.typeDownload = 'A'
				self.container.execute({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value] + "wget " + self.linkAddons + "/addonsE2.xml -O /tmp/addons.xml")
			elif (sel == "NExtra"):
				self['conn'].text = (_("Connetting to addons server.\nPlease wait..."))
				u.typeDownload = 'E'
				if self.linkExtra != None:
					self.containerExtra.execute({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value] + "wget " + self.linkExtra + "iuregdoiuwqcdiuewq/tmp.tmp -O /tmp/tmp.tmp")
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
					msg = _('No Upgrade available!\nYour decoder is up to date.')
					self.session.open(MessageBox, msg , MessageBox.TYPE_INFO)
					return
				if int(t.getVarSpaceKb()[0]) < self.FREESPACENEEDUPGRADE:
					msg = _('Not enough free space on flash to perform Upgrade!\nUpgrade require at least %d kB free on Flash.\nPlease remove some addons or skins before upgrade.') % self.FREESPACENEEDUPGRADE
					self.session.open(MessageBox, msg , MessageBox.TYPE_INFO)
					return
				self.session.openWithCallback(self.runUpgrade, MessageBox, _("Do you want to update your Dreambox?")+"\n"+_("\nAfter pressing OK, please wait!"))
			elif (sel == "NReload"):
				self['conn'].text = _("Reload settings, please wait...")
				self.reloadTimer.start(250, True)

	def relSetting(self):
		u.reloadSetting()
		self['conn'].text = _("Settings reloaded succesfully!")

	def runUpgrade(self, result):
		if result:
			try:
				from Plugins.SystemPlugins.SoftwareManager.plugin import UpdatePlugin
			except ImportError:
				self.session.open(MessageBox, _("The Softwaremanagement extension is not installed!\nPlease install it."), type = MessageBox.TYPE_INFO,timeout = 10 )
			else:
				self.session.open(UpdatePlugin, GetSkinPath())

	def runFinishedExtra(self, retval):
		if fileExists('/tmp/tmp.tmp'):
			try:
				f = open("/tmp/tmp.tmp", "r")
				line = f.readline() [:-1]
				f.close()
				self.container.execute({True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value] + "wget " + self.linkExtra + "iuregdoiuwqcdiuewq/" + line + " -O /tmp/addons.xml")
			except:
				self['conn'].text = (_("Server not found!\nPlease check internet connection."))
			remove('/tmp/tmp.tmp')
		else:
			self['conn'].text = (_("Server not found!\nPlease check internet connection."))

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
	
	def cancel(self):
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
		self["key_red"] = Label(_("Cancel"))
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
	EVENT_CURR = 0

	def __init__(self, session):
		Screen.__init__(self, session)
		self.IS_SKIN_SECTION = False
		self.url = ''
		self.filename = ''
		self.dstfilename = ''
		self.download = None
		self.installFile = False
		if u.pluginType.find('Skin') >= 0:
			self.skinName = "RAddonsDownSkin"
			self.IS_SKIN_SECTION = True
			self["preview"] = Pixmap()
			self["preview"].hide()
			self.checkPreviewContainer = eConsoleAppContainer()
			self.checkPreviewContainer.appClosed.append(self.fetchFinished)
		self.list = []
		self['list'] = List(self.list)
		self['conn'] = StaticText(_("Loading elements.\nPlease wait..."))
		self["key_red"] = Label(_("Cancel"))
		self["key_green"] = Label(_("Download"))
		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)

		self['type'] = StaticText('')
		self["progressbar"] = Progress()
		self["progressbar"].range = 1000
		self['spaceused'] = Progress()
		self['spaceusedtext'] = StaticText()
		self.activity = 0
		self.activityTimer = eTimer()
		self.activityTimer.callback.append(self.doActivityTimer)

		self._restartE2 = restartE2(session)
		self.linkAddons = t.readAddonsUrl()
		self.linkExtra = t.readExtraUrl()

		if self.IS_SKIN_SECTION:
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
		
		self.onLayoutFinish.append(self.loadPlugin)
		self.onShown.append(self.setWindowTitle)

		if self.IS_SKIN_SECTION:
			self["list"].onSelectionChanged.append(self.checkPreview)
	
	def setWindowTitle(self):
		self.setTitle(_("Download ") + str(u.pluginType))
		diskSpace = t.getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self["spaceusedtext"].text = _("Free space: %d kB (%d%%)") % (int(diskSpace[0]), percFree)
		self["spaceused"].setValue(percUsed)

	def doActivityTimer(self):
		self.activity += 10
		if self.activity == 1000:
			self.activity = 0
		self["progressbar"].value = self.activity

	def KeyOk(self):
		if self['list'].count() == 0:
			self.close()
		if not self.container.running() and self.download == None and not self.installFile:
			self.sel = self['list'].getIndex() 
			for tag in loadxml.plugin_list: 
				if tag [0] == u.pluginIndex:
					if tag [7] == self.sel:
						u.addonsName = tag [3]
						self.downloadAddons()
						return
		if self.installFile:
			self.installAddons(True)
			return
			
	def loadPlugin(self):
		self["progressbar"].value = 0
		del self.list[:]
		for tag in loadxml.plugin_list: 
			if tag [0] == u.pluginIndex:
				self.list.append((tag [3], tag [3]))
		self['list'].setList(self.list)
		if self.IS_SKIN_SECTION:
			self.checkPreview()
		else:
			self['conn'].text = _('Elements Loaded!.\nPlease select one to install.')

	def checkPreview(self):
		self.activityTimer.start(20, False)
		self['conn'].text = _('Loading Preview....\nPlease Wait..')
		self["preview"].hide()
		if self.checkPreviewContainer.running():
			self.checkPreviewContainer.kill()
		if len(self["list"].list) > 0:
			sel = self['list'].getIndex() 
			for tag in loadxml.plugin_list: 
				if tag [0] == u.pluginIndex:
					if tag [7] == sel:
						u.addonsName = tag [3]
						break
			self.getAddonsPar()
			system("rm -f /tmp/skin_preview.png")
			url = "%s%s/%s.png"% (self.linkAddons, u.dir, u.filename)
			cmd = {True:'/var/etc/proxy.sh && ',False:''}[config.proxy.isactive.value] + "wget " + url + " -O /tmp/skin_preview.png"
			self.checkPreviewContainer.execute(cmd)

	def fetchFinished(self, retval):
		if self.activityTimer.isActive():
			self.activityTimer.stop()
			self["progressbar"].value = 0
			self.activity = 0
		if path.exists("/tmp/skin_preview.png") and u.pluginType.find('Skin') >= 0:
			self.picload = ePicLoad()
			self.picload.PictureData.get().append(self.previewShow)
			self.picload.setPara((self["preview"].instance.size().width(), self["preview"].instance.size().height(), 1, 1, False, 1, "#00000000"))
			self.picload.startDecode("/tmp/skin_preview.png")	
		else:
			self['conn'].text = _('Elements Loaded!.\nPlease select one to install.')

	def previewShow(self, picInfo=None):
		ptr = self.picload.getData()
		if ptr != None:
			self["preview"].instance.setPixmap(ptr.__deref__())
			self['conn'].text = _('The selected Skin has a preview.\nPress OK button to show it on full screen.')
			self["preview"].show()
		else:
			self['conn'].text = _('Elements Loaded!.\nPlease select one to install.')
		del self.picload

	def openPreview(self):
		if path.exists("/tmp/skin_preview.png"):
			self.session.open(openPreviewScreen, "/tmp/skin_preview.png")

	def downloadAddons(self):
		self.getAddonsPar()
		if int(u.size) > int(t.getVarSpaceKb()[0]) and int(u.check) != 0:
			self["conn"].text = _('Not enough space!\nPlease delete addons before install new.')
			return
		url = {'E':self.linkExtra,'A':self.linkAddons}[u.typeDownload] + u.dir + "/" + u.filename 
		if not pathExists("/tmp/.-"):
			createDir("/tmp/.-")
		if config.proxy.isactive.value:
			cmd = "/var/etc/proxy.sh && wget %s -O /tmp/.-/%s" % (url ,".-")
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
		if fileExists(self.dstfilename):
			remove(self.dstfilename)
		self["conn"].text = _("Download file %s\nfailed!") % (self.filename)
		self["type"].text = error_message
		self.EVENT_CURR = self.EVENT_KILLED
		self.downloading()

	def finished(self, string = ""):
		if self.download:
			print "[Download_finished] " + str(string)
			self.EVENT_CURR = self.EVENT_DONE
			self["conn"].text = _("Download file %s\nfinished!") % (self.filename)
			self["type"].text = ''
			self.executedScript(self.EVENT_CURR)

	def cancel(self):
		if self.activityTimer.isActive():
			self.activityTimer.stop()
			self["progressbar"].value = 0
		if self.download:
			self.download.stop()
			if fileExists(self.dstfilename):
				remove(self.dstfilename)
			self.EVENT_CURR = self.EVENT_KILLED
			self.executedScript(self.EVENT_CURR)
			self.downloading()
			return
		if self.installFile:
			if pathExists('/tmp/.-'):
				system("rm -rf /tmp/.-")
			self.downloading()
			return
		if not self.container.running():
			if self.IS_SKIN_SECTION:
				if self.checkPreviewContainer.running():
					self.checkPreviewContainer.kill()
				del self.checkPreviewContainer.appClosed[:]
				del self.checkPreviewContainer
			del self.container.appClosed[:]
			del self.container
			self.close()
		else:
			self.container.kill()
			self['conn'].text = _('Process Killed by user.\nAddon not installed correctly!')

	def executedScript(self, *answer):
		if answer[0] == nemesisConsole.EVENT_DONE:
			if fileExists('/tmp/.-/.-'):
				self.downloading("Install")
			else:
				self['conn'].text = _('File: %s not found!\nPlease check your internet connection.') % u.filename
		elif answer[0] == nemesisConsole.EVENT_KILLED:
			self['conn'].text = _('Process Killed by user!\nAddon not downloaded.')
	
	def downloading(self, state=""):
		if state == "Install":	
			self.setTitle(_('Do you want install the addon: %s?') % u.addonsName)
			self["key_red"].setText(_("No"))
			self["key_green"].setText(_("Yes"))
			self.installFile = True
		elif state == "Installing":
			self.setTitle(_('Do you want install the addon: %s?') % u.addonsName)
			self["key_red"].setText(_("Cancel"))
			self["key_green"].setText("")
			self.installFile = False
			self["progressbar"].value = 0
			self['conn'].text = ''
		else:
			if self.activityTimer.isActive():
				self.activityTimer.stop()
			self["progressbar"].value = 0
			self.activity = 0
			self.setTitle(_("Download ") + str(u.pluginType))
			self["key_red"].setText(_("Cancel"))
			self["key_green"].setText(_("Download"))
			self.installFile = False
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
				self.activityTimer.start(20, False)
				self['type'].text = _('Install')
				args = {True: '--force-overwrite --force-defaults ',False: ''}[config.nemesis.ipkg.overwriteUpgrade.value]
				args = {True: '--force-reinstall --force-defaults ',False: ''}[config.nemesis.ipkg.forceReInstall.value]
				rename("/tmp/.-/.-", "/tmp/.-/p.ipk")
				self.container.execute("ipkg " + args + "install /tmp/.-/p.ipk")
			elif (u.filename.find('.tbz2') != -1):
				self.activityTimer.start(20, False)
				self['type'].text = _('Installing')
				if u.pluginType.find('Settings') >= 0:
					self['conn'].text = _("Remove old Settings\nPlease wait...")
					u.removeSetting()	
				self.container.execute("tar -jxvf /tmp/.-/.- -C /")
			else:
				self.downloading()
				self['conn'].text = _('File: %s\nis not a valid package!') % u.filename
	
	def runFinished(self, retval):
		plugins.readPluginList(resolveFilename(SCOPE_PLUGINS))
		if pathExists('/tmp/.-'):
			system("rm -rf /tmp/.-")
		if u.pluginType.find('Settings') >= 0:
			self['conn'].text = _("Reload new Settings\nPlease wait...")
			u.reloadSetting()
		self.downloading()
		self['conn'].text = _("Addon installed succesfully!")
		if fileExists('/tmp/.restartE2'):
			remove("/tmp/.restartE2")
			msg = _("Enigma2 will be now hard restarted to complete package installation.") + "\n" + _("Do You want restart enigma2 now?")
			self._restartE2.go(msg)

	def getAddonsPar(self):
		for tag in loadxml.plugin_list: 
			if tag [0] == u.pluginIndex:
				if tag [3] == u.addonsName:
					u.filename  = tag [2] 
					u.dir  = tag [4] 
					u.size  = tag [5] 
					u.check  = tag [6] 

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
		self["key_red"] = Label(_("Cancel"))
		self["key_green"] = Label(_("Install"))
		self["key_yellow"] = Label(_("Reload /tmp"))
		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)

		self['type'] = StaticText()
		self["progressbar"] = Progress()
		self["progressbar"].range = 1000
		self['spaceused'] = Progress()
		self['spaceusedtext'] = StaticText()
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
		diskSpace = t.getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self["spaceusedtext"].text = _("Free space: %d kB (%d%%)") % (int(diskSpace[0]), percFree)
		self["spaceused"].setValue(percUsed)

	def doActivityTimer(self):
		self.activity += 10
		if self.activity == 1000:
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
			if len(loadtmpdir.tmp_list) > 0:
				self.sel = self['list'].getIndex() 
				for p in loadtmpdir.tmp_list: 
					if (p [0] == self.sel):
						u.filename = p [1]
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
				self.activityTimer.start(20, False)
				args = {True: '--force-overwrite --force-defaults ',False: ''}[config.nemesis.ipkg.overwriteUpgrade.value]
				args = {True: '--force-reinstall --force-defaults ',False: ''}[config.nemesis.ipkg.forceReInstall.value]
				self.container.execute("ipkg " + args + "install /tmp/" + u.filename)
			elif (u.filename.find('.tbz2') != -1):
				self.downloading("Installing")
				self.activityTimer.start(20, False)
				self.container.execute("tar -jxvf /tmp/" + u.filename + " -C /")
			else:
				self.downloading()
				self['conn'].text = (_('File: %s\nis not a valid package!') % u.filename)

	def runFinished(self, retval):
		plugins.readPluginList(resolveFilename(SCOPE_PLUGINS))
		remove("/tmp/" + u.filename);
		self['conn'].text = (_("Addon: %s\ninstalled succesfully!") % u.filename)
		self.readTmp()
		self.downloading()
		if fileExists('/tmp/.restartE2'):
			remove('/tmp/.restartE2')
			msg = 'Enigma2 will be now hard restarted to complete package installation.\nDo You want restart enigma2 now?'
			self._restartE2.go(msg)

	def downloading(self, state=""):
		if state == "Install":	
			self.setTitle(_('Do you want install the addon: %s?') % u.addonsName)
			self["key_red"].setText(_("No"))
			self["key_green"].setText(_("Yes"))
			self.installFile = True
		elif state == "Installing":
			self['type'].text = _("Installing")
			self.setTitle('')
			self["key_red"].setText(_("Cancel"))
			self["key_green"].setText('')
			self.installFile = False
		else:
			if self.activityTimer.isActive():
				self.activityTimer.stop()
				self["progressbar"].value = 0
				self.activity = 0
			self['type'].text = ''
			self.setTitle(_("Manual Installation"))
			self["key_red"].setText(_("Cancel"))
			self["key_green"].setText(_("Install"))
			self.installFile = False

	def cancel(self):
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
		self['list'] = List(self.list)
		self['conn'] = StaticText("")
		self["title"] = Label(_("Remove Addons"))
		self["key_red"] = Label(_("Cancel"))
		self["key_green"] = Label(_("Remove"))
		self.container = eConsoleAppContainer()
		self.container.appClosed.append(self.runFinished)

		self['type'] = StaticText()
		self["progressbar"] = Progress()
		self["progressbar"].range = 1000
		self['spaceused'] = Progress()
		self['spaceusedtext'] = StaticText()
		self.activity = 0
		self.activityTimer = eTimer()
		self.activityTimer.callback.append(self.doActivityTimer)

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
		diskSpace = t.getVarSpaceKb()
		percFree = int((diskSpace[0] / diskSpace[1]) * 100)
		percUsed = int(((diskSpace[1] - diskSpace[0]) / diskSpace[1]) * 100)
		self["spaceusedtext"].text = _("Free space: %d kB (%d%%)") % (int(diskSpace[0]), percFree)
		self["spaceused"].setValue(percUsed)

	def doActivityTimer(self):
		self.activity += 10
		if self.activity == 1000:
			self.activity = 0
		self["progressbar"].value = self.activity

	def readTmp(self):
		self["progressbar"].value = 0
		if not self.container.running() and not self.installFile:
			loadunidir.load()
			del self.list[:]
			if len(loadunidir.uni_list) > 0:
				for fil in loadunidir.uni_list: 
					self.list.append((fil[1], fil[1] [:-10]))
			else:
				self['conn'].text = (_("Nothing to uninstall!"))
			self['list'].setList(self.list)
	
	def KeyOk(self):
		if self['list'].count() == 0:
			self.close()
		if not self.container.running() and not self.installFile:
			if len(loadunidir.uni_list) > 0:
				self.sel = self['list'].getIndex() 
				for p in loadunidir.uni_list: 
					if (p [0] == self.sel):
						u.filename = p [1]
				self.downloading("Remove")
				return
		if self.installFile:
				self.removeAddons(True)
				return
	
	def removeAddons(self, answer):
		if (answer is True):
			self.activityTimer.start(20, False)
			self.downloading("Removing")
			self['conn'].text = (_('Removing: %s.\nPlease wait...') % u.filename[:-10])
			self.container.execute("/usr/uninstall/" + u.filename)
	
	def runFinished(self, retval):
		plugins.readPluginList(resolveFilename(SCOPE_PLUGINS))
		self.readTmp()
		self['conn'].text = (_('Addons: %s\nremoved succeffully.') % u.filename[:-10])
		self.downloading()
		if fileExists('/tmp/.restartE2'):
			remove('/tmp/.restartE2')
			msg = 'Enigma2 will be now hard restarted to complete package installation.\nDo You want restart enigma2 now?'
			self._restartE2.go(msg, True)

	def downloading(self, state=""):
		if state == "Remove":
			self.setTitle(_('Do you want remove the addon: %s?') % u.filename[:-10])
			self["key_red"].setText(_("No"))
			self["key_green"].setText(_("Yes"))
			self.installFile = True
		elif state == "Removing":
			self['type'].text = _("Removing")
			self.setTitle('')
			self["key_red"].setText(_("Cancel"))
			self["key_green"].setText("")
			self.installFile = False
		else:
			if self.activityTimer.isActive():
				self.activityTimer.stop()
				self["progressbar"].value = 0
				self.activity = 0
			self['type'].text = ''
			self["progressbar"].value = 0
			self.setTitle(_("Remove Addons"))
			self["key_red"].setText(_("Cancel"))
			self["key_green"].setText(_("Remove"))
			self.installFile = False

	def cancel(self):
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
			self['conn'].text = (_('Process Killed by user.\nAddon not removed completly!'))
