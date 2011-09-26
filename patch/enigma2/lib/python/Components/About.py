from Tools.Directories import resolveFilename, SCOPE_SYSETC
from enigma import getEnigmaVersionString
from os import popen

class About:
	def __init__(self):
		pass

	def getVersionString(self):
		return self.getImageVersionString()

	def getImageVersionString(self):
		try:
			file = open(resolveFilename(SCOPE_SYSETC, 'image-version'), 'r')
			lines = file.readlines()
			for x in lines:
				splitted = x.split('=')
				if splitted[0] == "version":
					#     YYYY MM DD hh mm
					#0120 2005 11 29 01 16
					#0123 4567 89 01 23 45
					version = splitted[1]
					image_type = version[0] # 0 = release, 1 = experimental
					svn = version[1:6]
					year = version[6:10]
					month = version[10:12]
					day = version[12:14]
					date = '-'.join((day, month, year))
					if image_type == '0':
						image_type = "Release"
						return ' '.join((image_type, "SVN(%s)," % (svn), date))
					else:
						image_type = "Experimental"
						return ' '.join(("Release SVN(%s)," % (svn), date))
			file.close()
		except IOError:
			pass

	def getSvnVersionString(self):
		try:
			file = open(resolveFilename(SCOPE_SYSETC, 'image-version'), 'r')
			lines = file.readlines()
			for x in lines:
				splitted = x.split('=')
				if splitted[0] == "version":
					return splitted[1][1:4]+splitted[1][6]
			file.close()
		except IOError:
			pass

		return "unavailable"

	def getEnigmaVersionString(self):
		return getEnigmaVersionString()

	def getKernelVersionString(self):
		try:
			result = popen("uname -r","r").read().strip("\n").split('-')
			kernel_version = result[0]
			return kernel_version
		except:
			pass

		return "unknown"

	def getKernelVersionStringL(self):
		try:
			result = popen("uname -r","r").read().strip("\n")
			kernel_version = result
			return kernel_version
		except:
			pass

		return "unknown"

about = About()
