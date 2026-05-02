#
# USBImager CLI Root Makefile
#

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

package:
	$(MAKE) -C src package

help:
	$(MAKE) -C src help

.PHONY: all clean package help
