KDIR = /lib/modules/`uname -r`/build
TARGETS = control_station soldier
SOURCES = $(TARGETS:=.c)

all: kbuild $(TARGETS)

kbuild:
	make -C $(KDIR) M=`pwd`

$(TARGETS): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $@.c

clean:
	rm -f $(TARGETS)
	make -C $(KDIR) M=`pwd` clean

