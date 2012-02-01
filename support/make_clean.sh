#!/bin/bash

curdir=`pwd`
[ -e $curdir/machines ] || echo -n '500hd 800se' > $curdir/machines
machines=`cat machines`

if  [ $# -ne 0 ]; then
	args=$@
	for distro in $machines
	do
		cd $distro/build
		echo -e '\nRemove package(s) "'$args'" from dm'$distro' distro...\n'
		. env.source
 		bitbake -cclean $args
		if [ "$?" -ne "0" ]; then
			cd $curdir 
			exit 1
		fi
		cd $curdir 
	done
else
	echo "Usage : $0 {List of package(s) to remove}"
fi
