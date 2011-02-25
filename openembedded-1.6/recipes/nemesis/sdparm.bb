1DESCRIPTION = "The sdparm utility accesses SCSI device parameters"
HOMEPAGE = "http://sg.danny.cz/sg/sdparm.html"

PV="1.0.6"
PR="r0"

SRC_URI = "http://sg.danny.cz/sg/p/sdparm-1.06.tgz"
S = "${WORKDIR}/sdparm-1.06"

inherit autotools_stage
