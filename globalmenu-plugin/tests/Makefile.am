include $(top_srcdir)/Makefile.gtest

VALASOURCES = test-superride.vala test-module.vala

if ENABLE_TESTS
noinst_PROGRAMS =  $(VALASOURCES:.vala=)
TEST_PROGS += $(noinst_PROGRAMS)
endif 

STAMP = $(VALASOURCES:.vala=.c)
SUFFIXES = .vala .c 
.vala.c:
	$(VALAC) $(VALA_DEFAULT_PKGS) \
	-C \
	--vapidir=$(top_srcdir)/tests \
	--vapidir=../ \
	--pkg testman \
	--pkg gtk+-2.0 \
	--pkg globalmenu-plugin \
	--pkg gmodule-2.0 $^
	touch $@

AM_CPPFLAGS = -I.. \
	$(GTK_CFLAGS) \
	$(GLIB_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/tests \
	-I$(top_srcdir)/libsettings \
	$(NULL)

BUILT_SOURCES = $(STAMP)
LDADD = $(GTK_LIBS) \
	$(GLIB_LIBS) \
	../libglobalmenu-plugin.la \
	$(top_builddir)/tests/libtestman.la \
	$(NULL)

EXTRA_DIST += $(STAMP) $(VALASOURCES)
