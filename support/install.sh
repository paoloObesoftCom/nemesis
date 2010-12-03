#!/bin/sh
# -*- coding: utf-8 -*-
#  Copyright (C) 2008,2009 by adenin
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
###############################################################################
ANSI_BLACK="\033[37;30m"
ANSI_RED="\033[37;31m"
ANSI_GREEN="\033[37;32m"
ANSI_YELLOW="\033[37;33m"
ANSI_BLUE="\033[37;34m"
ANSI_MAGENTA="\033[37;35m"
ANSI_CYAN="\033[37;36m"
ANSI_WITHE="\033[37;37m"
ANSI_RESET="\033[37;39m"
ANSI_RESET2="\033[0m"
###############################################################################
_cpVersion()
{
local i=1
local result=0
while [ : ]
do
	a=`echo $1 | $CUT -f$i -d "."`
	b=`echo $2 | $CUT -f$i -d "."`
	a1=$a
	b1=$b
	if (test "$a1" = "") then a1="0"; fi;
	if (test "$b1" = "") then b1="0"; fi;
	if [ $a1 -lt $b1 ]; then
		result=-1
		break
	else
		if [ $a1 -gt $b1 ]; then
			result=1
			break
		fi
	fi
	if (test "$a" = "") then
		break
	fi;
	if (test "$b" = "") then
		break
	fi;
	i=`expr $i + 1`
done
echo $result
}
###############################################################################
# parameter 1 : tool
# parameter 2 : command (--version -v ...)
_getVersion()
{
	echo `$1 $2 2>&1|grep  -o -e "\<[0-9]\+\([0-9]*[.]*\)\+" | grep -m1 -o -e "\<[0-9]\+\([0-9]*[.]*\)\+" `
}
###############################################################################
_buildTool_git()
{
#GNU Tools
BASE_URL="http://git.savannah.gnu.org/cgit/"
local TOOL=$1
local VERSION=$2
local CONFIGURE_PARAM=$3
local INSTALL_PARAM=$4


	wget $BASE_URL/$TOOL.git/snapshot/$TOOL-$VERSION.tar.gz

	if ( test $? -ne 0 ) then 
		echo "konnte Source nicht downloaden"
		exit 72
	fi;

	tar xvfz $TOOL-$VERSION.tar.gz
	if ( test $? -ne 0 ) then 
		echo "konnte Source nicht entpacken"
		exit 72
	fi;

	cd $TOOL-$VERSION

	if ( test ! -f "configure" ) then
		if ( test -f "bootstrap" ) then
			./bootstrap
		else
			exit
#			if ( test -f "configure.ac" ) then
#				autoconf
#			else
#				echo "kein configure.ac gefunden"
#				exit
#			fi
		fi
	fi

	./configure $CONFIGURE_PARAM

	make

	$SUDO make $INSTALL_PARAM install

	cd ..
}
###############################################################################
_buildTool()
{
#GNU Tools
BASE_URL="http://ftp.gnu.org/gnu"
local TOOL=$1
local VERSION=$2
local CONFIGURE_PARAM=$3
local INSTALL_PARAM=$4


	wget $BASE_URL/$TOOL/$TOOL-$VERSION.tar.gz
	wget $BASE_URL/$TOOL/$TOOL-$VERSION.tar.gz.sig
#	gpg --verify $TOOL-$VERSION.tar.gz.sig $TOOL-$VERSION.tar.gz

	if ( test $? -ne 0 ) then 
		echo "konnte Source nicht downloaden"
		exit 72
	fi;

	tar xvfz $TOOL-$VERSION.tar.gz
	if ( test $? -ne 0 ) then 
		echo "konnte Source nicht entpacken"
		exit 72
	fi;

	cd $TOOL-$VERSION

	if ( test ! -f "configure" ) then
		if ( test -f "configure.ac" ) then
#			make configure
			autoconf
		else
			echo "kein configure.ac gefunden"
			exit
		fi
	fi

	./configure $CONFIGURE_PARAM

	make

	$SUDO make $INSTALL_PARAM install

	cd ..
}
###############################################################################
PATH_PATTERN="[-[:alnum:]_.~/]*\>"
FILE_PATTERN="[-[:alnum:]_.]*\>"
###############################################################################
# search non quoted values
# parameter 1 : file
# parameter 2 : identifer
# parameter 2 : search pattern
# result      : value
# identifer = value
# identifer := value

_getValue()
{
	echo `cat $1|grep -o -e "^$2[[:blank:]]*:\{0,1\}=[[:blank:]]*.*"|grep -o -e "=[[:blank:]]*.*"|grep -o -e "$3"`
}
###############################################################################
# search double quoted values
# parameter 1 : file
# parameter 2 : identifier
# parameter 2 : search pattern
# result      : Value without double quotes
# identifer = "value"

_getValueDQ()
{
	echo `cat $1|grep -o -e"^$2[[:blank:]]*=[[:blank:]]*\"$3\""|grep -o -e"\"$3\""|grep -o -e"$3"`
}
###############################################################################
_getTag()
{
	echo `cat $1 | grep -e $2| grep -m 1 -e"commit"| grep -o -e "[0-9a-f]\{40,40\}"`
}
###############################################################################
_which()
{
	which $1 2> /dev/null
#	type -p $1
}
###############################################################################
_get_parameter()
{
local	i=1
local	param=""
local	distri=""
local	result=""
	while [ : ]
	do
		param=`echo $1 | $CUT -f$i -d ":"`
		i=`expr $i + 1`
		distri=`echo $1 | $CUT -f$i -s -d ":"`
		i=`expr $i + 1`
		if (test "$param" = "") then break; fi;
		result=$param
		if (test "$distri" = "") then break; fi;
		if (test "$distri" = "$LINUX_DISTRIBUTION") then break; fi;
		if (test "$distri" = "*") then break; fi;
	done
	echo $result
}
###############################################################################
_download_src()
{
#Download Bitbake 1.8-dream for OE 1.6
[ -d $INIT_DIR/openembedded/bb ] || mkdir -p $INIT_DIR/openembedded/bb
cd $INIT_DIR/openembedded/bb
if 	[ -d 1.8-dream/.git ]; then
	echo "Update bitbake 1.8-dream"
	cd 1.8-dream
	git pull
else
	echo "Download bitbake 1.8-dream"
	[ -d 1.8-dream ] && rm -rf 1.8-dream
	git clone git://git.opendreambox.org/git/bitbake.git 1.8-dream
	if ( test $? -ne 0 ) then
		echo -e $ANSI_RED"Fehler beim downloaden von bitbake-1.8-dream"$ANSI_RESET2
		exit 72		#Code für: wichtige Systemdatei fehlt
	fi;
	cd 1.8-dream
	git checkout -f 1.8-dream
fi

#Download enigma2 from branch experimental
[ -d $INIT_DIR/src ] || mkdir $INIT_DIR/src
cd $INIT_DIR/src
if [ -d $INIT_DIR/src/enigma2/.git ];then
	echo "Update enigma2 from branch experimental"
	cd enigma2
	git pull
else
	echo "Download enigma2 from branch experimental"
	[ -d $INIT_DIR/src/enigma2 ] && rm -rf $INIT_DIR/src/enigma2
	git clone git://git.opendreambox.org/git/enigma2.git
	if ( test $? -ne 0 ) then
		echo -e $ANSI_RED"Fehler beim downloaden von enigma2 source"$ANSI_RESET2
		exit 72		#Code für: wichtige Systemdatei fehlt
	fi;
	cd enigma2
	git checkout -f experimental
fi

#Download enigma2-plugins from branch master
cd $INIT_DIR/src
if [ -d $INIT_DIR/src/enigma2-plugins/.git ];then
	echo "Update enigma2-plugins from branch master"
	cd enigma2-plugins
	git pull
else
	echo "Download enigma2-plugins from branch master"
	[ -d $INIT_DIR/src/enigma2-plugins ] && rm -rf $INIT_DIR/src/enigma2-plugins
	git clone git://git.opendreambox.org/git/obi/enigma2-plugins.git
	if ( test $? -ne 0 ) then
		echo -e $ANSI_RED"Fehler beim downloaden von enigma2-plugins source"$ANSI_RESET2
		exit 72		#Code für: wichtige Systemdatei fehlt
	fi;
	cd enigma2-plugins
	git checkout -f master
fi

#Download enigma1
cd $INIT_DIR/src
if [ -d $INIT_DIR/src/enigma/CVS ]; then
	echo "Update enigma1 from CVS"
	cd $INIT_DIR/src/enigma
	cvs update -P -A
else
	echo "Download enigma1 from CVS"
	[ -d $INIT_DIR/src/enigma ] && rm -rf $INIT_DIR/src/enigma
	export CVS_RSH=ssh
	cvs -d anoncvs@cvs.tuxbox.org:/cvs/tuxbox co apps/tuxbox/enigma
	if ( test $? -ne 0 ) then
		echo -e $ANSI_RED"Fehler beim downloaden von enigma1 source"$ANSI_RESET2
		exit 72		#Code für: wichtige Systemdatei fehlt
	fi;
	mv apps/tuxbox/enigma .
	rm -rf apps/tuxbox
	rm -rf apps/CVS
fi

# clean src folder
cd $INIT_DIR/src
find . -name ".svn" | xargs rm -rf

}
###############################################################################
_create_makefile()
{
for DISTRO_VERSION in "1.5" "1.6"
do
	cd $OE_ROOT
	mkdir -p $DISTRO_VERSION
	cd $DISTRO_VERSION
   	case $DISTRO_VERSION in
		1.5)
			BRANCH="opendreambox-1.5"
			BB_VERSION="1.6.8a"
			PACKAGES="packages"
			GIT_URL="git://git.opendreambox.org/git/openembedded.git"
			;;
		1.6) 	
			BRANCH="opendreambox-1.6"
			BB_VERSION="1.8-dream"
			PACKAGES="recipes"
			GIT_URL="git://git.opendreambox.org/git/openembedded.git"
			;;
	esac
	for MACHINE in "dm500hd" "dm500plus" "dm600pvr" "dm7020" "dm7025" "dm800" "dm800se" "dm8000"
	do
		mkdir -p $MACHINE
		cd $MACHINE

		echo '#!/usr/bin/make -f' > Makefile-opendreambox
		echo '#' >> Makefile-opendreambox
		echo '#  Makefile-opendreambox v0.4.7 (2010-10-11)' >> Makefile-opendreambox
		echo '#' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo '# target platform: dm500hd, dm500plus, dm600pvr, dm7020, dm7025, dm800, dm800se, dm8000' >> Makefile-opendreambox
		echo 'MACHINE='$MACHINE >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo '# for a list of some other repositories have' >> Makefile-opendreambox
		echo '# a look at http://git.opendreambox.org/' >> Makefile-opendreambox
		echo 'GIT_URL = '$GIT_URL >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo '# in case you want to send pull requests or generate patches' >> Makefile-opendreambox
		echo '#GIT_AUTHOR_NAME ?= xyz' >> Makefile-opendreambox
		echo '#GIT_AUTHOR_EMAIL ?= xyz@mail.net' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo '# you should not need to change anything below' >> Makefile-opendreambox
		echo 'BB_VERSION = '$BB_VERSION >> Makefile-opendreambox
		echo 'DISTRO_VERSION = '$DISTRO_VERSION >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'GIT = git' >> Makefile-opendreambox
		echo 'GIT_BRANCH = '$BRANCH >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'CPU_COUNT := $(shell grep processor /proc/cpuinfo | wc -l)' >> Makefile-opendreambox
		echo 'PWD := $(shell pwd)' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		if ( test "$DISTRO_VERSION" = "1.6" ); then
			echo 'ifeq ($(findstring $(MACHINE),dm500plus dm600pvr dm7020),$(MACHINE))' >> Makefile-opendreambox
			echo 'ARCH = powerpc' >> Makefile-opendreambox
			echo 'else' >> Makefile-opendreambox
			echo 'ARCH = mipsel' >> Makefile-opendreambox
			echo 'endif' >> Makefile-opendreambox
			echo '' >> Makefile-opendreambox
		fi
		echo 'OE_ROOT:='$OE_ROOT >> Makefile-opendreambox
		echo 'OE_BASE = $(PWD)' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'all: initialize' >> Makefile-opendreambox
		echo '	@echo ' >> Makefile-opendreambox
		echo '	@echo "Openembedded for the Dreambox environment has been initialized"' >> Makefile-opendreambox
		echo '	@echo "properly. Now you can either:"' >> Makefile-opendreambox
		echo '	@echo' >> Makefile-opendreambox
		echo '	@echo "  - make the 'image'-target to build an image, or"' >> Makefile-opendreambox
		echo '	@echo "  - go into $(MACHINE)/build/, source env.source and start on your own!"' >> Makefile-opendreambox
		echo '	@echo ' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo '.PHONY: image initialize openembedded-update' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'image: initialize openembedded-update' >> Makefile-opendreambox
		echo '	cd build; . ./env.source; bitbake dreambox-image' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'initialize: cache build build/conf build/conf/local.conf build/env.source ${OE_ROOT}/${DISTRO_VERSION}/openembedded' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'openembedded-update: ${OE_ROOT}/${DISTRO_VERSION}/openembedded' >> Makefile-opendreambox
		echo '	cd ${OE_ROOT}/${DISTRO_VERSION}/openembedded && $(GIT) pull' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'cache build/conf build:' >> Makefile-opendreambox
		echo '	mkdir -p $@' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'build/conf/local.conf:' >> Makefile-opendreambox
		echo "	echo 'DL_DIR = \"\$(OE_ROOT)/sources\"' > \$@" >> Makefile-opendreambox
		echo "	echo 'OE_BASE = \"\$(PWD)\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'BBFILES = \"\${OE_ROOT}/\${DISTRO_VERSION}/openembedded/${PACKAGES}/*/*.bb\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'BBMASK = \"(iphone.*|nslu.*|.*-sdk.*)\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'PREFERRED_PROVIDERS += \" virtual/\$\${TARGET_PREFIX}gcc-initial:gcc-cross-initial\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'PREFERRED_PROVIDERS += \" virtual/\$\${TARGET_PREFIX}gcc:gcc-cross\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'PREFERRED_PROVIDERS += \" virtual/\$\${TARGET_PREFIX}g++:gcc-cross\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'MACHINE = \"\$(MACHINE)\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'TARGET_OS = \"linux\"' >> \$@" >> Makefile-opendreambox
		case $DISTRO_VERSION in
			1.5)	echo "	echo 'DISTRO = \"opendreambox-\${DISTRO_VERSION}\"' >> \$@" >> Makefile-opendreambox;;
			1.6)	echo "	echo 'DISTRO = \"opendreambox\"' >> \$@" >> Makefile-opendreambox;;
		esac
		echo "	echo 'CACHE = \"\$(OE_BASE)/cache/oe-cache.\$\${USER}\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'OE_ALLOW_INSECURE_DOWNLOADS = \"1\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'BB_NUMBER_THREADS = \"\$(CPU_COUNT)\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'CVS_TARBALL_STASH = \"http://dreamboxupdate.com/sources-mirror/\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'TOPDIR = \"\$\${OE_BASE}/build\"' >> \$@" >> Makefile-opendreambox
		echo "	echo 'IMAGE_KEEPROOTFS = \"0\"' >> \$@" >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo 'build/env.source:' >> Makefile-opendreambox
		echo "	echo 'OE_ROOT=\${OE_ROOT}' > \$@" >> Makefile-opendreambox
		echo "	echo 'OE_BASE=\$(PWD)' >> \$@" >> Makefile-opendreambox
		echo "	echo 'export BBPATH=\"\$\${OE_ROOT}/\${DISTRO_VERSION}/openembedded/:\$\${OE_ROOT}/bb/\${BB_VERSION}/:\$\${OE_BASE}/build/\"' >> \$@" >> Makefile-opendreambox
		case $DISTRO_VERSION in
			1.5)	echo "	echo 'PATH=\$\${OE_ROOT}/bb/\${BB_VERSION}/bin:\$\${OE_ROOT}/bin:\$\${OE_BASE}/build/tmp/cross/bin:\$\${PATH}' >> \$@" >> Makefile-opendreambox;;
			1.6)	echo "	echo 'PATH=\$\${OE_ROOT}/bb/\${BB_VERSION}/bin:\$\${OE_ROOT}/bin:\$\${OE_BASE}/build/tmp/cross/\${ARCH}/bin:\$\${PATH}' >> \$@" >> Makefile-opendreambox;;
		esac
		echo "	echo 'export PATH' >> \$@" >> Makefile-opendreambox
		echo "	echo 'export LD_LIBRARY_PATH=' >> \$@" >> Makefile-opendreambox
		echo "	echo 'export LANG=C' >> \$@" >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo '${OE_ROOT}/${DISTRO_VERSION}/openembedded: ${OE_ROOT}/${DISTRO_VERSION}/openembedded/.git' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox
		echo '${OE_ROOT}/${DISTRO_VERSION}/openembedded/.git:' >> Makefile-opendreambox
		echo '	cd build; . ./env.source; cd ..' >> Makefile-opendreambox
		echo '	@if [ -d $(OE_ROOT)/${DISTRO_VERSION}/openembedded/_MTN ]; then echo "Please remove your old monotone repository from $(OE_ROOT)/${DISTRO_VERSION}/openembedded!"; exit 1; fi' >> Makefile-opendreambox
		echo '	$(GIT) clone -n $(GIT_URL) $(OE_ROOT)/${DISTRO_VERSION}/openembedded' >> Makefile-opendreambox
		echo '	cd $(OE_ROOT)/${DISTRO_VERSION}/openembedded && ( \' >> Makefile-opendreambox
		echo '		if [ -n "$(GIT_AUTHOR_EMAIL)" ]; then git config user.email "$(GIT_AUTHOR_EMAIL)"; fi; \' >> Makefile-opendreambox
		echo '		if [ -n "$(GIT_AUTHOR_NAME)" ]; then git config user.name "$(GIT_AUTHOR_NAME)"; fi; \' >> Makefile-opendreambox
		echo '		$(GIT) checkout -f $(GIT_BRANCH) \' >> Makefile-opendreambox
		echo '		)' >> Makefile-opendreambox
		echo '' >> Makefile-opendreambox

		filename="make_init.sh"
		echo '#!/bin/sh' > $filename
		echo "ENVPATH=$INIT_DIR" >> $filename
		echo "MACHINE=$MACHINE" >> $filename
		echo 'make -f Makefile-opendreambox initialize' >> $filename
		echo "cd $INIT_DIR/nemesis" >> $filename
		echo './src_patch.sh script' >> $filename
   	case $DISTRO_VERSION in
		1.5)
			echo './src_patch.sh oe_conf_15' >> $filename
			echo './src_patch.sh oe_pack_15' >> $filename
			;;
		1.6) 	
			echo './src_patch.sh oe_conf_16' >> $filename
			echo './src_patch.sh oe_pack_16' >> $filename
			;;
		esac
		echo "cd $OE_ROOT/$DISTRO_VERSION/$MACHINE" >> $filename
		echo 'rm -f build/env.source.backup*' >> $filename
		echo 'rm -f build/conf/local.conf.backup*' >> $filename
		echo '[ -e cache ] && rm -rf cache' >> $filename
		chmod 755 $filename

		cd $OLDPWD
	done
