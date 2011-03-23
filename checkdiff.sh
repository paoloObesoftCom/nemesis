#!/bin/sh

envpath=..
srcpath=$envpath/src
oesrc16=$envpath/openembedded/1.6
oesrc15=$envpath/openembedded/1.5

e1_cvssrc=$srcpath/enigma
e1_apps_libs=$srcpath/apps/apps.tuxbox.libs_cvs.tuxbox.org__20070307/libs
e1_apps_libtuxbox=$srcpath/apps/apps.tuxbox.libtuxbox_cvs.tuxbox.org__20040928/libtuxbox
e1_apps_plugins=$srcpath/apps/apps.tuxbox.plugins_cvs.tuxbox.org__20061009/plugins

e2_cvssrc=$srcpath/enigma2
e2_plugsrc=$srcpath/enigma2-plugins
e2_aiosrc=$srcpath/aio-grab
e2_libdvdsrc=$srcpath/libdreamdvd
e2_streamsrc=$srcpath/enigma2-streamproxy

check_enigma1()
{
	# Check enigma1
	kompare $e1_cvssrc/configure.ac patch/enigma1/enigma/configure.ac

	kompare $e1_cvssrc/src/Makefile.am patch/enigma1/enigma/src/Makefile.am
	kompare $e1_cvssrc/src/channelinfo.h patch/enigma1/enigma/src/channelinfo.h
	kompare $e1_cvssrc/src/channelinfo.cpp patch/enigma1/enigma/src/channelinfo.cpp
	kompare $e1_cvssrc/src/enigma.h patch/enigma1/enigma/src/enigma.h
	kompare $e1_cvssrc/src/enigma.cpp patch/enigma1/enigma/src/enigma.cpp
	kompare $e1_cvssrc/src/enigma_main.h patch/enigma1/enigma/src/enigma_main.h
	kompare $e1_cvssrc/src/enigma_main.cpp patch/enigma1/enigma/src/enigma_main.cpp
	kompare $e1_cvssrc/src/enigma_setup.h patch/enigma1/enigma/src/enigma_setup.h
	kompare $e1_cvssrc/src/enigma_setup.cpp patch/enigma1/enigma/src/enigma_setup.cpp
	kompare $e1_cvssrc/src/enigma_dyn.cpp patch/enigma1/enigma/src/enigma_dyn.cpp
	kompare $e1_cvssrc/src/enigma_dyn_timer.cpp patch/enigma1/enigma/src/enigma_dyn_timer.cpp
	kompare $e1_cvssrc/src/enigma_plugins.cpp patch/enigma1/enigma/src/enigma_plugins.cpp
	kompare $e1_cvssrc/src/setup_extra.cpp patch/enigma1/enigma/src/setup_extra.cpp
	kompare $e1_cvssrc/src/setup_harddisk.cpp patch/enigma1/enigma/src/setup_harddisk.cpp
	kompare $e1_cvssrc/src/setupvideo.cpp patch/enigma1/enigma/src/setupvideo.cpp
	kompare $e1_cvssrc/src/sselect.cpp patch/enigma1/enigma/src/sselect.cpp
	kompare $e1_cvssrc/src/timer.cpp patch/enigma1/enigma/src/timer.cpp
	
	kompare $e1_cvssrc/lib/base/eerror.cpp patch/enigma1/enigma/lib/base/eerror.cpp
	kompare $e1_cvssrc/lib/dvb/cahandler.cpp patch/enigma1/enigma/lib/dvb/cahandler.cpp
	kompare $e1_cvssrc/lib/dvb/edvb.cpp patch/enigma1/enigma/lib/dvb/edvb.cpp
	kompare $e1_cvssrc/lib/dvb/record.cpp patch/enigma1/enigma/lib/dvb/record.cpp
	kompare $e1_cvssrc/lib/dvb/epgcache.cpp patch/enigma1/enigma/lib/dvb/epgcache.cpp
	kompare $e1_cvssrc/lib/dvb/si.cpp patch/enigma1/enigma/lib/dvb/si.cpp
	kompare $e1_cvssrc/lib/gdi/lcd.cpp patch/enigma1/enigma/lib/gdi/lcd.cpp
	kompare $e1_cvssrc/lib/system/info.cpp patch/enigma1/enigma/lib/system/info.cpp
	
	kompare $e1_cvssrc/include/lib/base/eerror.h patch/enigma1/enigma/include/lib/base/eerror.h
	kompare $e1_cvssrc/include/lib/dvb/serviceplaylist.h patch/enigma1/enigma/include/lib/dvb/serviceplaylist.h
	kompare $e1_cvssrc/include/lib/dvb/epgcache.h patch/enigma1/enigma/include/lib/dvb/epgcache.h
	
	kompare $e1_cvssrc/data/resources/rcdm5xxx.xml patch/enigma1/enigma/data/resources/rcdm5xxx.xml
	kompare $e1_cvssrc/data/resources/rcdm7000.xml patch/enigma1/enigma/data/resources/rcdm7000.xml
	kompare $e1_cvssrc/data/skins/default.esml patch/enigma1/enigma/data/skins/default.esml
	kompare $e1_cvssrc/data/fonts/Makefile.am patch/enigma1/enigma/data/fonts/Makefile.am
}

