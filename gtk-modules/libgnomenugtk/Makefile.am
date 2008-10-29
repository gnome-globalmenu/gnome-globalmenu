NULL = 

AM_CPPFLAGS = \
	-g \
	$(GNOMENUGTK_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/libgmarkupdoc \
	-I$(top_srcdir)/libgnomenu \
	-DG_LOG_DOMAIN=\"GnomenuGTK\" \
	$(NULL)
AM_VALAC_FLAGS =\
	$(GNOMENUGTK_VALA_PKGS) \
 	--vapidir=$(top_srcdir)/libgmarkupdoc \
 	--vapidir=$(top_srcdir)/libgnomenu \
	--pkg=gmarkupdoc \
	--pkg=gtkcompat \
	--pkg=gnomenu \
	$(NULL)

BUILT_SOURCES = \
	gnomenugtk.vapi \
	libglobalmenu_gnome.vala.stamp  \
	$(libglobalmenu_gnome_la_VALASOURCES:.vala=.c) \
	$(libglobalmenu_gnome_la_VALASOURCES:.vala=.h) \
	$(NULL)

libglobalmenu_gnome_la_VALASOURCES = \
	gnomenugtk.vala \
	gnomenugtk-client.vala \
	gnomenugtk-document.vala  \
	$(NULL)

libglobalmenu_gnomedir = $(GTK_MODULES_DIR)
libglobalmenu_gnome_LTLIBRARIES = \
	libglobalmenu-gnome.la \
	$(NULL)

libglobalmenu_gnome_la_SOURCES = \
	$(libglobalmenu_gnome_la_VALASOURCES:.vala=.c) \
	$(libglobalmenu_gnome_la_VALASOURCES:.vala=.h) \
	utils.c \
	$(NULL)

gnomenugtk.vapi libglobalmenu_gnome.vala.stamp: $(libglobalmenu_gnome_la_VALASOURCES) gtkaqd.vapi
	$(VALAC) -C  $^
	touch gnomenugtk.vapi libglobalmenu_gnome.vala.stamp

libglobalmenu_gnome_la_LIBADD = \
		$(GNOMENUGTK_LIBS) \
		$(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)
libglobalmenu_gnome_la_LDFLAGS = \
		-module \
		$(NULL)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	gtkaqd.vapi \
	$(libglobalmenu_gnome_la_VALASOURCES) \
	$(NULL)

