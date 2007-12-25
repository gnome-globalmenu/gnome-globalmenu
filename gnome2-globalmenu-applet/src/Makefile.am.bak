## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/gnome2-globalmenu-applet/glade
glade_DATA = gnome2-globalmenu-applet.glade

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GNOME2_GLOBALMENU_APPLET_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gnome2-globalmenu-applet 

gnome2_globalmenu_applet_SOURCES = \
	clients.c                 \
	clients.h                 \
	main.c                 \
	misc.c                 \
	misc.h                 \
	typedefs.h 

gnome2_globalmenu_applet_LDFLAGS = 

gnome2_globalmenu_applet_LDADD = $(GNOME2_GLOBALMENU_APPLET_LIBS)

EXTRA_DIST = $(glade_DATA)















