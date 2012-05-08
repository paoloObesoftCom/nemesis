DESCRIPTION = "OpenDreambox: Enigma2 Task for the OpenDreambox Distribution"
SECTION = "opendreambox/base"
LICENSE = "MIT"

PV = "2.6"
PR = "r1"

inherit task

PROVIDES = "\
  task-opendreambox-ui \
  ${PACKAGES} \
"

PACKAGES = "\
  task-opendreambox-enigma2 \
"

DEPENDS += "\
  ${@base_contains("MACHINE_FEATURES", "dreambox-libpassthrough", "dreambox-libpassthrough", "", d)} \
"

#
# task-opendreambox-enigma2
#
RPROVIDES_task-opendreambox-enigma2 = "task-opendreambox-ui"
DESCRIPTION_task-opendreambox-enigma2 = "OpenDreambox: Enigma2 Dependencies"
RDEPENDS_task-opendreambox-enigma2 = "\
  dreambox-blindscan-utils \
  enigma2 \
  enigma2-defaultservices \
  enigma2-plugin-extensions-mediascanner \
  enigma2-plugin-systemplugins-frontprocessorupgrade \
  enigma2-plugin-systemplugins-hotplug \
  enigma2-plugin-systemplugins-networksetup \
  enigma2-plugin-systemplugins-networkwizard \
  enigma2-plugin-systemplugins-softwaremanager \
  enigma2-plugin-systemplugins-videotune \
  enigma2-streamproxy \
  tuxbox-tuxtxt-32bpp \
  enigma2-plugin-systemplugins-crossepg \
  enigma2-plugin-systemplugins-networkbrowser \
  enigma2-locale-en \
  enigma2-meta \
  enigma2-plugins-meta \
  enigma2-drivers-meta \  
"

RRECOMMENDS_task-opendreambox-enigma2 = "\
  aio-grab \
  python-crypt \
  python-netserver \
  python-twisted-core \
  python-twisted-protocols \
  python-twisted-web \
  enigma2-plugin-extensions-audiosync \
  enigma2-plugin-extensions-cutlisteditor \
  enigma2-plugin-extensions-graphmultiepg \
  enigma2-plugin-extensions-mediaplayer \
  enigma2-plugin-extensions-webbouqueteditor \
  enigma2-plugin-extensions-webinterface \
  enigma2-plugin-systemplugins-satfinder \
  enigma2-plugin-systemplugins-positionersetup \
  enigma2-plugin-systemplugins-skinselector \
  ${@base_contains("MACHINE_FEATURES", "wifi", "task-opendreambox-wlan", "", d)} \
  ${@base_contains("MACHINE_FEATURES", "dreambox-libpassthrough", "libpassthrough", "", d)} \
  enigma2-plugin-extensions-stayup \
  enigma2-plugin-extensions-epgsearch \
  enigma2-plugin-extensions-easymedia \
  enigma2-plugin-extensions-socketmmi \
  enigma2-plugin-extensions-genuinedreambox \
  enigma2-plugin-systemplugins-lcnscanner \
  enigma2-plugin-systemplugins-ledmanager \
  enigma2-locale-it \
  enigma2-locale-de \
"

RDEPENDS_task-opendreambox-enigma2_append_dm500hd = "\
  enigma2-plugin-systemplugins-videomode \
  enigma2-plugin-systemplugins-tempfancontrol \
  enigma2-plugin-systemplugins-3dsettings \
"

RDEPENDS_task-opendreambox-enigma2_append_dm800 = "\
  enigma2-plugin-systemplugins-videomode \
"

RDEPENDS_task-opendreambox-enigma2_append_dm800se = "\
  enigma2-plugin-systemplugins-videomode \
  enigma2-plugin-systemplugins-tempfancontrol \
  enigma2-plugin-systemplugins-3dsettings \
"

RDEPENDS_task-opendreambox-enigma2_append_dm8000 = "\
  enigma2-plugin-systemplugins-commoninterfaceassignment \
  enigma2-plugin-systemplugins-videomode \
  enigma2-plugin-systemplugins-nfiflash \
  enigma2-plugin-systemplugins-3dsettings \
"

RDEPENDS_task-opendreambox-enigma2_append_dm7020hd = "\
  enigma2-plugin-systemplugins-commoninterfaceassignment \
  enigma2-plugin-systemplugins-videomode \
  enigma2-plugin-systemplugins-nfiflash \
  enigma2-plugin-systemplugins-3dsettings \
"

RRECOMMENDS_task-opendreambox-enigma2_append_dm8000 = "\
  task-opendreambox-cdplayer \
  task-opendreambox-dvdplayer \
  task-opendreambox-dvdburn \
"

RRECOMMENDS_task-opendreambox-enigma2_append_dm7020hd = "\
  task-opendreambox-cdplayer \
  task-opendreambox-dvdplayer \
  task-opendreambox-dvdburn \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
