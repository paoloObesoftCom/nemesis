DESCRIPTION = "OpenDreambox: Base Task for the OpenDreambox Distribution"
SECTION = "opendreambox/base"
LICENSE = "MIT"
PV = "2.3"
PR = "r2"

inherit task

#
# task-opendreambox-base
#
DESCRIPTION_${PN} = "OpenDreambox: Basesystem utilities"

OPENDREAMBOX_BASE_ESSENTIAL = "\
  autofs \
  base-files-doc \
  dreambox-bootlogo \
  dreambox-compat \
  dreambox-wdog \
  ${@base_contains('MACHINE', 'dm7025', '', 'dreambox-tpmd', d)} \
  dreambox-feed-configs \
  e2fsprogs-e2fsck \
  e2fsprogs-mke2fs \
  fakelocale \
  netkit-base \
  opkg-nogpg \
  timezones-alternative \
  tuxbox-common \
  util-linux-sfdisk \
  vsftpd \
  hdparm \
"

NEMESIS_COMMON = "\
  nfs-utils \
  usbutils \
  openvpn \
  ez-ipupdate \
  bzip2 \
  libcap \
  libpcsclite \
  libbluray \
  python-gdata \
  dreambox-nemesis \
  nemesis-socket \
  nemesis-version \
"
 
OPENDREAMBOX_BASE_RECOMMENDS = "\
  openssh \
  openssh-sftp-server \
  dreambox-dccamd \
  sambaserver \
  zeroconf \
"

OPENDREAMBOX_BASE_OPTIONAL_RECOMMENDS = "\
  ccid \
  gdbserver \
  hddtemp \
  joe \
  mc \
  ncdu \
  smartmontools \
  avahi-daemon \
"

RDEPENDS_${PN} = "\
	${OPENDREAMBOX_BASE_ESSENTIAL} \
	${NEMESIS_COMMON} \
"

RRECOMMENDS_${PN} = "\
	${OPENDREAMBOX_BASE_RECOMMENDS} \
	${OPENDREAMBOX_BASE_OPTIONAL_RECOMMENDS} \
"

RRECOMMENDS_${PN}_dm7025 = "\
	${OPENDREAMBOX_BASE_RECOMMENDS} \
"
