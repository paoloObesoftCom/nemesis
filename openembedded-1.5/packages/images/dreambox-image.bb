export IMAGE_BASENAME = "dreambox-image"

OPENDREAMBOX_COMMON = "base-files busybox \
	ipkg initscripts-opendreambox sysvinit netbase openssh \
	base-passwd ncurses joe mc vsftpd timezones-alternative \
	netkit-base fakelocale less dreambox-bootlogo  \
	dreambox-dccamd dreambox-keymaps tuxbox-image-info dvbsnoop \
	dreambox-compat tuxbox-common mrouted smartmontools hddtemp \
	autofs"

OPENDREAMBOX_COMMON_D = "util-linux e2fsprogs \
	ppp module-init-tools samba"

OPENDREAMBOX_COMMON_R = "util-linux-sfdisk util-linux-fdisk e2fsprogs-mke2fs \
	e2fsprogs-e2fsck ppp module-init-tools-depmod \
	base-files-doc sambaserver sambamount avahi-daemon"

# experimental packages
OPENDREAMBOX_EXPERIMENTAL = "hotplug-ng enigma2 tuxbox-tuxtxt-32bpp twisted aio-grab"
OPENDREAMBOX_EXPERIMENTAL_R = "enigma2-plugin-systemplugins-frontprocessorupgrade \
	enigma2-plugin-systemplugins-softwaremanager \
	enigma2-plugin-systemplugins-positionersetup \
	enigma2-plugin-extensions-cutlisteditor \
	enigma2-plugin-systemplugins-satfinder \
	enigma2-plugin-systemplugins-videotune \
	enigma2-plugin-extensions-mediascanner \
	enigma2-plugin-extensions-webinterface \
	enigma2-plugin-extensions-graphmultiepg \
	enigma2-plugin-systemplugins-skinselector \
	enigma2-plugin-extensions-pictureplayer \
	enigma2-plugin-extensions-mediaplayer \
	enigma2-plugin-systemplugins-hotplug \
	enigma2-plugin-extensions-audiosync \
	enigma2-streamproxy ethtool \
	enigma2-defaultservices \
	enigma2-plugin-extensions-autotimer \
	enigma2-plugin-systemplugins-setpasswd \
	enigma2-plugin-extensions-vlcplayer \
	enigma2-plugin-systemplugins-networkbrowser \
	enigma2-plugin-extensions-imdb \
	enigma2-plugin-extensions-epgsearch \
	enigma2-plugin-systemplugins-autoresolution \
	enigma2-plugin-systemplugins-videoenhancement \
	python-gdata \
	enigma2-plugin-extensions-mytube \
	enigma2-plugin-extensions-moviecut \
	enigma2-plugin-extensions-movieretitle \
	enigma2-plugin-extensions-socketmmi \
	dreambox-nemesis \
	libusb \
	twisted python-crypt python-crypto \
	python-netserver python-pickle dreambox-blindscan-utils"

# legacy tuxbox stuff (enigma, plugins, ...)
OPENDREAMBOX_TUXBOX = "enigma ipkgpl"
OPENDREAMBOX_TUXBOX_D = "tuxbox-plugins tuxbox-plugins-enigma"
OPENDREAMBOX_TUXBOX_R = " \
	tuxbox-plugin-tuxtxt tuxbox-plugin-tuxcom enigma-plugin-dreamdata"

OPENDREAMBOX_TUXBOX_R_dm600pvr = " \
	tuxbox-plugin-tuxtxt tuxbox-plugin-tuxcom enigma-plugin-dreamdata enigma-blindscan"

OPENDREAMBOX_TUXBOX_R_dm500plus = " \
	tuxbox-plugin-tuxtxt tuxbox-plugin-tuxcom enigma-plugin-dreamdata enigma-blindscan"

# dvb api specific stuff
OPENDREAMBOX_V2_ONLY = "dreambox-dvb-tools tuxbox-stream"
OPENDREAMBOX_V3_ONLY = "dreambox-dvb-tools-v3 sctzap dvbtraffic"

# enigma languages
# disabled: enigma-locale-sr enigma-locale-ur
ENIGMA_LANGUAGE = "enigma-locale-ar enigma-locale-cs enigma-locale-da \
	enigma-locale-de enigma-locale-el enigma-locale-es enigma-locale-et \
	enigma-locale-fi enigma-locale-fr enigma-locale-hr enigma-locale-hu \
	enigma-locale-is enigma-locale-it enigma-locale-lt enigma-locale-nl \
	enigma-locale-no enigma-locale-pl enigma-locale-pt enigma-locale-ro \
	enigma-locale-ru enigma-locale-sk enigma-locale-sl \
	enigma-locale-sv enigma-locale-tr"

