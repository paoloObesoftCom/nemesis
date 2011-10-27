#!/bin/bash

curdir=`pwd`

for distro in 500hd 800 800se 8000 7020hd
do
	cd $distro/build
	echo -e '\nRemove package(s) linux from dm'$distro' distro...\n'
	. env.source
	bitbake -cclean linux-dm$distro
	cd $curdir 
done
