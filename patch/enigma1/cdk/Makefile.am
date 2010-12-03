PATH := $(hostprefix)/ccache-bin:$(hostprefix)/bin:$(PATH)
nemroot=$(hostprefix)/../../../../nemesis
BUILDENV := \
	AR=$(target)-ar \
	AS=$(target)-as \
	CC=$(target)-gcc \
	CXX=$(target)-g++ \
	NM=$(target)-nm \
	RANLIB=$(target)-ranlib \
	CFLAGS="$(TARGET_CFLAGS)" \
	CXXFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	PKG_CONFIG_PATH=$(targetprefix)/lib/pkgconfig

all: core libs root boot apps devel
extra: java contrib_apps fun dvb_apps

#######################
#
#   core
#

core: .directories .binutils .linuxdir .glibc .gcc

checkout_lcars:
	CURRENT_PATH=`pwd`;
	cd ../apps/tuxbox/lcars && cvs update -d -P -A;
	cd $$CURRENT_PATH;

checkout_neutrino:
	CURRENT_PATH=`pwd`;
	cd ../apps && cvs update -d -P -A;
	cd $$CURRENT_PATH; 

checkout:
	@for i in apps/tuxbox/enigma apps/dvb/dvbsnoop \
		apps/dvb/tools/stream apps/tuxbox/plugins \
		apps/tuxbox/libs ; do \
		CURRENT_PATH=`pwd`; \
		cd ../$$i && cvs update -d -P -A; \
		cd $$CURRENT_PATH; \
	done

.bootstrap:
	@if [ ! -f .glibc ]; then \
		$(MAKE) .glibc .gcc; \
	elif [ ! -f $(hostprefix)/bin/$(target)-gcc ]; then \
		$(MAKE) .gcc; \
	fi
	touch $@

.directories:
	@for i in bin boot dev etc include mnt lib lib/pkgconfig proc root sbin tmp var/run $(UCODEDIR); do \
		$(INSTALL) -d $(targetprefix)/$$i ; done;
	$(INSTALL) -d $(hostprefix)/$(target)
	$(INSTALL) -d $(bootprefix)
	-rm $(hostprefix)/$(target)/include
	-rm $(hostprefix)/$(target)/lib
	-ln -s $(targetprefix)/include $(hostprefix)/$(target)/include
	-ln -s $(targetprefix)/lib $(hostprefix)/$(target)/lib
if TARGETRULESET_FLASH
	$(INSTALL) -d $(flashprefix)
endif
	if [ -e $(ccachedir)/ccache ]; then \
		$(INSTALL) -d $(hostprefix)/ccache-bin ;\
		ln -s $(ccachedir)/ccache $(hostprefix)/ccache-bin/powerpc-tuxbox-linux-gnu-gcc ;\
		ln -s $(ccachedir)/ccache $(hostprefix)/ccache-bin/powerpc-tuxbox-linux-gnu-g++ ;\
		ln -s $(ccachedir)/ccache $(hostprefix)/ccache-bin/powerpc-tuxbox-linux-gnu-cpp ;\
		ln -s $(ccachedir)/ccache $(hostprefix)/ccache-bin/powerpc-tuxbox-linux-gnu-gcc-.3.4.4 ;\
	fi
	touch $@

.linuxdir: @DEPENDS_linux@
	@if [ ! -d $(targetprefix)/bin ]; then \
		$(MAKE) .directories; \
	fi
	@PREPARE_linux@
if BOXTYPE_DM7000
	cat Patches/dm7000_defconfig > @DIR_linux@/.config
endif
if BOXTYPE_DM56x0
	cat Patches/dm56x0_defconfig > @DIR_linux@/.config
	cp Patches/maxvar_dreambox.c @DIR_linux@/drivers/mtd/maps/dreambox.c
endif
if BOXTYPE_DM500
	cat Patches/dm500_defconfig > @DIR_linux@/.config
	cp Patches/maxvar_dreambox.c @DIR_linux@/drivers/mtd/maps/dreambox.c
endif
	$(MAKE) -C @DIR_linux@ oldconfig \
		ARCH=ppc
	$(MAKE) -C @DIR_linux@ include/linux/version.h \
		ARCH=ppc
	-ln -sf $(buildprefix)/linux/include/asm-ppc $(buildprefix)/linux/include/asm
	touch $@

.binutils: @DEPENDS_binutils@
	@if [ ! -h linux ]; then \
		$(MAKE) .linuxdir; \
	fi
	@PREPARE_binutils@
	cd @DIR_binutils@ && \
		CC=$(CC) \
		CFLAGS="$(CFLAGS) -D_FORTIFY_SOURCE=0" \
		@CONFIGURE_binutils@ \
			--target=$(target) \
			--prefix=$(hostprefix) \
			--disable-nls \
			--nfp && \
		$(MAKE) all all-gprof && \
		@INSTALL_binutils@
	@CLEANUP_binutils@
	touch $@

#
# gcc first stage without glibc
#
bootstrap_gcc: @DEPENDS_bootstrap_gcc@
	@if [ ! -d $(hostprefix)/$(target)/bin/as ]; then \
		$(MAKE) .binutils; \
	fi
	@PREPARE_bootstrap_gcc@
	$(INSTALL) -d $(hostprefix)/$(target)/sys-include
	ln -sf $(buildprefix)/linux/include/asm $(hostprefix)/$(target)/sys-include/
	@for i in linux asm-generic asm mtd ; do \
		rm -rf $(hostprefix)/$(target)/include/$$i 2> /dev/null || /bin/true; \
		ln -sf $(buildprefix)/linux/include/$$i $(hostprefix)/$(target)/include; \
	done;
	cd @DIR_bootstrap_gcc@ && \
		CC=$(CC) CFLAGS="$(CFLAGS) -D_FORTIFY_SOURCE=0" \
		@CONFIGURE_bootstrap_gcc@ \
			--build=$(build) \
			--host=$(build) \
			--target=$(target) \
			--prefix=$(hostprefix) \
			--with-cpu=$(CPU_MODEL) \
			--enable-target-optspace \
			--enable-languages="c" \
			--disable-shared \
			--disable-nls \
			--disable-threads \
			--with-float=soft && \
		$(MAKE) all && \
		@INSTALL_bootstrap_gcc@
	rm -rf $(hostprefix)/$(target)/sys-include
	@CLEANUP_bootstrap_gcc@

.glibc: @DEPENDS_glibc@ @DEPENDS_linux_libc_headers@
	@rm $(hostprefix)/bin/$(target)-gcc 2>/dev/null || /bin/true
	@if [ ! -f $(hostprefix)/bin/$(target)-gcc ]; then \
		$(MAKE) bootstrap_gcc; \
	elif [ ! -f $(hostprefix)/bin/$(target)-gcc ]; then \
		$(MAKE) .gcc; \
	fi
	@PREPARE_glibc@
	cd @DIR_glibc@ && \
		CC=$(target)-gcc \
		AR=$(target)-ar \
		RANLIB=$(target)-ranlib \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		@CONFIGURE_glibc@ \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--with-headers=$(buildprefix)/linux/include \
			--enable-kernel=2.6.5 \
			--disable-profile \
			--enable-shared \
			--enable-add-ons=nptl \
			--with-__thread \
			--without-fp \
			--with-tls && \
		$(MAKE) all && \
		@INSTALL_glibc@
	@CLEANUP_glibc@
	@PREPARE_linux_libc_headers@
	mv $(buildprefix)/linux-libc-headers/include/asm-ppc $(buildprefix)/linux-libc-headers/include/asm
	for i in linux asm asm_generic; do \
		rm -R $(hostprefix)/$(target)/include/$$i 2> /dev/null || /bin/true; \
		if [ $$i != asm_generic ] ; then \
			mv $(buildprefix)/linux-libc-headers/include/$$i $(hostprefix)/$(target)/include; \
		fi; \
	done;
	for i in config.h autoconf.h; do \
		ln -sf $(buildprefix)/linux/include/linux/$$i $(hostprefix)/$(target)/include/linux; \
	done;
	@CLEANUP_linux_libc_headers@
	rm $(hostprefix)/$(target)/include/asm-generic 2> /dev/null || /bin/true;
	rm $(buildprefix)/linux-libc-headers 2> /dev/null || /bin/true
	touch $@

#
# uClibc
# a minimalistic libc, won't currently work with libstdc++
#
.uclibc: @DEPENDS_uclibc@
	@PREPARE_uclibc@
	cd @DIR_uclibc@ && \
		$(MAKE) all CROSS=$(target)- && \
		@INSTALL_uclibc@
	@CLEANUP_uclibc@
	touch $@

#
# gcc second stage with glibc
#
.gcc: @DEPENDS_gcc@
	@if [ ! -f $(hostprefix)/$(target)/lib/libc.so ]; then \
		$(MAKE) .glibc; \
	fi
	@PREPARE_gcc@
	$(INSTALL) -d $(hostprefix)/$(target)/sys-include
	@if [ -s $(hostprefix)/$(target)/lib/nof ] ; then \
		rm $(hostprefix)/$(target)/lib/nof; \
	fi
	cp -p $(hostprefix)/$(target)/include/limits.h $(hostprefix)/$(target)/sys-include/
	cd @DIR_gcc@ && \
		CC=$(CC) CFLAGS="$(CFLAGS) -D_FORTIFY_SOURCE=0" \
		@CONFIGURE_gcc@ \
			--build=$(build) \
			--host=$(build) \
			--target=$(target) \
			--prefix=$(hostprefix) \
			--with-cpu=$(CPU_MODEL) \
			--enable-target-optspace \
			--enable-languages="c,c++" \
			--enable-shared \
			--enable-threads=posix \
			--disable-nls \
			--with-float=soft \
			--enable-__cxa_atexit && \
		$(MAKE) all && \
		@INSTALL_gcc@
	rm -rf $(hostprefix)/$(target)/sys-include
	@for i in `find $(hostprefix)/$(target)/lib/nof -type f` ; do mv $$i $(hostprefix)/$(target)/lib; done
	@for i in `find $(hostprefix)/$(target)/lib/nof -type l` ; do mv $$i $(hostprefix)/$(target)/lib; done
	rm -rf $(hostprefix)/$(target)/lib/nof
	ln -sf $(hostprefix)/$(target)/lib $(hostprefix)/$(target)/lib/nof
	@CLEANUP_gcc@
	touch $@

#######################
#
#   root
#

root: \
	.root .busybox .ftpd .modutils \
	.procps .watchdog

root_optional: \
	.portmap .mrouted

if BUSYBOX_113
.busybox: .bootstrap @DEPENDS_busybox113@
	@PREPARE_busybox113@
	cat Patches/busybox113.config > @DIR_busybox113@/.config
	cd @DIR_busybox113@ && \
		$(MAKE) all install \
			CROSS_COMPILE=$(target)- \
			CFLAGS_EXTRA="-Os -msoft-float" \
			ARCH=powerpc PREFIX=$(targetprefix) && \
	@CLEANUP_busybox113@
	touch $@
else
if BUSYBOX_17
.busybox: .bootstrap @DEPENDS_busybox17@
	@PREPARE_busybox17@
	cat Patches/busybox17.config > @DIR_busybox17@/.config
	cd @DIR_busybox17@ && \
		$(MAKE) all install \
			CROSS_COMPILE=$(target)- \
			CFLAGS_EXTRA="-Os -msoft-float" \
			ARCH=ppc PREFIX=$(targetprefix)
	@CLEANUP_busybox17@
	touch $@
else
if BUSYBOX_12
.busybox: .bootstrap @DEPENDS_busybox12@
	@PREPARE_busybox12@
	cat Patches/busybox12.config > @DIR_busybox12@/.config
	cd @DIR_busybox12@ && \
		$(MAKE) dep all \
			CROSS=$(target)- \
			CFLAGS_EXTRA="-Os -msoft-float" && \
		@INSTALL_busybox12@
	@CLEANUP_busybox12@
	touch $@
else
.busybox: .bootstrap @DEPENDS_busybox@
	@PREPARE_busybox@
	cat Patches/busybox.config > @DIR_busybox@/.config
	cd @DIR_busybox@ && \
		$(MAKE) dep all \
			CROSS=$(target)- \
			CFLAGS_EXTRA="-Os -msoft-float" && \
		@INSTALL_busybox@
	@CLEANUP_busybox@
	touch $@
endif
endif
endif

.root:
	$(MAKE) -C root install
	touch $@

.ftpd: .bootstrap @DEPENDS_ftpd@
	@PREPARE_ftpd@
	cd @DIR_ftpd@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) && \
		@INSTALL_ftpd@
	@CLEANUP_ftpd@
	touch $@

.modutils: .bootstrap @DEPENDS_modutils@
	@PREPARE_modutils@
	cd @DIR_modutils@ && \
		$(BUILDENV) \
		BUILDCC=$(CC) BUILDCFLAGS="-O2" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--disable-root-check \
			--disable-strip && \
		$(MAKE) && \
		@INSTALL_modutils@
	@CLEANUP_modutils@
	touch $@

.portmap: .bootstrap @DEPENDS_portmap@
	@PREPARE_portmap@
	cd @DIR_portmap@ && \
		$(BUILDENV) \
		$(MAKE) && \
		$(FAKEROOT) $(MAKE) BASEDIR=$(targetprefix) install && \
		cd .. && \
	@CLEANUP_portmap@
	touch $@

.procps: .bootstrap .libncurses @DEPENDS_procps@
	@PREPARE_procps@
	cd @DIR_procps@ && \
		$(BUILDENV) \
		$(MAKE) top ps && \
		@INSTALL_procps@
	@CLEANUP_procps@
	touch $@

.procps3: .bootstrap .libncurses @DEPENDS_procps3@
	@PREPARE_procps3@
	cd @DIR_procps3@ && \
		$(BUILDENV) \
		$(MAKE) CFLAGS="$(CFLAGS) -I$(targetprefix)/include/ncurses -D__GNU_LIBRARY__" top ps/ps && \
		@INSTALL_procps3@
	@CLEANUP_procps3@
	touch $@

.watchdog: .bootstrap @DEPENDS_watchdog@
	@PREPARE_watchdog@
	cd @DIR_watchdog@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--cache-file=/dev/null && \
		$(MAKE) all && \
		@INSTALL_watchdog@
	@CLEANUP_watchdog@
	touch $@

.mrouted: .bootstrap @DEPENDS_mrouted@
	@PREPARE_mrouted@
	cd @DIR_mrouted@ && \
		$(BUILDENV) \
		$(MAKE) all && \
		$(INSTALL) -m755 mrouted $(targetprefix)/bin/mrouted && \
		$(INSTALL) -m755 map-mbone $(targetprefix)/bin/map-mbone && \
		$(INSTALL) -m755 mrinfo $(targetprefix)/bin/mrinfo
	@CLEANUP_mrouted@
	touch $@

#######################
#
# mksquashfs with LZMA support
#
mksquashfs: .mksquashfs
.mksquashfs: .bootstrap @DEPENDS_squashfs@
	rm -rf @DIR_squashfs@
	mkdir -p @DIR_squashfs@
	cd @DIR_squashfs@ && \
	bunzip2 -cd ../Archive/lzma442.tar.bz2 | TAPE=- tar -x && \
	patch -p1 < ../Patches/lzma_zlib-stream.diff && \
	gunzip -cd ../Archive/squashfs2.2-r2.tar.gz | TAPE=- tar -x && \
	cd squashfs2.2-r2 && \
	patch -p1 < ../../Patches/mksquashfs_lzma.diff
	$(MAKE) -C @DIR_squashfs@/C/7zip/Compress/LZMA_Lib
	$(MAKE) -C @DIR_squashfs@/squashfs2.2-r2/squashfs-tools
	$(INSTALL) -m755 @DIR_squashfs@/squashfs2.2-r2/squashfs-tools/mksquashfs $(flashprefix)/
	@CLEANUP_squashfs@
	touch $@

#######################
#
#   contrib libs
#

if WITH_LIBCRYPTO
OPTIONAL_LIBCRYPTO = .libcrypto
endif

