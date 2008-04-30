AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_GTK_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I../libgnomenu  \
	$(NULL)

BUILT_SOURCES = libgnomenu-gtk.vala.stamp

libgnomenu_gtkdir = $(libdir)
libgnomenu_gtk_LTLIBRARIES = \
		libgnomenu-gtk.la

libgnomenu_gtk_la_VALASOURCES = \
		agentgtk.vala \
		$(NULL)

libgnomenu_gtk_la_SOURCES = \
		$(libgnomenu_gtk_la_VALASOURCES:.vala=.c) \
		$(libgnomenu_gtk_la_VALASOURCES:.vala=.h) \
		$(NULL)

libgnomenu-gtk.vala.stamp: $(libgnomenu_gtk_la_VALASOURCES)
	$(VALAC) -C $(LIBGNOMENU_GTK_VALA_PKGS) --vapidir=../libgnomenu --pkg gnomenu --library gnomenu-gtk $^
	touch $@

libgnomenu_gtk_la_LIBADD = \
		$(LIBGNOMENU_GTK_LIBS) \
		$(NULL)

EXTRA_DIST = libgnomenu-gtk.vala.stamp	
