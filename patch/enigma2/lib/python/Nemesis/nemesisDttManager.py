from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from Screens.Standby import TryQuitMainloop
from Components.Label import Label
from Components.ActionMap import ActionMap
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmapAlphaTest
from Tools.LoadPixmap import LoadPixmap
from Tools.Directories import fileExists
from nemesisTool import ListboxE1, GetSkinPath
from nemesisConsole import nemesisConsole
from os import path as os_path, listdir, unlink
import os

def readFile(myFile):
	try:
		mounts = open(myFile)
	except IOError:
		return 'None'
	
	line = mounts.readlines()
	mounts.close()
	return line[0].strip()

def checkDev():
	tunerList = [
		('2137:0001','as102','Super Digi KEY'),
		('07ca:a867','a867','AVerMedia A867 USB DVB-T'),
		('07ca:a800','a800','a8001'),
		('07ca:a801','a800','a8002'),
		('15a4:9020','af9005','af90051'),
		('0ccd:0055','af9005','af90052'),
		('10b9:6000','af9005','Ansonic'),
		('15a4:9015','af9015','Afatech AF9015'),
		('15a4:9016','af9015','Geniatech T328B'),
		('0413:6029','af9015','Leadtek WinFast DTV Dongle Gold'),
		('2304:022b','af9015','Pinnacle PCTV 71e'),
		('1b80:e399','af9015','KWorld USB Dual (399U)'),
		('13d3:3226','af9015','DigitalNow TinyTwin'),
		('13d3:3237','af9015','TwinHan AzureWave AD-TU700(704J)'),
		('0ccd:0069','af9015','TerraTec Cinergy T XE (rev2)'),
		('1b80:c160','af9015','af901512'),
		('07ca:a815','af9015','AVerMedia AVerTV Digi Volar X (A815)'),
		('1ae7:0381','af9015','Xtensions XD-380'),
		('1462:8801','af9015','MSI DIGIVOX Duo'),
		('07ca:8150','af9015','Fujitsu-Siemens Slim Mobile'),
		('10b9:8000','af9015','Telstar Starstick 2'),
		('07ca:a309','af9015','AVerMedia A309 Mini Card'),
		('1462:8807','af9015','MSI DIGIVOX mini III'),
		('1b80:e396','af9015','KWorld USB DVB-T TV 395U'),
		('1b80:e39b','af9015','af901521'),
		('1b80:e395','af9015','af901522'),
		('15a4:901b','af9015','af901523'),
		('07ca:850a','af9015','AVerTV Volar Black HD (A850)'),
		('07ca:a805','af9015','AVerMedia AVerTV Volar GPS 805 (A805)'),
		('1b80:e397','af9015','Conceptronic CTVDIGRCU V3.0'),
		('1b80:c810','af9015','KWorld Digial MC-810'),
		('0458:4012','af9015','af901528'),
		('1b80:e400','af9015','af901529'),
		('1b80:c161','af9015','af901530'),
		('1b80:e39d','af9015','af901531'),
		('1b80:e402','af9015','af901532'),
		('0413:6a04','af9015','af901533'),
		('1b80:e383','af9015','af901534'),
		('1b80:e39a','af9015','af901535'),
		('0ccd:0097','af9015','Terratec TerraTec HD MKII'),
		('15a4:1000','af9035','af90351'),
		('15a4:1001','af9035','Afatech EzCap'),
		('15a4:1002','af9035','af90353'),
		('15a4:1003','af9035','af90354'),
		('15a4:9035','af9035','af90355'),
		('0ccd:0093','af9035','af90356'),
		('07ca:0825','af9035','af90357'),
		('07ca:a867','a867','af90358'),
		('058f:6610','au6610','Sigmatek DVB-110'),
		('07ca:a310','ce6230','AVerMedia A310'),
		('8086:9500','ce6230','Intel CE9500 / CE6230'),
		('0ccd:0038','cinergyT2','cinergyT2'),
		('10b8:1e14','dib0700','DiBcom STK7700P'),
		('10b8:1e78','dib0700','dib07001'),
		('2040:9941','dib0700','Hauppauge Nova-T 500 Dual'),
		('2040:9950','dib0700','Hauppauge Nova-T 500 Dual'),
		('2040:7050','dib0700','Hauppauge Nova-T Stick'),
		('07ca:a807','dib0700','AVerMedia AVerTV Volar'),
		('185b:1e78','dib0700','Compro Videomate U500'),
		('1584:6003','dib0700','Uniwill STK7700P'),
		('0413:6f00','dib0700','Leadtek Winfast Dongle (STK7700P)'),
		('2040:7060','dib0700','Hauppauge Nova-TD Stick'),
		('07ca:b808','dib0700','AVerMedia AVerTV DVB-T Volar'),
		('2304:022c','dib0700','Pinnacle PCTV 2000e'),
		('0ccd:005a','dib0700','TerraTec Cinergy DT XS Diversity'),
		('2040:9580','dib0700','Hauppauge Nova-TD Stick'),
		('10b8:1ef0','dib0700','DiBcom STK7700D reference design'),
		('10b8:1ebc','dib0700','DiBcom STK7070P reference design'),
		('2304:0228','dib0700','Pinnacle PCTV DVB-T Flash Stick'),
		('10b8:1ebe','dib0700','DiBcom STK7070PD reference design'),
		('2304:0229','dib0700','Pinnacle PCTV 2001e Dual'),
		('185b:1e80','dib0700','Compro Videomate U500'),
		('07ca:b568','dib0700','AVerMedia AVerTV DVB-T Expres'),
		('1044:7001','dib0700','Gigabyte U7000'),
		('05d8:810f','dib0700','Artec T14BR DVB-T'),
		('0b05:171f','dib0700','ASUS My Cinema U3000'),
		('0b05:173f','dib0700','dib070024'),
		('2040:7070','dib0700','Hauppauge Nova-T-CE Stick'),
		('2040:7080','dib0700','Hauppauge Nova-T MyTV.t Stick'),
		('0ccd:0058','dib0700','TerraTec Cinergy HT USB XE'),
		('2304:022e','dib0700','Pinnacle Expresscard 320cx'),
		('2304:0236','dib0700','Pinnacle PCTV 72e'),
		('2304:0237','dib0700','Pinnacle PCTV 73e'),
		('1164:1edc','dib0700','YUAN High-Tech EC372S'),
		('0ccd:0060','dib0700','TerraTec Cinergy HT Express'),
		('0ccd:0078','dib0700','TerraTec Cinergy T USB XXS/ T3'),
		('0413:6f01','dib0700','Leadtek Winfast Dongle (STK7700P)'),
		('2040:5200','dib0700','Hauppauge Nova-TD Stick (52009)'),
		('2040:8400','dib0700','dib070036'),
		('1044:7002','dib0700','Gigabyte U8000-RH'),
		('1164:1f08','dib0700','YUAN High-Tech STK7700PH'),
		('0b05:1736','dib0700','ASUS My Cinema-U3000Hybrid'),
		('2304:023a','dib0700','Pinnacle PCTV HD Pro Stick (801e)'),
		('2304:023b','dib0700','Pinnacle PCTV HD Stick (801e SE)'),
		('0ccd:0062','dib0700','TerraTec Cinergy T Express'),
		('0ccd:0081','dib0700','TerraTec Cinergy DT XS Diversity/T5'),
		('1415:0003','dib0700','Sony PlayTV'),
		('1164:2edc','dib0700','YUAN High-Tech PD378S'),
		('2040:b200','dib0700','dib070046'),
		('2040:b210','dib0700','dib070047'),
		('1164:0871','dib0700','YUAN High-Tech MC770'),
		('0fd9:0021','dib0700','Elgato EyeTV DTT'),
		('0fd9:0020','dib0700','Elgato EyeTV DTT deluxe (PD378S)'),
		('0413:60f6','dib0700','Leadtek WinFast DTV Dongle H'),
		('0ccd:10a0','dib0700','dib070052'),
		('0ccd:10a1','dib0700','dib070053'),
		('1164:1efc','dib0700','YUAN High-Tech STK7700D'),
		('1164:1e8c','dib0700','YUAN High-Tech DiBcom STK7700D'),
		('2304:0243','dib0700','Pinnacle PCTV 73A'),
		('2013:0245','dib0700','Pinnacle PCTV 73e SE'),
		('2013:0248','dib0700','Pinnacle PCTV 282e'),
		('10b8:1e80','dib0700','DiBcom STK7770P reference design'),
		('0ccd:00ab','dib0700','TerraTec Cinergy T USB XXS (HD)'),
		('10b8:1f98','dib0700','DiBcom STK807xPVR reference design'),
		('10b8:1f90','dib0700','DiBcom STK807xP reference design'),
		('1554:5010','dib0700','Prolink Pixelview SBTVD'),
		('1e59:0002','dib0700','dib070064'),
		('2304:0245','dib0700','dib070065'),
		('2304:0248','dib0700','dib070066'),
		('10b8:1fa0','dib0700','dib070067'),
		('14aa:0001','dibusb-mb','AVerMedia AVerTV'),
		('14aa:0002','dibusb-mb','dibusb-mb2'),
		('185b:d000','dibusb-mb','Compro Videomate DVB-U2000'),
		('185b:d001','dibusb-mb','dibusb-mb4'),
		('145f:010c','dibusb-mb','dibusb-mb5'),
		('10b8:0bb8','dibusb-mb','DiBcom (MOD3000)'),
		('10b8:0bb9','dibusb-mb','dibusb-mb7'),
		('eb1a:17de','dibusb-mb','KWorld V-Stream XPERT DTV'),
		('eb1a:17df','dibusb-mb','dibusb-mb9'),
		('5032:0fa0','dibusb-mb','Grandtec USB1.1 DVB-T'),
		('5032:0fa1','dibusb-mb','dibusb-mb11'),
		('5032:0bb8','dibusb-mb','dibusb-mb12'),
		('5032:0bb9','dibusb-mb','dibusb-mb13'),
		('1025:005e','dibusb-mb','dibusb-mb14'),
		('1025:005f','dibusb-mb','dibusb-mb15'),
		('13d3:3201','dibusb-mb','Twinhan TwinhanDTV (VP7041)/(VP7041e)'),
		('13d3:3202','dibusb-mb','dibusb-mb17'),
		('1822:3201','dibusb-mb','dibusb-mb18'),
		('1822:3202','dibusb-mb','dibusb-mb19'),
		('05d8:8105','dibusb-mb','Artec T1 USB1.1 TVBOX with AN2135'),
		('05d8:8106','dibusb-mb','dibusb-mb21'),
		('05d8:8107','dibusb-mb','Artec T1 USB1.1 TVBOX with AN2235'),
		('05d8:8108','dibusb-mb','dibusb-mb23'),
		('06e1:a333','dibusb-mb','KWorld 300U/Instant PTV-333 '),
		('0547:2235','dibusb-mb','dibusb-mb25'),
		('10b8:0bc6','dibusb-mc','DiBcom (MOD3000P)'),
		('10b8:0bc7','dibusb-mc','dibusb-mc2'),
		('05d8:8109','dibusb-mc','Artec T1 USB2.0'),
		('05d8:810a','dibusb-mc','dibusb-mc4'),
		('04ca:f000','dibusb-mc','LITE-ON'),
		('04ca:f001','dibusb-mc','dibusb-mc6'),
		('eb1a:e360','dibusb-mc','MSI Digivox Mini SL'),
		('eb1a:e361','dibusb-mc','dibusb-mc8'),
		('5032:0bc6','dibusb-mc','GRAND USB2.0 DVB-T adapter'),
		('5032:0bc7','dibusb-mc','dibusb-mc10'),
		('05d8:810b','dibusb-mc','Artec T14'),
		('05d8:810c','dibusb-mc','dibusb-mc12'),
		('0413:6025','dibusb-mc','Leadtek USB2.0 Winfast (STK3000P)'),
		('0413:6026','dibusb-mc','dibusb-mc14'),
		('10b9:5000','dibusb-mc','dibusb-mc15'),
		('10b9:5001','dibusb-mc','dibusb-mc16'),
		('14aa:0201','dtt200u','WideView/YUAN/Yakumo/Hama/Typhoon (WT-200U)'),
		('14aa:0301','dtt200u','dtt200u2'),
		('14aa:0222','dtt200u','WideView WT-220U (Typhoon/Freecom)'),
		('14aa:0221','dtt200u','dtt200u4'),
		('14aa:022a','dtt200u','WideView WT-220U (ZL353)'),
		('14aa:022b','dtt200u','dtt200u6'),
		('14aa:0225','dtt200u','Freecom DVB-T (WT-220U)'),
		('14aa:0226','dtt200u','dtt200u8'),
		('14aa:0220','dtt200u','dtt200u9'),
		('18f3:0220','dtt200u','Miglia WT-220U'),
		('06be:a232','dtv5100','AME DTV-5100'),
		('18b4:1689','ec168','ec1681'),
		('18b4:fffa','ec168','ec1682'),
		('18b4:fffb','ec168','ec1683'),
		('18b4:1001','ec168','ec1684'),
		('18b4:1002','ec168','ec1685'),
		('0db0:5581','gl861','MSI Mega Sky 55801'),
		('05e3:f170','gl861','A-LINK DTU'),
		('2040:9300','nova-t-usb2','Hauppauge WinTV-NOVA-T'),
		('2040:9301','nova-t-usb2','nova-t-usb22'),
		('187f:0010','smsusb','smsusb1'),
		('187f:0100','smsusb','smsusb2'),
		('187f:0200','smsusb','smsusb3'),
		('187f:0201','smsusb','smsusb4'),
		('187f:0300','smsusb','smsusb5'),
		('2040:1700','smsusb','smsusb6'),
		('2040:1800','smsusb','smsusb7'),
		('2040:1801','smsusb','smsusb8'),
		('2040:5500','smsusb','smsusb9'),
		('2040:5510','smsusb','smsusb10'),
		('2040:5520','smsusb','smsusb11'),
		('2040:5530','smsusb','smsusb12'),
		('2040:5580','smsusb','smsusb13'),
		('2040:5590','smsusb','smsusb14'),
		('2040:b900','smsusb','smsusb15'),
		('2040:b910','smsusb','smsusb16'),
		('2040:b980','smsusb','smsusb17'),
		('2040:b990','smsusb','smsusb18'),
		('2040:c000','smsusb','Hauppauge Mini Stick (1248)'),
		('2040:c010','smsusb','smsusb20'),
		('2040:c080','smsusb','smsusb21'),
		('2040:c090','smsusb','smsusb22'),
		('13d3:3205','vp7045','Fujitsu-Siemens TV Tuner'),
		('13d3:3206','vp7045','vp70452'),
		('13d3:3223','vp7045','vp70453'),
		('13d3:3224','vp7045','vp70454'),
	]

	deviceDir = "/sys/bus/usb/devices/"
	devDir = listdir(deviceDir)
	myDevices = []
	for x in devDir[:]:
		idv = 'None'
		idp = 'None'
		if os_path.isdir(deviceDir + x):
			devFiles = listdir(deviceDir + x)
			for y in devFiles[:]:
				if os_path.isfile(deviceDir + x + "/" + y):
					if y == "idVendor":
						idv = readFile(deviceDir + x + "/" + y)
					if y == "idProduct":
						idp = readFile(deviceDir + x + "/" + y)
			myDevices.append("%s:%s" % (idv,idp))
	
	if myDevices:
		devicesList = []
		for myDevice in myDevices:
			for myTuner in tunerList:
				if myDevice == myTuner[0]:
					devicesList.append(myTuner)
		if devicesList:
			return devicesList
		else:
			return None
	else:
		return None

