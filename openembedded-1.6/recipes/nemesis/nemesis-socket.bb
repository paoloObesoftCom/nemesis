DESCRIPTION = "Nemesis socket"
HOMEPAGE = http://www.genesi-project.com
LICENSE = "GPLv2"
MAINTAINER = Gianathem

PR = "r1"
PV = "2.6"

SRC_URI = "file://nemesisd.c \
           file://nemesisc.c \
           file://nemesis.cfg \
           file://Makefile"

S = "${WORKDIR}"
PACKAGE_ARCH := "${MACHINE_ARCH}"
FILES_${PN} = "/*"

RCONFLICTS = "nemesisc nemesisd nemesis.cfg"
RREPLACES = "nemesisc nemesisd nemesis.cfg"

EXTRA_OEMAKE = "KSRC=${STAGING_KERNEL_DIR}"

do_compile() {
	unset CPPFLAGS CXXFLAGS CPP
	oe_runmake
}

do_install() {
	oe_runmake 'D=${D}' install
}