libs: \
	$(OPTIONAL_LIBCRYPTO) .libcurl .libdvbpsi .libffi .libfreetype .libjpeg \
	.libmad .libid3tag .libncurses .libpng .libreadline .libsdl .libsigc .libz .libvorbisidec

libs_optional: \
	.libcommoncplusplus .libdb2 .libnet .libnids .libpcap .libpopt .libungif \
	.libxml2 .libxmlparse .libxmlwrapp .libgif

.libboost: .bootstrap @DEPENDS_libboost@
	@PREPARE_libboost@
	cd @DIR_libboost@ && \
		@INSTALL_libboost@
	@CLEANUP_libboost@
	touch $@

.libcommoncplusplus: .bootstrap .libxml2 @DEPENDS_libcommoncplusplus@
	@PREPARE_libcommoncplusplus@
	cd @DIR_libcommoncplusplus@ &&\
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_libcommoncplusplus@
	@CLEANUP_libcommoncplusplus@
	touch $@

.libcrypto: .bootstrap @DEPENDS_libcrypto@
	@PREPARE_libcrypto@
	cd @DIR_libcrypto@ && \
		./Configure shared no-idea no-md2 no-md4 no-mdc2 no-rc2 no-rc5 tuxbox --prefix=/ --openssldir=/ && \
		$(MAKE) all && \
		@INSTALL_libcrypto@
	@CLEANUP_libcrypto@
	touch $@

.libcurl: .bootstrap @DEPENDS_libcurl@
	@PREPARE_libcurl@
	cd @DIR_libcurl@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--with-random && \
		$(MAKE) all && \
		rm -f $(hostprefix)/bin/curl-config && \
		sed -e "s,^prefix=,prefix=$(targetprefix)," < curl-config > $(hostprefix)/bin/curl-config && \
		chmod 755 $(hostprefix)/bin/curl-config && \
		@INSTALL_libcurl@
	@CLEANUP_libcurl@
	touch $@

.libdb2: .bootstrap @DEPENDS_libdb2@
	@PREPARE_libdb2@
	cd @DIR_libdb2@/dist && \
		chmod 755 configure && \
		autoconf && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--enable-compat185 \
			--enable-cxx && \
		$(MAKE) all && \
		@INSTALL_libdb2@
	@CLEANUP_libdb2@
	touch $@

.libdvbpsi: .bootstrap @DEPENDS_libdvbpsi@
	@PREPARE_libdvbpsi@
	cd @DIR_libdvbpsi@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= && \
		$(MAKE) all && \
		@INSTALL_libdvbpsi@
	@CLEANUP_libdvbpsi@
	touch $@

.libevent: .bootstrap @DEPENDS_libevent@
	@PREPARE_libevent@
	cd @DIR_libevent@ && \
		$(BUILDENV) \
		./configure \
			--prefix= \
			--build=$(build) \
			--host=$(target) \
			--disable-shared \
			--enable-static && \
		$(MAKE) all && \
		@INSTALL_libevent@
	@CLEANUP_libevent@
	touch $@

.libffi: .bootstrap @DEPENDS_libffi@
	@PREPARE_libffi@
	cd @DIR_libffi@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_libffi@
	@CLEANUP_libffi@
	touch $@
if FREETYPE_21
.libfreetype: .bootstrap @DEPENDS_libfreetype21@
	@PREPARE_libfreetype21@
	cd @DIR_libfreetype21@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= && \
		$(MAKE) all && \
		rm -f $(hostprefix)/bin/freetype-config && \
		sed -e "s,^prefix=,prefix=$(targetprefix)," < builds/unix/freetype-config > $(hostprefix)/bin/freetype-config && \
		chmod 755 $(hostprefix)/bin/freetype-config && \
		@INSTALL_libfreetype21@
	@CLEANUP_libfreetype21@
	touch $@
else
.libfreetype: .bootstrap @DEPENDS_libfreetype@
	@PREPARE_libfreetype@
	cd @DIR_libfreetype@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= && \
		$(MAKE) all && \
		rm -f $(hostprefix)/bin/freetype-config && \
		sed -e "s,^prefix=,prefix=$(targetprefix)," < builds/unix/freetype-config > $(hostprefix)/bin/freetype-config && \
		chmod 755 $(hostprefix)/bin/freetype-config && \
		@INSTALL_libfreetype@
	@CLEANUP_libfreetype@
	touch $@
endif
.libfribidi: .bootstrap @DEPENDS_libfribidi@
	@PREPARE_libfribidi@
	cd @DIR_libfribidi@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--enable-memopt && \
		$(MAKE) all && \
		@INSTALL_libfribidi@
if TARGETRULESET_FLASH
		cd $(hostprefix)/$(target)/lib && \
		ln -sf libfribidi.a libfribidi_pic.a
endif
	@CLEANUP_libfribidi@
	touch $@

.libgmp: .bootstrap @DEPENDS_libgmp@
	@PREPARE_libgmp@
	cd @DIR_libgmp@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_libgmp@
	@CLEANUP_libgmp@
	touch $@

.libjpeg: .bootstrap @DEPENDS_libjpeg@
	@PREPARE_libjpeg@
	cd @DIR_libjpeg@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--enable-static \
			--enable-shared && \
		$(MAKE) libjpeg.la && \
		@INSTALL_libjpeg@
if TARGETRULESET_FLASH
		cd $(hostprefix)/$(target)/lib && \
		ln -sf libjpeg.a libjpeg_pic.a
endif
	@CLEANUP_libjpeg@
	touch $@
.libdirectfb: .bootstrap .libfreetype .libjpeg .libpng .libz @DEPENDS_libdirectfb@
	@PREPARE_libdirectfb@
	cd @DIR_libdirectfb@ && \
	$(BUILDENV) \
	LDFLAGS=-L$(targetprefix)/lib \
	CPPFLAGS="-I$(buildprefix)/linux/arch/ppc" \
	CFLAGS="$(TARGET_CFLAGS) -I$(buildprefix)/linux/arch/ppc" \
	./autogen.sh \
	--build=$(build) \
	--host=$(target) \
	--prefix= \
	--disable-debug \
	--with-inputdrivers=linuxinput \
	--disable-sdl \
	--disable-multi \
	--without-tools \
	--with-gfxdrivers=none && \
	$(MAKE) all && \
	@INSTALL_libdirectfb@
	@CLEANUP_libdirectfb@
	mv $(targetprefix)/lib/libdirectfb.la $(targetprefix)/lib/libdirectfb.la.old
	mv $(targetprefix)/lib/libfusion.la $(targetprefix)/lib/libfusion.la.old
	sed -e "s, /lib, $(targetprefix)/lib,g" < $(targetprefix)/lib/libdirectfb.la.old >$(targetprefix)/lib/libdirectfb.la
	sed -e "s, /lib, $(targetprefix)/lib,g" < $(targetprefix)/lib/libfusion.la.old >$(targetprefix)/lib/libfusion.la
	rm -f $(targetprefix)/lib/libdirectfb.la.old
	rm -f $(targetprefix)/lib/libfusion.la.old
	touch $@

.libdirectfbpp: .bootstrap .libdirectfb @DEPENDS_libdirectfbpp@
	@PREPARE_libdirectfbpp@
	cd @DIR_libdirectfbpp@ && \
	$(BUILDENV) \
	./autogen.sh \
	--build=$(build) \
	--host=$(target) \
	--prefix= && \
	$(MAKE) all && \
	@INSTALL_libdirectfbpp@
	@CLEANUP_libdirectfbpp@
	mv $(targetprefix)/lib/libdfb++.la $(targetprefix)/lib/libdfb++.la.old
	sed -e "s, /lib, $(targetprefix)/lib,g" < $(targetprefix)/lib/libdfb++.la.old >$(targetprefix)/lib/libdfb++.la
	rm -f $(targetprefix)/lib/libdfb++.la.old
	touch $@

.libppdirectfb: .bootstrap .libdirectfb @DEPENDS_libppdirectfb@
	@PREPARE_libppdirectfb@
	cd @DIR_libppdirectfb@ && \
	$(BUILDENV) \
	./autogen.sh \
	--build=$(build) \
	--host=$(target) \
	--prefix= && \
	$(MAKE) all && \
	@INSTALL_libppdirectfb@
	@CLEANUP_libppdirectfb@
	mv $(targetprefix)/lib/lib++dfb.la $(targetprefix)/lib/lib++dfb.la.old
	sed -e "s, /lib, $(targetprefix)/lib,g" < $(targetprefix)/lib/lib++dfb.la.old >$(targetprefix)/lib/lib++dfb.la
	rm -f $(targetprefix)/lib/lib++dfb.la.old
	touch $@

.libmad: .bootstrap .libz @DEPENDS_libmad@
	@PREPARE_libmad@
	cd @DIR_libmad@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--enable-shared=yes \
			--disable-nls \
			--without-esd \
			--enable-fpm=ppc \
			--enable-sso \
			--enable-accuracy && \
		$(MAKE) all && \
		@INSTALL_libmad@
	@CLEANUP_libmad@
	touch $@

.libid3tag: .bootstrap .libz @DEPENDS_libid3tag@
	@PREPARE_libid3tag@
	cd @DIR_libid3tag@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--enable-shared=yes && \
		$(MAKE) all && \
		@INSTALL_libid3tag@
	@CLEANUP_libid3tag@
	touch $@

.libncurses: .bootstrap @DEPENDS_libncurses@
	@PREPARE_libncurses@
	cd @DIR_libncurses@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--with-terminfo-dirs=/share/terminfo \
			--disable-big-core \
			--without-debug \
			--without-progs \
			--without-ada \
			--with-shared \
			--without-profile \
			--disable-rpath \
			--without-cxx-binding \
			--with-fallbacks='linux vt100 xterm' && \
		$(MAKE) libs \
			HOSTCC=$(CC) \
			HOSTCCFLAGS="$(CFLAGS) -DHAVE_CONFIG_H -I../ncurses -DNDEBUG -D_GNU_SOURCE -I../include" \
			HOSTLDFLAGS="$(LDFLAGS)" && \
		@INSTALL_libncurses@
	@CLEANUP_libncurses@
	touch $@

.libnet: .bootstrap @DEPENDS_libnet@
	@PREPARE_libnet@
	cd @DIR_libnet@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--with-pf_packet=yes \
			--with-fast_x86_check=no && \
		$(MAKE) all && \
		@INSTALL_libnet@
	@CLEANUP_libnet@
	touch $@

.libnids: .bootstrap .libnet @DEPENDS_libnids@
	@PREPARE_libnids@
	cd @DIR_libnids@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_libnids@
	@CLEANUP_libnids@
	touch $@

.libpcap: .bootstrap @DEPENDS_libpcap@
	@PREPARE_libpcap@
	cd @DIR_libpcap@ && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--with-pcap=linux \
			--build=$(build) && \
		$(MAKE) all && \
		@INSTALL_libpcap@
	@CLEANUP_libpcap@
	touch $@

.libpng: .bootstrap .libz @DEPENDS_libpng@
	@PREPARE_libpng@
	cd @DIR_libpng@ && \
		$(MAKE) libpng.a libpng12.so libpng.pc libpng-config \
			$(BUILDENV) \
			CPPFLAGS="-DPNG_DEBUG=0" \
			prefix=$(targetprefix) && \
		@INSTALL_libpng@
	@CLEANUP_libpng@
	touch $@

.libpopt: .bootstrap @DEPENDS_libpopt@
	@PREPARE_libpopt@
	cd @DIR_libpopt@ && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--prefix=$(targetprefix)\
			--with-pic \
			--disable-nls && \
		$(MAKE) all && \
		@INSTALL_libpopt@
	@CLEANUP_libpopt@
	touch $@

.libreadline: .bootstrap @DEPENDS_libreadline@
	@PREPARE_libreadline@
	cd @DIR_libreadline@ && \
		autoconf && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= && \
		$(MAKE) all && \
		@INSTALL_libreadline@
	@CLEANUP_libreadline@
	touch $@

.libsdl: .bootstrap @DEPENDS_libsdl@
	@PREPARE_libsdl@
	cd @DIR_libsdl@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--disable-alsa \
			--disable-openbsdaudio \
			--disable-esd \
			--disable-arts \
			--disable-nas \
			--disable-diskaudio \
			--enable-nasm \
			--disable-nanox \
			--disable-video-x11 \
			--without-x \
			--enable-video-fbcon \
			--disable-video-photon \
			--disable-video-directfb \
			--disable-video-ps2gs \
			--disable-video-ggi \
			--disable-video-svga \
			--disable-video-vgl \
			--disable-video-aalib \
			--disable-video-dummy \
			--disable-video-opengl \
			--disable-stdio-redirect \
			--enable-joystick \
			--disable-directx && \
		$(MAKE) all && \
		rm -f $(hostprefix)/bin/sdl-config && \
		sed -e "s,^prefix=,prefix=$(targetprefix)," < sdl-config > $(hostprefix)/bin/sdl-config && \
		chmod 755 $(hostprefix)/bin/sdl-config && \
		@INSTALL_libsdl@
if TARGETRULESET_FLASH
		cd $(hostprefix)/$(target)/lib && \
		ln -sf libSDL.a libSDL_pic.a
endif
	@CLEANUP_libsdl@
	touch $@

.libsdl_image: .bootstrap .libsdl @DEPENDS_libsdl_image@
	@PREPARE_libsdl_image@
	cd @DIR_libsdl_image@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--disable-sdltest \
			--with-sdl-prefix=$(hostprefix)/lib \
			--with-sdl-exec-prefix=$(hostprefix) && \
		$(MAKE) && \
		@INSTALL_libsdl_image@
if TARGETRULESET_FLASH
		cd $(hostprefix)/$(target)/lib && \
		ln -sf libSDL_image.a libSDL_image_pic.a
endif
	@CLEANUP_libsdl_image@
	touch $@

.libsdl_mixer: .bootstrap .libsdl @DEPENDS_libsdl_mixer@
	@PREPARE_libsdl_mixer@
	cd @DIR_libsdl_mixer@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--disable-sdltest \
			--with-sdl-prefix=$(hostprefix)/lib \
			--with-sdl-exec-prefix=$(hostprefix) && \
		$(MAKE) && \
		@INSTALL_libsdl_mixer@
if TARGETRULESET_FLASH
		cd $(hostprefix)/$(target)/lib && \
		ln -sf libSDL_mixer.a libSDL_mixer_pic.a
endif
	@CLEANUP_libsdl_mixer@
	touch $@

.libsdl_sound: .bootstrap .libsdl @DEPENDS_libsdl_sound@
	@PREPARE_libsdl_sound@
	cd @DIR_libsdl_sound@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--disable-sdltest \
			--with-sdl-prefix=$(hostprefix)/lib \
			--with-sdl-exec-prefix=$(hostprefix) \
			--disable-ogg \
			--with-vorbis-prefix=$(hostprefix) && \
		$(MAKE) && \
		@INSTALL_libsdl_sound@
if TARGETRULESET_FLASH
		cd $(hostprefix)/$(target)/lib && \
		ln -sf libSDL_sound.a libSDL_sound_pic.a
endif
	@CLEANUP_libsdl_sound@
	touch $@

.libsdl_net: .bootstrap .libsdl @DEPENDS_libsdl_net@
	@PREPARE_libsdl_net@
	cd @DIR_libsdl_net@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--disable-sdltest \
			--with-sdl-prefix=$(hostprefix)/lib \
			--with-sdl-exec-prefix=$(hostprefix) && \
		$(MAKE) && \
		@INSTALL_libsdl_net@
if TARGETRULESET_FLASH
		cd $(hostprefix)/$(target)/lib && \
		ln -sf libSDL_net.a libSDL_net_pic.a
endif
	@CLEANUP_libsdl_net@
	touch $@

.libsigc: .bootstrap @DEPENDS_libsigc@
	@PREPARE_libsigc@
	cd @DIR_libsigc@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--disable-checks && \
		$(MAKE) all && \
		@INSTALL_libsigc@
	@CLEANUP_libsigc@
	touch $@

