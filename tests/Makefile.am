include $(top_srcdir)/Makefile.decl


STAMP = testman.vapi
VALASOURCES = testman.vala

$(STAMP) : $(VALASOURCES) 
	$(VALAC) -C --basedir=$(top_srcdir) --pkg gtk+-2.0 --library testman $(VALASOURCES)

noinst_LTLIBRARIES = libtestman.la

libtestman_la_SOURCES = $(VALASOURCES:.vala=.c) $(VALASOURCES:.vala=.h)
libtestman_la_LIBADD = $(GLIB_LIBS) $(GTK_LIBS)
libtestman_la_CPPFLAGS = \
			 -I$(top_srcdir) \
			 $(GLIB_CFLAGS) \
			 $(GTK_CFLAGS) \
			 '-DGDataTestFunc=void*'

BUILT_SOURCES = $(STAMP)
EXTRA_DIST += $(STAMP) $(VALASOURCES)

