from Screen import Screen
from Screens.DefaultWizard import DefaultWizard
from ServiceScan import ServiceScan
from Components.config import config, ConfigSubsection, ConfigSelection, \
	ConfigYesNo, ConfigInteger, getConfigListEntry, ConfigSlider, ConfigEnableDisable
from Components.ActionMap import NumberActionMap, ActionMap
from Components.ConfigList import ConfigListScreen
from Components.NimManager import nimmanager, getConfigSatlist
from Components.Label import Label
from Components.Pixmap import MultiPixmap
from Tools.Directories import resolveFilename, SCOPE_DEFAULTPARTITIONMOUNTDIR, SCOPE_DEFAULTDIR, SCOPE_DEFAULTPARTITION
from Tools.HardwareInfo import HardwareInfo
from Tools.Transponder import ConvertToHumanReadable
from Screens.MessageBox import MessageBox
from enigma import eTimer, eDVBFrontendParametersSatellite, eComponentScan, \
	eDVBSatelliteEquipmentControl as secClass, eDVBFrontendParametersTerrestrial, \
	eDVBFrontendParametersCable, eConsoleAppContainer, eDVBResourceManager, \
	eDVBFrontendParameters
from Nemesis.nemesisTool import isE232
_isE232 = isE232()

def buildTerTransponder(frequency,
		inversion=2, bandwidth = 3, fechigh = 6, feclow = 6,
		modulation = 2, transmission = 2, guard = 4,
		hierarchy = 4):
#	print "freq", frequency, "inv", inversion, "bw", bandwidth, "fech", fechigh, "fecl", feclow, "mod", modulation, "tm", transmission, "guard", guard, "hierarchy", hierarchy
	parm = eDVBFrontendParametersTerrestrial()
	parm.frequency = frequency
	parm.inversion = inversion
	parm.bandwidth = bandwidth
	parm.code_rate_HP = fechigh
	parm.code_rate_LP = feclow
	parm.modulation = modulation
	parm.transmission_mode = transmission
	parm.guard_interval = guard
	parm.hierarchy = hierarchy
	return parm

def getInitialTransponderList(tlist, pos):
	list = nimmanager.getTransponders(pos)
	for x in list:
		if x[0] == 0:		#SAT
			parm = eDVBFrontendParametersSatellite()
			parm.frequency = x[1]
			parm.symbol_rate = x[2]
			parm.polarisation = x[3]
			parm.fec = x[4]
			parm.inversion = x[7]
			parm.orbital_position = pos
			parm.system = x[5]
			parm.modulation = x[6]
			parm.rolloff = x[8]
			parm.pilot = x[9]
			tlist.append(parm)

def getInitialCableTransponderList(tlist, nim):
	list = nimmanager.getTranspondersCable(nim)
	for x in list:
		if x[0] == 1: #CABLE
			parm = eDVBFrontendParametersCable()
			parm.frequency = x[1]
			parm.symbol_rate = x[2]
			parm.modulation = x[3]
			parm.fec_inner = x[4]
			parm.inversion = parm.Inversion_Unknown
			#print "frequency:", x[1]
			#print "symbol_rate:", x[2]
			#print "modulation:", x[3]
			#print "fec_inner:", x[4]
			#print "inversion:", 2
			tlist.append(parm)

def getInitialTerrestrialTransponderList(tlist, region):
	list = nimmanager.getTranspondersTerrestrial(region)

	#self.transponders[self.parsedTer].append((2,freq,bw,const,crh,crl,guard,transm,hierarchy,inv))

	#def buildTerTransponder(frequency, inversion = 2, bandwidth = 3, fechigh = 6, feclow = 6,
				#modulation = 2, transmission = 2, guard = 4, hierarchy = 4):

	for x in list:
		if x[0] == 2: #TERRESTRIAL
			parm = buildTerTransponder(x[1], x[9], x[2], x[4], x[5], x[3], x[7], x[6], x[8])
			tlist.append(parm)

cable_bands = {
	"DVBC_BAND_EU_VHF_I" : 1 << 0,
	"DVBC_BAND_EU_MID" : 1 << 1,
	"DVBC_BAND_EU_VHF_III" : 1 << 2,
	"DVBC_BAND_EU_SUPER" : 1 << 3,
	"DVBC_BAND_EU_HYPER" : 1 << 4,
	"DVBC_BAND_EU_UHF_IV" : 1 << 5,
	"DVBC_BAND_EU_UHF_V" : 1 << 6,
	"DVBC_BAND_US_LO" : 1 << 7,
	"DVBC_BAND_US_MID" : 1 << 8,
	"DVBC_BAND_US_HI" : 1 << 9,
	"DVBC_BAND_US_SUPER" : 1 << 10,
	"DVBC_BAND_US_HYPER" : 1 << 11,
}

class TransponderSearchSupport:
	def tryGetRawFrontend(self, feid, ret_boolean=True, do_close=True):
		res_mgr = eDVBResourceManager.getInstance()
		if res_mgr:
			raw_channel = res_mgr.allocateRawChannel(self.feid)
			if raw_channel:
				frontend = raw_channel.getFrontend()
				if frontend:
					if do_close:
						frontend.closeFrontend() # immediate close... 
					if ret_boolean:
						del raw_channel
						del frontend
						return True
					return raw_channel, frontend
		if ret_boolean:
			return False
		return (False, False)

class CableTransponderSearchSupport:
	def cableTransponderSearchSessionClosed(self, *val):
		print "cableTransponderSearchSessionClosed, val", val
		self.cable_search_container.appClosed.remove(self.cableTransponderSearchClosed)
		self.cable_search_container.dataAvail.remove(self.getCableTransponderData)
		if val and len(val):
			if val[0]:
				self.setTransponderSearchResult(self.__tlist)
			else:
				self.cable_search_container.sendCtrlC()
				self.setTransponderSearchResult(None)
		self.cable_search_container = None
		self.cable_search_session = None
		self.__tlist = None
		self.TransponderSearchFinished()

	def cableTransponderSearchClosed(self, retval):
		print "cableTransponderSearch finished", retval
		self.cable_search_session.close(True)

	def getCableTransponderData(self, str):
		data = str.split()
		if len(data):
			if data[0] == 'OK':
				print str
				parm = eDVBFrontendParametersCable()
				qam = { "QAM16" : parm.Modulation_QAM16,
					"QAM32" : parm.Modulation_QAM32,
					"QAM64" : parm.Modulation_QAM64,
					"QAM128" : parm.Modulation_QAM128,
					"QAM256" : parm.Modulation_QAM256 }
				inv = { "INVERSION_OFF" : parm.Inversion_Off,
					"INVERSION_ON" : parm.Inversion_On,
					"INVERSION_AUTO" : parm.Inversion_Unknown }
				fec = { "FEC_AUTO" : parm.FEC_Auto,
					"FEC_1_2" : parm.FEC_1_2,
					"FEC_2_3" : parm.FEC_2_3,
					"FEC_3_4" : parm.FEC_3_4,
					"FEC_5_6": parm.FEC_5_6,
					"FEC_7_8" : parm.FEC_7_8,
					"FEC_8_9" : parm.FEC_8_9,
					"FEC_NONE" : parm.FEC_None }
				parm.frequency = int(data[1])
				parm.symbol_rate = int(data[2])
				parm.fec_inner = fec[data[3]]
				parm.modulation = qam[data[4]]
				parm.inversion = inv[data[5]]
				self.__tlist.append(parm)
			tmpstr = _("Try to find used Transponders in cable network.. please wait...")
			tmpstr += "\n\n"
			tmpstr += data[1]
			tmpstr += " kHz "
			tmpstr += data[0]
			self.cable_search_session["text"].setText(tmpstr)

	def startCableTransponderSearch(self, nim_idx):
		if not self.tryGetRawFrontend(nim_idx):
			self.session.nav.stopService()
			if not self.tryGetRawFrontend(nim_idx):
				if self.session.pipshown: # try to disable pip
					self.session.pipshown = False
					del self.session.pip
				if not self.tryGetRawFrontend(nim_idx):
					self.TransponderSearchFinished()
					return
		self.__tlist = [ ]
		self.cable_search_container = eConsoleAppContainer()
		self.cable_search_container.appClosed.append(self.cableTransponderSearchClosed)
		self.cable_search_container.dataAvail.append(self.getCableTransponderData)
		cableConfig = config.Nims[nim_idx].cable
		tunername = nimmanager.getNimName(nim_idx)
		bus = nimmanager.getI2CDevice(nim_idx)
		if bus is None:
			print "ERROR: could not get I2C device for nim", nim_idx, "for cable transponder search"
			bus = 2

		if tunername == "CXD1981":
			cmd = "cxd1978 --init --scan --verbose --wakeup --inv 2 --bus %d" % bus
		else:
			cmd = "tda1002x --init --scan --verbose --wakeup --inv 2 --bus %d" % bus
		
		if cableConfig.scan_type.value == "bands":
			cmd += " --scan-bands "
			bands = 0
			if cableConfig.scan_band_EU_VHF_I.value:
				bands |= cable_bands["DVBC_BAND_EU_VHF_I"]
			if cableConfig.scan_band_EU_MID.value:
				bands |= cable_bands["DVBC_BAND_EU_MID"]
			if cableConfig.scan_band_EU_VHF_III.value:
				bands |= cable_bands["DVBC_BAND_EU_VHF_III"]
			if cableConfig.scan_band_EU_UHF_IV.value:
				bands |= cable_bands["DVBC_BAND_EU_UHF_IV"]
			if cableConfig.scan_band_EU_UHF_V.value:
				bands |= cable_bands["DVBC_BAND_EU_UHF_V"]
			if cableConfig.scan_band_EU_SUPER.value:
				bands |= cable_bands["DVBC_BAND_EU_SUPER"]
			if cableConfig.scan_band_EU_HYPER.value:
				bands |= cable_bands["DVBC_BAND_EU_HYPER"]
			if cableConfig.scan_band_US_LOW.value:
				bands |= cable_bands["DVBC_BAND_US_LO"]
			if cableConfig.scan_band_US_MID.value:
				bands |= cable_bands["DVBC_BAND_US_MID"]
			if cableConfig.scan_band_US_HIGH.value:
				bands |= cable_bands["DVBC_BAND_US_HI"]
			if cableConfig.scan_band_US_SUPER.value:
				bands |= cable_bands["DVBC_BAND_US_SUPER"]
			if cableConfig.scan_band_US_HYPER.value:
				bands |= cable_bands["DVBC_BAND_US_HYPER"]
			cmd += str(bands)
		else:
			cmd += " --scan-stepsize "
			cmd += str(cableConfig.scan_frequency_steps.value)
		if cableConfig.scan_mod_qam16.value:
			cmd += " --mod 16"
		if cableConfig.scan_mod_qam32.value:
			cmd += " --mod 32"
		if cableConfig.scan_mod_qam64.value:
			cmd += " --mod 64"
		if cableConfig.scan_mod_qam128.value:
			cmd += " --mod 128"
		if cableConfig.scan_mod_qam256.value:
			cmd += " --mod 256"
		if cableConfig.scan_sr_6900.value:
			cmd += " --sr 6900000"
		if cableConfig.scan_sr_6875.value:
			cmd += " --sr 6875000"
		if cableConfig.scan_sr_ext1.value > 450:
			cmd += " --sr "
			cmd += str(cableConfig.scan_sr_ext1.value)
			cmd += "000"
		if cableConfig.scan_sr_ext2.value > 450:
			cmd += " --sr "
			cmd += str(cableConfig.scan_sr_ext2.value)
			cmd += "000"
		print "TDA1002x CMD is", cmd

		self.cable_search_container.execute(cmd)
		tmpstr = _("Try to find used transponders in cable network.. please wait...")
		tmpstr += "\n\n..."
		self.cable_search_session = self.session.openWithCallback(self.cableTransponderSearchSessionClosed, MessageBox, tmpstr, MessageBox.TYPE_INFO)

