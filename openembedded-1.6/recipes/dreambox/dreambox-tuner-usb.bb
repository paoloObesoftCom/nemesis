DESCRIPTION = "USB DVB-T/C Tuner Support Files"
LICENSE = "GPL"
DEPENDS += " v4l-dvb-modules"

PV = "2.2"
PR = "r1"

SRC_URI = "file://vtuner \
           file://restartDtt.sh \
           file://loaddttmodules.sh \
           file://v4l-dvb-firmwares.tar.gz \
"

S = "${WORKDIR}/"
PACKAGE_ARCH := "${MACHINE_ARCH}"
FILES_${PN} = "/"

do_install() {
	
	oe_dvb_path=${OE_BASE}/../openembedded/recipes/v4l-dvb
	dvb_srcdate=`grep SRCDATE ${oe_dvb_path}/v4l-dvb-modules_hg.bb| grep -m 1 "[12][09][0-9][0-9][01][0-9][0-3][0-9]" |sed -e 's/[a-zA-Z" =]//g'`
	dvb_srcrev=`grep PR ${oe_dvb_path}/v4l-dvb-modules_hg.bb| sed -e 's/[a-zA-Z" =]//g'`
	KERNEL_VERSION=2.6.18-7.4-${MACHINE}

	install -d ${D}/usr/bin
	install -d ${D}/usr/script
	install -d ${D}/etc/init.d
	install -d ${D}/lib/firmware
	install -d ${D}/lib/modules/dvbt
	
	install -m 0755 ${WORKDIR}/restartDtt.sh ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/vtuner ${D}/usr/bin
	install -m 0755 ${WORKDIR}/loaddttmodules.sh ${D}/usr/script
	install -m 644 ${WORKDIR}/*.hex ${D}/lib/firmware
	install -m 644 ${WORKDIR}/*.inp ${D}/lib/firmware
	install -m 644 ${WORKDIR}/*.fw ${D}/lib/firmware
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/common/tuners/*.ko  ${D}/lib/modules/dvbt
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/dvb/a867/*.ko  ${D}/lib/modules/dvbt
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/dvb/as102/*.ko  ${D}/lib/modules/dvbt
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/dvb/b2c2/*.ko  ${D}/lib/modules/dvbt
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/dvb/dvb-usb/*.ko  ${D}/lib/modules/dvbt
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/dvb/frontends/*.ko  ${D}/lib/modules/dvbt
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/dvb/siano/*.ko  ${D}/lib/modules/dvbt
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/dvb/ttusb-budget/*.ko  ${D}/lib/modules/dvbt
	install -m 644 ${OE_BASE}/build/tmp/work/${MACHINE}-oe-linux/v4l-dvb-modules-0.0+hg${dvb_srcdate}-r${dvb_srcrev}/package/lib/modules/${KERNEL_VERSION}/kernel/drivers/media/dvb/ttusb-dec/*.ko  ${D}/lib/modules/dvbt

}

