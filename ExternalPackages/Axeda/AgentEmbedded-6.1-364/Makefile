# $Id: Makefile,v 1.20.2.2 2010/08/13 16:01:39 dkhodos Exp $

# Specify target platform settings and set of features

# run 'make HOST=1' for Linux/i368 build
ifdef HOST
  # Linux/i368 settings
  PLATFORM = linux-i386
  CROSS_COMPILE =
  EXTRA_CFLAGS =
  EXTRA_LDFLAGS =
else
  # E-Box settings
  PATH := /opt/poky/1.2/sysroots/i686-pokysdk-linux/usr/bin/armv6-vfp-poky-linux-gnueabi:$(PATH)
  PLATFORM = 
  CROSS_COMPILE = arm-poky-linux-gnueabi-
  EXTRA_CFLAGS = -I/opt/poky/1.2/sysroots/armv6-vfp-poky-linux-gnueabi/usr/include 
  EXTRA_LDFLAGS = -L/opt/poky/1.2/sysroots/armv6-vfp-poky-linux-gnueabi/usr/lib
endif
BIG_ENDIAN = no
HAVE_OPENSSL = yes
HAVE_PTHREADS = yes
ENABLE_LARGEFILE64 = no


# uClinux/m68k-pic-coff settings
#PLATFORM = uClinux-m68k-coff
#CROSS_COMPILE = m68k-pic-coff-
#EXTRA_CFLAGS = -fPIC -fomit-frame-pointer
#EXTRA_LDFLAGS = -L/opt/uClinux/m68k-pic-coff/lib
#BIG_ENDIAN = yes
#HAVE_OPENSSL = no
#HAVE_PTHREADS = no
#ENABLE_LARGEFILE64 = no

# uClinux/m68k-pic-elf settings
#PLATFORM = uClinux-m68k-elf
#CROSS_COMPILE = m68k-elf-
#EXTRA_CFLAGS = -fomit-frame-pointer -msep-data -m68000 -Dm68k
#EXTRA_LDFLAGS = -m68000 -Wl,-elf2flt -Wl,-move-rodata
#BIG_ENDIAN = yes
#HAVE_OPENSSL = no
#HAVE_PTHREADS = no
#ENABLE_LARGEFILE64 = no

# Solaris/SPARC settings
#PLATFORM = sun-sparc
#CROSS_COMPILE =
#EXTRA_CFLAGS =
#EXTRA_LDFLAGS =
#BIG_ENDIAN = yes
#HAVE_OPENSSL = no
#HAVE_PTHREADS = yes
#ENABLE_LARGEFILE64 = yes

# The following features are available:
#	-D_REENTRANT				- MT support
#	-DENABLE_SSL				- SSL support (requires openssl)
#	-DENABLE_REMOTE_SESSION		- remote session support 
#	-DENABLE_FILE_TRANSFER		- file transfer support

#FEATURES = -D_REENTRANT -D__GATEWAY__ -DENABLE_REMOTE_SESSION -DENABLE_FILE_TRANSFER -DENABLE_SSL
FEATURES = -D_REENTRANT -DENABLE_REMOTE_SESSION -DENABLE_FILE_TRANSFER -DENABLE_SSL

CONFIG =
ifeq ($(BIG_ENDIAN),yes)
	CONFIG += -DAE_BIG_ENDIAN
endif
ifeq ($(HAVE_OPENSSL),yes)
	CONFIG += -DHAVE_OPENSSL
endif
ifeq ($(HAVE_PTHREADS),yes)
	CONFIG += -DHAVE_PTHREADS
endif
ifeq ($(ENABLE_LARGEFILE64),yes)
	CONFIG += -DENABLE_LARGEFILE64
	CONFIG += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
endif

TOP_DIR = $(shell pwd)
AE_SRC_DIR = Cobf
LIB_SRC_DIR = Libsrc
LIB_DIR = lib
DEMO_DIR = Demo
LIB_SRC_DIRS = $(LIB_SRC_DIR)/expat $(LIB_SRC_DIR)/zlib
ifneq ($(HAVE_OPENSSL),yes)
	LIB_SRC_DIRS += $(LIB_SRC_DIR)/libdes
endif
SUBDIRS = $(LIB_SRC_DIRS) $(DEMO_DIR)
EXTRA_CFLAGS +=  -I$(TOP_DIR)/Sysdeps/Unix
CFLAGS = -O2 -Wall -I. -I$(TOP_DIR)/$(AE_SRC_DIR) -I$(TOP_DIR)/Include -I$(TOP_DIR)/Compat -I$(TOP_DIR)/Sysdeps/Unix -I$(LIB_DIR)/include $(EXTRA_CFLAGS) $(CONFIG) $(FEATURES)
LDFLAGS = $(EXTRA_LDFLAGS) -L$(LIB_DIR)/lib

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib

OBJS = a0.o a1.o a2.o a3.o a4.o a5.o a6.o a7.o a8.o a9.o a10.o a11.o a12.o a13.o a14.o a15.o a16.o a17.o a18.o a19.o a20.o a21.o a22.o a23.o a24.o a25.o a26.o a27.o a28.o AeOS.o

COMPAT_OBJS = md4c.o md5c.o

VPATH = $(AE_SRC_DIR):Compat:Sysdeps/Unix

MAKE_FLAGS = CC=$(CC) AR=$(AR) RANLIB=$(RANLIB) TOP_DIR=$(TOP_DIR) PLATFORM=$(PLATFORM) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)" BIG_ENDIAN=$(BIG_ENDIAN) HAVE_OPENSSL=$(HAVE_OPENSSL) HAVE_PTHREADS=$(HAVE_PTHREADS) ENABLE_LARGEFILE64=$(ENABLE_LARGEFILE64)

AE_LIB = libAgentEmbedded.a

.PHONY: all clean distclean

all:
	for n in $(LIB_SRC_DIRS); do \
		$(MAKE) -C $$n $(EXTRA_CFLAGS) $(MAKE_FLAGS) install || exit 1; \
	done
	$(CC) -MM $(CFLAGS) $(AE_SRC_DIR)/*.c > .dep
	$(MAKE) -C . $(AE_LIB)
	$(MAKE) -C $(DEMO_DIR) $(MAKE_FLAGS) all

clean:
	rm -f *.o *.a .dep
	rm -rf $(LIB_DIR)
	$(MAKE) -C $(DEMO_DIR) $(MAKE_FLAGS) $@

distclean: clean
	for n in $(LIB_SRC_DIRS); do \
		$(MAKE) -C $$n $(MAKE_FLAGS) $@ || exit 1; \
	done

$(AE_LIB): $(OBJS) $(COMPAT_OBJS)
	$(AR) rc $@ $^

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

ifeq ($(MAKECMDGOALS), $(AE_LIB))
-include .dep
endif
