include ../Makefile.include

BINARIES_tests = \
mprocess \
sprocess \
rtc_serv \
rtc_agt \
rtc_slv

noinst_PROGRAMS = $(BINARIES_tests)
noinst_LIBRARIES = libzbxipcservice_test.a

IPC_COMPILER_FLAGS = \
        -I$(srcdir)/nlib/include \
        -I$(srcdir)/nlib/include/common \
        $(CMOCKA_CFLAGS)

IPC_LIBS = \
$(builddir)/libzbxipcservice_test.a

libzbxipcservice_test_a_SOURCES = \
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

libzbxipcservice_test_a_CFLAGS = \
	$(LIBEVENT_CFLAGS)

mprocess_SOURCES = \
       mprocess.c
mprocess_LDADD = \
        $(IPC_LIBS)
mprocess_LDADD += @SERVER_LIBS@
mprocess_LDFLAGS = @SERVER_LDFLAGS@ $(CMOCKA_LDFLAGS)
mprocess_CFLAGS = $(IPC_COMPILER_FLAGS)

sprocess_SOURCES = \
       sprocess.c
sprocess_LDADD = \
$(IPC_LIBS)
sprocess_LDADD += @SERVER_LIBS@
sprocess_LDFLAGS = @SERVER_LDFLAGS@ $(CMOCKA_LDFLAGS)
sprocess_CFLAGS = $(IPC_COMPILER_FLAGS)

rtc_serv_SOURCES = \
rtc_serv.c \
rtc_service.c \
rtc_client.c \
rtc.c \
rtc.h
rtc_serv_LDADD = \
$(IPC_LIBS)
rtc_serv_LDADD += @SERVER_LIBS@
rtc_serv_LDFLAGS = @SERVER_LDFLAGS@ $(CMOCKA_LDFLAGS)
rtc_serv_CFLAGS = $(IPC_COMPILER_FLAGS)

rtc_agt_SOURCES = \
rtc_agt.c \
rtc_client.c \
rtc.c \
rtc.h
rtc_agt_LDADD = \
$(IPC_LIBS)
rtc_agt_LDADD += @SERVER_LIBS@
rtc_agt_LDFLAGS = @SERVER_LDFLAGS@ $(CMOCKA_LDFLAGS)
rtc_agt_CFLAGS = $(IPC_COMPILER_FLAGS)

rtc_slv_SOURCES = \
rtc_slv.c \
rtc_client.c \
rtc.c \
rtc.h
rtc_slv_LDADD = \
$(IPC_LIBS)
rtc_slv_LDADD += @SERVER_LIBS@
rtc_slv_LDFLAGS = @SERVER_LDFLAGS@ $(CMOCKA_LDFLAGS)
rtc_slv_CFLAGS = $(IPC_COMPILER_FLAGS)
