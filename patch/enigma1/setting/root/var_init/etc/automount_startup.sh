#!/bin/sh
name='Automount'
binarypath='/sbin/'
binaryname='automount'
TIMEOUT=600
DEBUG=
VERBOSE=
#Debug parameters (note that DEBUG and VERBOSE are meaningful only with syslogd)
#TIMEOUT=30
#DEBUG=-d
#VERBOSE=-v
GHOSTING=-g
PIDFILE=/var/run/automount.pid
AUTOFSMOUNTDIR=/var/autofs
MAPFILE=/var/etc/auto.net
MD=/lib/modules/$(uname -r)/

case "$1" in
start)
	if [ -e ${MAPFILE} ]; then
		if [ ! -e /var/etc/.dont_start_${binaryname} ]; then
			grep -q autofs4 /proc/modules || insmod $MD/kernel/fs/autofs4/autofs4.ko
			mkdir /tmp/lock
			ln -sf /tmp/lock /var
			${binarypath}${binaryname} $DEBUG $GHOSTING $VERBOSE -t $TIMEOUT -p $PIDFILE $AUTOFSMOUNTDIR file $MAPFILE
		fi
	fi
	;;
stop)
	if [ -f ${PIDFILE} ] ; then
		kill -USR2 `cat ${PIDFILE}` 2> /dev/null
	fi
	;;
*)
	$0 stop
	;;
esac

exit 0
