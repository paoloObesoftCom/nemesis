DESCRIPTION = "Nemesis version"
LICENSE = "GPL"

REV = "166"
PR = "r0"
PV = "2.0-svn-${REV}"

S = "${WORKDIR}/"
PACKAGE_ARCH := "${MACHINE_ARCH}"
FILES_${PN} = "/"

do_install() {

	IMAGE_VERSION="${REV}"
	IMAGE_DATE="$(date +%Y%m%d%H%M)"
	IMAGE_TYPE="1"
	echo "version=${IMAGE_TYPE}${IMAGE_VERSION}${IMAGE_DATE}" > ${S}image-version
	echo "comment=${DISTRO_NAME}" >> ${S}image-version
	echo "target=9" >> ${S}image-version
	echo "creator=Gianathem" >> ${S}image-version
	echo "url=http://www.edg-nemesis.tv/" >> ${S}image-version
	echo "catalog=http://ops.edg-nemesis.tv/" >> ${S}image-version

	install -d "${D}/etc/"
	install -m 644 "${WORKDIR}/image-version" "${D}/etc"

}
