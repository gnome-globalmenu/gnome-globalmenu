AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

AM_VALAC_FLAGS =  \
	$(LIBGNOMENU_VALA_PKGS) \
	$(NULL)

BUILT_SOURCES = \
	libgmarkupdoc.vala.stamp \
	libgmarkupdoc-base.vala.stamp \
	libgmarkupdoc-named.vala.stamp \
	libgmarkupdoc-dbus.vala.stamp \
	$(NULL)

libgmarkupdocdir = $(libdir)
libgmarkupdoc_LTLIBRARIES = \
		libgmarkupdoc.la \
		libgmarkupdoc-base.la \
		libgmarkupdoc-named.la \
		libgmarkupdoc-dbus.la \
		$(NULL)

libgmarkupdoc_la_VALASOURCES = \
		gmarkupdoc.vala \
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



libgmarkupdoc_la_SOURCES = \
		$(libgmarkupdoc_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_la_VALASOURCES:.vala=.h) \
		$(NULL)

libgmarkupdoc_base_la_SOURCES = \
		$(libgmarkupdoc_base_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_base_la_VALASOURCES:.vala=.h) \
		$(NULL)

libgmarkupdoc_dbus_la_SOURCES = \
		$(libgmarkupdoc_dbus_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_dbus_la_VALASOURCES:.vala=.h) \
		$(NULL)

libgmarkupdoc_named_la_SOURCES = \
		$(libgmarkupdoc_named_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_named_la_VALASOURCES:.vala=.h) \
		$(NULL)


# gmarkupdoc.vapi is built from the other three libraries
gmarkupdoc.vapi: gmarkupdoc-base.vapi gmarkupdoc-dbus.vapi gmarkupdoc-named.vapi
	cat $^ > $@

libgmarkupdoc.vala.stamp: $(libgmarkupdoc_la_VALASOURCES)
	$(VALAC) -C   $^
	touch $@
gmarkupdoc-base.vapi libgmarkupdoc-base.vala.stamp: $(libgmarkupdoc_base_la_VALASOURCES)
	$(VALAC) -C  --library gmarkupdoc-base  $^
	touch $@
gmarkupdoc-named.vapi libgmarkupdoc-named.vala.stamp: $(libgmarkupdoc_named_la_VALASOURCES) gmarkupdoc-base.vapi
	$(VALAC) -C  --library gmarkupdoc-named  $^
	touch $@
gmarkupdoc-dbus.vapi libgmarkupdoc-dbus.vala.stamp: $(libgmarkupdoc_dbus_la_VALASOURCES) gmarkupdoc-named.vapi gmarkupdoc-base.vapi
	$(VALAC) -C  --library gmarkupdoc-dbus $^
	touch $@

libgmarkupdoc_la_LIBADD = \
		libgmarkupdoc-base.la \
		libgmarkupdoc-named.la \
		libgmarkupdoc-dbus.la \
		$(NULL)
libgmarkupdoc_dbus_la_LIBADD = \
		$(LIBGNOMENU_LIBS) \
		$(NULL)

EXTRA_DIST =  \
	libgmarkupdoc.vala.stamp \
	libgmarkupdoc-base.vala.stamp \
	libgmarkupdoc-dbus.vala.stamp \
	libgmarkupdoc-named.vala.stamp \
	$(libgmarkupdoc_la_VALASOURCES) \
	$(libgmarkupdoc_base_la_VALASOURCES) \
	$(libgmarkupdoc_dbus_la_VALASOURCES) \
	$(libgmarkupdoc_named_la_VALASOURCES) \
	$(NULL)


vapidir = $(VALA_VAPI_DIR)
dist_vapi_DATA = \
	gmarkupdoc.vapi \
	gmarkupdoc-base.vapi \
	gmarkupdoc-dbus.vapi \
	gmarkupdoc-named.vapi \
	$(NULL)
headerdir = $(includedir)/libgmarkupdoc
dist_header_DATA = \
	$(libgmarkupdoc_la_VALASOURCES:%.vala=%.h) \
	$(libgmarkupdoc_base_la_VALASOURCES:%.vala=%.h) \
	$(libgmarkupdoc_dbus_la_VALASOURCES:%.vala=%.h) \
	$(libgmarkupdoc_named_la_VALASOURCES:%.vala=%.h) \
	$(NULL)

