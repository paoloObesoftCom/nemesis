#!/bin/sh
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

curdir=`pwd`
cd ..
envpath=`pwd`
cd $curdir
srcpath=$envpath/src
[ -e $envpath/src_patched ] || mkdir $envpath/src_patched 
[ -e $envpath/openembedded/sources ] || mkdir $envpath//openembedded/sources
srcpatchedpath=$envpath/src_patched
sourcespath=$envpath/openembedded/sources
oepath15=$envpath/openembedded/1.5
oepath16=$envpath/openembedded/1.6
cvspath=$envpath/cvs
oe_e2_16_path=$oepath16/openembedded/recipes/enigma2

PV_N=`grep -m 1 "PV" $oe_e2_16_path/enigma2-nemesis.bb | cut -d "=" -f 2 | sed -e 's/[" {}$A-Z]//g'`
PR_N=`grep -m 1 "PR" $oe_e2_16_path/enigma2-nemesis.bb | cut -d "=" -f 2 | sed -e 's/[" {}$A-Z]//g'`

PV=`grep -m 1 "PV" $oe_e2_16_path/enigma2.bb | cut -d "=" -f 2 | sed -e 's/[" {}$A-Z]//g'`
SRCDATE=`grep -m 1 "SRCDATE" $oe_e2_16_path/enigma2.bb | cut -d "=" -f 2 | sed -e 's/[a-zA-Z" =]//g'`

download_source ()
{
# download new enigma2 source
	echo -n "Download enigma2_${PV}${SRCDATE}_${MACHINE}.tar.bz2..."
	wget http://dreamboxupdate.com/download/snapshots/enigma2_${PV}${SRCDATE}_${MACHINE}.tar.bz2 -q && echo 'Done!'
	if ( test $? -ne 0 ) then
		echo -e $ANSI_RED"Failed to download enigma2 source"$ANSI_RESET2
		exit 1
	fi;
}

