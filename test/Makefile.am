
AM_CFLAGS = -I$(top_srcdir) \
	$(TEST_SUITE_CFLAGS)

LDADDS = $(TEST_SUITE_LIBS) $(top_builddir)/libgnomenu/libgnomenu.la

noinst_PROGRAMS = testmenubars testintro
#testsocket testserver testclient testboth testmenu testmenubars testtools

testsocket_SOURCES = testsocket.c
testsocket_LDADD = $(LDADDS)
testtools_SOURCES = testtools.c
testtools_LDADD = $(LDADDS)
testserver_SOURCES = testserver.c
testserver_LDADD = $(LDADDS)
testclient_SOURCES = testclient.c
testclient_LDADD = $(LDADDS)
testboth_SOURCES = testboth.c
testboth_LDADD = $(LDADDS)
testmenu_SOURCES = testmenu.c
testmenu_LDADD = $(LDADDS)
testmenubars_SOURCES = testmenubars.c
testmenubars_LDADD = $(LDADDS)
testintro_SOURCES = testintro.c
testintro_LDADD = $(LDADDS)