OPENDREAMBOX_TUXBOX_R += " ${ENIGMA_LANGUAGE}"
OPENDREAMBOX_TUXBOX_R_dm600pvr += " ${ENIGMA_LANGUAGE}"
OPENDREAMBOX_TUXBOX_R_dm500plus += " ${ENIGMA_LANGUAGE}"

MODEM_SUPPORT = "enigma-modem"
MODEM_SUPPORT_R = "kernel-module-crc-ccitt kernel-module-ppp-async \
	kernel-module-ppp-generic \
	kernel-module-slhc update-modules"

MODEM_SUPPORT_E2 = "dreambox-modem-ppp-scripts"
MODEM_SUPPORT_E2_R = "${MODEM_SUPPORT_R} dreambox-modem-ppp-scripts enigma2-plugin-extensions-modem"

WLAN_SUPPORT = "wireless-tools wlan-rt73 zd1211b wpa-supplicant"
WLAN_SUPPORT_dm8000 = "wireless-tools wpa-supplicant"
WLAN_SUPPORT_R = "enigma2-plugin-systemplugins-wirelesslan 	enigma2-plugin-systemplugins-networkwizard"

WLAN_MADWIFI = "madwifi-ng"
WLAN_MADWIFI_R = "madwifi-ng-modules madwifi-ng-tools"

DVDPLAYER_SUPPORT_R = "enigma2-plugin-extensions-dvdplayer cdrkit"

DVDBURN_SUPPORT_R = "enigma2-plugin-extensions-dvdburn dvd+rw-tools dvdauthor mjpegtools cdrkit python-imaging projectx"

CDPLAYER_SUPPORT_R = "kernel-module-cdfs libcddb libcdio enigma2-plugin-extensions-cdinfo"

NEMESIS_E1_COMMON_PACKAGES = "dreambox-nemesis nfs-utils"

NEMESIS_E2_COMMON_PACKAGES = "nfs-utils openvpn ez-ipupdate ntp ntpdate"

# now machine specific:
OPENDREAMBOX_COMMON_MACHINE_dm600pvr += "${NEMESIS_E1_COMMON_PACKAGES} ${OPENDREAMBOX_V2_ONLY} ${OPENDREAMBOX_TUXBOX} ${MODEM_SUPPORT} enigma-genuine-dreambox"
OPENDREAMBOX_COMMON_MACHINE_R_dm600pvr += "${OPENDREAMBOX_TUXBOX_R} ${MODEM_SUPPORT_R} dreambox-blindscan-utils"
OPENDREAMBOX_COMMON_MACHINE_D_dm600pvr += "${OPENDREAMBOX_TUXBOX_D} dreambox-tpmd"

OPENDREAMBOX_COMMON_MACHINE_dm500plus += "${NEMESIS_E1_COMMON_PACKAGES} ${OPENDREAMBOX_V2_ONLY} ${OPENDREAMBOX_TUXBOX} ${MODEM_SUPPORT} enigma-genuine-dreambox"
OPENDREAMBOX_COMMON_MACHINE_R_dm500plus += "${OPENDREAMBOX_TUXBOX_R} ${MODEM_SUPPORT_R} dreambox-blindscan-utils"
OPENDREAMBOX_COMMON_MACHINE_D_dm500plus += "${OPENDREAMBOX_TUXBOX_D} dreambox-tpmd"

OPENDREAMBOX_COMMON_MACHINE_dm7020 += "${NEMESIS_E1_COMMON_PACKAGES} ${OPENDREAMBOX_V2_ONLY} ${OPENDREAMBOX_TUXBOX} ${MODEM_SUPPORT}"
OPENDREAMBOX_COMMON_MACHINE_R_dm7020 += "${OPENDREAMBOX_TUXBOX_R} ${MODEM_SUPPORT_R}"
OPENDREAMBOX_COMMON_MACHINE_D_dm7020 += "${OPENDREAMBOX_TUXBOX_D}"

OPENDREAMBOX_COMMON_MACHINE_dm7025 += "${OPENDREAMBOX_V3_ONLY} ${OPENDREAMBOX_EXPERIMENTAL} ${WLAN_SUPPORT}"
OPENDREAMBOX_COMMON_MACHINE_R_dm7025 += "${OPENDREAMBOX_EXPERIMENTAL_R} ${WLAN_SUPPORT_R}"
OPENDREAMBOX_COMMON_MACHINE_D_dm7025 += ""

