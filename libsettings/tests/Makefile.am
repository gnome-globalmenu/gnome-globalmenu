include $(top_srcdir)/Makefile.gtest

if ENABLE_TESTS
TEST_PROGS +=  $(noinst_PROGRAMS)
noinst_PROGRAMS =$(VALASOURCES:.vala=)
endif

VALASOURCES = \
	test-watch-settings.vala

STAMP = $(VALASOURCES:.vala=.c)
SUFFIXES = .vala .c

.vala.c:
	$(VALAC) $(VALA_DEFAULT_PKGS) -C \
		--vapidir=.. \
		--pkg gtk+-2.0 \
		--pkg globalmenu-settings $^
	touch $@

AM_CPPFLAGS = -I.. \
	$(GTK_CFLAGS) \
	$(GLIB_CFLAGS) \
	-include $(CONFIG_HEADER) 

BUILT_SOURCES = $(STAMP)
LDADD = $(GTK_LIBS) \
	$(GLIB_LIBS) \
	$(WNCK_LIBS) \
	../libsettings.la

EXTRA_DIST += $(STAMP) $(VALASOURCES)
