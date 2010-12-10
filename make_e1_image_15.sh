#!/bin/sh

curdir=`pwd`
envpath=`grep -m 1 "ENVPATH=" ../make_init.sh | cut -d "=" -f 2`
dmver=`grep "MACHINE=" ../make_init.sh | cut -d "=" -f 2`
dmdir=`grep "OE_BASE=" env.source | cut -d "=" -f 2`/
patchdir=$envpath/nemesis

ver='1.0'
enigmaver='1'
subver=''
data=`date +'%d-%m-%Y-%H-%M'`
imagename="NewDE-${ver}${subver}-${dmver}-e${enigmaver}_${data}"

rebuild_nemesis()
{
	. ./env.source
	bitbake -cclean dreambox-nemesis
	bitbake dreambox-nemesis
}

rebuild_enigma()
{
	. ./env.source
	rm -f tmp/deploy/images/*
	bitbake -cclean enigma
	bitbake enigma
}

rebuild_image()
{
	. ./env.source
	rm -f tmp/deploy/images/*
	bitbake -cclean tuxbox-image-info dreambox-image
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
	./src_patch.sh oe_pack_15
	./src_patch.sh enigma1
 	cd $curdir
}

rebuild_all()
{
	. ./env.source
	bitbake -cclean initscripts-opendreambox enigma base-files \
					tuxbox-common tuxbox-image-info dreambox-nemesis dreambox-image \
					tuxbox-plugins libtuxtxt tuxbox-plugins-enigma base-passwd
	rm tmp/deploy/images/*
	bitbake dreambox-image
}

rebuild_plugins()
{
	. ./env.source
	bitbake -cclean tuxbox-plugins libtuxtxt
	bitbake libtuxtxt tuxbox-plugins
}

rebuild_plugins_enigma()
{
	. ./env.source
	bitbake -cclean tuxbox-plugins-enigma
	bitbake -v tuxbox-plugins-enigma
}

rebuild_linux()
{
	. ./env.source
	bitbake -cclean linux-$dmver
	bitbake -v linux-$dmver
}

case "$1" in
    patch)
		patch_image
		;;
	enigma)
		rebuild_enigma
		;;
	rebuild)
		rebuild_image
		;;
	move)
		move_image
		;;
	plugins)
		rebuild_plugins
		;;
	plugins-enigma)
		rebuild_plugins_enigma
		;;
	linux)
		rebuild_linux
		;;
	nemesis)
		rebuild_nemesis
		;;
	all)
		rebuild_all
		;;
    *)
		echo $"Usage: $0 {patch|enigma|rebuild|all|move|plugins|plugins-enigma|linux|nemesis}"
		;;
esac

