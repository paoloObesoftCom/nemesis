# -*- coding: ISO-8859-1 -*-
#===============================================================================
# ledManager 0.2 Plugin by DarkVolli 2009
# Big thanks to dr.best for his support in autostart...
# www.dreambox-tools.info
#
# This is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2, or (at your option) any later
# version.
#===============================================================================

from Plugins.Plugin import PluginDescriptor
from Screens.Screen import Screen
from Screens.Standby import TryQuitMainloop
from Screens.MessageBox import MessageBox
from Components.ActionMap import ActionMap
from Components.MenuList import MenuList
from Components.Sources.StaticText import StaticText
from Components.ConfigList import ConfigListScreen
from Components.config import config
from Components.config import ConfigSubsection
from Components.config import NoSave
from Components.config import ConfigSequence
from Components.config import ConfigSelection
from Components.config import getConfigListEntry
from Components.SystemInfo import SystemInfo
from Components.Label import Label

from Components.Sources.Boolean import Boolean
from Components.Sources.RecordState import RecordState
from Components.Converter.Combine import Combine
from Components.Renderer.FrontpanelLed import FrontpanelLed

# class for storing defaults in settings...
patterns_limits = [(0,2),(0,2),(0,2),(0,2)]
class ConfigLEDpatterns(ConfigSequence):
	def __init__(self, default):
		ConfigSequence.__init__(self, seperator = ",", limits = patterns_limits, default = default)

config.plugins.ledManager  = ConfigSubsection()
config.plugins.ledManager.led0 = ConfigLEDpatterns(default = [0,2,0,2])
config.plugins.ledManager.led1 = ConfigLEDpatterns(default = [1,1,0,0])

def autostart(session, **kwargs):
	session.screen["RecordState"] = None
	session.screen["Standby"] = None
	session.screen["RecordState"] = RecordState(session)
	session.screen["Standby"] = Boolean(fixed = False)

	combine = Combine(func = lambda s: {(False, False): 0, (False, True): 1, (True, False): 2, (True, True): 3}[(s[0].boolean, s[1].boolean)])
	combine.connect(session.screen["Standby"])
	combine.connect(session.screen["RecordState"])

	PATTERN_ON     = (20, 0xffffffff, 0xffffffff)
	PATTERN_OFF    = (20, 0, 0)
	PATTERN_BLINK  = (20, 0x55555555, 0xa7fccf7a)
	PATTERNS = (PATTERN_OFF, PATTERN_ON, PATTERN_BLINK)

	nr_leds = SystemInfo.get("NumFrontpanelLEDs", 0)
	if nr_leds == 1:
		p00 = config.plugins.ledManager.led0.value[0]
		p01 = config.plugins.ledManager.led0.value[1]
		p02 = config.plugins.ledManager.led0.value[2]
		p03 = config.plugins.ledManager.led0.value[3]
		FrontpanelLed(which = 0, boolean = False, patterns = [PATTERNS[p00],PATTERNS[p01],PATTERNS[p02],PATTERNS[p03]]).connect(combine)
	elif nr_leds == 2:
		p00 = config.plugins.ledManager.led0.value[0]
		p01 = config.plugins.ledManager.led0.value[1]
		p02 = config.plugins.ledManager.led0.value[2]
		p03 = config.plugins.ledManager.led0.value[3]
		p10 = config.plugins.ledManager.led1.value[0]
		p11 = config.plugins.ledManager.led1.value[1]
		p12 = config.plugins.ledManager.led1.value[2]
		p13 = config.plugins.ledManager.led1.value[3]
		FrontpanelLed(which = 0, boolean = False, patterns = [PATTERNS[p00],PATTERNS[p01],PATTERNS[p02],PATTERNS[p03]]).connect(combine)
		FrontpanelLed(which = 1, boolean = False, patterns = [PATTERNS[p10],PATTERNS[p11],PATTERNS[p12],PATTERNS[p13]]).connect(combine)

