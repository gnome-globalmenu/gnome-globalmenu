
AM_CFLAGS = -I$(top_srcdir) \
	$(TEST_SUITE_CFLAGS) -g
AM_LDFLAGS = -g
LDADDS = $(TEST_SUITE_LIBS) $(top_builddir)/libgnomenu/libgnomenu.la

noinst_PROGRAMS = testmenubars testintro testtools testglobal testmenubar testgrab

testsocket_SOURCES = testsocket.c
testsocket_LDADD = $(LDADDS)
testtools_SOURCES = testtools.c
testtools_LDADD = $(LDADDS)
testglobal_SOURCES = testglobal.c
testglobal_LDADD = $(LDADDS)
testmenubar_SOURCES = testmenubar.c
testmenubar_LDADD = $(LDADDS)
testmenubars_SOURCES = testmenubars.c
testmenubars_LDADD = $(LDADDS)
testintro_SOURCES = testintro.c
testintro_LDADD = $(LDADDS)
testgrab_SOURCES = testgrab.c
testgrab_LDADD = $(LDADDS)
