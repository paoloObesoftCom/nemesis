#!/bin/sh
name='Samba'
binarypath='/bin/'
binaryname='smbd'

case "$1" in
start)
	if [ ! -e /var/etc/.dont_start_${binaryname} ]; then
		${binarypath}${binaryname} -D
		${binarypath}nmbd -D
	fi
	;;
stop)
	killall -9 nmbd smbd
	;;
*)
	$0 stop
	;;
esac

exit 0