class ledManager(Screen, ConfigListScreen):
	skin = """
	<screen name="ledManager" position="center,130" size="720,500" title="LED Manager 0.1">
		<widget name="config" position="10,10" size="700,440" scrollbarMode="showOnDemand" />
		<ePixmap pixmap="skin_default/buttons/select_s.png" position="40,450" size="240,44" zPosition="1" alphatest="on" />
		<ePixmap pixmap="skin_default/buttons/select_s.png" position="440,450" size="240,44" zPosition="1" alphatest="on" />
		<widget name="canceltext" position="40,452" zPosition="2" size="240,40" font="Prive2;22" valign="center" halign="center" foregroundColor="red" transparent="1" />
		<widget name="oktext" position="440,452" zPosition="2" size="240,40" font="Prive2;22" valign="center" halign="center" foregroundColor="green" transparent="1" />
	</screen>"""

	def __init__(self, session):
		self.skin = ledManager.skin
		self.session = session
		Screen.__init__(self, session)

		self.nr_leds = SystemInfo.get("NumFrontpanelLEDs", 0)
		selectopts = {"0": "OFF", "1": "ON", "2": "BLINK"}

		self.led0ConfigEntry0 = NoSave(ConfigSelection(selectopts, str(config.plugins.ledManager.led0.value[0])))
		self.led0ConfigEntry1 = NoSave(ConfigSelection(selectopts, str(config.plugins.ledManager.led0.value[1])))
		self.led0ConfigEntry2 = NoSave(ConfigSelection(selectopts, str(config.plugins.ledManager.led0.value[2])))
		self.led0ConfigEntry3 = NoSave(ConfigSelection(selectopts, str(config.plugins.ledManager.led0.value[3])))
		if self.nr_leds > 1:
			self.led1ConfigEntry0 = NoSave(ConfigSelection(selectopts, str(config.plugins.ledManager.led1.value[0])))
			self.led1ConfigEntry1 = NoSave(ConfigSelection(selectopts, str(config.plugins.ledManager.led1.value[1])))
			self.led1ConfigEntry2 = NoSave(ConfigSelection(selectopts, str(config.plugins.ledManager.led1.value[2])))
			self.led1ConfigEntry3 = NoSave(ConfigSelection(selectopts, str(config.plugins.ledManager.led1.value[3])))

		self["oktext"] = Label(_("OK"))
		self["canceltext"] = Label(_("Exit"))

		self.createSetup()
		ConfigListScreen.__init__(self, self.list, session = session)

		self["actions"] = ActionMap(["OkCancelActions", "ColorActions"],
		{
			"cancel": self.cancel,
			"green" : self.green,
			"ok" : self.green,
			"red": self.cancel
		}, -2)

		if self.nr_leds == 0:
			self.onExecBegin.append(self.errorMbox)

	def errorMbox(self):
		info = self.session.open(MessageBox,_("No frontpanel LED's found!"), MessageBox.TYPE_ERROR)
		info.setTitle(_("LED Manager"))
		self.close()

	def createSetup(self):
		self.list = []
		self.list.append(getConfigListEntry(_("LED 0: Box is on"), self.led0ConfigEntry0))
		self.list.append(getConfigListEntry(_("LED 0: Box is on and recording"), self.led0ConfigEntry1))
		self.list.append(getConfigListEntry(_("LED 0: Box is standby"), self.led0ConfigEntry2))
		self.list.append(getConfigListEntry(_("LED 0: Box is standby and recording"), self.led0ConfigEntry3))
		if self.nr_leds > 1:
			self.list.append(getConfigListEntry(_("LED 1: Box is on"), self.led1ConfigEntry0))
			self.list.append(getConfigListEntry(_("LED 1: Box is on and recording"), self.led1ConfigEntry1))
			self.list.append(getConfigListEntry(_("LED 1: Box is standby"), self.led1ConfigEntry2))
			self.list.append(getConfigListEntry(_("LED 1: Box is standby and recording"), self.led1ConfigEntry3))

	def cancel(self):
		self.close()

	def green(self):
		config.plugins.ledManager.led0.value[0] = int(self.led0ConfigEntry0.value)
		config.plugins.ledManager.led0.value[1] = int(self.led0ConfigEntry1.value)
		config.plugins.ledManager.led0.value[2] = int(self.led0ConfigEntry2.value)
		config.plugins.ledManager.led0.value[3] = int(self.led0ConfigEntry3.value)
		config.plugins.ledManager.led0.save()
		if self.nr_leds > 1:
			config.plugins.ledManager.led1.value[0] = int(self.led1ConfigEntry0.value)
			config.plugins.ledManager.led1.value[1] = int(self.led1ConfigEntry1.value)
			config.plugins.ledManager.led1.value[2] = int(self.led1ConfigEntry2.value)
			config.plugins.ledManager.led1.value[3] = int(self.led1ConfigEntry3.value)
			config.plugins.ledManager.led1.save()

		msg = _("Please restart Enigma to apply the new configuration.") + "\n" + _("Do You want restart enigma2 now?")
		box = self.session.openWithCallback(self.restartGUI, MessageBox, msg , MessageBox.TYPE_YESNO, timeout = 10)
		box.setTitle(_('Restart Enigma2'))

	def restartGUI(self, answer):
		if answer is True:
			self.session.open(TryQuitMainloop, 3)
		self.close()

def ledManagerMain(session, **kwargs):
	session.open(ledManager)

def ledManagerSetup(menuid, **kwargs):
	if menuid == "system":
		return [(_("LED Manager..."), ledManagerMain, "ledManager", None)]
	else:
		return []

def Plugins(**kwargs):
	list = [PluginDescriptor(where = PluginDescriptor.WHERE_SESSIONSTART, fnc = autostart)]
	list.append(PluginDescriptor(name="LED Manager", description=_("setup for frontpanel led(s)"), where = [PluginDescriptor.WHERE_MENU], fnc=ledManagerSetup))
	return list
