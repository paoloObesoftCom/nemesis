DESCRIPTION = "Read OpenTV epg from sat or import from xmltv or csv"
HOMEPAGE = http://www.crossepg.com
DEPENDS = "libxml2 zlib python"
LICENSE = "GPLv2"

PN = "enigma2-plugin-systemplugins-crossepg"

SRCDATE = "20120130"
BRANCH = "master"
PV = "2.5-git${SRCDATE}"
SRCREV = "b5da37cb22ef34bdb909a13e6ff76f9d22e1a5cf"
PR = "r1"

SRC_URI = "git://github.com/E2OpenPlugins/e2openplugin-CrossEPG.git;protocol=git;branch=${BRANCH};tag=${SRCREV} \
           file://download_epg.sh \
           file://convert_epg.sh \
           file://crossepg_epgmove.sh \
           file://mpepg.bin \
           file://mpepg.conf \
           file://aliases.conf \
           file://crossepglib.py \
           file://crossepg_loader.py \
           file://crossepg_setup.py \
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
	install -d "${D}/usr/crossepg/aliases"
	install -m 755 "${WORKDIR}/crossepg_epgmove.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/download_epg.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/convert_epg.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/mpepg.bin" "${D}/usr/crossepg/import"
	install -m 644 "${WORKDIR}/crossepg.config" "${D}/usr/crossepg"
	install -m 644 "${WORKDIR}/aliases.conf" "${D}/usr/crossepg/aliases"
	install -m 644 "${WORKDIR}/mpepg.conf" "${D}/usr/crossepg/import"
	install -m 644 "${WORKDIR}/crossepglib.py" "${D}/usr/lib/enigma2/python/Plugins/SystemPlugins/CrossEPG"
	install -m 644 "${WORKDIR}/crossepg_loader.py" "${D}/usr/lib/enigma2/python/Plugins/SystemPlugins/CrossEPG"
	install -m 644 "${WORKDIR}/crossepg_setup.py" "${D}/usr/lib/enigma2/python/Plugins/SystemPlugins/CrossEPG"
}
