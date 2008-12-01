NULL = 

STAMP = vala-stamp

BUILT_SOURCES = \
	$(STAMP) \
	$(NULL)

VALASOURCES = \
	globalmenu.vala \
	globalmenu-main.vala \
	$(NULL)

libglobalmenu_gnomedir = $(GTK_MODULES_DIR)
lib_LTLIBRARIES = \
	libglobalmenu-gnome.la \
	$(NULL)

libglobalmenu_gnome_la_SOURCES = \
	$(VALASOURCES:.vala=.c) \
	$(VALASOURCES:.vala=.h) \
	globalmenu-helper.c \
	dyn-patch.h \
	dyn-patch-menubar.c \
	dyn-patch-helper.h \
	dyn-patch-helper.c \
	dyn-patch-widget.c \
	$(NULL)

$(STAMP): $(VALASOURCES) gtkaqd.vapi
	$(VALAC) -C --pkg gtk+-2.0 --pkg gio-unix-2.0 $^
	touch $@

libglobalmenu_gnome_la_LIBADD = \
		$(GLIB_LIBS) \
		$(GTK_LIBS) \
		$(GIO_LIBS) \
		$(NULL)
libglobalmenu_gnome_la_LDFLAGS = \
		-version-info 1:0:0 \
		-release 0.7.0 \
		-no-undefined \
		-module

libglobalmenu_gnome_la_CPPFLAGS = $(GLIB_CFLAGS) $(GTK_CFLAGS) $(GIO_CFLAGS)

EXTRA_DIST = $(STAMP) \
	gtkaqd.vapi \
	$(VALASOURCES) \
	$(NULL)

