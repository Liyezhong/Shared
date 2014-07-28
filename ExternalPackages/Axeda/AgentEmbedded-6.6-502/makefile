# Specify target platform settings and set of features

# run 'make HOST=1' for Linux/i368 build
ifdef HOST
  # Linux/i368 settings
  PLATFORM = linux-i386
  CROSS_COMPILE =
  EXTRA_CFLAGS  = $(EXTRA_ARCH)
  EXTRA_LDFLAGS = $(EXTRA_ARCH)
else
  # E-Box settings
  PATH := /opt/poky/1.3.2/sysroots/i686-pokysdk-linux/usr/bin/armv6-vfp-poky-linux-gnueabi:$(PATH)
  PLATFORM =
  CROSS_COMPILE = arm-poky-linux-gnueabi-
  EXTRA_CFLAGS = -I/opt/poky/1.3.2/sysroots/armv6-vfp-poky-linux-gnueabi/usr/include
  EXTRA_LDFLAGS = -L/opt/poky/1.3.2/sysroots/armv6-vfp-poky-linux-gnueabi/usr/lib
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
#	-D__GATEWAY__				- "Gateway mode" support
#	-DENABLE_SSL				- SSL support (requires openssl)
#	-DENABLE_REMOTE_SESSION		- remote session support 
#	-DENABLE_FILE_TRANSFER		- file transfer support

#FEATURES = -D_REENTRANT -D__GATEWAY__ -DENABLE_REMOTE_SESSION -DENABLE_FILE_TRANSFER -DENABLE_SSL
FEATURES = -D_REENTRANT -DENABLE_REMOTE_SESSION -DENABLE_FILE_TRANSFER -DENABLE_SSL

TOP_DIR = $(shell pwd)
DEMO_DIR = $(TOP_DIR)/Demo
TEST_HARNESS_DIR = $(TOP_DIR)/AeTestHarness
LIB_SRC_DIR = $(TOP_DIR)/Libsrc
EXPAT_DIR = $(LIB_SRC_DIR)/expat
ZLIB_DIR = $(LIB_SRC_DIR)/zlib
LIB_SRC_SUBDIRS = $(EXPAT_DIR) $(ZLIB_DIR)

INCLUDE_DIRS = $(TOP_DIR) $(TOP_DIR)/Include $(TOP_DIR)/Compat $(EXPAT_DIR)/xmlparse $(ZLIB_DIR)
LIB_DIRS = $(TOP_DIR) $(EXPAT_DIR)/xmlparse $(ZLIB_DIR)
LIBS = -lexpat -lz

ifeq ($(BIG_ENDIAN),yes)
	CONFIG += -DAE_BIG_ENDIAN
endif

ifeq ($(HAVE_OPENSSL),yes)
	CONFIG += -DHAVE_OPENSSL
	LIBS += -lssl -lcrypto
	ifneq ($(OPENSSL_DIR),)
		INCLUDE_DIRS += $(OPENSSL_DIR)/include
		LIB_DIRS += $(OPENSSL_DIR)/lib
	endif
else
    LIBDES_DIR = $(LIB_SRC_DIR)/libdes
	LIB_SRC_SUBDIRS += $(LIBDES_DIR)
	INCLUDE_DIRS += $(LIBDES_DIR)
	LIB_DIRS += $(LIBDES_DIR)
	LIBS += -ldes
endif

ifeq ($(HAVE_PTHREADS),yes)
	CONFIG += -DHAVE_PTHREADS
	LIBS += -lpthread
endif

ifeq ($(ENABLE_LARGEFILE64),yes)
	FEATURES += -DENABLE_LARGEFILE64
	CONFIG += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
endif

ifeq ($(PLATFORM),sun-sparc)
	LIBS += -lsocket -lnsl
endif

LIBS += -lm -lc

EXTRA_CFLAGS += -I$(TOP_DIR)/Sysdeps/Unix
CFLAGS = -O2 -Wall $(INCLUDE_DIRS:%=-I%) $(EXTRA_CFLAGS) $(CONFIG) $(FEATURES)

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib

OBJS = AeAgentRestart.o AeContainers.o AeDRM.o AeDRMEMessage.o AeDRMQueue.o AeDRMSOAP.o	\
	AeInterface.o AeUtil.o AeWebAuth.o AeWebCommon.o AeWebConnection.o \
	AeWebConnectionSOCKS.o AeWebRequest.o AeWebTransaction.o AeWebUserAgent.o \
	AeXML.o \
	AeRemoteSession.o AeRemoteChannel.o AeRemoteDirectConnection.o \
	AeRemoteDirectTransport.o AeRemoteHTTPTransport.o AeRemoteDesktopProbe.o \
	AeFileTransfer.o AeFileDownload.o AeFileUpload.o AeGzip.o AeTar.o \
	AeNullFileFilter.o \
	Sysdeps/Unix/AeOS.o AeInitLocal.o \
	Compat/md4c.o Compat/md5c.o

DEPS = $(OBJS:%.o=%.d)

AE_LIB = $(TOP_DIR)/libAgentEmbedded.a

MAKE_FLAGS = CC=$(CC) AR=$(AR) RANLIB=$(RANLIB) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)"
LIB_MAKE_FLAGS = $(MAKE_FLAGS) BIG_ENDIAN=$(BIG_ENDIAN)
APP_MAKE_FLAGS = $(MAKE_FLAGS) PLATFORM=$(PLATFORM) CONFIG="$(CONFIG)" FEATURES="$(FEATURES)" INCLUDE_DIRS="$(INCLUDE_DIRS)" LIB_DIRS="$(LIB_DIRS)" LIBS="$(LIBS)" AE_LIB="$(AE_LIB)"

.PHONY: all clean libs demo test_harness

all: libs $(AE_LIB) demo test_harness 

clean:
	for n in $(LIB_SRC_SUBDIRS); do \
		$(MAKE) -C $$n $(LIB_MAKE_FLAGS) $@ || exit 1; \
	done
	rm -f $(OBJS) $(DEPS) $(AE_LIB)
	#$(MAKE) -C $(DEMO_DIR) $(APP_MAKE_FLAGS) $@
ifneq ($(wildcard $(TEST_HARNESS_DIR)),)
	$(MAKE) -C $(TEST_HARNESS_DIR) $(APP_MAKE_FLAGS) $@
endif

libs:
	for n in $(LIB_SRC_SUBDIRS); do \
		$(MAKE) -C $$n $(LIB_MAKE_FLAGS) all || exit 1; \
	done

$(AE_LIB): $(OBJS)
	$(AR) rc $@ $^

demo:
	#$(MAKE) -C $(DEMO_DIR) $(APP_MAKE_FLAGS) all

test_harness:
ifneq ($(wildcard $(TEST_HARNESS_DIR)),)
	$(MAKE) -C $(TEST_HARNESS_DIR) $(APP_MAKE_FLAGS) all
endif

.c.o:
	$(CC) -MMD -MP $(CFLAGS) -c -o $@ $<

-include $(DEPS)