.libvorbisidec: .bootstrap @DEPENDS_libvorbisidec@
	@PREPARE_libvorbisidec@
	cd @DIR_libvorbisidec@ && \
		$(BUILDENV) \
		./autogen.sh \
			--build=$(build) \
			--host=$(target) \
			--prefix= && \
		$(MAKE) && \
		@INSTALL_libvorbisidec@
	@CLEANUP_libvorbisidec@
	touch $@

.libungif: .bootstrap @DEPENDS_libungif@
	@PREPARE_libungif@
	cd @DIR_libungif@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--without-x && \
		$(MAKE) && \
		@INSTALL_libungif@
if TARGETRULESET_FLASH
		cd $(hostprefix)/$(target)/lib && \
		ln -sf libungif.a libungif_pic.a
endif
	@CLEANUP_libungif@
	touch $@

.libupnp: .bootstrap @DEPENDS_libupnp@
	@PREPARE_libupnp@
	cd @DIR_libupnp@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--without-documentation && \
		$(MAKE) && \
		@INSTALL_libupnp@
	@CLEANUP_libupnp@
	touch $@

.libxml2: .bootstrap @DEPENDS_libxml2@
	@PREPARE_libxml2@
	cd @DIR_libxml2@ && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--without-html-dir \
			--with-threads \
			--without-ftp \
			--without-http \
			--without-html \
			--without-catalog \
			--without-docbook \
			--with-xpath \
			--without-xptr \
			--without-xinclude \
			--without-iconv \
			--without-debug \
			--without-mem-debug \
			--without-python && \
		$(MAKE) all && \
		@INSTALL_libxml2@
	@CLEANUP_libxml2@
	touch $@

.libxmlparse: .bootstrap @DEPENDS_libxmlparse@
	@PREPARE_libxmlparse@
	cd @DIR_libxmlparse@ && \
		aclocal -I . && \
		libtoolize --automake --force && \
		autoconf && \
		automake --add-missing --force-missing && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--enable-ambiguous \
			--enable-libstdcpp3 && \
		$(MAKE) all && \
		@INSTALL_libxmlparse@
	@CLEANUP_libxmlparse@
	touch $@

.libxmlwrapp: .bootstrap .libxml2 @DEPENDS_libxmlwrapp@
	@PREPARE_libxmlwrapp@
	cd @DIR_libxmlwrapp@ && \
		$(BUILDENV) \
		./configure.pl \
			--disable-examples \
			--prefix $(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_libxmlwrapp@
	@CLEANUP_libxmlwrapp@
	touch $@

.libz: .bootstrap @DEPENDS_libz@
	@PREPARE_libz@
	cd @DIR_libz@ && \
		$(BUILDENV) \
		./configure \
			--prefix= \
			--shared && \
		$(MAKE) all && \
		@INSTALL_libz@
	@CLEANUP_libz@
	touch $@

#######################
#
#	Database
#

if ENABLE_SQLITE
.sqlite: .bootstrap @DEPENDS_sqlite@
	@PREPARE_sqlite@
	cd @DIR_sqlite@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_sqlite@
	@CLEANUP_sqlite@
	touch $@
endif
#######################
#
#   contrib apps
#

contrib_apps: .dropbear .console_data .console_tools .fbset .lsof .ssh .diffutils .less .tcpdump .lirc .xrc
contrib_apps_optional: .dsniff .pump .thttpd

.console_data: .bootstrap @DEPENDS_console_data@
	@PREPARE_console_data@
	cd @DIR_console_data@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--with-main_compressor=gzip && \
		@INSTALL_console_data@
	@CLEANUP_console_data@
	touch $@

.console_tools: .bootstrap .console_data @DEPENDS_console_tools@
	@PREPARE_console_tools@
	cd @DIR_console_tools@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--disable-nls && \
		@INSTALL_console_tools@
	@CLEANUP_console_tools@
	touch $@

.dsniff: .bootstrap .libnet .libnids .libdb2 .rpcgen @DEPENDS_dsniff@
	@PREPARE_dsniff@
	cd @DIR_dsniff@ && \
		CC=$(target)-gcc \
		RANLIB=$(target)-ranlib \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--without-x && \
		$(MAKE) all && \
		@INSTALL_dsniff@
	@CLEANUP_dsniff@
	touch $@

.fbset: .bootstrap @DEPENDS_fbset@
	@PREPARE_fbset@
	cd @DIR_fbset@ && \
		$(MAKE) \
			$(BUILDENV) && \
		@INSTALL_fbset@
	@CLEANUP_fbset@
	touch $@

.lirc: .bootstrap @DEPENDS_lirc@
	@PREPARE_lirc@
	cd @DIR_lirc@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		mknod=/bin/true \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--with-devdir=/dev \
			--with-driver=serial \
			--with-kerneldir=$(buildprefix)/linux-$(KERNELVERSION) \
			--with-moduledir=$(targetprefix)/lib/modules/$(KERNELVERSION)/misc \
			--without-x && \
		@INSTALL_lirc@
	@CLEANUP_lirc@
	touch $@

.polipo: .bootstrap @DEPENDS_polipo@
	@PREPARE_polipo@
	cd @DIR_polipo@ && \
		$(MAKE) polipo \
			CC=$(target)-gcc \
			CDEBUGFLAGS="$(TARGET_CFLAGS)" \
			LDFLAGS="$(TARGET_LDFLAGS)" \
			PREFIX="" && \
		@INSTALL_polipo@
		$(INSTALL) -d $(targetprefix)/var/etc/polipo
		$(INSTALL) -m644 @DIR_polipo@/config.dbox2 $(targetprefix)/var/etc/polipo/config
		$(INSTALL) -m644 @DIR_polipo@/forbidden.dbox2 $(targetprefix)/var/etc/polipo/forbidden
		$(INSTALL) -d $(targetprefix)/share/polipo/www
		$(INSTALL) -m644 @DIR_polipo@/localindex.html $(targetprefix)/share/polipo/www/index.html
		@ln -sf /var/etc/polipo $(targetprefix)/etc/polipo
	@CLEANUP_polipo@
	touch $@

.utillinux: .bootstrap @DEPENDS_utillinux@
	@PREPARE_utillinux@
	cd @DIR_utillinux@ && \
		CC=$(target)-gcc \
		CFLAGS="-Os -msoft-float" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure && \
		$(MAKE) ARCH=ppc all && \
		@INSTALL_utillinux@
	@CLEANUP_utillinux@
	touch $@
.automount: .bootstrap @DEPENDS_automount@
	@PREPARE_automount@
	cd @DIR_automount@  && \
		$(BUILDENV) STRIP=$(target)-strip \
		$(MAKE) && \
		$(MAKE) install INSTALLROOT=$(targetprefix)
	rm -rf @DIR_automount@
	$(INSTALL) $(buildprefix)/root/etc/init.d/start_automount $(targetprefix)/etc/init.d
	ln -sf /proc/mounts $(targetprefix)/etc/mtab
	@touch $@

.nfs_utils: .bootstrap @DEPENDS_nfs_utils@
	@PREPARE_nfs_utils@
	cd @DIR_nfs_utils@ && \
		CC=$(target)-gcc \
		CFLAGS="-Os -msoft-float" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure && \
		$(MAKE) ARCH=ppc all && \
		$(FAKEROOT) $(MAKE) install_prefix=$(targetprefix) install && \
		cd .. && \
	@CLEANUP_nfs_utils@
	touch $@

.samba: .bootstrap @DEPENDS_samba@
	@PREPARE_samba@
	cd @DIR_samba@ && \
		cd source && \
		(for i in smbd nmbd smbmount smbmnt; do \
			$(MAKE) $$i; \
			cp $$i $(targetprefix)/bin; \
		done)
	@CLEANUP_samba@
	touch $@

.etherwake: .bootstrap @DEPENDS_etherwake@
	@PREPARE_etherwake@
	cd @DIR_etherwake@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) && \
		@INSTALL_etherwake@
	@CLEANUP_etherwake@
	touch $@

.reiserfsprogs: .bootstrap @DEPENDS_reiserfsprogs@
	@PREPARE_reiserfsprogs@
	cd @DIR_reiserfsprogs@ && \
		CC=$(target)-gcc \
		RANLIB=$(target)-ranlib \
		CFLAGS="-Os -msoft-float" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--target=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) && \
		@INSTALL_reiserfsprogs@
	@CLEANUP_reiserfsprogs@
	touch $@

.e2fsprogs: .bootstrap @DEPENDS_e2fsprogs@
	@PREPARE_e2fsprogs@
	cd @DIR_e2fsprogs@ && \
		CC=$(target)-gcc \
		RANLIB=$(target)-ranlib \
		CFLAGS="-Os -msoft-float" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--target=$(target) \
			--prefix=$(targetprefix) \
			--with-cc=$(target)-gcc \
			--with-linker=$(target)-ld \
			--disable-evms \
			--enable-elf-shlibs \
			--enable-htree \
			--disable-profile \
			--disable-swapfs \
			--disable-debugfs \
			--disable-image \
			--disable-resizer \
			--enable-dynamic-e2fsck \
			--enable-fsck \
			--with-gnu-ld \
			--disable-nls && \
		$(MAKE) libs progs && \
		@INSTALL_e2fsprogs@
	@CLEANUP_e2fsprogs@
	touch $@

.lsof: .bootstrap @DEPENDS_lsof@
	@PREPARE_lsof@
	cd @DIR_lsof@ && \
		CROSS_COMPILE=$(target)- \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		LSOF_VSTR=$(KERNELVERSION) \
		LINUX_CLIB="-DGLIBCV=202" \
		./Configure -n linux && \
		$(MAKE) all && \
		@INSTALL_lsof@
	@CLEANUP_lsof@
	touch $@

.pump: .bootstrap .libpopt @DEPENDS_pump@
	@PREPARE_pump@
	cd @DIR_pump@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) all && \
		@INSTALL_pump@
	@CLEANUP_pump@
	touch $@

.ssh: .bootstrap .libcrypto .libz @DEPENDS_ssh@
	@PREPARE_ssh@
	cd @DIR_ssh@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--sysconfdir=/etc/ssh \
			--without-shadow \
			--with-4in6 \
			--disable-suid-ssh \
			--with-path="/bin:/sbin" \
			--with-privsep-user=sshd \
			--with-privsep-path=/share/empty && \
		$(MAKE) all && \
		@INSTALL_ssh@
	@CLEANUP_ssh@
	touch $@

.tcpdump: .bootstrap .libpcap @DEPENDS_tcpdump@
	@PREPARE_tcpdump@
	cd @DIR_tcpdump@ && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--libdir=$(targetprefix)/lib \
			--build=$(build) && \
		$(MAKE) && \
		@INSTALL_tcpdump@
	@CLEANUP_tcpdump@
	touch $@

.tor: .bootstrap .libevent .libcrypto @DEPENDS_tor@
	@PREPARE_tor@
	cd @DIR_tor@ && \
		$(BUILDENV) \
		CROSS_COMPILE=$(target)- \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--sysconfdir=/var/etc && \
		$(MAKE) && \
		@INSTALL_tor@
	@CLEANUP_tor@
	touch $@

.thttpd: @DEPENDS_thttpd@
	@PREPARE_thttpd@
	cd @DIR_thttpd@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_thttpd@
	@CLEANUP_thttpd@
	touch $@

.ushare: .bootstrap .libupnp @DEPENDS_ushare@
	@PREPARE_ushare@
	cd @DIR_ushare@ && \
		$(BUILDENV) \
		./configure \
			--prefix= \
			--disable-dlna \
			--disable-nls \
			--disable-strip \
			--disable-optimize \
			--cross-compile && \
		$(MAKE) && \
		@INSTALL_ushare@
		mv $(targetprefix)/etc/ushare.conf $(targetprefix)/etc/ushare.conf.orig
	@CLEANUP_ushare@
	touch $@

.xrc: .bootstrap .lirc @DEPENDS_xrc@
	@PREPARE_xrc@
	cd @DIR_xrc@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--disable-qpe \
			--disable-qt-embedded \
			--without-x && \
		@INSTALL_xrc@
	@CLEANUP_xrc@
	touch $@

.less: .bootstrap .libncurses @DEPENDS_less@
	@PREPARE_less@
	cd @DIR_less@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_less@
	@CLEANUP_less@
	touch $@

.diffutils: .bootstrap @DEPENDS_diffutils@
	@PREPARE_diffutils@
	cd @DIR_diffutils@ && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_diffutils@
	@CLEANUP_diffutils@
	touch $@

.dropbear: .bootstrap .libz @DEPENDS_dropbear@
	@PREPARE_dropbear@
	cd @DIR_dropbear@ && \
		$(BUILDENV) \
		./configure \
			--disable-syslog \
			--disable-lastlog \
			--disable-shadow \
			--prefix= \
			--build=$(build) \
			--host=$(target) \
			--disable-utmp \
			--disable-utmpx \
			--disable-wtmp \
			--disable-wtmpx && \
		cp ../Patches/dropbear-options.h options.h && \
		$(MAKE) PROGRAMS="dropbear dropbearkey scp" MULTI=1 && \
		mkdir -p $(targetprefix)/var_init/etc/dropbear && \
		mkdir -p $(targetprefix)/var_init/.ssh && \
		@INSTALL_dropbear@ && \
		$(target)-strip --strip-all $(targetprefix)/sbin/dropbearmulti
	@CLEANUP_dropbear@
	touch $@

.ipkg: .bootstrap @DEPENDS_ipkg@
	@PREPARE_ipkg@
	cd @DIR_ipkg@ && \
		$(BUILDENV) \
		./configure \
			crosscompiling=yes \
			--build=$(build) \
			--host=$(target) \
			--with-ipkglibdir=/var/ && \
		$(MAKE) all && \
		$(INSTALL) -d $(targetprefix)/bin && \
		mv .libs/ipkg-cl .libs/ipkg && \
		ln -sf /var/etc/ipkg.conf $(targetprefix)/etc/ipkg.conf && \
		$(INSTALL) .libs/ipkg $(targetprefix)/bin && \
		$(INSTALL) .libs/libipkg.so.0 $(targetprefix)/lib
	@CLEANUP_ipkg@
	touch $@

#######################
#
#   development tools
#

devel: .gdb .ltrace .strace .nano .joe
devel_optional: .gdb-remote .insight .ksymoops

.gdb: .bootstrap .libncurses @DEPENDS_gdb@
	@PREPARE_gdb@
	cd @DIR_gdb@ && \
		$(BUILDENV) \
		LD_LIBRARY_PATH=$(libdir) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--nfp \
			--disable-tui \
			--without-tui \
			--disable-sim \
			--without-sim \
			--prefix= && \
		$(MAKE) all-gdb && \
		@INSTALL_gdb@
	@CLEANUP_gdb@
	touch $@

.gdb-remote: @DEPENDS_gdb@
	@PREPARE_gdb@
	cd @DIR_gdb@ && \
		./configure \
			--build=$(build) \
			--host=$(build) \
			--target=$(target) \
			--prefix=$(hostprefix) && \
		$(MAKE) all-gdb && \
		$(MAKE) install-gdb
	@CLEANUP_gdb@
	touch $@

.ksymoops: .bootstrap @DEPENDS_ksymoops@
	@PREPARE_ksymoops@
	cd @DIR_ksymoops@ && \
		$(MAKE) CC=$(target)-gcc DEF_MAP="\\\"/boot/System.map-*r\\\"" DEF_VMLINUX="\\\"/boot/vmlinux-*r\\\"" &&\
		@INSTALL_ksymoops@
	@CLEANUP_ksymoops@
	touch $@

.insight: @DEPENDS_insight@
	@PREPARE_insight@
	cd @DIR_insight@ && \
		./configure \
			--build=$(build) \
			--host=$(build) \
			--target=$(target) \
			--prefix=$(hostprefix) && \
		$(MAKE) all-gdb && \
		$(MAKE) install-gdb
	@CLEANUP_insight@
	touch $@

.ltrace: .bootstrap @DEPENDS_ltrace@
	@PREPARE_ltrace@
	cd @DIR_ltrace@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--target=$(target) \
			--prefix= && \
		$(MAKE) clean all LD=$(target)-ld && \
		@INSTALL_ltrace@
	@CLEANUP_ltrace@
	touch $@