from time import time

FOUND_HISTORY_SIZE = 8

from Components.Sources.CanvasSource import CanvasSource
from Components.Sources.List import List
from Tools.Directories import fileExists
from enigma import eTimer

class SatBlindscanState(Screen):
	def __init__(self, session, fe_num, text):
		Screen.__init__(self, session)
		self["list"]=List()
		self["text"]=Label()
		self["text"].setText(text)
		self["post_action"]=Label()
		self["progress"]=Label()
		self["actions"] = ActionMap(["OkCancelActions", "ColorActions"],
		{
			"ok": self.keyOk,
			"cancel": self.keyCancel,
			"green": self.keyGreen,
		}, -2)
		self.fe_num = fe_num
		self["constellation"] = CanvasSource()
		self.onLayoutFinish.append(self.updateConstellation)
		self.tmr = eTimer()
		self.tmr.callback.append(self.updateConstellation)
		self.constellation_supported = None
		if fe_num != -1:
			self.post_action=1
			self.finished=0
			self.keyGreen()
		else:
			self.post_action=-1

	def keyGreen(self):
		if self.finished:
			self.close(True)
		elif self.post_action != -1:
			self.post_action ^= 1
			if self.post_action:
				self["post_action"].setText(_("MANUALLY start service searching, press green to change"))
			else:
				self["post_action"].setText(_("AUTOMATICALLY start service searching, press green to change"))

	def setFinished(self):
		if self.post_action:
			self.finished=1
			self["text"].setText(_("Transponder searching finished!"))
			self["post_action"].setText(_("Press green to start service searching!"))
		else:
			self.close(True)

	def getConstellationBitmap(self, cnt=1):
		ret = []
		path = "/proc/stb/frontend/%d/constellation_bitmap" %self.fe_num
		if self.constellation_supported is None:
			s = fileExists(path)
			self.constellation_supported = s
			if not s:
				self["constellation"].fill(0,0,256,256,0x25101010)
				self["constellation"].flush()

		if self.constellation_supported:
			while cnt > 0:
				f = open(path, "r")
				ret.append(f.readline())
				cnt -= 1
				f.close()
		return ret

	def updateConstellation(self, constellation_bitmap_list=None):
		if self.constellation_supported or self.constellation_supported is None:
			pass
		else:
			return
		self["constellation"].fill(0,0,256,256,0x25101010)
		if constellation_bitmap_list:
			bitmap_list = constellation_bitmap_list
		else:
			bitmap_list = self.getConstellationBitmap()
		for bitmap in bitmap_list:
			Q = []
			I = []
			for pos in range(0,30,2):
				val = int(bitmap[pos:pos+2], 16)
				I.append(val)
			for pos in range(30,60,2):
				val = int(bitmap[pos:pos+2], 16)
				Q.append(val)
				pos += 2
			for i in range(15):
				self["constellation"].fill(I[i],Q[i],1,1,0x25ffffff)
		self["constellation"].flush()
		if constellation_bitmap_list:
			self.tmr.start(3000, True)
		else:
			self.tmr.start(50, True)

	def keyOk(self):
		cur_sel = self["list"].current
		if cur_sel:
			self.updateConstellation(cur_sel[1])

	def keyCancel(self):
		self.tmr.stop()
		self.close(False)

class SatelliteTransponderSearchSupport:
	def satelliteTransponderSearchSessionClosed(self, *val):
		if self.frontend:
			self.frontend.getStateChangeSignal().remove(self.frontendStateChanged)
			self.frontend = None
			self.channel = None
		print "satelliteTransponderSearchSessionClosed, val", val
		if val and len(val):
			if val[0]:
				self.setTransponderSearchResult(self.__tlist)
			else:
				self.setTransponderSearchResult(None)
		self.satellite_search_session = None
		self.__tlist = None
		self.TransponderSearchFinished()

	def frontendStateChanged(self, frontend_ptr):
		x = { }
		self.frontend.getFrontendStatus(x)
		assert x, "getFrontendStatus failed!"
		if x["tuner_state"] in ("LOCKED", "FAILED"):
			state = self.satellite_search_session

			d = { }
			self.frontend.getTransponderData(d, False)
			d["tuner_type"] = 'DVB-S'
			r = ConvertToHumanReadable(d)

			if x["tuner_state"] == "LOCKED":
				freq = d["frequency"]
				parm = eDVBFrontendParametersSatellite()
				parm.frequency = int(round(float(freq*2) / 1000)) * 1000
				parm.frequency /= 2
				fstr = str(parm.frequency)
				if self.parm.polarisation == eDVBFrontendParametersSatellite.Polarisation_Horizontal:
					fstr += "H KHz SR"
				elif self.parm.polarisation == eDVBFrontendParametersSatellite.Polarisation_Vertical:
					fstr += "V KHz SR"
				elif self.parm.polarisation == eDVBFrontendParametersSatellite.Polarisation_CircularLeft:
					fstr += "L KHz SR"
				elif self.parm.polarisation == eDVBFrontendParametersSatellite.Polarisation_CircularRight:
					fstr += "R KHz SR"
				sr = d["symbol_rate"]
#				print "SR before round", sr
				if sr < 0:
					print "WARNING blind SR is < 0... skip"
					self.parm.frequency += self.parm.symbol_rate
				else:
					sr_rounded = round(float(sr*2L) / 1000) * 1000
					sr_rounded /= 2
