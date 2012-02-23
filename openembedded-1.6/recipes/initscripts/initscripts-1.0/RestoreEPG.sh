#!/bin/sh

#Check if epg.date exist
for i in usb hdd cf
do
	if [ -e /media/$i/epg.dat ]; then
		echo "Found epg.dat on /media/$i/"
		exit 0
	fi
done

#Restore EPG
for i in usb hdd cf
do
	echo "Check epg.dat.save on /media/$i/"
	if [ -e /media/$i/epg.dat.save ]; then
		cp /media/$i/epg.dat.save /media/$i/epg.dat
		echo "Found epg.dat.save on /media/$i/"
		exit 0
	fi
done

exit 0
