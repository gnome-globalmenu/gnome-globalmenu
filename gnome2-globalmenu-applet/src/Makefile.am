## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/gnome2-globalmenu-applet/glade
INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GNOME2_GLOBALMENU_APPLET_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

libexec_PROGRAMS = gnome2-globalmenu-applet 

gnome2_globalmenu_applet_SOURCES = \
	main.c                 \
	misc.c                 \
	misc.h                 \
	typedefs.h                 \
	ui.c                 \
	ui.h                 \
	menuclients.c                 \
	menuclients.h                 \
	menuserver.c                 \
	menuserver.h                 \
	gtkmenuembed-x11.h 

gnome2_globalmenu_applet_LDFLAGS = 

gnome2_globalmenu_applet_LDADD = $(GNOME2_GLOBALMENU_APPLET_LIBS)

EXTRA_DIST = $(glade_DATA)

























