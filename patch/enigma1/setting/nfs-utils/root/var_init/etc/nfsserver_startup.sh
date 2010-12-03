#!/bin/sh
name='NFS Server'
binarypath='/etc/init.d/'
binaryname='nfsserver'

case "$1" in
start)
	if [ ! -e /var/etc/.dont_start_${binaryname} ]; then
		${binarypath}${binaryname} start
	fi
	;;
stop)
	${binarypath}${binaryname} stop
	;;
*)
	$0 stop
	;;
esac

exit 0
