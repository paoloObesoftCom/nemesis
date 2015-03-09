DESCRIPTION = "Read OpenTV epg from sat or import from xmltv or csv"
HOMEPAGE = http://www.crossepg.com
DEPENDS = "libxml2 zlib python"
LICENSE = "GPLv2"

PN = "enigma2-plugin-systemplugins-crossepg"

SRCDATE = "20150309"
BRANCH = "master"
PV = "2.6-git${SRCDATE}"
SRCREV = "3ab1eb3ed15d5c775d004d28557cf3955ecd3412"
PR = "r5"

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
           file://skyit_hotbird_13.0.conf \
           file://rytecnemesis_xmltv.conf \
           file://mediaset_script.conf \
           file://alias.conf \
           file://alias.py \
           file://mediaprem.conf \
           file://mediaprem.py \
           file://mediaset.conf \
           file://mediaset.py \
           file://rai.conf \
           file://rai.py \
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
	install -d "${D}/usr/crossepg/scripts"
	install -d "${D}/usr/crossepg/scripts/alias"
	install -d "${D}/usr/crossepg/scripts/mediaprem"
	install -d "${D}/usr/crossepg/scripts/mediaset"
	install -d "${D}/usr/crossepg/scripts/rai"
	install -m 755 "${WORKDIR}/crossepg_epgmove.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/download_epg.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/convert_epg.sh" "${D}/usr/crossepg"
	install -m 755 "${WORKDIR}/mpepg.bin" "${D}/usr/crossepg/import"
	install -m 644 "${WORKDIR}/crossepg.config" "${D}/usr/crossepg"
	install -m 644 "${WORKDIR}/aliases.conf" "${D}/usr/crossepg/aliases"
	install -m 644 "${WORKDIR}/mpepg.conf" "${D}/usr/crossepg/import"
	install -m 644 "${WORKDIR}/skyit_hotbird_13.0.conf" "${D}/usr/crossepg/providers"
	install -m 644 "${WORKDIR}/rytecnemesis_xmltv.conf" "${D}/usr/crossepg/providers"
	install -m 644 "${WORKDIR}/mediaset_script.conf" "${D}/usr/crossepg/providers"
	install -m 644 "${WORKDIR}/alias.conf" "${D}/usr/crossepg/scripts/alias"
	install -m 644 "${WORKDIR}/alias.py" "${D}/usr/crossepg/scripts/alias"
	install -m 644 "${WORKDIR}/mediaprem.conf" "${D}/usr/crossepg/scripts/mediaprem"
	install -m 644 "${WORKDIR}/mediaprem.py" "${D}/usr/crossepg/scripts/mediaprem"
	install -m 644 "${WORKDIR}/mediaset.conf" "${D}/usr/crossepg/scripts/mediaset"
	install -m 644 "${WORKDIR}/mediaset.py" "${D}/usr/crossepg/scripts/mediaset"
	install -m 644 "${WORKDIR}/rai.conf" "${D}/usr/crossepg/scripts/rai"
	install -m 644 "${WORKDIR}/rai.py" "${D}/usr/crossepg/scripts/rai"
	install -m 644 "${WORKDIR}/crossepglib.py" "${D}/usr/lib/enigma2/python/Plugins/SystemPlugins/CrossEPG"
	install -m 644 "${WORKDIR}/crossepg_loader.py" "${D}/usr/lib/enigma2/python/Plugins/SystemPlugins/CrossEPG"
	install -m 644 "${WORKDIR}/crossepg_setup.py" "${D}/usr/lib/enigma2/python/Plugins/SystemPlugins/CrossEPG"
}
