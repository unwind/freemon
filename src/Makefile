#
# The best makefiles are hand-writtten.
#

PKGS	= vte-2.90

CFLAGS	= -Wall -ansi -pedantic -std=c99 $(shell pkg-config --cflags $(PKGS))

LDLIBS	= $(shell pkg-config --libs $(PKGS))

ALL	= frdm-mntr

.PHONY:	clean

# ----------------------------------------------------------------------

frdm-mntr:	frdm-mntr.o gui.o

# ----------------------------------------------------------------------

clean:
	rm -f *.o