check_skin()
{
	kompare $e2_cvssrc/data/skin_default.xml patch/enigma2/data/skin_default.xml
	kompare $e2_cvssrc/data/skin_default/Makefile.am patch/enigma2/data/skin_default/Makefile.am
	kompare $e2_cvssrc/data/skin_default/menu/Makefile.am patch/enigma2/data/skin_default/menu/Makefile.am
	kompare $e2_cvssrc/data/skin_default/icons/Makefile.am patch/enigma2/data/skin_default/icons/Makefile.am
	kompare $e2_cvssrc/data/skin_default/buttons/Makefile.am patch/enigma2/data/skin_default/buttons/Makefile.am
}

check_enigma2()
{
	# Check enigma2
	kompare $e2_cvssrc/tools/enigma2.sh.in patch/enigma2/tools/enigma2.sh.in

	kompare $e2_cvssrc/RecordTimer.py patch/enigma2/RecordTimer.py
	kompare $e2_cvssrc/mytest.py patch/enigma2/mytest.py
	kompare $e2_cvssrc/skin.py patch/enigma2/skin.py
	kompare $e2_cvssrc/configure.ac patch/enigma2/configure.ac
	
	kompare $e2_cvssrc/lib/python/enigma_python.i patch/enigma2/lib/python/enigma_python.i
	kompare $e2_cvssrc/lib/python/Makefile.am patch/enigma2/lib/python/Makefile.am
	
	kompare $e2_cvssrc/lib/python/Plugins/SystemPlugins/SoftwareManager/plugin.py patch/enigma2/lib/python/Plugins/SystemPlugins/SoftwareManager/plugin.py
	kompare $e2_cvssrc/lib/python/Plugins/SystemPlugins/WirelessLan/Wlan.py patch/enigma2/lib/python/Plugins/SystemPlugins/WirelessLan/Wlan.py
	kompare $e2_cvssrc/lib/python/Plugins/Extensions/GraphMultiEPG/GraphMultiEpg.py patch/enigma2/lib/python/Plugins/Extensions/GraphMultiEPG/GraphMultiEpg.py

	kompare $e2_cvssrc/lib/python/Screens/AudioSelection.py patch/enigma2/lib/python/Screens/AudioSelection.py
	kompare $e2_cvssrc/lib/python/Screens/InfoBar.py patch/enigma2/lib/python/Screens/InfoBar.py
	kompare $e2_cvssrc/lib/python/Screens/InfoBarGenerics.py patch/enigma2/lib/python/Screens/InfoBarGenerics.py
	kompare $e2_cvssrc/lib/python/Screens/TimerEntry.py patch/enigma2/lib/python/Screens/TimerEntry.py
	
	kompare $e2_cvssrc/lib/python/Components/Ipkg.py patch/enigma2/lib/python/Components/Ipkg.py
	kompare $e2_cvssrc/lib/python/Components/About.py patch/enigma2/lib/python/Components/About.py
	kompare $e2_cvssrc/lib/python/Components/UsageConfig.py patch/enigma2/lib/python/Components/UsageConfig.py
	kompare $e2_cvssrc/lib/python/Components/Lcd.py patch/enigma2/lib/python/Components/Lcd.py
	kompare $e2_cvssrc/lib/python/Components/TimerList.py patch/enigma2/lib/python/Components/TimerList.py
	kompare $e2_cvssrc/lib/python/Components/Language.py patch/enigma2/lib/python/Components/Language.py
	
	kompare $e2_cvssrc/lib/python/Components/Converter/Makefile.am patch/enigma2/lib/python/Components/Converter/Makefile.am
	kompare $e2_cvssrc/lib/python/Components/Converter/ServiceName.py patch/enigma2/lib/python/Components/Converter/ServiceName.py
	kompare $e2_cvssrc/lib/python/Components/Converter/ClockToText.py patch/enigma2/lib/python/Components/Converter/ClockToText.py
	kompare $e2_cvssrc/lib/python/Components/Converter/FrontendInfo.py patch/enigma2/lib/python/Components/Converter/FrontendInfo.py

	kompare $e2_cvssrc/lib/python/Components/Renderer/Picon.py patch/enigma2/lib/python/Components/Renderer/Picon.py
	
	kompare $e2_cvssrc/data/Makefile.am patch/enigma2/data/Makefile.am
	kompare $e2_cvssrc/data/keymap.xml patch/enigma2/data/keymap.xml
	kompare $e2_cvssrc/data/setup.xml patch/enigma2/data/setup.xml
	kompare $e2_cvssrc/data/menu.xml patch/enigma2/data/menu.xml
	kompare $e2_cvssrc/data/fonts/Makefile.am patch/enigma2/data/fonts/Makefile.am

	kompare $e2_cvssrc/main/Makefile.am patch/enigma2/main/Makefile.am
	kompare $e2_cvssrc/main/enigma.cpp patch/enigma2/main/enigma.cpp

	kompare $e2_cvssrc/lib/dvb/epgcache.h patch/enigma2/lib/dvb/epgcache.h
	kompare $e2_cvssrc/lib/dvb/epgcache.cpp patch/enigma2/lib/dvb/epgcache.cpp
	kompare $e2_cvssrc/lib/dvb/eit.cpp patch/enigma2/lib/dvb/eit.cpp

}

