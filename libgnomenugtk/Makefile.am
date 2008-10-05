NULL = 

AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/libgmarkupdoc \
	-I$(top_builddir)/libgmarkupdoc \
	-I$(top_srcdir)/libgnomenu \
	-I$(top_builddir)/libgnomenu \
	$(NULL)
AM_VALAC_FLAGS =\
	$(LIBGNOMENU_VALA_PKGS) \
 	--vapidir=$(top_srcdir)/libgmarkupdoc \
 	--vapidir=$(top_builddir)/libgmarkupdoc \
 	--vapidir=$(top_srcdir)/libgnomenu \
 	--vapidir=$(top_builddir)/libgnomenu \
	--pkg=gmarkupdoc \
	--pkg=gnomenu \
	$(NULL)

BUILT_SOURCES = libgnomenugtk.vala.stamp

libgnomenugtk_la_VALASOURCES = \
	bindinggtk.vala \
	bindinggtksingleton.vala \
	bindinggtkdocument.vala  \
	$(NULL)

libgnomenugtkdir = $(libdir)
libgnomenugtk_LTLIBRARIES = \
	libgnomenugtk.la \
	$(NULL)

libgnomenugtk_la_SOURCES = \
	$(libgnomenugtk_la_VALASOURCES:.vala=.c) \
	$(libgnomenugtk_la_VALASOURCES:.vala=.h) \
	bindinggtk-c.c \
	$(NULL)

gnomenugtk.vapi libgnomenugtk.vala.stamp: $(libgnomenugtk_la_VALASOURCES)
	$(VALAC) -C  $^
	touch $@

libgnomenugtk_la_LIBADD = \
		$(LIBGNOMENU_LIBS) \
		$(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

EXTRA_DIST = libgnomenugtk.vala.stamp $(libgnomenugtk_la_VALASOURCES)
