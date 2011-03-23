NEMESISD_OBJ = nemesisd.o
NEMESISC_OBJ = nemesisc.o

NEMESISD_BIN = nemesisd
NEMESISC_BIN = nemesisc

NEMESISD_CFG = nemesis.cfg

all: clean $(NEMESISD_BIN) $(NEMESISC_BIN)

$(NEMESISD_OBJ):
	$(CC) $(CFLAGS) -c -o $@ $(@:.o=.c)

$(NEMESISC_OBJ):
	$(CC) $(CFLAGS) -c -o $@ $(@:.o=.c)

$(NEMESISD_BIN): $(NEMESISD_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(NEMESISD_OBJ)
	$(STRIP) $@

$(NEMESISC_BIN): $(NEMESISC_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(NEMESISC_OBJ)
	$(STRIP) $@

clean:
	rm -f $(NEMESISC_BIN) $(NEMESISD_BIN) *.o

install:
	install -d $(D)/etc
	install -d $(D)/usr/bin
	install -m 644 $(NEMESISD_CFG) $(D)/etc
	install -m 755 $(NEMESISD_BIN) $(D)/usr/bin
	install -m 755 $(NEMESISC_BIN) $(D)/usr/bin