check_plugins_e2()
{
	# Check Plugins
	kompare $e2_plugsrc/webinterface/src/web-data/objects.js patch/enigma2-plugins/webinterface/src/web-data/objects.js 
	kompare $e2_plugsrc/webinterface/src/web-data/timer.js patch/enigma2-plugins/webinterface/src/web-data/timer.js 
	kompare $e2_plugsrc/webinterface/src/WebComponents/Sources/Timer.py patch/enigma2-plugins/webinterface/src/WebComponents/Sources/Timer.py
	kompare $e2_plugsrc/webinterface/src/WebComponents/Sources/WAPfunctions.py patch/enigma2-plugins/webinterface/src/WebComponents/Sources/WAPfunctions.py
	kompare $e2_plugsrc/quickbutton/src/keymap.xml patch/enigma2-plugins/quickbutton/src/keymap.xml
	kompare $e2_plugsrc/quickbutton/src/plugin.py patch/enigma2-plugins/quickbutton/src/plugin.py
}

check_oe_16()
{
	# Check openembedded 1.6
	kompare $oesrc16/openembedded/conf/distro/opendreambox.conf openembedded-1.6/conf/distro/opendreambox.conf
	kompare $oesrc16/openembedded/conf/machine/dm500hd.conf openembedded-1.6/conf/machine/dm500hd.conf
	kompare $oesrc16/openembedded/conf/machine/dm800.conf openembedded-1.6/conf/machine/dm800.conf
	kompare $oesrc16/openembedded/conf/machine/dm800se.conf openembedded-1.6/conf/machine/dm800se.conf
	kompare $oesrc16/openembedded/conf/machine/dm7020hd.conf openembedded-1.6/conf/machine/dm7020hd.conf
	kompare $oesrc16/openembedded/conf/machine/dm7025.conf openembedded-1.6/conf/machine/dm7025.conf
	kompare $oesrc16/openembedded/conf/machine/dm8000.conf openembedded-1.6/conf/machine/dm8000.conf
	kompare $oesrc16/openembedded/recipes/base-files/base-files/opendreambox/profile openembedded-1.6/recipes/base-files/base-files/opendreambox/profile
	kompare $oesrc16/openembedded/recipes/base-passwd/base-passwd_3.5.20.bb openembedded-1.6/recipes/base-passwd/base-passwd_3.5.20.bb
	kompare $oesrc16/openembedded/recipes/busybox/busybox_1.15.3.bb openembedded-1.6/recipes/busybox/busybox_1.15.3.bb
	kompare $oesrc16/openembedded/recipes/busybox/busybox-1.15.3/opendreambox/defconfig openembedded-1.6/recipes/busybox/busybox-1.15.3/opendreambox/defconfig
	kompare $oesrc16/openembedded/recipes/dreambox/dreambox-dccamd.bb openembedded-1.6/recipes/dreambox/dreambox-dccamd.bb
	kompare $oesrc16/openembedded/recipes/dreambox/dreambox-bootlogo.bb openembedded-1.6/recipes/dreambox/dreambox-bootlogo.bb
	kompare $oesrc16/openembedded/recipes/dreambox/dreambox-feed-configs.bb openembedded-1.6/recipes/dreambox/dreambox-feed-configs.bb
	kompare $oesrc16/openembedded/recipes/enigma2/enigma2.bb openembedded-1.6/recipes/enigma2/enigma2.bb
	kompare $oesrc16/openembedded/recipes/enigma2/enigma2-plugins.bb openembedded-1.6/recipes/enigma2/enigma2-plugins.bb
	kompare $oesrc16/openembedded/recipes/ez-ipupdate/ez-ipupdate_3.0.10.bb openembedded-1.6/recipes/ez-ipupdate/ez-ipupdate_3.0.10.bb
	kompare $oesrc16/openembedded/recipes/images/dreambox-image.bb openembedded-1.6/recipes/images/dreambox-image.bb
	kompare $oesrc16/openembedded/recipes/initscripts/initscripts-1.0/dm500hd/bootup openembedded-1.6/recipes/initscripts/initscripts-1.0/dm500hd/bootup
	kompare $oesrc16/openembedded/recipes/initscripts/initscripts-1.0/dm800/bootup openembedded-1.6/recipes/initscripts/initscripts-1.0/dm800/bootup
	kompare $oesrc16/openembedded/recipes/initscripts/initscripts-1.0/dm800se/bootup openembedded-1.6/recipes/initscripts/initscripts-1.0/dm800se/bootup
	kompare $oesrc16/openembedded/recipes/initscripts/initscripts-1.0/dm7020hd/bootup openembedded-1.6/recipes/initscripts/initscripts-1.0/dm7020hd/bootup
	kompare $oesrc16/openembedded/recipes/initscripts/initscripts-1.0/dm8000/bootup openembedded-1.6/recipes/initscripts/initscripts-1.0/dm8000/bootup
	kompare $oesrc16/openembedded/recipes/initscripts/initscripts-opendreambox_1.0.bb openembedded-1.6/recipes/initscripts/initscripts-opendreambox_1.0.bb
	kompare $oesrc16/openembedded/recipes/linux/linux-dm500hd-2.6.18/defconfig openembedded-1.6/recipes/linux/linux-dm500hd-2.6.18/defconfig
	kompare $oesrc16/openembedded/recipes/linux/linux-dm800-2.6.18/defconfig openembedded-1.6/recipes/linux/linux-dm800-2.6.18/defconfig
	kompare $oesrc16/openembedded/recipes/linux/linux-dm800se-2.6.18/defconfig openembedded-1.6/recipes/linux/linux-dm800se-2.6.18/defconfig
	kompare $oesrc16/openembedded/recipes/linux/linux-dm7020hd-2.6.18/defconfig openembedded-1.6/recipes/linux/linux-dm7020hd-2.6.18/defconfig
	kompare $oesrc16/openembedded/recipes/linux/linux-dm8000-2.6.18/defconfig openembedded-1.6/recipes/linux/linux-dm8000-2.6.18/defconfig
	kompare $oesrc16/openembedded/recipes/linux/linux-dm7025/dm7025_defconfig openembedded-1.6/recipes/linux/linux-dm7025/dm7025_defconfig
	kompare $oesrc16/openembedded/recipes/openssl/openssl.inc openembedded-1.6/recipes/openssl/openssl.inc
	kompare $oesrc16/openembedded/recipes/netkit-base/netkit-base-0.17/opendreambox/inetd.conf openembedded-1.6/recipes/netkit-base/netkit-base-0.17/opendreambox/inetd.conf
	kompare $oesrc16/openembedded/recipes/nfs-utils/nfs-utils_1.1.2.bb openembedded-1.6/recipes/nfs-utils/nfs-utils_1.1.2.bb
	kompare $oesrc16/openembedded/recipes/samba/samba.inc openembedded-1.6/recipes/samba/samba.inc
	kompare $oesrc16/openembedded/recipes/tasks/task-opendreambox-base.bb openembedded-1.6/recipes/tasks/task-opendreambox-base.bb
	kompare $oesrc16/openembedded/recipes/tasks/task-opendreambox-dvdplayer.bb openembedded-1.6/recipes/tasks/task-opendreambox-dvdplayer.bb
	kompare $oesrc16/openembedded/recipes/tasks/task-opendreambox-enigma2.bb openembedded-1.6/recipes/tasks/task-opendreambox-enigma2.bb
	kompare $oesrc16/openembedded/recipes/tasks/task-opendreambox-wlan.bb openembedded-1.6/recipes/tasks/task-opendreambox-wlan.bb
	kompare $oesrc16/openembedded/recipes/vsftpd/files/init openembedded-1.6/recipes/vsftpd/files/init
	kompare $oesrc16/openembedded/recipes/vsftpd/files/opendreambox/vsftpd.conf openembedded-1.6/recipes/vsftpd/files/opendreambox/vsftpd.conf
	kompare $oesrc16/openembedded/recipes/v4l-dvb/v4l-dvb-modules_hg.bb openembedded-1.6/recipes/v4l-dvb/v4l-dvb-modules_hg.bb
	kompare $oesrc16/openembedded/recipes/v4l-dvb/v4l-dvb-modules.inc openembedded-1.6/recipes/v4l-dvb/v4l-dvb-modules.inc
	kompare $oesrc16/openembedded/recipes/v4l-dvb/files/defconfig openembedded-1.6/recipes/v4l-dvb/files/defconfig

}

