#!/bin/sh

cmdtoexecute=$1
host=$2
dev=$3
check_umount=$4
force_umount=$5
if [ -z $1 ]; then cmdtoexecute='cf_auto'; fi
if [ -z $2 ]; then host='sdb'; fi
if [ -z $3 ]; then dev='/media/cf'; fi
if [ -z $4 ]; then check_umount='1'; fi
if [ -z $5 ]; then force_umount='0'; fi

umount_usb_device()
{
	if [ $check_umount = '1' ]; then
		umount $dev
		if [ `mount | grep /dev/${host}| wc -l` -gt 0 ]; then
			if [ $force_umount = '1' ]; then
				echo "USB flash is busy, trying find and kill processes..."
				uproc=`/sbin/lsof -Fp ${dev}`
				pids=`echo $uproc | sed 'y/p/ /'`
				kill -9 $pids
				umount $dev
				if [ `mount | grep /dev/${host}| wc -l` -gt 0 ]; then
					echo "ERROR: umount failed!"
					exit 1
				fi
			else
				echo "ERROR: umount failed!"
				exit 1
			fi
		fi
	fi
}

umount_hdd_device()
{
	if [ $check_umount = '1' ]; then
		umount $dev
		if [ `mount | grep ${dev}| wc -l` -gt 0 ]; then
			if [ $force_umount = '1' ]; then
				echo "HDD is busy, trying find and kill processes..."
				uproc=`/sbin/lsof -Fp ${dev}`
				pids=`echo $uproc | sed 'y/p/ /'`
				kill -9 $pids
				umount $dev
				if [ `mount | grep ${dev}| wc -l` -gt 0 ]; then
					echo "ERROR: umount failed!"
					exit 1
				fi
			else
				echo "ERROR: umount failed!"
				exit 1
			fi
		fi
	fi
}

case $cmdtoexecute in
# USB Tools
	"usb_filecheck" )
		echo "DS: Filesystemcheck USB"
		swapoff -a > /dev/null 2>&1
		umount_usb_device
		if [ -b /dev/scsi/$host/bus0/target0/lun0/part1 ]; then
			echo "DS: Filesystemcheck USB..."
			/sbin/fsck.ext3 -f -v -n /dev/$host
			mount /dev/$host $dev
			swapon -a > /dev/null 2>&1
		else
			echo "Device /dev/ide/${host}/bus0/target0/lun0/disc, not Found!"
		fi
		;;
	"usb_disc")
		umount_usb_device
		if [ -b /dev/$host ]; then
			echo "Start creating FS on USB flash"
			echo "y" | mkfs.ext3 -i 16384 -b 1024 /dev/$host
			echo "Trying mount USB flash"
			mount /dev/$host $dev
		else
			echo "Device /dev/${host}, not Found!"
		fi
		;;
	"usb_part1")
		umount_usb_device
		if [ -b /dev/$host ]; then
			echo "Trying to create part"
			echo "0" | /sbin/sfdisk -f /dev/$host
			echo "Start creating FS on USB flash"
			echo "y" | /sbin/mkfs.ext3 -i 16384 -b 1024 /dev/$host1
			echo "Trying mount USB flash"
			mount /dev/$host1 $dev
		else
			echo "Device /dev/${host}, not Found!"
		fi
		;;
	"usb_auto")
		umount_usb_device
		if [ -b /dev/$host ]; then
			echo "Trying to create part"
			echo "0" | /sbin/sfdisk -f /dev/$host
			echo "Start creating FS on USB flash"
			echo "y" | /sbin/mkfs.ext3 -i 16384 -b 1024 /dev/${host}1
			echo "Trying mount USB flash"
			mount /dev/${host}1 $dev
			touch $dev/.is 2>/dev/null
			if [ -f $dev/.is ]; then
				rm -f $dev/.is	
				exit 0
			else
				echo "Creating part final with fail"
				echo "Trying format USB flash as disk"
				echo "y" | mkfs.ext3 -i 16384 -b 1024 /dev/$host
				echo "Trying mount USB flash"
				mount /dev/$host $dev
			fi
		else
			echo "Device /dev/${host}, not Found!"
		fi
		;;