#					print "SR after round", sr_rounded
					parm.symbol_rate = int(sr_rounded)
					fstr += str(parm.symbol_rate/1000)
					parm.fec = d["fec_inner"]
					fstr += " "
					fstr += r["fec_inner"]
					parm.inversion = d["inversion"]
					parm.polarisation = d["polarization"]
					parm.orbital_position = d["orbital_position"]
					parm.system = d["system"]
					fstr += " "
					fstr += r["system"]
					parm.modulation = d["modulation"]
					fstr += " "
					fstr += r["modulation"]

					if parm.system == eDVBFrontendParametersSatellite.System_DVB_S2:
						parm.rolloff = d["rolloff"]
						parm.pilot = d["pilot"]
					self.__tlist.append(parm)

					print "LOCKED at", freq, "SEARCHED at", self.parm.frequency, "half bw", (135L*((sr+1000)/1000)/200), "half search range", (self.parm.symbol_rate/2)
					self.parm.frequency = freq
					self.parm.frequency += (135L*((sr+999)/1000)/200)
					self.parm.frequency += self.parm.symbol_rate/2

					bm = state.getConstellationBitmap(5)
					self.tp_found.append((fstr, bm))
					state.updateConstellation(bm)

					if len(self.tp_found):
						state["list"].updateList(self.tp_found)
					else:
						state["list"].setList(self.tp_found)
						state["list"].setIndex(0)
			else:
				self.parm.frequency += self.parm.symbol_rate

			print "NEXT freq", self.parm.frequency

			mhz_complete, mhz_done = self.stats()
			seconds_done = int(time() - self.start_time)

			if self.parm.frequency > self.range_list[self.current_range][1]:
				self.parm = self.setNextRange()
				if self.parm is not None:
					tparm = eDVBFrontendParameters()
					tparm.setDVBS(self.parm, False)
					self.frontend.tune(tparm)
				else:
					tmpstr = _("%dMhz scanned") %mhz_complete
					tmpstr += ', '
					tmpstr += _("%d transponders found at %d:%02dmin") %(len(self.tp_found),seconds_done / 60, seconds_done % 60)
					state["progress"].setText(tmpstr)
					state.setFinished()
					self.frontend.getStateChangeSignal().remove(self.frontendStateChanged)
					self.frontend = None
					self.channel = None
					return

			tmpstr = str((self.parm.frequency+500)/1000)
			if self.parm.polarisation == eDVBFrontendParametersSatellite.Polarisation_Horizontal:
				tmpstr += "H"
			elif self.parm.polarisation == eDVBFrontendParametersSatellite.Polarisation_Vertical:
				tmpstr += "V"
			elif self.parm.polarisation == eDVBFrontendParametersSatellite.Polarisation_CircularLeft:
				tmpstr += "L"
			elif self.parm.polarisation == eDVBFrontendParametersSatellite.Polarisation_CircularRight:
				tmpstr += "R"

			tmpstr += ', '
			tmpstr += "%d/%dMhz" %(mhz_done, mhz_complete)

			tmpstr += ", "
			tmpstr += _("%d transponder(s) found") %len(self.tp_found)

			tmpstr += ', '

			seconds_complete = (seconds_done * mhz_complete) / mhz_done
			tmpstr += _("%d:%02d/%d:%02dmin") %(seconds_done / 60, seconds_done % 60, seconds_complete / 60, seconds_complete % 60)

			state["progress"].setText(tmpstr)

			self.tuneNext()
		else:
			print "unhandled tuner state", x["tuner_state"]

	def tuneNext(self):
		tparm = eDVBFrontendParameters()
		tparm.setDVBS(self.parm, False)
		self.frontend.tune(tparm)

	def setNextRange(self):
		if self.current_range is None:
			self.current_range = 0
		else:
			self.current_range += 1
		if len(self.range_list) > self.current_range:
			bs_range = self.range_list[self.current_range]
			parm = eDVBFrontendParametersSatellite()
			parm.frequency = bs_range[0]
			if self.nim.isCompatible("DVB-S2"):
				steps = { 5 : 2000, 4 : 4000, 3 : 6000, 2 : 8000, 1 : 10000 }[self.scan_sat.bs_accuracy.value]
				parm.system = self.scan_sat.bs_system.value
				parm.pilot = eDVBFrontendParametersSatellite.Pilot_Unknown
				parm.rolloff = eDVBFrontendParametersSatellite.RollOff_alpha_0_35
			else:
				steps = 4000
				parm.system = eDVBFrontendParametersSatellite.System_DVB_S
			parm.symbol_rate = steps
			parm.fec = eDVBFrontendParametersSatellite.FEC_Auto
			parm.inversion = eDVBFrontendParametersSatellite.Inversion_Unknown
			parm.polarisation = bs_range[2]
			parm.orbital_position = self.orb_pos
			parm.modulation = eDVBFrontendParametersSatellite.Modulation_QPSK
			return parm
		return None

	def stats(self):
		mhz_complete = 0
		mhz_done = 0
		cnt = 0
		for range in self.range_list:
			mhz = (range[1] - range[0]) / 1000
			mhz_complete += mhz
			if cnt == self.current_range:
				mhz_done += (self.parm.frequency - range[0]) / 1000
			elif cnt < self.current_range:
				mhz_done += mhz
			cnt += 1
		return mhz_complete, mhz_done

	def startSatelliteTransponderSearch(self, nim_idx, orb_pos):
		self.frontend = None
		self.orb_pos = orb_pos
		self.nim = nimmanager.nim_slots[nim_idx]
		tunername = nimmanager.getNimName(nim_idx)
		self.__tlist = [ ]
		self.tp_found = [ ]
		self.current_range = None
		self.range_list = [ ]
		tuner_no = -1

		print "tunername", tunername
		if tunername in ("BCM4505", "BCM4506 (internal)", "Alps BSBE1 C01A/D01A."):
			(self.channel, self.frontend) = self.tryGetRawFrontend(nim_idx, False, False)
			if not self.frontend:
				self.session.nav.stopService()
				(self.channel, self.frontend) = self.tryGetRawFrontend(nim_idx, False, False)
				if not self.frontend:
					if self.session.pipshown: # try to disable pip
						self.session.pipshown = False
						del self.session.pip
					(self.channel, self.frontend) = self.tryGetRawFrontend(nim_idx, False, False)
					if not self.frontend:
						print "couldn't allocate tuner %d for blindscan!!!" %nim_idx
						return
			self.frontend.getStateChangeSignal().append(self.frontendStateChanged)

			if self.scan_sat.bs_vertical.value:
				self.range_list.append((self.scan_sat.bs_freq_start.value * 1000, self.scan_sat.bs_freq_stop.value * 1000, eDVBFrontendParametersSatellite.Polarisation_Vertical))
			if self.scan_sat.bs_horizontal.value:
				self.range_list.append((self.scan_sat.bs_freq_start.value * 1000, self.scan_sat.bs_freq_stop.value * 1000, eDVBFrontendParametersSatellite.Polarisation_Horizontal))

			self.parm = self.setNextRange()
			if self.parm is not None:
				tparm = eDVBFrontendParameters()
				tparm.setDVBS(self.parm, False)
				self.frontend.tune(tparm)
				self.start_time = time()
				tmpstr = _("Try to find used satellite transponders...")
			else:
				tmpstr = _("Nothing to scan! Press Exit!")
			x = { }
			data = self.frontend.getFrontendData(x)
			if _isE232:
				tuner_no = x["slot_number"]
			else:
				tuner_no = x["tuner_number"]
		else:
			tmpstr = _("Blindscan is not supported by this tuner (%s)") %tunername
		self.satellite_search_session = self.session.openWithCallback(self.satelliteTransponderSearchSessionClosed, SatBlindscanState, tuner_no, tmpstr)
		self.tuneNextTimer = eTimer()
		self.tuneNextTimer.callback.append(self.tuneNext)

class DefaultSatLists(DefaultWizard):
	def __init__(self, session, silent = True, showSteps = False):
		self.xmlfile = "defaultsatlists.xml"
		DefaultWizard.__init__(self, session, silent, showSteps, neededTag = "services")
		print "configuredSats:", nimmanager.getConfiguredSats()
		self["rc"] = MultiPixmap()

		self.onShown.append(self.initRc)

	def initRc(self):
		self["rc"].setPixmapNum(config.misc.rcused.value)		

	def setDirectory(self):
		self.directory = []
		self.directory.append(resolveFilename(SCOPE_DEFAULTDIR))
		import os
		os.system("mount %s %s" % (resolveFilename(SCOPE_DEFAULTPARTITION), resolveFilename(SCOPE_DEFAULTPARTITIONMOUNTDIR)))
		self.directory.append(resolveFilename(SCOPE_DEFAULTPARTITIONMOUNTDIR))
				
	def statusCallback(self, status, progress):
		print "statusCallback:", status, progress
		from Components.DreamInfoHandler import DreamInfoHandler
		if status == DreamInfoHandler.STATUS_DONE:
			self["text"].setText(_("The installation of the default services lists is finished.") + "\n\n" + _("Please press OK to continue."))
			self.markDone()
			self.disableKeys = False

