from Screens.Screen import Screen
from Screens.InputBox import InputBox
from Screens.VirtualKeyBoard import VirtualKeyBoard
from Screens.MessageBox import MessageBox
from Screens.EventView import EventViewSimple
from Screens.TimerEntry import TimerEntry
from Screens.TimerEdit import TimerSanityConflict, TimerEditList
from Components.ActionMap import ActionMap,NumberActionMap
from Components.Label import Label
from Components.Sources.StaticText import StaticText
from Components.Sources.List import List
from Components.TimerSanityCheck import TimerSanityCheck
from Components.config import config, ConfigNothing, ConfigFile
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmapAlphaTest
from Components.MenuList import MenuList
from Tools.LoadPixmap import LoadPixmap
from Tools.Directories import fileExists, SCOPE_PLUGINS
from os import system, remove as os_remove
from nemesisTool import GetSkinPath, isE232, GetPiconPath, initEpg, checkDevice as t_checkDevice
from nemesisConsole import nemesisConsole
from nemesisSetting import NSetup
from nemesisShowPanel import nemesisShowPanel
from enigma import eTimer, eEPGCache, eServiceCenter, eServiceReference
from ServiceReference import ServiceReference
from RecordTimer import RecordTimerEntry
from Components.PluginComponent import plugins
from Components.PluginList import *
from Plugins.Plugin import PluginDescriptor
import string

_isE232 = isE232()
epg = eEPGCache.getInstance()
configfile = ConfigFile()

def getSid(sid):
	EPG_CHANNEL_INFO_sid="%X" % int(string.split(sid,":")[0],16)
	temp="%X" % int(string.split(sid,":")[1],16)
	EPG_CHANNEL_INFO_tsid="%X" % int(string.split(sid,":")[2],16)
	EPG_CHANNEL_INFO_onid="%X" % int(string.split(sid,":")[3],16)
	return '1:0:1:'+EPG_CHANNEL_INFO_sid+':'+EPG_CHANNEL_INFO_tsid+':'+EPG_CHANNEL_INFO_onid+':'+temp+':0:0:0:'

