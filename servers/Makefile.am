AM_CFLAGS = -I$(top_srcdir)

#if with xfce, libexec_PROGRAM= xfce-globalmenu-plugin or whatever.

libexec_PROGRAMS = gnome-globalmenu-applet
bin_PROGRAMS = globalmenu-server

DISTCLEANFILES = 
VAST_SOURCE_CODE = application.h \
	application.c \
	log.h

gnome_globalmenu_applet_SOURCES = \
	main-gnome.c \
	$(VAST_SOURCE_CODE)
gnome_globalmenu_applet_CFLAGS = \
	$(GNOME_GLOBALMENU_APPLET_CFLAGS)

globalmenu_server_SOURCES = \
	main.c \
	$(VAST_SOURCE_CODE)
globalmenu_server_CFLAGS =\
	$(GLOBALMENU_SERVER_CFLAGS)

gnome_globalmenu_applet_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la \
				$(GNOME_GLOBALMENU_APPLET_LIBS)
globalmenu_server_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la	\
				$(GLOBALMENU_SERVER_LIBS)
				
server_filedir = `pkg-config --variable=libdir bonobo-activation-2.0`/bonobo/servers
server_file_DATA = GNOME_GlobalMenuApplet.server

DISTCLEANFILES += $(server_file_DATA)

GNOME_GlobalMenuApplet.server:
	./make-server-file.sh $(libexecdir) > $@

