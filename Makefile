#
# Top-level Makefile for frdm-mntr. Not much to see, here.
#

.PHONY: clean install

# --------------------------------------------------------------

all:
	$(MAKE) -C src

# --------------------------------------------------------------

clean:
	$(MAKE) -C src clean

install:
	@echo "The install target really isn't here, yet. Sorry."
#	$(MAKE) -C src install
