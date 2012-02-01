DESCRIPTION = "StayUp for Lazy people"
DEPENDS = "libxml2 python dreambox-nemesis"
LICENSE = "GPLv2"
RDEPENDS = "dreambox-nemesis (>=1.0-r5)"

REV ="5"
PR = "r0"
PN = "enigma2-plugin-extensions-stayup"
PV = "svn-${REV}"

SRC_URI = "svn://osp.nemesis.tv/stayUP;module=trunk;proto=svn;rev=${REV}"

S = "${WORKDIR}/trunk"
PACKAGE_ARCH := "${MACHINE_ARCH}"

FILES_${PN} = "/"

do_install() {
	install -d ${D}/usr/lib/enigma2/python/Plugins/Extensions/StayUP
	install -d ${D}/usr/lib/enigma2/python/Plugins/Extensions/StayUP/meta
	install -m 644 ${S}/*.py ${D}/usr/lib/enigma2/python/Plugins/Extensions/StayUP
	install -m 755 ${S}/ping_host.sh ${D}/usr/lib/enigma2/python/Plugins/Extensions/StayUP
	install -m 644 ${S}/plugin-fs8.png ${D}/usr/lib/enigma2/python/Plugins/Extensions/StayUP
	install -m 644 ${S}/help.txt ${D}/usr/lib/enigma2/python/Plugins/Extensions/StayUP
	install -m 644 ${S}/enigma2-stayUP-r10.ipk.MASTERSIGNATURE ${D}/usr/lib/enigma2/python/Plugins/Extensions/StayUP
	install -m 644 ${S}/meta/plugin_stayup.xml ${D}/usr/lib/enigma2/python/Plugins/Extensions/StayUP/meta
}