class ScanSetup(ConfigListScreen, Screen, TransponderSearchSupport, CableTransponderSearchSupport, SatelliteTransponderSearchSupport):
	def __init__(self, session):
		Screen.__init__(self, session)

		self.finished_cb = None
		self.updateSatList()
		self.service = session.nav.getCurrentService()
		self.feinfo = None
		frontendData = None
		if self.service is not None:
			self.feinfo = self.service.frontendInfo()
			frontendData = self.feinfo and self.feinfo.getAll(True)
		
		self.createConfig(frontendData)

		del self.feinfo
		del self.service

		self["actions"] = NumberActionMap(["SetupActions"],
		{
			"ok": self.keyGo,
			"cancel": self.keyCancel,
		}, -2)

		self.statusTimer = eTimer()
		self.statusTimer.callback.append(self.updateStatus)
		#self.statusTimer.start(5000, True)

		self.list = []
		ConfigListScreen.__init__(self, self.list)
		if not self.scan_nims.value == "":
			self.createSetup()
			self["introduction"] = Label(_("Press OK to start the scan"))
		else:
			self["introduction"] = Label(_("Nothing to scan!\nPlease setup your tuner settings before you start a service scan."))

	def runAsync(self, finished_cb):
		self.finished_cb = finished_cb
		self.keyGo()

	def updateSatList(self):
		self.satList = []
		for slot in nimmanager.nim_slots:
			if slot.isCompatible("DVB-S"):
				self.satList.append(nimmanager.getSatListForNim(slot.slot))
			else:
				self.satList.append(None)

	def createSetup(self):
		self.list = []
		self.multiscanlist = []
		index_to_scan = int(self.scan_nims.value)
		print "ID: ", index_to_scan

		self.tunerEntry = getConfigListEntry(_("Tuner"), self.scan_nims)
		self.list.append(self.tunerEntry)
		
		if self.scan_nims == [ ]:
			return
		
		self.typeOfScanEntry = None
		self.systemEntry = None
		self.modulationEntry = None
		self.satelliteEntry = None
		nim = nimmanager.nim_slots[index_to_scan]
		if nim.isCompatible("DVB-S"):
			self.typeOfScanEntry = getConfigListEntry(_("Type of scan"), self.scan_type)
			self.list.append(self.typeOfScanEntry)
		elif nim.isCompatible("DVB-C"):
			self.typeOfScanEntry = getConfigListEntry(_("Type of scan"), self.scan_typecable)
			self.list.append(self.typeOfScanEntry)
		elif nim.isCompatible("DVB-T"):
			self.typeOfScanEntry = getConfigListEntry(_("Type of scan"), self.scan_typeterrestrial)
			self.list.append(self.typeOfScanEntry)

		self.scan_networkScan.value = False
		if nim.isCompatible("DVB-S"):
			if self.scan_type.value == "single_transponder":
				self.updateSatList()
				if nim.isCompatible("DVB-S2"):
					self.systemEntry = getConfigListEntry(_('System'), self.scan_sat.system)
					self.list.append(self.systemEntry)
				else:
					# downgrade to dvb-s, in case a -s2 config was active
					self.scan_sat.system.value = eDVBFrontendParametersSatellite.System_DVB_S
				self.list.append(getConfigListEntry(_('Satellite'), self.scan_satselection[index_to_scan]))
				self.list.append(getConfigListEntry(_('Frequency'), self.scan_sat.frequency))
				self.list.append(getConfigListEntry(_('Inversion'), self.scan_sat.inversion))
				self.list.append(getConfigListEntry(_('Symbol rate'), self.scan_sat.symbolrate))
				self.list.append(getConfigListEntry(_('Polarization'), self.scan_sat.polarization))
				if self.scan_sat.system.value == eDVBFrontendParametersSatellite.System_DVB_S:
					self.list.append(getConfigListEntry(_("FEC"), self.scan_sat.fec))
				elif self.scan_sat.system.value == eDVBFrontendParametersSatellite.System_DVB_S2:
					self.list.append(getConfigListEntry(_("FEC"), self.scan_sat.fec_s2))
					self.modulationEntry = getConfigListEntry(_('Modulation'), self.scan_sat.modulation)
					self.list.append(self.modulationEntry)
					self.list.append(getConfigListEntry(_('Roll-off'), self.scan_sat.rolloff))
					self.list.append(getConfigListEntry(_('Pilot'), self.scan_sat.pilot))
			elif self.scan_type.value == "single_satellite":
				self.updateSatList()
				self.list.append(getConfigListEntry(_("Satellite"), self.scan_satselection[index_to_scan]))
				self.scan_networkScan.value = True
			elif self.scan_type.value == "blind_scan":
				self.updateSatList()
				selected_sat_pos = self.scan_satselection[index_to_scan].value
				limit_list = self.nim_sat_frequency_range[index_to_scan][int(selected_sat_pos)]
				l = limit_list[0]
				limits = ( l[0]/1000, l[1]/1000 )
				self.scan_sat.bs_freq_start = ConfigInteger(default = limits[0], limits = (limits[0], limits[1]))
				self.scan_sat.bs_freq_stop = ConfigInteger(default = limits[1], limits = (limits[0], limits[1]))
				self.satelliteEntry = getConfigListEntry(_("Satellite"), self.scan_satselection[index_to_scan])
				self.list.append(self.satelliteEntry)
				self.list.append(getConfigListEntry(_("Frequency start"), self.scan_sat.bs_freq_start))
				self.list.append(getConfigListEntry(_("Frequency stop"), self.scan_sat.bs_freq_stop))
				if nim.isCompatible("DVB-S2"):
					self.list.append(getConfigListEntry(_("Accuracy (higher is better)"), self.scan_sat.bs_accuracy))
				self.list.append(getConfigListEntry(_("Horizontal"), self.scan_sat.bs_horizontal))
				self.list.append(getConfigListEntry(_("Vertical"), self.scan_sat.bs_vertical))
				self.scan_networkScan.value = True
			elif self.scan_type.value.find("multisat") != -1:
				tlist = []
				SatList = nimmanager.getSatListForNim(index_to_scan)
				for x in SatList:
					if self.Satexists(tlist, x[0]) == 0:
						tlist.append(x[0])
						sat = ConfigEnableDisable(default = self.scan_type.value.find("_yes") != -1 and True or False)
						configEntry = getConfigListEntry(nimmanager.getSatDescription(x[0]), sat)
						self.list.append(configEntry)
						self.multiscanlist.append((x[0], sat))
				self.scan_networkScan.value = True
		elif nim.isCompatible("DVB-C"):
			if self.scan_typecable.value == "single_transponder":
				self.list.append(getConfigListEntry(_("Frequency"), self.scan_cab.frequency))
				self.list.append(getConfigListEntry(_("Inversion"), self.scan_cab.inversion))
				self.list.append(getConfigListEntry(_("Symbol rate"), self.scan_cab.symbolrate))
				self.list.append(getConfigListEntry(_("Modulation"), self.scan_cab.modulation))
				self.list.append(getConfigListEntry(_("FEC"), self.scan_cab.fec))
		elif nim.isCompatible("DVB-T"):
			if self.scan_typeterrestrial.value == "single_transponder":
				self.list.append(getConfigListEntry(_("Frequency"), self.scan_ter.frequency))
				self.list.append(getConfigListEntry(_("Inversion"), self.scan_ter.inversion))
				self.list.append(getConfigListEntry(_("Bandwidth"), self.scan_ter.bandwidth))
				self.list.append(getConfigListEntry(_("Code rate HP"), self.scan_ter.fechigh))
				self.list.append(getConfigListEntry(_("Code rate LP"), self.scan_ter.feclow))
				self.list.append(getConfigListEntry(_("Modulation"), self.scan_ter.modulation))
				self.list.append(getConfigListEntry(_("Transmission mode"), self.scan_ter.transmission))
				self.list.append(getConfigListEntry(_("Guard interval"), self.scan_ter.guard))
				self.list.append(getConfigListEntry(_("Hierarchy info"), self.scan_ter.hierarchy))
		self.list.append(getConfigListEntry(_("Network scan"), self.scan_networkScan))
		self.list.append(getConfigListEntry(_("Clear before scan"), self.scan_clearallservices))
		self.list.append(getConfigListEntry(_("Only Free scan"), self.scan_onlyfree))
		self["config"].list = self.list
		self["config"].l.setList(self.list)

	def Satexists(self, tlist, pos):
		for x in tlist:
			if x == pos:
				return 1
		return 0

	def newConfig(self):
		cur = self["config"].getCurrent()
		print "cur is", cur
		if cur == self.typeOfScanEntry or \
			cur == self.tunerEntry or \
			cur == self.systemEntry or \
			(self.modulationEntry and self.systemEntry[1].value == eDVBFrontendParametersSatellite.System_DVB_S2 and cur == self.modulationEntry) or \
			(self.satelliteEntry and cur == self.satelliteEntry):
			self.createSetup()

	def createConfig(self, frontendData):
							   #("Type", frontendData["system"], TYPE_TEXT),
					   #("Modulation", frontendData["modulation"], TYPE_TEXT),
					   #("Orbital position", frontendData["orbital_position"], TYPE_VALUE_DEC),
					   #("Frequency", frontendData["frequency"], TYPE_VALUE_DEC),
					   #("Symbolrate", frontendData["symbol_rate"], TYPE_VALUE_DEC),
					   #("Polarization", frontendData["polarization"], TYPE_TEXT),
					   #("Inversion", frontendData["inversion"], TYPE_TEXT),
					   #("FEC inner", frontendData["fec_inner"], TYPE_TEXT),
				   		#)
		#elif frontendData["tuner_type"] == "DVB-C":
			#return ( ("NIM", ['A', 'B', 'C', 'D'][frontendData["tuner_number"]], TYPE_TEXT),
					   #("Type", frontendData["tuner_type"], TYPE_TEXT),
					   #("Frequency", frontendData["frequency"], TYPE_VALUE_DEC),
					   #("Symbolrate", frontendData["symbol_rate"], TYPE_VALUE_DEC),
					   #("Modulation", frontendData["modulation"], TYPE_TEXT),
					   #("Inversion", frontendData["inversion"], TYPE_TEXT),
			#		   ("FEC inner", frontendData["fec_inner"], TYPE_TEXT),
				   		#)
		#elif frontendData["tuner_type"] == "DVB-T":
			#return ( ("NIM", ['A', 'B', 'C', 'D'][frontendData["tuner_number"]], TYPE_TEXT),
					   #("Type", frontendData["tuner_type"], TYPE_TEXT),
					   #("Frequency", frontendData["frequency"], TYPE_VALUE_DEC),
					   #("Inversion", frontendData["inversion"], TYPE_TEXT),
					   #("Bandwidth", frontendData["bandwidth"], TYPE_VALUE_DEC),
					   #("CodeRateLP", frontendData["code_rate_lp"], TYPE_TEXT),
					   #("CodeRateHP", frontendData["code_rate_hp"], TYPE_TEXT),
					   #("Constellation", frontendData["constellation"], TYPE_TEXT),
					   #("Transmission Mode", frontendData["transmission_mode"], TYPE_TEXT),
					   #("Guard Interval", frontendData["guard_interval"], TYPE_TEXT),
					   #("Hierarchy Inform.", frontendData["hierarchy_information"], TYPE_TEXT),
			defaultSat = {
				"orbpos": 192,
				"system": eDVBFrontendParametersSatellite.System_DVB_S,
				"frequency": 11836,
				"inversion": eDVBFrontendParametersSatellite.Inversion_Unknown,
				"symbolrate": 27500,
				"polarization": eDVBFrontendParametersSatellite.Polarisation_Horizontal,
				"fec": eDVBFrontendParametersSatellite.FEC_Auto,
				"fec_s2": eDVBFrontendParametersSatellite.FEC_9_10,
				"modulation": eDVBFrontendParametersSatellite.Modulation_QPSK }
			defaultCab = {
				"frequency": 466,
				"inversion": eDVBFrontendParametersCable.Inversion_Unknown,
				"modulation": eDVBFrontendParametersCable.Modulation_QAM64,
				"fec": eDVBFrontendParametersCable.FEC_Auto,
				"symbolrate": 6900 }
			defaultTer = {
				"frequency" : 466000,
				"inversion" : eDVBFrontendParametersTerrestrial.Inversion_Unknown,
				"bandwidth" : eDVBFrontendParametersTerrestrial.Bandwidth_7MHz,
				"fechigh" : eDVBFrontendParametersTerrestrial.FEC_Auto,
				"feclow" : eDVBFrontendParametersTerrestrial.FEC_Auto,
				"modulation" : eDVBFrontendParametersTerrestrial.Modulation_Auto,
				"transmission_mode" : eDVBFrontendParametersTerrestrial.TransmissionMode_Auto,
				"guard_interval" : eDVBFrontendParametersTerrestrial.GuardInterval_Auto,
				"hierarchy": eDVBFrontendParametersTerrestrial.Hierarchy_Auto }

			if frontendData is not None:
				ttype = frontendData.get("tuner_type", "UNKNOWN")
				if ttype == "DVB-S":
					defaultSat["system"] = frontendData.get("system", eDVBFrontendParametersSatellite.System_DVB_S)
					defaultSat["frequency"] = frontendData.get("frequency", 0) / 1000
					defaultSat["inversion"] = frontendData.get("inversion", eDVBFrontendParametersSatellite.Inversion_Unknown)
					defaultSat["symbolrate"] = frontendData.get("symbol_rate", 0) / 1000
					defaultSat["polarization"] = frontendData.get("polarization", eDVBFrontendParametersSatellite.Polarisation_Horizontal)
					if defaultSat["system"] == eDVBFrontendParametersSatellite.System_DVB_S2:
						defaultSat["fec_s2"] = frontendData.get("fec_inner", eDVBFrontendParametersSatellite.FEC_Auto)
						defaultSat["rolloff"] = frontendData.get("rolloff", eDVBFrontendParametersSatellite.RollOff_alpha_0_35)
						defaultSat["pilot"] = frontendData.get("pilot", eDVBFrontendParametersSatellite.Pilot_Unknown)
					else:
						defaultSat["fec"] = frontendData.get("fec_inner", eDVBFrontendParametersSatellite.FEC_Auto)
					defaultSat["modulation"] = frontendData.get("modulation", eDVBFrontendParametersSatellite.Modulation_QPSK)
					defaultSat["orbpos"] = frontendData.get("orbital_position", 0)
				elif ttype == "DVB-C":
					defaultCab["frequency"] = frontendData.get("frequency", 0) / 1000
					defaultCab["symbolrate"] = frontendData.get("symbol_rate", 0) / 1000
					defaultCab["inversion"] = frontendData.get("inversion", eDVBFrontendParametersCable.Inversion_Unknown)
					defaultCab["fec"] = frontendData.get("fec_inner", eDVBFrontendParametersCable.FEC_Auto)
					defaultCab["modulation"] = frontendData.get("modulation", eDVBFrontendParametersCable.Modulation_QAM16)
				elif ttype == "DVB-T":
					defaultTer["frequency"] = frontendData.get("frequency", 0)
					defaultTer["inversion"] = frontendData.get("inversion", eDVBFrontendParametersTerrestrial.Inversion_Unknown)
					defaultTer["bandwidth"] = frontendData.get("bandwidth", eDVBFrontendParametersTerrestrial.Bandwidth_7MHz)
					defaultTer["fechigh"] = frontendData.get("code_rate_hp", eDVBFrontendParametersTerrestrial.FEC_Auto)
					defaultTer["feclow"] = frontendData.get("code_rate_lp", eDVBFrontendParametersTerrestrial.FEC_Auto)
					defaultTer["modulation"] = frontendData.get("constellation", eDVBFrontendParametersTerrestrial.Modulation_Auto)
					defaultTer["transmission_mode"] = frontendData.get("transmission_mode", eDVBFrontendParametersTerrestrial.TransmissionMode_Auto)
					defaultTer["guard_interval"] = frontendData.get("guard_interval", eDVBFrontendParametersTerrestrial.GuardInterval_Auto)
					defaultTer["hierarchy"] = frontendData.get("hierarchy_information", eDVBFrontendParametersTerrestrial.Hierarchy_Auto)

			self.scan_sat = ConfigSubsection()
			self.scan_cab = ConfigSubsection()
			self.scan_ter = ConfigSubsection()

			if _isE232:
				self.scan_type = ConfigSelection(default = "single_transponder", choices = [("single_transponder", _("Single transponder")), ("single_satellite", _("Single satellite")), ("multisat", _("Multisat")), ("multisat_yes", _("Multisat")), ("blind_scan", _("Blindscan"))])
			else:
				self.scan_type = ConfigSelection(default = "single_transponder", choices = [("single_transponder", _("Single transponder")), ("single_satellite", _("Single satellite")), ("multisat", _("Multisat")), ("multisat_yes", _("Multisat"))])
			self.scan_typecable = ConfigSelection(default = "single_transponder", choices = [("single_transponder", _("Single transponder")), ("complete", _("Complete"))])
			self.scan_typeterrestrial = ConfigSelection(default = "single_transponder", choices = [("single_transponder", _("Single transponder")), ("complete", _("Complete"))])
			self.scan_clearallservices = ConfigSelection(default = "no", choices = [("no", _("no")), ("yes", _("yes")), ("yes_hold_feeds", _("yes (keep feeds)"))])
			self.scan_onlyfree = ConfigYesNo(default = False)
			self.scan_networkScan = ConfigYesNo(default = False)

			nim_list = []
			# collect all nims which are *not* set to "nothing"
			for n in nimmanager.nim_slots:
				if n.config_mode == "nothing":
					continue
				if n.config_mode == "advanced" and len(nimmanager.getSatListForNim(n.slot)) < 1:
					continue
				if n.config_mode in ("loopthrough", "satposdepends"):
					root_id = nimmanager.sec.getRoot(n.slot_id, int(n.config.connectedTo.value))
					if n.type == nimmanager.nim_slots[root_id].type: # check if connected from a DVB-S to DVB-S2 Nim or vice versa
						continue
				nim_list.append((str(n.slot), n.friendly_full_description))

			self.scan_nims = ConfigSelection(choices = nim_list)

			# status
			self.scan_snr = ConfigSlider()
			self.scan_snr.enabled = False
			self.scan_agc = ConfigSlider()
			self.scan_agc.enabled = False
			self.scan_ber = ConfigSlider()
			self.scan_ber.enabled = False

			# sat
			self.scan_sat.system = ConfigSelection(default = defaultSat["system"], choices = [
				(eDVBFrontendParametersSatellite.System_DVB_S, _("DVB-S")),
				(eDVBFrontendParametersSatellite.System_DVB_S2, _("DVB-S2"))])
			self.scan_sat.frequency = ConfigInteger(default = defaultSat["frequency"], limits = (1, 99999))
			self.scan_sat.inversion = ConfigSelection(default = defaultSat["inversion"], choices = [
				(eDVBFrontendParametersSatellite.Inversion_Off, _("Off")),
				(eDVBFrontendParametersSatellite.Inversion_On, _("On")),
				(eDVBFrontendParametersSatellite.Inversion_Unknown, _("Auto"))])
			self.scan_sat.symbolrate = ConfigInteger(default = defaultSat["symbolrate"], limits = (1, 99999))
			self.scan_sat.polarization = ConfigSelection(default = defaultSat["polarization"], choices = [
				(eDVBFrontendParametersSatellite.Polarisation_Horizontal, _("horizontal")),
				(eDVBFrontendParametersSatellite.Polarisation_Vertical, _("vertical")),
				(eDVBFrontendParametersSatellite.Polarisation_CircularLeft, _("circular left")),
				(eDVBFrontendParametersSatellite.Polarisation_CircularRight, _("circular right"))])
			self.scan_sat.fec = ConfigSelection(default = defaultSat["fec"], choices = [
				(eDVBFrontendParametersSatellite.FEC_Auto, _("Auto")),
				(eDVBFrontendParametersSatellite.FEC_1_2, "1/2"),
				(eDVBFrontendParametersSatellite.FEC_2_3, "2/3"),
				(eDVBFrontendParametersSatellite.FEC_3_4, "3/4"),
				(eDVBFrontendParametersSatellite.FEC_5_6, "5/6"),
				(eDVBFrontendParametersSatellite.FEC_7_8, "7/8"),
				(eDVBFrontendParametersSatellite.FEC_None, _("None"))])
			self.scan_sat.fec_s2 = ConfigSelection(default = defaultSat["fec_s2"], choices = [
				(eDVBFrontendParametersSatellite.FEC_1_2, "1/2"),
				(eDVBFrontendParametersSatellite.FEC_2_3, "2/3"),
				(eDVBFrontendParametersSatellite.FEC_3_4, "3/4"),
				(eDVBFrontendParametersSatellite.FEC_3_5, "3/5"),
				(eDVBFrontendParametersSatellite.FEC_4_5, "4/5"),
				(eDVBFrontendParametersSatellite.FEC_5_6, "5/6"),
				(eDVBFrontendParametersSatellite.FEC_7_8, "7/8"),
				(eDVBFrontendParametersSatellite.FEC_8_9, "8/9"),
				(eDVBFrontendParametersSatellite.FEC_9_10, "9/10")])
			self.scan_sat.modulation = ConfigSelection(default = defaultSat["modulation"], choices = [
				(eDVBFrontendParametersSatellite.Modulation_QPSK, "QPSK"),
				(eDVBFrontendParametersSatellite.Modulation_8PSK, "8PSK")])
			self.scan_sat.rolloff = ConfigSelection(default = defaultSat.get("rolloff", eDVBFrontendParametersSatellite.RollOff_alpha_0_35), choices = [
				(eDVBFrontendParametersSatellite.RollOff_alpha_0_35, "0.35"),
				(eDVBFrontendParametersSatellite.RollOff_alpha_0_25, "0.25"),
				(eDVBFrontendParametersSatellite.RollOff_alpha_0_20, "0.20")])
			self.scan_sat.pilot = ConfigSelection(default = defaultSat.get("pilot", eDVBFrontendParametersSatellite.Pilot_Unknown), choices = [
				(eDVBFrontendParametersSatellite.Pilot_Off, _("Off")),
				(eDVBFrontendParametersSatellite.Pilot_On, _("On")),
				(eDVBFrontendParametersSatellite.Pilot_Unknown, _("Auto"))])


			if _isE232:
				self.scan_sat.bs_system = ConfigSelection(default = eDVBFrontendParametersSatellite.System_DVB_S2, 
					choices = [ (eDVBFrontendParametersSatellite.System_DVB_S2, _("DVB-S + DVB-S2")),
						(eDVBFrontendParametersSatellite.System_DVB_S, _("DVB-S only"))])

				self.scan_sat.bs_accuracy = ConfigSelection(default = 2, choices = [ (1, "1"), (2, "2"), (3, "3"), (4, "4"), (5, "5")])

				self.scan_sat.bs_horizontal = ConfigYesNo(default = True)
				self.scan_sat.bs_vertical = ConfigYesNo(default = True)

			# cable
			self.scan_cab.frequency = ConfigInteger(default = defaultCab["frequency"], limits = (50, 999))
			self.scan_cab.inversion = ConfigSelection(default = defaultCab["inversion"], choices = [
				(eDVBFrontendParametersCable.Inversion_Off, _("Off")),
				(eDVBFrontendParametersCable.Inversion_On, _("On")),
				(eDVBFrontendParametersCable.Inversion_Unknown, _("Auto"))])
			self.scan_cab.modulation = ConfigSelection(default = defaultCab["modulation"], choices = [
				(eDVBFrontendParametersCable.Modulation_QAM16, "16-QAM"),
				(eDVBFrontendParametersCable.Modulation_QAM32, "32-QAM"),
				(eDVBFrontendParametersCable.Modulation_QAM64, "64-QAM"),
				(eDVBFrontendParametersCable.Modulation_QAM128, "128-QAM"),
				(eDVBFrontendParametersCable.Modulation_QAM256, "256-QAM")])
			self.scan_cab.fec = ConfigSelection(default = defaultCab["fec"], choices = [
				(eDVBFrontendParametersCable.FEC_Auto, _("Auto")),
				(eDVBFrontendParametersCable.FEC_1_2, "1/2"),
				(eDVBFrontendParametersCable.FEC_2_3, "2/3"),
				(eDVBFrontendParametersCable.FEC_3_4, "3/4"),
				(eDVBFrontendParametersCable.FEC_5_6, "5/6"),
				(eDVBFrontendParametersCable.FEC_7_8, "7/8"),
				(eDVBFrontendParametersCable.FEC_8_9, "8/9"),
				(eDVBFrontendParametersCable.FEC_None, _("None"))])
			self.scan_cab.symbolrate = ConfigInteger(default = defaultCab["symbolrate"], limits = (1, 99999))

			# terrestial
			self.scan_ter.frequency = ConfigInteger(default = 466000, limits = (50000, 999000))
			self.scan_ter.inversion = ConfigSelection(default = defaultTer["inversion"], choices = [
				(eDVBFrontendParametersTerrestrial.Inversion_Off, _("Off")),
				(eDVBFrontendParametersTerrestrial.Inversion_On, _("On")),
				(eDVBFrontendParametersTerrestrial.Inversion_Unknown, _("Auto"))])
			# WORKAROUND: we can't use BW-auto
			self.scan_ter.bandwidth = ConfigSelection(default = defaultTer["bandwidth"], choices = [
				(eDVBFrontendParametersTerrestrial.Bandwidth_8MHz, "8MHz"),
				(eDVBFrontendParametersTerrestrial.Bandwidth_7MHz, "7MHz"),
				(eDVBFrontendParametersTerrestrial.Bandwidth_6MHz, "6MHz")])
			#, (eDVBFrontendParametersTerrestrial.Bandwidth_Auto, _("Auto"))))
			self.scan_ter.fechigh = ConfigSelection(default = defaultTer["fechigh"], choices = [
				(eDVBFrontendParametersTerrestrial.FEC_1_2, "1/2"),
				(eDVBFrontendParametersTerrestrial.FEC_2_3, "2/3"),
				(eDVBFrontendParametersTerrestrial.FEC_3_4, "3/4"),
				(eDVBFrontendParametersTerrestrial.FEC_5_6, "5/6"),
				(eDVBFrontendParametersTerrestrial.FEC_7_8, "7/8"),
				(eDVBFrontendParametersTerrestrial.FEC_Auto, _("Auto"))])
			self.scan_ter.feclow = ConfigSelection(default = defaultTer["feclow"], choices = [
				(eDVBFrontendParametersTerrestrial.FEC_1_2, "1/2"),
				(eDVBFrontendParametersTerrestrial.FEC_2_3, "2/3"),
				(eDVBFrontendParametersTerrestrial.FEC_3_4, "3/4"),
				(eDVBFrontendParametersTerrestrial.FEC_5_6, "5/6"),
				(eDVBFrontendParametersTerrestrial.FEC_7_8, "7/8"),
				(eDVBFrontendParametersTerrestrial.FEC_Auto, _("Auto"))])
			self.scan_ter.modulation = ConfigSelection(default = defaultTer["modulation"], choices = [
				(eDVBFrontendParametersTerrestrial.Modulation_QPSK, "QPSK"),
				(eDVBFrontendParametersTerrestrial.Modulation_QAM16, "QAM16"),
				(eDVBFrontendParametersTerrestrial.Modulation_QAM64, "QAM64"),
				(eDVBFrontendParametersTerrestrial.Modulation_Auto, _("Auto"))])
			self.scan_ter.transmission = ConfigSelection(default = defaultTer["transmission_mode"], choices = [
				(eDVBFrontendParametersTerrestrial.TransmissionMode_2k, "2K"),
				(eDVBFrontendParametersTerrestrial.TransmissionMode_8k, "8K"),
				(eDVBFrontendParametersTerrestrial.TransmissionMode_Auto, _("Auto"))])
			self.scan_ter.guard = ConfigSelection(default = defaultTer["guard_interval"], choices = [
				(eDVBFrontendParametersTerrestrial.GuardInterval_1_32, "1/32"),
				(eDVBFrontendParametersTerrestrial.GuardInterval_1_16, "1/16"),
				(eDVBFrontendParametersTerrestrial.GuardInterval_1_8, "1/8"),
				(eDVBFrontendParametersTerrestrial.GuardInterval_1_4, "1/4"),
				(eDVBFrontendParametersTerrestrial.GuardInterval_Auto, _("Auto"))])
			self.scan_ter.hierarchy = ConfigSelection(default = defaultTer["hierarchy"], choices = [
				(eDVBFrontendParametersTerrestrial.Hierarchy_None, _("None")),
				(eDVBFrontendParametersTerrestrial.Hierarchy_1, "1"),
				(eDVBFrontendParametersTerrestrial.Hierarchy_2, "2"),
				(eDVBFrontendParametersTerrestrial.Hierarchy_4, "4"),
				(eDVBFrontendParametersTerrestrial.Hierarchy_Auto, _("Auto"))])

			self.scan_scansat = {}
			for sat in nimmanager.satList:
				#print sat[1]
				self.scan_scansat[sat[0]] = ConfigYesNo(default = False)

			if _isE232:
				sec = secClass.getInstance()

				self.nim_sat_frequency_range = []
				self.scan_satselection = []
				for slot in nimmanager.nim_slots:
					slot_id = slot.slot
					self.nim_sat_frequency_range.append
					if slot.isCompatible("DVB-S"):
						satlist_for_slot = self.satList[slot_id]
						self.scan_satselection.append(getConfigSatlist(defaultSat["orbpos"], satlist_for_slot))
						sat_freq_range = { }
						for sat in satlist_for_slot:
							orbpos = sat[0]
							sat_freq_range[orbpos] = sec.getFrequencyRangeList(slot_id, orbpos)
						self.nim_sat_frequency_range.append(sat_freq_range)
					else:
						self.nim_sat_frequency_range.append(None)
						self.scan_satselection.append(None)
			else:
				self.scan_satselection = []
				for slot in nimmanager.nim_slots:
					if slot.isCompatible("DVB-S"):
						self.scan_satselection.append(getConfigSatlist(defaultSat["orbpos"], self.satList[slot.slot]))
					else:
						self.scan_satselection.append(None)
				

			return True

	def keyLeft(self):
		ConfigListScreen.keyLeft(self)
		self.newConfig()

	def keyRight(self):
		ConfigListScreen.keyRight(self)
		self.newConfig()

	def updateStatus(self):
		print "updatestatus"

	def addSatTransponder(self, tlist, frequency, symbol_rate, polarisation, fec, inversion, orbital_position, system, modulation, rolloff, pilot):
		print "Add Sat: frequ: " + str(frequency) + " symbol: " + str(symbol_rate) + " pol: " + str(polarisation) + " fec: " + str(fec) + " inversion: " + str(inversion) + " modulation: " + str(modulation) + " system: " + str(system) + " rolloff" + str(rolloff) + " pilot" + str(pilot)
		print "orbpos: " + str(orbital_position)
		parm = eDVBFrontendParametersSatellite()
		parm.modulation = modulation
		parm.system = system
		parm.frequency = frequency * 1000
		parm.symbol_rate = symbol_rate * 1000
		parm.polarisation = polarisation
		parm.fec = fec
		parm.inversion = inversion
		parm.orbital_position = orbital_position
		parm.rolloff = rolloff
		parm.pilot = pilot
		tlist.append(parm)

	def addCabTransponder(self, tlist, frequency, symbol_rate, modulation, fec, inversion):
		print "Add Cab: frequ: " + str(frequency) + " symbol: " + str(symbol_rate) + " pol: " + str(modulation) + " fec: " + str(fec) + " inversion: " + str(inversion)
		parm = eDVBFrontendParametersCable()
		parm.frequency = frequency * 1000
		parm.symbol_rate = symbol_rate * 1000
		parm.modulation = modulation
		parm.fec = fec
		parm.inversion = inversion
		tlist.append(parm)

	def addTerTransponder(self, tlist, *args, **kwargs):
		tlist.append(buildTerTransponder(*args, **kwargs))

	def keyGo(self):
		if self.scan_nims.value == "":
			return
		tlist = []
		flags = None
		startScan = True
		removeAll = True
		index_to_scan = int(self.scan_nims.value)
		
		if self.scan_nims == [ ]:
			self.session.open(MessageBox, _("No tuner is enabled!\nPlease setup your tuner settings before you start a service scan."), MessageBox.TYPE_ERROR)
			return

		nim = nimmanager.nim_slots[index_to_scan]
		print "nim", nim.slot
		if nim.isCompatible("DVB-S"):
			print "is compatible with DVB-S"
			if self.scan_type.value == "single_transponder":
				# these lists are generated for each tuner, so this has work.
				assert len(self.satList) > index_to_scan
				assert len(self.scan_satselection) > index_to_scan
				
				nimsats = self.satList[index_to_scan]
				selsatidx = self.scan_satselection[index_to_scan].index

				# however, the satList itself could be empty. in that case, "index" is 0 (for "None").
				if len(nimsats):
					orbpos = nimsats[selsatidx][0]
					if self.scan_sat.system.value == eDVBFrontendParametersSatellite.System_DVB_S:
						fec = self.scan_sat.fec.value
					else:
						fec = self.scan_sat.fec_s2.value
					print "add sat transponder"
					self.addSatTransponder(tlist, self.scan_sat.frequency.value,
								self.scan_sat.symbolrate.value,
								self.scan_sat.polarization.value,
								fec,
								self.scan_sat.inversion.value,
								orbpos,
								self.scan_sat.system.value,
								self.scan_sat.modulation.value,
								self.scan_sat.rolloff.value,
								self.scan_sat.pilot.value)
				removeAll = False
			elif self.scan_type.value == "single_satellite":
				sat = self.satList[index_to_scan][self.scan_satselection[index_to_scan].index]
				getInitialTransponderList(tlist, sat[0])
			elif self.scan_type.value.find("multisat") != -1:
				SatList = nimmanager.getSatListForNim(index_to_scan)
				for x in self.multiscanlist:
					if x[1].value:
						print "   " + str(x[0])
						getInitialTransponderList(tlist, x[0])
			elif self.scan_type.value.find("blind_scan") != -1:
				startScan = False


		elif nim.isCompatible("DVB-C"):
			if self.scan_typecable.value == "single_transponder":
				self.addCabTransponder(tlist, self.scan_cab.frequency.value,
											  self.scan_cab.symbolrate.value,
											  self.scan_cab.modulation.value,
											  self.scan_cab.fec.value,
											  self.scan_cab.inversion.value)
				removeAll = False
			elif self.scan_typecable.value == "complete":
				if config.Nims[index_to_scan].cable.scan_type.value == "provider":
					getInitialCableTransponderList(tlist, index_to_scan)
				else:
					startScan = False

		elif nim.isCompatible("DVB-T"):
			if self.scan_typeterrestrial.value == "single_transponder":
				self.addTerTransponder(tlist,
						self.scan_ter.frequency.value * 1000,
						inversion = self.scan_ter.inversion.value,
						bandwidth = self.scan_ter.bandwidth.value,
						fechigh = self.scan_ter.fechigh.value,
						feclow = self.scan_ter.feclow.value,
						modulation = self.scan_ter.modulation.value,
						transmission = self.scan_ter.transmission.value,
						guard = self.scan_ter.guard.value,
						hierarchy = self.scan_ter.hierarchy.value)
				removeAll = False
			elif self.scan_typeterrestrial.value == "complete":
				getInitialTerrestrialTransponderList(tlist, nimmanager.getTerrestrialDescription(index_to_scan))

		flags = self.scan_networkScan.value and eComponentScan.scanNetworkSearch or 0

		tmp = self.scan_clearallservices.value
		if tmp == "yes":
			flags |= eComponentScan.scanRemoveServices
		elif tmp == "yes_hold_feeds":
			flags |= eComponentScan.scanRemoveServices
			flags |= eComponentScan.scanDontRemoveFeeds

		if tmp != "no" and not removeAll:
			flags |= eComponentScan.scanDontRemoveUnscanned

		if self.scan_onlyfree.value:
			flags |= eComponentScan.scanOnlyFree

		for x in self["config"].list:
			x[1].save()

		if startScan:
			self.startScan(tlist, flags, index_to_scan)
		else:
			self.flags = flags
			self.feid = index_to_scan
			self.tlist = []
			if nim.isCompatible("DVB-C"):
				self.startCableTransponderSearch(self.feid)
			else:
				sat = self.satList[index_to_scan][self.scan_satselection[index_to_scan].index]
				self.startSatelliteTransponderSearch(self.feid, sat[0])

	def setTransponderSearchResult(self, tlist):
		self.tlist = tlist

	def TransponderSearchFinished(self):
		if self.tlist is None:
			self.tlist = []
		else:
			self.startScan(self.tlist, self.flags, self.feid)

	def startScan(self, tlist, flags, feid):
		if len(tlist):
			# flags |= eComponentScan.scanSearchBAT
			if self.finished_cb:
				self.session.openWithCallback(self.finished_cb, ServiceScan, [{"transponders": tlist, "feid": feid, "flags": flags}])
			else:
				self.session.open(ServiceScan, [{"transponders": tlist, "feid": feid, "flags": flags}])
		else:
			if self.finished_cb:
				self.session.openWithCallback(self.finished_cb, MessageBox, _("Nothing to scan!\nPlease setup your tuner settings before you start a service scan."), MessageBox.TYPE_ERROR)
			else:
				self.session.open(MessageBox, _("Nothing to scan!\nPlease setup your tuner settings before you start a service scan."), MessageBox.TYPE_ERROR)

	def keyCancel(self):
		for x in self["config"].list:
			x[1].cancel()
		self.close()

