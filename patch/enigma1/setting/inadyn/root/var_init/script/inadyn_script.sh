#!/bin/sh
DAEMON=/bin/inadyn
NAME=inadyn
DESC='InaDyn dynamic DNS Client'
name='E-mail'
binary='inadyn'

if [ -e /var/etc/inadynpar.conf ]; then
	test -f $DAEMON || exit 0
	set -e
	case "$1" in
	start)
			ARGS=" --input_file /var/etc/inadynpar.conf"
			echo -n "starting $DESC: $NAME... "
			$DAEMON $ARGS &
			echo "done."
		;;
	stop)
		echo -n "stopping $DESC: $NAME... "
		killall -9 $NAME
		echo "done."
		;;
	*)
		echo "Usage: $0 {start|stop}"
		exit 1
		;;
	esac
fi

exit 0
