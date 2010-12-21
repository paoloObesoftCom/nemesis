from Screens.Screen import Screen
from Components.Label import Label
from Components.Pixmap import *
from Components.ServiceEventTracker import ServiceEventTracker
from Tools.HardwareInfo import HardwareInfo
from Components.config import config
from Tools.LoadPixmap import LoadPixmap
from Tools.Directories import fileExists, resolveFilename, SCOPE_SKIN_IMAGE, SCOPE_CURRENT_SKIN
from ServiceReference import ServiceReference
from enigma import eConsoleAppContainer, ePoint, eSize, ePixmap, eTimer, eServiceCenter, eServiceReference, iServiceInformation, iPlayableService, eDVBFrontendParametersSatellite, eDVBFrontendParametersCable, eDVBFrontendParametersTerrestrial, getEnigmaVersionString, eRect, gFont, gRGB
from string import upper
import re

class nemesisEI(Screen):
	__module__ = __name__

	searchPaths = ('/usr/share/enigma2/%s/',
				'/usr/share/enigma2/hd_glass14/%s/',
				'/media/ba/%s/',
				'/media/cf/%s/',
				'/media/sda1/%s/',
				'/media/sda/%s/',
				'/media/usb/%s/')

	def __init__(self, session):
		Screen.__init__(self, session)
		self.allCaids = {
				"06" : "ird", "01" : "seca", "18" : "nagra",
				"05" : "via", "0B" : "conax", "17" : "beta",
				"0D" : "crw", "4A" : "dream", "09" : "nds",
				"4AE0" : "dre", "4AE1" : "dre", "55" : "bul", "26" : "bis"
				}
		self.caRequired = [
				"06", "01",
				"18", "05",
				"0B", "17",
				"0D", "4A",
				"09", "4AE1",
				"4AE0", "55", "26"
				]
		self.dynCAsystem = [
				"Dbeta_Demm", "Dbeta_ecm",
				"Dseca_Demm", "Dseca_ecm",
				"Dird_Demm", "Dird_ecm",
				"Dcrw_Demm", "Dcrw_ecm",
				"Dnagra_Demm", "Dnagra_ecm",
				"Dnds_Demm", "Dnds_ecm",
				"Dvia_Demm", "Dvia_ecm",
				"Dconax_Demm", "Dconax_ecm",
				"Ddream_Demm", "Ddream_ecm",
				"Ddre_Demm", "Ddre_ecm",
				"Dbul_Demm", "Dbul_ecm",
				"Dbis_Demm", "Dbis_ecm",				
				"Dfta" , "Dcrd", "Demu", "Dnet"
				]
		for x in self.dynCAsystem:
			self[x] = Pixmap()
		self["subserv"] = Pixmap()
		self["wide"] = MultiPixmap()
		self["dolby"] = Pixmap()
		self["txt"] = Pixmap()
		self["hd_sd"] = MultiPixmap()
		self["crypt"] = Pixmap()
		self["tuner"] = MultiPixmap()
		self["subserv"].hide()
		self["wide"].hide()
		self["dolby"].hide()
		self["txt"].hide()
		self["hd_sd"].hide()
		self["crypt"].hide()
		self["tuner"].hide()
		self["picProvSat"] = Pixmap()
		self["TP_info"] = Label("")
		self["TP_type"] = Label("")
		self["Video_size"] = Label("")
		self['ecmLineInfo'] = Label()
		self["CAID_info"] = Label("")
		self.tmpAdr = ""
		self.tstprov = "x"
		self.tstsat = ""
		self.pngname = ""
		self.tstca = ""
		self.pngnamesat = ""
		self.typeCAviaCam = ""
		self.typeCAviaCam2 = ""
		self.caidsLive = ""
		self.count = 0
		self.iscalcstart = True
		self.runChkTimer = eTimer()
		self.runChkTimer.timeout.get().append(self.__updCamECM)
		self.emm_timer = eTimer()
		self.emm_timer.timeout.get().append(self.__updateEMM)
		self.__evStart()
		self.__event_tracker = ServiceEventTracker(screen=self, eventmap={iPlayableService.evStart: self.__evStart, iPlayableService.evUpdatedInfo: self.__evUpdatedInfo, iPlayableService.evTunedIn: self.__evTunedIn, iPlayableService.evUpdatedEventInfo: self.__evUpdatedInfo, iPlayableService.evVideoSizeChanged: self.__evUpdatedInfo})
		self.onLayoutFinish.append(self.startloadpixmap)

	def startloadpixmap(self):
		if self.iscalcstart:
			try:
				self.widthDynIco = self["crypt"].instance.size().width()
				self.widthDynIco = self.widthDynIco + 2
				self.posX_DynIco = self["crypt"].instance.position().x()
				self.posY_DynIco = self["crypt"].instance.position().y()
				self.width_D_icons = self["Dbeta_Demm"].instance.size().width()
				self.width_D_icons = self.width_D_icons + 2
				self.posY_DynIco_CA = self["Dbeta_Demm"].instance.position().y()
				self.x_dyn_pos_CA = self["Dbeta_Demm"].instance.position().x()
			except:
				pass
			self.iscalcstart = False

	def __evStart(self):
		if self.emm_timer.isActive():
			self.emm_timer.stop()
		if self.runChkTimer.isActive():
			self.runChkTimer.stop()
		self.count = 0
		self.tmpAdr = ""
		self["Video_size"].setText("--- x ---")
		self["ecmLineInfo"].setText("No info from Cam")
		self["TP_type"].setText("No data")
		self["TP_info"].setText("No tuner data")
		self["CAID_info"].setText("No data")
		self.typeCAviaCam = ""
		self.displayDynClean()

	def __evTunedIn(self):
		service = self.session.nav.getCurrentService()
		info = (service and service.info())
		if info:
			self.setTunerInfo(service)
			self.runChkTimer.start(750)
			self.emm_timer.start(700)

	def __updateEMM(self):
		self.emm_timer.stop()
		service = self.session.nav.getCurrentService()
		info = service and service.info()
		if info is not None:
			self["CAID_info"].setText((self.getCaidInfo()))
			self.caidsLive = info.getInfoObject(iServiceInformation.sCAIDs)
			self.showDyn_EMM_ECM(self.caidsLive)
			
	def __updCamECM(self):
		service = self.session.nav.getCurrentService()
		info = (service and service.info())
		if info:
			serviceinfo = service.info()
			if serviceinfo.getInfo(iServiceInformation.sIsCrypted):
				if (self.count < 4):
					self.count = (self.count + 1)
				else:
					self.runChkTimer.changeInterval(15000)
				self.readEcmInfo()
			else:			
				self["ecmLineInfo"].setText("Free To Air")

	def getCaidInfo(self):
		service = self.session.nav.getCurrentService()
		cainfo = "Caids: "
		if service:
			info = service and service.info()
			if info:
				caids = info.getInfoObject(iServiceInformation.sCAIDs)
				if caids:
					if len(caids) > 0:
						for caid in caids:
							caid = self.int2hex(caid)
							if len(caid) == 3:
								caid = "0%s" % caid
							caid = caid.upper()							
							cainfo += caid
							cainfo += " "
						return cainfo
		return "No CA info available"

	def readEcmInfo(self):
		try:
			tmp = open('/tmp/ecm.info', 'r')
			content = tmp.read()
			tmp.close
		except:
			content = ""
		lines = content.split("\n")
		system=caid=provider=pid=using=adress=hops=share=ecmTime=coding=typecam=provid="..............\n"
		if content != "":
			typcm = False
			for line in lines:
				if line.startswith("=====") and typecam == "..............\n":
					typecam = "Mgcamd or Mbox\n"
					typcm = True
				elif typecam == "..............\n":
					typecam = "CCcam\n"
				elif typecam == "CCcam\n" and line.startswith("reader:"):
					typecam = "OScam\n"
				elif typecam == "CCcam\n" and line.startswith("FROM:"):
					typecam = "Camd3\n"
				if line.startswith("system:") or line.startswith("===="):
					if line.startswith("===="):
						system = str(line.split(' ')[1].split())
						system = system[2:-2] + " ECM" + "\n"
					else:
						system = self.parseEcmInfoLine(line) + "\n"
					if len(system) > 19:
						system = "%s..." % system[:16]
						system = system + "\n"
				if line.startswith("caid:") or line.startswith("====") or line.startswith("CAID "):
					if line.startswith("===="):
						caid = str(self.parseInfoLine(line)).upper()
						idx = caid.index("X")
						caid = caid[idx+1:]
						caid = caid[:4]+"\n"
					elif line.startswith("CAID"):
						caid = str(line.split(' ')[1].split())
						caid = caid.upper()
						caid = caid[2:-2]
						idx = caid.index("X")
						caid = caid[idx+1:]
						caid = caid[:4]+"\n"
					else:
						caid = str(self.parseEcmInfoLine(line)).upper()
						idx = caid.index("X")
						caid = caid[idx+1:]
						if len(caid) == 3:
							caid = "0%s" % caid
						caid = caid + "\n"
					if caid == "4AE1\n" or caid == "4AE0\n":
						dyntmpcaid = caid[:-1]
					else:
						dyntmpcaid = caid[:2]
					if caid.startswith("18"):
						coding = "Nagravision"
					elif caid.startswith("17"):
						coding = "Betacrypt"
					elif caid.startswith("01"):
						coding = "Seca"
					elif caid.startswith("06"):
						coding = "Irdeto"
					elif caid.startswith("05"):
						coding="Viaccess"
					elif caid.startswith("0B"):
						coding="Conax"
					elif caid.startswith("0D"):
						coding="Cryptoworks"
					elif caid.startswith("4A"):
						coding="Dreamcrypt"
					elif caid.startswith("09"):
						coding="Videoguard"
					elif caid.startswith("55"):
						coding="Bulcrypt"
					elif caid.startswith("26"):
						coding="Biss"
					else:
						coding="Unknown"
					if caid == "4AE1\n" or caid == "4AE0\n":
						coding="DRE-crypt"
				if line.startswith("provid:") or line.startswith("CAID ") or line.startswith("SysID"):
					if line.startswith("CAID "):
						provid = str(line.split(' ')[5].split())
						provid = provid[2:-2]
						provid = provid.upper()
						idx = provid.index("X")
						provid = provid[idx+1:] + "\n"
					elif line.startswith("SysID"):
						provid = str(line.split(' ')[1].split())
						provid = provid[2:-2]
						provid = provid.upper()
						provid = provid + "\n"
					else:
						provid = self.parseEcmInfoLine(line)
						provid = provid.upper()
						provid = provid[2:] + "\n"
				elif line.startswith("provider:") or line.startswith("prov: "):
					provider = self.parseEcmInfoLine(line) + "\n"
					if provider.__contains__(","):
						idx = provider.index(",")
						provider = provider[:idx-1] + "\n"
					if provider.__contains__("0X") or provider.__contains__("0x"):
						if provider.__contains__("X"):
							idx = provider.index("X")
						else:
							idx = provider.index("x")
						provider = provider[idx+1:]
					if len(provider) > 19:
						provider = "%s..." % provider[:16]
						provider = provider + "\n"
				if line.startswith("pid:") or line.startswith("====") or line.startswith("CAID "):
					if line.startswith("===="):
						pid = str(line.split(' ')[7].split())
						pid = pid[2:-2]
					elif line.startswith("CAID "):
						pid = str(line.split(' ')[3].split())
						pid = pid[2:-3]
					else:
						pid = self.parseEcmInfoLine(line)
					pid = pid.upper()
					idx = pid.index("X")
					pid = pid[idx+1:] + "\n"
				if line.startswith("using:") or line.startswith("source:") or line.startswith("reader:") or line.startswith("decode:"):
					if line.startswith("source:"):
						using = str(line.split(' ')[1].split())
						using = using[2:-2] + "\n"
					elif line.startswith("decode:"):
						if typcm:
							typecam = "Mgcamd or Mbox\n"
						else:
							typecam = "Gbox\n"
						if line.__contains__("slot") or line.__contains__("Local"):
							using = "Cardreader\n"
						elif line.__contains__("Internal"):
							using = "Emu\n"
							typecam = "Gbox\n"
						else:
							using = "Network\n"
					else:
						using = self.parseEcmInfoLine(line) + "\n"
						if using == "sci\n":
							using = "Cardreader\n"
					if line.__contains__("emu") or line.__contains__("Internal"):
						self.typeCAviaCam2 = "emu" 
				if line.startswith("address:") or line.startswith("source:") or line.startswith("from:") or line.startswith("FROM:") or line.startswith("decode:"):
					if line.startswith("source: net"):
						idex = line.index("(")
						adress = line[idex:]
						adress = adress.replace("\n", "")
						adress = str(adress.split(' ')[2].split())
						adress = adress[2:-3] + "\n"
						if adress.__contains__("127.0.0.1"):
							adress = "Local card\n"
					elif line.startswith("source: emu") or line.startswith("decode:"):
						adress = "..............\n"
					else:
						adress = self.parseEcmInfoLine(line) + "\n"
						if adress == "/dev/sci0\n":
							if HardwareInfo().get_device_name() == 'dm800se':
								adress = "Upper slot \n"
							else:
								adress = "Lower slot \n"
						elif adress == "/dev/sci1\n":
							if HardwareInfo().get_device_name() == 'dm800se':
								adress = "Lower slot \n"
							else:
								adress = "Upper slot \n"
						elif adress.__contains__("local"):
							adress = "Local slot\n"
						elif coding == "Unknown":
							adress = "..............\n"
					if len(adress) > 19:
						adress = "%s..." % adress[:16]
						adress = adress + "\n"
					if self.tmpAdr != adress:
						self.tmpAdr = adress
						if adress.__contains__("slot") or adress.__contains__("local") or (line.startswith("decode:") and (line.__contains__("slot") or line.__contains__("Local"))):
							self.typeCAviaCam2 = "crd"
						elif line.__contains__("emu"):
							self.typeCAviaCam2 = "emu"
						elif (adress != "..............\n" and using != "unsupported CAs\n") or (line.startswith("decode:") and line.__contains__("Network")):
							self.typeCAviaCam2 = "net"
				elif line.startswith("hops:"):
					hops = self.parseEcmInfoLine(line) + "\n"
				if line.startswith("share:") or line.startswith("source:"):
					if line.startswith("source: net"):
						idex = line.index("(")
						share = line[idex:]
						share = share.replace("\n", "")
						share = str(share.split(' ')[0].split())
						share = share[3:-2] + "\n"
					elif line.startswith("source: emu"):
						share = "..............\n"
					else:
						share = self.parseEcmInfoLine(line) + "\n"
					if len(share) > 19:
						share = "%s..." % share[:16]
						share = share + "\n"
				elif line.startswith("ecm time:") or line.startswith("Time:(") or line.startswith("Time: (") or line.startswith("1") or line.startswith("2") or line.startswith("3") or line.startswith("4") or line.startswith("5") or line.startswith("6") or line.startswith("7") or line.startswith("8") or line.startswith("9"):
					if line.startswith("ecm time:"):
						ecmTime = self.parseEcmInfoLine(line) + "\n"
					elif line.startswith("Time:(") or line.startswith("Time: ("):
						idex = line.index("(")
						ecmTime = line[idex+1:-2] + "\n"
					else:
						ecmTime = str(line.split(' ')[0].split())
						ecmTime = "0.%s" % ecmTime[2:-2]
						ecmTime = ecmTime + "\n"
			if provider == "Unknown\n":
				provider = "..............\n"
			tmpline = ""
			if using == "unsupported CAs\n":
				tmpline = "Unsuported CA found, no correct ECM info !!!"
			else:
				if typecam != "..............\n":
					tmpline = "Cam: " + typecam[:-1]
				if caid != "..............\n":
					tmpline = tmpline + ", Caid: " + caid[:-1]
				if config.nemesis.shownetdet.value:
					if adress != "..............\n":
						tmpline = tmpline + ", Source: " + adress[:-1]
					if using != "..............\n" and adress == "..............\n":
						tmpline = tmpline + ", Source: " + using[:-1]
					if using != "..............\n" and typecam == "OScam\n":
						tmpline = tmpline + ", Using: " + using[:-1]
				if provid != "..............\n" and typecam == "Gbox\n":
					tmpline = tmpline + ", ProvId: " + provid[:-1]
				if provider != "..............\n" and typecam == "Gbox\n":
					tmpline = tmpline + ", Provider: " + provider[:-1]
				if pid != "..............\n":
					tmpline = tmpline + ", Pid: " + pid[:-1]
				if hops != "..............\n":
					tmpline = tmpline + ", Hops: " + hops[:-1]
				if ecmTime != "..............\n":
					tmpline = tmpline + ", Time: " + ecmTime[:-1]
			self["ecmLineInfo"].setText(tmpline)   						
			if (self.tstca != coding or self.typeCAviaCam != self.typeCAviaCam2) and (using != "unsupported CAs\n"):
				self.tstca = coding 
				self.typeCAviaCam = self.typeCAviaCam2
				self.displayDynClean()
				self.showDyn_EMM_ECM(self.caidsLive, dyntmpcaid, self.typeCAviaCam2)
			return
		self.displayDynClean()
		self.showDyn_EMM_ECM(self.caidsLive)
		self["ecmLineInfo"].setText("No info from Cam")

	def __evUpdatedInfo(self):
		service = self.session.nav.getCurrentService()
		info = (service and service.info())
		if info is not None:
			tmp = self.getServiceInfoString(info, iServiceInformation.sVideoWidth)
			tmp2 = self.getServiceInfoString(info, iServiceInformation.sVideoHeight)
			self["Video_size"].setText((tmp + " x " + tmp2))
			x_dyn_pos = self.posX_DynIco
			feinfo = (service and service.frontendInfo())
			if (feinfo is not None):
				frontendData = (feinfo and feinfo.getAll(True))
				if (frontendData is not None):
					num = frontendData.get("tuner_number")
					if num == 0 or num == 1 or num == 2 or num == 3:
						self["tuner"].setPixmapNum(num)
						self["tuner"].show()
						x_dyn_pos = x_dyn_pos - self.widthDynIco
					else:
						self["tuner"].hide()
				else:
					self["tuner"].hide()
			else:
				self["tuner"].hide()
			if info.getInfo(iServiceInformation.sIsCrypted):
				self["crypt"].move(ePoint(x_dyn_pos, self.posY_DynIco))
				self["crypt"].show()
				x_dyn_pos = x_dyn_pos - self.widthDynIco
			else:
				self["crypt"].hide()
			if tmp != "N/A":
				if int(tmp) < 721:
					self["hd_sd"].setPixmapNum(0)
				else:
					self["hd_sd"].setPixmapNum(1)
				self["hd_sd"].move(ePoint(x_dyn_pos, self.posY_DynIco))
				self["hd_sd"].show()
				x_dyn_pos = x_dyn_pos - self.widthDynIco
			else:
				self["hd_sd"].hide()
			tpid = info.getInfo(iServiceInformation.sTXTPID)
			if tpid != -1:
				self["txt"].move(ePoint(x_dyn_pos, self.posY_DynIco))
				self["txt"].show()
				x_dyn_pos = x_dyn_pos - self.widthDynIco
			else:
				self["txt"].hide()
			tmp = "not ok"
			audio = service.audioTracks()
			if audio:
				n = audio.getNumberOfTracks()
				idx = 0
				while idx < n:
					i = audio.getTrackInfo(idx)
					description = i.getDescription();
					if "AC3" in description or "DTS" in description:
						tmp = "ok"
					idx += 1
			if tmp == "ok":
				self["dolby"].move(ePoint(x_dyn_pos, self.posY_DynIco))
				self["dolby"].show()
				x_dyn_pos = x_dyn_pos - self.widthDynIco
			else:
				self["dolby"].hide()
			tmp = self.getServiceInfoString(info, iServiceInformation.sAspect)
			if tmp != "N/A":
				if (info.getInfo(iServiceInformation.sAspect) in (3, 4, 7, 8, 0xB, 0xC, 0xF, 0x10)):
					self["wide"].setPixmapNum(1)
				else:
					self["wide"].setPixmapNum(0)
				self["wide"].move(ePoint(x_dyn_pos, self.posY_DynIco))
				self["wide"].show()
				x_dyn_pos = x_dyn_pos - self.widthDynIco
			else:
				self["wide"].hide()
			subservices = service.subServices()
			if subservices and subservices.getNumberOfSubservices() > 0:
				self["subserv"].move(ePoint(x_dyn_pos, self.posY_DynIco))
				self["subserv"].show()
			else:
				self["subserv"].hide()

	def getServiceInfoString(self, info, what, convert = lambda x: "%d" % x):
		v = info.getInfo(what)
		if v == -1:
			return "N/A"
		if v == -2:
			return info.getInfoString(what)
		return convert(v)

	def parseEcmInfoLine(self, line):
		if line.__contains__(":"):
			idx = line.index(":")
			line = line[idx+1:]
			line = line.replace("\n", "")
			while line.startswith(" "):
				line = line[1:]
			while line.endswith(" "):
				line = line[:-1]
			return line
		else:
			return ""

	def parseInfoLine(self, line):
		if line.__contains__("CaID"):
			idx = line.index("D")
			line = line[idx+1:]
			line = line.replace("\n", "")
			while line.startswith(" "):
				line = line[1:]
			while line.endswith(" "):
				line = line[:-1]
			return line
		else:
			return ""
		
	def int2hex(self, int):
		return "%x" % int

	def getServiceInfoValue(self, info, what, ref=None):
		v = ref and info.getInfo(ref, what) or info.getInfo(what)
		if v != iServiceInformation.resIsString:
			return "N/A"
		return ref and info.getInfoString(ref, what) or info.getInfoString(what)
		
	def setTunerInfo(self, service):
		pngname = ""
		ref = None
		info = service and service.info()
		provname = self.getServiceInfoValue(info, iServiceInformation.sProvider, ref)
		if self.tstprov != provname:
			self.tstprov = provname 
			if provname.startswith("T-K"):
				sname = "T-KABEL"
			elif provname == "T-Systems/MTI":
				sname = "T-SYSTEMS"
			else:
				sname = provname.upper()
			self.path = "piconProv"
			pngname = self.findPicon(sname)
			if pngname == "":
				pngname = self.findPicon("picon_default")
				if pngname == "":
					tmp = resolveFilename(SCOPE_CURRENT_SKIN, "picon_default.png")
					if fileExists(tmp):
						pngname = tmp
					else:
						pngname = resolveFilename(SCOPE_SKIN_IMAGE, "skin_default/picon_default.png")
			if self.pngname != pngname:
				self.pngname = pngname
		tunerinfo = tptype = ""
		feinfo = (service and service.frontendInfo())
		if (feinfo is not None):
			frontendData = (feinfo and feinfo.getAll(True))
			if (frontendData is not None):
				frequency = (str(int(frontendData.get("frequency", 0) / 1000)) + " MHz")
				symbolrate = str(int(frontendData.get("symbol_rate", 0) / 1000))
				if frontendData.get("tuner_type") == "DVB-S" or frontendData.get("tuner_type") == "DVB-S2":
					try:	
						orb = {
										3590:'Thor/Intelsat (1.0W)',3560:'Amos (4.0W)',3550:'Atlantic Bird (5.0W)',3530:'Nilesat/Atlantic Bird (7.0W)',
										3520:'Atlantic Bird (8.0W)',3475:'Atlantic Bird (12.5W)',3460:'Express (14.0W)', 3450:'Telstar (15.0W)',
										3420:'Intelsat (18.0W)',3380:'Nss (22.0W)',3355:'Intelsat (24.5W)', 3325:'Intelsat (27.5W)',3300:'Hispasat (30.0W)',
										3285:'Intelsat (31.5W)',3170:'Intelsat (43.0W)',3150:'Intelsat (45.0W)',3070:'Intelsat (53.0W)',3045:'Intelsat (55.5W)',
										3020:'Intelsat 9 (58.0W)',2990:'Amazonas (61.0W)',2900:'Star One (70.0W)',2880:'AMC 6 (72.0W)',2875:'Echostar 6 (72.7W)',
										2860:'Horizons (74.0W)',2810:'AMC5 (79.0W)',2780:'NIMIQ 4 (82.0W)',2690:'NIMIQ 1 (91.0W)',3592:'Thor/Intelsat (0.8W)',
										2985:'Echostar 3,12 (61.5W)',2830:'Echostar 8 (77.0W)',2630:'Galaxy 19 (97.0W)',2500:'Echostar 10,11 (110.0W)',
										2502:'DirectTV 5 (110.0W)',2410:'Echostar 7 Anik F3 (119.0W)',2391:'Galaxy 23 (121.0W)',2390:'Echostar 9 (121.0W)',
										2412:'DirectTV 7S (119.0W)',2310:'Galaxy 27 (129.0W)',2311:'Ciel 2 (129.0W)',2120:'Echostar 2 (148.0W)',
										1100:'BSat 1A,2A (110.0E)',1101:'N-Sat 110 (110.0E)',1131:'KoreaSat 5 (113.0E)',1440:'SuperBird 7,C2 (144.0E)',
										1006:'AsiaSat 2 (100.5E)',1030:'Express A2 (103.0E)',1056:'Asiasat 3S (105.5E)',1082:'NSS 11 (108.2E)',
										881:'ST1 (88.0E)',900:'Yamal 201 (90.0E)',917:'Mesat (91.5E)',950:'Insat 4B (95.0E)',951:'NSS 6 (95.0E)',
										765:'Telestar (76.5E)',785:'ThaiCom 5 (78.5E)',800:'Express (80.0E)',830:'Insat 4A (83.0E)',850:'Intelsat 709 (85.2E)',
										750:'Abs (75.0E)',720:'Intelsat (72.0E)',705:'Eutelsat W5 (70.5E)',685:'Intelsat (68.5E)',620:'Intelsat 902 (62.0E)',
										600:'Intelsat 904 (60.0E)',570:'Nss (57.0E)',530:'Express AM22 (53.0E)',480:'Eutelsat 2F2 (48.0E)',450:'Intelsat (45.0E)',
										420:'Turksat 2A (42.0E)',400:'Express AM1 (40.0E)',390:'Hellas Sat 2 (39.0E)',380:'Paksat 1 (38.0E)',
										360:'Eutelsat Sesat (36.0E)',335:'Astra 1M (33.5E)',330:'Eurobird 3 (33.0E)',328:'Galaxy 11 (32.8E)',
										315:'Astra 5A (31.5E)',310:'Turksat (31.0E)',305:'Arabsat (30.5E)',285:'Eurobird 1 (28.5E)',
										284:'Eurobird/Astra (28.2E)',282:'Eurobird/Astra (28.2E)',1220:'AsiaSat (122.0E)',1380:'Telstar 18 (138.0E)',
										260:'Badr 3/4 (26.0E)',255:'Eurobird 2 (25.5E)',232:'Astra 3A/3B (23.5E)',235:'Astra 3A/3B (23.5E)',215:'Eutelsat (21.5E)',
										216:'Eutelsat W6 (21.6E)',210:'AfriStar 1 (21.0E)',192:'Astra 1F (19.2E)',160:'Eutelsat W2 (16.0E)',
										130:'Hot Bird 6,7A,8 (13.0E)',100:'Eutelsat W1 (10.0E)',90:'Eurobird 9 (9.0E)',70:'Eutelsat W3A (7.0E)',
										50:'Sirius 4 (5.0E)',48:'Astra 4A (4.8E)',30:'Telecom 2 (3.0E)'
									}[frontendData.get("orbital_position", "None")]
					except:
					    orb = 'Unsupported SAT: %s' % str([frontendData.get("orbital_position", "None")])
					try:	
						orbPic = {
										3590:'10W',3560:'40W',3550:'50W',3530:'70W',
										3520:'80W',3475:'125W',3460:'140)', 3450:'150W',
										3420:'180W',3380:'220W',3355:'245W', 3325:'275W',3300:'300W',
										3285:'315W',3170:'430W',3150:'450W',3070:'530W',3045:'555W',
										3020:'580W',2990:'610W',2900:'700W',2880:'720W',2875:'727W',
										2860:'740W',2810:'790W',2780:'820W',2690:'910W',3592:'08W',
										2985:'615W',2830:'770W',2630:'970W',2500:'1100W',
										2502:'1100W',2410:'1190W',2391:'1210W',2390:'1210W',
										2412:'1190W',2310:'1290W',2311:'1290W',2120:'1480W',
										1100:'1100E',1101:'1100E',1131:'1130E',1440:'1440E',
										1006:'1005E',1030:'1030E',1056:'1055E',1082:'1082E',
										881:'880E',900:'900E',917:'915E',950:'950E',951:'950E',
										765:'765E',785:'785E',800:'800E',830:'830E',850:'852E',
										750:'750E',720:'720E',705:'705E',685:'685E',620:'620E',
										600:'600E',570:'570E',530:'530E',480:'480E',450:'450E',
										420:'420E',400:'400E',390:'390E',380:'380E',
										360:'360E',335:'335E',330:'330E',328:'328E',
										315:'315E',310:'310E',305:'305E',285:'285E',
										284:'282E',282:'282E',1220:'1220E',1380:'1380E',
										260:'260E',255:'255E',235:'235E',232:'235E',215:'215E',
										216:'216E',210:'210E',192:'192E',160:'160E',
										130:'130E',100:'100E',90:'90E',70:'70E',
										50:'50E',48:'48E',30:'30E'
									}[frontendData.get("orbital_position", "None")]
					except:
						orbPic = 'Unsupported SAT: %s' % str([frontendData.get("orbital_position", "None")])
					pngname = ""
					if self.tstsat != orbPic:
						self.tstsat = orbPic
						self.path = "piconSat"
						pngname = self.findPicon(orbPic)
						if pngname == "":
							pngname = self.findPicon("picon_default")
							if pngname == "":
								tmp = resolveFilename(SCOPE_CURRENT_SKIN, "picon_default.png")
								if fileExists(tmp):
									pngname = tmp
								else:
									pngname = resolveFilename(SCOPE_SKIN_IMAGE, "skin_default/picon_default.png")
						if pngname != "":
							self.pngnamesat = pngname
					pol = {
									eDVBFrontendParametersSatellite.Polarisation_Horizontal : "H",
									eDVBFrontendParametersSatellite.Polarisation_Vertical : "V",
									eDVBFrontendParametersSatellite.Polarisation_CircularLeft : "CL",
									eDVBFrontendParametersSatellite.Polarisation_CircularRight : "CR"
								}[frontendData.get("polarization", eDVBFrontendParametersSatellite.Polarisation_Horizontal)]		
					fec = {
									eDVBFrontendParametersSatellite.FEC_None : "None",
									eDVBFrontendParametersSatellite.FEC_Auto : "Auto",
									eDVBFrontendParametersSatellite.FEC_1_2 : "1/2",
									eDVBFrontendParametersSatellite.FEC_2_3 : "2/3",
									eDVBFrontendParametersSatellite.FEC_3_4 : "3/4",
									eDVBFrontendParametersSatellite.FEC_5_6 : "5/6",
									eDVBFrontendParametersSatellite.FEC_7_8 : "7/8",
									eDVBFrontendParametersSatellite.FEC_3_5 : "3/5",
									eDVBFrontendParametersSatellite.FEC_4_5 : "4/5",
									eDVBFrontendParametersSatellite.FEC_8_9 : "8/9",
									eDVBFrontendParametersSatellite.FEC_9_10 : "9/10"
								}[frontendData.get("fec_inner", eDVBFrontendParametersSatellite.FEC_Auto)]	
					system = {
									eDVBFrontendParametersSatellite.System_DVB_S : "DVB-S",
									eDVBFrontendParametersSatellite.System_DVB_S2 : "DVB-S2",
								}[frontendData.get("system", eDVBFrontendParametersSatellite.System_DVB_S)]	
					modulation = {
									eDVBFrontendParametersSatellite.Modulation_Auto : "Auto",
									eDVBFrontendParametersSatellite.Modulation_QPSK : "QPSK",
									eDVBFrontendParametersSatellite.Modulation_8PSK : "8PSK",
									eDVBFrontendParametersSatellite.Modulation_QAM16 : "QAM16",
								}[frontendData.get("modulation", eDVBFrontendParametersSatellite.Modulation_Auto)]	
					tunerinfo = (frequency + "  " + pol + "  " + fec + "  " + symbolrate + "  " + orb)
					tptype = system + "  " + modulation
					self["TP_type"].setText(tptype)
					if self.pngnamesat != "" and self.pngname != "":
						self.runanim(self.pngname, self.pngnamesat)
				elif (frontendData.get("tuner_type") == "DVB-C"):
					fec = {
									eDVBFrontendParametersCable.FEC_None : "None",
									eDVBFrontendParametersCable.FEC_Auto : "Auto",
									eDVBFrontendParametersCable.FEC_1_2 : "1/2",
									eDVBFrontendParametersCable.FEC_2_3 : "2/3",
									eDVBFrontendParametersCable.FEC_3_4 : "3/4",
									eDVBFrontendParametersCable.FEC_5_6 : "5/6",
									eDVBFrontendParametersCable.FEC_7_8 : "7/8",
									eDVBFrontendParametersCable.FEC_8_9 : "8/9"
								}[frontendData.get("fec_inner", eDVBFrontendParametersCable.FEC_Auto)]
					tunerinfo = (frequency + "  " + fec + "  " + symbolrate)
					tptype = "DVB-C"
					self["TP_type"].setText(tptype)
					if self.tstsat != "picon_cable":
						self.tstsat = "picon_cable"
						self.path = "piconSat"
						pngname = self.findPicon("picon_cable")
						if pngname == "":
							pngname = self.findPicon("picon_default")
							if pngname == "":
								tmp = resolveFilename(SCOPE_CURRENT_SKIN, "picon_default.png")
								if fileExists(tmp):
									pngname = tmp
								else:
									pngname = resolveFilename(SCOPE_SKIN_IMAGE, "skin_default/picon_default.png")
						if pngname != "":
							self.pngnamesat = pngname
							self.runanim(self.pngname, self.pngnamesat)
				elif (frontendData.get("tuner_type") == "DVB-T"):
					pol = {
									eDVBFrontendParametersTerrestrial.Modulation_Auto : "Auto",
									eDVBFrontendParametersTerrestrial.Modulation_QPSK : "QPSK",
									eDVBFrontendParametersTerrestrial.Modulation_QAM16 : "QAM16",
									eDVBFrontendParametersTerrestrial.Modulation_QAM64 : "QAM64"
								}[frontendData.get("constellation", eDVBFrontendParametersTerrestrial.Modulation_Auto)]
					fec = {
									eDVBFrontendParametersTerrestrial.FEC_Auto : "Auto",
									eDVBFrontendParametersTerrestrial.FEC_1_2 : "1/2",
									eDVBFrontendParametersTerrestrial.FEC_2_3 : "2/3",
									eDVBFrontendParametersTerrestrial.FEC_3_4 : "3/4",
									eDVBFrontendParametersTerrestrial.FEC_5_6 : "5/6",
									eDVBFrontendParametersTerrestrial.FEC_7_8 : "7/8"
								}[frontendData.get("code_rate_lp", eDVBFrontendParametersTerrestrial.FEC_Auto)]
					tunerinfo = (frequency + "  " + symbolrate + "  " + fec)	
					if self.tstsat != "picon_trs":
						self.tstsat = "picon_trs"
						self.path = "piconSat"
						pngname = self.findPicon("picon_trs")
						if pngname == "":
							pngname = self.findPicon("picon_default")
							if pngname == "":
								tmp = resolveFilename(SCOPE_CURRENT_SKIN, "picon_default.png")
								if fileExists(tmp):
									pngname = tmp
								else:
									pngname = resolveFilename(SCOPE_SKIN_IMAGE, "skin_default/picon_default.png")
						if pngname != "":
							self.pngnamesat = pngname
							self.runanim(self.pngname, self.pngnamesat)
					tptype = "DVB-T  " + pol
					self["TP_type"].setText(tptype)
				else:
					tunerinfo = (frequency + "  " + symbolrate)	
					tptype = "No detected"
					self["TP_type"].setText(tptype)
					if self.tstsat != "picon_trs":
						self.tstsat = "picon_trs"
						self.path = "piconSat"
						pngname = self.findPicon("picon_trs")
						if pngname == "":
							pngname = self.findPicon("picon_default")
							if pngname == "":
								tmp = resolveFilename(SCOPE_CURRENT_SKIN, "picon_default.png")
								if fileExists(tmp):
									pngname = tmp
								else:
									pngname = resolveFilename(SCOPE_SKIN_IMAGE, "skin_default/picon_default.png")
						if pngname != "":
							self.pngnamesat = pngname
							self.runanim(self.pngname, self.pngnamesat)
				self["TP_info"].setText(tunerinfo)
				return
		self["TP_type"].setText("No data")
		self["TP_info"].setText("No tuner data")

	def runanim(self, pic1, pic2):
		self.slide = 2
		self.steps = 4
		self.pics = []
		self.pics.append(LoadPixmap(pic2))
		self.pics.append(LoadPixmap(pic1))
		self.timerpics = eTimer()
		self.timerpics.callback.append(self.timerpicsEvent)
		self.timerpics.start(100, True)

	def timerpicsEvent(self):
		if self.steps != 0:
			self.timerpics.stop()
			self["picProvSat"].instance.setPixmap(self.pics[self.slide - 1])
			self.steps = self.steps-1
			self.slide = self.slide-1
			if self.slide == 0:
				self.slide =	2		
			self.timerpics.start(2000, True)
		else:
			self.timerpics.stop()
			self["picProvSat"].instance.setPixmapFromFile(self.pngname)

	def findPicon(self, piconName):
		if fileExists("/etc/piconPaths.conf"):
			try:
				f = open("/etc/piconPaths.conf", "r")
				content = f.read()
				f.close()
			except:
				content = ""	
			contentInfo = content.split("\n")
			if content != "":
				for line in contentInfo:
					if line.startswith(self.path):
						if line.__contains__("="):
							stmp = str(line.split('=')[1].split())
							pngname = stmp[2:-2] + self.path + "/" + piconName + ".png"
							if fileExists(pngname):
								return pngname
		for path in self.searchPaths:
			pngname = (path % self.path) + piconName + ".png"
			if fileExists(pngname):
				return pngname
		return ""       
	
	def showDyn_EMM_ECM(self, caids, ecm="xxx", typeECM="xxx"):
		ecm_founded = False
		caRequiredstatus = {}
		for x in self.caRequired:
			caRequiredstatus[x] = True
		x_dyn_pos_CA = self.x_dyn_pos_CA
		if caids:
			if len(caids) > 0:
				if self.allCaids.has_key(ecm):
					ecm_founded = True
				for caid in caids:
					caid = self.int2hex(caid)
					if len(caid) == 3:
						caid = "0%s" % caid
					caid = caid.upper()
					if caid != "4AE1" or caid == "4AE0": 
						caid = caid[:2]
					if self.allCaids.has_key(caid) and caRequiredstatus.get(caid):
						if caid == ecm:
							pass
						else:
							system = self.allCaids.get(caid)
							try:
								self["D%s_Demm" % system].move(ePoint(x_dyn_pos_CA, self.posY_DynIco_CA))
								self["D%s_Demm" % system].show()
								x_dyn_pos_CA = x_dyn_pos_CA - self.width_D_icons
								caRequiredstatus[caid] = False
							except:
								pass
				if ecm_founded or ecm == "26":
					system = self.allCaids.get(ecm)
					try:
						self["D%s_ecm" % system].move(ePoint(x_dyn_pos_CA, self.posY_DynIco_CA))
						self["D%s_ecm" % system].show()
						x_dyn_pos_CA = x_dyn_pos_CA - self.width_D_icons
					except:
						pass
		else:
			typeECM = "fta"
		if typeECM != "xxx":
			try:
				self["D%s" % typeECM].move(ePoint(x_dyn_pos_CA, self.posY_DynIco_CA))
				self["D%s" % typeECM].show()
			except:
				pass
						
	def displayDynClean(self):
		for x in self.dynCAsystem:
			try:
				self[x].hide()
			except:
				pass


			
			