done
}
###############################################################################
_check_uid()
{
	if [ ! `id -u` == "0" ]; then
		echo -e $ANSI_RED"\nkeine Rootrechte\n"$ANSI_RESET
	else
		echo -e $ANSI_GREEN"\033[37;32m\nok,we are root :)\n"$ANSI_RESET
	fi
}
###############################################################################
_get_linuxdistribution()
{
	local TEMP=""
	local LINUX_DISTRIBUTION=""

	for LINUX_DISTRIBUTION in "Debian" "Mandrake" "Mandriva" "Fedora" "Red Hat" "SuSE" "Ubuntu" "unknown" 
	do
		TMP=`cat /proc/version | $GREP -i "$LINUX_DISTRIBUTION"`
		if (test "$TMP" != "") then
			LINUX_DISTRIBUTION=`echo $LINUX_DISTRIBUTION | sed -e 's/[ ]//g'`
			break
		fi;
	done
	echo $LINUX_DISTRIBUTION
}
###############################################################################
# ist apt-get installiert und existiert /ets/apt/sources.list, dann benutze apt-get
# ist yast installiert, dann benutze yast
# ist yum installiert und existiert /etc/yum.repos.d , dann benutze yum
_setinstallcmd()
{
SUDO=`_which sudo`
YUM=$(_which yum)
APTGET=$(_which apt-get)
YAST=$(_which yast)
URPMI=$(_which urpmi)
URPME=$(_which urpme)

if ( test "$APTGET" != "" ) then
	echo "apt-get gefunden"
	if ( test ! -s /ets/apt/sources.list ) then
		echo "sources.list gefunden"
		INSTALL_CMD="$APTGET -y --force-yes install "
		REMOVE_CMD="$APTGET -y remove "
		UPDATE_CMD="$APTGET -y update"
		UPTODATE=""
	fi;
elif ( test "$YUM" != "" ) then
	echo "yum gefunden"
	if ( test ! -s etc/yum.repos.d ) then
		echo "yum.repos.d gefunden"
		INSTALL_CMD="$YUM -y install "
		REMOVE_CMD="$YUM -y remove "
		UPDATE_CMD=""
		UPTODATE="y"
	fi;
elif ( test "$YAST" != "" ) then
	echo "yast gefunden"
	INSTALL_CMD="$YAST --install "
	REMOVE_CMD="$YAST --remove "
	UPDATE_CMD=""
	UPTODATE="y"
elif ( test "$URPMI" != "" ) then
	echo "urpmi gefunden"
	INSTALL_CMD="$URPMI "
	REMOVE_CMD="$URPME "
	UPDATE_CMD=""
	UPTODATE="y"
else
	echo -e $ANSI_RED"keine Packetverwaltung (apt-get, yum, yast, urpmi) gefunden"$ANSI_RESET
	exit 72		#Code für: wichtige Systemdatei fehlt
fi;	
echo "INSTALL_CMD = $INSTALL_CMD" > install.helper
echo "REMOVE_CMD = $REMOVE_CMD" >> install.helper
echo "UPDATE_CMD = $UPDATE_CMD" >> install.helper
echo "UPTODATE = $UPTODATE" >> install.helper
echo "SUDO = $SUDO" >> install.helper
chmod 666 install.helper
}
###############################################################################
_doasroot()
{
_check_uid

LINUX_DISTRIBUTION=$(_get_linuxdistribution)

echo $LINUX_DISTRIBUTION

###############################################################################
_setinstallcmd
##########################################################
#zuerst dafür sorgen, dass bash als Shell gesetzt ist
##########################################################
TMP=`ls -l /bin/sh | $GREP "bash"`
if ( test "$TMP" != "") then
	echo "bash ist korrekt gesetzt"
else
	echo "bash wird gesetzt"
	TMP=`whereis bash | $GREP "/bin/bash"`
	echo $TMP
	if (test "$TMP" != "") then
		echo "bash ist installiert"
	else
		echo "installiere bash"
		$INSTALL_CMD bash
	fi;
	echo "verlinke bash"
	rm -f /bin/sh
	ln -s /bin/bash /bin/sh	# ln -f gibt es nicht immer, deswegen vorher rm
	./install.sh --install-tools
	exit
fi;

##########################################################
echo "Teste rsh"
TMP=`type -p rsh`
if ( test "$TMP" != "") then
	echo "rsh ok"
else
	echo "rsh wird verlinkt"
	SSHPATH=`type -p ssh | $GREP "/bin/ssh"`
	echo $SSHPATH
	if (test "$SSHPATH" != "") then
		echo "ssh ist installiert"
	else
		echo "installiere ssh"
		$INSTALL_CMD ssh
	fi;
	echo "verlinke rsh"
	SSHPATH=`type -p ssh | $GREP "/bin/ssh"`
	if (test "$SSHPATH" != "") then
		rm -f /bin/rsh
		ln -s $SSHPATH /bin/rsh		# ln -f gibt es nicht immer, deswegen vorher rm
	else
		echo "rsh konnte nicht verlinkt werden"
	fi;
fi;

##########################################################
# nötige Libraries
##########################################################
#TODO!!!! das ist im Moment nur für Linux mit apt-get und yast getestet!!!
if [ ! `id -u` == "0" ]; then
	echo -e "\a"
fi;
case $LINUX_DISTRIBUTION in
    'SuSE')		for todo in "libid3tag-devel" \
			"libsigc++2-devel" \
			"libpng-devel" \
			"libjpeg-devel" \
			"libcurl-devel" \
			"libxml2-devel" \
			"libxslt-devel" \
			"libexpat-devel"
			do
				if [ ! `id -u` == "0" ]; then
					echo -e "\a"
				fi;
				$INSTALL_CMD $todo
				if ( test $? -ne 0 ) then 
					echo -e $ANSI_YELLOW"Fehler beim installieren der Libary $todo"$ANSI_RESET2
				fi;
			done;;
    'Fedora')		for todo in "libid3tag-devel" \
			"libsigc++20-devel" \
			"libpng-devel" \
			"libjpeg-devel" \
			"libcurl-devel" \
			"libxml2-devel" \
			"libxml2-static" \
			"libxslt-devel" \
			"glibc-devel" \
			"glibc-static" \
			"glibc-utils" \
			"glibc-common" \
			"docbook2X" \
			"expat-devel"
			do
				if [ ! `id -u` == "0" ]; then
					echo -e "\a"
				fi;
				$INSTALL_CMD $todo
				if ( test $? -ne 0 ) then 
					echo -e $ANSI_YELLOW"Fehler beim installieren der Libary $todo"$ANSI_RESET2
				fi;
			done;;

	*)		for todo in "libfribidi-dev" \
			"python-pysqlite2" \
			"libid3tag0-dev" \
			"libstdc++6-4.1-dev" \
			"libstdc++6-4.2-dev" \
			"libstdc++6-4.3-dev" \
			"libstdc++6-4.4-dev" \
			"libgnutls-dev" \
			"libmad0-dev" \
			"libsigc++-1.2-dev" \
			"libungif4-dev" \
			"libpng12-dev" \
			"libjpeg-dev" \
			"libcurl4-openssl-dev" \
			"libcurl3-openssl-dev" \
			"libxml2-dev" \
			"libxslt1-dev" \
			"libexpat-dev" \
			"libexpat1-dev" \
			"libfreetype6-dev"
			do
				if [ ! `id -u` == "0" ]; then
					echo -e "\a"
				fi;
				$INSTALL_CMD $todo
				if ( test $? -ne 0 ) then 
					echo -e $ANSI_YELLOW"Fehler beim installieren der Libary $todo"$ANSI_RESET2
				fi;
			done;;
