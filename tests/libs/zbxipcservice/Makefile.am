include ../Makefile.include

BINARIES_tests = \
mprocess \
sprocess \
rtc_serv \
rtc_agt \
rtc_slv

noinst_PROGRAMS = $(BINARIES_tests)

IPC_COMPILER_FLAGS = \
        -I@top_srcdir@/tests \
        $(CMOCKA_CFLAGS)

IPC_LIBS = \
$(top_srcdir)/src/libs/zbxipcservice/libzbxipcservice.a \
$(top_srcdir)/src/libs/zbxserialize/libzbxserialize.a \
$(top_srcdir)/src/libs/zbxjson/libzbxjson.a \
$(top_srcdir)/src/libs/zbxcachehistory/libzbxcachehistory.a \
$(top_srcdir)/src/libs/zbxself/libzbxself.a \
$(top_srcdir)/src/libs/zbxdiag/libzbxdiag.a \
$(top_srcdir)/src/libs/zbxlog/libzbxlog.a \
$(top_srcdir)/src/libs/zbxthreads/libzbxthreads.a \
$(top_srcdir)/src/libs/zbxmutexs/libzbxmutexs.a \
$(top_srcdir)/src/libs/zbxprof/libzbxprof.a \
$(top_srcdir)/src/libs/zbxnix/libzbxnix.a \
$(top_srcdir)/src/libs/zbxalgo/libzbxalgo.a \
$(top_srcdir)/src/libs/zbxnum/libzbxnum.a \
$(top_srcdir)/src/libs/zbxtime/libzbxtime.a \
$(top_srcdir)/src/libs/zbxstr/libzbxstr.a \
$(top_srcdir)/src/libs/zbxcommon/libzbxcommon.a

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
