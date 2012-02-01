#!/bin/bash

curdir=`pwd`
patchdir=$curdir/nemesis
[ -e $curdir/machines ] || echo -n '500hd 800se' > $curdir/machines
machines=`cat machines`

if [ $# -ne 0 ]; then
	for arg in $@
	do
		if [ $arg = "patch" ]; then
			echo "Patching enigma for all distro..."
			cd $patchdir
			./src_patch.sh oe_pack_16
			./src_patch.sh enigma2
			./src_patch.sh pluginse2
			cd $curdir
		elif [ $arg = "create" ]; then
			for distro in $machines
			do
				cd $distro/build
 				./make_e2_image.sh pack
				if [ "$?" -ne "0" ]; then
					exit 1
				fi
				cd $curdir 
			done
		elif [ $arg = "build" ]; then
			for distro in $machines
			do
				cd $distro/build
				echo -e '\nMake image for dm'$distro' distro...\n'
 				./make_e2_image.sh all
				if [ "$?" -ne "0" ]; then
					cd $curdir 
					exit 1
				fi
				cd $curdir 
			done
		elif [ $arg = "move" ]; then
			for distro in $machines
			do
				cd $distro/build
				echo -e 'Move image for dm'$distro' on '${HOME}'/Images...'
				./make_e2_image.sh move
				cd $curdir 
			done
		else
			echo "One or more arguments: {$@} are not Valid!"
		fi
	done
else
	echo "Usage : $0 {List of arguments (patch e/o create e/o build e/o move)}"
fi
