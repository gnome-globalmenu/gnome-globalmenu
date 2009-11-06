NULL = 
SUBDIRS = . tests

BUILT_SOURCES = \
	vala-ccode \
	$(NULL)

VALASOURCES = \
	superrider.vala \
	info-factory.vala \
	pref.vala \
	info.vala \
	patcher.vala \
	module-main.vala \
	$(NULL)

libgnomenu_gtkdir = $(GTK_MODULES_DIR)
libgnomenu_gtk_LTLIBRARIES = \
	libgnomenu-gtk.la \
	$(NULL)

libgnomenu_gtk_la_SOURCES= \
	$(VALASOURCES:.vala=.c) \
	globalmenu-helper.c \
	globalmenu-gnome.h \
	$(NULL)

vala-ccode : $(VALASOURCES)
	$(VALAC) $(VALA_DEFAULT_PKGS) --thread -C --basedir=$(top_srcdir) \
		--library=globalmenu-gnome -H globalmenu-gnome.h --vapidir=. $(VALASOURCES) \
		--vapidir=../../libgnomenu \
		--pkg gmodule-2.0 \
		--pkg gtk+-2.0 \
		--pkg gdk-x11-2.0 \
		--pkg gnomenu 
	touch $@

libgnomenu_gtk_la_LIBADD = \
		$(GLIB_LIBS) \
		$(GTK_LIBS) \
		$(NULL)

libgnomenu_gtk_la_LDFLAGS = \
		-g \
		-version-info 1:0:0 \
		-avoid-version \
		-no-undefined \
		-export-dynamic \
		-module

#		-Wl,--as-needed 
libgnomenu_gtk_la_CPPFLAGS = \
	  -I$(top_srcdir) $(GLIB_CFLAGS) $(GTK_CFLAGS) \
	  -include $(CONFIG_HEADER) \
	  -DG_LOG_DOMAIN=\"GlobalMenu\" \
	  -g

EXTRA_DIST = vala-ccode \
	globalmenu-gnome.h \
	$(VALASOURCES) \
	$(NULL)