.strace: .bootstrap @DEPENDS_strace@
	@PREPARE_strace@
	cd @DIR_strace@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--target=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_strace@
	@CLEANUP_strace@
	touch $@

.nano: .bootstrap .libncurses @DEPENDS_nano@
	@PREPARE_nano@
	cd @DIR_nano@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=  \
			--includedir=$(targetprefix)/include && \
		$(MAKE) all && \
		@INSTALL_nano@
	@CLEANUP_nano@
	touch $@
.joe: .bootstrap .libncurses @DEPENDS_joe@ 
	@PREPARE_joe@
	cd @DIR_joe@ && \
		$(BUILDENV) \
		./configure \
		--build=$(build) \
		--host=$(target) \
		--prefix=  \
		--includedir=$(targetprefix)/include && \
		$(MAKE) all && \
		@INSTALL_joe@
		@CLEANUP_joe@
		touch $@
																								
#######################
#
# java stuff
#

java: .kaffeh .kaffe

# for x86
.kaffeh: .bootstrap @DEPENDS_kaffeh@
	@PREPARE_kaffeh@
	cd @DIR_kaffeh@ && \
		./configure \
			--prefix=$(hostprefix) \
			--disable-dependency-tracking \
			--without-x \
			--without-suncompat \
			--disable-gcj && \
		$(MAKE) all && \
		@INSTALL_kaffeh@
	@CLEANUP_kaffeh@
	touch $@

# for ppc
.kaffe: .bootstrap .kaffeh .libffi @DEPENDS_kaffe@
	@PREPARE_kaffe@
	cd @DIR_kaffe@ && \
		CC=$(target)-gcc CXX=$(target)-g++ \
		CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--host=$(target) \
			--prefix=$(targetprefix) \
			--libexecdir=$(targetprefix)/bin \
			--enable-feedback \
			--with-stats \
			--disable-dependency-tracking \
			--without-x \
			--without-suncompat \
			--disable-gcj && \
		$(MAKE) all && \
		@INSTALL_kaffe@
	@CLEANUP_kaffe@
	touch $@

#######################
#
#   fun stuff
#

fun: .gnuboy .scummvm .sdldoom

.gnuboy: .bootstrap @DEPENDS_gnuboy@
	@PREPARE_gnuboy@
	cd @DIR_gnuboy@ && \
		autoconf && \
		CC=$(target)-gcc \
		CFLAGS="$(TARGET_CFLAGS) -I$(driverdir)/include" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix=$(targetprefix) && \
		$(MAKE) all && \
		@INSTALL_gnuboy@
	@CLEANUP_gnuboy@
	touch $@

.scummvm: .bootstrap .libreadline .libsdl @DEPENDS_scummvm@
	@PREPARE_scummvm@
	cd @DIR_scummvm@ && \
		$(BUILDENV) \
		./configure \
		    --host=$(target) \
		    --disable-debug \
		    --disable-zlib \
		    --disable-mt32emu \
		    --disable-tremor \
		    --prefix=$(targetprefix) \
		    --with-sdl-prefix=$(hostprefix) && \
		$(MAKE) \
			AR="$(target)-ar cru" \
			LD=$(target)-ld \
			CC=$(target)-gcc \
			CXX=$(target)-g++ \
			CFLAGS="$(TARGET_CFLAGS)" \
			LDFLAGS="$(TARGET_LDFLAGS)" && \
		@INSTALL_scummvm@
	@CLEANUP_scummvm@
	touch $@

.sdldoom: .bootstrap .libsdl @DEPENDS_sdldoom@
	@PREPARE_sdldoom@
	cd @DIR_sdldoom@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= && \
		$(MAKE) all && \
		@INSTALL_sdldoom@
	@CLEANUP_sdldoom@
	touch $@

.wxbase: .bootstrap .libsdl @DEPENDS_wxbase@
	@PREPARE_wxbase@
	cd @DIR_wxbase@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--with-regex=builtin \
			--with-expat=builtin \
			--with-sdl=no \
			--with-sdl-prefix=$(hostprefix)/lib \
			--with-sdl-exec-prefix=$(hostprefix) \
			--disable-shared \
			--disable-gui \
			--disable-compat24 \
			--enable-unicode \
			--enable-largefile \
			--disable-gtktest \
			--disable-sdltest && \
		$(MAKE) && \
		@INSTALL_wxbase@
	@CLEANUP_wxbase@
	touch $@

.amule: .bootstrap .wxbase .libpng @DEPENDS_amule@
	@PREPARE_amule@
	cd @DIR_amule@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--enable-amulecmd \
			--enable-webserver \
			--enable-amule-daemon \
			--with-libpng-prefix=$(targetprefix) \
			--with-wx-prefix=$(targetprefix) \
			--disable-debug \
			--disable-monolithic \
			--disable-amule-gui \
			--disable-cas \
			--disable-wxcas \
			--disable-alc \
			--disable-alcc \
			--enable-embedded-crypto \
			--without-gdlib-config \
			--without-x \
			--disable-nls && \
		$(MAKE) \
			HOSTCC=$(CC) &&\
		@INSTALL_amule@
		$(INSTALL) @DIR_amule@/amulestart $(targetprefix)/bin/
		$(INSTALL) @DIR_amule@/amule.install $(targetprefix)/bin/
	@CLEANUP_amule@
	touch $@

.ctorrent: .bootstrap @DEPENDS_ctorrent@
	@PREPARE_ctorrent@
	cd @DIR_ctorrent@ && \
		$(BUILDENV) \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= && \
		$(MAKE) all && \
		@INSTALL_ctorrent@
	@CLEANUP_ctorrent@
	touch $@

#######################
#
#  DVB apps
#

dvb_apps: .dvbdate .dvbstream .dvbtext .dvbtune .vls

.dvbdate: .bootstrap @DEPENDS_dvbdate@
	@PREPARE_dvbdate@
	cd @DIR_dvbdate@ && \
		$(MAKE) dvbdate \
			CC=$(target)-gcc \
			CFLAGS="$(TARGET_CFLAGS) -I$(driverdir)/include" \
			LDFLAGS="$(TARGET_LDFLAGS)" && \
		@INSTALL_dvbdate@
	@CLEANUP_dvbdate@
	touch $@

.dvbstream: .bootstrap @DEPENDS_dvbstream@
	@PREPARE_dvbstream@
	cd @DIR_dvbstream@ && \
		$(MAKE) dvbstream rtpfeed\
			INCS="-I$(driverdir)/include" \
			CC=$(target)-gcc \
			CFLAGS="$(TARGET_CFLAGS) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE" && \
		@INSTALL_dvbstream@
	@CLEANUP_dvbstream@
	touch $@

.dvbtext: .bootstrap @DEPENDS_dvbtext@
	@PREPARE_dvbtext@
	cd @DIR_dvbtext@ && \
		$(target)-gcc $(TARGET_CFLAGS) -I$(driverdir)/include -o dvbtext dvbtext.c && \
		@INSTALL_dvbtext@
	@CLEANUP_dvbtext@
	touch $@

.dvbtune: .bootstrap @DEPENDS_dvbtune@
	@PREPARE_dvbtune@
	cd @DIR_dvbtune@ && \
		$(MAKE) \
			CC=$(target)-gcc \
			CFLAGS="$(TARGET_CFLAGS) -I$(driverdir)/include" \
			LDFLAGS="$(TARGET_LDFLAGS)" && \
		@INSTALL_dvbtune@
	@CLEANUP_dvbtune@
	touch $@

.vls: .bootstrap .libdvbpsi @DEPENDS_vls@
	@PREPARE_vls@
	cd @DIR_vls@ && \
		CC=$(target)-gcc \
		CXX=$(target)-g++ \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--build=$(build) \
			--host=$(target) \
			--prefix= \
			--disable-dvd \
			--with-dvb=$(driverdir)/include && \
		$(MAKE) all && \
		@INSTALL_vls@
	@CLEANUP_vls@
	touch $@

#######################
#
#   own kernel
#

boot: .linuxkernel

.linuxkernel: .bootstrap .linuxdir
	$(MAKE) -C @DIR_linux@ zImage modules \
		ARCH=ppc \
		CROSS_COMPILE=$(target)-
	$(MAKE) -C @DIR_linux@ modules_install \
		ARCH=ppc \
		CROSS_COMPILE=$(target)- \
		DEPMOD=/bin/true \
		INSTALL_MOD_PATH=$(targetprefix)

if MAINTAINER_MODE
else
	$(MAKE) -C @DIR_linux@ clean
endif
	touch $@

#######################
#
#   own applications
#

apps: .dvbsnoop .enigma .lcars .lcdmenu .neutrino .plugins .stream

CONFIGURE_OPTS = \
	--build=$(build) \
	--host=$(target) \
	--prefix=$(targetprefix) \
	--with-driver=$(driverdir) \
	--with-dvbincludes=$(driverdir)/include \
	--with-target=cdk \
	--with-boxtype=$(BOXTYPE) \
	--with-webif=$(WEBIF) \
	--with-epg=$(EPG) \
	--with-flashtool=$(FLASHTOOL) \
	--with-reiserfs=$(REISERFS) \
	--with-ext-flashtool=$(EXTFLASHTOOL) \
	--with-enigma-debug=$(ENIGMA_DEBUG) \
	--with-mhw-epg=$(MHW_EPG)

if MAINTAINER_MODE
CONFIGURE_OPTS_MAINTAINER = \
	--enable-maintainer-mode
endif
if ENABLE_FREESATEPG
CONFIGURE_OPTS_EPG = \
	--enable-freesatepg
endif
if ENABLE_IPKG
CONFIGURE_OPTS_IPKG = \
	--enable-ipkg
endif
if ENABLE_SQLITE
CONFIGURE_OPTS_SQLITE = \
	--enable-sqlite
endif
if TARGETRULESET_FLASH
CONFIGURE_OPTS_DEBUG = \
	--without-debug
endif

CONFIGURE = \
	./autogen.sh && \
	CC=$(target)-gcc \
	CXX=$(target)-g++ \
	CFLAGS="-Wall $(TARGET_CFLAGS)" \
	CXXFLAGS="-Wall $(TARGET_CXXFLAGS)" \
	./configure $(CONFIGURE_OPTS) $(CONFIGURE_OPTS_MAINTAINER) $(CONFIGURE_OPTS_DEBUG) $(CONFIGURE_OPTS_EPG) $(CONFIGURE_OPTS_SQLITE) $(CONFIGURE_OPTS_IPKG)

# dvb/svbsnoop

$(appsdir)/dvb/dvbsnoop/Makefile: .bootstrap
	cd $(appsdir)/dvb/dvbsnoop && $(CONFIGURE) CPPFLAGS="$(CPPFLAGS) -I$(driverdir)/include"

.dvbsnoop: $(appsdir)/dvb/dvbsnoop/Makefile
	$(MAKE) -C $(appsdir)/dvb/dvbsnoop all install
	touch $@

$(appsdir)/dvb/zapit/Makefile: .bootstrap .misc_libs
	cd $(appsdir)/dvb/zapit && $(CONFIGURE) --with-neutrino-source=$(appsdir)/tuxbox/neutrino

.zapit: $(appsdir)/dvb/zapit/Makefile
	$(MAKE) -C $(appsdir)/dvb/zapit all install
	touch $@

# dvb/tools

$(appsdir)/dvb/tools/Makefile: .bootstrap .misc_libs
	cd $(appsdir)/dvb/tools && $(CONFIGURE) --with-boxtype=dreambox

.stream: $(appsdir)/dvb/tools/Makefile .busybox
	$(MAKE) -C $(appsdir)/dvb/tools/stream all install
	touch $@

# misc/libs

$(appsdir)/misc/libs/Makefile: .bootstrap .libz
	cd $(appsdir)/misc/libs && $(CONFIGURE)

.misc_libs: $(appsdir)/misc/libs/Makefile
	$(MAKE) -C $(appsdir)/misc/libs all install
if TARGETRULESET_FLASH
	cd $(hostprefix)/$(target)/lib && \
	ln -sf libxmltree.a libxmltree_pic.a && \
	ln -sf libmd5sum.a libmd5sum_pic.a && \
	ln -sf libmpegtools.a libmpegtools_pic.a
endif
	touch $@

# misc/tools

$(appsdir)/misc/tools/config.status: .bootstrap
	cd $(appsdir)/misc/tools && $(CONFIGURE)

.misc_tools: $(appsdir)/misc/tools/config.status
	$(MAKE) -C $(appsdir)/misc/tools all install CFLAGS=-I$(appsdir)/../cdk/linux/include
	touch $@

# tuxbox/enigma

$(appsdir)/tuxbox/enigma/Makefile: .bootstrap .libfreetype .libfribidi .libmad .libid3tag .libpng .libsigc .libtuxbox .misc_libs .plugins .libvorbisidec
	cd $(appsdir)/tuxbox/enigma && $(CONFIGURE)

if WEBIF_EXPERT
OPTIONAL_LIBCURL = .libcurl
endif

.enigma: $(appsdir)/tuxbox/enigma/Makefile .tuxbox_tools .libungif .libjpeg $(OPTIONAL_LIBCURL)
	$(MAKE) -C $(appsdir)/tuxbox/enigma all install
	touch $@

# tuxbox/lcars

$(appsdir)/tuxbox/lcars/Makefile: .bootstrap .linuxkernel .libcurl .libtuxbox .libfreetype .misc_libs .tuxbox_libs .tuxbox_tools .plugins .automount core
	cd $(appsdir)/tuxbox/lcars && $(CONFIGURE)

.lcars: $(appsdir)/tuxbox/lcars/Makefile
	$(MAKE) -C $(appsdir)/tuxbox/lcars all install
	touch $@

# lcd-savers

$(appsdir)/tuxbox/funstuff/lcd-savers/Makefile: .bootstrap .libfreetype .misc_libs .tuxbox_libs
	cd $(appsdir)/tuxbox/funstuff && $(CONFIGURE)

.lcdsavers: $(appsdir)/tuxbox/funstuff/lcd-savers/Makefile
	$(MAKE) -C $(appsdir)/tuxbox/funstuff/lcd-savers all install
	touch $@

# tuxbox/lcd

$(appsdir)/tuxbox/lcd/Makefile: .bootstrap .libfreetype .misc_libs .tuxbox_libs
	cd $(appsdir)/tuxbox/lcd && $(CONFIGURE)

.lcdmenu: $(appsdir)/tuxbox/lcd/Makefile
	$(MAKE) -C $(appsdir)/tuxbox/lcd/lcdmenu all install
	touch $@

# tuxbox/libs

$(appsdir)/tuxbox/libs/Makefile: .bootstrap .libfreetype
	cd $(appsdir)/tuxbox/libs && $(CONFIGURE)

.tuxbox_libs: $(appsdir)/tuxbox/libs/Makefile
	$(MAKE) -C $(appsdir)/tuxbox/libs all install
	touch $@

# tuxbox/libtuxbox

$(appsdir)/tuxbox/libtuxbox/config.status: .bootstrap
	cd $(appsdir)/tuxbox/libtuxbox && $(CONFIGURE)

.libtuxbox: $(appsdir)/tuxbox/libtuxbox/config.status
	$(MAKE) -C $(appsdir)/tuxbox/libtuxbox all install
	touch $@

# tuxbox/neutrino

$(appsdir)/dvb/config/config.status:
	cd $(appsdir)/dvb/config && $(CONFIGURE)

.config: $(appsdir)/dvb/config/config.status
	$(MAKE) -C $(appsdir)/dvb/config all install
	touch $@

$(appsdir)/tuxbox/neutrino/Makefile: .bootstrap .libboost .libcurl .libfreetype .libtuxbox .misc_libs .libsigc .plugins .tuxbox_libs .zapit .tuxbox_tools_all .libid3tag .libmad .libvorbisidec .misc_tools .config
	cd $(appsdir)/tuxbox/neutrino && $(CONFIGURE)

.neutrino: $(appsdir)/tuxbox/neutrino/Makefile
	$(MAKE) -C $(appsdir)/tuxbox/neutrino all install
	touch $@