class nemesisEpgPanel(Screen):
	__module__ = __name__

	def getPlugins(self):
		plist = []
		pluginlist = plugins.getPlugins(PluginDescriptor.WHERE_PLUGINMENU)
		for plugin in pluginlist:
			plist.append(PluginEntryComponent(plugin))
		for p in plist:
			if (p[0].name.find("EPGSearch") != -1):
				self.epgserchplugin = p[0]
	
	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self['list'] = List(self.list)
		self.myEpgDownloader = "sleep %s && /usr/crossepg/download_epg.sh %s" % (config.nemepg.zapdelay.value, config.nemepg.demux.value)
		self.myEpgConverter = "sleep %s && /usr/crossepg/convert_epg.sh" % config.nemepg.zapdelay.value
		self.ref = None
		self.downIMode = False
		self["title"] = Label(_("EPG Control Center"))
		self["key_red"] = Label(_("Exit"))
		self['conn'] = StaticText()
		self.epgserchplugin = ''
		self.getPlugins()

		isCrossEPG = False
		if fileExists(resolveFilename(SCOPE_PLUGINS, "SystemPlugins/CrossEPG/plugin.py")):
			isCrossEPG = True

		self.epgMenuList = [
			('searchEPGe2',_('EPG Search (enigma2)'),'icons/search.png',self.epgserchplugin),
			('searchEPG',_('EPG Search (Nemesis)'),'icons/search.png',True),
			('searchEPGLast',_('EPG Search History (Nemesis)'),'icons/search.png',True),
			('setupCrossEPG',_('CrossEPG Setup'),'icons/extensions.png',isCrossEPG),
			('downloadEPG',_('Download EPG with CrossEPG'),'icons/get.png',True),
			('reloadEPG',_('Load EPG in Enigma cache'),'icons/manual.png',True),
			('eraseEPG',_('Erase Enigma EPG cache'),'icons/remove.png',not _isE232),
			('backupEPG',_('Backup Enigma EPG cache'),'icons/save.png',not _isE232),
			('createTIMER',_('Create EPG Timer Entry'),'icons/addtimer.png',True),
			('openTIMER',_('Open Timer List'),'icons/timer.png',True),
			('epglog',_('View EPG Timer Log'),'icons/log.png',True),
			('configEPG',_('EPG Configuration'),'icons/setup.png',True)
			]
		
		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			"red": self.__onClose,
			'back': self.__onClose
		},-2)
		
		self.saveEPGTimer = eTimer()
		self.saveEPGTimer.timeout.get().append(self.backupEPG)
		self.reloadEPGTimer = eTimer()
		self.reloadEPGTimer.timeout.get().append(self.reloadEPG)
		self.clearEPGTimer = eTimer()
		self.clearEPGTimer.timeout.get().append(self.clearEPG)
		self.onLayoutFinish.append(self.updateList)
		self.onShown.append(self.setWindowTitle)
		self["list"].onSelectionChanged.append(self.setWindowTitle)
	
	def __onClose(self):
		if self.saveEPGTimer.isActive():
			self.saveEPGTimer.stop()
		if self.reloadEPGTimer.isActive():
			self.reloadEPGTimer.stop()
		if self.clearEPGTimer.isActive():
			self.clearEPGTimer.stop()
		self.close()

	def setWindowTitle(self):
		self.setTitle("EPG Control Center")
		self['conn'].text =	''
		
	def updateList(self):
		skin_path = GetSkinPath()
		del self.list[:]
		for i in self.epgMenuList:
			if i[3]:
				self.list.append((i[0], i[1], LoadPixmap(skin_path + i[2])))
		self['list'].setList(self.list)
	
	def checkDevice(self):
		if t_checkDevice(config.misc.epgcache_filename.value[:-8]):
			return True
		self['conn'].text = _('Problem to write on %s.\nTry to reboot the machine!') % (config.misc.epgcache_filename.value + ".test")
		return False
	
	def zap(self, nref):
		self.session.nav.playService(nref)
	
	def KeyOk(self):
		sel = self["list"].getCurrent()[0]
		if (sel == "searchEPGe2"):
			if self.epgserchplugin:
				self.epgserchplugin(session=self.session, servicelist=self)
		elif (sel == "searchEPG"):
			if config.nemesis.usevkeyboard.value:
				self.session.openWithCallback(self.beginSearch, VirtualKeyBoard, title = _("Enter event to search:"), text = "")
			else:
				self.session.openWithCallback(self.beginSearch,InputBox, title = _("Enter event to search:"), windowTitle = _("Search in EPG cache"), text="")
		elif (sel == "searchEPGLast"):
			self.session.open(NEpgSearchLast)
		elif (sel == "setupCrossEPG"):
			try:
				from Plugins.SystemPlugins.CrossEPG.crossepg_main import crossepg_main
			except ImportError:
				self['conn'].text = _("The CrossEPG Plugin is not installed!\nPlease install it.")
			else:
				crossepg_main.setup(self.session)
		elif (sel == "downloadEPG"):
			if self.checkDevice():
				self.ref = self.session.nav.getCurrentlyPlayingServiceReference()
				if config.nemepg.downskyit.value:
					self.downloadItEPG()
				elif config.nemepg.downskyuk.value:
					self.downloadUkEPG()
		elif (sel == "reloadEPG"):
			if not initEpg():
				self['conn'].text = _('File epg.dat not found!\nPlease use Download EPG with CrossEPG!')
				return
			self["actions"].setEnabled(False)
			self['conn'].text = _('Load EPG data in Enigma cache from:\n%s.\nPlease Wait...') % config.misc.epgcache_filename.value
			self.reloadEPGTimer.start(500, True)
		elif (sel == "eraseEPG"):
			self['conn'].text = _('Erasing EPG Chache.\nPlease Wait...')
			self["actions"].setEnabled(False)
			self.clearEPGTimer.start(500, True)
		elif (sel == "backupEPG"):
			self['conn'].text = _('Backup Enigma EPG data on:\n%s.\nPlease Wait...') % (config.misc.epgcache_filename.value + ".save")
			self["actions"].setEnabled(False)
			self.saveEPGTimer.start(500, True)
		elif (sel == "createTIMER"):
			serviceref = ServiceReference(getSid(config.nemepg.skyitch.value))
			begin = 1239332400
			end = 1239333600
			name = "Download EPG Daily"
			description = "Please do not remove this entry!"
			timer = RecordTimerEntry(serviceref, begin, end, name, description, 66, False, 2, 1)
			timer.repeated = 127
			self.session.openWithCallback(self.finishedAdd, TimerEntry, timer)
		elif (sel == "openTIMER"):
			self.session.open(TimerEditList)
		elif (sel == "epglog"):
			self.session.open(nemesisShowPanel, '/usr/log/crossepg.log' ,_('EPG Timer log')) 
		elif (sel == "configEPG"):
			self.session.openWithCallback(self.saveConfig, NSetup, "epg")
	
	def finishedAdd(self, answer):
		if answer[0]:
			entry = answer[1]
			simulTimerList = self.session.nav.RecordTimer.record(entry)
			if simulTimerList is not None:
				for x in simulTimerList:
					if x.setAutoincreaseEnd(entry):
						self.session.nav.RecordTimer.timeChanged(x)
				simulTimerList = self.session.nav.RecordTimer.record(entry)
				if simulTimerList is not None:
					self.session.openWithCallback(self.finishSanityCorrection, TimerSanityConflict, simulTimerList)
	
	def finishSanityCorrection(self, answer):
		self.finishedAdd(answer)
	
	def saveConfig(self, *answer):
		if answer:
			configfile.save()
	
	def beginSearch(self, cmd):
		if cmd is not None:
			self.session.open(NEpgSearch, cmd)

	def downloadItEPG(self):
		self.zap(eServiceReference(getSid(config.nemepg.skyitch.value)))
		cmd = self.myEpgDownloader + " skyitalia"
		self.session.openWithCallback(self.afterDwnSkyIt, nemesisConsole, cmd, _('Download EPG SKY-IT is running...'))
	
	def afterDwnSkyIt(self, *answer):
		if answer[0] == nemesisConsole.EVENT_DONE:
			if config.nemepg.downskyuk.value:
				self.downloadUkEPG()
			else:
				self.zap(self.ref)
				self.callEpgConverter()
		else:
			self.zap(self.ref)
			system("rm -f /tmp/crossepg*")
				
	def downloadUkEPG(self):
		self.zap(eServiceReference(getSid(config.nemepg.skyukch.value)))
		cmd = self.myEpgDownloader + " skyuk"
		self.session.openWithCallback(self.afterDwnSkyUk, nemesisConsole, cmd, _('Download EPG SKY-UK is running...'))
	
	def afterDwnSkyUk(self, *answer):
		self.zap(self.ref)
		if answer[0] == nemesisConsole.EVENT_DONE:
			self.callEpgConverter()
		else:
			system("rm -f /tmp/crossepg*")
	
	def callEpgConverter(self):
		self.ref = None
		self.session.openWithCallback(self.downEpgFinish, nemesisConsole, self.myEpgConverter , _('Convert EPG...'))
	
	def downEpgFinish(self, *answer):
		system("rm -f /tmp/crossepg*")
		if answer[0] == nemesisConsole.EVENT_DONE:
			self['conn'].text = _('Load EPG data in Enigma cache from:\n%s.\nPlease Wait...') % config.misc.epgcache_filename.value
			self["actions"].setEnabled(False)
			self.reloadEPGTimer.start(1000, True)
			
	def reloadEPG(self):
		if self.reloadEPGTimer.isActive():
			self.reloadEPGTimer.stop()
		if not fileExists(config.misc.epgcache_filename.value):
			system("cp " + config.misc.epgcache_filename.value + ".save " + config.misc.epgcache_filename.value)
		if config.nemepg.clearcache.value:
			try:
				epg.flushEPG()
			except:
				pass
		epg.load()
		try:
			epg.save()
			if fileExists(config.misc.epgcache_filename.value):
				system("cp " +  config.misc.epgcache_filename.value + " " + config.misc.epgcache_filename.value + ".save")
		except:
			pass
		self.__onClose()

	def clearEPG(self):
		if self.clearEPGTimer.isActive():
			self.clearEPGTimer.stop()
		try:
			epg.flushEPG()
		except:
			pass
		if config.nemepg.clearbackup.value:
			system("rm -f " + config.misc.epgcache_filename.value + ".save")
		self["actions"].setEnabled(True)
		self['conn'].text = _('Done')
	
	def backupEPG(self):
		if self.saveEPGTimer.isActive():
			self.saveEPGTimer.stop()
		epg.save()
		if fileExists(config.misc.epgcache_filename.value):
			system("mv " + config.misc.epgcache_filename.value + " " + config.misc.epgcache_filename.value + ".save")
		self["actions"].setEnabled(True)
		self['conn'].text = _('Done')

