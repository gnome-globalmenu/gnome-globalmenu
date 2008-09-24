SUBDIRS = . gtk tests
AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = libgnomenu.vala.stamp

libgnomenudir = $(libdir)
libgnomenu_LTLIBRARIES = \
		libgnomenu.la

libgnomenu_la_VALASOURCES = \
		xmlparser.vala \
		xml.vala \
		server.vala  \
		client.vala \
		nodefactory.vala \
		$(NULL)


libgnomenu_la_SOURCES = \
		$(libgnomenu_la_VALASOURCES:.vala=.c) \
		$(libgnomenu_la_VALASOURCES:.vala=.h) \
		$(NULL)


gnomenu.vapi libgnomenu.vala.stamp: $(libgnomenu_la_VALASOURCES)
	$(VALAC) -C $(LIBGNOMENU_VALA_PKGS) --vapidir=$(srcdir) --library gnomenu --pkg valacompat $^
	touch $@

libgnomenu_la_LIBADD = \
		$(LIBGNOMENU_LIBS) \
		$(NULL)

EXTRA_DIST = libgnomenu.vala.stamp	
