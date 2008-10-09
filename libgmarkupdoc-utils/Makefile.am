NULL = 

AM_VALAC_FLAGS =  \
	$(LIBGNOMENU_VALA_PKGS) \
	--vapidir=$(top_srcdir)/libgmarkupdoc \
	--vapidir=$(top_builddir)/libgmarkupdoc \
	--pkg gmarkupdoc \
	$(NULL)

AM_CPPFLAGS = \
	-g \
	-I$(top_srcdir)/libgmarkupdoc \
	-I$(top_builddir)/libgmarkupdoc \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = libgmarkupdoc-utils.vala.stamp

libgmarkupdoc_utilsdir = $(libdir)
libgmarkupdoc_utils_LTLIBRARIES = \
		libgmarkupdoc-utils.la

libgmarkupdoc_utils_la_VALASOURCES = \
		document.vala \
		listview.vala \
		dbusview.vala \
		remotedocument.vala \
		$(NULL)


libgmarkupdoc_utils_la_SOURCES = \
		$(libgmarkupdoc_utils_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_utils_la_VALASOURCES:.vala=.h) \
		$(NULL)


gmarkupdoc-utils.vapi libgmarkupdoc-utils.vala.stamp: $(libgmarkupdoc_utils_la_VALASOURCES)
	$(VALAC) -C --library gmarkupdoc-utils $^
	touch $@

libgmarkupdoc_utils_la_LIBADD = \
		$(LIBGNOMENU_LIBS) \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

EXTRA_DIST = libgmarkupdoc_utils.vala.stamp $(libgmarkupdoc_utils_la_VALASOURCES)

vapidir = $(VALA_VAPI_DIR)
dist_vapi_DATA = gmarkupdoc-utils.vapi
