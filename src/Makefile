#
# The best makefiles are hand-writtten.
#

DEFS	= -DVERSION=\"1.2\"

PKGS	= vte-2.90

CFLAGS	= -Wall -ansi -pedantic -std=c99 -g $(shell pkg-config --cflags $(PKGS)) $(DEFS)

LDLIBS	= $(shell pkg-config --libs $(PKGS))

BINARY	= freemon

ALL	= $(BINARY)

.PHONY:	clean

# ----------------------------------------------------------------------

freemon:	action-about.o action-config.o action-upload.o autodetect.o boardid.o config.o $(BINARY).o gui.o target.o tty.o

autodetect.o:	autodetect.c autodetect-linux.c

# ----------------------------------------------------------------------

clean:
	rm -f *.o $(BINARY)
