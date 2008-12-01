include $(top_srcdir)/Makefile.decl



VALASOURCES = test-serializer.vala
STAMP = $(VALASOURCES:.vala=.stamp)
SUFFIXES = .vala .stamp
.vala.stamp:
	$(VALAC) -C --vapidir=$(top_srcdir)/tests --vapidir=../ --pkg testman --pkg gtk+-2.0 --pkg globalmenu-gnome-private $^
	touch $@

TEST_PROGS +=  $(VALASOURCES:.vala=)
	
noinst_PROGRAMS = $(TEST_PROGS)
AM_CPPFLAGS = $(GTK_CFLAGS) $(GLIB_CFLAGS)
BUILT_SOURCES = $(STAMP)
LDADD = $(GTK_LIBS) $(GLIB_LIBS) ../libglobalmenu-gnome.la $(top_builddir)/tests/libtestman.la
EXTRA_DIST += $(STAMP) $(VALASOURCES)

