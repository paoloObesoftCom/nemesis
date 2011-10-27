#!/bin/sh

url="edg@osp.nemesis.tv"
#url="edg@osp.edg-nemesis.tv"

case "$1" in
	test)
		for distro in 500hd 800 800se 8000 7020hd
		do
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ $url:/var/www/html/1.6/EDG-Test/dm$distro
		done
		;;
	prod)
		for distro in 500hd 800 800se 8000 7020hd
		do
			rm -rf  $distro/build/tmp/deploy/ipk/nemesis
			rsync -avz -e ssh --delete $distro/build/tmp/deploy/ipk/ $url:/var/www/html/1.6/EDG/dm$distro
		done
		;;
	image)
		rsync -avz -e ssh --delete ${HOME}/Images/ $url:/var/www/html/1.6/EDG-Test/Images
		;;
	*)
		echo "Usage: $0 {test|prod|image}"
		;;
esac
