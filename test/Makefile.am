
INCLUDES = -I$(top_srcdir) \
	$(TEST_SUITE_CFLAGS)

LDADDS = $(top_builddir)/libgnomenu/libgnomenu.la

noinst_PROGRAMS = testsocket testserver testclient testclisvr testmenu

testsocket_SOURCES = testsocket.c
testsocket_LDADD = $(LDADDS)
testserver_SOURCES = testserver.c
testserver_LDADD = $(LDADDS)
testclient_SOURCES = testclient.c
testclient_LDADD = $(LDADDS)
testclisvr_SOURCES = testclisvr.c
testclisvr_LDADD = $(LDADDS)
testmenu_SOURCES = testmenu.c
testmenu_LDADD = $(LDADDS)
