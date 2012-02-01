#!/bin/sh

curdir=`pwd`
url="edg@osp.nemesis.tv"
[ -e $curdir/machines ] || echo -n '500hd 800se' > $curdir/machines
machines=`cat machines`

case "$1" in
	test)
 		for distro in $machines
		do
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ $url:/var/www/html/1.6/EDG-Test/dm$distro
		done
		;;
	prod)
		for distro in $machines
		do
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ $url:/var/www/html/1.6/EDG/dm$distro
		done
		;;
	image)
		rsync -avz -e ssh --delete ${HOME}/Images/ $url:/var/www/html/1.6/EDG-Test/Images
		;;
	save)
		for distro in $machines
		do
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ admin@nas:/share/Qweb/NEMESIS/dm$distro
		done
		rsync -avz -e ssh --delete src/enigma2 nathan:/Repository/src/oe_save/
		rsync -avz -e ssh --delete nemesis nathan:/Repository/src/oe_save/
		;;
	*)
		echo "Usage: $0 {test|prod|image}"
		;;
esac
