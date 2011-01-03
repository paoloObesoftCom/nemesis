#!/bin/sh

curdir=`pwd`
model=`grep "MODEL=" ../make_init.sh | cut -d "=" -f 2`
envdir=`grep "OEROOT=" ../make_init.sh | cut -d "=" -f 2`
patchdir=$envdir/nemesis
data=`date +'%d-%m-%Y-%H-%M'`
ver='5.0'
subver=''

move_image()
{
	imagename="EDG-Nemesis-${ver}${subver}-${model}_${data}"
	mv ../root/cdkflash/complete.img ${HOME}/${imagename}.img
	cd ${HOME}
	zip ${imagename}.zip ${imagename}.img
	rm -f ${imagename}.img
	cd ${curdir}
	rm -f ../root/cdkflash/*.img
}

build_image()
{
	make dreamboximage_root
}

rebuild_image()
{
	rm -f ../root/cdkflash/.part_reiserfs
	rm -f ../root/cdkflash/.part_plugin_pacman
	rm -f ../root/cdkflash/.part_plugin_tetris
	rm -f ../root/cdkflash/.part_plugin_snake
	rm -f ../root/cdkflash/.part_plugin_dreamflash
	rm -f ../root/cdkflash/.part_plugin_movieplayer

	case $model in
		dm500)
			rm -f ../root/cdkflash/.part_sambaserver
			rm -f ../root/cdkflash/.part_fstools
			rm -f ../root/cdkflash/.part_cifs
			;;
		dm56x0)
			rm -f ../root/cdkflash/.part_sambaserver
			rm -f ../root/cdkflash/.part_fstools
			rm -f ../root/cdkflash/.part_cifs
			;;
	esac

	make rebuild-flash
	make flash-compress
}

rebuild_enigma()
{
	rm .enigma
	make .enigma
}

rebuild_kernel()
{
	rm .linuxdir .linuxkernel
	make -linuxkernel
}

patch_enigma()
{
	echo -n "Patching enigma for ${model}... "
	rm -rf ../apps/tuxbox/enigma
	cp -r $envdir/src/enigma/ ../apps/tuxbox
	cp -r $patchdir/patch/enigma1/enigma/ ../apps/tuxbox
	find ../apps/tuxbox/enigma -name ".svn" | xargs rm -rf
	cp -r $patchdir/patch/po/enigma1/it.po ../apps/tuxbox/enigma/po
	cp -r $patchdir/patch/po/enigma1/Makefile.am ../apps/tuxbox/enigma/po
	cat $patchdir/patch/po/enigma1/panel_it.po >> ../apps/tuxbox/enigma/po/it.po
	[ -e .enigma ] && rm .enigma
	echo "Done!"
}
	
patch_cdk()
{
	echo -n "Patching cdk for ${model}... "
	cp -r $patchdir/patch/enigma1/cdk/* .
	find . -name ".svn" | xargs rm -rf
	echo "Done!"
	./prepare $model
	rm .root
	make .root
}

case "$1" in
	patch_enigma)
		patch_enigma
		;;
	patch_cdk)
		patch_cdk
		;;
	move)
		move_image
		;;
	enigma)
		rebuild_enigma
		;;
	kernel)
		rebuild_kernel
		;;
	rebuild)
		rebuild_image
		;;
	image)
		build_image
		;;
	*)
		echo "Usage: $0 {patch_enigma|patch_cdk|move|enigma|rebuild|kernel|image}"
		;;
esac
