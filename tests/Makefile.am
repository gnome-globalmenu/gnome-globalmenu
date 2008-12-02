include $(top_srcdir)/Makefile.decl


STAMP = testman.vapi
VALASOURCES = testman.vala

$(STAMP) : $(VALASOURCES)
	$(VALAC) -C --basedir=$(top_srcdir) --library testman $^
	sed testman.h -e '10i\ typedef void* GDataTestFunc;' > testman.h.new; 
	mv testman.h.new testman.h

noinst_LTLIBRARIES = libtestman.la

libtestman_la_SOURCES = $(VALASOURCES:.vala=.c) $(VALASOURCES:.vala=.h)
libtestman_la_LIBADD = $(GLIB_LIBS)
libtestman_la_CPPFLAGS = $(GLIB_CFLAGS)

BUILT_SOURCES = $(STAMP)
EXTRA_DIST += $(STAMP) $(VALASOURCES)

