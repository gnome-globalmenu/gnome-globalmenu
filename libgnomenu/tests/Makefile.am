include $(top_srcdir)/Makefile.decl



VALASOURCES = test-parser.vala  test-menubar.vala test-window.vala
STAMP = $(VALASOURCES:.vala=.stamp)
SUFFIXES = .vala .stamp
.vala.stamp:
	$(VALAC) -C --vapidir=$(top_srcdir)/tests --vapidir=$(top_srcdir)/libgnomenu --pkg testman --pkg gtk+-2.0 --pkg gnomenu-private $^
	touch $@

TEST_PROGS +=  $(VALASOURCES:.vala=)
	
#	test-menushell test-parser test-menubar
noinst_PROGRAMS = $(TEST_PROGS)
AM_CPPFLAGS = -I$(top_srcdir) -DSRCDIR=\"$(srcdir)\" $(GTK_CFLAGS) $(GLIB_CFLAGS)
BUILT_SOURCES = $(STAMP)
LDADD = $(GTK_LIBS) $(GLIB_LIBS) ../libgnomenu.la ../../tests/libtestman.la
# default rules doesn't include the .h files!
EXTRA_DIST += $(STAMP) $(VALASOURCES) $(VALASOURCES:.vala=.h) evo-nolabel.xml evo.xml

