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
	-DG_LOG_DOMAIN=\"Gnomenu\" \
	$(NULL)

BUILT_SOURCES = \
	gnomenu.h \
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
	sed -e 's/GtkCompat/Gtk/g' gnomenu.vapi > gnomenu.vapi.edited
	mv gnomenu.vapi.edited gnomenu.vapi
	touch gnomenu.vapi libgnomenu.vala.stamp

libgnomenu_la_LIBADD = \
		$(GNOMENU_LIBS) \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

_HEADERS_ = \
		$(libgnomenu_la_VALASOURCES:.vala=.h)

gnomenu.h: Makefile
	echo '#ifndef _GNOMENU_H_' > $@ ;
	echo '#define _GNOMENU_H_' >> $@ ;
	for i in $(_HEADERS_); do \
		echo '#include "'$$i'"' >> $@ ;\
	done;
	echo '#endif' >> $@ ;

noinst_PROGRAMS = example
example_LDADD = \
		$(top_builddir)/libgnomenu/libgnomenu.la 

EXTRA_DIST = \
	 $(BUILT_SOURCES) \
	 $(libgnomenu_la_VALASOURCES)

DISTCLEANFILES = gnomenu.h

vapidir = $(VALA_VAPI_DIR)
dist_vapi_DATA = gnomenu.vapi

headerdir = $(includedir)/libgnomenu
dist_header_DATA = \
	gnomenu.h \
	$(_HEADERS_) \
	$(NULL)

