include $(top_srcdir)/Makefile.decl

if ENABLE_TESTS
TEST_PROGS +=  $(noinst_PROGRAMS)
noinst_PROGRAMS =$(VALASOURCES:.vala=)
endif

VALASOURCES = test-label.vala test-grab.vala test-parser.vala  test-menubar.vala test-window.vala test-leak.vala

STAMP = $(VALASOURCES:.vala=.c)
SUFFIXES = .vala .c

.vala.c:
	$(VALAC) -C --vapidir=$(top_srcdir)/tests --vapidir=$(top_srcdir)/libgnomenu --pkg testman --pkg gtk+-2.0 --pkg gnomenu-private $^
	if [ $@ == test-leak.c ]; then bash patch.sh; fi;
	touch $@

AM_CPPFLAGS = -I$(top_srcdir) $(GTK_CFLAGS) $(GLIB_CFLAGS) -include $(CONFIG_HEADER)
BUILT_SOURCES = $(STAMP)
LDADD = $(GTK_LIBS) $(GLIB_LIBS) ../libgnomenu.la ../../tests/libtestman.la
# default rules doesn't include the .h files!
EXTRA_DIST += $(STAMP) $(VALASOURCES) $(VALASOURCES:.vala=.h) evo-nolabel.xml evo.xml
