#!/bin/sh

envpath=..
srcpath=$envpath/src
[ -e $envpath/src_patched ] || mkdir $envpath/src_patched 
[ -e $envpath/openembedded/sources ] || mkdir $envpath//openembedded/sources
srcpatchedpath=$envpath/src_patched
sourcespath=$envpath/openembedded/sources
oepath15=$envpath/openembedded/1.5
oepath16=$envpath/openembedded/1.6
cvspath=$envpath/cvs
curdir=`pwd`

oe_e1_15_path=$oepath15/openembedded/packages/enigma
e1_libs_path=$srcpath/apps/apps.tuxbox.libs_cvs.tuxbox.org__20070307/libs
e1_libtuxbox_path=$srcpath/apps/apps.tuxbox.libtuxbox_cvs.tuxbox.org__20040928/libtuxbox
e1_plugins_path=$srcpath/apps/apps.tuxbox.plugins_cvs.tuxbox.org__20061009/plugins
e1_cvssrc=$srcpath/enigma
if [ -e $oe_e1_15_path/enigma_cvs.bb ]; then
	e1_srcdate15=`grep SRCDATE $oe_e1_15_path/enigma_cvs.bb| grep -m 1 "[12][09][0-9][0-9][01][0-9][0-3][0-9]" |sed -e 's/[a-zA-Z" =]//g'`
fi

e1_arc_15=apps.tuxbox.enigma_cvs.tuxbox.org__$e1_srcdate15.tar.gz
e1_plugins=apps.tuxbox.plugins_cvs.tuxbox.org__20061009.tar.gz
e1_libtuxbox=apps/apps.tuxbox.libtuxbox_cvs.tuxbox.org__20040928.tar.gz
e1_libs=apps.tuxbox.libs_cvs.tuxbox.org__20070307.tar.gz

oe_e2_15_path=$oepath15/openembedded/packages/enigma2
oe_e2_16_path=$oepath16/openembedded/recipes/enigma2
e2_cvssrc=$srcpath/enigma2

e2_arc_16=enigma2_NDE_16.tar.gz
e2_plugins=enigma2-plugins_NDE.tar.gz

