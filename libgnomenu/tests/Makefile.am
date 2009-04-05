include $(top_srcdir)/Makefile.decl

if ENABLE_TESTS
TEST_PROGS +=  $(noinst_PROGRAMS)
noinst_PROGRAMS =$(VALASOURCES:.vala=)
endif

VALASOURCES = test-xlib.vala test-label.vala test-grab.vala test-parser.vala  test-menubar.vala test-window.vala test-leak.vala test-globalmenu.vala test-application.vala

STAMP = $(VALASOURCES:.vala=.c)
SUFFIXES = .vala .c

.vala.c:
	$(VALAC) $(VALA_DEFAULT_PKGS) -C --vapidir=$(top_srcdir)/tests --vapidir=$(top_srcdir)/libgnomenu --pkg testman --pkg gtk+-2.0 --pkg gnomenu --pkg xlib $^ 
	if [ $@ == test-leak.c ]; then bash patch.sh; fi;
	touch $@

AM_CPPFLAGS = -I$(top_srcdir) $(GTK_CFLAGS) $(GLIB_CFLAGS) $(WNCK_CFLAGS) -include $(CONFIG_HEADER) -DWNCK_I_KNOW_THIS_IS_UNSTABLE '-DGDataTestFunc=void*'
BUILT_SOURCES = $(STAMP)
LDADD = $(GTK_LIBS) $(GLIB_LIBS) ../libgnomenu.la ../../tests/libtestman.la
# default rules doesn't include the .h files!
EXTRA_DIST += $(STAMP) $(VALASOURCES) $(VALASOURCES:.vala=.h) evo-nolabel.xml evo.xml
