DESCRIPTION = "LCN scanner for DVB-T services"
DEPENDS = "python"
LICENSE = "GPLv2"

PN = "enigma2-plugin-systemplugins-lcnscanner"

SRCDATE = "20110306"
BRANCH = "master"
PV = "2.0-git${SRCDATE}"
SRCREV = "8bc72766a0af5904f3d9386a123a163e7adf55dd"
PR = "r1"

SRC_URI = "git://openee.git.sourceforge.net/gitroot/openee/lcnscanner;protocol=git;branch=${BRANCH};tag=${SRCREV}"
S = "${WORKDIR}/git"

FILES_${PN} = "/usr/*"

do_install() {
	oe_runmake 'D=${D}' install
}
