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
			rm -f ${home}/Nemesis*
			for distro in 500hd 800 800se 8000
			do
				cd $distro/build
				echo -e '\nMake image for dm'$distro' distro...\n'
 				./make_e2_image.sh all
				if [ "$?" -eq "0" ]; then
					echo -e 'Move image for dm'$distro' on '${HOME}'...\n'
 					./make_e2_image.sh move
				else
					exit 1
				fi
				cd $curdir 
			done
		else
			echo "One or more arguments: {$@} are not Valid!"
		fi
	done
else
	echo "Usage : $0 {List of arguments (patch e/o build)}"
fi