esac

##########################################################
#Sourceliste checken
#	sed -e '/http:/s/# deb/deb/g' /etc/apt/sources.list > /tmp/test
#	sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak
#	sudo mv /tmp/test /etc/apt/sources.list

##########################################################
#Tools prüfen und installieren

#Aufbau von TOOLPARAMETER am Beispiel für makeinfo
# Name des Tool									makeinfo
# Kommando zum erfragen der Version						--version
# die Version, die mindestens installiert sein muss				0
# Package-Name, wenn dieser anders als der Toolname ist (z.B. bei makeinfo)	texinfo

for TOOLPARAMETER in \
	"sudo -V 1.6" \
	"bash --version 3.2.39" \
	"mc --version 0" \
	"cvs --version 1.12.12" \
	"autoconf --version 2.57" \
	"automake --version 1.10" \
	"libtool --version 1.4.2" \
	"gettext --version 0.17" \
	"make --version 3.79" \
	"makeinfo --version 0 texinfo" \
	"tar --version 0" \
	"gunzip --version 0" \
	"patch --version 0" \
	"infocmp -V 0 ncurses-devel:SuSE:ncurses-bin" \
	"gcc --version  3.0" \
	"g++ --version  3.0 gcc-c++:SuSE:gcc-c++:RedHat:g++" \
	"bison --version  0" \
	"pkg-config --version 0.2" \
	"wget --version 0" \
	"help2man --version 0" \
	"diffstat --version 0" \
	"texi2html --version 0" \
	"gawk --version 0" \
	"gettext --version 0" \
	"bunzip2 --version 0 bzip2" \
	"python -V 2.4.3" \
	"uudecode --version 0 sharutils" \
	"swig -version 1.3.36" \
	"flex --version 2.3" \
	"asciidoc --version 8.2.2" \
	"xmlto --version 0.0.18" \
	"db2x_xsltproc --version 0.8.7 docbook2x" \
	"svn --version 0 subversion"

#todo
###psyco jit compiler

do
	TOOL=`echo $TOOLPARAMETER | $CUT -f 1 -d " "`
	VERSION_CMD=`echo $TOOLPARAMETER | $CUT -f 2 -d " "`
	MIN_VERSION=`echo $TOOLPARAMETER | $CUT -f 3 -d " "`

	TMP=`echo $TOOLPARAMETER | $CUT -f 4 -d " "`
	if (test "$TMP" = "") then
		INSTALL_PACKAGE=$TOOL
	else
		INSTALL_PACKAGE=`_get_parameter $TMP`
	fi;

#	echo $TOOL $VERSION_CMD
	TOOLPATH=`_which $TOOL`
	if (test "$TOOLPATH" != "") then
		CURRENT_VERSION=$(_getVersion $TOOL $VERSION_CMD)
		echo "$TOOL: "$CURRENT_VERSION
		if ( test "$(_cpVersion "$CURRENT_VERSION" "$MIN_VERSION")" = "-1" ); then
			echo "$CURRENT_VERSION < $MIN_VERSION"
			echo "Version veraltet"
			echo "update"
			if (test "$UPTODATE" != "y"); then
				UPTODATE="y"
				$UPDATE_CMD
			fi
			$INSTALL_CMD $INSTALL_PACKAGE
		fi
	else
		echo "installiere Package $INSTALL_PACKAGE für Tool $TOOL"
		if (test "$UPTODATE" != "y"); then
			UPTODATE="y"
			$UPDATE_CMD
		fi;
		$INSTALL_CMD $INSTALL_PACKAGE
		#Kontrolle, ob das Tool nun installiert ist
		TOOLPATH=`_which $TOOL`
		if (test "$TOOLPATH" != "") then
			echo "$TOOL: "`$TOOL $VERSION_CMD | $GREP $SEARCHPATTERN | $CUT -f$POSITION -d " "`
		else
			echo -e "\033[37;41m$TOOL lies sich nicht installieren.\033[37;0m"
			exit 72
		fi;
	fi;
done
exit 0
}
###############################################################################
###############################################################################
INIT_DIR=$PWD
CUT=`_which cut`
GREP=`_which grep`
SUDO=`_which sudo`
###############################################################################

if (test "$#" -ge 2) then
	echo "install.sh Version $INSTALL_VERSION"
	exit
fi;

if (test "$#" = 1) then
	case $1 in
	'--install-tools')
		_doasroot
		exit;;
	'--version')	
		echo "install.sh Version $INSTALL_VERSION"
		exit;;
	'--download')
		_download_src
		exit;;
	'--makefile')
		cd $INIT_DIR
		mkdir -p openembedded
		cd openembedded
		OE_ROOT=$PWD
		_create_makefile
		exit;;
	*)		
		echo "install.sh Version $INSTALL_VERSION"
		echo "helptext blablabla"
		exit;;
	esac
fi;

if [ ! `id -u` == "0" ]; then
	echo -e "\a"
fi;
if ( test "$SUDO" == "") then
	su -c "./install.sh --install-tools"
	installresult=$?
else
	sudo ./install.sh --install-tools
	installresult=$?
	sudo -k
		if ( test "$installresult" = 1 ) then
			echo -e "\nHmm...,\nalso wenn 'sudo' nicht geht, dann versuchen wir es doch mal mit 'su' ;)\n"
			su -c "./install.sh --install-tools"
			installresult=$?
		fi;
fi;
if ( test "$installresult" -ne 0 ) then
	echo "Abbruch"
	exit
fi;

FILE="install.helper"
SUDO=$(_getValue $FILE "SUDO" "$PATH_PATTERN")
INSTALL_CMD=$(_getValue $FILE "INSTALL_CMD" "$PATH_PATTERN")
REMOVE_CMD=$(_getValue $FILE "REMOVE_CMD" "$PATH_PATTERN")
UPDATE_CMD=$(_getValue $FILE "UPDATE_CMD" "$PATH_PATTERN")
rm -f install.helper
################################################################################################################
################################################################################################################
cd $INIT_DIR
mkdir -p cvs
cd cvs
CVS_ROOT=$PWD
for old_db in "dm500" "dm7000" "dm56x0"
do
	filename="make_init.sh"
	cd $CVS_ROOT
	mkdir -p $old_db
	[ -d Archive ] || mkdir Archive
	cd $old_db
	echo '#!/bin/sh' > $filename
	echo "MODEL=$old_db" >> $filename
	echo "OEROOT=$INIT_DIR" >> $filename
	echo 'export CVS_RSH=ssh' >> $filename
	echo 'cvs -d anoncvs@cvs.tuxbox.org:/cvs/tuxbox -z3 co -P -rdreambox .' >> $filename
	echo 'cd cdk' >> $filename
	echo 'ln -s ../../Archive' >> $filename
	echo 'chmod ugo+x prepare' >> $filename
	echo "./prepare $old_db" >> $filename
	echo 'make checkout' >> $filename
	echo "cd $INIT_DIR/nemesis" >> $filename
	echo './src_patch.sh script' >> $filename
	echo "cd $CVS_ROOT/$old_db/cdk" >> $filename
	echo './make_cvs_image.sh patch_cdk' >> $filename
	chmod 755 $filename
	cd $OLDPWD
done
################################################################################################################
cd $INIT_DIR
mkdir -p openembedded
cd openembedded
OE_ROOT=$PWD
#------------------------------------------------------------------------------
#neues git bauen
#_setinstallcmd
GIT_PATH=`_which git`
if ( test "$GIT_PATH" = "") then
	GIT_VERSION="0"
else
	GIT_VERSION=$(_getVersion "git" "--version")
fi;
GIT_REQUEST='1.6.5.6'

CPU=`cat /proc/cpuinfo | grep -e "^system type" | grep -o -e "BCM97xxx"`
if (test "$CPU" = "BCM97xxx") then
	CONFIGURE_PARAM="--build=mipsel-debian-linux-gnu --host=mipsel-linux"
	MAKE_PARAM="NO_NSEC=1 prefix=$OE_ROOT all"
	INSTALL_PARAM="NO_NSEC=1 prefix=$OE_ROOT all"
else
	MAKE_PARAM="prefix=/usr all"
	INSTALL_PARAM="prefix=/usr all"
fi

CURRENT_VERSION=$GIT_VERSION
MIN_VERSION=$GIT_REQUEST
echo "git:"$GIT_VERSION
if ( test "$(_cpVersion "$CURRENT_VERSION" "$MIN_VERSION")" = "-1" ); then
	echo "$CURRENT_VERSION < $MIN_VERSION"
	echo "Version veraltet"
	echo "update"
	if [ ! `id -u` == "0" ]; then
		echo -e "\a"
	fi;
	sudo $INSTALL_CMD libssl-dev
	sudo $INSTALL_CMD openssl-devel
	if [ ! `id -u` == "0" ]; then
		echo -e "\a"
	fi;
	#;.................................................
	echo "erste Variante: git"
	git clone git://git.kernel.org/pub/scm/git/git.git
	if ( test $? -ne 0 ) then 
		# wir hholen das git über das http-Protokoll , weil das auch hinter Firewalls immer geht
		echo "zweite Variante: http"
		git clone http://www.kernel.org/pub/scm/git/git.git
	fi;
	if [ ! `id -u` == "0" ]; then
		echo -e "\a"
	fi;
	sudo $REMOVE_CMD git git-core

	if ( test ! -d "git" ) then
		if ( test ! -d "git-${GIT_REQUEST}.tar.gz" ) then
			echo "dritte Variante: source"
			wget http://kernel.org/pub/software/scm/git/git-${GIT_REQUEST}.tar.gz
			if ( test $? -ne 0 ) then 
				echo "konnte git Source nicht downloaden"
				exit 72
			fi;
			tar xvfz git-${GIT_REQUEST}.tar.gz
			if ( test $? -ne 0 ) then 
				echo "konnte git Source nicht entpacken"
				exit 72
			fi;
			cd git-${GIT_REQUEST}
		fi;
	else
		cd git
	fi;

	make configure
	./configure $CONFIGURE_PARAM
	make $MAKE_PARAM
	echo -e $ANSI_GREEN"\ninstalliere git"$ANSI_RESET2
	$SUDO make $INSTALL_PARAM install
	cd ..
	#rm -rf git
	#;.................................................
fi
#------------------------------------------------------------------------------
#Bitbake holen und für den Gebrauch in Python 2.6 patchen
mkdir -p bb
cd bb
SVN_BITBAKE_PARENT="svn://svn.berlios.de/bitbake/"
for BB_VERSION in "1.6.8" "1.8.12"
do
	echo "download bitbake-${BB_VERSION}"
	svn checkout ${SVN_BITBAKE_PARENT}/tags/bitbake-${BB_VERSION} ${BB_VERSION}
	if ( test $? -ne 0 ) then
		wget http://download.berlios.de/bitbake/bitbake-${BB_VERSION}.tar.gz
		if ( test $? -ne 0 ) then
			wget http://download2.berlios.de/bitbake/bitbake-${BB_VERSION}.tar.gz
			if ( test $? -ne 0 ) then
				echo -e $ANSI_RED"Fehler beim downloaden von bitbake-${BB_VERSION}"$ANSI_RESET2
				exit 72		#Code für: wichtige Systemdatei fehlt
			fi;
		fi;
		tar xvfz bitbake-${BB_VERSION}.tar.gz
		mv bitbake-${BB_VERSION} ${BB_VERSION}
	fi;
	echo "patch bitbake-${BB_VERSION}"
	if ( test -d ${BB_VERSION} ) then
		rm -fr ${BB_VERSION}a
		cp -r ${BB_VERSION} ${BB_VERSION}a
		CUT1CMD="1,/start-file-${BB_VERSION}a/ d"
		CUT2CMD="/end-file-${BB_VERSION}a/,$ d"
		sed -e "$CUT1CMD" -e "$CUT2CMD" < $INIT_DIR/install.sh | uudecode -o ${BB_VERSION}a.diff
		patch -Nsp1 -d ${BB_VERSION}a < ${BB_VERSION}a.diff
	else
		echo -e $ANSI_RED"konnte Verzeichnis $PWD/${BB_VERSION} nicht finden"$ANSI_RESET2
		exit 72		#Code für: wichtige Systemdatei fehlt
	fi;