create ()
{
	nemesis_e2_pack=$oepath16/$MACHINE/build/tmp/work/nemesis-oe-linux/enigma2-nemesis-${PV_N}-${PR_N}/package
	cd $srcpatchedpath

	[ -e enigma2_${PV}${SRCDATE}_${MACHINE}.tar.bz2 ] || download_source
	[ -d enigma2_${MACHINE} ] && rm -rf enigma2_${MACHINE}
	[ -d enigma2_${PV}${SRCDATE}_${MACHINE} ] && rm -rf enigma2_${PV}${SRCDATE}_${MACHINE}

	echo -n "Extract enigma2_${PV}${SRCDATE}_${MACHINE}.tar.bz2..."
	tar -jxf enigma2_${PV}${SRCDATE}_${MACHINE}.tar.bz2
	cp -r enigma2_${PV}${SRCDATE}_${MACHINE} enigma2_$MACHINE
	rm -f LICENSE
	echo 'Done!'

	echo -n "Prepare package..."
	cd $nemesis_e2_pack
	[ -e usr/lib/enigma2/python/enigma.py ] && mv usr/lib/enigma2/python/enigma.py usr/lib/enigma2/python/enigma.py.nemesis
	[ -e usr/bin/enigma2 ] && mv usr/bin/enigma2 usr/bin/enigma2.nemesis

	cd $srcpatchedpath/enigma2_$MACHINE
	mv usr/lib/enigma2/python/enigma.py  $nemesis_e2_pack/usr/lib/enigma2/python/enigma.py.32
	mv usr/bin/enigma2 $nemesis_e2_pack/usr/bin/enigma2.32

	cd $srcpatchedpath
	mv enigma2_$MACHINE/usr/share/doc .
	mv enigma2_$MACHINE/usr/share/meta .
	rm -rf enigma2_$MACHINE/usr/bin
	rm -rf enigma2_$MACHINE/usr/share
	rm -rf enigma2_$MACHINE/usr/lib/enigma2/python/*.py*
	rm -rf enigma2_$MACHINE/usr/lib/enigma2/python/Components
	rm -rf enigma2_$MACHINE/usr/lib/enigma2/python/Screens
	rm -rf enigma2_$MACHINE/usr/lib/enigma2/python/Tools
	rm -rf enigma2_$MACHINE/usr/lib/enigma2/python/Plugins/Extensions/GraphMultiEPG
	rm -rf enigma2_$MACHINE/usr/lib/enigma2/python/Plugins/SystemPlugins/SoftwareManager
	rm -rf enigma2_$MACHINE/usr/lib/enigma2/python/Plugins/SystemPlugins/WirelessLan

	cd $srcpatchedpath
	cp -rf $nemesis_e2_pack/usr/bin enigma2_$MACHINE/usr/
	cp -rf $nemesis_e2_pack/usr/share enigma2_$MACHINE/usr/
	cp -f $nemesis_e2_pack/usr/lib/enigma2/python/*.py* enigma2_$MACHINE/usr/lib/enigma2/python/
	cp -rf $nemesis_e2_pack/usr/lib/enigma2/python/Components enigma2_$MACHINE/usr/lib/enigma2/python/
	cp -rf $nemesis_e2_pack/usr/lib/enigma2/python/Screens enigma2_$MACHINE/usr/lib/enigma2/python/
	cp -rf $nemesis_e2_pack/usr/lib/enigma2/python/Tools enigma2_$MACHINE/usr/lib/enigma2/python/
	cp -rf $nemesis_e2_pack/usr/lib/enigma2/python/Nemesis enigma2_$MACHINE/usr/lib/enigma2/python/
	cp -rf $nemesis_e2_pack/usr/lib/enigma2/python/Plugins/Extensions/GraphMultiEPG enigma2_$MACHINE/usr/lib/enigma2/python/Plugins/Extensions/
	cp -rf $nemesis_e2_pack/usr/lib/enigma2/python/Plugins/SystemPlugins/SoftwareManager enigma2_$MACHINE/usr/lib/enigma2/python/Plugins/SystemPlugins/
	cp -rf $nemesis_e2_pack/usr/lib/enigma2/python/Plugins/SystemPlugins/WirelessLan enigma2_$MACHINE/usr/lib/enigma2/python/Plugins/SystemPlugins/
	cp -rf $nemesis_e2_pack/usr/lib/enigma2/python/Plugins/SystemPlugins/ledManager enigma2_$MACHINE/usr/lib/enigma2/python/Plugins/SystemPlugins/
	cp -rf meta/*  enigma2_$MACHINE/usr/share/meta/
	mv doc  enigma2_$MACHINE/usr/share/
	rm -rf meta

	[ -e enigma2_$MACHINE.tar.bz2 ] && rm -f enigma2_$MACHINE.tar.bz2
	tar -jcf  enigma2_$MACHINE.tar.bz2 enigma2_$MACHINE/
	[ -e enigma2_$MACHINE.tar.bz2 ] && mv  enigma2_$MACHINE.tar.bz2 $sourcespath/
	echo 'Done!'

	find enigma2_$MACHINE/ -name "*.pyc" | xargs rm -f
	find enigma2_$MACHINE/ -name "*.pyo" | xargs rm -f
	find enigma2_${PV}${SRCDATE}_${MACHINE}/ -name "*.pyc" | xargs rm -f
	find enigma2_${PV}${SRCDATE}_${MACHINE}/ -name "*.pyo" | xargs rm -f
}

if [ $# -ne 0 ]; then
	for arg in $@
	do
		if [ $arg == "all" ]; then
			for distro in dm500hd dm800 dm800se dm8000 dm7020hd
			do
				MACHINE=$distro
				create
			done
			exit 0
		fi
	done
	for arg in $@
	do
		case $arg in
			dm500hd|dm800|dm800se|dm8000|dm7020hd)
				MACHINE=$arg
				create
			;;
			*)
				echo "One or more parameter are incorrect: {$@}"
				echo "Usage : $0 {[dm500hd e/o dm800 e/o dm800se e/o dm800  e/o dm7020hd] or all}"
				exit 1
			;;
		esac
	done
else
	echo "Usage : $0 {[dm500hd e/o dm800 e/o dm800se e/o dm800  e/o dm7020hd] or all}"
fi
