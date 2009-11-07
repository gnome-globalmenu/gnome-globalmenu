VALASOURCES = globalmenu-settings.vala
STAMPS = $(VALASOURCES:.vala=.c)

bin_PROGRAMS = $(VALASOURCES:.vala=)

SUFFIXES = .vala .c

.vala.c:
	$(VALAC) $(VALA_DEFAULT_PKGS) \
	-C \
	--vapidir=$(top_srcdir)/libsettings \
	--pkg gtk+-2.0 \
	--pkg globalmenu-settings \
	$^
	touch $@

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libsettings \
	$(GTK_CFLAGS) \
	$(GLIB_CFLAGS) \
	-include $(CONFIG_HEADER)

BUILT_SOURCES = $(STAMP)

LDADD = $(GTK_LIBS) \
	$(GLIB_LIBS) \
	$(top_builddir)/libsettings/libsettings.la 

EXTRA_DIST = $(STAMP) $(VALASOURCES)  $(VALASOURCES:.vala=.c)
