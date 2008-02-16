
INCLUDES = -I$(top_srcdir) \
	$(TEST_SUITE_CFLAGS)

LDADDS = $(top_builddir)/libgnomenu/libgnomenu.la

noinst_PROGRAMS = testsocket testserver testclient testboth
#testmenu

testsocket_SOURCES = testsocket.c
testsocket_LDADD = $(LDADDS)
testserver_SOURCES = testserver.c
testserver_LDADD = $(LDADDS)
testclient_SOURCES = testclient.c
testclient_LDADD = $(LDADDS)
testboth_SOURCES = testboth.c
testboth_LDADD = $(LDADDS)
testmenu_SOURCES = testmenu.c
testmenu_LDADD = $(LDADDS)
