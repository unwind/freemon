#
# The best makefiles are hand-writtten.
#

PKGS	= vte-2.90

CFLAGS	= -Wall -ansi -pedantic -std=c99 -g $(shell pkg-config --cflags $(PKGS))

LDLIBS	= $(shell pkg-config --libs $(PKGS))

ALL	= freemon

.PHONY:	clean

# ----------------------------------------------------------------------

freemon:	action-connect.o action-upload.o autodetect.o freemon.o gui.o target.o tty.o tty-fifo.o tty-serial.o

autodetect.o:	autodetect.c autodetect-linux.c

# ----------------------------------------------------------------------

clean:
	rm -f *.o
