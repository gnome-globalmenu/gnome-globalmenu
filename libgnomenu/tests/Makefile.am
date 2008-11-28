include $(top_srcdir)/Makefile.decl



VALASOURCES = test-parser.vala  test-menubar.vala
STAMP = $(VALASOURCES:.vala=.stamp)
SUFFIXES = .vala .stamp
.vala.stamp:
	$(VALAC) -C --vapidir=$(top_srcdir)/tests --vapidir=$(top_srcdir)/libgnomenu --pkg testman --pkg gtk+-2.0 --pkg gnomenu $^
	touch $@

TEST_PROGS +=  test-parser test-menubar
	
#	test-menushell test-parser test-menubar
noinst_PROGRAMS = $(TEST_PROGS)
AM_CPPFLAGS = $(GTK_CFLAGS) $(GLIB_CFLAGS)
BUILT_SOURCES = $(STAMP)
LDADD = $(GTK_LIBS) $(GLIB_LIBS) ../libgnomenu.la ../../tests/libtestman.la
EXTRA_DIST += $(STAMP) $(VALASOURCES)