# HDD Tools
	"hdd_filecheck" )
		echo "DS: Filesystemcheck HDD"
		host=host0
		dev=/media/hdd
		umount_hdd_device
		if [ -b /dev/$host ]; then
			echo "DS: doing Filesystemcheck HDD..."
			/sbin/fsck.ext3 -f -v -n /dev/${host}1
			mount /dev/${host}1 $dev
			swapon -a > /dev/null 2>&1
		else
			echo "Device /dev/${host}, not Found!"
		fi
		;;
# CF Tools
	"cf_filecheck" )
		echo "DS: Filesystemcheck CF"
		swapoff -a > /dev/null 2>&1
		umount_hdd_device
		if [ -b /dev/ide/$host/bus0/target0/lun0/disc ]; then
			echo "DS: Filesystemcheck CF..."
			/sbin/fsck.ext3 -f -v -p /dev/ide/$host/bus0/target0/lun0/part1
			mount /dev/ide/$host/bus0/target0/lun0/part1 $dev
			swapon -a > /dev/null 2>&1
		else
			echo "Device /dev/ide/${host}/bus0/target0/lun0/disc, not Found!"
		fi
		;;
	"cf_disc")
		umount_hdd_device
		if [ -b /dev/ide/$host/bus0/target0/lun0/disc ]; then
			echo "Start creating FS on CF flash"
			echo "y" | mkfs.ext3 -i 16384 -b 1024 /dev/ide/$host/bus0/target0/lun0/disc
			echo "Trying mount CF flash"
			mount /dev/ide/$host/bus0/target0/lun0/disc $dev
		else
			echo "Device /dev/ide/${host}/bus0/target0/lun0/disc, not Found!"
		fi
		;;
	"cf_part1")
		umount_hdd_device
		if [ -b /dev/ide/$host/bus0/target0/lun0/disc ]; then
			echo "Trying to create part"
			echo "0" | /sbin/sfdisk -f /dev/ide/$host/bus0/target0/lun0/disc
			echo "Start creating FS on CF flash"
			echo "y" | /sbin/mkfs.ext3 -i 16384 -b 1024 /dev/ide/$host/bus0/target0/lun0/part1
			echo "Trying mount CF flash"
			mount /dev/ide/$host/bus0/target0/lun0/disc $dev
		else
			echo "Device /dev/ide/${host}/bus0/target0/lun0/disc, not Found!"
		fi
		;;
	"cf_auto")
		umount_hdd_device
		if [ -b /dev/ide/$host/bus0/target0/lun0/disc ]; then
			echo "Trying to create part"
			echo "0" | /sbin/sfdisk -f /dev/ide/$host/bus0/target0/lun0/disc
			echo "Start creating FS on CF flash"
			echo "y" | /sbin/mkfs.ext3 -i 16384 -b 1024 /dev/ide/$host/bus0/target0/lun0/part1
			echo "Trying mount CF flash"
			mount /dev/ide/$host/bus0/target0/lun0/part1 $dev
			touch /media/cf/.is 2>/dev/null
			if [ -f /media/cf/.is ]; then
				rm -f /media/cf/.is
				exit 0
			else
				echo "Creating part final with fail"
				echo "Trying format CF flash as disk"
				echo "y" | mkfs.ext3 -i 16384 -b 1024 /dev/ide/$host/bus0/target0/lun0/disc
				echo "Trying mount CF flash"
				mount /dev/ide/$host/bus0/target0/lun0/disc $dev
				touch $dev/.is 2>/dev/null
			fi
		else
			echo "Device /dev/ide/${host}/bus0/target0/lun0/disc, not Found!"
		fi
		;;
esac