done

#Python-Version testen
PYTHON_VERSION=$(_getVersion "python" "-V")
if [ `echo $PYTHON_VERSION |$CUT -f1 -d "."` -gt 2 ]; then
	echo -e "Python $PYTHON_VERSION wird möglicherweise nicht unterstützt\nDie letzte getestete Version war Python 2.6"
	USE_BB_PATCH="y"
fi;
if [ `echo $PYTHON_VERSION |$CUT -f1 -d "."` = 2 ]; then
	if [ `echo $PYTHON_VERSION |$CUT -f2 -d "."` -gt 5 ]; then
		USE_BB_PATCH="y"
	fi;
fi;
#------------------------------------------------------------------------------
#das neuste tar bauen
TAR_VERSION=$(_getVersion "tar" "--version")
CPU=`cat /proc/cpuinfo | grep -e "^system type" | grep -o -e "BCM97xxx"`
if (test "$CPU" = "BCM97xxx") then
	CONFIGURE_PARAM="--build=mipsel-debian-linux-gnu --host=mipsel-linux"
fi

CURRENT_VERSION=$TAR_VERSION
MIN_VERSION='1.22.90'
echo "tar: "$CURRENT_VERSION
if ( test "$(_cpVersion "$CURRENT_VERSION" "$MIN_VERSION")" = "-1" ); then
	echo "$CURRENT_VERSION < $MIN_VERSION"
	echo "Version veraltet"
	echo "update"
	_buildTool_git "tar" "master" "$CONFIGURE_PARAM" "prefix=/" "$SUDO"
fi
#------------------------------------------------------------------------------
#neues gettext bauen
#_setinstallcmd
GETTEXT_PATH=`_which gettext`
if ( test "$GETTEXT_PATH" = "") then
	GETTEXT_VERSION="0"
else
	GETTEXT_VERSION=$(_getVersion "gettext" "--version")
fi;
GETTEXT_REQUEST='0.17'

CURRENT_VERSION=$GETTEXT_VERSION
MIN_VERSION=$GETTEXT_REQUEST
echo "gettext: "$CURRENT_VERSION
if ( test "$(_cpVersion "$CURRENT_VERSION" "$MIN_VERSION")" = "-1" ); then
	echo "$CURRENT_VERSION < $MIN_VERSION"
	echo "Version veraltet"
	echo "update"
	_buildTool "gettext" "0.17" "--exec-prefix=/usr"
fi
#------------------------------------------------------------------------------
#sed 4.1.5 bauen weil sed 4.2.1 fehlerhaft ist
SED_VERSION=$(_getVersion "sed" "--version")
CPU=`cat /proc/cpuinfo | grep -e "^system type" | grep -o -e "BCM97xxx"`
if (test "$CPU" = "BCM97xxx") then
	CONFIGURE_PARAM="--build=mipsel-debian-linux-gnu --host=mipsel-linux"
fi
#_buildTool "sed" "4.1.5" "$CONFIGURE_PARAM --exec-prefix=$OE_ROOT"
CURRENT_VERSION=$SED_VERSION
MIN_VERSION='4.1.5'
if ( test "$(_cpVersion "$CURRENT_VERSION" "$MIN_VERSION")" != "0" ); then
	echo "$CURRENT_VERSION != $MIN_VERSION"
	_buildTool "sed" "4.1.5" "$CONFIGURE_PARAM --exec-prefix=/"
fi;

_download_src
_create_makefile

curdir=`pwd`
cd $INIT_DIR
for MACHINE in "500hd" "7025" "800" "800se" "8000" "7020hd"
do
		[ -e $MACHINE ] || ln -s openembedded/1.6/dm$MACHINE $MACHINE
done
for MACHINE in "500plus" "7020" "600pvr"
do
		[ -e $MACHINE ] || ln -s openembedded/1.5/dm$MACHINE $MACHINE
done

cd $curdir

