#!/bin/sh
#Start DVBT/C Adapter
killall -9 vtuner
sleep 2
if [ -e "/etc/dtt.devices" ]; then
	isenabled=0
	for dttDev in `cat /etc/dtt.devices`; do
		numadapter=`ls -1 /dev/dvb/ | wc -l`
		sh /usr/script/loaddttmodules.sh $dttDev
		cnt=0
		while [[ $numadapter -ge `ls -1 /dev/dvb/ | wc -l` && $cnt -lt 5 ]]; do
			let cnt=$cnt+1
			sleep 1
		done
		isenabled=1
	done
	if [ $isenabled -ge 1 ]; then
		cnt=1
		while [[ -d /dev/dvb/adapter${cnt} ]]
		do
			/usr/bin/vtuner ${cnt} >/dev/null 2>&1 &
			let cnt=$cnt+1
		done
	fi
fi
