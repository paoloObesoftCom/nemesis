from Screens.Screen import Screen
from Components.config import *
from Components.Label import Label
from Components.ActionMap import ActionMap
from Components.ConfigList import ConfigListScreen, ConfigList
from Plugins.Plugin import PluginDescriptor

config.plugins.FadeSet = ConfigSubsection()
config.plugins.FadeSet.fadeIn = ConfigYesNo(default = True) # fade in enable
config.plugins.FadeSet.fadeOut = ConfigYesNo(default = True) # fade out enable
config.plugins.FadeSet.fadeInOnZap = ConfigYesNo(default = False) # fade in enable on Zap
config.plugins.FadeSet.timeout = ConfigInteger(5, (1,10)) # fade in/out value

def startSetup(menuid, **kwargs):
	if menuid != "system":
		return [ ]
	return [(_("Setup Fading"), main, None, None)]

def Plugins(path, **kwargs):
	return [ PluginDescriptor(name=_("Setup Fading"), description=_("Setup Fading"), where = PluginDescriptor.WHERE_MENU, fnc=startSetup) ]

def main(session,**kwargs):
	session.open(FadeSetScreenSetup)

class FadeSetScreenSetup(Screen, ConfigListScreen):

	skin = """
	<screen name="FadeSetSetup" position="center,center" size="620,300" title="FadeSet" backgroundColor="#31000000" >
		<widget name="config" position="20,20" size="580,280" zPosition="1" transparent="0" backgroundColor="#31000000" scrollbarMode="showOnDemand" />
		<widget name="key_green" position="330,260" zPosition="3" size="270,40" valign="center" halign="center" font="Regular;22" transparent="1" foregroundColor="green" />
		<widget name="key_red" position="20,260" zPosition="3" size="270,40" valign="center" halign="center" font="Regular;22" transparent="1" foregroundColor="red" />
	</screen>"""

	def __init__(self, session):
		Screen.__init__(self, session)
		self["key_green"] = Label(_("Save"))
		self["key_red"] = Label(_("Cancel"))
		self["actions"] = ActionMap(["SetupActions", "ColorActions"],
		{
			"green": self.save,
			"ok": self.save,
			"red": self.exit,
			"cancel": self.exit
		}, -2)

		FadeSetConfigList = []
		FadeSetConfigList.append(getConfigListEntry(_(" Enable fade in of infobar:"), config.plugins.FadeSet.fadeIn))
		FadeSetConfigList.append(getConfigListEntry(_(" Enable fade out of infobar:"), config.plugins.FadeSet.fadeOut))
		FadeSetConfigList.append(getConfigListEntry(_(" Enable fade in of infobar on Zap:"), config.plugins.FadeSet.fadeInOnZap))
		FadeSetConfigList.append(getConfigListEntry(_(" Speed for fade in/out of infobar (1-10):"), config.plugins.FadeSet.timeout))
		ConfigListScreen.__init__(self, FadeSetConfigList, session)
        
	def save(self):
		for x in self["config"].list:
			x[1].save()
		configfile.save()
		self.close()     

	def exit(self):
		for x in self["config"].list:
			x[1].cancel()
		self.close()    		
