PN = "enigma2-nemesis"
PR = "r0"
PV = "2.4"

SRC_URI = "file://../../../../sources/enigma2.git_Nemesis_16.tar.gz"

S = "${WORKDIR}/git"

FILES_${PN} += "${datadir}/fonts"
FILES_${PN}-meta = "${datadir}/meta"
PACKAGES += "${PN}-meta"
PACKAGE_ARCH = "nemesis"

inherit autotools pkgconfig

EXTRA_OECONF = " \
        BUILD_SYS=${BUILD_SYS} \
        HOST_SYS=${HOST_SYS} \
        STAGING_INCDIR=${STAGING_INCDIR} \
        STAGING_LIBDIR=${STAGING_LIBDIR} \
"
pkg_preinst_${PN} () {
	if [ "x$D" != "x" ]; then
		exit 1
	fi
}

pkg_postinst_${PN} () {
	if [ "x$D" != "x" ]; then
		exit 1
	fi
}
