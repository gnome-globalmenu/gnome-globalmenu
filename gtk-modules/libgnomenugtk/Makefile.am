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
	libgnomenugtk.vala.stamp  \
	$(libgnomenugtk_la_VALASOURCES:.vala=.c) \
	$(libgnomenugtk_la_VALASOURCES:.vala=.h) \
	$(NULL)

libgnomenugtk_la_VALASOURCES = \
	gnomenugtk.vala \
	gnomenugtk-client.vala \
	gnomenugtk-document.vala  \
	$(NULL)

libgnomenugtkdir = $(GTK_MODULES_DIR)
libgnomenugtk_LTLIBRARIES = \
	libgnomenugtk.la \
	$(NULL)

libgnomenugtk_la_SOURCES = \
	$(libgnomenugtk_la_VALASOURCES:.vala=.c) \
	$(libgnomenugtk_la_VALASOURCES:.vala=.h) \
	utils.c \
	$(NULL)

gnomenugtk.vapi libgnomenugtk.vala.stamp: $(libgnomenugtk_la_VALASOURCES) gtkaqd.vapi
	$(VALAC) -C  $^
	touch gnomenugtk.vapi libgnomenugtk.vala.stamp

libgnomenugtk_la_LIBADD = \
		$(GNOMENUGTK_LIBS) \
		$(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	gtkaqd.vapi \
	$(libgnomenugtk_la_VALASOURCES) \
	$(NULL)

