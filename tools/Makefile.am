
VALASOURCES = image-to-vala.vala
STAMPS = $(VALASOURCES:.vala=.c)
noinst_PROGRAMS = $(VALASOURCES:.vala=)

SUFFIXES = .vala .c
.vala.c:
	$(VALAC) -C --pkg gtk+-2.0 $^
	touch $@

AM_CPPFLAGS = -I$(top_srcdir) $(GTK_CFLAGS) $(GLIB_CFLAGS) -include $(CONFIG_HEADER)
BUILT_SOURCES = $(STAMP)

LDADD = $(GTK_LIBS) $(GLIB_LIBS)

EXTRA_DIST = $(STAMP) $(VALASOURCES) $(VALASOURCES:.vala=.h) $(VALASOURCES:.vala=.c)
