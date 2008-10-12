AM_CPPFLAGS = \
	-g \
	-include $(CONFIG_HEADER) \
	-DG_LOG_DOMAIN=\"GMarkupDoc\" \
	$(NULL)

AM_VALAC_FLAGS =  \
	--pkg gtkcompat \
	$(NULL)


libgmarkupdocdir = $(libdir)
libgmarkupdoc_LTLIBRARIES = \
		libgmarkupdoc-base.la \
		libgmarkupdoc-list.la \
		libgmarkupdoc-dbus.la \
		libgmarkupdoc.la \
		$(NULL)

libgmarkupdoc_la_SOURCES = \
		gmarkupdoc.c \
		gmarkupdoc.h \
		$(NULL)
libgmarkupdoc_la_CFLAGS = \
	$(GMARKUPDOC_BASE_CFLAGS) \
	$(GMARKUPDOC_LIST_CFLAGS) \
	$(GMARKUPDOC_DBUS_CFLAGS) \
	$(NULL)
libgmarkupdoc_base_la_VALASOURCES = \
		gmarkupdoc-documentmodel.vala \
		gmarkupdoc-node.vala \
		gmarkupdoc-section.vala \
		gmarkupdoc-parser.vala \
		gmarkupdoc-document.vala \
		$(NULL)

libgmarkupdoc_list_la_VALASOURCES = \
		gmarkupdoc-documenttreeadapter.vala \
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
	libgmarkupdoc.vala.stamp \
	$(libgmarkupdoc_base_la_VALASOURCES:.vala=.c) \
	$(libgmarkupdoc_base_la_VALASOURCES:.vala=.h) \
	$(libgmarkupdoc_dbus_la_VALASOURCES:.vala=.c) \
	$(libgmarkupdoc_dbus_la_VALASOURCES:.vala=.h) \
	$(libgmarkupdoc_list_la_VALASOURCES:.vala=.c) \
	$(libgmarkupdoc_list_la_VALASOURCES:.vala=.h) \
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

libgmarkupdoc_list_la_SOURCES = \
		$(libgmarkupdoc_list_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_list_la_VALASOURCES:.vala=.h) \
		$(NULL)
libgmarkupdoc_list_la_CFLAGS = $(GMARKUPDOC_LIST_CFLAGS)
libgmarkupdoc_list_la_LIBADD = $(GMARKUPDOC_LIST_LIBS)

gmarkupdoc.vapi libgmarkupdoc.vala.stamp: $(libgmarkupdoc_base_la_VALASOURCES) $(libgmarkupdoc_list_la_VALASOURCES) $(libgmarkupdoc_dbus_la_VALASOURCES) 
	$(VALAC) -C $(GMARKUPDOC_BASE_VALA_PKGS) $(GMARKUPDOC_DBUS_VALA_PKGS) $(GMARKUPDOC_LIST_VALA_PKGS) --library gmarkupdoc $^
	sed -e 's;GtkCompat.Container;Gtk.Container;g' gmarkupdoc.vapi > gmarkupdoc.vapi.new
	mv gmarkupdoc.vapi.new gmarkupdoc.vapi
	touch gmarkupdoc.vapi libgmarkupdoc.vala.stamp

_HEADERS_ = $(libgmarkupdoc_base_la_VALASOURCES:%.vala=%.h) \
	$(libgmarkupdoc_dbus_la_VALASOURCES:%.vala=%.h) \
	$(libgmarkupdoc_list_la_VALASOURCES:%.vala=%.h) \
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

libgmarkupdoc_la_LIBADD = \
		libgmarkupdoc-base.la \
		libgmarkupdoc-list.la \
		libgmarkupdoc-dbus.la \
		$(NULL)

EXTRA_DIST =  \
	$(BUILT_SOURCES) \
	$(libgmarkupdoc_base_la_VALASOURCES) \
	$(libgmarkupdoc_dbus_la_VALASOURCES) \
	$(libgmarkupdoc_list_la_VALASOURCES) \
	$(NULL)
DISTCLEANFILES = gmarkupdoc.c gmarkupdoc.h

vapidir = $(VALA_VAPI_DIR)
dist_vapi_DATA = \
	gmarkupdoc.vapi \
	$(NULL)
headerdir = $(includedir)/libgmarkupdoc
dist_header_DATA = \
	gmarkupdoc.h \
	$(_HEADERS_) \
	$(NULL)

