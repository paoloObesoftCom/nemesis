# -*- coding: iso-8859-1 -*-
# (c) 2006 Stephan Reichholf
# This Software is Free, use it where you want, when you want for whatever you want and modify it if you want but don't remove my copyright!
from Screens.Screen import Screen
from Screens.Standby import TryQuitMainloop
from Screens.MessageBox import MessageBox
from Components.ActionMap import NumberActionMap
from Components.Pixmap import Pixmap
from Components.Sources.StaticText import StaticText
from Components.MenuList import MenuList
from Plugins.Plugin import PluginDescriptor
from Components.config import config
from Tools.Directories import resolveFilename, SCOPE_PLUGINS, fileExists
from os import path, walk
from enigma import eEnv
from Nemesis.nemesisTool import GetSkinsPath

class SkinSelector(Screen):
	# for i18n:
	# _("Choose your Skin")
	skinlist = []
	root = eEnv.resolve("${datadir}/enigma2")

	def __init__(self, session, args = None):

		Screen.__init__(self, session)

		self.skinlist = []
		self.previewPath = ""
		self["key_red"] = StaticText(_("Close"))
		self["introduction"] = StaticText(_("Press OK to activate the selected skin."))
		self["SkinList"] = MenuList(self.skinlist)
		self["Preview"] = Pixmap()

		self["actions"] = NumberActionMap(["WizardActions", "InputActions", "EPGSelectActions"],
		{
			"ok": self.ok,
			"back": self.close,
			"red": self.close,
			"up": self.up,
			"down": self.down,
			"left": self.left,
			"right": self.right,
			"info": self.info,
		}, -1)

		self.onLayoutFinish.append(self.layoutFinished)

	def layoutFinished(self):
		for skinPath in GetSkinsPath():
			path.walk(skinPath[1], self.find, "")
		self.skinlist.sort()
		tmp = config.skin.primary_skin.value.find('/skin.xml')
		if tmp != -1:
			tmp = config.skin.primary_skin.value[:tmp]
			idx = 0
			for skin in self.skinlist:
				if skin[0] == tmp:
					break
				idx += 1
			if idx < len(self.skinlist):
				self["SkinList"].moveToIndex(idx)
		self.loadPreview()

	def readTitle(self, dirname, subdir = None):
		path = '%s/skin.xml.title' % dirname
		
		if not fileExists(path):
			if subdir:
				return subdir
			return "Default Skin"
			
		title = open(path, 'r').readline()[:-1]
		if not title:
			if subdir:
				return subdir
			return "Default Skin"
		return title

	def up(self):
		self["SkinList"].up()
		self.loadPreview()

	def down(self):
		self["SkinList"].down()
		self.loadPreview()

	def left(self):
		self["SkinList"].pageUp()
		self.loadPreview()

	def right(self):
		self["SkinList"].pageDown()
		self.loadPreview()

	def info(self):
		aboutbox = self.session.open(MessageBox,_("Enigma2 Skinselector\n\nIf you experience any problems please contact\nstephan@reichholf.net\n\n\xA9 2006 - Stephan Reichholf"), MessageBox.TYPE_INFO)
		aboutbox.setTitle(_("About..."))

	def find(self, arg, dirname, names):
		for x in names:
			if x == "skin.xml":
				if dirname <> self.root:
					subdir = dirname.split("/")
					subdir = subdir[len(subdir) - 1]
					title = self.readTitle(dirname, subdir)
				else:
					subdir = "Default Skin"
					dirname = self.root
					title = self.readTitle(dirname)
				self.skinlist.append((title, subdir,dirname))

	def ok(self):
		if self["SkinList"].getCurrent()[1] == "Default Skin":
			skinfile = "skin.xml"
		else:
			skinfile = self["SkinList"].getCurrent()[1]+"/skin.xml"

		print "Skinselector: Selected Skin: "+self.root+skinfile
		config.skin.primary_skin.value = skinfile
		config.skin.primary_skin.save()
		restartbox = self.session.openWithCallback(self.restartGUI,MessageBox,_("GUI needs a restart to apply a new skin\nDo you want to Restart the GUI now?"), MessageBox.TYPE_YESNO)
		restartbox.setTitle(_("Restart GUI now?"))

	def loadPreview(self):
		try:
			pngpath = "%s/prev.png" % self["SkinList"].getCurrent()[2]
		except:
			pngpath = resolveFilename(SCOPE_PLUGINS, "SystemPlugins/SkinSelector/noprev.png")

		if not path.exists(pngpath):
			pngpath = resolveFilename(SCOPE_PLUGINS, "SystemPlugins/SkinSelector/noprev.png")

		if self.previewPath != pngpath:
			self.previewPath = pngpath

		self["Preview"].instance.setPixmapFromFile(self.previewPath)

	def restartGUI(self, answer):
		if answer is True:
			self.session.open(TryQuitMainloop, 3)

def SkinSelMain(session, **kwargs):
	session.open(SkinSelector)

def SkinSelSetup(menuid, **kwargs):
	if menuid == "system":
		return [(_("Skin"), SkinSelMain, "skin_selector", None)]
	else:
		return []

def Plugins(**kwargs):
	return PluginDescriptor(name="Skinselector", description=_("Select Your Skin"), where = PluginDescriptor.WHERE_MENU, needsRestart = False, fnc=SkinSelSetup)
