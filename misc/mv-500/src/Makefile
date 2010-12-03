include Makefile.head

.cpp.o:
	$(CXX)  $(CFLAGS) -c -o $@ $<

all: $(NAME).so epgidx multiepg mvccat

C2FLAGS = -g -Wall -mcpu=405 -msoft-float -mmultiple -mstring -meabi -pipe -Os -DHAVE_DREAMBOX_HARDWARE -I $(CDK_SRC)/driver/include -DHAVE_OST_DMX_H=1
CC = $(BOXBIN)/powerpc-tuxbox-linux-gnu-gcc

getepg.so: getepg.c
	$(CC) $(CFLAGS) $(LINK_OPTIONS) -o getepg.so  getepg.c
	$(STRIP) -s getepg.so

otvepg.o: otvepg.c otvepg.h
	$(CC) -c $(C2FLAGS) otvepg.c

multiepg: multiepg.c si_tables.h mhwepg.o mhwepg2.o otvepg.o
	$(CC) $(C2FLAGS) multiepg.c mhwepg.o mhwepg2.o otvepg.o -o multiepg
#	$(STRIP) -s multiepg

mhwepg2.o: mhwepg2.c mhwepg2.h si_tables.h
	$(CC) -c $(C2FLAGS) mhwepg2.c

mhwepg.o: mhwepg.c mhwepg.h si_tables.h
	$(CC)  -c $(C2FLAGS) mhwepg.c

epgidx: epgidx.c
	$(CXX) $(CFLAGS) epgidx.c -o epgidx
	$(STRIP) -s epgidx
#	gcc epgidx.c -o epgidx_loc

mv.o: mv-conf.cpp mv.cpp conf.h mv.h
	$(CXX)  $(CFLAGS) -c -o $@ mv.cpp

inputmanager.o: inputmanager.cpp epgui.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

mvccat: mvccat.o inputdefreader.o
	$(CXX) mvccat.o inputdefreader.o -o mvccat
	$(STRIP) -s mvccat

mvccat.o: mvccat.cpp 
	$(CXX) $(CFLAGS) -c -o $@ $<

$(NAME).so: $(OBJECTS)
	$(CXX) $(LINK_OPTIONS) -o $(NAME).so  $(OBJECTS)
	$(STRIP) -s $(NAME).so

install: all 
#	cp $(NAME).so extepg.so
	ncftpput -V -u root -p dreambox db.local /var/tuxbox/plugins/ $(NAME).so

include Makefile.tail
