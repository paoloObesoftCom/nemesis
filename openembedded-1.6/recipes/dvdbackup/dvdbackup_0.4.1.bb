DESCRIPTION = "libdvdread provides a simple foundation for reading DVD video disks. \
It provides the functionality that is required to access many DVDs. \
It parses IFO files, reads NAV-blocks, and performs CSS authentication and descrambling."
HOMEPAGE = "http://dvdbackup.sourceforge.net/"
LICENSE = "GPL"

DEPENDS = "libdvdread"
RDEPENDS = "libdvdread"

PV = "0.4.1"
PR = "r2"

SRC_URI = "http://downloads.sourceforge.net/dvdbackup/dvdbackup-0.4.1.tar.bz2"

inherit autotools

EXTRA_OECONF = ""

EXTRA_OECONF_opendreambox = ""

do_stage() {
	autotools_stage_all
}