# tuxbox/plugins

$(appsdir)/tuxbox/plugins/Makefile: .bootstrap .libcurl .libjpeg .libpng .libungif .tuxbox_libs
	cd $(appsdir)/tuxbox/plugins && $(CONFIGURE)

.plugins: $(appsdir)/tuxbox/plugins/Makefile
	$(MAKE) -C $(appsdir)/tuxbox/plugins/fx2/lib all install
	$(MAKE) -C $(appsdir)/tuxbox/plugins all install
	touch $@

# tuxbox/tools

$(appsdir)/tuxbox/tools/Makefile: .bootstrap .libtuxbox
	cd $(appsdir)/tuxbox/tools && $(CONFIGURE)

.tuxbox_tools: $(appsdir)/tuxbox/tools/Makefile
	$(MAKE) -C $(appsdir)/tuxbox/tools/misc all install
	touch $@

.tuxbox_tools_all: $(appsdir)/tuxbox/tools/Makefile
	$(MAKE) -C $(appsdir)/tuxbox/tools all install
	touch $@

#######################
#
#   flash
#

if TARGETRULESET_FLASH
if BOXTYPE_DM7000
.dreamfiles: @DEPENDS_dreamdriver_dm7000@ @DEPENDS_dreamfiles@
	@PREPARE_dreamfiles@
	@PREPARE_dreamdriver_dm7000@
endif
if BOXTYPE_DM56x0
.dreamfiles: @DEPENDS_dreamdriver_dm56x0@ @DEPENDS_dreamfiles@
	@PREPARE_dreamfiles@
	@PREPARE_dreamdriver_dm56x0@
endif
if BOXTYPE_DM500
.dreamfiles: @DEPENDS_dreamdriver_dm500@ @DEPENDS_dreamfiles@
	@PREPARE_dreamfiles@
	@PREPARE_dreamdriver_dm500@
endif
	@for i in dreamfiles boot mkcramfs-e mksquashfs mklibs.py mksquashfs_lzma_patches.tar.bz2 ; do \
		rm -R $(flashprefix)/$$i 2>/dev/null || /bin/true; \
		mv $$i $(flashprefix) 2>/dev/null || /bin/true; \
	done
	touch $@

if BOXTYPE_DM7000
dreamboximage_root: core .enigma .reiserfsprogs .e2fsprogs .samba \
	.etherwake .utillinux .linuxkernel .dreamfiles .dvbsnoop .automount \
	flash-enigma flash-plugin_tetris flash-plugin_snake \
	flash-plugin_pacman flash-plugin_tuxtxt flash-plugin_ngrabstartstop \
	flash-ftpd flash-dvb-tools flash-reiserfs flash-fstools flash-sambaserver \
	flash-etherwake flash-dreamfiles flash-sambaclient flash-cifs flash-dvbsnoop \
	flash-plugin_tuxcom flash-automount
if REISERFS_YES
	$(MAKE) flash-mkreiserfs
endif
if WEBIF_EXPERT
	$(MAKE) flash-plugin_dreamflash
	$(MAKE) flash-plugin_movieplayer
endif
if ENABLE_IPKG
	$(MAKE) flash-ipkg
endif
else
dreamboximage_root: core .enigma .etherwake .linuxkernel .dreamfiles .dvbsnoop \
	flash-enigma flash-plugin_tetris flash-plugin_snake \
	flash-plugin_pacman flash-plugin_tuxtxt flash-plugin_ngrabstartstop \
	flash-ftpd flash-dvb-tools flash-etherwake flash-dreamfiles flash-dvbsnoop \
	flash-sambaclient
if ENABLE_IPKG
	$(MAKE) flash-ipkg
endif
endif

dreamboximage_neutrinoroot: core .neutrino .etherwake .linuxkernel .dreamfiles .dvbsnoop .automount\
	flash-neutrino \
	flash-plugin_tuxtxt flash-plugin_tuxcom flash-misc-tools\
	flash-ftpd flash-dvb-tools flash-etherwake flash-dreamfiles flash-dvbsnoop \
	flash-sambaclient flash-automount

rebuild-flash:
	-rm -rf $(flashprefix)/root
	-rm -f $(flashprefix)/.flash
	@for i in `find $(flashprefix) -type f -name "*.img"` ; do rm $$i; done
	-rm -f $(flashprefix)/*.img
	$(MAKE) flash-lib

$(flashprefix)/.flash: .busybox .root
	-rm -rf $(flashprefix)/root
	@for i in bin dev lib/tuxbox proc sbin var tmp etc; do \
		$(INSTALL) -d $(flashprefix)/root/$$i ; done

	$(INSTALL) $(targetprefix)/bin/busybox $(flashprefix)/root/bin
	@for i in `find $(targetprefix)/bin/ -lname "*busybox"` ; do cp -a $$i $(flashprefix)/root/bin/; done
	@for i in `find $(targetprefix)/sbin/ -lname "*busybox"` ; do cp -a $$i $(flashprefix)/root/sbin/; done
	ln -sf busybox $(flashprefix)/root/bin/udhcpc
	ln -sf telnetd $(flashprefix)/root/sbin/in.telnetd

	$(INSTALL) -d $(flashprefix)/root/lib/modules/$(KERNELVERSION)
	cp -R $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel $(flashprefix)/root/lib/modules/$(KERNELVERSION)
if BOXTYPE_DM7000
#remove cifs module when no cifs is requested
	@if [ ! -f $(flashprefix)/.part_cifs -a -d $(flashprefix)/root/lib/modules/$(KERNELVERSION)/kernel/fs/cifs ] ; then \
		rm -rf $(flashprefix)/root/lib/modules/$(KERNELVERSION)/kernel/fs/cifs; \
	fi
#remove cifs module when no smbfs is requested
	@if [ ! -f $(flashprefix)/.part_sambaclient -a -d $(flashprefix)/root/lib/modules/$(KERNELVERSION)/kernel/fs/smbfs ] ; then \
		rm -rf $(flashprefix)/root/lib/modules/$(KERNELVERSION)/kernel/fs/smbfs; \
	fi
#remove reiserfs module when no reiserfs is requested
	@if [ ! -f $(flashprefix)/.part_reiserfs -a ! -f $(flashprefix)/.part_mkreiserfs -a -d $(flashprefix)/root/lib/modules/$(KERNELVERSION)/kernel/fs/reiserfs ] ; then \
		rm -rf $(flashprefix)/root/lib/modules/$(KERNELVERSION)/kernel/fs/reiserfs; \
	fi
endif
	@touch $@

flash-ftpd: $(flashprefix)/.part_ftpd
$(flashprefix)/.part_ftpd: $(flashprefix)/.flash .ftpd
	$(INSTALL) $(targetprefix)/sbin/in.ftpd $(flashprefix)/root/sbin
	$(INSTALL) -d $(flashprefix)/root/share/empty
	@touch $@

flash-dvb-tools: $(flashprefix)/.part_dvb_tools
$(flashprefix)/.part_dvb_tools: $(flashprefix)/.flash .stream
	@for i in streampes streamsec streamts udpstreampes; do \
		$(INSTALL) $(targetprefix)/sbin/$$i $(flashprefix)/root/sbin; done;
	@if [ -e $(targetprefix)/bin/dvbnet ] ; then \
		$(INSTALL) $(targetprefix)/bin/dvbnet $(flashprefix)/root/bin; \
	fi
	@touch $@

flash-misc-tools: $(flashprefix)/.part_misc_tools
$(flashprefix)/.part_misc_tools: $(flashprefix)/.flash .misc_tools
	$(INSTALL) $(targetprefix)/bin/fbshot	$(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/sbin/fcp	$(flashprefix)/root/sbin
	@touch $@

flash-scummvm: $(flashprefix)/.part_scummvm .scummvm
$(flashprefix)/.part_scummvm: $(flashprefix)/.flash
	$(INSTALL) $(targetprefix)/bin/scummvm $(flashprefix)/root/bin;
	@touch $@

flash-dvbsnoop: $(flashprefix)/.part_dvbsnoop .dvbsnoop
$(flashprefix)/.part_dvbsnoop: $(flashprefix)/.flash
	$(INSTALL) $(targetprefix)/bin/dvbsnoop $(flashprefix)/root/bin;
	@touch $@

flash-reiserfs: $(flashprefix)/.part_reiserfs
$(flashprefix)/.part_reiserfs: $(flashprefix)/.flash .reiserfsprogs
	@touch $@

flash-fstools: $(flashprefix)/.part_fstools
$(flashprefix)/.part_fstools: $(flashprefix)/.flash .e2fsprogs .reiserfsprogs .utillinux
	$(INSTALL) -d $(flashprefix)/root/sbin
# reiserfsck is only copied to flashroot when .part_reiserfsck exist
	@for i in reiserfsck e2fsck sfdisk mke2fs; do \
		if [ $$i != reiserfsck -o -f $(flashprefix)/.part_reiserfs ]; then \
			$(INSTALL) $(targetprefix)/sbin/$$i $(flashprefix)/root/sbin; \
		fi; \
	done;
	cd $(flashprefix)/root/sbin && \
	    ln -sf mke2fs mkfs.ext3 && \
	    ln -sf mke2fs mkfs.ext2 && \
	    ln -sf e2fsck fsck.ext2 && \
	    ln -sf e2fsck fsck.ext3 
	@touch $@

flash-mkreiserfs: $(flashprefix)/.part_mkreiserfs
$(flashprefix)/.part_mkreiserfs: $(flashprefix)/.part_reiserfs
	$(INSTALL) -m755 $(targetprefix)/sbin/mkreiserfs $(flashprefix)/root/sbin
	@touch $@

flash-sambaserver: $(flashprefix)/.part_sambaserver
$(flashprefix)/.part_sambaserver: $(flashprefix)/.flash .samba
	$(INSTALL) -d $(flashprefix)/root/bin
	@for i in smbd nmbd ; do \
		$(INSTALL) $(targetprefix)/bin/$$i $(flashprefix)/root/bin; done
	@touch $@

flash-sambaclient: $(flashprefix)/.part_sambaclient
$(flashprefix)/.part_sambaclient: $(flashprefix)/.flash .samba
	$(INSTALL) -d $(flashprefix)/root/bin
	@for i in smbmount smbmnt ; do \
		$(INSTALL) $(targetprefix)/bin/$$i $(flashprefix)/root/bin; done
	@touch $@

flash-automount: $(flashprefix)/.part_automount
$(flashprefix)/.part_automount: $(flashprefix)/.flash
	cp -pa $(targetprefix)/etc/init.d/start_automount $(flashprefix)/root/etc/init.d
	rm -f $(flashprefix)/root/etc/init.d/autofs
	cp -pa $(targetprefix)/sbin/automount $(flashprefix)/root/sbin
	$(INSTALL) -d $(flashprefix)/root/lib/autofs
	@for i in lookup_file.so mount_bind.so mount_generic.so mount_nfs.so parse_sun.so; do \
	cp -pa $(targetprefix)/lib/autofs/$$i $(flashprefix)/root/lib/autofs; done
	ln -sf /proc/mounts $(flashprefix)/root/etc/mtab
	ln -sf /var/etc/auto.net $(flashprefix)/root/etc/auto.net
	@touch $@

flash-cifs: $(flashprefix)/.part_cifs
$(flashprefix)/.part_cifs: $(flashprefix)/.flash .linuxkernel
	@touch $@

flash-etherwake: $(flashprefix)/.part_etherwake
$(flashprefix)/.part_etherwake: $(flashprefix)/.flash .etherwake
	$(INSTALL) -d $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/bin/etherwake $(flashprefix)/root/bin
	@touch $@

flash-dreamfiles: $(flashprefix)/.part_dreamfiles
$(flashprefix)/.part_dreamfiles: $(flashprefix)/.flash .dreamfiles
	@cp -R $(flashprefix)/dreamfiles/* $(flashprefix)/root
	@if [ -f $(flashprefix)/dreamfiles/.version ] ; then \
		cp $(flashprefix)/dreamfiles/.version $(flashprefix)/root; \
	fi
# var_init
	@cp -R $(targetprefix)/var_init $(flashprefix)/root
	@for i in log mnt mnt/cf mnt/nfs mnt/usb run tuxbox/plugins tuxbox/config ; do \
		$(INSTALL) -d $(flashprefix)/root/var_init/$$i; \
	done;
	@if [ -e $(flashprefix)/.part_enigma ]; then \
	    for i in tuxtxt enigma/cable enigma/fonts enigma/pictures enigma/resources enigma/skins enigma/terrestrial; do \
		$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config/$$i; \
	     done \
	fi
	@if [ -e $(flashprefix)/.part_neutrino ] ; then \
		$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config/zapit; \
	fi
	@ln -sf /tmp $(flashprefix)/root/var_init/tmp
	@ln -sf /proc/mounts $(flashprefix)/root/var_init/etc/mtab
# etc
	@cp -R $(targetprefix)/etc $(flashprefix)/root
# lib
	@for i in ar_AE cs_CZ da_DK el_GR es_ES et_EE fi_FI fr_FR hr_HR \
	hu_HU is_IS it_IT lt_LT nl_NL no_NO pl_PL pt_PT ro_RO ru_RU sk_SK \
	sl_SI sr_YU sv_SE tr_TR ur_IN; do \
		ln -sf de_DE $(flashprefix)/root/lib/locale/$$i; \
	done;
	@for i in ISO8859-1.so ISO8859-2.so ISO8859-7.so UNICODE.so; do \
		cp $(targetprefix)/lib/gconv/$$i $(flashprefix)/root/lib/gconv; \
	done;
# share
	@cp -R $(targetprefix)/share/udhcpc $(flashprefix)/root/share
if BOXTYPE_DM500
	@if [ -e $(flashprefix)/.part_enigma -o -e $(flashprefix)/.part_neutrino ] ; then \
		for i in cables.xml satellites.xml terrestrial.xml; do \
			cp -R $(targetprefix)/share/tuxbox/$$i $(flashprefix)/root/share/tuxbox; \
			ln -sf tuxbox/$$i $(flashprefix)/root/share; \
		done \
	fi
else
	@cp $(targetprefix)/share/tuxbox/satellites.xml $(flashprefix)/root/share/tuxbox;
	@ln -sf tuxbox/satellites.xml $(flashprefix)/root/share;
endif
# misc
	@ln -sf /var/mnt $(flashprefix)/root/mnt
	@if [ ! -d $(flashprefix)/root/hdd ]; then mkdir $(flashprefix)/root/hdd; fi

if !BOXTYPE_DM7000
	@if [ -e $(flashprefix)/.part_enigma ] ; then \
	    echo "i:/ezap/osd/alpha=00000000" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config; \
	    echo "i:/ezap/osd/brightness=00000073" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config; \
	    echo "i:/ezap/osd/gamma=00000066" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config; \
	fi
endif

	@for i in skins/small_red*.esml skins/small_red*.info pictures/small-red pictures/triaxlogo-fs8.png ; do \
		rm -R $(flashprefix)/root/share/tuxbox/enigma/$$i; done

if !BOXTYPE_DM500
	@for i in cables.xml terrestrial.xml; do \
		if [ -f $(flashprefix)/root/share/tuxbox/$$i ]; then \
			rm $(flashprefix)/root/share/tuxbox/$$i; \
		fi; \
	done;
endif

if BOXTYPE_DM7000
	echo "i:/elitedvb/extra/cahandlerReconnect=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/elitedvb/extra/hideerror=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/extras/showSatPos=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/extras/mhwepg=00000000" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/ezap/osd/showCurrentRemaining=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/ezap/osd/showOSDOnEITUpdate=00000000" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/extras/hdparm-m=00000080" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/extras/hdparm-s=00000078" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/Nemesis-Project/epgManager/stopemu=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "s:/ezap/ui/skin=/share/tuxbox/enigma/skins/bm_s.esml" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
endif

if BOXTYPE_DM56x0
	echo "i:/elitedvb/extra/cahandlerReconnect=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/elitedvb/extra/hideerror=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/extras/showSatPos=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/extras/mhwepg=00000000" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/ezap/osd/showCurrentRemaining=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/ezap/osd/showOSDOnEITUpdate=00000000" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/Nemesis-Project/epgManager/stopemu=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "s:/ezap/ui/skin=/share/tuxbox/enigma/skins/bm_s.esml" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
endif

if BOXTYPE_DM500
	echo "i:/elitedvb/extra/cahandlerReconnect=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/elitedvb/extra/hideerror=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/extras/showSatPos=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/extras/mhwepg=00000000" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/ezap/osd/showCurrentRemaining=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/ezap/osd/showOSDOnEITUpdate=00000000" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "i:/Nemesis-Project/epgManager/stopemu=00000001" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
	echo "s:/ezap/ui/skin=/share/tuxbox/enigma/skins/bm_s.esml" >> $(flashprefix)/root/var_init/tuxbox/config/enigma/config
endif

	@touch $@

flash-enigma: $(flashprefix)/.part_enigma
$(flashprefix)/.part_enigma: $(flashprefix)/.flash .enigma
	$(INSTALL) $(targetprefix)/bin/enigma $(flashprefix)/root/bin

	$(INSTALL) -d $(flashprefix)/root/share/fonts
#if BOXTYPE_DM7000
#	@for i in blue*.ttf md_khmurabi_10.ttf pakenham.ttf unmrs.pfa; do \
#		$(INSTALL) -m644 $(targetprefix)/share/fonts/$$i $(flashprefix)/root/share/fonts; done
#else
	@for i in md_khmurabi_10.ttf pakenham.ttf unmrs.pfa greg.ttf bignumber.ttf ds-digital.ttf; do \
		$(INSTALL) -m644 $(targetprefix)/share/fonts/$$i $(flashprefix)/root/share/fonts; done
#endif		
	$(INSTALL) -d $(flashprefix)/root/share/locale
	@for i in `find $(targetprefix)/share/locale -name "tuxbox-enigma.mo"` ; \
		do cp -pa $${i%%/LC_MESSAGES/tuxbox-enigma.mo} $(flashprefix)/root/share/locale; done
	@for i in ar cs da de el es et fi fr hr hu is lt nl no pl pt ro ru sk sl sr sv tr ur ; do rm -R $(flashprefix)/root/share/locale/$$i; done
	@rm -rf $(flashprefix)/root/share/locale/LC_MESSAGES
	@find $(flashprefix)/root/share/locale -name libc.mo -o -name nano.mo | xargs rm -f
	$(INSTALL) $(appsdir)/tuxbox/enigma/po/locale.alias $(flashprefix)/root/share/locale
	$(INSTALL) -d $(flashprefix)/root/share/tuxbox/enigma
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config/enigma
	$(INSTALL) -m 0644 $(targetprefix)/var/tuxbox/config/enigma/encoding.conf $(flashprefix)/root/var_init/tuxbox/config/enigma

	cp -pa $(targetprefix)/share/tuxbox/enigma/* $(flashprefix)/root/share/tuxbox/enigma

	@for i in `find $(flashprefix)/root/share/tuxbox/enigma/htdocs` ; do \
		case $$i in \
		*/rc_dbox2.jpg | */rc_dbox2_small.jpg | */topbalk?.png | */topbalk?_small.png) \
			rm $$i;; \
		esac; \
	done;
