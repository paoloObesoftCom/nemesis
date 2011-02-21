DESCRIPTION = "gstreamer-0.10.32  decoding DTS downmix dm8000/dm500hd/dm800se/dm7020hd"
HOMEPAGE = "http://www.genesi-project.it"
LICENSE = "GPL"

PV="1.0"
PR="r6"

DEPENDS += " libdca (>= 0.0.5) \
          openssl (>= 0.9.8j) \
          gst-plugin-dvbmediasink (>= 0.10+git20110210) \
          gstreamer (>= 0.10.32) \
          libxml2 (>= 2.7.3)"

SRC_URI = "file://libgstdvbaudiosinkext.so"

S = "${WORKDIR}/"
FILES_${PN} = "/"


do_install() {
	install -d "${D}/usr/lib/gstreamer-0.10"
	install -m 644 "${S}/libgstdvbaudiosinkext.so" "${D}/usr/lib/gstreamer-0.10/"
}