check_oe_15()
{
	# Check openembedded
	kompare $oesrc15/openembedded/conf/machine/dm500plus.conf openembedded-1.5/conf/machine/dm500plus.conf
	kompare $oesrc15/openembedded/conf/machine/dm600pvr.conf openembedded-1.5/conf/machine/dm600pvr.conf
	kompare $oesrc15/openembedded/conf/machine/dm7020.conf openembedded-1.5/conf/machine/dm7020.conf
	kompare $oesrc15/openembedded/packages/autofs/autofs_4.1.4.bb openembedded-1.5/packages/autofs/autofs_4.1.4.bb
	kompare $oesrc15/openembedded/packages/base-files/base-files/opendreambox/profile openembedded-1.5/packages/base-files/base-files/opendreambox/profile
	kompare $oesrc15/openembedded/packages/base-passwd/base-passwd_3.5.9.bb openembedded-1.5/packages/base-passwd/base-passwd_3.5.9.bb
	kompare $oesrc15/openembedded/packages/enigma/enigma_cvs.bb openembedded-1.5/packages/enigma/enigma_cvs.bb
	kompare $oesrc15/openembedded/packages/enigma/enigma/add_blindscan_to_menu.diff openembedded-1.5/packages/enigma/enigma/add_blindscan_to_menu.diff
	kompare $oesrc15/openembedded/packages/ez-ipupdate/ez-ipupdate_3.0.10.bb openembedded-1.5/packages/ez-ipupdate/ez-ipupdate_3.0.10.bb
	kompare $oesrc15/openembedded/packages/images/dreambox-image.bb openembedded-1.5/packages/images/dreambox-image.bb
	kompare $oesrc15/openembedded/packages/initscripts/initscripts-opendreambox_1.0.bb openembedded-1.5/packages/initscripts/initscripts-opendreambox_1.0.bb
	kompare $oesrc15/openembedded/packages/libdreamdvd/libdreamdvd.bb openembedded-1.5/packages/libdreamdvd/libdreamdvd.bb 
	kompare $oesrc15/openembedded/packages/openssl/openssl.inc openembedded-1.5/packages/openssl/openssl.inc
	kompare $oesrc15/openembedded/packages/netkit-base/netkit-base-0.17/opendreambox/inetd.conf openembedded-1.5/packages/netkit-base/netkit-base-0.17/opendreambox/inetd.conf
	kompare $oesrc15/openembedded/packages/nfs-utils/nfs-utils_1.0.6.bb openembedded-1.5/packages/nfs-utils/nfs-utils_1.0.6.bb
	kompare $oesrc15/openembedded/packages/samba/samba.inc openembedded-1.5/packages/samba/samba.inc
	kompare $oesrc15/openembedded/packages/vsftpd/files/opendreambox/vsftpd.conf openembedded-1.5/packages/vsftpd/files/opendreambox/vsftpd.conf
}

