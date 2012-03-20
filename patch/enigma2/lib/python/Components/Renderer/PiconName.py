##
## Picon renderer by Gruffy .. some speedups by Ghost
##
from Components.config import config
from Renderer import Renderer
from enigma import ePixmap, eEnv, ePicLoad
from Tools.Directories import fileExists, SCOPE_SKIN_IMAGE, SCOPE_CURRENT_SKIN, resolveFilename
from Nemesis.nemesisTool import GetPiconPath

class PiconName(Renderer):
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
				sname = self.source.text.upper()
				sname = sname.replace('\xc2\x86', '').replace('\xc2\x87', '')
				pngname = self.nameCache.get(sname, "")
				if pngname == "":
					pngname = self.findPicon(sname)
			if pngname == "": # no picon for service found
				pngname = self.nameCache.get("default", "")
				if pngname == "": # no default yet in cache..
					pngname = self.findPicon("picon_default")
					if pngname == "":
						tmp = resolveFilename(SCOPE_CURRENT_SKIN, "picon_default.png")
						if fileExists(tmp):
							pngname = tmp
						else:
							pngname = resolveFilename(SCOPE_SKIN_IMAGE, "skin_default/picon_default.png")
					self.nameCache["default"] = pngname
			if self.pngname != pngname:
				# setScale doesn't work very well, so we work around it
				#self.picload = ePicLoad()
				#self.picload.PictureData.get().append(self.piconShow)
				#self.picload.setPara((self.instance.size().width(), self.instance.size().height(), 1, 1, False, 1, "#00000000"))
				#self.picload.startDecode(pngname)
				self.instance.setPixmapFromFile(pngname)
				self.pngname = pngname

	#def piconShow(self, picInfo=None):
		#ptr = self.picload.getData()
		#if ptr != None:
			#self.instance.setPixmap(ptr.__deref__())
		#del self.picload

	def findPicon(self, serviceName):

		searchPaths = GetPiconPath()

		for path in searchPaths:
			pngname = (path % self.path) + serviceName + ".png"
			if fileExists(pngname):
				return pngname
		return ""
