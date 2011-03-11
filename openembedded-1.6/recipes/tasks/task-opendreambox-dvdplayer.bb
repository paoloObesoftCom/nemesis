DESCRIPTION = "OpenDreambox: DVD-Player Task for the OpenDreambox Distribution"
SECTION = "opendreambox/base"
LICENSE = "MIT"
PV = "2.2"
PR = "r1"

inherit task

#
# task-opendreambox-dvdplayer
#
DESCRIPTION_${PN} = "OpenDreambox: DVD-Player Support"
DEPENDS = "enigma2"
RDEPENDS_${PN} = "\
  dvdbackup \
  libdvdread \
  cdrkit \
  kernel-module-udf \
  kernel-module-isofs \
  enigma2-plugin-extensions-dvdplayer \
"

