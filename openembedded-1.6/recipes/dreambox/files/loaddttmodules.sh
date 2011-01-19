#!/bin/sh

DRVDIR=/lib/modules/dvbt

case "$1" in
dvb-a867)
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-a867.ko
	;;
dvb-as102)
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-as102.ko
	;;
b2c2-flexcop-usb)
	;;
dvb-usb-a800)
	insmod $DRVDIR/mt2060.ko
	insmod $DRVDIR/dibx000_common.ko
	insmod $DRVDIR/dib3000mc.ko
	insmod $DRVDIR/dvb-usb-dibusb-common.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-a800.ko
	;;
dvb-usb-af9005)
	insmod $DRVDIR/qt1010.ko
	insmod $DRVDIR/mt2060.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-af9005.ko
	;;
dvb-usb-af9015)
	insmod $DRVDIR/mxl5005s.ko
	insmod $DRVDIR/mc44s803.ko
	insmod $DRVDIR/mt2060.ko
	insmod $DRVDIR/qt1010.ko
	insmod $DRVDIR/tda18271.ko
	insmod $DRVDIR/tda18218.ko
	insmod $DRVDIR/dvb-usb.ko
	insmod $DRVDIR/af9013.ko
	insmod $DRVDIR/dvb-pll.ko
	sleep 1
	insmod $DRVDIR/dvb-usb-af9015.ko
	;;
dvb-usb-af9035)
	insmod $DRVDIR/mxl5007t.ko
	insmod $DRVDIR/tua9001.ko
	insmod $DRVDIR/af9033.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 1
	insmod $DRVDIR/dvb-usb-af9035.ko
	;;
dvb-usb-anysee)
	;;
dvb-usb-au6610)
	insmod $DRVDIR/zl10353.ko
	insmod $DRVDIR/qt1010.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-au6610.ko
	;;
dvb-usb-az6027)
	insmod $DRVDIR/stb6100.ko
	insmod $DRVDIR/stb0899.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-az6027.ko
	;;
dvb-usb-ce6230)
	insmod $DRVDIR/mxl5005s.ko
	insmod $DRVDIR/zl10353.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-ce6230.ko
	;;
dvb-usb-cinergyT2)
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-cinergyT2.ko
	;;
dvb-usb-cxusb)
	;;
dvb-usb-dib0700)
	insmod $DRVDIR/mt2266.ko
	insmod $DRVDIR/tuner-xc2028.ko
	insmod $DRVDIR/mxl5007t.ko
	insmod $DRVDIR/xc5000.ko
	insmod $DRVDIR/s5h1411.ko
	insmod $DRVDIR/mt2060.ko
	insmod $DRVDIR/lgdt3305.ko
	insmod $DRVDIR/dibx000_common.ko
	insmod $DRVDIR/dib7000p.ko
	insmod $DRVDIR/dib0090.ko
	insmod $DRVDIR/dib7000m.ko
	insmod $DRVDIR/dib0070.ko
	insmod $DRVDIR/dib8000.ko
	insmod $DRVDIR/dib3000mc.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 1
	insmod $DRVDIR/dvb-usb-dib0700.ko
	;;
dvb-usb-dibusb-mb)
	insmod $DRVDIR/mt2060.ko
	insmod $DRVDIR/dibx000_common.ko
	insmod $DRVDIR/dib3000mc.ko
	insmod $DRVDIR/dib3000mb.ko
	insmod $DRVDIR/dvb-usb-dibusb-common.ko
	insmod $DRVDIR/dvb-usb.ko
	insmod $DRVDIR/dvb-pll.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-dibusb-mb.ko
	;;
dvb-usb-dibusb-mc)
	insmod $DRVDIR/mt2060.ko
	insmod $DRVDIR/dibx000_common.ko
	insmod $DRVDIR/dib3000mc.ko
	insmod $DRVDIR/dvb-usb-dibusb-common.ko
	insmod $DRVDIR/dvb-pll.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-dibusb-mc.ko
	;;
dvb-usb-digitv)
	;;
dvb-usb-dtt200u)
	insmod $DRVDIR/dvb-usb.ko
	insmod $DRVDIR/dvb-usb-dtt200u.ko
	;;
dvb-usb-dtv5100)
	insmod $DRVDIR/zl10353.ko
	insmod $DRVDIR/qt1010.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-dtv5100.ko
	;;
dvb-usb-dw2102)
	insmod $DRVDIR/stv6110.ko
	insmod $DRVDIR/stb6100.ko
	insmod $DRVDIR/cx24116.ko
	insmod $DRVDIR/tda10023.ko
	insmod $DRVDIR/stb6000.ko
	insmod $DRVDIR/ds3000.ko
	insmod $DRVDIR/si21xx.ko
	insmod $DRVDIR/mt312.ko
	insmod $DRVDIR/stv0900.ko
	insmod $DRVDIR/zl10039.ko
	insmod $DRVDIR/st0299.ko
	insmod $DRVDIR/stv0288.ko
	insmod $DRVDIR/dvb-usb.ko
	insmod $DRVDIR/dvb-pll.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-dw2102.ko
	;;
dvb-usb-ec168)
	insmod $DRVDIR/mxl5005s.ko
	insmod $DRVDIR/ec100.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-ec168.ko
	;;
dvb-usb-friio)
	;;
dvb-usb-gl861)
	insmod $DRVDIR/zl10353.ko
	insmod $DRVDIR/qt1010.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-gl861.ko
	;;
dvb-usb-gp8psk)
	;;
dvb-usb-m920x)
	insmod $DRVDIR/mt352.ko
	insmod $DRVDIR/tuner-simple.ko
	insmod $DRVDIR/tuner-types.ko
	insmod $DRVDIR/tda1004x.ko
	insmod $DRVDIR/tda827x.ko
	insmod $DRVDIR/qt1010.ko
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-920x.ko
	;;
dvb-usb-nova-t-usb2)
	insmod $DRVDIR/mt2060.ko
	insmod $DRVDIR/dibx000_common.ko
	insmod $DRVDIR/dib3000mc.ko
	insmod $DRVDIR/dvb-usb-dibusb-common.ko
	insmod $DRVDIR/dvb-usb.ko
	insmod $DRVDIR/dvb-pll.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-nova-t-usb2.ko
	;;
dvb-usb-opera)
	;;
dvb-usb-ttusb2)
	;;
dvb-usb-umt-010)
	;;
dvb-usb-vp702x)
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-vp702x.ko
	;;
dvb-usb-vp7045)
	insmod $DRVDIR/dvb-usb.ko
	sleep 2
	insmod $DRVDIR/dvb-usb-vp7045.ko
	;;
smsusb)
	insmod $DRVDIR/smsmdtv.ko
	sleep 1
	insmod $DRVDIR/smsdvb.ko
	insmod $DRVDIR/smsusb.ko
	;;
dvb-ttusb-budget)
	;;
ttusb-dec)
	;;
em28xx)
	;;
*)
	$0
	;;
esac

exit $?
