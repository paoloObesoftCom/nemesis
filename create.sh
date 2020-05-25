#!/bin/bash

export PATH=$PATH:/e2/oscam/compiler/mips:/e2/oscam/compiler/mips/bin
export PATH=$PATH:/e2/oscam/compiler/ppc:/e2/oscam/compiler/ppc/bin

arg=$@
arg_n=$#

ppc_LIBUSBDIR=/e2/oscam/compiler/ppc/powerpc-linux/
ppc_LIBRTDIR=/e2/oscam/compiler/ppc/powerpc-linux/

mips_LIBUSBDIR=/e2/oscam/compiler/mips/mipsel-unknown-linux-gnu/sys-root/usr
mips_LIBRTDIR=/e2/oscam/compiler/mips/mipsel-unknown-linux-gnu/sys-root/usr

i386_OPENSSLINCLUDEDIR=/e2/oscam/openssl/include

REMOTE_SERVER="10.8.0.1 10.7.0.1 10.9.0.1 10.6.0.1"
#REMOTE_SERVER="10.8.0.1"

move_stable ()
{
	for i in $REMOTE_SERVER
	do
		echo -n "Copy to $i..."
		scp -q oscam-1.10/build-i386/oscam.i386 root@$i:config/oscam/oscam.i386.new
		echo "Done!"
	done
}

move_svn ()
{
	for i in $REMOTE_SERVER
	do
		echo -n "Copy to $i..."
		scp -q oscam-svn/build-i386/oscam.i386 root@$i:config/oscam/oscam.i386.svn
		echo "Done!"
	done
}

compile ()
{
# 	for i in mips ppc i386
	for i in mips
	do
		if [ -e build-$i ]; then
			cd build-$i
			rm -rf *
			echo "Create Oscam for $i in:" `pwd`
			if [ $i == "mips" ]; then 
 				cmake -DWEBIF=1 -DCMAKE_TOOLCHAIN_FILE=../../compiler/mips/toolchain-mipsel-tuxbox.cmake ..
			fi
			if [ $i == "ppc" ]; then 
				cmake -DWEBIF=1 -DCMAKE_TOOLCHAIN_FILE=../../compiler/ppc/toolchain-powerpc-tuxbox.cmake ..
			fi
			if [ $i == "i386" ]; then 
				cmake -DOPENSSL_INCLUDE_DIR=$i386_OPENSSLDIR -DWEBIF=1 ..
			fi
			make
			mv oscam oscam.$i
			zip oscam.$i.zip oscam.$i
			cd ..
		fi
	done
}

copy-to-vuplus()
{
	scp oscam-svn/build-mips/oscam.mips root@192.168.1.70:/usr/bin/oscam
	scp oscam-svn/build-mips/oscam.mips root@192.168.1.40:/usr/bin/oscam
}

download()
{
	if [ -d "oscam-svn" ]; then
		cd oscam-svn
		echo "Update Oscam in:" `pwd`
		svn update
		cd ..
	else
		echo "Get last Oscam..."
		svn co http://www.streamboard.tv/svn/oscam/trunk oscam-svn
	fi
	if [ -d "oscam-dev" ]; then
		cd oscam-dev
		echo "Update Oscam in:" `pwd`
		svn update
		cd ..
	else
		echo "Get Oscam devel..."
		svn co http://www.streamboard.tv/svn/oscam/trunk oscam-dev
	fi
	if [ -d "oscam-1.10" ]; then
		cd oscam-1.10
		echo "Update Oscam in:" `pwd`
		svn update
		cd ..
	else
		echo "Get Oscam stable..."
		svn co http://www.streamboard.tv/svn/oscam/tags/1.10 oscam-1.10
	fi
	cp toolchains/* oscam-dev/toolchains
	cp toolchains/* oscam-svn/toolchains
	cp toolchains/* oscam-1.10/toolchains
}

check_dir()
{
	[ -e build-ppc ] || mkdir build-ppc
	[ -e build-mips ] || mkdir build-mips
	[ -e build-i386 ] || mkdir build-i386 
}

copy_unicam()
{
	if [ $arg_n == 2 ]; then
		file_to=`echo $arg | cut -d' ' -f2`
		for i in 6 7 8 9
		do
			scp $file_to root@10.$i.0.1:config/unicam/unicam-x86
		done
	else
		echo "Usage: $0 unicam filename"
	fi
}

case "$1" in
	stable)
		cd oscam-1.10
		check_dir
		compile
		;;
	svn)
		cd oscam-svn
		check_dir
		compile	
		;;
	devel)
		cd oscam-dev
		check_dir
		compile
		;;
	get)
		download
		;;
	move)
		move_stable
		;;
	movesvn)
		move_svn
		;;
	unicam)
		copy_unicam
		;;
	copy)
		copy-to-vuplus
		;;
	*)
		echo "Usage: $0 {stable|devel|svn|get|move|movesvn|unicam|copy}"
		;;
esac

cd ..

