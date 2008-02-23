AM_CFLAGS = -I$(top_srcdir)

#if with xfce, libexec_PROGRAM= xfce-globalmenu-plugin or whatever.

libexec_PROGRAMS =
if ENABLE_GNOME_APPLET 
libexec_PROGRAMS += gnome-globalmenu-applet
gnome_filedir = `pkg-config --variable=libdir bonobo-activation-2.0`/bonobo/servers
gnome_file_DATA = GNOME_GlobalMenuApplet.server
endif
if ENABLE_XFCE_PLUGIN
libexec_PROGRAMS += xfce-globalmenu-plugin
xfce_filedir = `pkg-config --variable=desktopdatadir libxfce4panel-1.0`
xfce_file_DATA = xfce-globalmenu-plugin.desktop
endif

if ENABLE_STANDALONE
libexec_PROGRAMS += globalmenu-server
endif

DISTCLEANFILES = 
VAST_SOURCE_CODE = application.h \
	application.c \
	menuserver.h \
	menuserver.c \
	log.h

gnome_globalmenu_applet_SOURCES = \
	main-gnome.c \
	$(VAST_SOURCE_CODE)
gnome_globalmenu_applet_CFLAGS = \
	$(GNOME_GLOBALMENU_APPLET_CFLAGS)
gnome_globalmenu_applet_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la \
				$(GNOME_GLOBALMENU_APPLET_LIBS)

xfce_globalmenu_plugin_SOURCES = \
	main-xfce.c \
	$(VAST_SOURCE_CODE)
xfce_globalmenu_plugin_CFLAGS =\
	$(XFCE_GLOBALMENU_PLUGIN_CFLAGS)
xfce_globalmenu_plugin_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la	\
				$(XFCE_GLOBALMENU_PLUGIN_LIBS)

globalmenu_server_SOURCES = \
	main.c \
	$(VAST_SOURCE_CODE)
globalmenu_server_CFLAGS =\
	$(GLOBALMENU_SERVER_CFLAGS)
globalmenu_server_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la	\
				$(GLOBALMENU_SERVER_LIBS)


				
DISTCLEANFILES += $(server_file_DATA)

GNOME_GlobalMenuApplet.server:
	./make-gnome-data.sh $@ $(libexecdir)
xfce-globalmenu-plugin.desktop:
	./make-xfce-data.sh $@ $(libexecdir)
