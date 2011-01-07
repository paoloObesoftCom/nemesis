DESCRIPTION = "OpenDreambox: Nemesis task for the OpenDreambox Distribution"
SECTION = "opendreambox/base"
LICENSE = "MIT"
PR = "r03"

inherit task

PACKAGES = "\
  task-opendreambox-nemesis \
"

#
# task-opendreambox-nemesis
#
DESCRIPTION_task-opendreambox-nemesis = "OpenDreambox: Nemesis Dependencies"
RDEPENDS_task-opendreambox-nemesis = "\
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

RRECOMMENDS_task-opendreambox-nemesis_append_dm800 = "\
	dvdbackup \
	task-opendreambox-dvdplayer \
	dreambox-tuner-usb \
"

RRECOMMENDS_task-opendreambox-nemesis_append_dm800se = "\
	dvdbackup \
	task-opendreambox-dvdplayer \
	dreambox-tuner-usb \
"

RRECOMMENDS_task-opendreambox-nemesis_append_dm500hd = "\
	dvdbackup \
	task-opendreambox-dvdplayer \
	dreambox-tuner-usb \
"

RRECOMMENDS_task-opendreambox-nemesis_append_dm7020hd = "\
	dvdbackup \
	task-opendreambox-dvdplayer \
	dreambox-tuner-usb \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