if BOXTYPE_DM7000
#	@for i in neutrino chk_v2 blueqt carbon ; do \
#	    rm -R $(flashprefix)/root/share/tuxbox/enigma/pictures/$$i; done
#	@for i in neutrino chk_blue2 blueqt carbon ; do \
#		rm -R $(flashprefix)/root/share/tuxbox/enigma/skins/$$i.*; done

	@for i in `find $(flashprefix)/root/share/tuxbox/enigma/resources -type f` ; do \
		case $$i in \
		/*keymappings.xml | */rcdboxold.xml | \
		*/rcdm7000.xml | */rcdreambox_keyboard.xml ) ;; \
		*) rm $$i;; \
		esac; \
	done;
	@if [ ! -d $(flashprefix)/root/root ]; then mkdir $(flashprefix)/root/root; fi
endif
if BOXTYPE_DM500
	CURRENT_PATH=`pwd` && \
	cd $(flashprefix)/root/share/tuxbox/enigma/skins && \
	patch -p0 < $$CURRENT_PATH/Patches/default_lc.esml.diff
	@for i in `find $(flashprefix)/root/share/tuxbox/enigma/resources -type f` ; do \
		case $$i in \
		/*keymappings.xml | */rcdboxold.xml | \
		*/rcdm5xxx.xml.7000 | */rcdm5xxx.xml.PVR |\
		*/rcdm5xxx.xml | */rcdreambox_keyboard.xml ) ;; \
		*) rm $$i;; \
		esac; \
	done;
endif
if BOXTYPE_DM56x0
	CURRENT_PATH=`pwd` && \
	cd $(flashprefix)/root/share/tuxbox/enigma/skins && \
	patch -p0 < $$CURRENT_PATH/Patches/default_lc.esml.diff
	@for i in `find $(flashprefix)/root/share/tuxbox/enigma/resources -type f` ; do \
		case $$i in \
		/*keymappings.xml | */rcdboxold.xml | \
		*/rcdm5xxx.xml | */rcdreambox_keyboard.xml ) ;; \
		*) rm $$i;; \
		esac; \
	done;