class manageDttDevice(Screen):
	__module__ = __name__
	
	skin = """
		<screen position="80,95" size="560,430">
			<widget name="title" position="10,5" size="320,55" font="Regular;28" foregroundColor="#ff2525" backgroundColor="transpBlack" transparent="1"/>
			<widget name="list" position="10,65" size="540,340" scrollbarMode="showOnDemand" />
			<widget name="key_red" position="0,510" size="280,20" zPosition="1" font="Regular;22" valign="center" foregroundColor="#0064c7" backgroundColor="#9f1313" transparent="1" />
		</screen>"""

	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		self.devList = checkDev()
		self.devstatus = {}
		self["title"] = Label(_("Manage Dtt Adapter"))
		self['list'] = ListboxE1(self.list)
		self["key_red"] = Label(_("Exit"))
		self['actions'] = ActionMap(['WizardActions','ColorActions'],
		{
			'ok': self.KeyOk,
			"red": self.close,
			'back': self.close
		})
		self.onLayoutFinish.append(self.updateList)
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(_("Manage DVB-T/C Adapter"))
		
	def KeyOk(self):
		if self.devList:
			sel = self["list"].getCurrent()[0]
			if (sel):
				self.devstatus[sel] = not self.devstatus[sel]
				self.saveStatus()
				self.updateList()
				if self.devstatus[sel]:
					msg = _('Enigma2 will be now hard restarted to enable DVB/T.please wait\nDo You want restart now?')
					box = self.session.openWithCallback(self.restartEnigma2, MessageBox, msg , MessageBox.TYPE_YESNO)
					box.setTitle(_('Restart Enigma2'))
				else:
					msg = _('Dreambox will be now rebooted to disable DVB/T.\nDo You want reboot the box now?')
					box = self.session.openWithCallback(self.rebootDream, MessageBox, msg , MessageBox.TYPE_YESNO)
					box.setTitle(_('Reboot Dreambox'))
		else:
			self.close()

	def restartEnigma2(self, answer):
		if (answer is True):
			self.session.open(TryQuitMainloop, 3)
	
	def rebootDream(self, answer):
		if (answer is True):
			self.session.open(TryQuitMainloop, 2)

	def readStatus(self):
		try:
			if self.devList:
				for x in self.devList:
					self.devstatus[x[1]] = False
				if fileExists('/etc/dtt.devices'):
					loadedDevices = open('/etc/dtt.devices', 'r')
					for d in loadedDevices.readlines():
						for dev in self.devList:
							if dev[1] == d[:-1]:
								self.devstatus[dev[1]] = True
					loadedDevices.close()
			else:
				if fileExists('/etc/dtt.devices'):
					unlink("/etc/dtt.devices")
				return None
		except IOError:
			return None
	
	def saveStatus(self):
		out = open('/etc/dtt.devices', 'w')
		for dev in self.devList:
			if self.devstatus.get(dev[1]):
				out.write(dev[1] + '\n')
		out.close()
	
	def updateList(self):
		self.readStatus()
		del self.list[:]
		skin_path = GetSkinPath()
		if self.devList:
			for dev in self.devList:
				res = [dev[1]]
				res.append(MultiContentEntryText(pos=(0, 5), size=(400, 32), font=0, text=dev[2]))
				png = LoadPixmap({ True:skin_path + 'menu/menu_on.png',False:skin_path + 'menu/menu_off.png' }[self.devstatus.get(dev[1])])
				res.append(MultiContentEntryPixmapAlphaTest(pos=(420, 6), size=(80, 23), png=png))
				self.list.append(res)
			self['list'].l.setList(self.list)
