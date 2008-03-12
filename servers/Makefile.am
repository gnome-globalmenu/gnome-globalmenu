AM_CPPFLAGS = -I$(top_srcdir) \
				-DLOCALEDIR=\"${GNOME_GLOBALMENU_LOCALEDIR}\"

#if with xfce, libexec_PROGRAM= xfce-globalmenu-plugin or whatever.

libexec_PROGRAMS =
bin_PROGRAMS = 
if ENABLE_GNOME_APPLET 
libexec_PROGRAMS += gnome-globalmenu-applet

APPLET_LOCATION = $(libexecdir)/gnome-globalmenu-applet

#serverdir       = $(libdir)/bonobo/servers
serverdir = $(BONOBO_SERVER_FILES_PATH)
server_in_files = GNOME_GlobalMenuApplet.server.in
server_DATA     = $(server_in_files:.server.in=.server)

$(server_in_files): $(server_in_files:.server.in=.server.in.in)
	sed -e "s|\@LOCATION\@|$(APPLET_LOCATION)|" \
	    $< > $@
@INTLTOOL_SERVER_RULE@

#schemadir= @GCONF_SCHEMA_FILE_DIR@
#schema_DATA= gnome-globalmenu-applet.schemas

schemasdir       = $(sysconfdir)/gconf/schemas
schemas_in_files = gnome-globalmenu-applet.schemas.in
schemas_DATA     = $(schemas_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

endif
if ENABLE_XFCE_PLUGIN
libexec_PROGRAMS += xfce-globalmenu-plugin
PLUGIN_LOCATION = $(libexecdir)/xfce-globalmenu-plugin
#desktopdir = $(datadir)/xfce4/panel-plugins
desktopdir = $(XFCE_DESKTOP_FILES_PATH)
desktop_in_files = xfce-globalmenu-plugin.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
$(desktop_in_files): $(desktop_in_files:.desktop.in=.desktop.in.in)
	sed -e "s|\@LOCATION\@|$(PLUGIN_LOCATION)|" \
	    $< > $@
@INTLTOOL_DESKTOP_RULE@

endif

if ENABLE_STANDALONE
bin_PROGRAMS += gtk-globalmenu-server
endif


DISTCLEANFILES = 
VAST_SOURCE_CODE = application.h \
	application.c \
	menuserver.h \
	menuserver.c \
	intl.h \
	utils.c \
	utils.h \
	log.h

gnome_globalmenu_applet_SOURCES = \
	main-gnome.c \
	application-gnome.c \
	application-gnome.h \
	$(VAST_SOURCE_CODE)
gnome_globalmenu_applet_CFLAGS = \
	$(GNOME_GLOBALMENU_APPLET_CFLAGS)
gnome_globalmenu_applet_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la \
				$(GNOME_GLOBALMENU_APPLET_LIBS)

xfce_globalmenu_plugin_SOURCES = \
	main-xfce.c \
	application-xfce.c \
	$(VAST_SOURCE_CODE)
xfce_globalmenu_plugin_CFLAGS =\
	$(XFCE_GLOBALMENU_PLUGIN_CFLAGS)
xfce_globalmenu_plugin_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la	\
				$(XFCE_GLOBALMENU_PLUGIN_LIBS)

gtk_globalmenu_server_SOURCES = \
	main.c \
	$(VAST_SOURCE_CODE)
gtk_globalmenu_server_CFLAGS =\
	$(GLOBALMENU_SERVER_CFLAGS)
gtk_globalmenu_server_LDADD = $(top_builddir)/libgnomenu/libgnomenu.la	\
				$(GLOBALMENU_SERVER_LIBS)


				
DISTCLEANFILES += $(server_file_DATA) GNOME_GlobalMenuApplet.server.in xfce-globalmenu-plugin.desktop.in xfce-globalmenu-plugin.desktop gnome-globalmenu-applet.schemas

EXTRA_DIST = make-xfce-data.sh \
	gnome-globalmenu-applet.schemas.in \
	GNOME_GlobalMenuApplet.server.in.in \
	xfce-globalmenu-plugin.desktop.in.in

#xfce-globalmenu-plugin.desktop:
#	/bin/sh $(srcdir)/make-xfce-data.sh $@ $(libexecdir)


install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		gconftool-2 --makefile-install-rule $(schemas_DATA)
