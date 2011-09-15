require v4l-dvb-modules.inc

DEPENDS += " mercurial-native"

SRCDATE = "20110914"
SRCREV = "6e0befab696a"
PV = "0.0+hg${SRCDATE}"
PR = "r0"

SRC_URI = "hg://linuxtv.org/hg/;module=v4l-dvb;rev=${SRCREV} \
           file://defconfig \
           file://v4l-dvb-compat.patch;patch=1 \
           file://v4l-2.6.18-compat.patch;patch=1 \
           file://fix-blocking-demux.patch;patch=1 \
           file://basic-dvb-t2-support.patch;patch=1 \
           file://localversion.patch;patch=1 \
           file://fix-strip.patch;patch=1 \
           file://build-fix.patch;patch=1 \
           file://backport-*.patch \
           file://v4l-dvb-as102.patch;patch=1 \
           file://v4l-dvb-a867.patch;patch=1 \
           file://v4l-dvb-tua9001.patch;patch=1 \
           file://v4l-dvb-af9033.patch;patch=1 \
           file://v4l-dvb-mxl5007_fix_for_af9035.patch;patch=1 \
           file://v4l-dvb-af9035.patch;patch=1 \
           file://v4l-dvb-tda18218.patch;patch=1 \
           file://v4l-dvb-af9013_fix_for_tda18218_tuner.patch;patch=1 \
           file://v4l-dvb-af9015_fix_for_tda18218_tuner.patch;patch=1 \
           file://v4l-dvb-af9013_add_firmware5.1.patch;patch=1 \
           file://v4l-dvb-af9015_fix_for_A815M.patch;patch=1 \
           file://v4l-dvb-em28xx_fix.patch;patch=1 \
           file://v4l-dvb-af9015_add_a850red.patch;patch=1 \
"

S = "${WORKDIR}/v4l-dvb"

do_munge() {
	CUR=`pwd`
	cd ${S}/linux
	oenote "cd to '${S}/linux'";
	for i in `ls ${WORKDIR}/backport-*.patch | sort -n | xargs`; do
		oenote "Applying v4l-dvb backport patch '$i'";
		patch -p1 < $i;
	done;
	cd $CUR
}

addtask munge before do_compile after do_patch