endif

	$(INSTALL) -d $(flashprefix)/root/share/zoneinfo
	$(INSTALL) -m644 $(targetprefix)/share/zoneinfo/* $(flashprefix)/root/share/zoneinfo
	rm -f $(flashprefix)/root/share/zoneinfo/*tab

	ln -sf /var/etc/localtime $(flashprefix)/root/etc

if WEBIF_EXPERT
	$(INSTALL) $(targetprefix)/bin/showlogo $(flashprefix)/root/bin
if BOXTYPE_DM7000
	$(INSTALL) $(targetprefix)/bin/bootmenue $(flashprefix)/root/bin
endif
	$(INSTALL) $(targetprefix)/bin/chttpd $(flashprefix)/root/bin
endif

if ENABLE_SQLITE
	$(INSTALL) $(targetprefix)/bin/sqlite3 $(flashprefix)/root/bin
endif
	@touch $@

flash-ushare: $(flashprefix)/.part_ushare
$(flashprefix)/.part_ushare: $(flashprefix)/.flash .ushare
	$(INSTALL) $(targetprefix)/bin/ushare $(flashprefix)/root/bin
	$(INSTALL) -d $(flashprefix)/root/etc
	@ln -sf /var/etc/ushare.conf $(flashprefix)/root/etc
	$(INSTALL) -d $(flashprefix)/root/var_init/etc
	$(INSTALL) $(targetprefix)/etc/ushare.conf.orig $(flashprefix)/root/var_init/etc/ushare.conf
	@sed -i "{s#USHARE_NAME=#USHARE_NAME=Dreambox#};{s#USHARE_DIR=#USHARE_DIR=/hdd,/var/mnt#}" \
		$(flashprefix)/root/var_init/etc/ushare.conf
	@if [ ! -e $(flashprefix)/root/var_init/etc/init ]; then \
		echo "#!/bin/sh" >> $(flashprefix)/root/var_init/etc/init; fi
	@echo "#ushare - delayed start, wait until network is up" >> $(flashprefix)/root/var_init/etc/init
	@echo "sleep 30 && ushare -Do &" >> $(flashprefix)/root/var_init/etc/init
	@chmod 755 $(flashprefix)/root/var_init/etc/init
	@touch $@

flash-amule: $(flashprefix)/.part_amule
$(flashprefix)/.part_amule: $(flashprefix)/.flash .amule
	@rm -rf $(flashprefix)/aMule $(flashprefix)/aMule.tar
	$(INSTALL) -d $(flashprefix)/aMule
	@for i in amulecmd amuled amulestart amuleweb ed2k; \
		do cp -a $(targetprefix)/bin/$$i $(flashprefix)/root/bin; done
	$(INSTALL) -d $(flashprefix)/aMule/.aMule
	@cp -a $(targetprefix)/share/amule/webserver $(flashprefix)/aMule/.aMule
	@cp -a $(targetprefix)/bin/amule.install $(flashprefix)/install
	chmod 755 $(flashprefix)/install
	CURRENT_PATH=`pwd` && cd $(flashprefix) && tar -cf install.tar install && rm install && cd $$CURRENT_PATH
	@touch $@

flash-polipo: $(flashprefix)/.part_polipo
$(flashprefix)/.part_polipo: $(flashprefix)/.flash .polipo
	$(INSTALL) $(targetprefix)/bin/polipo $(flashprefix)/root/bin
	$(INSTALL) -d $(flashprefix)/root/share
	@cp -ap $(targetprefix)/share/polipo $(flashprefix)/root/share
	$(INSTALL) -d $(flashprefix)/root/var_init/etc
	@cp -pa $(targetprefix)/var/etc/polipo $(flashprefix)/root/var_init/etc
	@echo "diskCacheRoot = \"/hdd/cache/\"" >> $(flashprefix)/root/var_init/etc/polipo/config
	@if [ -f $(flashprefix)/.part_tor ]; then \
		echo "socksParentProxy = localhost:9050" >> $(flashprefix)/root/var_init/etc/polipo/config; fi
	@if [ ! -e $(flashprefix)/root/var_init/etc/init ]; then \
		echo "#!/bin/sh" >> $(flashprefix)/root/var_init/etc/init; fi
	@echo "#Polipo - delayed start, wait until network is up" >> $(flashprefix)/root/var_init/etc/init
	@echo "sleep 30 && polipo -c /var/etc/polipo/config &" >> $(flashprefix)/root/var_init/etc/init
	@echo "export http_proxy=http://localhost:8123/" >> $(flashprefix)/root/var_init/etc/init
	chmod 755 $(flashprefix)/root/var_init/etc/init
	@if [ ! -e $(flashprefix)/root/var_init/etc/uinit ]; then \
		echo "#!/bin/sh" >> $(flashprefix)/root/var_init/etc/uinit; fi
	@echo "#Stop polipo and purge the on-disk cache" >> $(flashprefix)/root/var_init/etc/uinit
	@echo "killall polipo && polipo -x" >> $(flashprefix)/root/var_init/etc/uinit
	chmod 755 $(flashprefix)/root/var_init/etc/uinit
	@touch $@

flash-ctorrent: $(flashprefix)/.part_ctorrent
$(flashprefix)/.part_ctorrent: $(flashprefix)/.flash .ctorrent
	$(INSTALL) -d $(flashprefix)/root/var_init/bin
	@echo "#!/bin/sh" > $(flashprefix)/root/var_init/bin/ctorrent.sh
	@echo "#Optional IP of a Server" >> $(flashprefix)/root/var_init/bin/ctorrent.sh
	@echo "Server=192.168.0.11" >> $(flashprefix)/root/var_init/bin/ctorrent.sh
	@echo "#Optional Port is 2780 (TCP)" >> $(flashprefix)/root/var_init/bin/ctorrent.sh
	@echo "Port=2780" >> $(flashprefix)/root/var_init/bin/ctorrent.sh
	@echo "#Exit after 1:1 ratio, dont use cache memory" >> $(flashprefix)/root/var_init/bin/ctorrent.sh
	@echo "/bin/ctorrent -E 1 -C 0 -S \$$Server:\$$Port \$$1 \$$2 \$$3" >> $(flashprefix)/root/var_init/bin/ctorrent.sh
	$(INSTALL) $(targetprefix)/bin/ctorrent $(flashprefix)/root/bin
	@touch $@

flash-tor: $(flashprefix)/.part_tor
$(flashprefix)/.part_tor: $(flashprefix)/.flash .tor
	$(INSTALL) $(targetprefix)/bin/tor $(flashprefix)/root/bin
	$(INSTALL) -d $(flashprefix)/root/var_init/bin
	@ln -sf /hdd/tor $(flashprefix)/root/var_init/bin
	$(INSTALL) -d $(flashprefix)/root/var_init/etc
	@cp -pa $(targetprefix)/var/etc/tor $(flashprefix)/root/var_init/etc
	@ln -sf /var/etc/tor $(flashprefix)/root/etc
	@sed -e "{s,#RunAsDaemon,RunAsDaemon,};{s,#DataDirectory /var/lib/tor,DataDirectory /hdd/tor,}" \
		< $(flashprefix)/root/var_init/etc/tor/torrc.sample > $(flashprefix)/root/var_init/etc/tor/torrc
	@if [ ! -e $(flashprefix)/root/var_init/etc/init ]; then \
		echo "#!/bin/sh" >> $(flashprefix)/root/var_init/etc/init; fi
	@echo "#tor - delayed start, wait until network is up" >> $(flashprefix)/root/var_init/etc/init
	@echo "sleep 30 && tor &" >> $(flashprefix)/root/var_init/etc/init
	chmod 755 $(flashprefix)/root/var_init/etc/init
	@if [ ! -e $(flashprefix)/root/var_init/etc/uinit ]; then \
		echo "#!/bin/sh" >> $(flashprefix)/root/var_init/etc/uinit; fi
	@echo "#Stop tor" >> $(flashprefix)/root/var_init/etc/uinit
	@echo "killall tor" >> $(flashprefix)/root/var_init/etc/uinit
	chmod 755 $(flashprefix)/root/var_init/etc/uinit
	@touch $@

flash-joe: $(flashprefix)/.part_joe
$(flashprefix)/.part_joe: $(flashprefix)/.flash .joe
	$(INSTALL) -d $(flashprefix)/root/etc/joe
	cp -ap $(targetprefix)/etc/joe/* $(flashprefix)/root/etc/joe/
	$(INSTALL) $(targetprefix)/bin/joe $(flashprefix)/root/bin
	$(target)-strip --strip-all $(flashprefix)/root/bin/joe
	@touch $@

flash-ssh: $(flashprefix)/.part_ssh
$(flashprefix)/.part_ssh: $(flashprefix)/.flash .ssh
	$(INSTALL) -d $(flashprefix)/root/etc/ssh
	cp -ap $(targetprefix)/etc/ssh/ssh_config $(flashprefix)/root/etc/ssh
	$(INSTALL) $(targetprefix)/bin/ssh $(targetprefix)/bin/scp \
		$(targetprefix)/bin/sftp $(flashprefix)/root/bin
	@touch $@

flash-sshd: $(flashprefix)/.part_sshd
$(flashprefix)/.part_sshd: $(flashprefix)/.flash .ssh
	$(INSTALL) -d $(flashprefix)/root/etc/ssh
	$(INSTALL) -d $(flashprefix)/root/libexec
	$(INSTALL) -d $(flashprefix)/root/share/empty
	$(INSTALL) $(targetprefix)/bin/ssh-keygen $(targetprefix)/bin/scp \
		$(flashprefix)/root/bin
	cp -ap $(targetprefix)/etc/ssh/* $(flashprefix)/root/etc/ssh
	$(INSTALL) $(targetprefix)/libexec/sftp-server $(flashprefix)/root/libexec
	$(INSTALL) $(targetprefix)/sbin/sshd $(flashprefix)/root/sbin
	@touch $@

flash-plugin_dreamdata: $(flashprefix)/.part_plugin_dreamdata
$(flashprefix)/.part_plugin_dreamdata: $(flashprefix)/.flash .mrouted .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/dreamdata.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/dreamdata.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) -m644 $(targetprefix)/var/tuxbox/config/dreamdata.xml $(flashprefix)/root/var_init/tuxbox/config/
	$(INSTALL) $(targetprefix)/bin/mrouted $(flashprefix)/root/bin
	@touch $@

flash-plugin_mosaic: $(flashprefix)/.part_plugin_mosaic
$(flashprefix)/.part_plugin_mosaic: $(flashprefix)/.flash .enigma .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/mosaic.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/mosaic.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_pip: $(flashprefix)/.part_plugin_pip
$(flashprefix)/.part_plugin_pip: $(flashprefix)/.flash .enigma .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/pip.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/pip.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_rss: $(flashprefix)/.part_plugin_rss
$(flashprefix)/.part_plugin_rss: $(flashprefix)/.flash .plugins flash-enigma
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/rss.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/rss.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) -m644 $(targetprefix)/var/tuxbox/config/feeds.xml $(flashprefix)/root/var_init/tuxbox/config/
	@touch $@

flash-plugin_script: $(flashprefix)/.part_plugin_script
$(flashprefix)/.part_plugin_script: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/script.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/script.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_movieplayer: $(flashprefix)/.part_plugin_movieplayer
$(flashprefix)/.part_plugin_movieplayer: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/movieplayer.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/movieplayer.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_dbswitch: $(flashprefix)/.part_plugin_dbswitch
$(flashprefix)/.part_plugin_dbswitch: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/dbswitch.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/dbswitch.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_getset: $(flashprefix)/.part_plugin_getset
$(flashprefix)/.part_plugin_getset: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/enigma_getset.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/enigma_getset.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_tuxtxt: $(flashprefix)/.part_plugin_tuxtxt
$(flashprefix)/.part_plugin_tuxtxt: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/share/fonts
#	$(INSTALL) -m644 $(targetprefix)/share/fonts/tuxtxt.ttf $(flashprefix)/root/share/fonts/
	$(INSTALL) -m644 $(targetprefix)/share/fonts/tuxtxt.otb $(flashprefix)/root/share/fonts/
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxtxt.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxtxt.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config/tuxtxt
	$(INSTALL) -m644 $(targetprefix)/var/tuxbox/config/tuxtxt/tuxtxt.conf $(flashprefix)/root/var_init/tuxbox/config/tuxtxt
	@touch $@

flash-plugin_tuxcom: $(flashprefix)/.part_plugin_tuxcom
$(flashprefix)/.part_plugin_tuxcom: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config/tuxcom
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxcom.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxcom.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) -m755 $(targetprefix)/var/tuxbox/config/tuxcom/mp3 $(flashprefix)/root/var_init/tuxbox/config/tuxcom
	@touch $@

flash-plugin_tuxcal: $(flashprefix)/.part_plugin_tuxcal
$(flashprefix)/.part_plugin_tuxcal: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxcal.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxcal.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_tuxmail: $(flashprefix)/.part_plugin_tuxmail
$(flashprefix)/.part_plugin_tuxmail: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/share/fonts
	$(INSTALL) -m644 $(targetprefix)/share/fonts/tuxtxt.ttf $(flashprefix)/root/share/fonts/
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxmail.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxmail.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/bin/tuxmaild $(flashprefix)/root/bin/
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config/tuxmail
	$(INSTALL) -m644 $(targetprefix)/var/tuxbox/config/tuxmail/tuxmail.conf $(flashprefix)/root/var_init/tuxbox/config/tuxmail/tuxmail.conf.default
	@touch $@

flash-plugin_libfx2: $(flashprefix)/.part_plugin_libfx2
$(flashprefix)/.part_plugin_libfx2: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	cd $(flashprefix)/root/lib/tuxbox/plugins && ln -sf /lib/libfx2.so libfx2.so 
	@touch $@

flash-plugin_lcdcirc: $(flashprefix)/.part_plugin_lcdcirc
$(flashprefix)/.part_plugin_lcdcirc: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/lcdcirc.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/lcdcirc.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_satfind: $(flashprefix)/.part_plugin_satfind
$(flashprefix)/.part_plugin_satfind: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/satfind.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/satfind.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_pacman: $(flashprefix)/.part_plugin_pacman
$(flashprefix)/.part_plugin_pacman: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/pacman.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/pacman.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_snake: $(flashprefix)/.part_plugin_snake
$(flashprefix)/.part_plugin_snake: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/snake.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/snake.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_tetris: $(flashprefix)/.part_plugin_tetris
$(flashprefix)/.part_plugin_tetris: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tetris.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tetris.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_links: $(flashprefix)/.part_plugin_links
$(flashprefix)/.part_plugin_links: $(flashprefix)/.flash
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/links.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/links.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/plugins
	$(INSTALL) -d $(flashprefix)/root/var_init/etc
	@echo "#!/bin/sh" >> install
	@echo "#Wo ist links" >> install
	@echo "LINKSHOME=/tmp" >> install
	@echo "if [ -d /hdd/links ] ; then LINKSHOME=/hdd/links ; fi" >> install
	@echo "if [ -d /mnt/cf/links ] ; then LINKSHOME=/mnt/cf/links ; fi" >> install
	@echo "if [ -d /mnt/usb/links ] ; then LINKSHOME=/mnt/usb/links ; fi" >> install
	@echo "if [ ! -d /var/tuxbox/plugins ] ; then mkdir /var/tuxbox/plugins ; fi" >> install
	@echo "ln -sf \$$LINKSHOME/links.so /var/tuxbox/plugins/links.so" >> install
	@echo "ln -sf \$$LINKSHOME/links.cfg /var/tuxbox/plugins/links.cfg" >> install
	@echo "/bin/chmod 744 \$$LINKSHOME/links.*" >> install
	@echo "if [ ! -e /var/etc/init ] ; then" >> install
	@echo "	echo \"#!/bin/sh\" >> /var/etc/init" >> install
	@echo "fi" >> install
	@echo "echo \"#Links Startpage\" >> /var/etc/init" >> install
	@echo "echo \"export WWW_HOME=\\\"www.dream-multimedia-tv.de\\\"\" >> /var/etc/init" >> install
	@echo "chmod +x /var/etc/init" >> install
	@chmod +x install
	@tar cf install.tar install
	$(INSTALL) install.tar $(flashprefix)/root/var_init/etc
	@rm install
	@touch $@

flash-plugin_lemmings: $(flashprefix)/.part_plugin_lemmings
$(flashprefix)/.part_plugin_lemmings: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/lemmings.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/lemmings.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_solitair: $(flashprefix)/.part_plugin_solitair
$(flashprefix)/.part_plugin_solitair: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/solitair.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/solitair.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_sol: $(flashprefix)/.part_plugin_sol
$(flashprefix)/.part_plugin_sol: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/sol.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/sol.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_mastermind: $(flashprefix)/.part_plugin_mastermind
$(flashprefix)/.part_plugin_mastermind: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/master.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/master.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_sokoban: $(flashprefix)/.part_plugin_sokoban
$(flashprefix)/.part_plugin_sokoban: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/soko.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/soko.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) -d $(flashprefix)/root/share/tuxbox/sokoban
	cp -pa $(targetprefix)/share/tuxbox/sokoban/*.xsb $(flashprefix)/root/share/tuxbox/sokoban/
	@touch $@

flash-plugin_tankwars: $(flashprefix)/.part_plugin_tankwars
$(flashprefix)/.part_plugin_tankwars: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tank.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tank.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_tuxwetter: $(flashprefix)/.part_plugin_tuxwetter
$(flashprefix)/.part_plugin_tuxwetter: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) -d $(flashprefix)/root/share/tuxbox/tuxwetter/
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config/tuxwetter
	$(INSTALL) $(targetprefix)/bin/tuxwetter $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxwetter.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/tuxwetter.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) -m644 $(targetprefix)/share/tuxbox/tuxwetter/* $(flashprefix)/root/share/tuxbox/tuxwetter/
	$(INSTALL) $(targetprefix)/var/tuxbox/config/tuxwetter/convert.list $(flashprefix)/root/var_init/tuxbox/config/tuxwetter/
	$(INSTALL) $(targetprefix)/var/tuxbox/config/tuxwetter/tuxwetter.conf $(flashprefix)/root/var_init/tuxbox/config/tuxwetter/
	@touch $@

flash-plugin_viergewinnt: $(flashprefix)/.part_plugin_viergewinnt
$(flashprefix)/.part_plugin_viergewinnt: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/vierg.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/vierg.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_weather: $(flashprefix)/.part_plugin_weather
$(flashprefix)/.part_plugin_weather: $(flashprefix)/.flash .plugins flash-enigma
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) -d $(flashprefix)/root/share/tuxbox/weather
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/weather.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/weather.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) -m644 $(targetprefix)/share/tuxbox/weather/*.png $(flashprefix)/root/share/tuxbox/weather/
	$(INSTALL) -m644 $(targetprefix)/var/tuxbox/config/weather.xml $(flashprefix)/root/var_init/tuxbox/config/
	@touch $@

flash-plugin_yahtzee: $(flashprefix)/.part_plugin_yahtzee
$(flashprefix)/.part_plugin_yahtzee: $(flashprefix)/.flash flash-plugin_libfx2
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/yahtzee.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/yahtzee.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_ngrabstartstop: $(flashprefix)/.part_plugin_ngrabstartstop
$(flashprefix)/.part_plugin_ngrabstartstop: $(flashprefix)/.flash .plugins flash-enigma
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	@for i in so cfg; do \
		cp $(targetprefix)/lib/tuxbox/plugins/ngrab*.$$i $(flashprefix)/root/lib/tuxbox/plugins/ ; done;
	@touch $@

flash-plugin_dslstartstop: $(flashprefix)/.part_plugin_ngrabtools
$(flashprefix)/.part_plugin_dslstartstop: $(flashprefix)/.flash .plugins flash-enigma
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	@for i in so cfg; do \
		cp $(targetprefix)/lib/tuxbox/plugins/dsl*con*.$$i $(flashprefix)/root/lib/tuxbox/plugins/ ; done;
	@touch $@

flash-plugin_dreamflash: $(flashprefix)/.part_plugin_dreamflash
$(flashprefix)/.part_plugin_dreamflash: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/dreamflash.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/dreamflash.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-plugin_scummvm: $(flashprefix)/.part_plugin_scummvm
$(flashprefix)/.part_plugin_scummvm: $(flashprefix)/.flash .plugins
	$(INSTALL) -d $(flashprefix)/root/lib/tuxbox/plugins
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/scummvm.so $(flashprefix)/root/lib/tuxbox/plugins/
	$(INSTALL) $(targetprefix)/lib/tuxbox/plugins/scummvm.cfg $(flashprefix)/root/lib/tuxbox/plugins/
	@touch $@

flash-less: $(flashprefix)/.part_less
$(flashprefix)/.part_less: $(flashprefix)/.flash .less
	$(INSTALL) $(targetprefix)/bin/less $(flashprefix)/root/bin
	@touch $@

flash-diffutils: $(flashprefix)/.part_diffutils
$(flashprefix)/.part_diffutils: $(flashprefix)/.flash .diffutils
	$(INSTALL) $(targetprefix)/bin/diff $(flashprefix)/root/bin
	@touch $@

flash-dropbear: $(flashprefix)/.part_dropbear
$(flashprefix)/.part_dropbear: $(flashprefix)/.flash .dropbear
	$(INSTALL) $(targetprefix)/sbin/dropbearmulti $(flashprefix)/root/sbin
	for i in dropbear scp dropbearkey; do \
		ln -sf dropbearmulti $(flashprefix)/root/sbin/$$i; done;
	mkdir -p $(flashprefix)/root/var_init/.ssh
	ln -sf /var/.ssh $(flashprefix)/root/.ssh
	@touch $@

flash-ipkg: $(flashprefix)/.part_ipkg
$(flashprefix)/.part_ipkg: $(flashprefix)/.flash .ipkg
	$(INSTALL) $(targetprefix)/bin/ipkg $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/lib/libipkg.so.0 $(flashprefix)/root/lib
	ln -sf  /var/etc/ipkg.conf $(flashprefix)/root/etc/ipkg.conf
	@touch $@

############################# UNTESTED!!! ##################################
flash-neutrino: $(flashprefix)/.part_neutrino
$(flashprefix)/.part_neutrino: $(flashprefix)/.flash .neutrino
	$(INSTALL) -d $(flashprefix)/root/share/fonts
	$(INSTALL) -d $(flashprefix)/root/share/iso-codes
	$(INSTALL) -d $(flashprefix)/root/share/tuxbox
	$(INSTALL) -d $(flashprefix)/root/etc/init.d
	$(INSTALL) -d $(flashprefix)/root/var_init/etc/init.d
	$(INSTALL) -d $(flashprefix)/root/var_init/tuxbox/config/lirc
	$(INSTALL) $(targetprefix)/bin/neutrino \
		$(targetprefix)/bin/nhttpd $(targetprefix)/bin/timerd \
		$(flashprefix)/root/bin
	cp -pa $(targetprefix)/share/tuxbox/neutrino $(targetprefix)/share/tuxbox/lcdd \
		$(flashprefix)/root/share/tuxbox
#	$(INSTALL) $(targetprefix)/bin/camd2 $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/bin/sectionsd $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/bin/sectionsdcontrol $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/bin/pzapit $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/bin/zapit $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/bin/satfind $(flashprefix)/root/bin
	cp -pa $(appsdir)/tuxbox/enigma/data/fonts/bluebold.ttf $(flashprefix)/root/share/fonts
	cp -pa $(appsdir)/tuxbox/enigma/data/fonts/bluehigh.ttf $(flashprefix)/root/share/fonts
	cp -pa $(appsdir)/tuxbox/enigma/data/fonts/md_khmurabi_10.ttf $(flashprefix)/root/share/fonts
	cp -pa $(appsdir)/tuxbox/enigma/data/fonts/pakenham.ttf $(flashprefix)/root/share/fonts
	cp -pa $(appsdir)/tuxbox/enigma/data/fonts/unmrs.pfa $(flashprefix)/root/share/fonts
	cp -pa $(targetprefix)/share/fonts/micron*.ttf $(flashprefix)/root/share/fonts
	cp -pa $(targetprefix)/share/fonts/12.pcf.gz $(flashprefix)/root/share/fonts
	cp -pa $(targetprefix)/share/fonts/14B.pcf.gz $(flashprefix)/root/share/fonts
	cp -pa $(targetprefix)/share/fonts/15B.pcf.gz $(flashprefix)/root/share/fonts
	cp -pa $(targetprefix)/share/iso-codes/iso-639.tab $(flashprefix)/root/share/iso-codes
	@ln -sf /var/etc/network $(targetprefix)/etc/network
	ln -sf /var/etc/localtime $(flashprefix)/root/etc
	rm -rf $(flashprefix)/root/var_init/run
	ln -sf /tmp $(flashprefix)/root/var_init/run
	@touch $@

flash-lcars: $(flashprefix)/.part_lcars
$(flashprefix)/.part_lcars: $(flashprefix)/.flash .lcars
	$(INSTALL) -d $(flashprefix)/root/share/fonts
	$(INSTALL) $(targetprefix)/bin/lcars $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/bin/aniplay $(flashprefix)/root/bin
	$(INSTALL) $(targetprefix)/bin/lcdstars $(flashprefix)/root/bin
	cp -pa $(targetprefix)/share/fonts/ds9.ttf $(flashprefix)/root/share/fonts
	cp -pa $(targetprefix)/share/tuxbox/lcars $(flashprefix)/root/share/tuxbox
	mkdir $(flashprefix)/root/var_init
	mkdir $(flashprefix)/root/var_init/tuxbox
	mkdir $(flashprefix)/root/var_init/tuxbox/config
	cp -pa $(targetprefix)/var/tuxbox/config/lcars $(flashprefix)/root/var_init/tuxbox/config
	@touch $@

flash-lcdmenu: $(flashprefix)/.part_lcdmenu
$(flashprefix)/.part_lcdmenu: $(flashprefix)/.flash .lcdmenu
	$(INSTALL) -d $(flashprefix)/root/share/fonts
	$(INSTALL) $(targetprefix)/bin/lcdmenu $(flashprefix)/root/bin
	cp -pa $(targetprefix)/share/fonts/micron.ttf $(flashprefix)/root/share/fonts
	@touch $@
############################################################################

flash-lib: $(flashprefix)/.lib 
$(flashprefix)/.lib: $(flashprefix)/.flash $(shell ls -r -1 $(flashprefix)/.part_* 2>/dev/null)
	@find $(flashprefix)/root/lib -maxdepth 1 -type f -o -type l | xargs rm -f
	@cp -a $(targetprefix)/lib/libnss_dns-?.*.so $(flashprefix)/root/lib
	@cp -a $(targetprefix)/lib/libnss_files-?.*.so $(flashprefix)/root/lib
	@cp -a $(targetprefix)/lib/libfribidi.so* $(flashprefix)/root/lib
	@if [ -e ./customize-flash-pre ] ; then \
		chmod +x ./customize-flash-pre; \
		./customize-flash-pre $(flashprefix) ; \
	fi
	@$(PYTHON) $(flashprefix)/mklibs.py --target $(target) --ldlib ld.so.1 --libc-extras-dir $(targetprefix)/lib/libc_pic \
		-d $(flashprefix)/root/lib \
		-D -L $(targetprefix)/lib \
		--root $(flashprefix)/root \
		`find $(flashprefix)/root/bin/ -path "*bin/?*"` \
		`find $(flashprefix)/root/lib/ -name "libnss_*"` \
		`find $(flashprefix)/root/lib/tuxbox/ -name "*.so" -type f` \
		`find $(flashprefix)/root/lib/autofs/ -name "*.so" -type f` \
		`find $(flashprefix)/root/sbin/ -path "*sbin/?*"`
	cd $(flashprefix)/root/lib && ln -sf libgcc_s_nof.so.1 libgcc_s.so.1
	@if [ -e ./customize-flash ] ; then \
		chmod +x ./customize-flash; \
		./customize-flash $(flashprefix); \
	fi
	@$(target)-strip --remove-section=.comment --remove-section=.note --strip-unneeded \
		`find $(flashprefix)/root/ -path "*bin/?*"` 2>/dev/null || /bin/true
	@$(target)-strip --remove-section=.comment --remove-section=.note --strip-unneeded \
		`find $(flashprefix)/root/lib -name "*.so*"` 2>/dev/null || /bin/true
	@rm `find $(flashprefix)/root/lib/ -name "*.la"` 2>/dev/null || /bin/true
	@if [ -e $(flashprefix)/.part_plugin_links ] ; then \
		mv $(flashprefix)/root/lib/tuxbox/plugins/links.* $(flashprefix); \
	fi
	@if [ -e $(flashprefix)/.part_scummvm ] ; then \
		mv $(flashprefix)/root/bin/scummvm $(flashprefix); \
	fi
	@if [ -e $(flashprefix)/.part_amule ]; then \
		for i in amulecmd amuled amulestart amuleweb ed2k; do \
			mv $(flashprefix)/root/bin/$$i $(flashprefix)/aMule; done && \
		CURRENT_PATH=`pwd` && cd $(flashprefix) && \
		tar -cf aMule.tar aMule && gzip -f aMule.tar && \
		rm -rf aMule && cd $$CURRENT_PATH; \
	fi
	@if [ -e $(flashprefix)/.part_tor ]; then \
		mv $(flashprefix)/root/bin/tor $(flashprefix) && \
		CURRENT_PATH=`pwd` && cd $(flashprefix) && \
		tar -czf tor.tar.gz tor && \
		rm -rf tor && cd $$CURRENT_PATH; \
	fi
	@chmod u+rwX,go+rX-w -R $(flashprefix)/root/
	@touch $@

flash-compress: $(flashprefix)/boot-cramfs.img $(flashprefix)/root-squashfs.img $(flashprefix)/complete.img

$(flashprefix)/boot-cramfs.img: .linuxkernel $(flashprefix)/.part_dreamfiles
# Nemesis
if BOXTYPE_DM7000
	install -m755 ${nemroot}/misc/socket/nemesisc ${flashprefix}/root/bin/
	install -m755 ${nemroot}/misc/socket/nemesisd ${flashprefix}/root/bin/
	install ${nemroot}/logo/cvs/bild ${flashprefix}/boot/root/platform/kernel/bild
	install ${nemroot}/logo/cvs/bild1 ${flashprefix}/root/bin/bild1
	install ${nemroot}/logo/cvs/bild2 ${flashprefix}/root/bin/bild2
	install ${nemroot}/logo/cvs/swap ${flashprefix}/root/bin/swap
	mkdir -p ${flashprefix}/root/var_init/{lib,.ssh,scam,keys,log,sbin,scce,spool,uninstall}
	mkdir -p ${flashprefix}/root/var_init/mnt/{dvd,cifs,cdrom}
	cp -rf ${nemroot}/patch/enigma1/setting/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/vlcf/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/nfs-utils/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/iptables/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/inadyn/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/openvpn/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/weekepglog/root ${flashprefix}/
	find ${flashprefix}/root/ -name ".svn" | xargs rm -rf 
	rm -f ${flashprefix}/root/etc/init.d/start_automount
	rm -f ${flashprefix}/root/etc/auto.*
	ln -s /var/.ssh ${flashprefix}/root/.ssh
	ln -s /var ${flashprefix}/root/usr
	ln -s /var/mnt/nfs ${flashprefix}/root/nfs
	ln -s /var/mnt/cf ${flashprefix}/root/cf
	ln -s /var/mnt/usb ${flashprefix}/root/usb
	ln -s /var/mnt ${flashprefix}/root/media
	ln -s /var/autofs ${flashprefix}/root/autofs
	ln -s /var/autofs/epg ${flashprefix}/root/epg
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libc.so.6 ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libcurl.so.2 ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libfx2.so ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libssl.so.0.9.7 ${flashprefix}/root/lib/
	ln -s libssl.so.0.9.7 ${flashprefix}/root/lib/libssl.so
	ln -s libssl.so.0.9.7 ${flashprefix}/root/lib/libssl.so.0
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libcrypto.so.0.9.7 ${flashprefix}/root/var_init/lib/
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so.0
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so.0.9.7
	@if [ ! -e ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net ]; then \
			mkdir ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net; \
	fi
	install ${nemroot}/patch/enigma1/setting/lib/tun.ko ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net/
endif

if BOXTYPE_DM500
	install -m755 ${nemroot}/misc/socket/nemesisd ${flashprefix}/root/bin/
	install ${nemroot}/logo/cvs/bild ${flashprefix}/boot/root/platform/kernel/bild
# 	install ${nemroot}/logo/cvs/bild1 ${flashprefix}/root/bin/bild1
# 	install ${nemroot}/logo/cvs/bild2 ${flashprefix}/root/bin/bild2
# 	install ${nemroot}/logo/cvs/ferase ${flashprefix}/root/bin/ferase
	mkdir -p ${flashprefix}/root/var_init/{lib,.ssh,scam,keys,log,sbin,scce,spool,uninstall}
	cp -rf ${nemroot}/patch/enigma1/setting/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/iptables/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/inadyn/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/openvpn/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/weekepglog/root ${flashprefix}/
	find ${flashprefix}/root/ -name ".svn" | xargs rm -rf 
	rm -rf ${flashprefix}/root/share/tuxbox/enigma/skins/default.esml.orig
	rm -rf ${flashprefix}/root/var_init/etc/samba_startup.sh
	rm -rf ${flashprefix}/root/var_init/etc/nfsserver_startup.sh
	rm -f ${flashprefix}/root/etc/init.d/start_automount
	rm -f ${flashprefix}/root/etc/auto.*
	ln -s /var/.ssh ${flashprefix}/root/.ssh
	ln -s /var ${flashprefix}/root/usr
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libc.so.6 ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libcurl.so.2 ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libfx2.so ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libssl.so.0.9.7 ${flashprefix}/root/lib/
	ln -s libssl.so.0.9.7 ${flashprefix}/root/lib/libssl.so
	ln -s libssl.so.0.9.7 ${flashprefix}/root/lib/libssl.so.0
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libcrypto.so.0.9.7 ${flashprefix}/root/var_init/lib/
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so.0
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so.0.9.7
	@if [ ! -e ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net ]; then \
			mkdir ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net; \
	fi
	install ${nemroot}/patch/enigma1/setting/lib/tun.ko ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net/
endif

if BOXTYPE_DM56x0
	install -m755 ${nemroot}/misc/socket/nemesisd ${flashprefix}/root/bin/
	install ${nemroot}/logo/cvs/bild ${flashprefix}/boot/root/platform/kernel/bild
# 	install ${nemroot}/logo/cvs/bild1 ${flashprefix}/root/bin/bild1
# 	install ${nemroot}/logo/cvs/bild2 ${flashprefix}/root/bin/bild2
# 	install ${nemroot}/logo/cvs/ferase ${flashprefix}/root/bin/ferase
	mkdir -p ${flashprefix}/root/var_init/{lib,.ssh,scam,keys,log,sbin,scce,spool,uninstall}
	cp -rf ${nemroot}/patch/enigma1/setting/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/iptables/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/inadyn/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/openvpn/root ${flashprefix}/
	cp -rf ${nemroot}/patch/enigma1/setting/weekepglog/root ${flashprefix}/
	find ${flashprefix}/root/ -name ".svn" | xargs rm -rf 
	rm -rf ${flashprefix}/root/share/tuxbox/enigma/skins/default.esml.orig
	rm -rf ${flashprefix}/root/var_init/etc/samba_startup.sh
	rm -rf ${flashprefix}/root/var_init/etc/nfsserver_startup.sh
	rm -f ${flashprefix}/root/etc/init.d/start_automount
	rm -f ${flashprefix}/root/etc/auto.*
	ln -s /var/.ssh ${flashprefix}/root/.ssh
	ln -s /var ${flashprefix}/root/usr
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libc.so.6 ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libcurl.so.2 ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libfx2.so ${flashprefix}/root/lib/
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libssl.so.0.9.7 ${flashprefix}/root/lib/
	ln -s libssl.so.0.9.7 ${flashprefix}/root/lib/libssl.so
	ln -s libssl.so.0.9.7 ${flashprefix}/root/lib/libssl.so.0
	install -m644 ${nemroot}/patch/enigma1/setting/lib/libcrypto.so.0.9.7 ${flashprefix}/root/var_init/lib/
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so.0
	ln -s /var/lib/libcrypto.so.0.9.7 ${flashprefix}/root/lib/libcrypto.so.0.9.7
	@if [ ! -e ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net ]; then \
			mkdir ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net; \
	fi
	install ${nemroot}/patch/enigma1/setting/lib/tun.ko ${flashprefix}/root/lib/modules/2.6.9/kernel/drivers/net/
endif

	ln -s /var/share/locale/ar ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/al ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/cs ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/da ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/de ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/el ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/es ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/et ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/fi ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/fr ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/he ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/hr ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/hu ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/is ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/lt ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/lv ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/nl ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/no ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/pl ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/pt ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/ro ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/ru ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/sk ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/sl ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/sr ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/sv ${flashprefix}/root/share/locale/
	ln -s /var/share/locale/tr ${flashprefix}/root/share/locale/
# /Nemesis

	cp $(buildprefix)/linux/arch/ppc/boot/images/zImage.treeboot $(flashprefix)/boot/root/platform/kernel/os
if BOXTYPE_DM7000
	$(flashprefix)/mkcramfs-e -eb $(flashprefix)/boot $(flashprefix)/boot-cramfs.img
else
# 	mv $(flashprefix)/boot/root/platform/kernel/bild .
	$(flashprefix)/mkcramfs-e -eb $(flashprefix)/boot $(flashprefix)/boot-cramfs.img
# 	mv ./bild $(flashprefix)/boot/root/platform/kernel
endif
	@if [ `stat -c %s $(flashprefix)/boot-cramfs.img` -gt 1179648 ]; then \
		echo "ERROR: CramFS part is too big for image (max. allowed 1179648 bytes)"; \
		rm -f $(flashprefix)/boot-cramfs.img.too-big 2> /dev/null || /bin/true; \
		mv $(flashprefix)/boot-cramfs.img $(flashprefix)/boot-cramfs.img.too-big; \
		exit 1; \
	fi

$(flashprefix)/root-squashfs.img:
if BOXTYPE_DM7000
	$(flashprefix)/mksquashfs $(flashprefix)/root $(flashprefix)/root-squashfs.img -be -all-root
	@if [ `stat -c %s $(flashprefix)/root-squashfs.img` -gt 5111808 ]; then \
		echo "ERROR: SquashFS part is too big for image (max. allowed 5111808 bytes)"; \
		rm -f $(flashprefix)/root-squashfs.img.too-big 2> /dev/null || /bin/true; \
		mv $(flashprefix)/root-squashfs.img $(flashprefix)/root-squashfs.img.too-big; \
		exit 1; \
	fi
else
	$(flashprefix)/mksquashfs $(flashprefix)/root $(flashprefix)/root-squashfs.img -be -all-root
	@if [ `stat -c %s $(flashprefix)/root-squashfs.img` -gt 4063232 ]; then \
		echo "ERROR: SquashFS part is too big for image (max. allowed 4063232 bytes)"; \
		rm -f $(flashprefix)/root-squashfs.img.too-big 2> /dev/null || /bin/true; \
		mv $(flashprefix)/root-squashfs.img $(flashprefix)/root-squashfs.img.too-big; \
		exit 1; \
	fi
endif

$(flashprefix)/complete.img: $(flashprefix)/boot-cramfs.img $(flashprefix)/root-squashfs.img
	cp $(flashprefix)/boot-cramfs.img $(flashprefix)/complete.img; \
	dd if=$(flashprefix)/root-squashfs.img of=$(flashprefix)/complete.img bs=1024 seek=1152
endif

#######################
#
#   internal
#

clean-local:
	-$(MAKE) -C etc clean
	-$(MAKE) -C $(appsdir) clean
	-$(MAKE) -C $(driverdir) clean \
		KERNEL_LOCATION=$(buildprefix)/linux
	-rm -f linux
	-rm -rf build
	-@CLEANUP@

distclean-local:
	-$(MAKE) -C etc distclean
	-$(MAKE) -C $(appsdir) distclean
	-rm Makefile-archive
	-rm .bootstrap
	-rm .directories .linuxdir .binutils .glibc .gcc .rpcgen
	-rm .root .busybox .ftpd .modutils .procps .procps3 .watchdog .mrouted
	-rm .libboost .libcommoncplusplus .libcrypto .libcurl .libdb2 .libdvbpsi .libevent .libffi .libfreetype
	-rm .libncurses .libfribidi .libjpeg .libmad .libid3tag .libnet .libnids .libpcap .libpng .libpopt
	-rm .libreadline .libsdl .libsigc .libungif .libupnp .libxml2 .libxmlwrapp .libz .libvorbisidec .libgmp
	-rm .cabextract
	-rm .gdb .gdb-remote .insight .ksymoops .ltrace .strace .nano
	-rm .kaffeh .kaffe
	-rm .gnuboy .scummvm .sdldoom .wxbase .amule .ctorrent
	-rm .linuxkernel .reiserfsprogs .e2fsprogs .samba .dreamfiles .etherwake .utillinux
	-rm .dvbsnoop .enigma .lcars .lcdmenu .neutrino .stream .zapit
	-rm .console_data .console_tools .dropbear .dsniff .fbset .lirc 
	-rm .polipo .pump .ssh .tcpdump .tor .ushare .xrc .thttpd
	-rm .dvbdate .dvbstream .dvbtext .dvbtune .vls .sqlite
	-rm .misc_libs .misc_tools .plugins .libtuxbox .tuxbox_libs .tuxbox_tools .tuxbox_tools_all
	-rm -rf $(targetprefix)
	-rm -rf $(hostprefix)
if TARGETRULESET_FLASH
	-rm -rf $(flashprefix)
endif

@ARCHIVE@

if MAINTAINER_MODE
Makefile-archive: $(top_srcdir)/rules-archive
	$(top_srcdir)/rules-archive.pl $(top_srcdir)/rules-archive > Makefile-archive
endif

if TARGETRULESET_FLASH
.PHONY: core libs root apps boot devel java copy lib compress
else
.PHONY: core libs root apps boot devel java
endif

EXTRA_DIST = \
	rules.pl rules-archive.pl \
	rules-archive \
	rules-install rules-install-flash \
	rules-make rules-make-cygwin

ACLOCAL_AMFLAGS = -I .

CONFIG_STATUS_DEPENDENCIES = \
	$(top_srcdir)/rules.pl \
	$(top_srcdir)/rules-install $(top_srcdir)/rules-install-flash \
	$(top_srcdir)/rules-make $(top_srcdir)/rules-make-cygwin \
	Makefile-archive

