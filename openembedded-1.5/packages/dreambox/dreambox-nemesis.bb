DESCRIPTION = "Nemesis files"
LICENSE = "GPL"

PR = "r0"

FILES_${PN} = "/"

do_install() {
	
	srcpath=${OE_BASE}/../../..

	mkdir -p ${D}/usr/
	mkdir -p ${D}/etc/
	mkdir -p ${D}/media/
	mkdir -p ${D}/bin/
	mkdir -p ${D}/home/root/.ssh

	if [ "${MACHINE}" = "dm500plus" ]; then
		cp -r $srcpath/nemesis/root/enigma1/root/* ${D}/
		mkdir ${D}/media/usb
		ln -s media/usb ${D}/usb
		ln -s /var/autofs ${D}/autofs
		ln -s /var/autofs/epg ${D}/epg
	fi

	if [ "${MACHINE}" = "dm600pvr" ]; then
		cp -r $srcpath/nemesis/root/enigma1/root/* ${D}/
		mkdir ${D}/media/usb
		ln -s media/usb ${D}/usb
		ln -s /var/autofs ${D}/autofs
		ln -s /var/autofs/epg ${D}/epg
	fi

	if [ "${MACHINE}" = "dm7020" ]; then
		cp -r $srcpath/nemesis/root/enigma1/root/* ${D}/
		cp -r $srcpath/nemesis/root/enigma1/root_7020/* ${D}/
		mkdir -p ${D}/media/{cf,usb}
		ln -s media/usb ${D}/usb
		ln -s media/cf ${D}/cf
		ln -s /var/autofs ${D}/autofs
		ln -s /var/autofs/epg ${D}/epg
	fi

	if [ "${MACHINE}" = "dm500hd" ]; then
		cp -r $srcpath/nemesis/root/enigma2-1.5/root/* ${D}/
		mkdir ${D}/media/usb
		ln -s media/usb ${D}/usb
	fi

	if [ "${MACHINE}" = "dm800" ]; then
		cp -r $srcpath/nemesis/root/enigma2-1.5/root/* ${D}/
		mkdir ${D}/media/{usb,usb1,usb2}
		ln -s media/usb ${D}/usb
		ln -s media/usb1 ${D}/usb1
		ln -s media/usb2 ${D}/usb2
	fi

	if [ "${MACHINE}" = "dm800se" ]; then
		cp -r $srcpath/nemesis/root/enigma2-1.5/root/* ${D}/
		mkdir ${D}/media/{usb,usb1,usb2}
		ln -s media/usb ${D}/usb
		ln -s media/usb1 ${D}/usb1
		ln -s media/usb2 ${D}/usb2
	fi

	if [ "${MACHINE}" = "dm8000" ]; then
		cp -r $srcpath/nemesis/root/enigma2-1.5/root/* ${D}/
		mkdir ${D}/media/{usb,usb1,usb2,cf}
		ln -s media/cf ${D}/cf
		ln -s media/usb ${D}/usb
		ln -s media/usb1 ${D}/usb1
		ln -s media/usb2 ${D}/usb2
	fi

	if [ "${MACHINE}" = "dm7025" ]; then
		cp -r $srcpath/nemesis/root/enigma2-1.5/root/* ${D}/
		mkdir ${D}/media/usb
		ln -s media/usb ${D}/usb
	fi

	find ${D}/ -name ".svn" | xargs rm -rf
}
