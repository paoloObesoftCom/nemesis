DESCRIPTION = "Kernel based automounter for linux."
SECTION = "base"
LICENSE = "GPL"

PR="r4"

SRC_URI = "ftp://ftp.kernel.org/pub/linux/daemons/autofs/v4/autofs-${PV}.tar.bz2 \
           ftp://ftp.kernel.org/pub/linux/daemons/autofs/v4/autofs-4.1.4-misc-fixes.patch;patch=1 \
           ftp://ftp.kernel.org/pub/linux/daemons/autofs/v4/autofs-4.1.4-multi-parse-fix.patch;patch=1 \
           ftp://ftp.kernel.org/pub/linux/daemons/autofs/v4/autofs-4.1.4-non-replicated-ping.patch;patch=1 \
           ftp://ftp.kernel.org/pub/linux/daemons/autofs/v4/autofs-4.1.4-locking-fix-1.patch;patch=1 \
           file://cross.patch;patch=1 \
           file://Makefile.rules-cross.patch;patch=1 \
	   			 file://install.patch;patch=1 \
           file://autofs-add-hotplug.patch;patch=1 \
           file://auto.master file://auto.network \
	   			 file://autofs"

inherit autotools pkgconfig update-rc.d

EXTRA_OEMAKE="TARGET_PREFIX=${TARGET_PREFIX}"
PARALLEL_MAKE = ""

INITSCRIPT_PACKAGES = "autofs"
INITSCRIPT_NAME = "autofs"
INITSCRIPT_PARAMS = "defaults 21 19"


do_configure_prepend () {
	if [ ! -e acinclude.m4 ]; then
		cp aclocal.m4 acinclude.m4
	fi
}

do_install () {
	oe_runmake 'INSTALLROOT=${D}' install
	install -d ${D}${sysconfdir}/init.d
	install ${WORKDIR}/autofs ${D}${sysconfdir}/init.d
	install ${WORKDIR}/auto.master ${D}${sysconfdir}/auto.master
	install ${WORKDIR}/auto.network ${D}${sysconfdir}/auto.network
	if [ "${MACHINE}" = "dm500plus" -o "${MACHINE}" = "dm600pvr" -o "${MACHINE}" = "dm7020" ]; then
		rm -rf ${D}/etc/init.d
		rm -rf ${D}/var
		rm -f ${D}/etc/auto*
	fi
}

