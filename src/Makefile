#
# The best makefiles are hand-writtten.
#

PKGS	= vte-2.90

CFLAGS	= -Wall -ansi -pedantic -std=c99 -g $(shell pkg-config --cflags $(PKGS))

LDLIBS	= $(shell pkg-config --libs $(PKGS))

BINARY	= freemon

ALL	= $(BINARY)

.PHONY:	clean

# ----------------------------------------------------------------------

freemon:	action-upload.o autodetect.o boardid.o $(BINARY).o gui.o target.o tty.o tty-fifo.o tty-serial.o

autodetect.o:	autodetect.c autodetect-linux.c

# ----------------------------------------------------------------------

clean:
	rm -f *.o $(BINARY)