oe_package_15()
{
	# patch oe package
	echo -n "Patch OE 1.5 packages...."
	mkdir -p $envpath/tmp/oe/packages
	cp -r openembedded-1.5/packages/* $envpath/tmp/oe/packages
	find $envpath/tmp/oe/packages -name ".svn" | xargs rm -rf
	cp -r $envpath/tmp/oe/packages/* $oepath15/openembedded/packages
	rm -rf $envpath/tmp
	echo "Patched!"
}

oe_package_16()
{
	# patch oe package
	echo -n "Patch OE 1.6 recipes...."
	mkdir -p $envpath/tmp/oe/recipes
	cp -r openembedded-1.6/recipes/* $envpath/tmp/oe/recipes
	find $envpath/tmp/oe/recipes -name ".svn" | xargs rm -rf
	cp -r $envpath/tmp/oe/recipes/* $oepath16/openembedded/recipes
	rm -rf $envpath/tmp
	echo "Patched!"
}

oe_conf_15()
{
	# patch oe conf
	echo -n "Patch OE 1.5 conf...."
	mkdir -p $envpath/tmp/oe/conf
	cp -r openembedded-1.5/conf/* $envpath/tmp/oe/conf
	find $envpath/tmp/oe/conf -name ".svn" | xargs rm -rf
	cp -r $envpath/tmp/oe/conf/* $oepath15/openembedded/conf
	rm -rf $envpath/tmp
	echo "Patched!"
}

oe_conf_16()
{
	# patch oe conf
	echo -n "Patch OE 1.6 conf...."
	mkdir -p $envpath/tmp/oe/conf
	cp -r openembedded-1.6/conf/* $envpath/tmp/oe/conf
	find $envpath/tmp/oe/conf -name ".svn" | xargs rm -rf
	cp -r $envpath/tmp/oe/conf/* $oepath16/openembedded/conf
	rm -rf $envpath/tmp
	echo "Patched!"
}

enigma1_15()
{
	# Prepare enigma1 package
	if [ -e $e1_cvssrc ]; then
		echo -n "Remove $srcpatchedpath/enigma... "
		[ -e $srcpatchedpath/enigma ] && rm -r $srcpatchedpath/enigma
		echo 'Removed!'
		echo -n "Copy $srcpath/enigma/* $srcpatchedpath/enigma... "
		cp -r $srcpath/enigma $srcpatchedpath/ && echo 'Copied!'
		echo -n "patch/enigma1/enigma/* to $srcpatchedpath/enigma... "
		cp -r patch/enigma1/enigma/* $srcpatchedpath/enigma/ && echo 'Copied!'
		echo -n "patch/po/enigma1/it.po to $srcpatchedpath/enigma/po... "
		cp -r patch/po/enigma1/it.po $srcpatchedpath/enigma/po && echo 'Copied!'
		echo -n "patch/po/enigma1/Makefile.am to $srcpatchedpath/enigma/po... "
		cp -r patch/po/enigma1/Makefile.am $srcpatchedpath/enigma/po && echo 'Copied!'
		echo -n "Merge patch/po/enigma1/panel_it.po in $srcpatchedpath/enigma/po/it.po... "
		cat patch/po/enigma1/panel_it.po >> $srcpatchedpath/enigma/po/it.po  && echo 'Merged!'
		cd $srcpatchedpath
		find enigma/ -name ".svn" | xargs rm -rf
		echo -n "Create $e1_arc_15 archive... "
		tar -zcf $e1_arc_15 enigma/ && echo 'Created!'
		echo -n "Move $e1_arc_15 to $sourcespath... "
		mv $e1_arc_15 $sourcespath  && echo 'Moved!'
		echo -n "Copy $srcpath/apps/* $sourcespath... "
		cp -r $srcpath/apps/* $sourcespath && echo 'Copied!'
	else
		echo "$e1_cvssrc not Found!"
		echo "Please obtain e1 cvs sources in your env path!"
		exit 1
	fi
	cd $curdir
}

enigma2_16()
{
	# Prepare enigma2 OE 1.6 package
	if [ -e $e2_cvssrc ]; then
		echo -n "Remove $srcpatchedpath/enigma2... "
		[ -e $srcpatchedpath/enigma2 ] && rm -rf $srcpatchedpath/enigma2
		echo 'Removed!'
		mkdir $srcpatchedpath/enigma2
		echo -n "Copy $e2_cvssrc $srcpatchedpath/... "
		cp -r $e2_cvssrc $srcpatchedpath  && echo 'Copied!'
		echo -n "Copy patch/enigma2/* to $srcpatchedpath/enigma2/... "
		cp -r patch/enigma2/* $srcpatchedpath/enigma2  && echo 'Copied!'
		echo -n "Copy patch/po/enigma2/it.po to $srcpatchedpath/enigma2/po... "
		cp -r patch/po/enigma2/it.po $srcpatchedpath/enigma2/po  && echo 'Copied!'
		echo -n "Copy patch/po/enigma2/Makefile.am to $srcpatchedpath/enigma2/po... "
		cp -r patch/po/enigma2/Makefile.am $srcpatchedpath/enigma2/po  && echo 'Copied!'
		echo -n "Merge patch/po/enigma2/panel_it.po in $srcpatchedpath/enigma2/po/it.po... "
		cat patch/po/enigma2/panel_it.po >> $srcpatchedpath/enigma2/po/it.po  && echo 'Merged!'
		cd $srcpatchedpath
		find enigma2/ -name ".svn" | xargs rm -rf
		echo -n "Create $e2_arc_16 archive... "
		tar -zcf $e2_arc_16 enigma2/  && echo 'Created!'
		echo -n "Move $e2_arc_16 to $sourcespath... "
		mv $e2_arc_16 $sourcespath  && echo 'Moved!'
	else
		echo "$e2_cvssrc not Found!"
		echo "Please run the command './install.sh' in your env path!"
		exit 1
	fi
	cd $curdir
}

enigma2_plugins()
{
	# prepare enigma2 plugins package
	if [ -e $srcpath/enigma2-plugins ]; then
		echo 'Prepare enigma2 plugins package...'
		echo -n "Remove $srcpatchedpath/enigma2-plugins/... "
		[ -e $srcpatchedpath/enigma2-plugins ] && rm -rf $srcpatchedpath/enigma2-plugins
		echo 'Removed!'
		cp -r $srcpath/enigma2-plugins $srcpatchedpath/
		cp -r patch/enigma2-plugins/* $srcpatchedpath/enigma2-plugins/
		cd $srcpatchedpath
		find . -name ".svn" | xargs rm -rf
		echo -n "Create $e2_plugins archive... "
		tar -zcf $e2_plugins enigma2-plugins/ && echo 'Created!'
		mv $e2_plugins $sourcespath
		cd $curdir
	else
		echo "$srcpath/enigma2-plugins not Found!"
		echo "Please run the command './install.sh' in your env path!"
		exit 1
	fi
}

script()
{
	# copy make image scripts
	for i in dm800 dm8000 dm7025 dm500hd dm800se dm7020hd
	do
		[ -d $oepath16/$i/build ] && cp -v make_e2_image_16.sh $oepath16/$i/build/make_e2_image.sh
	done
	for i in dm500plus dm600pvr dm7020
	do
		[ -d $oepath15/$i/build ] && cp -v make_e1_image_15.sh $oepath15/$i/build/make_e1_image.sh
	done
	for i in dm500 dm56x0 dm7000
	do
		[ -d $cvspath/$i/cdk ] && cp -v make_cvs_image.sh $cvspath/$i/cdk/
	done
}

case "$1" in
	enigma1)
		[ -e $oe_e1_15_path/enigma_cvs.bb ] &&	enigma1_15
		;;
	enigma2)
		if [ -e $oe_e2_16_path/enigma2.bb ]; then
			enigma2_16
		fi
		;;
	pluginse2)
		enigma2_plugins
		;;
	oe_pack_15)
		oe_package_15
		;;
	oe_pack_16)
		oe_package_16
		;;
	oe_pack_all)
		oe_package_15
		oe_package_16
		;;
	oe_conf_15)
		oe_conf_15
		;;
	oe_conf_16)
		oe_conf_16
		;;
	oe_conf_all)
		oe_conf_15
		oe_conf_16
		;;
	script)
		script
		;;
	*)
		echo "Usage: $0 {command}"
		echo 'command:enigma1'
		echo -e '\tenigma2\n\tpluginse2\n\toe_pack_15\n\toe_pack_16\n\toe_pack_all\n\toe_conf_15\n\toe_conf_16\n\toe_conf_all\n\tscript'
		;;
esac

# fine
cd $curdir