class NEpgSearch(Screen):
	__module__ = __name__

	def __init__(self, session, cmd):
		Screen.__init__(self, session)
		self.myserv = []
		self.list = []
		self['list'] = List(self.list)
		self.cmd = cmd
		self['actions'] = ActionMap(['WizardActions'],
		{
			'ok': self.KeyOk,
			'back': self.close
		})
		self.onLayoutFinish.append(self.searchEvent)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_("EPG Search"))
	
	def searchEvent(self):
		mycache = []
		if len(self.cmd) > 1:
			if fileExists('/etc/epg_cache'):
				f = open('/etc/epg_cache', 'r')
				for line in f.readlines():
					if (self.cmd != line.strip()):
						mycache.append(line.strip())
				f.close()
		mycache.append(self.cmd)
		
		if len(mycache) > 10:
			mycache = mycache[1:]
		f1 = open('/etc/epg_cache', 'w')
		for fil in mycache:
			f1.write((fil + '\n'))
		f1.close()
		
		del self.list[:]
		self.myserv = epg.search(('IR',config.nemepg.elemnum.value,eEPGCache.PARTIAL_TITLE_SEARCH,self.cmd,1))
		if self.myserv:
			for fil in self.myserv:
				sserv = ServiceReference(fil[1])
				provider = sserv.getServiceName()
				if config.nemesis.picontype.value == "Reference":
					picon = self.find_Picon(fil[1])
				else:
					picon = self.find_Picon(provider)
				event = epg.lookupEventId(sserv.ref, int(fil[0]))
				strview = (((((((event.getBeginTimeString() + '   ') + provider) + '   ') + event.getEventName()) + '   (') + ('%d min' % (event.getDuration() / 60))) + ')')
				ext = event.getShortDescription()
				if ext == '':
					ext = event.getExtendedDescription()
				if len(ext) > 200:
					ext = (ext[:200] + '..')
				self.list.append((strview, ext, LoadPixmap(picon)))
			self['list'].setList(self.list)
		else:
			self.menTimer = eTimer()
			self.menTimer.timeout.get().append(self.SearchNot)
			self.menTimer.start(100, False)
	
	def find_Picon(self, sname):
		searchPaths = GetPiconPath()
		
		if config.nemesis.picontype.value == "Reference":
			pos = sname.rfind(':')
			if (pos != -1):
				sname = sname[:pos].rstrip(':').replace(':', '_')
		
		for path in searchPaths:
			pngname = (((path % 'picon') + sname) + '.png')
			if fileExists(pngname):
				return pngname
		return '/usr/share/enigma2/skin_default/picon_default.png'
	
	def KeyOk(self):
		myi = self['list'].getIndex()
		if self.myserv:
			fil = self.myserv[myi]
			sserv = ServiceReference(fil[1])
			event = epg.lookupEventId(sserv.ref, int(fil[0]))
			self.session.open(EventViewSimple, event, sserv)
	
	def SearchNot(self):
		self.menTimer.stop()
		self.session.open(MessageBox, _('Sorry no events found matching to the search criteria.'), MessageBox.TYPE_INFO)

