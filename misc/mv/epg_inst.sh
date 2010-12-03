#!/bin/sh

if grep "/var/mnt/usb" /proc/mounts
then
  DST=1
elif grep "/mnt/usb" /proc/mounts
then
  DST=2
elif grep "/var/mnt/cf" /proc/mounts
then
  DST=3
elif grep "/mnt/cf" /proc/mounts
then
  DST=4
elif grep "/hdd" /proc/mounts
then
  DST=5
else
  DST=6
fi

PASS=dreambox
heure0=0630
LIT_AU_BOOT=N
KILLENIGMA=O

# destination
echo -e \
"Where do you want to install :
	1 : USB (/var/mnt/usb)
	2 : USB (/mnt/usb)
	3 : CF  (/var/mnt/cf)
	4 : CF  (/mnt/cf)
	5 : hard disk (/hdd/mv)
	6 : /var/tuxbox/config/mv
	   (défaut=$DST)"
read -p "	" TEST

DST=${TEST:-$DST}
case "$DST" in
	"1"|"U"|"u")
	PATHMV=/var/mnt/usb/mv
	PATHMV2=\\/var\\/mnt\\/usb\\/mv
	;;
	"2")
	PATHMV=/mnt/usb/mv
	PATHMV2=\\/mnt\\/usb\\/mv
	;;
	"2")
	PATHMV=/mnt/usb/mv
	PATHMV2=\\/mnt\\/usb\\/mv
	;;
	"3")
	PATHMV=/var/mnt/cf/mv
	PATHMV2=\\/var\\/mnt\\/cf\\/mv
	;;
	"4")
	PATHMV=/mnt/cf/mv
	PATHMV2=\\/mnt\\/cf\\/mv
	;;
	"5"|"D"|"d"|"")
	PATHMV=/hdd/mv
	PATHMV2=\\/hdd\\/mv
	;;
	"6")
	PATHMV=/var/tuxbox/config/mv
	PATHMV2=\\/var\\/tuxbox\\/config\\/mv
	;;
	*)
	echo "Unknown answer ??? stop!\n"
	exit 1
	;;
esac

# mot de passe root
echo -e \
" Enter root password :
	(default=dreambox)
	(for sandbox (POD) : enter sandbox)"
read -p "	" TEST

case "$TEST" in
	"")
	PASS="dreambox"
	;;
	*)
	PASS="$TEST"
	;;
esac

# kill enigma ?
echo -e \
"Do you want to restart enigma each night ?
	This allows to free memory
	and avoid crashes.
	(default=Y)"
read -p "	" TEST
case "$TEST" in
	"Y"|"y"|"O"|"o"|"")
	KILLENIGMA=O
	;;
	"N"|"n")
	KILLENIGMA=N
	;;
	*)
	echo "Unknown answer ??? stop!\n"
	exit 1
	;;
esac

# heure de chargement
echo -e \
"At what time to program the automatic loading of the EPG ?
	format : HHmi (example : 0730 for 7 hours 30 minutes)
	default : 0630"
read -p "	" TEST
case "$TEST" in
	"")
	heure0="0630"
	;;
	*)
	heure0="$TEST"
	;;
esac

PATHINST=`pwd`

rm -rf /var/tuxbox/config/mv

mkdir -p $PATHMV 2>/dev/null
if [ ! -d $PATHMV ]
then
  echo "Unable to create MV directory !!!"
  exit 1
fi
cd $PATHMV/..

  tar zxf $PATHINST/inst_mv.tgz
  cd mv
  cp -p mv.cfg /var/tuxbox/plugins/extepg.cfg
  rm -f /var/tuxbox/plugins/extepg.so
  ln -s $PATHMV/mv.so /var/tuxbox/plugins/extepg.so
  ln -s $PATHMV /var/tuxbox/config/mv
  mkdir -p /var/lib 2>/dev/null
  ln -s /lib/libgcc_s.so.1 /var/lib/libgcc_s_nof.so.1

mkdir -p /var/bin 2>/dev/null
sed "s/^heure0=.*/heure0=$heure0/;s/^LIT_AU_BOOT=.*/LIT_AU_BOOT=$LIT_AU_BOOT/" getepgd >/var/bin/getepgd
chmod 755 /var/bin/getepgd

rm -f /var/bin/mvccat
cp -p mvccat /var/bin

sed "s/PASS=.*/PASS=$PASS/;s/KILLENIGMA=.*/KILLENIGMA=$KILLENIGMA/" getepg >getepg.tmp
mv getepg.tmp getepg
chmod 755 getepg

# mise en place de getepgd au demarrage
if [ ! -f /var/etc/init  ]
then
        echo "pids=\$(ps  | grep 'getepgd'|grep -v grep | sed 's/^[ ]*\([0-9]*\).*/\1/')" >/var/etc/init
        echo "kill -9 \$pids" >>/var/etc/init
	echo "/var/bin/getepgd >/dev/null 2>&1 </dev/null &" >>/var/etc/init
	chmod 755 /var/etc/init
else
  if grep "getepgd" /var/etc/init
  then
	echo
  else
        echo "pids=\$(ps  | grep 'getepgd'|grep -v grep | sed 's/^[ ]*\([0-9]*\).*/\1/')" >>/var/etc/init
        echo "kill -9 \$pids" >>/var/etc/init
	echo "/var/bin/getepgd >/dev/null 2>&1 </dev/null &" >>/var/etc/init
	chmod 755 /var/etc/init
  fi
fi

# redémarrage de getepgd :
pids=$(ps  | grep 'getepgd'|grep -v grep | sed 's/^[ ]*\([0-9]*\).*/\1/')
kill -9 $pids
/var/bin/getepgd >/dev/null 2>&1 </dev/null &

