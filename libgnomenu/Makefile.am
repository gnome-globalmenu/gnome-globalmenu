NULL = 

AM_VALAC_FLAGS =  \
	$(GNOMENU_VALA_PKGS) \
	--vapidir=$(top_srcdir)/libgmarkupdoc \
	--pkg gmarkupdoc \
	--pkg gtkcompat \
	$(NULL)

AM_CPPFLAGS = \
	-g \
	-I$(top_srcdir)/libgmarkupdoc \
	$(GNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = \
	libgnomenu.vala.stamp \
	$(libgnomenu_la_VALASOURCES:.vala=.c) \
	$(libgnomenu_la_VALASOURCES:.vala=.h) \
	$(NULL)

libgnomenudir = $(libdir)
libgnomenu_LTLIBRARIES = \
		libgnomenu.la

libgnomenu_la_VALASOURCES = \
		gnomenu-client.vala \
		gnomenu-menuview.vala \
		gnomenu-menubar.vala \
		gnomenu-server.vala \
		$(NULL)


libgnomenu_la_SOURCES = \
		$(libgnomenu_la_VALASOURCES:.vala=.c) \
		$(libgnomenu_la_VALASOURCES:.vala=.h) \
		$(NULL)


gnomenu.vapi libgnomenu.vala.stamp: $(libgnomenu_la_VALASOURCES)
	$(VALAC) -C --library gnomenu $^
	touch $@

libgnomenu_la_LIBADD = \
		$(GNOMENU_LIBS) \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

EXTRA_DIST = \
	 $(BUILT_SOURCES) \
	 $(libgnomenu_la_VALASOURCES)

vapidir = $(VALA_VAPI_DIR)
dist_vapi_DATA = gnomenu.vapi
