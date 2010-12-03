FILES = rcS rcK start init1 rcS.7000 rcS.500 rcS.56x0

EXTRA_DIST = $(FILES)

all-local: $(FILES)

install-data-local: $(FILES)
	install -d $(targetprefix)/etc/init.d
	@for i in $(FILES) ; do \
		install -m755 $$i $(targetprefix)/etc/init.d ; \
	done

if BOXTYPE_DM7000
	install -m755 rcS.7000 $(targetprefix)/etc/init.d/rcS
	rm -f $(targetprefix)/etc/init.d/rcS.*
endif

if BOXTYPE_DM56x0
	install -m755 rcS.56x0 $(targetprefix)/etc/init.d/rcS
	rm -f $(targetprefix)/etc/init.d/rcS.*
endif

if BOXTYPE_DM500
	install -m755 rcS.500 $(targetprefix)/etc/init.d/rcS
	rm -f $(targetprefix)/etc/init.d/rcS.*
endif