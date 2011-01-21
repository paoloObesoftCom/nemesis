# -*- coding: utf-8 -*-
from Components.Converter.Converter import Converter
from enigma import iServiceInformation, iPlayableService, iPlayableServicePtr, eServiceCenter, eServiceReference
from Components.Element import cached

class ServiceSat(Converter, object):
	SAT = 0
	NUMBER = 0
	
	def __init__(self, type):
		Converter.__init__(self, type)
		self.list = []
		if type == "Sat":
			self.type = self.SAT
		if type == "Number":
			self.type = self.NUMBER

	@cached
	def getText(self):
		service = self.source.service
		frontendInfo = service.frontendInfo()
		frontendData = frontendInfo and frontendInfo.getAll(True)
		if frontendData is not None:
			tuner_type = frontendData.get("tuner_type", "None")
			if tuner_type == "DVB-S" or tuner_type == "DVB-S2":
				if self.type == self.SAT:
					name = {
					3590:"10W",3560:"40W",3550:"50W",3530:"70W",
					3520:"80W",3475:"125W",3460:"140)", 3450:"150W",
					3420:"180W",3380:"220W",3355:"245W", 3325:"275W",3300:"300W",
					3285:"315W",3170:"430W",3150:"450W",3070:"530W",3045:"555W",
					3020:"580W",2990:"610W",2900:"700W",2880:"720W",2875:"727W",
					2860:"740W",2810:"790W",2780:"820W",2690:"910W",3592:"08W",
					2985:"615W",2830:"770W",2630:"970W",2500:"1100W",
					2502:"1100W",2410:"1190W",2391:"1210W",2390:"1210W",
					2412:"1190W",2310:"1290W",2311:"1290W",2120:"1480W",
					1100:"1100E",1101:"1100E",1131:"1130E",1440:"1440E",
					1006:"1005E",1030:"1030E",1056:"1055E",1082:"1082E",
					881:"880E",900:"900E",917:"915E",950:"950E",951:"950E",
					765:"765E",785:"785E",800:"800E",830:"830E",850:"852E",
					750:"750E",720:"720E",705:"705E",685:"685E",620:"620E",
					600:"600E",570:"570E",530:"530E",480:"480E",450:"450E",
					420:"420E",400:"400E",390:"390E",380:"380E",
					360:"360E",335:"335E",330:"330E",328:"328E",
					315:"315E",310:"310E",305:"305E",285:"285E",
					284:"282E",282:"282E",1220:"1220E",1380:"1380E",
					260:"260E",255:"255E",235:"235E",232:"235E",215:"215E",
					216:"216E",210:"210E",192:"192E",160:"160E",
					130:"130E",100:"100E",90:"90E",70:"70E",
					50:"50E",48:"48E",30:"30E"
					}[frontendData.get("orbital_position", "None")]
					return name
				if self.type == self.NUMBER:
					return frontendData.get("orbital_position", "None")
			elif tuner_type == "DVB-C":
				return "picon_cable"
			elif tuner_type == "DVB-T":
				return "picon_trs"
			else:
				return ""
		return ""

	text = property(getText)
