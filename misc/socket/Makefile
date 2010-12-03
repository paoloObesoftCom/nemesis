VERSIONE = 1.0

CDK_SRC=/e2/cvs/dm7000
CDKBIN=/e2/cvs/dm7000/root/cdk/bin
CDKROOT=/e2/cvs/dm7000/root/cdkroot
GCC=${CDKBIN}/powerpc-tuxbox-linux-gnu-g++
STRIP=${CDKBIN}/powerpc-tuxbox-linux-gnu-strip

INCLUDES="-I${CDK_SRC}/apps/tuxbox/enigma/include -I${CDK_SRC}/apps/tuxbox/enigma -I${CDKROOT}/include/sigc++-1.2 -I${CDKROOT}/lib/sigc++-1.2/include -I${CDKROOT}/include/freetype2 -I${CDK_SRC}/apps/tuxbox/enigma/include -I${CDK_SRC}/apps/tuxbox/enigma -I${CDK_SRC}/apps/misc/libs/libxmltree -I${CDK_SRC}/apps/tuxbox/plugins/include -I${CDK_SRC}/apps/tuxbox/enigma/src -I${CDK_SRC}/driver/include -I${CDK_SRC}/apps/misc/libs/libmd5sum"


CFLAGS = -fno-rtti -fno-exceptions -Wall -O2 -mcpu=405 -msoft-float -mmultiple -mstring -meabi -g3 -gdwarf-2 -pipe


all: nemesisd nemesisc


nemesisd: nemesisd.c 
	$(GCC) $(CFLAGS) -o nemesisd nemesisd.c
	$(STRIP) -s nemesisd

nemesisc: nemesisc.c 
	$(GCC) $(CFLAGS) -o nemesisc nemesisc.c
	$(STRIP) -s nemesisc

clean:
	-rm -rf nemesisc
	-rm -rf nemesisd

