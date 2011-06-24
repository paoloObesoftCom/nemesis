DESCRIPTION = "LCN scanner for DVB-T services"
DEPENDS = "python"
LICENSE = "GPLv2"

PN = "enigma2-plugin-systemplugins-lcnscanner"

SRCDATE = "20110615"
BRANCH = "master"
PV = "2.0-git${SRCDATE}"
SRCREV = "02b7aca32693015f2977ad34c887eed3d49c36c3"
PR = "r1"

SRC_URI = "git://openee.git.sourceforge.net/gitroot/openee/lcnscanner;protocol=git;branch=${BRANCH};tag=${SRCREV}"
S = "${WORKDIR}/git"

FILES_${PN} = "/usr/*"

do_install() {
	oe_runmake 'D=${D}' install
}
