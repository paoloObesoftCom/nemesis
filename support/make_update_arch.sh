#!/bin/sh

curdir=`pwd`
url="ospneme@nemesis.tv"
url_casa="root@192.168.1.102"
[ -e $curdir/machines ] || echo -n '500hd 800se' > $curdir/machines
machines=`cat machines`

case "$1" in
	test)
		for distro in $machines
		do
			echo "Sinchronize Test Environement for dm$distro...."
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ $url:/var/www/vhosts/nemesisupd/httpdocs/1.6/EDG-Test/dm$distro
		done
		;;
	home)
		for distro in $machines
		do
			echo "Sinchronize Home Environement for dm$distro...."
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ $url_casa:/var/www/nemesis/1.6/EDG/dm$distro
		done
		;;
	prod)
		for distro in $machines
		do
			echo "Sinchronize Production Environement for dm$distro...."
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ $url:/var/www/vhosts/nemesisupd/httpdocs/1.6/EDG/dm$distro
		done
		;;
	image)
		rsync -avz -e ssh --delete ${HOME}/Images/ $url:/var/www/vhosts/httpdocs/NemesisImage
		;;
	save)
		for distro in $machines
		do
			echo "Sinchronize Domestic Environement for dm$distro...."
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ admin@192.168.1.100:/share/Qweb/NEMESIS/dm$distro
		done
# 		echo "Sinchronize Enigma2 patched sources...."
# 		rsync -avz -e ssh --delete src/enigma2 192.168.1.102:/Repository/src/oe_save/
# 		echo "Sinchronize Nemesis sources...."
# 		rsync -avz -e ssh --delete nemesis 192.168.1.102:/Repository/src/oe_save/
		;;
	*)
		echo "Usage: $0 {test|prod|image}"
		;;
esac
