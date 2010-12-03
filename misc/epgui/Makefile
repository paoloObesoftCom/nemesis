VERSIONE = 1.0

CDK_SRC = /e2/cvs/dm7000
CDKROOT = /e2/cvs/dm7000/root/cdkroot
CDKBIN = /e2/cvs/dm7000/root/cdk/bin
CXX = $(CDKBIN)/powerpc-tuxbox-linux-gnu-g++
GCC = $(CDKBIN)/powerpc-tuxbox-linux-gnu-gcc
STRIP = $(CDKBIN)/powerpc-tuxbox-linux-gnu-strip

INCLUDES = -I$(CDK_SRC)/apps/tuxbox/enigma/include -I$(CDKROOT)/include -I$(CDKROOT)/include/sigc++-1.2 -I$(CDKROOT)/lib/sigc++-1.2/include -I$(CDKROOT)/include/freetype2 -I$(CDK_SRC)/apps/tuxbox/enigma -I$(CDK_SRC)/apps/misc/libs/libxmltree -I$(CDK_SRC)/apps/tuxbox/plugins/include -I$(CDK_SRC)/apps/tuxbox/enigma/src -I$(CDK_SRC)/driver/include -I$(CDK_SRC)/apps/misc/libs/libmd5sum -I$(CDK_SRC)/apps/tuxbox/plugins/include/bits
# a
# CFLAGS = -DEMU_PLUGIN $(INCLUDES) -fno-rtti -fno-exceptions -Wall -O2 -mcpu=405 -msoft-float -mmultiple -mstring -meabi -g3 -gdwarf-2 -pipe
# b
CFLAGS = -DEMU_PLUGIN $(INCLUDES) -fno-rtti -fno-exceptions -Wall -O2 -mcpu=405 -msoft-float -mmultiple -mstring -g -ggdb3 -pipe

LINK_OPTIONS = -shared -Wall -O2 -mcpu=405 -msoft-float -mmultiple -mstring -g -ggdb3 -pipe

PLUGIN = epgui.so
OBJECTS = epgui.o

all: $(PLUGIN)
$(PLUGIN): $(OBJECTS)
	$(CXX) $(LINK_OPTIONS) -o $(PLUGIN) $(OBJECTS)
	$(STRIP) -s $(PLUGIN)
	
%.o: %.cpp 
	$(CXX) $(CFLAGS) -c $<

clean:
	-rm -rf *.o
	-rm -rf *.so