class ScanSimple(ConfigListScreen, Screen, TransponderSearchSupport, CableTransponderSearchSupport):
	def getNetworksForNim(self, nim):
		if nim.isCompatible("DVB-S"):
			networks = nimmanager.getSatListForNim(nim.slot)
		elif not nim.empty:
			networks = [ nim.type ] # "DVB-C" or "DVB-T". TODO: seperate networks for different C/T tuners, if we want to support that.
		else:
			# empty tuners provide no networks.
			networks = [ ]
		return networks

	def __init__(self, session):
		Screen.__init__(self, session)

		self["actions"] = ActionMap(["SetupActions"],
		{
			"ok": self.keyGo,
			"cancel": self.keyCancel,
		}, -2)

		self.list = []
		tlist = []

		known_networks = [ ]
		nims_to_scan = [ ]
		self.finished_cb = None

		for nim in nimmanager.nim_slots:
			# collect networks provided by this tuner

			need_scan = False
			networks = self.getNetworksForNim(nim)
			
			print "nim %d provides" % nim.slot, networks
			print "known:", known_networks

			# we only need to scan on the first tuner which provides a network.
			# this gives the first tuner for each network priority for scanning.
			for x in networks:
				if x not in known_networks:
					need_scan = True
					print x, "not in ", known_networks
					known_networks.append(x)
					
			# don't offer to scan nims if nothing is connected
			if not nimmanager.somethingConnected(nim.slot):
				need_scan = False				

			if need_scan:
				nims_to_scan.append(nim)

		# we save the config elements to use them on keyGo
		self.nim_enable = [ ]

		if len(nims_to_scan):
			self.scan_clearallservices = ConfigSelection(default = "yes", choices = [("no", _("no")), ("yes", _("yes")), ("yes_hold_feeds", _("yes (keep feeds)"))])
			self.list.append(getConfigListEntry(_("Clear before scan"), self.scan_clearallservices))

			for nim in nims_to_scan:
				nimconfig = ConfigYesNo(default = True)
				nimconfig.nim_index = nim.slot
				self.nim_enable.append(nimconfig)
				self.list.append(getConfigListEntry(_("Scan ") + nim.slot_name + " (" + nim.friendly_type + ")", nimconfig))

		ConfigListScreen.__init__(self, self.list)
		self["header"] = Label(_("Automatic Scan"))
		self["footer"] = Label(_("Press OK to scan"))

	def runAsync(self, finished_cb):
		self.finished_cb = finished_cb
		self.keyGo()

	def keyGo(self):
		self.scanList = []
		self.known_networks = set()
		self.nim_iter=0
		self.buildTransponderList()

	def buildTransponderList(self): # this method is called multiple times because of asynchronous stuff
		APPEND_NOW = 0
		SEARCH_CABLE_TRANSPONDERS = 1
		action = APPEND_NOW

		n = self.nim_iter < len(self.nim_enable) and self.nim_enable[self.nim_iter] or None
		self.nim_iter += 1
		if n:
			if n.value: # check if nim is enabled
				flags = 0
				nim = nimmanager.nim_slots[n.nim_index]
				networks = set(self.getNetworksForNim(nim))

				# don't scan anything twice
				networks.discard(self.known_networks)

				tlist = [ ]
				if nim.isCompatible("DVB-S"):
					# get initial transponders for each satellite to be scanned
					for sat in networks:
						getInitialTransponderList(tlist, sat[0])
				elif nim.isCompatible("DVB-C"):
					if config.Nims[nim.slot].cable.scan_type.value == "provider":
						getInitialCableTransponderList(tlist, nim.slot)
					else:
						action = SEARCH_CABLE_TRANSPONDERS
				elif nim.isCompatible("DVB-T"):
					getInitialTerrestrialTransponderList(tlist, nimmanager.getTerrestrialDescription(nim.slot))
				else:
					assert False

				flags |= eComponentScan.scanNetworkSearch #FIXMEEE.. use flags from cables / satellites / terrestrial.xml
				tmp = self.scan_clearallservices.value
				if tmp == "yes":
					flags |= eComponentScan.scanRemoveServices
				elif tmp == "yes_hold_feeds":
					flags |= eComponentScan.scanRemoveServices
					flags |= eComponentScan.scanDontRemoveFeeds

				if action == APPEND_NOW:
					self.scanList.append({"transponders": tlist, "feid": nim.slot, "flags": flags})
				elif action == SEARCH_CABLE_TRANSPONDERS:
					self.flags = flags
					self.feid = nim.slot
					self.startCableTransponderSearch(nim.slot)
					return
				else:
					assert False

			self.buildTransponderList() # recursive call of this function !!!
			return
		# when we are here, then the recursion is finished and all enabled nims are checked
		# so we now start the real transponder scan
		self.startScan(self.scanList)

	def startScan(self, scanList):
		if len(scanList):
			if self.finished_cb:
				self.session.openWithCallback(self.finished_cb, ServiceScan, scanList = scanList)
			else:
				self.session.open(ServiceScan, scanList = scanList)
		else:
			if self.finished_cb:
				self.session.openWithCallback(self.finished_cb, MessageBox, _("Nothing to scan!\nPlease setup your tuner settings before you start a service scan."), MessageBox.TYPE_ERROR)
			else:
				self.session.open(MessageBox, _("Nothing to scan!\nPlease setup your tuner settings before you start a service scan."), MessageBox.TYPE_ERROR)

	def setTransponderSearchResult(self, tlist):
		if tlist is not None:
			self.scanList.append({"transponders": tlist, "feid": self.feid, "flags": self.flags})

	def TransponderSearchFinished(self):
		self.buildTransponderList()

	def keyCancel(self):
		self.close()

	def Satexists(self, tlist, pos):
		for x in tlist:
			if x == pos:
				return 1
		return 0

