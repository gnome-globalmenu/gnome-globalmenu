include $(top_srcdir)/Makefile.decl

VALASOURCES = test-serializer.vala

if ENABLE_TESTS
noinst_PROGRAMS =  $(VALASOURCES:.vala=)
TEST_PROGS += $(noinst_PROGRAMS)
endif 

STAMP = $(VALASOURCES:.vala=.stamp)
SUFFIXES = .vala .stamp
.vala.stamp:
	$(VALAC) -C --vapidir=$(top_srcdir)/tests --vapidir=../ --pkg testman --pkg gtk+-2.0 --pkg globalmenu-gnome-private $^
	touch $@

AM_CPPFLAGS = -I$(top_srcdir) $(GTK_CFLAGS) $(GLIB_CFLAGS)
BUILT_SOURCES = $(STAMP)
LDADD = $(GTK_LIBS) $(GLIB_LIBS) ../libglobalmenu-gnome.la $(top_builddir)/tests/libtestman.la
EXTRA_DIST += $(STAMP) $(VALASOURCES) $(VALASOURCES:.vala=.h)

