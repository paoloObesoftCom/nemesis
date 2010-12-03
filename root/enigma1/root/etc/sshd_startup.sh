#!/bin/sh
name='OpenSSh (sshd Deamon)'
binarypath='/etc/init.d/'
binaryname='sshd'

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
