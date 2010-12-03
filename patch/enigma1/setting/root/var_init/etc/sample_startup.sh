#!/bin/sh
#--------------------------------------------------------------------------------------------------------
# You can create more boot script, please dont use this script but make a copy
# command:
# cp sample_startup.sh your-name-script_startup.sh
# this script name must terminate with _startup.sh
#--------------------------------------------------------------------------------------------------------

#--------------------------------------------------------------------------------------------------------
# PLease change the following 3 line with your parameter
#--------------------------------------------------------------------------------------------------------

name='Your deamon name'  #Dipalay name in a deamon's panel
binarypath='/var/bin/'   #Binary executable path with / final
binaryname='yourbinary'	 #Binary executable name

#--------------------------------------------------------------------------------------------------------
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!  DON'T CHANGE THE FOLLOWING CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------------------------------------------------------------------

	case "$1" in
	start)
		if [ -e ${binarypath}${binaryname} ]; then
			[ ! -e /var/etc/.dont_start_${binaryname} ] && ${binarypath}${binaryname} &
		fi
		;;
	stop)
		killall -9 ${binaryname}
		;;
	*)
		$0 stop
		;;
	esac
exit 0
