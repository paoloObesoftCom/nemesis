DESCRIPTION = "Bootlogo support"
SECTION = "base"
PRIORITY = "required"
LICENSE = "proprietary"
MAINTAINER = "Felix Domke <tmbinc@elitedvb.net>"

IMAGES_VERSION = "2"

BINARY_VERSION = "1"
BINARY_VERSION_dm500hd = "4"
BINARY_VERSION_dm7025 = "2"
BINARY_VERSION_dm800 = "2"
BINARY_VERSION_dm800se = "4"
BINARY_VERSION_dm7020hd = "4"
BINARY_VERSION_dm8000 = "6"

PV = "${BINARY_VERSION}.${IMAGES_VERSION}"
PR = "r6"

SRC_URI = "http://sources.dreamboxupdate.com/download/7020/bootlogo-${MACHINE}-${BINARY_VERSION}.elf \
	file://bootlogo.mvi \
	file://bootlogo_wait.mvi \
	file://backdrop.mvi \
	file://bootlogo.jpg"

SRC_URI_append_dm800 = " file://switchoff.mvi"

S = "${WORKDIR}/"

MVI = "bootlogo backdrop bootlogo_wait"
MVI_append_dm800 = " switchoff"

do_install() {
	install -d ${D}/boot
	install -m 0755 ${S}/bootlogo-${MACHINE}-${BINARY_VERSION}.elf ${D}/boot/bootlogo.elf
	for i in ${MVI}; do
		install -m 0755 ${S}/$i.mvi ${D}/boot/$i.mvi;
	done;
}

do_install_dm800() {
	install -d ${D}/boot
	install -d ${D}/usr/share
	install -m 0755 ${S}/bootlogo-${MACHINE}-${BINARY_VERSION}.elf ${D}/boot/bootlogo.elf
	install -m 0755 ${S}/bootlogo.jpg ${D}/boot/bootlogo.jpg
	for i in ${MVI}; do
		install -m 0755 ${S}/$i.mvi ${D}/usr/share/$i.mvi;
		ln -sf /usr/share/$i.mvi ${D}/boot/$i.mvi;
	done;
}

do_install_dm500hd() {
	do_install_dm800
}

do_install_dm800se() {
	do_install_dm800
}

do_install_dm7020hd() {
	do_install_dm800
}

do_install_dm8000() {
	do_install_dm800
}

pkg_preinst() {
	[ -d /proc/stb ] && mount -o rw,remount /boot
}

pkg_postinst() {
	[ -d /proc/stb ] && mount -o ro,remount /boot
}

pkg_prerm() {
	[ -d /proc/stb ] && mount -o rw,remount /boot
}

pkg_postrm() {
	[ -d /proc/stb ] && mount -o ro,remount /boot
}

PACKAGE_ARCH := "${MACHINE_ARCH}"
FILES_${PN} = "/boot /usr/share"
