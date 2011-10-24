DESCRIPTION = "Read OpenTV epg from sat or import from xmltv or csv"
HOMEPAGE = http://www.crossepg.com
DEPENDS = "libxml2 zlib python"
LICENSE = "GPLv2"

PN = "enigma2-plugin-systemplugins-crossepg"

SRCDATE = "20111020"
BRANCH = "master"
PV = "2.3-git${SRCDATE}"
SRCREV = "6121f90aa1ed6891f6551c6b1de8655ccac713ec"
PR = "r1"

SRC_URI = "git://github.com/E2OpenPlugins/e2openplugin-CrossEPG.git;protocol=git;branch=${BRANCH};tag=${SRCREV} \
           file://download_epg.sh \
           file://convert_epg.sh \
           file://mpepg.bin \
           file://mpepg.conf \
           file://crossepg.config"

S = "${WORKDIR}/git"
PACKAGE_ARCH := "${MACHINE_ARCH}"

FILES_${PN} = "/usr/*"

CFLAGS_append = " -I${STAGING_INCDIR}/libxml2/ -I${STAGING_INCDIR}/python2.6/"

RCONFLICTS_${PN} = "enigma2-plugin-systemplugins-crossepg"
RREPLACES_${PN} = "enigma2-plugin-systemplugins-crossepg"

do_compile() {
	oe_runmake SWIG="swig"
}

do_install() {
	oe_runmake 'D=${D}' install
}

do_install_append () {
	install -d "${D}/usr/crossepg"
	install -d "${D}/usr/crossepg/import"
	install -d "${D}/usr/crossepg/providers"
	install -m 755 "${WORKDIR}/download_epg.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/convert_epg.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/mpepg.bin" "${D}/usr/crossepg/import"
	install -m 644 "${WORKDIR}/crossepg.config" "${D}/usr/crossepg"
	install -m 644 "${WORKDIR}/mpepg.conf" "${D}/usr/crossepg/import"
}
