DESCRIPTION = "Read OpenTV epg from sat or import from xmltv or csv"
HOMEPAGE = http://www.crossepg.com
DEPENDS = "libxml2 zlib python"
LICENSE = "GPLv2"

PN = "enigma2-plugin-systemplugins-crossepg"

SRCDATE = "20111102"
BRANCH = "master"
PV = "2.4-git${SRCDATE}"
SRCREV = "3e635eeead5b8e00b25e58ce4a52d84f2c666b3d"
PR = "r2"

SRC_URI = "git://github.com/E2OpenPlugins/e2openplugin-CrossEPG.git;protocol=git;branch=${BRANCH};tag=${SRCREV} \
           file://download_epg.sh \
           file://convert_epg.sh \
           file://mpepg.bin \
           file://mpepg.conf \
           file://setup_hd.xml \
           file://menu_hd.xml \
           file://crossepg.config"

S = "${WORKDIR}/git"
PACKAGE_ARCH := "${MACHINE_ARCH}"

FILES_${PN} = "/usr/*"

CFLAGS_append = " -I${STAGING_INCDIR}/libxml2/ -I${STAGING_INCDIR}/python2.6/"

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
	install -m 644 "${WORKDIR}/menu_hd.xml" "${D}/usr/lib/enigma2/python/Plugins/SystemPlugins/CrossEPG/skins"
	install -m 644 "${WORKDIR}/setup_hd.xml" "${D}/usr/lib/enigma2/python/Plugins/SystemPlugins/CrossEPG/skins"
}
