NULL = 

STAMP = vala-stamp

BUILT_SOURCES = \
	$(STAMP) \
	$(NULL)

VALASOURCES = \
	gnomenugtk.vala \
	$(NULL)

libglobalmenu_gnomedir = $(GTK_MODULES_DIR)
lib_LTLIBRARIES = \
	libglobalmenu-gnome.la \
	$(NULL)

libglobalmenu_gnome_la_SOURCES = \
	$(VALASOURCES:.vala=.c) \
	$(VALASOURCES:.vala=.h) \
	utils.c \
	$(NULL)

$(STAMP): $(VALASOURCES) gtkaqd.vapi
	$(VALAC) -C --pkg gtk+-2.0 --pkg gio-unix-2.0 $^

libglobalmenu_gnome_la_LIBADD = \
		$(GLIB_LIBS) \
		$(GTK_LIBS) \
		$(GIO_LIBS) \
		$(NULL)
libglobalmenu_gnome_la_LDFLAGS = \
		-version-info 1:0:0 \
		-release 0.7.0 \
		-module

libglobalmenu_gnome_la_CPPFLAGS = $(GLIB_CFLAGS) $(GTK_CFLAGS) $(GIO_CFLAGS)

EXTRA_DIST = $(STAMP) \
	gtkaqd.vapi \
	$(VALASOURCES) \
	$(NULL)

