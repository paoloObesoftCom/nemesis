#!/bin/sh 
export PATH=/sbin:/usr/sbin:/bin:/usr/bin

ALLOW_LAN_ACCESS="1"
USERS="/var/etc/firewall.users"
IPTABLES=`which iptables`
MODULES="/lib/modules/$(uname -r)/kernel/net/ipv4/netfilter"

if [ "$IPTABLES#" = "#" ]; then echo "Iptables binary not found !"; exit; fi
if [ ! -r /var/iptables/libipt_standard.so ] ; then echo "Iptables object files not found !"; exit; fi
if [ ! -r $MODULES/ip_tables.ko ]; then echo "Kernel firewall modules not found !"; exit; fi

if [ -e /var/etc/.dont_ena_lan ] ; then 
	ALLOW_LAN_ACCESS="0"
fi

case "$1" in
    'start')
	check=`lsmod | grep -c "^ip_tables"`
	if [ $check = 0 ]
	then
		insmod $MODULES/ip_tables.ko
	fi

	check=`lsmod | grep -c "^ip_conntrack"`
	if [ $check = 0 ]
	then
		insmod $MODULES/ip_conntrack.ko
	fi
	
	check=`lsmod | grep -c "^iptable_nat"`
	if [ -r $MODULES/iptable_nat.ko ] && [ $check = 0 ]
	then
		insmod $MODULES/iptable_nat.ko
	fi

        for MOD in `ls $MODULES | sed -e "s/\([^. ]*\).*/\1/"`
        do
             check=`lsmod | grep -c "^$MOD"`
             if [ $check = 0 ] && [ $MOD != 'ip_tables' ]
             then
                   insmod $MODULES/$MOD.ko
             fi
        done

        LOOPBACK=`/sbin/ifconfig lo | grep 'inet addr:' | sed -e 's/.*inet addr:\([^ ]*\).*/\1/'`
        INTERNAL=`/sbin/ifconfig eth0 | grep 'inet addr:' | sed -e 's/.*inet addr:\([^ ]*\).*/\1/' | cut -d"." -f1-3`.0/24

        $IPTABLES -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT 
        $IPTABLES -A OUTPUT -m state --state NEW,ESTABLISHED -j ACCEPT 
        $IPTABLES -A INPUT -s $LOOPBACK -j ACCEPT
        $IPTABLES -P INPUT DROP
        $IPTABLES -P FORWARD DROP
	if [ $ALLOW_LAN_ACCESS = "1" ]; then
	        $IPTABLES -A INPUT -i eth0 -s $INTERNAL -j ACCEPT 
	fi

        if [ ! -r $USERS ]; then
            echo "Can't read: $USERS."
            echo "Loading only default rules."
        else   
       		# first call dos2unix to be sure the file is in unix format
       		dos2unix $USERS
		cat $USERS | while read IP comments ; do
			$IPTABLES -A INPUT -s $IP -j ACCEPT
		done 
        fi
        echo "Firewall is active!."
        exit 1
    ;;

    'stop')
        $IPTABLES -P INPUT ACCEPT 
        $IPTABLES -P OUTPUT ACCEPT 
        $IPTABLES -P FORWARD ACCEPT 
        $IPTABLES -F 
        $IPTABLES -X

        echo "Firewall disabled."
    exit 2
    ;;

    'restart')
    	$0 stop || $0 start || echo "Firewall has restarted." || echo "" || exit 3
    ;;
    
    'status')
	$IPTABLES -L
	exit 4
    ;;

	*)
	echo
	echo "Options: $0 {start|stop|restart|status}"
	echo 
    exit 6
esac
exit 0
