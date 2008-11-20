include $(top_srcdir)/Makefile.decl

NULL =

AM_CPPFLAGS = \
	-I$(top_srcdir)/gee \
	$(GLIB_CFLAGS) \
	$(NULL)

BUILT_SOURCES = gee.vala.stamp

lib_LTLIBRARIES = \
	libdom-gee.la \
	$(NULL)

libgee_la_VALASOURCES = \
	arraylist.vala \
	collection.vala \
	hashmap.vala \
	hashset.vala \
	iterable.vala \
	iterator.vala \
	list.vala \
	map.vala \
	readonlycollection.vala \
	readonlylist.vala \
	readonlymap.vala \
	readonlyset.vala \
	set.vala \
	$(NULL)

libdom_gee_la_SOURCES = \
	gee.vala.stamp \
	$(libgee_la_VALASOURCES:.vala=.c) \
	$(libgee_la_VALASOURCES:.vala=.h) \
	$(NULL)

geeincludedir = $(includedir)/dom-gee-1.0/gee

geeinclude_HEADERS = \
	$(libgee_la_VALASOURCES:.vala=.h) \
	$(NULL)

dom-gee-1.0.vapi gee.vala.stamp: $(libgee_la_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir)/gee --library dom-gee-1.0 $^
	touch $@

libdom_gee_la_LIBADD = \
	$(GLIB_LIBS) \
	$(NULL)

vapidir = $(datadir)/vala/vapi

dist_vapi_DATA = \
	dom-gee-1.0.vapi \
	$(NULL)

EXTRA_DIST += $(libgee_la_VALASOURCES) dom-gee-1.0.vapi gee.vala.stamp