check_language_e1()
{
	# Check po files
	kompare $e1_cvssrc/po/it.po patch/po/enigma1/it.po
	kompare $e1_cvssrc/po/Makefile.am patch/po/enigma1/Makefile.am
}

check_language_e2()
{
	# Check po files
	kompare $e2_cvssrc/po/it.po patch/po/enigma2/it.po
	kompare $e2_cvssrc/po/lt.po patch/po/enigma2/lt.po
	kompare $e2_cvssrc/po/Makefile.am patch/po/enigma2/Makefile.am
}

check_e1_cvs()
{
	# Check e1 cvs
	cervisia $e1_cvssrc
	cervisia $e1_apps_libs
	cervisia $e1_apps_libtuxbox
	cervisia $e1_apps_plugins &

}

check_e2_cvs()
{
	# Check e2 cvs
	cervisia $e2_streamsrc
	cervisia $e2_aiosrc
}

case "$1" in
	e1)
		check_enigma1
		;;
	e2)
		check_enigma2
		;;
	pluginse2)
		check_plugins_e2
		;;
	oe15)
		check_oe_15
		;;
	oe16)
		check_oe_16
		;;
	po_e1)
		check_language_e1
		;;
	po_e2)
		check_language_e2
		;;
	all)
		check_oe
		check_enigma
		check_plugins
		check_language
		;;
	e2_cvs)
		check_e2_cvs
		;;
	e1_cvs)
		check_e1_cvs
		;;
	skin)
		check_skin
		;;
	*)
		echo "Usage : $0 {e1|e2|pluginse2|oe15|oe16|po_e1|po_e2|all|e2_cvs|e1_cvs|skin}"
		;;
esac

