DESCRIPTION = "Realtek RTL8187L"
SECTION = "kernel/modules"
LICENSE = "GPL"
RDEPENDS = "wireless-tools"

inherit module

PR = "r5"

SRC_URI = "file://rtl8187l.tar.gz \
	       file://rtl8187l_fix_for_makefile.patch;patch=1 \
	       file://rtl8187l_fix_for_code.patch;patch=1 \
	       file://rtl8187l_fix_for_ieee_softmac.patch;patch=1 \
"

S = "${WORKDIR}/rtl8187l"
EXTRA_OEMAKE = "KSRC=${STAGING_KERNEL_DIR}"

do_compile () {
        unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS CC LD CPP
        oe_runmake 'MODPATH={D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/net' \
                   'KERNEL_SOURCE=${STAGING_KERNEL_DIR}' \
                   'KDIR=${STAGING_KERNEL_DIR}' \
                   'KSRC=${STAGING_KERNEL_DIR}' \
                   'KVER=${KERNEL_VERSION}' \
                   'CC=${KERNEL_CC}' \
                   'LD=${KERNEL_LD}'
}

do_install() {
	install -d ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/net
	install -m 0644 ${S}/ieee80211/ieee80211_crypt-rtl.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/net
	install -m 0644 ${S}/ieee80211/ieee80211_crypt_ccmp-rtl.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/net
	install -m 0644 ${S}/ieee80211/ieee80211_crypt_tkip-rtl.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/net
	install -m 0644 ${S}/ieee80211/ieee80211_crypt_wep-rtl.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/net
	install -m 0644 ${S}/ieee80211/ieee80211-rtl.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/net
        install -m 0644 ${S}/rtl8187/rtl8187.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/net
}