echo -e "\033[37;32mFertig.\nHave fun.\a\033[37;0m"
sleep 1
echo -e "\a"
sleep 1
echo -e "\a"
exit
start-file-1.6.8a
begin-base64 644 1.6.8a.diff
ZGlmZiAtTmF1ciAxLjYuOC9iaW4vYml0YmFrZSAxLjYuOGEvYmluL2JpdGJh
a2UKLS0tIDEuNi44L2Jpbi9iaXRiYWtlCTIwMDktMDEtMDIgMTM6NDA6MDYu
MDAwMDAwMDAwICswMTAwCisrKyAxLjYuOGEvYmluL2JpdGJha2UJMjAwOC0x
Mi0yOCAwMDoxOTo0NS4wMDAwMDAwMDAgKzAxMDAKQEAgLTI2LDEyICsyNiwx
NSBAQAogc3lzLnBhdGguaW5zZXJ0KDAsb3MucGF0aC5qb2luKG9zLnBhdGgu
ZGlybmFtZShvcy5wYXRoLmRpcm5hbWUoc3lzLmFyZ3ZbMF0pKSwgJ2xpYicp
KQogaW1wb3J0IGJiCiBmcm9tIGJiIGltcG9ydCB1dGlscywgZGF0YSwgcGFy
c2UsIGV2ZW50LCBjYWNoZSwgcHJvdmlkZXJzCi1mcm9tIHNldHMgaW1wb3J0
IFNldAogaW1wb3J0IGl0ZXJ0b29scywgb3B0cGFyc2UsIHNyZV9jb25zdGFu
dHMKK3RyeToKKyAgICBzZXQKK2V4Y2VwdCBOYW1lRXJyb3I6CisgICAgZnJv
bSBzZXRzIGltcG9ydCBTZXQgYXMgc2V0CiAKIHBhcnNlc3BpbiA9IGl0ZXJ0
b29scy5jeWNsZSggcid8Ly1cXCcgKQogCi1fX3ZlcnNpb25fXyA9ICIxLjYu
OCIKK19fdmVyc2lvbl9fID0gIjEuNi44YSIKIAogIz09PT09PT09PT09PT09
PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09
PT09PT09PT09PT09PT09PT0jCiAjIEJCUGFyc2luZ1N0YXR1cwpAQCAtNTIs
MTMgKzU1LDEzIEBACiAgICAgICAgIHNlbGYuYmJmaWxlX2NvbmZpZ19wcmlv
cml0aWVzID0gW10KICAgICAgICAgc2VsZi5pZ25vcmVkX2RlcGVuZGVuY2ll
cyA9IE5vbmUKICAgICAgICAgc2VsZi5wb3NzaWJsZV93b3JsZCA9IFtdCi0g
ICAgICAgIHNlbGYud29ybGRfdGFyZ2V0ID0gU2V0KCkKKyAgICAgICAgc2Vs
Zi53b3JsZF90YXJnZXQgPSBzZXQoKQogICAgICAgICBzZWxmLnBrZ19wbiA9
IHt9CiAgICAgICAgIHNlbGYucGtnX2ZuID0ge30KICAgICAgICAgc2VsZi5w
a2dfcHZwciA9IHt9CiAgICAgICAgIHNlbGYucGtnX2RwID0ge30KICAgICAg
ICAgc2VsZi5wbl9wcm92aWRlcyA9IHt9Ci0gICAgICAgIHNlbGYuYWxsX2Rl
cGVuZHMgPSBTZXQoKQorICAgICAgICBzZWxmLmFsbF9kZXBlbmRzID0gc2V0
KCkKICAgICAgICAgc2VsZi5idWlsZF9hbGwgPSB7fQogICAgICAgICBzZWxm
LmRlcHMgPSB7fQogICAgICAgICBzZWxmLnJ1bmRlcHMgPSB7fQpAQCAtNzgs
NyArODEsNyBAQAogICAgICAgICBwdiAgICAgICA9IGJiX2NhY2hlLmdldFZh
cignUFYnLCBmaWxlX25hbWUsIFRydWUpCiAgICAgICAgIHByICAgICAgID0g
YmJfY2FjaGUuZ2V0VmFyKCdQUicsIGZpbGVfbmFtZSwgVHJ1ZSkKICAgICAg
ICAgZHAgICAgICAgPSBpbnQoYmJfY2FjaGUuZ2V0VmFyKCdERUZBVUxUX1BS
RUZFUkVOQ0UnLCBmaWxlX25hbWUsIFRydWUpIG9yICIwIikKLSAgICAgICAg
cHJvdmlkZXMgID0gU2V0KFtwbl0gKyAoYmJfY2FjaGUuZ2V0VmFyKCJQUk9W
SURFUyIsIGZpbGVfbmFtZSwgVHJ1ZSkgb3IgIiIpLnNwbGl0KCkpCisgICAg
ICAgIHByb3ZpZGVzICA9IHNldChbcG5dICsgKGJiX2NhY2hlLmdldFZhcigi
UFJPVklERVMiLCBmaWxlX25hbWUsIFRydWUpIG9yICIiKS5zcGxpdCgpKQog
ICAgICAgICBkZXBlbmRzICAgPSAgYmIudXRpbHMuZXhwbG9kZV9kZXBzKGJi
X2NhY2hlLmdldFZhcigiREVQRU5EUyIsIGZpbGVfbmFtZSwgVHJ1ZSkgb3Ig
IiIpCiAgICAgICAgIHBhY2thZ2VzICA9IChiYl9jYWNoZS5nZXRWYXIoJ1BB
Q0tBR0VTJywgZmlsZV9uYW1lLCBUcnVlKSBvciAiIikuc3BsaXQoKQogICAg
ICAgICBwYWNrYWdlc19keW5hbWljID0gKGJiX2NhY2hlLmdldFZhcignUEFD
S0FHRVNfRFlOQU1JQycsIGZpbGVfbmFtZSwgVHJ1ZSkgb3IgIiIpLnNwbGl0
KCkKQEAgLTEwNCw3ICsxMDcsNyBAQAogICAgICAgICAjIEZvcndhcmQ6IHZp
cnR1YWwgLT4gW2ZpbGVuYW1lc10KICAgICAgICAgIyBSZXZlcnNlOiBQTiAt
PiBbdmlydHVhbHNdCiAgICAgICAgIGlmIHBuIG5vdCBpbiBzZWxmLnBuX3By
b3ZpZGVzOgotICAgICAgICAgICAgc2VsZi5wbl9wcm92aWRlc1twbl0gPSBT
ZXQoKQorICAgICAgICAgICAgc2VsZi5wbl9wcm92aWRlc1twbl0gPSBzZXQo
KQogICAgICAgICBzZWxmLnBuX3Byb3ZpZGVzW3BuXSB8PSBwcm92aWRlcwog
CiAgICAgICAgIGZvciBwcm92aWRlIGluIHByb3ZpZGVzOgpAQCAtMTEyLDcg
KzExNSw3IEBACiAgICAgICAgICAgICAgICAgc2VsZi5wcm92aWRlcnNbcHJv
dmlkZV0gPSBbXQogICAgICAgICAgICAgc2VsZi5wcm92aWRlcnNbcHJvdmlk
ZV0uYXBwZW5kKGZpbGVfbmFtZSkKIAotICAgICAgICBzZWxmLmRlcHNbZmls
ZV9uYW1lXSA9IFNldCgpCisgICAgICAgIHNlbGYuZGVwc1tmaWxlX25hbWVd
ID0gc2V0KCkKICAgICAgICAgZm9yIGRlcCBpbiBkZXBlbmRzOgogICAgICAg
ICAgICAgc2VsZi5hbGxfZGVwZW5kcy5hZGQoZGVwKQogICAgICAgICAgICAg
c2VsZi5kZXBzW2ZpbGVfbmFtZV0uYWRkKGRlcCkKQEAgLTkwNyw3ICs5MTAs
NyBAQAogICAgICAgICBzZWxmLnN0YXR1cyA9IEJCUGFyc2luZ1N0YXR1cygp
CiAKICAgICAgICAgaWdub3JlID0gYmIuZGF0YS5nZXRWYXIoIkFTU1VNRV9Q
Uk9WSURFRCIsIHNlbGYuY29uZmlndXJhdGlvbi5kYXRhLCAxKSBvciAiIgot
ICAgICAgICBzZWxmLnN0YXR1cy5pZ25vcmVkX2RlcGVuZGVuY2llcyA9IFNl
dCggaWdub3JlLnNwbGl0KCkgKQorICAgICAgICBzZWxmLnN0YXR1cy5pZ25v
cmVkX2RlcGVuZGVuY2llcyA9IHNldCggaWdub3JlLnNwbGl0KCkgKQogCiAg
ICAgICAgIHNlbGYuaGFuZGxlQ29sbGVjdGlvbnMoIGJiLmRhdGEuZ2V0VmFy
KCJCQkZJTEVfQ09MTEVDVElPTlMiLCBzZWxmLmNvbmZpZ3VyYXRpb24uZGF0
YSwgMSkgKQogCmRpZmYgLU5hdXIgMS42LjgvbGliL2JiL0NPVy5weSAxLjYu
OGEvbGliL2JiL0NPVy5weQotLS0gMS42LjgvbGliL2JiL0NPVy5weQkyMDA5
LTAxLTAyIDEzOjQwOjA2LjAwMDAwMDAwMCArMDEwMAorKysgMS42LjhhL2xp
Yi9iYi9DT1cucHkJMjAwOC0xMi0yOCAwMDoyMjo1NS4wMDAwMDAwMDAgKzAx
MDAKQEAgLTI2LDE3ICsyNiwzMiBAQAogZnJvbSBpbnNwZWN0IGltcG9ydCBn
ZXRtcm8KIAogaW1wb3J0IGNvcHkKLWltcG9ydCB0eXBlcywgc2V0cwotdHlw
ZXMuSW1tdXRhYmxlVHlwZXMgPSB0dXBsZShbIFwKLSAgICB0eXBlcy5Cb29s
ZWFuVHlwZSwgXAotICAgIHR5cGVzLkNvbXBsZXhUeXBlLCBcCi0gICAgdHlw
ZXMuRmxvYXRUeXBlLCBcCi0gICAgdHlwZXMuSW50VHlwZSwgXAotICAgIHR5
cGVzLkxvbmdUeXBlLCBcCi0gICAgdHlwZXMuTm9uZVR5cGUsIFwKLSAgICB0
eXBlcy5UdXBsZVR5cGUsIFwKLSAgICBzZXRzLkltbXV0YWJsZVNldF0gKyBc
Ci0gICAgbGlzdCh0eXBlcy5TdHJpbmdUeXBlcykpCitpbXBvcnQgdHlwZXMK
K3RyeToKKyAgICBzZXQKKyAgICB0eXBlcy5JbW11dGFibGVUeXBlcyA9IHR1
cGxlKFsgXAorICAgICAgICB0eXBlcy5Cb29sZWFuVHlwZSwgXAorICAgICAg
ICB0eXBlcy5Db21wbGV4VHlwZSwgXAorICAgICAgICB0eXBlcy5GbG9hdFR5
cGUsIFwKKyAgICAgICAgdHlwZXMuSW50VHlwZSwgXAorICAgICAgICB0eXBl
cy5Mb25nVHlwZSwgXAorICAgICAgICB0eXBlcy5Ob25lVHlwZSwgXAorICAg
ICAgICB0eXBlcy5UdXBsZVR5cGUsIFwKKyAgICAgICAgZnJvemVuc2V0XSAr
IFwKKyAgICAgICAgbGlzdCh0eXBlcy5TdHJpbmdUeXBlcykpCitleGNlcHQg
TmFtZUVycm9yOgorICAgIGltcG9ydCBzZXRzCisgICAgdHlwZXMuSW1tdXRh
YmxlVHlwZXMgPSB0dXBsZShbIFwKKyAgICAgICAgdHlwZXMuQm9vbGVhblR5
cGUsIFwKKyAgICAgICAgdHlwZXMuQ29tcGxleFR5cGUsIFwKKyAgICAgICAg
dHlwZXMuRmxvYXRUeXBlLCBcCisgICAgICAgIHR5cGVzLkludFR5cGUsIFwK
KyAgICAgICAgdHlwZXMuTG9uZ1R5cGUsIFwKKyAgICAgICAgdHlwZXMuTm9u
ZVR5cGUsIFwKKyAgICAgICAgdHlwZXMuVHVwbGVUeXBlLCBcCisgICAgICAg
IHNldHMuSW1tdXRhYmxlU2V0XSArIFwKKyAgICAgICAgbGlzdCh0eXBlcy5T
dHJpbmdUeXBlcykpCisKIAogTVVUQUJMRSA9ICJfX211dGFibGVfXyIKIApk
aWZmIC1OYXVyIDEuNi44L2xpYi9iYi9kYXRhX3NtYXJ0LnB5IDEuNi44YS9s
aWIvYmIvZGF0YV9zbWFydC5weQotLS0gMS42LjgvbGliL2JiL2RhdGFfc21h
cnQucHkJMjAwOS0wMS0wMiAxMzo0MDowNi4wMDAwMDAwMDAgKzAxMDAKKysr
IDEuNi44YS9saWIvYmIvZGF0YV9zbWFydC5weQkyMDA4LTEyLTI4IDAwOjE4
OjI5LjAwMDAwMDAwMCArMDEwMApAQCAtMzEsOSArMzEsMTEgQEAKIGltcG9y
dCBjb3B5LCBvcywgcmUsIHN5cywgdGltZSwgdHlwZXMKIGZyb20gYmIgICBp
bXBvcnQgbm90ZSwgZGVidWcsIGVycm9yLCBmYXRhbCwgdXRpbHMsIG1ldGhv
ZHBvb2wKIGZyb20gQ09XICBpbXBvcnQgQ09XRGljdEJhc2UKLWZyb20gc2V0
cyBpbXBvcnQgU2V0CiBmcm9tIG5ldyAgaW1wb3J0IGNsYXNzb2JqCi0KK3Ry
eToKKyAgICBzZXQKK2V4Y2VwdCBOYW1lRXJyb3I6CisgICAgZnJvbSBzZXRz
IGltcG9ydCBTZXQgYXMgc2V0CiAKIF9fc2V0dmFyX2tleXdvcmRfXyA9IFsi
X2FwcGVuZCIsIl9wcmVwZW5kIl0KIF9fc2V0dmFyX3JlZ2V4cF9fID0gcmUu
Y29tcGlsZSgnKD9QPGJhc2U+Lio/KSg/UDxrZXl3b3JkPl9hcHBlbmR8X3By
ZXBlbmQpKF8oP1A8YWRkPi4qKSk/JykKQEAgLTEzMCw3ICsxMzIsNyBAQAog
ICAgICAgICAgICAgdHJ5OgogICAgICAgICAgICAgICAgIHNlbGYuX3NwZWNp
YWxfdmFsdWVzW2tleXdvcmRdLmFkZCggYmFzZSApCiAgICAgICAgICAgICBl
eGNlcHQ6Ci0gICAgICAgICAgICAgICAgc2VsZi5fc3BlY2lhbF92YWx1ZXNb
a2V5d29yZF0gPSBTZXQoKQorICAgICAgICAgICAgICAgIHNlbGYuX3NwZWNp
YWxfdmFsdWVzW2tleXdvcmRdID0gc2V0KCkKICAgICAgICAgICAgICAgICBz
ZWxmLl9zcGVjaWFsX3ZhbHVlc1trZXl3b3JkXS5hZGQoIGJhc2UgKQogCiAg
ICAgICAgICAgICByZXR1cm4KQEAgLTE0NSw3ICsxNDcsNyBAQAogICAgICAg
ICBpZiAnXycgaW4gdmFyOgogICAgICAgICAgICAgb3ZlcnJpZGUgPSB2YXJb
dmFyLnJmaW5kKCdfJykrMTpdCiAgICAgICAgICAgICBpZiBub3Qgc2VsZi5f
c2Vlbl9vdmVycmlkZXMuaGFzX2tleShvdmVycmlkZSk6Ci0gICAgICAgICAg
ICAgICAgc2VsZi5fc2Vlbl9vdmVycmlkZXNbb3ZlcnJpZGVdID0gU2V0KCkK
KyAgICAgICAgICAgICAgICBzZWxmLl9zZWVuX292ZXJyaWRlc1tvdmVycmlk
ZV0gPSBzZXQoKQogICAgICAgICAgICAgc2VsZi5fc2Vlbl9vdmVycmlkZXNb
b3ZlcnJpZGVdLmFkZCggdmFyICkKIAogICAgICAgICAjIHNldHRpbmcgdmFy
CmRpZmYgLU5hdXIgMS42LjgvbGliL2JiL2ZldGNoL2dpdC5weSAxLjYuOGIv
bGliL2JiL2ZldGNoL2dpdC5weQotLS0gMS42LjgvbGliL2JiL2ZldGNoL2dp
dC5weQkyMDA5LTExLTIxIDEzOjMzOjU2LjAwMDAwMDAwMCArMDEwMAorKysg
MS42LjhiL2xpYi9iYi9mZXRjaC9naXQucHkJMjAwOS0xMS0yMSAxNDoyMDo0
MS4wMDAwMDAwMDAgKzAxMDAKQEAgLTEsMjQgKzEsMjQgQEAKKyMhL3Vzci9i
aW4vZW52IHB5dGhvbgogIyBleDp0cz00OnN3PTQ6c3RzPTQ6ZXQKICMgLSot
IHRhYi13aWR0aDogNDsgYy1iYXNpYy1vZmZzZXQ6IDQ7IGluZGVudC10YWJz
LW1vZGU6IG5pbCAtKi0KICIiIgogQml0QmFrZSAnRmV0Y2gnIGdpdCBpbXBs
ZW1lbnRhdGlvbgogCi0iIiIKK0NvcHlyaWdodCAoQykgMjAwNSBSaWNoYXJk
IFB1cmRpZQogCi0jQ29weXJpZ2h0IChDKSAyMDA1IFJpY2hhcmQgUHVyZGll
Ci0jCi0jIFRoaXMgcHJvZ3JhbSBpcyBmcmVlIHNvZnR3YXJlOyB5b3UgY2Fu
IHJlZGlzdHJpYnV0ZSBpdCBhbmQvb3IgbW9kaWZ5Ci0jIGl0IHVuZGVyIHRo
ZSB0ZXJtcyBvZiB0aGUgR05VIEdlbmVyYWwgUHVibGljIExpY2Vuc2UgdmVy
c2lvbiAyIGFzCi0jIHB1Ymxpc2hlZCBieSB0aGUgRnJlZSBTb2Z0d2FyZSBG
b3VuZGF0aW9uLgotIwotIyBUaGlzIHByb2dyYW0gaXMgZGlzdHJpYnV0ZWQg
aW4gdGhlIGhvcGUgdGhhdCBpdCB3aWxsIGJlIHVzZWZ1bCwKLSMgYnV0IFdJ
VEhPVVQgQU5ZIFdBUlJBTlRZOyB3aXRob3V0IGV2ZW4gdGhlIGltcGxpZWQg
d2FycmFudHkgb2YKLSMgTUVSQ0hBTlRBQklMSVRZIG9yIEZJVE5FU1MgRk9S
IEEgUEFSVElDVUxBUiBQVVJQT1NFLiAgU2VlIHRoZQotIyBHTlUgR2VuZXJh
bCBQdWJsaWMgTGljZW5zZSBmb3IgbW9yZSBkZXRhaWxzLgotIwotIyBZb3Ug
c2hvdWxkIGhhdmUgcmVjZWl2ZWQgYSBjb3B5IG9mIHRoZSBHTlUgR2VuZXJh
bCBQdWJsaWMgTGljZW5zZSBhbG9uZwotIyB3aXRoIHRoaXMgcHJvZ3JhbTsg
aWYgbm90LCB3cml0ZSB0byB0aGUgRnJlZSBTb2Z0d2FyZSBGb3VuZGF0aW9u
LCBJbmMuLAotIyA1MSBGcmFua2xpbiBTdHJlZXQsIEZpZnRoIEZsb29yLCBC
b3N0b24sIE1BIDAyMTEwLTEzMDEgVVNBLgorVGhpcyBwcm9ncmFtIGlzIGZy
ZWUgc29mdHdhcmU7IHlvdSBjYW4gcmVkaXN0cmlidXRlIGl0IGFuZC9vciBt
b2RpZnkgaXQgdW5kZXIKK3RoZSB0ZXJtcyBvZiB0aGUgR05VIEdlbmVyYWwg
UHVibGljIExpY2Vuc2UgYXMgcHVibGlzaGVkIGJ5IHRoZSBGcmVlIFNvZnR3
YXJlCitGb3VuZGF0aW9uOyBlaXRoZXIgdmVyc2lvbiAyIG9mIHRoZSBMaWNl
bnNlLCBvciAoYXQgeW91ciBvcHRpb24pIGFueSBsYXRlcgordmVyc2lvbi4K
KworVGhpcyBwcm9ncmFtIGlzIGRpc3RyaWJ1dGVkIGluIHRoZSBob3BlIHRo
YXQgaXQgd2lsbCBiZSB1c2VmdWwsIGJ1dCBXSVRIT1VUCitBTlkgV0FSUkFO
VFk7IHdpdGhvdXQgZXZlbiB0aGUgaW1wbGllZCB3YXJyYW50eSBvZiBNRVJD
SEFOVEFCSUxJVFkgb3IgRklUTkVTUworRk9SIEEgUEFSVElDVUxBUiBQVVJQ
T1NFLiBTZWUgdGhlIEdOVSBHZW5lcmFsIFB1YmxpYyBMaWNlbnNlIGZvciBt
b3JlIGRldGFpbHMuCisKK1lvdSBzaG91bGQgaGF2ZSByZWNlaXZlZCBhIGNv
cHkgb2YgdGhlIEdOVSBHZW5lcmFsIFB1YmxpYyBMaWNlbnNlIGFsb25nIHdp
dGgKK3RoaXMgcHJvZ3JhbTsgaWYgbm90LCB3cml0ZSB0byB0aGUgRnJlZSBT
b2Z0d2FyZSBGb3VuZGF0aW9uLCBJbmMuLCA1OSBUZW1wbGUKK1BsYWNlLCBT
dWl0ZSAzMzAsIEJvc3RvbiwgTUEgMDIxMTEtMTMwNyBVU0EuIAorIiIiCiAK
IGltcG9ydCBvcywgcmUKIGltcG9ydCBiYgpAQCAtMzUsMjYgKzM1LDcwIEBA
CiAgICAgICAgIGZvciBuYW1lIGluIGRpcnM6CiAgICAgICAgICAgICBvcy5y
bWRpcihvcy5wYXRoLmpvaW4ocm9vdCwgbmFtZSkpCiAKLWRlZiBydW5naXRj
bWQoY21kLGQpOgorZGVmIHJ1bmZldGNoY21kKGNtZCwgZCwgcXVpZXQgPSBG
YWxzZSk6CiAKICAgICBiYi5kZWJ1ZygxLCAiUnVubmluZyAlcyIgJSBjbWQp
CiAKICAgICAjIE5lZWQgdG8gZXhwb3J0IFBBVEggYXMgZ2l0IGlzIGxpa2Vs
eSB0byBiZSBpbiBtZXRhZGF0YSBwYXRocyAKICAgICAjIHJhdGhlciB0aGFu
IGhvc3QgcHJvdmlkZWQKLSAgICBwYXRoY21kID0gJ2V4cG9ydCBQQVRIPSVz
OyAlcycgJSAoZGF0YS5leHBhbmQoJyR7UEFUSH0nLCBkKSwgY21kKQorICAg
IGNtZCA9ICdleHBvcnQgUEFUSD0lczsgJXMnICUgKGRhdGEuZXhwYW5kKCck
e1BBVEh9JywgZCksIGNtZCkKIAotICAgIG15cmV0ID0gb3Muc3lzdGVtKHBh
dGhjbWQpCisgICAgIyByZWRpcmVjdCBzdGRlcnIgdG8gc3Rkb3V0CisgICAg
c3Rkb3V0X2hhbmRsZSA9IG9zLnBvcGVuKGNtZCArICIgMj4mMSIsICJyIikK
KyAgICBvdXRwdXQgPSAiIgorCisgICAgd2hpbGUgMToKKyAgICAgICAgbGlu
ZSA9IHN0ZG91dF9oYW5kbGUucmVhZGxpbmUoKQorICAgICAgICBpZiBub3Qg
bGluZToKKyAgICAgICAgICAgIGJyZWFrCisgICAgICAgIGlmIG5vdCBxdWll
dDoKKyAgICAgICAgICAgIHByaW50IGxpbmUKKyAgICAgICAgb3V0cHV0ICs9
IGxpbmUKKworICAgIHN0YXR1cyA9ICBzdGRvdXRfaGFuZGxlLmNsb3NlKCkg
b3IgMAorICAgIHNpZ25hbCA9IHN0YXR1cyA+PiA4CisgICAgZXhpdHN0YXR1
cyA9IHN0YXR1cyAmIDB4ZmYKKworICAgIGlmIHNpZ25hbDoKKyAgICAgICAg
cmFpc2UgRmV0Y2hFcnJvcigiRmV0Y2ggY29tbWFuZCAlcyBmYWlsZWQgd2l0
aCBzaWduYWwgJXMsIG91dHB1dDpcbiVzIiAlIChjbWQsIHNpZ25hbCwgb3V0
cHV0KSkKKyAgICBlbGlmIHN0YXR1cyAhPSAwOgorICAgICAgICByYWlzZSBG
ZXRjaEVycm9yKCJGZXRjaCBjb21tYW5kICVzIGZhaWxlZCB3aXRoIGV4aXQg
Y29kZSAlcywgb3V0cHV0OlxuJXMiICUgKGNtZCwgc3RhdHVzLCBvdXRwdXQp
KQorCisgICAgcmV0dXJuIG91dHB1dAorCitkZWYgY29udGFpbnNfcmVmKHRh
ZywgZCk6CisgICAgb3V0cHV0ID0gcnVuZmV0Y2hjbWQoImdpdCBsb2cgLS1w
cmV0dHk9b25lbGluZSAtbiAxICVzIC0tIDI+IC9kZXYvbnVsbCB8IHdjIC1s
IiAlIHRhZywgZCwgVHJ1ZSkKKyAgICByZXR1cm4gb3V0cHV0LnNwbGl0KClb
MF0gIT0gIjAiCisKK2RlZiBsYXRlc3RfcmV2aXNpb24ocHJvdG8sIHVzZXIs
IGhvc3QsIHBhdGgsIGJyYW5jaCwgZCk6CisgICAgIiIiCisgICAgQ29tcHV0
ZSB0aGUgSEVBRCByZXZpc2lvbiBmb3IgdGhlIHVybAorICAgICIiIgorICAg
IGlmIHVzZXI6CisgICAgICAgIHVzZXJuYW1lID0gdXNlciArICdAJworICAg
IGVsc2U6CisgICAgICAgIHVzZXJuYW1lID0gIiIKIAotICAgIGlmIG15cmV0
ICE9IDA6Ci0gICAgICAgIHJhaXNlIEZldGNoRXJyb3IoIkdpdDogJXMgZmFp
bGVkIiAlIHBhdGhjbWQpCisgICAgb3V0cHV0ID0gcnVuZmV0Y2hjbWQoImdp
dCBscy1yZW1vdGUgJXM6Ly8lcyVzJXMgJXMiICUgKHByb3RvLCB1c2VybmFt
ZSwgaG9zdCwgcGF0aCwgYnJhbmNoKSwgZCwgVHJ1ZSkKKyAgICByZXR1cm4g
b3V0cHV0LnNwbGl0KClbMF0KKworZGVmIGdldGJyYW5jaChwYXJtKToKKyAg
ICBpZiAnYnJhbmNoJyBpbiBwYXJtOgorICAgICAgICBicmFuY2ggPSBwYXJt
WydicmFuY2gnXQorICAgIGVsc2U6CisgICAgICAgIGJyYW5jaCA9ICJtYXN0
ZXIiCisgICAgaWYgbm90IGJyYW5jaDoKKyAgICAgICAgYnJhbmNoID0gIm1h
c3RlciIKIAotZGVmIGdldHRhZyhwYXJtKToKKyAgICByZXR1cm4gYnJhbmNo
CisKK2RlZiBnZXR0YWcocGFybSwgcHJvdG8sIHVzZXIsIGhvc3QsIHBhdGgs
IGJyYW5jaCwgZCk6CiAgICAgaWYgJ3RhZycgaW4gcGFybToKICAgICAgICAg
dGFnID0gcGFybVsndGFnJ10KICAgICBlbHNlOgotICAgICAgICB0YWcgPSAi
IgotICAgIGlmIG5vdCB0YWc6Ci0gICAgICAgIHRhZyA9ICJtYXN0ZXIiCisg
ICAgICAgIHRhZyA9IGxhdGVzdF9yZXZpc2lvbihwcm90bywgdXNlciwgaG9z
dCwgcGF0aCwgYnJhbmNoLCBkKQorICAgIGlmIG5vdCB0YWcgb3IgdGFnID09
ICJtYXN0ZXIiOgorICAgICAgICB0YWcgPSBsYXRlc3RfcmV2aXNpb24ocHJv
dG8sIHVzZXIsIGhvc3QsIHBhdGgsIGJyYW5jaCwgZCkKIAogICAgIHJldHVy
biB0YWcKIApAQCAtNzYsNyArMTIwLDkgQEAKICAgICBpZiAibG9jYWxwYXRo
IiBpbiBwYXJtOgogICAgICAgICByZXR1cm4gcGFybVsibG9jYWxwYXRoIl0K
IAotICAgIHRhZyA9IGdldHRhZyhwYXJtKQorICAgIHByb3RvID0gZ2V0cHJv
dG9jb2wocGFybSkKKyAgICBicmFuY2ggPSBnZXRicmFuY2gocGFybSkKKyAg
ICB0YWcgPSBnZXR0YWcocGFybSwgcHJvdG8sIHVzZXIsIGhvc3QsIHBhdGgs
IGJyYW5jaCwgZCkKIAogICAgIHJldHVybiBkYXRhLmV4cGFuZCgnZ2l0XyVz
JXNfJXMudGFyLmd6JyAlIChob3N0LCBwYXRoLnJlcGxhY2UoJy8nLCAnLicp
LCB0YWcpLCBkKQogCkBAIC0xMDQsOCArMTUwLDE0IEBACiAgICAgICAgIGZv
ciBsb2MgaW4gdXJsczoKICAgICAgICAgICAgICh0eXBlLCBob3N0LCBwYXRo
LCB1c2VyLCBwc3dkLCBwYXJtKSA9IGJiLmRlY29kZXVybChkYXRhLmV4cGFu
ZChsb2MsIGQpKQogCi0gICAgICAgICAgICB0YWcgPSBnZXR0YWcocGFybSkK
ICAgICAgICAgICAgIHByb3RvID0gZ2V0cHJvdG9jb2wocGFybSkKKyAgICAg
ICAgICAgIGJyYW5jaCA9IGdldGJyYW5jaChwYXJtKQorICAgICAgICAgICAg
dGFnID0gZ2V0dGFnKHBhcm0sIHByb3RvLCB1c2VyLCBob3N0LCBwYXRoLCBi
cmFuY2gsIGQpCisKKyAgICAgICAgICAgIGlmIHVzZXI6CisgICAgICAgICAg
ICAgICAgdXNlcm5hbWUgPSB1c2VyICsgJ0AnCisgICAgICAgICAgICBlbHNl
OgorICAgICAgICAgICAgICAgIHVzZXJuYW1lID0gIiIKIAogICAgICAgICAg
ICAgZ2l0c3JjbmFtZSA9ICclcyVzJyAlIChob3N0LCBwYXRoLnJlcGxhY2Uo
Jy8nLCAnLicpKQogCkBAIC0xMTYsNDMgKzE2OCwzOSBAQAogICAgICAgICAg
ICAgY29uYW1lID0gJyVzJyAlICh0YWcpCiAgICAgICAgICAgICBjb2RpciA9
IG9zLnBhdGguam9pbihyZXBvZGlyLCBjb25hbWUpCiAKLSAgICAgICAgICAg
IGNvZmlsZSA9IHNlbGYubG9jYWxwYXRoKGxvYywgZCkKLQotICAgICAgICAg
ICAgIyB0YWc9PSJtYXN0ZXIiIG11c3QgYWx3YXlzIHVwZGF0ZQotICAgICAg
ICAgICAgaWYgKHRhZyAhPSAibWFzdGVyIikgYW5kIEZldGNoLnRyeV9taXJy
b3IoZCwgbG9jYWxmaWxlKGxvYywgZCkpOgotICAgICAgICAgICAgICAgIGJi
LmRlYnVnKDEsICIlcyBhbHJlYWR5IGV4aXN0cyAob3Igd2FzIHN0YXNoZWQp
LiBTa2lwcGluZyBnaXQgY2hlY2tvdXQuIiAlIGNvZmlsZSkKLSAgICAgICAg
ICAgICAgICBjb250aW51ZQotCiAgICAgICAgICAgICBpZiBub3Qgb3MucGF0
aC5leGlzdHMocmVwb2Rpcik6CiAgICAgICAgICAgICAgICAgaWYgRmV0Y2gu
dHJ5X21pcnJvcihkLCByZXBvZmlsZW5hbWUpOiAgICAKICAgICAgICAgICAg
ICAgICAgICAgYmIubWtkaXJoaWVyKHJlcG9kaXIpCiAgICAgICAgICAgICAg
ICAgICAgIG9zLmNoZGlyKHJlcG9kaXIpCi0gICAgICAgICAgICAgICAgICAg
IHJ1bmdpdGNtZCgidGFyIC14emYgJXMiICUgKHJlcG9maWxlKSxkKQorICAg
ICAgICAgICAgICAgICAgICBydW5mZXRjaGNtZCgidGFyIC14emYgJXMiICUg
KHJlcG9maWxlKSxkKQogICAgICAgICAgICAgICAgIGVsc2U6Ci0gICAgICAg
ICAgICAgICAgICAgIHJ1bmdpdGNtZCgiZ2l0IGNsb25lIC1uICVzOi8vJXMl
cyAlcyIgJSAocHJvdG8sIGhvc3QsIHBhdGgsIHJlcG9kaXIpLGQpCisgICAg
ICAgICAgICAgICAgICAgIHJ1bmZldGNoY21kKCJnaXQgY2xvbmUgLW4gJXM6
Ly8lcyVzJXMgJXMiICUgKHByb3RvLCB1c2VybmFtZSwgaG9zdCwgcGF0aCwg
cmVwb2RpciksZCkKIAogICAgICAgICAgICAgb3MuY2hkaXIocmVwb2RpcikK
LSAgICAgICAgICAgIHJ1bmdpdGNtZCgiZ2l0IHB1bGwgJXM6Ly8lcyVzIiAl
IChwcm90bywgaG9zdCwgcGF0aCksZCkKLSAgICAgICAgICAgIHJ1bmdpdGNt
ZCgiZ2l0IHB1bGwgLS10YWdzICVzOi8vJXMlcyIgJSAocHJvdG8sIGhvc3Qs
IHBhdGgpLGQpCi0gICAgICAgICAgICBydW5naXRjbWQoImdpdCBwcnVuZS1w
YWNrZWQiLCBkKQotICAgICAgICAgICAgcnVuZ2l0Y21kKCJnaXQgcGFjay1y
ZWR1bmRhbnQgLS1hbGwgfCB4YXJncyAtciBybSIsIGQpCiAgICAgICAgICAg
ICAjIFJlbW92ZSBhbGwgYnV0IHRoZSAuZ2l0IGRpcmVjdG9yeQotICAgICAg
ICAgICAgcnVuZ2l0Y21kKCJybSAqIC1SZiIsIGQpCi0gICAgICAgICAgICAj
IG9sZCBtZXRob2Qgb2YgZG93bmxvYWRpbmcgdGFncwotICAgICAgICAgICAg
I3J1bmdpdGNtZCgicnN5bmMgLWEgLS12ZXJib3NlIC0tc3RhdHMgLS1wcm9n
cmVzcyByc3luYzovLyVzJXMvICVzIiAlIChob3N0LCBwYXRoLCBvcy5wYXRo
LmpvaW4ocmVwb2RpciwgIi5naXQiLCAiIikpLGQpCisgICAgICAgICAgICBp
ZiBub3QgY29udGFpbnNfcmVmKHRhZywgZCk6CisgICAgICAgICAgICAgICAg
cnVuZmV0Y2hjbWQoInJtICogLVJmIiwgZCkKKyAgICAgICAgICAgICAgICBy
dW5mZXRjaGNtZCgiZ2l0IGZldGNoICVzOi8vJXMlcyVzICVzIiAlIChwcm90
bywgdXNlcm5hbWUsIGhvc3QsIHBhdGgsIGJyYW5jaCksIGQpCisgICAgICAg
ICAgICAgICAgcnVuZmV0Y2hjbWQoImdpdCBmZXRjaCAtLXRhZ3MgJXM6Ly8l
cyVzJXMiICUgKHByb3RvLCB1c2VybmFtZSwgaG9zdCwgcGF0aCksIGQpCisg
ICAgICAgICAgICAgICAgcnVuZmV0Y2hjbWQoImdpdCBwcnVuZS1wYWNrZWQi
LCBkKQorICAgICAgICAgICAgICAgIHJ1bmZldGNoY21kKCJnaXQgcGFjay1y
ZWR1bmRhbnQgLS1hbGwgfCB4YXJncyAtciBybSIsIGQpCiAKICAgICAgICAg
ICAgIG9zLmNoZGlyKHJlcG9kaXIpCiAgICAgICAgICAgICBiYi5ub3RlKCJD
cmVhdGluZyB0YXJiYWxsIG9mIGdpdCByZXBvc2l0b3J5IikKLSAgICAgICAg
ICAgIHJ1bmdpdGNtZCgidGFyIC1jemYgJXMgJXMiICUgKHJlcG9maWxlLCBv
cy5wYXRoLmpvaW4oIi4iLCAiLmdpdCIsICIqIikgKSxkKQorICAgICAgICAg
ICAgcnVuZmV0Y2hjbWQoInRhciAtY3pmICVzICVzIiAlIChyZXBvZmlsZSwg
b3MucGF0aC5qb2luKCIuIiwgIi5naXQiLCAiKiIpICksZCkKIAogICAgICAg
ICAgICAgaWYgb3MucGF0aC5leGlzdHMoY29kaXIpOgogICAgICAgICAgICAg
ICAgIHBydW5lZGlyKGNvZGlyKQogCiAgICAgICAgICAgICBiYi5ta2Rpcmhp
ZXIoY29kaXIpCiAgICAgICAgICAgICBvcy5jaGRpcihyZXBvZGlyKQotICAg
ICAgICAgICAgcnVuZ2l0Y21kKCJnaXQgcmVhZC10cmVlICVzIiAlICh0YWcp
LGQpCi0gICAgICAgICAgICBydW5naXRjbWQoImdpdCBjaGVja291dC1pbmRl
eCAtcSAtZiAtLXByZWZpeD0lcyAtYSIgJSAob3MucGF0aC5qb2luKGNvZGly
LCAiZ2l0IiwgIiIpKSxkKQorICAgICAgICAgICAgY29wYXRoID0gb3MucGF0
aC5qb2luKGNvZGlyLCAiZ2l0IiwgIiIpCisgICAgICAgICAgICBydW5mZXRj
aGNtZCgiZ2l0IHJlYWQtdHJlZSAlcyIgJSAodGFnKSxkKQorICAgICAgICAg
ICAgcnVuZmV0Y2hjbWQoImdpdCBjaGVja291dC1pbmRleCAtcSAtZiAtLXBy
ZWZpeD0lcyAtYSIgJSAoY29wYXRoKSxkKQorICAgICAgICAgICAgYmIubWtk
aXJoaWVyKG9zLnBhdGguam9pbihjb3BhdGgsICIuZ2l0IiwgIiIpKQorICAg
ICAgICAgICAgcnVuZmV0Y2hjbWQoIkxBTkc9XCJlblwiIGdpdCBsb2cgJXMg
LS1tYXgtY291bnQ9MSAtLWRhdGU9aXNvIC0tID4gJXMuZ2l0L2xhc3RfY29t
bWl0X2luZm8iICUgKHRhZywgY29wYXRoKSxkKQorICAgICAgICAgICAgcnVu
ZmV0Y2hjbWQoImVjaG8gJXMgPiAlcy5naXQvYnJhbmNoIiAlIChicmFuY2gs
IGNvcGF0aCksZCkKIAogICAgICAgICAgICAgb3MuY2hkaXIoY29kaXIpCiAg
ICAgICAgICAgICBiYi5ub3RlKCJDcmVhdGluZyB0YXJiYWxsIG9mIGdpdCBj
aGVja291dCIpCi0gICAgICAgICAgICBydW5naXRjbWQoInRhciAtY3pmICVz
ICVzIiAlIChjb2ZpbGUsIG9zLnBhdGguam9pbigiLiIsICIqIikgKSxkKQor
ICAgICAgICAgICAgcnVuZmV0Y2hjbWQoInRhciAtY3pmICVzICVzIiAlIChz
ZWxmLmxvY2FscGF0aChsb2MsIGQpLCBvcy5wYXRoLmpvaW4oIi4iLCAiKiIp
ICksZCkK
====
end-file-1.6.8a
start-file-1.8.12a
begin-base64 644 1.8.12a.diff
ZGlmZiAtTmF1ciAxLjguMTIvYmluL2JpdGJha2UgMS44LjEyYS9iaW4vYml0
YmFrZQotLS0gMS44LjEyL2Jpbi9iaXRiYWtlCTIwMDktMDEtMDIgMTM6NDE6
MzMuMDAwMDAwMDAwICswMTAwCisrKyAxLjguMTJhL2Jpbi9iaXRiYWtlCTIw
MDgtMTItMjggMDA6MzY6MzUuMDAwMDAwMDAwICswMTAwCkBAIC0yNyw3ICsy
Nyw3IEBACiBpbXBvcnQgYmIKIGZyb20gYmIgaW1wb3J0IGNvb2tlcgogCi1f
X3ZlcnNpb25fXyA9ICIxLjguMTIiCitfX3ZlcnNpb25fXyA9ICIxLjguMTJh
IgogCiAjPT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09
PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PSMKICMgQkJP
cHRpb25zCmRpZmYgLU5hdXIgMS44LjEyL2xpYi9iYi9DT1cucHkgMS44LjEy
YS9saWIvYmIvQ09XLnB5Ci0tLSAxLjguMTIvbGliL2JiL0NPVy5weQkyMDA5
LTAxLTAyIDEzOjQxOjMzLjAwMDAwMDAwMCArMDEwMAorKysgMS44LjEyYS9s
aWIvYmIvQ09XLnB5CTIwMDgtMTItMjggMDA6MzY6MDguMDAwMDAwMDAwICsw
MTAwCkBAIC0yNiwxNyArMjYsMzEgQEAKIGZyb20gaW5zcGVjdCBpbXBvcnQg
Z2V0bXJvCiAKIGltcG9ydCBjb3B5Ci1pbXBvcnQgdHlwZXMsIHNldHMKLXR5
cGVzLkltbXV0YWJsZVR5cGVzID0gdHVwbGUoWyBcCi0gICAgdHlwZXMuQm9v
bGVhblR5cGUsIFwKLSAgICB0eXBlcy5Db21wbGV4VHlwZSwgXAotICAgIHR5
cGVzLkZsb2F0VHlwZSwgXAotICAgIHR5cGVzLkludFR5cGUsIFwKLSAgICB0
eXBlcy5Mb25nVHlwZSwgXAotICAgIHR5cGVzLk5vbmVUeXBlLCBcCi0gICAg
dHlwZXMuVHVwbGVUeXBlLCBcCi0gICAgc2V0cy5JbW11dGFibGVTZXRdICsg
XAotICAgIGxpc3QodHlwZXMuU3RyaW5nVHlwZXMpKQoraW1wb3J0IHR5cGVz
Cit0cnk6CisgICAgc2V0CisgICAgdHlwZXMuSW1tdXRhYmxlVHlwZXMgPSB0
dXBsZShbIFwKKyAgICAgICAgdHlwZXMuQm9vbGVhblR5cGUsIFwKKyAgICAg
ICAgdHlwZXMuQ29tcGxleFR5cGUsIFwKKyAgICAgICAgdHlwZXMuRmxvYXRU
eXBlLCBcCisgICAgICAgIHR5cGVzLkludFR5cGUsIFwKKyAgICAgICAgdHlw
ZXMuTG9uZ1R5cGUsIFwKKyAgICAgICAgdHlwZXMuTm9uZVR5cGUsIFwKKyAg
ICAgICAgdHlwZXMuVHVwbGVUeXBlLCBcCisgICAgICAgIGZyb3plbnNldF0g
KyBcCisgICAgICAgIGxpc3QodHlwZXMuU3RyaW5nVHlwZXMpKQorZXhjZXB0
IE5hbWVFcnJvcjoKKyAgICBpbXBvcnQgc2V0cworICAgIHR5cGVzLkltbXV0
YWJsZVR5cGVzID0gdHVwbGUoWyBcCisgICAgICAgIHR5cGVzLkJvb2xlYW5U
eXBlLCBcCisgICAgICAgIHR5cGVzLkNvbXBsZXhUeXBlLCBcCisgICAgICAg
IHR5cGVzLkZsb2F0VHlwZSwgXAorICAgICAgICB0eXBlcy5JbnRUeXBlLCBc
CisgICAgICAgIHR5cGVzLkxvbmdUeXBlLCBcCisgICAgICAgIHR5cGVzLk5v
bmVUeXBlLCBcCisgICAgICAgIHR5cGVzLlR1cGxlVHlwZSwgXAorICAgICAg
ICBzZXRzLkltbXV0YWJsZVNldF0gKyBcCisgICAgICAgIGxpc3QodHlwZXMu
U3RyaW5nVHlwZXMpKQogCiBNVVRBQkxFID0gIl9fbXV0YWJsZV9fIgogCmRp
ZmYgLU5hdXIgMS44LjEyL2xpYi9iYi9jYWNoZS5weSAxLjguMTJhL2xpYi9i
Yi9jYWNoZS5weQotLS0gMS44LjEyL2xpYi9iYi9jYWNoZS5weQkyMDA5LTAx
LTAyIDEzOjQxOjMzLjAwMDAwMDAwMCArMDEwMAorKysgMS44LjEyYS9saWIv
YmIvY2FjaGUucHkJMjAwOC0xMi0yOCAwMDozMDoxMS4wMDAwMDAwMDAgKzAx
MDAKQEAgLTMxLDcgKzMxLDEwIEBACiBpbXBvcnQgb3MsIHJlCiBpbXBvcnQg
YmIuZGF0YQogaW1wb3J0IGJiLnV0aWxzCi1mcm9tIHNldHMgaW1wb3J0IFNl
dAordHJ5OgorICAgIHNldAorZXhjZXB0IE5hbWVFcnJvcjoKKyAgICBmcm9t
IHNldHMgaW1wb3J0IFNldCBhcyBzZXQKIAogdHJ5OgogICAgIGltcG9ydCBj
UGlja2xlIGFzIHBpY2tsZQpAQCAtNDczLDYgKzQ3Niw2IEBACiAgICAgICAg
IChzZXQgZWxzZXdoZXJlKQogICAgICAgICAiIiIKICAgICAgICAgc2VsZi5p
Z25vcmVkX2RlcGVuZGVuY2llcyA9IFtdCi0gICAgICAgIHNlbGYud29ybGRf
dGFyZ2V0ID0gU2V0KCkKKyAgICAgICAgc2VsZi53b3JsZF90YXJnZXQgPSBz
ZXQoKQogICAgICAgICBzZWxmLmJiZmlsZV9wcmlvcml0eSA9IHt9CiAgICAg
ICAgIHNlbGYuYmJmaWxlX2NvbmZpZ19wcmlvcml0aWVzID0gW10KZGlmZiAt
TmF1ciAxLjguMTIvbGliL2JiL2Nvb2tlci5weSAxLjguMTJhL2xpYi9iYi9j
b29rZXIucHkKLS0tIDEuOC4xMi9saWIvYmIvY29va2VyLnB5CTIwMDktMDEt
MDIgMTM6NDE6MzMuMDAwMDAwMDAwICswMTAwCisrKyAxLjguMTJhL2xpYi9i
Yi9jb29rZXIucHkJMjAwOC0xMi0yOCAwMDoyODozOC4wMDAwMDAwMDAgKzAx
MDAKQEAgLTI1LDcgKzI1LDEwIEBACiBpbXBvcnQgc3lzLCBvcywgZ2V0b3B0
LCBnbG9iLCBjb3B5LCBvcy5wYXRoLCByZSwgdGltZQogaW1wb3J0IGJiCiBm
cm9tIGJiIGltcG9ydCB1dGlscywgZGF0YSwgcGFyc2UsIGV2ZW50LCBjYWNo
ZSwgcHJvdmlkZXJzLCB0YXNrZGF0YSwgcnVucXVldWUKLWZyb20gc2V0cyBp
bXBvcnQgU2V0Cit0cnk6CisgICAgc2V0CitleGNlcHQgTmFtZUVycm9yOgor
ICAgIGZyb20gc2V0cyBpbXBvcnQgU2V0IGFzIHNldAogaW1wb3J0IGl0ZXJ0
b29scywgc3JlX2NvbnN0YW50cwogCiBwYXJzZXNwaW4gPSBpdGVydG9vbHMu
Y3ljbGUoIHInfC8tXFwnICkKQEAgLTQ4NSw3ICs0ODgsNyBAQAogCiAgICAg
ICAgICMgVHdlYWsgc29tZSB2YXJpYWJsZXMKICAgICAgICAgaXRlbSA9IHNl
bGYuYmJfY2FjaGUuZ2V0VmFyKCdQTicsIGZuLCBUcnVlKQotICAgICAgICBz
ZWxmLnN0YXR1cy5pZ25vcmVkX2RlcGVuZGVuY2llcyA9IFNldCgpCisgICAg
ICAgIHNlbGYuc3RhdHVzLmlnbm9yZWRfZGVwZW5kZW5jaWVzID0gc2V0KCkK
ICAgICAgICAgc2VsZi5zdGF0dXMuYmJmaWxlX3ByaW9yaXR5W2ZuXSA9IDEK
IAogICAgICAgICAjIFJlbW92ZSBleHRlcm5hbCBkZXBlbmRlbmNpZXMKQEAg
LTU3Nyw3ICs1ODAsNyBAQAogICAgICAgICBzZWxmLnN0YXR1cyA9IGJiLmNh
Y2hlLkNhY2hlRGF0YSgpCiAKICAgICAgICAgaWdub3JlID0gYmIuZGF0YS5n
ZXRWYXIoIkFTU1VNRV9QUk9WSURFRCIsIHNlbGYuY29uZmlndXJhdGlvbi5k
YXRhLCAxKSBvciAiIgotICAgICAgICBzZWxmLnN0YXR1cy5pZ25vcmVkX2Rl
cGVuZGVuY2llcyA9IFNldCggaWdub3JlLnNwbGl0KCkgKQorICAgICAgICBz
ZWxmLnN0YXR1cy5pZ25vcmVkX2RlcGVuZGVuY2llcyA9IHNldCggaWdub3Jl
LnNwbGl0KCkgKQogCiAgICAgICAgIHNlbGYuaGFuZGxlQ29sbGVjdGlvbnMo
IGJiLmRhdGEuZ2V0VmFyKCJCQkZJTEVfQ09MTEVDVElPTlMiLCBzZWxmLmNv
bmZpZ3VyYXRpb24uZGF0YSwgMSkgKQogCmRpZmYgLU5hdXIgMS44LjEyL2xp
Yi9iYi9kYXRhX3NtYXJ0LnB5IDEuOC4xMmEvbGliL2JiL2RhdGFfc21hcnQu
cHkKLS0tIDEuOC4xMi9saWIvYmIvZGF0YV9zbWFydC5weQkyMDA5LTAxLTAy
IDEzOjQxOjMzLjAwMDAwMDAwMCArMDEwMAorKysgMS44LjEyYS9saWIvYmIv
ZGF0YV9zbWFydC5weQkyMDA4LTEyLTI4IDAwOjI2OjU3LjAwMDAwMDAwMCAr
MDEwMApAQCAtMzIsNyArMzIsMTAgQEAKIGltcG9ydCBiYgogZnJvbSBiYiAg
IGltcG9ydCB1dGlscywgbWV0aG9kcG9vbAogZnJvbSBDT1cgIGltcG9ydCBD
T1dEaWN0QmFzZQotZnJvbSBzZXRzIGltcG9ydCBTZXQKK3RyeToKKyAgICBz
ZXQKK2V4Y2VwdCBOYW1lRXJyb3I6CisgICAgZnJvbSBzZXRzIGltcG9ydCBT
ZXQgYXMgc2V0CiBmcm9tIG5ldyAgaW1wb3J0IGNsYXNzb2JqCiAKIApAQCAt
MTQyLDcgKzE0NSw3IEBACiAgICAgICAgICAgICB0cnk6CiAgICAgICAgICAg
ICAgICAgc2VsZi5fc3BlY2lhbF92YWx1ZXNba2V5d29yZF0uYWRkKCBiYXNl
ICkKICAgICAgICAgICAgIGV4Y2VwdDoKLSAgICAgICAgICAgICAgICBzZWxm
Ll9zcGVjaWFsX3ZhbHVlc1trZXl3b3JkXSA9IFNldCgpCisgICAgICAgICAg
ICAgICAgc2VsZi5fc3BlY2lhbF92YWx1ZXNba2V5d29yZF0gPSBzZXQoKQog
ICAgICAgICAgICAgICAgIHNlbGYuX3NwZWNpYWxfdmFsdWVzW2tleXdvcmRd
LmFkZCggYmFzZSApCiAKICAgICAgICAgICAgIHJldHVybgpAQCAtMTU0LDcg
KzE1Nyw3IEBACiAgICAgICAgIGlmICdfJyBpbiB2YXI6CiAgICAgICAgICAg
ICBvdmVycmlkZSA9IHZhclt2YXIucmZpbmQoJ18nKSsxOl0KICAgICAgICAg
ICAgIGlmIG5vdCBzZWxmLl9zZWVuX292ZXJyaWRlcy5oYXNfa2V5KG92ZXJy
aWRlKToKLSAgICAgICAgICAgICAgICBzZWxmLl9zZWVuX292ZXJyaWRlc1tv
dmVycmlkZV0gPSBTZXQoKQorICAgICAgICAgICAgICAgIHNlbGYuX3NlZW5f
b3ZlcnJpZGVzW292ZXJyaWRlXSA9IHNldCgpCiAgICAgICAgICAgICBzZWxm
Ll9zZWVuX292ZXJyaWRlc1tvdmVycmlkZV0uYWRkKCB2YXIgKQogCiAgICAg
ICAgICMgc2V0dGluZyB2YXIKZGlmZiAtTmF1ciAxLjguMTIvbGliL2JiL3J1
bnF1ZXVlLnB5IDEuOC4xMmEvbGliL2JiL3J1bnF1ZXVlLnB5Ci0tLSAxLjgu
MTIvbGliL2JiL3J1bnF1ZXVlLnB5CTIwMDktMDEtMDIgMTM6NDE6MzMuMDAw
MDAwMDAwICswMTAwCisrKyAxLjguMTJhL2xpYi9iYi9ydW5xdWV1ZS5weQky
MDA4LTEyLTI4IDAwOjMxOjU3LjAwMDAwMDAwMCArMDEwMApAQCAtMjMsNyAr
MjMsMTAgQEAKICMgNTEgRnJhbmtsaW4gU3RyZWV0LCBGaWZ0aCBGbG9vciwg
Qm9zdG9uLCBNQSAwMjExMC0xMzAxIFVTQS4KIAogZnJvbSBiYiBpbXBvcnQg
bXNnLCBkYXRhLCBldmVudCwgbWtkaXJoaWVyLCB1dGlscwotZnJvbSBzZXRz
IGltcG9ydCBTZXQgCit0cnk6CisgICAgc2V0CitleGNlcHQgTmFtZUVycm9y
OgorICAgIGZyb20gc2V0cyBpbXBvcnQgU2V0IGFzIHNldCAKIGltcG9ydCBi
Yiwgb3MsIHN5cwogaW1wb3J0IHNpZ25hbAogaW1wb3J0IHN0YXQKQEAgLTUy
NSw4ICs1MjgsOCBAQAogCiAgICAgICAgICAgICBzZWxmLnJ1bnFfZm5pZC5h
cHBlbmQodGFza0RhdGEudGFza3NfZm5pZFt0YXNrXSkKICAgICAgICAgICAg
IHNlbGYucnVucV90YXNrLmFwcGVuZCh0YXNrRGF0YS50YXNrc19uYW1lW3Rh
c2tdKQotICAgICAgICAgICAgc2VsZi5ydW5xX2RlcGVuZHMuYXBwZW5kKFNl
dChkZXBlbmRzKSkKLSAgICAgICAgICAgIHNlbGYucnVucV9yZXZkZXBzLmFw
cGVuZChTZXQoKSkKKyAgICAgICAgICAgIHNlbGYucnVucV9kZXBlbmRzLmFw
cGVuZChzZXQoZGVwZW5kcykpCisgICAgICAgICAgICBzZWxmLnJ1bnFfcmV2
ZGVwcy5hcHBlbmQoc2V0KCkpCiAKICAgICAgICAgICAgIHJ1bnFfYnVpbGQu
YXBwZW5kKDApCiAKQEAgLTYyMiw3ICs2MjUsNyBAQAogICAgICAgICAgICAg
ICAgIGlmIG1hcHNbb3JpZ2RlcF0gPT0gLTE6CiAgICAgICAgICAgICAgICAg
ICAgIGJiLm1zZy5mYXRhbChiYi5tc2cuZG9tYWluLlJ1blF1ZXVlLCAiSW52
YWxpZCBtYXBwaW5nIC0gU2hvdWxkIG5ldmVyIGhhcHBlbiEiKQogICAgICAg
ICAgICAgICAgIG5ld2RlcHMuYXBwZW5kKG1hcHNbb3JpZ2RlcF0pCi0gICAg
ICAgICAgICBzZWxmLnJ1bnFfZGVwZW5kc1tsaXN0aWRdID0gU2V0KG5ld2Rl
cHMpCisgICAgICAgICAgICBzZWxmLnJ1bnFfZGVwZW5kc1tsaXN0aWRdID0g
c2V0KG5ld2RlcHMpCiAKICAgICAgICAgYmIubXNnLm5vdGUoMiwgYmIubXNn
LmRvbWFpbi5SdW5RdWV1ZSwgIkFzc2lnbiBXZWlnaHRpbmdzIikKIAo=
====
end-file-1.8.12a

