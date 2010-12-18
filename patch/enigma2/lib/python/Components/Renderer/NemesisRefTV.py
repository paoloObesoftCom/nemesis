#######################################################################
#
#    Converter for Nemesis
#    Coded by shamann (c)2010
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#    
#######################################################################

from Tools.LoadPixmap import LoadPixmap
from Components.Pixmap import Pixmap
from Renderer import Renderer
from enigma import ePixmap, eEnv
from Tools.Directories import fileExists, SCOPE_SKIN_IMAGE, SCOPE_CURRENT_SKIN, resolveFilename
from Components.config import *
from enigma import eServiceReference, eServiceCenter, iPlayableService, iServiceInformation

class NemesisRefTV(Renderer):
	if config.nemesis.usepiconinhdd.value:
		searchPaths = (eEnv.resolve('${datadir}/enigma2/%s/'),'/media/cf/%s/','/media/usb/%s/','/media/hdd/%s/')
	else:
		searchPaths = (eEnv.resolve('${datadir}/enigma2/%s/'),'/media/cf/%s/','/media/usb/%s/')

	def __init__(self):
		Renderer.__init__(self)
		self.path = "picon"
		self.nameCache = { }
		self.pngname = ""

	def applySkin(self, desktop, parent):
		attribs = [ ]
		for (attrib, value) in self.skinAttributes:
			if attrib == "path":
				self.path = value
			else:
				attribs.append((attrib,value))
		self.skinAttributes = attribs
		return Renderer.applySkin(self, desktop, parent)

	GUI_WIDGET = ePixmap

	def changed(self, what):
		if self.instance:
			pngname = ""
			if what[0] != self.CHANGED_CLEAR:
				service = self.source.service
				sname = service.toString()
				pos = sname.rfind(':')
				if pos != -1:
					sname = sname[:pos].rstrip(':').replace(':','_')
				pngname = self.nameCache.get(sname, "")
				if pngname == "":
					pngname = self.findPicon(sname)
					if pngname != "":
						self.nameCache[sname] = pngname
			if pngname == "":
				pngname = self.nameCache.get("default", "")
				if pngname == "":
					pngname = self.findPicon("picon_default")
					if pngname == "":
						tmp = resolveFilename(SCOPE_CURRENT_SKIN, "picon_default.png")
						if fileExists(tmp):
							pngname = tmp
						else:
							pngname = resolveFilename(SCOPE_SKIN_IMAGE, "skin_default/picon_default.png")
					self.nameCache["default"] = pngname
			if self.pngname != pngname:
				self.pngname = pngname
				self.instance.setPixmapFromFile(self.pngname)
            		
	def findPicon(self, serviceName):
		for path in self.searchPaths:
			pngname = (path % self.path) + serviceName + ".png"
			if fileExists(pngname):
				return pngname
		return ""
