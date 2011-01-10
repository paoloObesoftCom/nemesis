DESCRIPTION = "OpenDreambox: Nemesis task for the OpenDreambox Distribution"
SECTION = "opendreambox/base"
LICENSE = "MIT"
PR = "r07"

inherit task

PACKAGES = "\
  task-opendreambox-nemesis \
"

#
# task-opendreambox-nemesis
#
DESCRIPTION_task-opendreambox-nemesis = "OpenDreambox: Nemesis Dependencies"
DEPENDS_task-opendreambox-nemesis = "\
	task-opendreambox-cdplayer \
	task-opendreambox-dvdplayer \
	task-opendreambox-dvdburn \
	dreambox-tuner-usb \
"
RDEPENDS_task-opendreambox-nemesis = "\
	dvdbackup \
	ccid \
	nfs-utils \
	usbutils \
	openvpn \
	ez-ipupdate \
	bzip2 \
	libcap \
	python-gdata \
	dreambox-nemesis \
	nemesis-version \
"

RRECOMMENDS_task-opendreambox-nemesis = "\
	enigma2-plugin-extensions-stayup \
	enigma2-plugin-systemplugins-crossepg \
	enigma2-plugin-extensions-audiosync \
	enigma2-plugin-systemplugins-networkbrowser \
	enigma2-plugin-extensions-epgsearch \
	enigma2-plugin-extensions-easymedia \
	enigma2-plugin-extensions-socketmmi \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
