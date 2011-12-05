#!/bin/sh

demux=$1
case "$2" in
	skyitalia)
		/usr/crossepg/crossepg_downloader -x $demux -d /tmp -p skyit_hotbird_13.0
		;;
	skyuk)
		/usr/crossepg/crossepg_downloader -x $demux -d /tmp -p skyuk_astra2_28.2
		;;
	skyuk284)
		/usr/crossepg/crossepg_downloader -x $demux -d /tmp -p skyuk_astra2_28.4
		;;
	ausat)
		/usr/crossepg/crossepg_downloader -x $demux -d /tmp -p ausat_optusc1_156.0
		;;
	*)
		echo $"Usage :$0 {/dev/dvb/adapter0/demux0} {skyitalia|skyuk|skyuk284|ausat}"
	;;
esac
