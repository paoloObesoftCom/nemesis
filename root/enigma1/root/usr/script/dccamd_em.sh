#!/bin/sh
#emuname=DCCamd
#ecminfofile=ecm.info

if [ -e /bin/dccamd ] ; then
	binemu=/bin/dccamd
else
	binemu=/usr/bin/dccamd
fi

remove_tmp () {
	rm -rf /tmp/ecm.info /tmp/pid.info /tmp/cardinfo /tmp/mg*
}

case "$1" in
	start)
	remove_tmp
	$binemu &
	sleep 3
	;;
	stop)
	killall -9 dccamd
	remove_tmp
	sleep 2
	;;
	*)
	$0 stop
	exit 1
	;;
esac

exit 0
 
