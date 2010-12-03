#!/bin/sh
name='Crond'
binarypath='/usr/sbin/'
binaryname='crond'

case "$1" in
start)
	if [ ! -e /var/etc/.dont_start_${binaryname} ]; then
		[ ! -e /var/spool/cron ] && mkdir /var/spool/cron
		[ ! -e /var/spool/cron/crontabs ] && mkdir /var/spool/cron/crontabs
		${binarypath}${binaryname}
	fi
	;;
stop)
	killall -9 crond
	;;
*)
	$0 stop
	;;
esac

exit 0
