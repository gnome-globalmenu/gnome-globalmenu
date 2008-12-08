NULL = 
SUBDIRS = . tests
STAMP = globalmenu-gnome-private.vapi

BUILT_SOURCES = \
	$(STAMP) \
	$(NULL)

VALASOURCES = \
	globalmenu.vala \
	globalmenu-main.vala \
	globalmenu-serializer.vala \
	globalmenu-locator.vala \
	$(NULL)

libglobalmenu_gnome_ladir = $(GTK_MODULES_DIR)
lib_LTLIBRARIES = \
	libglobalmenu-gnome.la \
	$(NULL)

libglobalmenu_gnome_la_SOURCES = \
	$(VALASOURCES:.vala=.c) \
	$(VALASOURCES:.vala=.h) \
	globalmenu-helper.c \
	dyn-patch-menubar.c \
	dyn-patch-helper.h \
	dyn-patch-helper.c \
	dyn-patch-widget.c \
	$(NULL)

$(STAMP): $(VALASOURCES) gtkaqd.vapi
	$(VALAC) -C --basedir=$(top_srcdir) --pkg gtk+-2.0 --pkg gio-unix-2.0 --library=$(@:.vapi=) $^
	touch $@
$(STAMP:.vapi=.deps):
	echo > $@

libglobalmenu_gnome_la_LIBADD = \
		$(GLIB_LIBS) \
		$(GTK_LIBS) \
		$(GIO_LIBS) \
		$(NULL)
libglobalmenu_gnome_la_LDFLAGS = \
		-version-info 1:0:0 \
		-release 0.7.0 \
		-no-undefined \
		-export-dynamic \
		-module

libglobalmenu_gnome_la_CPPFLAGS = \
	  -I$(top_srcdir) $(GLIB_CFLAGS) $(GTK_CFLAGS) $(GIO_CFLAGS) \
	  -DG_LOG_DOMAIN=\"GlobalMenu\"

EXTRA_DIST = $(STAMP) $(STAMP:.vapi=.deps)\
	gtkaqd.vapi \
	$(VALASOURCES) \
	$(NULL)