OPENDREAMBOX_COMMON_MACHINE_dm800 += "${NEMESIS_E2_COMMON_PACKAGES} ${OPENDREAMBOX_V3_ONLY} ${OPENDREAMBOX_EXPERIMENTAL} ${WLAN_SUPPORT}"
OPENDREAMBOX_COMMON_MACHINE_R_dm800 += "${OPENDREAMBOX_EXPERIMENTAL_R} ${WLAN_SUPPORT_R} enigma2-plugin-systemplugins-videomode \
	enigma2-plugin-extensions-dvdbackup \
	${DVDPLAYER_SUPPORT_R} ${CDPLAYER_SUPPORT_R} enigma2-plugin-extensions-genuinedreambox ${MODEM_SUPPORT_E2_R}"
OPENDREAMBOX_COMMON_MACHINE_D_dm800 += "dreambox-tpmd"

OPENDREAMBOX_COMMON_MACHINE_dm500hd += "${OPENDREAMBOX_V3_ONLY} ${OPENDREAMBOX_EXPERIMENTAL} ${MODEM_SUPPORT_E2}"
OPENDREAMBOX_COMMON_MACHINE_R_dm500hd += "${OPENDREAMBOX_EXPERIMENTAL_R} enigma2-plugin-systemplugins-videomode \
	enigma2-plugin-extensions-genuinedreambox ${MODEM_SUPPORT_E2_R} enigma2-plugin-systemplugins-tempfancontrol \
	${DVDPLAYER_SUPPORT_R}"
OPENDREAMBOX_COMMON_MACHINE_D_dm500hd += "dreambox-tpmd"

OPENDREAMBOX_COMMON_MACHINE_dm8000 += "${NEMESIS_E2_COMMON_PACKAGES} ${OPENDREAMBOX_V3_ONLY} ${OPENDREAMBOX_EXPERIMENTAL} ${WLAN_SUPPORT} ${WLAN_MADWIFI}"
OPENDREAMBOX_COMMON_MACHINE_R_dm8000 += "${OPENDREAMBOX_EXPERIMENTAL_R} \
	enigma2-plugin-extensions-dvdbackup \
	${WLAN_SUPPORT_R} ${WLAN_MADWIFI_R} ${DVDPLAYER_SUPPORT_R} \
	${DVDBURN_SUPPORT_R} enigma2-plugin-systemplugins-videomode \
	${CDPLAYER_SUPPORT_R} enigma2-plugin-systemplugins-commoninterfaceassignment \
	enigma2-plugin-extensions-genuinedreambox"
OPENDREAMBOX_COMMON_MACHINE_D_dm8000 += "dreambox-tpmd"

# collect the stuff into OPENDREAMBOX_COMMON
OPENDREAMBOX_COMMON += " ${OPENDREAMBOX_COMMON_MACHINE}"
OPENDREAMBOX_COMMON_R += " ${OPENDREAMBOX_COMMON_MACHINE_R}"
OPENDREAMBOX_COMMON_D += " ${OPENDREAMBOX_COMMON_MACHINE_D}"

# add bootstrap stuff
DEPENDS = "${OPENDREAMBOX_COMMON} ${BOOTSTRAP_EXTRA_DEPENDS} ${OPENDREAMBOX_COMMON_D}"
RDEPENDS = "${OPENDREAMBOX_COMMON} ${BOOTSTRAP_EXTRA_RDEPENDS} ${OPENDREAMBOX_COMMON_R}"

# we don't want any locales, at least not in the common way.
IMAGE_LINGUAS = " "

export IPKG_INSTALL = '${RDEPENDS}'

inherit image_ipk

export NFO = '${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.nfo'

do_rootfs_append() {
	if [ "${MACHINE}" = "dm800" -o "${MACHINE}" = "dm8000" ]; then
		VER=`grep Version: "${IMAGE_ROOTFS}/usr/lib/ipkg/info/enigma2.control" | cut -b 10-12`
		printf "Enigma2: Experimental ${VER}\n" > ${NFO}
	fi
	printf "Machine: Dreambox ${MACHINE}\n" >> ${NFO}
	DATE=`date +%Y-%m-%d' '%H':'%M`
	printf "Date: ${DATE}\n" >> ${NFO}
	printf "Issuer: Dream Multimedia TV\n" >> ${NFO}
	VER=`echo ${DISTRO_VERSION} | cut -b 1-3`
	printf "Link: http://dreamboxupdate.com/${DISTRO}/${VER}/${MACHINE}/experimental\n" >> ${NFO}
	if [ "${DESC}" != "" ]; then
		printf "Description: ${DESC}\n" >> ${NFO}
		printf "${DESC}\n" >> ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.desc
	fi
	MD5SUM=`md5sum ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.nfi | cut -b 1-32`
	printf "MD5: ${MD5SUM}\n" >> ${NFO}
}
