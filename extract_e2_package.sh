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
[ -e $envpath/e2_unpaked ] || mkdir $envpath/e2_unpaked
[ -e $envpath/openembedded/sources ] || mkdir $envpath//openembedded/sources
srcpatchedpath=$envpath/e2_unpaked
oepath16=$envpath/openembedded/1.6
oe_e2_16_path=$oepath16/openembedded/recipes/enigma2

[ -e $srcpatchedpath/enigma2 ] || ln -s $envpath/src/enigma2 $srcpatchedpath/
[ -e $srcpatchedpath/enigma2.orig ] || ln -s $envpath/src/enigma2.orig $srcpatchedpath/

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

remove_unused ()
{
	[ -d enigma2_${MACHINE} ] && rm -rf enigma2_${MACHINE}
	[ -d enigma2_${PV}${SRCDATE}_${MACHINE} ] && rm -rf enigma2_${PV}${SRCDATE}_${MACHINE}
}

clean ()
{
	echo "remove $srcpatchedpath/$MACHINE..."
	rm -rf $srcpatchedpath/$MACHINE
}

extract()
{
	cd $srcpatchedpath

	[ -e enigma2_${PV}${SRCDATE}_${MACHINE}.tar.bz2 ] || download_source
	remove_unused

	echo -n "Extract enigma2_${PV}${SRCDATE}_${MACHINE}.tar.bz2..."
	tar -jxf enigma2_${PV}${SRCDATE}_${MACHINE}.tar.bz2
	cp -r enigma2_${PV}${SRCDATE}_${MACHINE} enigma2_$MACHINE
	rm -f LICENSE
	find enigma2_$MACHINE/ -name "*.pyc" | xargs rm -f
	find enigma2_$MACHINE/ -name "*.pyo" | xargs rm -f
	echo 'Done'

	cd $srcpatchedpath

	[ -d $MACHINE ] && rm -rf $MACHINE
	mkdir $MACHINE

	mv enigma2_$MACHINE/usr/share $MACHINE/data
	mv $MACHINE/data/enigma2/* $MACHINE/data
	rm -rf $MACHINE/data/meta
	rm -rf $MACHINE/data/doc
	rmdir $MACHINE/data/enigma2/
	mkdir -p $MACHINE/lib/
	mv enigma2_$MACHINE/usr/lib/enigma2/python/*.py $MACHINE/
	mv enigma2_$MACHINE/usr/lib/enigma2/python $MACHINE/lib
	rm -f $MACHINE/enigma.py

	remove_unused
}

if [ $# -ne 0 ]; then
	for arg in $@
	do
		if [ $arg == "compare" ]; then
			MACHINE=dm800se
			kdiff3 $srcpatchedpath/$MACHINE $srcpatchedpath/enigma2
			kdiff3 $srcpatchedpath/$MACHINE $srcpatchedpath/enigma2.orig
			exit 0
		fi
		if [ $arg == "all" -o $arg == "clean" ]; then
			for distro in dm500hd dm800 dm800se dm8000 dm7020hd
			do
				MACHINE=$distro
				[ $arg == "all" ] && extract
				[ $arg == "clean" ] && clean
			done
			exit 0
		fi
	done
	for arg in $@
	do
		case $arg in
			dm500hd|dm800|dm800se|dm8000|dm7020hd)
				MACHINE=$arg
				extract
			;;
			*)
				echo "One or more parameter are incorrect: {$@}"
				echo "Usage : $0 {[dm500hd e/o dm800 e/o dm800se e/o dm8000 e/o dm7020hd] or all|clean|compare}"
				exit 1
			;;
		esac
	done
else
	echo "Usage : $0 {[dm500hd e/o dm800 e/o dm800se e/o dm8000 e/o dm7020hd] or all|clean|compare}"
fi
