#!/bin/bash

curdir=`pwd`

if  [ $# -ne 0 ]; then
	args=$@
	for distro in 500hd 800 800se 8000 7020hd
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
