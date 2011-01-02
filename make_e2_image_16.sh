#!/bin/sh

curdir=`pwd`
envpath=`grep -m 1 "ENVPATH=" ../make_init.sh | cut -d "=" -f 2`
dmver=`grep "MACHINE=" ../make_init.sh | cut -d "=" -f 2`
dmdir=`grep "OE_BASE=" env.source | cut -d "=" -f 2`/
git_dir=`grep "OE_ROOT=" env.source | cut -d "=" -f 2`/1.6/openembedded/conf
patchdir=$envpath/nemesis

[ -e $git_dir/checksums.ini ] && rm -f $git_dir/checksums.ini
ver='2.1'
enigmaver='2'
oever='1.6'
subver='-Beta'
data=`date +'%d-%m-%Y-%H-%M'`
imagename="EDG-${ver}${subver}-${dmver}-e${enigmaver}-OE(${oever})_${data}"

rebuild_enigma()
{
	. ./env.source
	bitbake -cclean enigma2 task-opendreambox-enigma2
	bitbake enigma2 task-opendreambox-enigma2
}

rebuild_plugins()
{
	. ./env.source
	bitbake -cclean enigma2-plugins
	bitbake enigma2-plugins
}

rebuild_image()
{
	. ./env.source
	rm -f tmp/deploy/images/*
	bitbake dreambox-image
}

move_image()
{
	cp tmp/deploy/images/*.nfi ${HOME}/${imagename}.nfi
	cd ${HOME}
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

rebuild_feed()
{
	. ./env.source
	bitbake -cclean dreambox-feed-configs
	bitbake dreambox-feed-configs
}

rebuild_logo()
{
	. ./env.source
	bitbake -cclean dreambox-bootlogo
	bitbake dreambox-bootlogo
}

rebuild_nemesis()
{
	. ./env.source
	bitbake -cclean dreambox-nemesis
	bitbake dreambox-nemesis
}

rebuild_tuner_usb()
{
	. ./env.source
	bitbake -cclean dreambox-tuner-usb
	bitbake dreambox-tuner-usb
}

rebuild_linux()
{
	. ./env.source
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
	all)
		rebuild_image
		;;
	*)
		echo $"Usage for $dmver: $0 {patch|enigma2|plugins|feed|all|logo|move|kernel|nemesis|tuner}"
		;;
esac


