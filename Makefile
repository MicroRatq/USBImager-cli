#
#  USBImager CLI Makefile
#
#  A specialized headless version optimized for automated workflows.
#  Derived from the original USBImager by bzt.
#

####### overall configuration #######

TARGET = usbimager-cli
CC ?= gcc
STRIP ?= strip
WINDRES ?= windres
CFLAGS = -Isrc -D_FILE_OFFSET_BITS=64 -D__USE_FILE_OFFSET64 -D__USE_LARGEFILE -Wall -Wextra -pedantic --std=c99 -O3 -fvisibility=hidden -DUSE_PHY=1
LDFLAGS =
LIBS =
OUT_DIR ?= build
VPATH = src

VERSION = $(shell cat src/main.h|grep USBIMAGER_VERSION|cut -d '"' -f 2)
SRC = iso_burner.c stream_minimal.c

####### detect operating system and platform #######

ifeq ($(OS),Windows_NT)
# Windows (mingw)
WIN = 1
SRC += disks_win.c
LDFLAGS += -static -static-libgcc
LIBS += -lsetupapi -lole32 -luser32 -lkernel32
TARGET := $(TARGET).exe
CFLAGS += -DNDEBUG -DWINVER=0x0500 -DUNICODE=1
WINDRES ?= windres
RESOURCE_OBJ = manifest.o
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
# MacOSX
MACOSX = 1
SRC += disks_darwin.m
override LDFLAGS += -framework CoreFoundation -framework IOKit -framework DiskArbitration -framework Foundation
else
# Linux
LINUX = 1
SRC += disks_linux.c
endif
endif

OBJ = $(addprefix $(OUT_DIR)/,$(patsubst %.m,%.o,$(SRC:.c=.o)))

####### rules to compile #######

all: $(TARGET)

$(OUT_DIR)/%.o: src/%.c
	@mkdir -p $(OUT_DIR) 2>/dev/null || mkdir $(OUT_DIR) 2>/dev/null || true
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUT_DIR)/%.o: src/%.m
	@mkdir -p $(OUT_DIR) 2>/dev/null || mkdir $(OUT_DIR) 2>/dev/null || true
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUT_DIR)/manifest.o: src/manifest.xml
	@mkdir -p $(OUT_DIR) 2>/dev/null || mkdir $(OUT_DIR) 2>/dev/null || true
	@echo Building manifest...
	@echo 1 24 "$<" | $(WINDRES) -o $@

$(TARGET): $(OBJ) $(if $(RESOURCE_OBJ),$(OUT_DIR)/$(RESOURCE_OBJ))
	@mkdir -p $(OUT_DIR) 2>/dev/null || mkdir $(OUT_DIR) 2>/dev/null || true
	$(CC) $(LDFLAGS) -o $(OUT_DIR)/$@ $(OBJ) $(if $(RESOURCE_OBJ),$(OUT_DIR)/$(RESOURCE_OBJ)) $(LIBS)
ifeq ($(DEBUG),)
	$(STRIP) $(OUT_DIR)/$@
endif

####### cleanup #######

clean:
	rm -f *.o src/*.o $(OUT_DIR)/*.o $(OUT_DIR)/$(TARGET) 2>/dev/null || true

distclean: clean
	rm -rf $(OUT_DIR) 2>/dev/null || true

####### package creation #######

package: $(TARGET)
	@mkdir -p $(OUT_DIR)/package 2>/dev/null || true
	@cp $(OUT_DIR)/$(TARGET) $(OUT_DIR)/package/
	@cp LICENSE $(OUT_DIR)/package/
	@cp README.md $(OUT_DIR)/package/
	@rm -f ./usbimager-cli_$(VERSION).zip 2>/dev/null || true
	cd $(OUT_DIR)/package && zip -r -9 ../usbimager-cli_$(VERSION).zip .
	@rm -rf $(OUT_DIR)/package

####### help #######

help:
	@printf "USBImager CLI Makefile\n"
	@printf "Targets:\n"
	@printf "    all      Compile the CLI tool to the build directory\n"
	@printf "    clean    Remove object files and the binary\n"
	@printf "    package  Create a ZIP package with the binary and documentation\n"

.PHONY: all clean distclean package help
