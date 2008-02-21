INCLUDES = -I$(top_srcdir) \
	$(GNOME_GLOBALMENU_APPLET_CFLAGS)

libexec_PROGRAMS = gnome-globalmenu-applet

DISTCLEANFILES = 
gnome_globalmenu_applet_SOURCES = \
	main.c \
	application.h

gnome_globalmenu_applet_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la \
				$(GNOME_GLOBALMENU_APPLET_LIBS)

server_filedir = `pkg-config --variable=libdir bonobo-activation-2.0`/bonobo/servers
server_file_DATA = GNOME_GlobalMenuApplet.server

DISTCLEANFILES += $(server_file_DATA)

GNOME_GlobalMenuApplet.server:
	./make-server-file.sh $(libexecdir) > $@

