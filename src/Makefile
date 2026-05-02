#
#  usbimager/Makefile (CLI Version)
#
#  A specialized headless version optimized for automated workflows.
#  Derived from the original USBImager by bzt.
#

####### overall configuration #######

TARGET = usbimager-cli
CC = gcc
LD = gcc
STRIP = strip
CFLAGS = -D_FILE_OFFSET_BITS=64 -D__USE_FILE_OFFSET64 -D__USE_LARGEFILE -Wall -Wextra -pedantic --std=c99 -O3 -fvisibility=hidden -DUSE_PHY=1
LDFLAGS =
LIBS =
OUT_DIR ?= ../build

VERSION = $(shell cat main.h|grep USBIMAGER_VERSION|cut -d '"' -f 2)
SRC = iso_burner.c stream_minimal.c
OBJ = $(SRC:.c=.o)

####### detect operating system and platform #######

ifneq ("$(wildcard /bin/*.exe)","")
# Windows (mingw)
WIN = 1
SRC += disks_win.c
LDFLAGS += -static -static-libgcc
LIBS += -lsetupapi -lole32 -luser32 -lkernel32
TARGET := $(TARGET).exe
CFLAGS += -DNDEBUG -DWINVER=0x0500 -DUNICODE=1
else
ifneq ("$(wildcard /Library/Frameworks)","")
# MacOSX
MACOSX = 1
SRC += disks_darwin.c
LDFLAGS += -framework CoreFoundation -framework IOKit -framework DiskArbitration
LD = ld
else
# Linux
LINUX = 1
SRC += disks_linux.c
endif
endif

OBJ += $(SRC:.c=.o)
OBJ := $(sort $(OBJ))

####### rules to compile #######

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJ)
	@mkdir -p $(OUT_DIR) 2>/dev/null || mkdir $(OUT_DIR) 2>/dev/null || true
	$(LD) $(LDFLAGS) -o $(OUT_DIR)/$@ $(OBJ) $(LIBS)
ifeq ($(DEBUG),)
	$(STRIP) $(OUT_DIR)/$@
endif

####### cleanup #######

clean:
	rm -f *.o $(OUT_DIR)/$(TARGET) 2>/dev/null || true

distclean: clean
	rm -rf $(OUT_DIR) 2>/dev/null || true

####### package creation #######

package: $(TARGET)
	@mkdir -p $(OUT_DIR)/package 2>/dev/null || true
	@cp $(OUT_DIR)/$(TARGET) $(OUT_DIR)/package/
	@cp ../LICENSE $(OUT_DIR)/package/
	@cp ../README.md $(OUT_DIR)/package/
	@rm -f ../usbimager-cli_$(VERSION).zip 2>/dev/null || true
	cd $(OUT_DIR)/package && zip -r -9 ../../usbimager-cli_$(VERSION).zip .
	@rm -rf $(OUT_DIR)/package

####### help #######

help:
	@printf "USBImager CLI Makefile\n"
	@printf "Targets:\n"
	@printf "    all      Compile the CLI tool to the build directory\n"
	@printf "    clean    Remove object files and the binary\n"
	@printf "    package  Create a ZIP package with the binary and documentation\n"
