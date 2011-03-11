#!/bin/bash

curdir=`pwd`

if  [ $# -ne 0 ]; then
	args=$@
	for distro in 500hd 800 800se 8000
	do
		cd $distro/build
		echo -e '\nRemove package(s) "'$args'" from dm'$distro' distro...\n'
		. env.source
 		bitbake -cclean $args
		cd $curdir 
	done
else
	echo "Usage : $0 {List of package(s) to remove}"
fi