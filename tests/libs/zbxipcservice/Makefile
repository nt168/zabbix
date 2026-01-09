# Portable Makefile for tests/libs/zbxipcservice
#
# Design goals:
#   - No dependency on the parent Zabbix source/build tree (no top_srcdir/top_builddir, no ../../.. includes).
#   - Build uses only sources/headers inside THIS directory (plus system libraries).
#   - You can copy this whole folder anywhere and run `make`.
#
# System deps (Ubuntu/Debian example):
#   sudo apt-get install build-essential libevent-dev libsqlite3-dev zlib1g-dev libcmocka-dev
#
# Build outputs (in current dir):
#   ./mprocess ./sprocess ./rtc_serv ./rtc_agt ./rtc_slv
#
# Optional:
#   make WITH_CMOCKA=0          # if you don't have libcmocka installed
#   make clean

CC      ?= gcc
AR      ?= ar
RM      ?= rm -f
MKDIR_P ?= mkdir -p

BUILD   ?= build
OBJDIR  := $(BUILD)/obj
LIBDIR  := $(BUILD)/lib

# --- Flags ------------------------------------------------------------------
CSTD    ?= -std=gnu11
CFLAGS  ?= -g -O2 -DHAVE_SYS_UTSNAME_H=1
WARN    ?= -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers

CPPFLAGS ?=
CPPFLAGS += -D_GNU_SOURCE -DHAVE_CONFIG_H
CPPFLAGS += -I. -Iipc -Inlib/include -Inlib/include/common

LDFLAGS ?=

# --- System libraries --------------------------------------------------------
# (override any of these from the command line if needed)
WITH_CMOCKA   ?= 1
SQLITE_LIBS   ?= -lsqlite3
ZLIB_LIBS     ?= -lz
PTHREAD_LIBS  ?= -pthread
LIBEVENT_LIBS ?= -levent_core -levent_extra -levent_pthreads
CMOCKA_LIBS   ?= -lcmocka

LDLIBS_BASE := $(SQLITE_LIBS) $(ZLIB_LIBS) $(LIBEVENT_LIBS) $(PTHREAD_LIBS)
ifeq ($(WITH_CMOCKA),1)
  LDLIBS_BASE += $(CMOCKA_LIBS)
endif

# --- Static library: libzbxipcservice_test.a ---------------------------------
LIBZBXIPC_A := $(LIBDIR)/libzbxipcservice_test.a

LIBZBXIPC_SRC := \
	ipc/ipcservice.c \
	nlib/src/zbxalgo/algodefs.c \
	nlib/src/zbxalgo/binaryheap.c \
	nlib/src/zbxalgo/hashmap.c \
	nlib/src/zbxalgo/hashset.c \
	nlib/src/zbxalgo/int128.c \
	nlib/src/zbxalgo/linked_list.c \
	nlib/src/zbxalgo/prediction.c \
	nlib/src/zbxalgo/queue.c \
	nlib/src/zbxalgo/vector.c \
	nlib/src/zbxcommon/common_log.c \
	nlib/src/zbxcommon/common_str.c \
	nlib/src/zbxcommon/components_strings_representations.c \
	nlib/src/zbxcommon/cuid.c \
	nlib/src/zbxcommon/libc_wrappers.c \
	nlib/src/zbxcommon/misc.c \
	nlib/src/zbxcommon/setproctitle.c \
	nlib/src/zbxnum/num.c \
	nlib/src/zbxprof/prof.c \
	nlib/src/zbxstr/str.c \
	nlib/src/zbxtime/time.c

LIBZBXIPC_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(LIBZBXIPC_SRC))

# --- Programs ----------------------------------------------------------------
BINARIES := mprocess sprocess rtc_serv rtc_agt rtc_slv

MPROCESS_SRC := mprocess.c
SPROCESS_SRC := sprocess.c

RTC_COMMON_SRC := rtc.c rtc_client.c
RTC_SERV_SRC   := rtc_serv.c rtc_service.c $(RTC_COMMON_SRC)
RTC_AGT_SRC    := rtc_agt.c $(RTC_COMMON_SRC)
RTC_SLV_SRC    := rtc_slv.c $(RTC_COMMON_SRC)

MPROCESS_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(MPROCESS_SRC))
SPROCESS_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(SPROCESS_SRC))
RTC_SERV_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(RTC_SERV_SRC))
RTC_AGT_OBJ  := $(patsubst %.c,$(OBJDIR)/%.o,$(RTC_AGT_SRC))
RTC_SLV_OBJ  := $(patsubst %.c,$(OBJDIR)/%.o,$(RTC_SLV_SRC))

DEPS := \
	$(LIBZBXIPC_OBJ:.o=.d) \
	$(MPROCESS_OBJ:.o=.d) \
	$(SPROCESS_OBJ:.o=.d) \
	$(RTC_SERV_OBJ:.o=.d) \
	$(RTC_AGT_OBJ:.o=.d) \
	$(RTC_SLV_OBJ:.o=.d)

.PHONY: all clean distclean print-vars
all: $(BINARIES)

# --- Build rules -------------------------------------------------------------

# Compile any .c in this tree into $(OBJDIR)/... keeping the same subdir layout.
$(OBJDIR)/%.o: %.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CSTD) $(CFLAGS) $(WARN) -MMD -MP -c $< -o $@

# Archive the static library
$(LIBZBXIPC_A): $(LIBZBXIPC_OBJ)
	@$(MKDIR_P) $(dir $@)
	$(AR) rcs $@ $^

# Link programs
mprocess: $(MPROCESS_OBJ) $(LIBZBXIPC_A)
	$(CC) $(CSTD) $(CFLAGS) $(WARN) $(LDFLAGS) -o $@ $(MPROCESS_OBJ) $(LIBZBXIPC_A) $(LDLIBS_BASE)

sprocess: $(SPROCESS_OBJ) $(LIBZBXIPC_A)
	$(CC) $(CSTD) $(CFLAGS) $(WARN) $(LDFLAGS) -o $@ $(SPROCESS_OBJ) $(LIBZBXIPC_A) $(LDLIBS_BASE)

rtc_serv: $(RTC_SERV_OBJ) $(LIBZBXIPC_A)
	$(CC) $(CSTD) $(CFLAGS) $(WARN) $(LDFLAGS) -o $@ $(RTC_SERV_OBJ) $(LIBZBXIPC_A) $(LDLIBS_BASE)

rtc_agt: $(RTC_AGT_OBJ) $(LIBZBXIPC_A)
	$(CC) $(CSTD) $(CFLAGS) $(WARN) $(LDFLAGS) -o $@ $(RTC_AGT_OBJ) $(LIBZBXIPC_A) $(LDLIBS_BASE)

rtc_slv: $(RTC_SLV_OBJ) $(LIBZBXIPC_A)
	$(CC) $(CSTD) $(CFLAGS) $(WARN) $(LDFLAGS) -o $@ $(RTC_SLV_OBJ) $(LIBZBXIPC_A) $(LDLIBS_BASE)

-include $(DEPS)

# --- Housekeeping ------------------------------------------------------------
clean:
	$(RM) -r $(BUILD) *.o *.a *.d
	$(RM) $(BINARIES)

distclean: clean

print-vars:
	@echo "CC=$(CC)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "CPPFLAGS=$(CPPFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo "LDLIBS_BASE=$(LDLIBS_BASE)"

