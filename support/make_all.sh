#!/bin/bash

curdir=`pwd`
patchdir=$curdir/nemesis

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
		elif [ $arg = "build" ]; then
			for distro in 500hd 800 800se 8000
			do
				cd $distro/build
				echo -e '\nMake image for dm'$distro' distro...\n'
 				./make_e2_image.sh all
				if [ "$?" -ne "0" ]; then
					exit 1
				fi
				cd $curdir 
			done
		elif [ $arg = "move" ]; then
			rm -f ${HOME}/Nemesis*
			for distro in 500hd 800 800se 8000
			do
				cd $distro/build
				echo -e 'Move image for dm'$distro' on '${HOME}'...'
				./make_e2_image.sh move
				cd $curdir 
			done
		else
			echo "One or more arguments: {$@} are not Valid!"
		fi
	done
else
	echo "Usage : $0 {List of arguments (patch e/o build e/o move)}"
fi
