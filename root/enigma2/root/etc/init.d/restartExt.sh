#!/bin/sh

remove_tmp () {
	rm -rf /tmp/ecm.info
}

[ -e /usr/bin/csactive ] && export SRVACTIVE=$(cat /usr/bin/csactive)
[ -e /usr/bin/emuactive ] && export EMUACTIVE=$(cat /usr/bin/emuactive)

remove_tmp
[ -e /var/bin/emuactive ] && /usr/script/${EMUACTIVE}_em.sh stop
[ -e /var/bin/csactive ] && /usr/script/${SRVACTIVE}_cs.sh stop
sleep 1
[ -e /var/bin/csactive ] && /usr/script/${SRVACTIVE}_cs.sh start
sleep 7
[ -e /var/bin/emuactive ] && /usr/script/${EMUACTIVE}_em.sh start

exit 0








