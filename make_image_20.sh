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
envpath=`grep -m 1 "ENVPATH=" ../make_init.sh | cut -d "=" -f 2`
dmver=`grep "MACHINE=" ../make_init.sh | cut -d "=" -f 2`
dmdir=`grep "OE_BASE=" env.source | cut -d "=" -f 2`/
git_dir=`grep "OE_ROOT=" env.source | cut -d "=" -f 2`/1.6/openembedded/conf
patchdir=$envpath/nemesis

svn_ver=`grep -m 1 "SVN" ${envpath}/git/recipes/nemesis/nemesis-version.bb | cut -d "-" -f 2  | sed -e 's/[a-zA-Z" =]//g'`
pr_ver=`grep -m 1 "PR" ${envpath}/git/recipes/nemesis/nemesis-version.bb | cut -d "-" -f 2  | sed -e 's/[a-zA-Z" =]//g'`
ver=`grep -m 1 "PV" ${envpath}/git/recipes/nemesis/nemesis-version.bb | cut -d "=" -f 2 | sed -e 's/[a-zA-Z" =]//g' | cut -d "-" -f 1`
enigmaver='2'
oever='1.6'
subver=''
data=`date +'%d-%m-%Y-%H-%M'`
imagename="Nemesis${ver}${subver}-${dmver}-e${enigmaver}-OE(${oever})-SVN(${svn_ver}r${pr_ver})"

[ -e $git_dir/checksums.ini ] && rm -f $git_dir/checksums.ini

. ../opendreambox/env.source

rebuild_enigma()
{
	bitbake -cclean enigma2 task-opendreambox-enigma2
	bitbake enigma2 task-opendreambox-enigma2
}

rebuild_plugins()
{
	bitbake -cclean enigma2-plugins
	bitbake enigma2-plugins
}

rebuild_image()
{
	rm -f tmp/deploy/images/*
	bitbake dreambox-image
	exit $?
}

move_image()
{
	cp tmp/deploy/images/*.nfi ${HOME}/Images/${imagename}.nfi
	cd ${HOME}/Images
	zip ${imagename}.zip ${imagename}.nfi
	rm -f ${imagename}.nfi
	cd ${curdir}
}

patch_image()
{
	echo "Patching enigma for ${dmver}..."
	cd $patchdir
	./src_patch.sh oe_pack_16
	./src_patch.sh enigma2
	./src_patch.sh pluginse2
 	cd $curdir
}

create_e2_package()
{
	echo "Creating enigma2 package for ${dmver}..."
	bitbake -cclean enigma2-nemesis
	bitbake enigma2-nemesis
	if ( test $? -ne 0 ) then
		echo -e $ANSI_RED"Failed to build enima2-nemesis!"$ANSI_RESET2
		exit 1
	fi;
	cd $patchdir
	./create_e2_package.sh ${dmver}
	if ( test $? -ne 0 ) then
		echo -e $ANSI_RED"Failed to create e2 package!"$ANSI_RESET2
		exit 1
	fi;
 	cd $curdir
}

rebuild_feed()
{
	bitbake -cclean dreambox-feed-configs
	bitbake dreambox-feed-configs
}

rebuild_logo()
{
	bitbake -cclean dreambox-bootlogo
	bitbake dreambox-bootlogo
}

rebuild_nemesis()
{
	bitbake -cclean dreambox-nemesis
	bitbake dreambox-nemesis
}

rebuild_tuner_usb()
{
	bitbake -cclean dreambox-tuner-usb
	bitbake dreambox-tuner-usb
}

rebuild_linux()
{
	bitbake -cclean linux-$dmver task-boot
	bitbake linux-$dmver task-boot
}

case "$1" in
	patch)
		patch_image
		;;
	enigma2)
		rebuild_enigma
		;;
	pack)
		create_e2_package
		;;
	plugins)
		rebuild_plugins
		;;
	feed)
		rebuild_feed
		;;
	move)
		move_image
		;;
	kernel)
		rebuild_linux
		;;
	logo)
		rebuild_logo
		;;
	nemesis)
		rebuild_nemesis
		;;
	tuner)
		rebuild_tuner_usb
		;;
	clean)
		. ./env.source
		bitbake -cclean dreambox-image
			;;
	all)
		rebuild_image
		;;
	*)
		echo "Script to build image: ${imagename}" 
		echo $"Usage for $dmver: $0 {patch|enigma2|plugins|feed|all|logo|move|kernel|nemesis|tuner|clean|pack}"
		exit 1
		;;
esac