class NEpgSearchLast(Screen):
	__module__ = __name__

	def __init__(self, session):
		Screen.__init__(self, session)
		mycache = []
		if fileExists('/etc/epg_cache'):
			f = open('/etc/epg_cache', 'r')
			for line in f.readlines():
				mycache.append(line.strip())
			f.close()
		
		if (len(mycache) > 0):
			mycache.reverse()
		
		self['list'] = MenuList(mycache)
		self['key_red'] = Label(_('Clear History'))
		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			'back': self.close,
			'red': self.clearCache
		})

	def KeyOk(self):
		myi = self['list'].getCurrent()
		if myi:
			self.session.open(NEpgSearch, myi)

	def clearCache(self):
		if fileExists('/etc/epg_cache'):
			os_remove('/etc/epg_cache')
		mycache = []
		self['list'].l.setList(mycache)

class nemesisEpgPanelOpen():
	__module__ = __name__
	
	def __init__(self):
		self['nemesisEpgPanelOpen'] = ActionMap(['InfobarSubserviceSelectionActions'], {'nemesisEpanel': self.NemesisEpgOpen})
	
	def NemesisEpgOpen(self):
		service = self.session.nav.getCurrentService()
		ts = service and service.timeshift()
		if ts:
			if ts.isTimeshiftActive():
				return
		self.session.open(nemesisEpgPanel)
