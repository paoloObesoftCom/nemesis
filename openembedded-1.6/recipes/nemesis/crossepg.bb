DESCRIPTION = "Read OpenTV epg from sat or import from xmltv or csv"
HOMEPAGE = http://www.crossepg.com
DEPENDS = "libxml2 zlib python"
LICENSE = "GPLv2"

REV ="151"
PR = "r3"
PN = "enigma2-plugin-systemplugins-crossepg"
PV = "0.5.9999-svn-${REV}"

SRC_URI = "svn://crossepg.googlecode.com/svn;module=trunk;proto=https;rev=${REV} \
           file://download_epg.sh \
           file://convert_epg.sh \
           file://mp2csv.bin \
           file://mp2csv.config \
           file://crossepg.config"

S = "${WORKDIR}/trunk"
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
	install -m 755 "${WORKDIR}/download_epg.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/convert_epg.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/mp2csv.bin" "${D}/usr/crossepg/import"
	install -m 644 "${WORKDIR}/crossepg.config" "${D}/usr/crossepg"
	install -m 644 "${WORKDIR}/mp2csv.config" "${D}/usr/crossepg/import"
}
