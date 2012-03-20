DESCRIPTION = "Configuration files for online package repositories aka feeds"
PR = "r6"

DISTRO_FEED_PREFIX = "Nemesis"
DISTRO_FEED_URI = "http://nemesis.tv/1.6/NEMESIS/${MACHINE}"

SRC_URI = "file://addons.url"

do_compile() {
    mkdir -p ${S}/${sysconfdir}/opkg
    for feed in all ${TARGET_ARCH} ${MACHINE_ARCH}; do
        echo "src/gz ${DISTRO_FEED_PREFIX}-${feed} ${DISTRO_FEED_URI}/${feed}" > ${S}/${sysconfdir}/opkg/${feed}-feed.conf
    done
}
do_install () {
        install -d ${D}${sysconfdir}
        install -m 0644 ${WORKDIR}/addons.url ${D}/${sysconfdir}/addons.url
        install -d ${D}${sysconfdir}/opkg
        install -m 0644 ${S}/${sysconfdir}/opkg/* ${D}${sysconfdir}/opkg/
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

CONFFILES_${PN} += '${@ " ".join( [ ( "${sysconfdir}/opkg/%s-feed.conf" % feed ) for feed in "all ${TARGET_ARCH} ${MACHINE_ARCH}".split() ] ) }'
