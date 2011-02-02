DESCRIPTION = "Nemesis files"
LICENSE = "GPL"

PR = "r13"

S = "${WORKDIR}/"
PACKAGE_ARCH := "${MACHINE_ARCH}"
FILES_${PN} = "/"

do_install() {
	
	srcpath=${DL_DIR}/../..

	mkdir -p ${D}/usr/{log,scam,uninstall,scce,www,keys}
	mkdir -p ${D}/etc/init.d
	mkdir ${D}/sbin/
	mkdir -p ${D}/home/root/.ssh

	if [ "${MACHINE}" = "dm500hd" ]; then
		cp -rf $srcpath/nemesis/root/enigma2/root/* ${D}/
		mkdir -p ${D}/media/{usb,usb1,usb2}
		ln -s media/usb ${D}/usb
		ln -s media/usb1 ${D}/usb1
		ln -s media/usb2 ${D}/usb2
	fi

	if [ "${MACHINE}" = "dm800" ]; then
		cp -rf $srcpath/nemesis/root/enigma2/root/* ${D}/
		mkdir -p ${D}/media/{usb,usb1,usb2}
		ln -s media/usb ${D}/usb
		ln -s media/usb1 ${D}/usb1
		ln -s media/usb2 ${D}/usb2
	fi

	if [ "${MACHINE}" = "dm800se" ]; then
		cp -rf $srcpath/nemesis/root/enigma2/root/* ${D}/
		mkdir -p ${D}/media/{usb,usb1,usb2}
		ln -s media/usb ${D}/usb
		ln -s media/usb1 ${D}/usb1
		ln -s media/usb2 ${D}/usb2
	fi

	if [ "${MACHINE}" = "dm7020hd" ]; then
		cp -rf $srcpath/nemesis/root/enigma2/root/* ${D}/
		mkdir -p ${D}/media/{usb,usb1,usb2}
		ln -s media/usb ${D}/usb
		ln -s media/usb1 ${D}/usb1
		ln -s media/usb2 ${D}/usb2
	fi

	if [ "${MACHINE}" = "dm8000" ]; then
		cp -rf $srcpath/nemesis/root/enigma2/root/* ${D}/
		mkdir -p ${D}/media/{usb,usb1,usb2,cf}
		ln -s media/cf ${D}/cf
		ln -s media/usb ${D}/usb
		ln -s media/usb1 ${D}/usb1
		ln -s media/usb2 ${D}/usb2
	fi

	if [ "${MACHINE}" = "dm7025" ]; then
		cp -rf $srcpath/nemesis/root/enigma2/root/* ${D}/
		cp -rf $srcpath/nemesis/root/enigma2/root_7025/* ${D}/
		mkdir -p ${D}/media/usb
		ln -s media/usb ${D}/usb
	fi

	find ${D}/ -name ".svn" | xargs rm -rf
}

