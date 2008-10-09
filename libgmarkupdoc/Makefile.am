AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

AM_VALAC_FLAGS =  \
	$(LIBGNOMENU_VALA_PKGS) \
	$(NULL)

BUILT_SOURCES = libgmarkupdoc.vala.stamp

libgmarkupdocdir = $(libdir)
libgmarkupdoc_LTLIBRARIES = \
		libgmarkupdoc.la

libgmarkupdoc_la_VALASOURCES = \
		gmarkupdoc.vala \
		gmarkupdoc-document.vala \
		gmarkupdoc-node.vala \
		gmarkupdoc-section.vala \
		gmarkupdoc-parser.vala \
		$(NULL)


libgmarkupdoc_la_SOURCES = \
		$(libgmarkupdoc_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_la_VALASOURCES:.vala=.h) \
		$(NULL)


gnomenu.vapi libgmarkupdoc.vala.stamp: $(libgmarkupdoc_la_VALASOURCES)
	$(VALAC) -C  --library gmarkupdoc  $^
	touch $@

libgmarkupdoc_la_LIBADD = \
		$(LIBGNOMENU_LIBS) \
		$(NULL)

EXTRA_DIST = libgmarkupdoc.vala.stamp $(libgmarkupdoc_la_VALASOURCES)

vapidir = $(VALA_VAPI_DIR)
dist_vapi_DATA = gmarkupdoc.vapi
headerdir = $(includedir)/libgmarkupdoc
dist_header_DATA = $(libgmarkupdoc_la_VALASOURCES:%.vala=%.h)
