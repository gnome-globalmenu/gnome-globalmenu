AM_CPPFLAGS = \
	-g \
	-include $(CONFIG_HEADER) \
	$(NULL)

AM_VALAC_FLAGS =  \
	$(NULL)


libgmarkupdocdir = $(libdir)
libgmarkupdoc_LTLIBRARIES = \
		libgmarkupdoc-base.la \
		libgmarkupdoc-named.la \
		libgmarkupdoc-dbus.la \
		libgmarkupdoc.la \
		$(NULL)

libgmarkupdoc_la_SOURCES = \
		gmarkupdoc.c \
		gmarkupdoc.h \
		$(NULL)
libgmarkupdoc_la_CFLAGS = \
	$(GMARKUPDOC_BASE_CFLAGS) \
	$(GMARKUPDOC_NAMED_CFLAGS) \
	$(GMARKUPDOC_DBUS_CFLAGS) \
	$(NULL)
libgmarkupdoc_base_la_VALASOURCES = \
		gmarkupdoc-documentmodel.vala \
		gmarkupdoc-node.vala \
		gmarkupdoc-section.vala \
		gmarkupdoc-parser.vala \
		$(NULL)

libgmarkupdoc_named_la_VALASOURCES = \
		gmarkupdoc-document.vala \
		gmarkupdoc-listview.vala \
		$(NULL)

libgmarkupdoc_dbus_la_VALASOURCES = \
		gmarkupdoc-dbusview.vala \
		gmarkupdoc-remotedocument.vala \
		$(NULL)

BUILT_SOURCES = \
	gmarkupdoc.vapi \
	gmarkupdoc.h \
	gmarkupdoc.c \
   	gmarkupdoc-base.vapi \
	libgmarkupdoc-base.vala.stamp \
	$(libgmarkupdoc_base_la_VALASOURCES:.vala=.c) \
	$(libgmarkupdoc_base_la_VALASOURCES:.vala=.h) \
	gmarkupdoc-dbus.vapi \
	libgmarkupdoc-dbus.vala.stamp \
	$(libgmarkupdoc_dbus_la_VALASOURCES:.vala=.c) \
	$(libgmarkupdoc_dbus_la_VALASOURCES:.vala=.h) \
	gmarkupdoc-named.vapi \
	libgmarkupdoc-named.vala.stamp \
	$(libgmarkupdoc_named_la_VALASOURCES:.vala=.c) \
	$(libgmarkupdoc_named_la_VALASOURCES:.vala=.h) \
	$(NULL)

libgmarkupdoc_base_la_SOURCES = \
		$(libgmarkupdoc_base_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_base_la_VALASOURCES:.vala=.h) \
		$(NULL)
libgmarkupdoc_base_la_CFLAGS = $(GMARKUPDOC_BASE_CFLAGS)
libgmarkupdoc_base_la_LIBADD = $(GMARKUPDOC_BASE_LIBS)

libgmarkupdoc_dbus_la_SOURCES = \
		$(libgmarkupdoc_dbus_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_dbus_la_VALASOURCES:.vala=.h) \
		$(NULL)
libgmarkupdoc_dbus_la_CFLAGS = $(GMARKUPDOC_DBUS_CFLAGS)
libgmarkupdoc_dbus_la_LIBADD = $(GMARKUPDOC_DBUS_LIBS)

libgmarkupdoc_named_la_SOURCES = \
		$(libgmarkupdoc_named_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_named_la_VALASOURCES:.vala=.h) \
		$(NULL)
libgmarkupdoc_named_la_CFLAGS = $(GMARKUPDOC_NAMED_CFLAGS)
libgmarkupdoc_named_la_LIBADD = $(GMARKUPDOC_NAMED_LIBS)

gmarkupdoc-base.vapi libgmarkupdoc-base.vala.stamp: $(libgmarkupdoc_base_la_VALASOURCES)
	$(VALAC) -C $(GMARKUPDOC_BASE_VALA_PKGS) --library gmarkupdoc-base  $^
	touch $@
gmarkupdoc-named.vapi libgmarkupdoc-named.vala.stamp: $(libgmarkupdoc_named_la_VALASOURCES) gmarkupdoc-base.vapi
	$(VALAC) -C $(GMARKUPDOC_NAMED_VALA_PKGS) --library gmarkupdoc-named  $^
	touch $@
gmarkupdoc-dbus.vapi libgmarkupdoc-dbus.vala.stamp: $(libgmarkupdoc_dbus_la_VALASOURCES) gmarkupdoc-named.vapi gmarkupdoc-base.vapi
	$(VALAC) -C $(GMARKUPDOC_DBUS_VALA_PKGS) --library gmarkupdoc-dbus $^
	touch $@

_HEADERS_ = $(libgmarkupdoc_base_la_VALASOURCES:%.vala=%.h) \
	$(libgmarkupdoc_dbus_la_VALASOURCES:%.vala=%.h) \
	$(libgmarkupdoc_named_la_VALASOURCES:%.vala=%.h) \
	$(NULL)

gmarkupdoc.c: Makefile
	echo '#include "gmarkupdoc.h"' > $@
	echo 'extern void * dummy = NULL;' >> $@
gmarkupdoc.h: $(_HEADERS_) Makefile
	echo '#ifndef _G_MARKUP_DOC_H_' > $@ ;
	echo '#define _G_MARKUP_DOC_H_' >> $@ ;
	for i in $(_HEADERS_); do \
		echo '#include "'$$i'"' >> $@ ;\
	done;
	echo '#endif' >> $@ ;
gmarkupdoc.vapi: gmarkupdoc-base.vapi gmarkupdoc-dbus.vapi gmarkupdoc-named.vapi
	cat $^ > $@

libgmarkupdoc_la_LIBADD = \
		libgmarkupdoc-base.la \
		libgmarkupdoc-named.la \
		libgmarkupdoc-dbus.la \
		$(NULL)

EXTRA_DIST =  \
	$(BUILT_SOURCES) \
	$(libgmarkupdoc_base_la_VALASOURCES) \
	$(libgmarkupdoc_dbus_la_VALASOURCES) \
	$(libgmarkupdoc_named_la_VALASOURCES) \
	$(NULL)
DISTCLEANFILES = gmarkupdoc.c gmarkupdoc.h

vapidir = $(VALA_VAPI_DIR)
dist_vapi_DATA = \
	gmarkupdoc.vapi \
	gmarkupdoc-base.vapi \
	gmarkupdoc-dbus.vapi \
	gmarkupdoc-named.vapi \
	$(NULL)
headerdir = $(includedir)/libgmarkupdoc
dist_header_DATA = \
	gmarkupdoc.h \
	$(_HEADERS_) \
	$(NULL)

