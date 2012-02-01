#!/bin/bash

curdir=`pwd`
[ -e $curdir/machines ] || echo -n '500hd 800se' > $curdir/machines
machines=`cat machines`

for distro in $machines
do
	cd $distro/build
	echo -e '\nRemove package(s) linux from dm'$distro' distro...\n'
	. env.source
	bitbake -cclean linux-dm$distro
	cd $curdir 
done
