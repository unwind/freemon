#
# The best makefiles are hand-writtten.
#

PKGS	= vte-2.90

CFLAGS	= -Wall -ansi -pedantic -std=c99 -g $(shell pkg-config --cflags $(PKGS))

LDLIBS	= $(shell pkg-config --libs $(PKGS))

ALL	= frdm-mntr

.PHONY:	clean

# ----------------------------------------------------------------------

frdm-mntr:	action-upload.o frdm-mntr.o gui.o tty.o

# ----------------------------------------------------------------------

clean:
	rm -f *.o
