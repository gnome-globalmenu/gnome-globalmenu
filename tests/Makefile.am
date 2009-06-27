include $(top_srcdir)/Makefile.decl


STAMP = testman.vapi
VALASOURCES = testman.vala

$(STAMP) : $(VALASOURCES) 
	$(VALAC) -H testman.h -C --basedir=$(top_srcdir) --pkg gtk+-2.0 --library testman $(VALASOURCES)

noinst_LTLIBRARIES = libtestman.la

libtestman_la_SOURCES = $(VALASOURCES:.vala=.c) testman.h
libtestman_la_LIBADD = $(GLIB_LIBS) $(GTK_LIBS)
libtestman_la_CPPFLAGS = \
			 -include "config.h" \
			 -include "configmake.h" \
			 -I$(top_srcdir) \
			 $(GLIB_CFLAGS) \
			 $(GTK_CFLAGS)

BUILT_SOURCES = $(STAMP)
EXTRA_DIST += $(STAMP) $(VALASOURCES)

