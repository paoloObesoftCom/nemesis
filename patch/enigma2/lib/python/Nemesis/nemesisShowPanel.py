from Screens.Screen import Screen
from Components.Sources.List import List
from Components.ActionMap import ActionMap, NumberActionMap
from Components.Label import Label
from Components.Sources.StaticText import StaticText

class nemesisShowPanel(Screen):
	__module__ = __name__

	def __init__(self, session, file , Wtitle, text = None):
		Screen.__init__(self, session)
		
		self.file = file
		self.text = text
		self.Wtitle = Wtitle
		self.list = []
		self["close"] = Label(_("Close"))
		self["abouttxt"] = Label()
		self["conn"] = StaticText('')
		self["list"] = List()

		self['actions'] = ActionMap(['WizardActions', 'ColorActions'], {
			'red': self.close,
			'ok': self.close,
			'back': self.close
			})
		
		if self.file: 
			self.loadData()
			self["abouttxt"].hide()
			self["list"] = List(self.list)
			self["list"].onSelectionChanged.append(self.openDetails)
			self.onLayoutFinish.append(self.openDetails)
		else:
			if self.text: 
				self["abouttxt"].setText(self.text)
		
		self.onShown.append(self.setWindowTitle)
	
	def setWindowTitle(self):
		self.setTitle(self.Wtitle)
	
	def loadData(self):
		try:
			f = open(self.file, 'r')
			for line in f.readlines():
				self.list.append(("0",line))
			f.close()
		except:
			mess = _('File: %s not found!') % self.file
			self.list.append(("0",mess))
		self['list'].setList(self.list)

	def openDetails(self):
		if self['list'].count() > 0:
			self["conn"].text = self["list"].getCurrent()[1]