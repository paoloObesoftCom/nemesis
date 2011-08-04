DESCRIPTION = "nemesis version"
LICENSE = "GPL"

SVN = "089"
PR = "r1"
PV = "2.2-svn-${SVN}"

S = "${WORKDIR}/"
PACKAGE_ARCH := "${MACHINE_ARCH}"
FILES_${PN} = "/"

do_install() {

	IMAGE_VERSION="${SVN}"
	IMAGE_DATE="$(date +%Y%m%d%H%M)"
	IMAGE_TYPE="1"
	echo "version=${IMAGE_TYPE}${IMAGE_VERSION}${IMAGE_DATE}" > ${S}image-version
	echo "comment=${DISTRO_NAME}" >> ${S}image-version
	echo "target=9" >> ${S}image-version
	echo "creator=Gianathem" >> ${S}image-version
	echo "url=http://www.edg-nemesis.tv/" >> ${S}image-version
	echo "catalog=http://www.edg-nemesis.tv/" >> ${S}image-version

	install -d "${D}/etc/"
	install -m 644 "${WORKDIR}/image-version" "${D}/etc"

}
