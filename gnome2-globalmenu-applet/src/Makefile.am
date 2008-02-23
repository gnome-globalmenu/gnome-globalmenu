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

libexec_PROGRAMS = gnome2-globalmenu-applet xfce4-globalmenu-plugin
xfce4_globalmenu_plugin_SOURCES = 

gnome2_globalmenu_applet_SOURCES = \
	main.c                 \
	typedefs.h                 \
	ui.c                 \
	ui.h                 \
	menuclients.c                 \
	menuclients.h                 \
	menuserver.c                 \
	menuserver.h                 \
	gtkmenuembed-x11.h                 \
	application.h 		\
	preference.h	\
	preference.c

xfce4-globalmenu-plugin: gnome2-globalmenu-applet
	ln -s gnome2-globalmenu-applet xfce4-globalmenu-plugin

#gtkmenuembed-x11.h: ../../gtk2-aqd/gtkmenuembed-x11.h
#	cp ../../gtk2-aqd/gtkmenuembed-x11.h $@ 


gnome2_globalmenu_applet_LDFLAGS = 

gnome2_globalmenu_applet_LDADD = $(GNOME2_GLOBALMENU_APPLET_LIBS)

EXTRA_DIST = $(glade_DATA)




























