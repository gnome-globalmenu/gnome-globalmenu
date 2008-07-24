AM_CPPFLAGS = -I$(top_srcdir) \
				-DLOCALEDIR=\"${GNOME_GLOBALMENU_LOCALEDIR}\" 
AM_CPPFLAGS += -DGLADEDIR=\"$(pkgdatadir)\"
LIBS = $(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgtkhotkey/libgtkhotkey.la

libexec_PROGRAMS =

gladedir = $(pkgdatadir)
glade_DATA = application.glade GnomenuServerApplet.glade

@INTLTOOL_SERVER_RULE@
@INTLTOOL_SCHEMAS_RULE@
@INTLTOOL_DESKTOP_RULE@

bonobo_data     = GNOME_GlobalMenuApplet.server
schemas_data = gnome-globalmenu-applet.schemas
plugin_data = xfce-globalmenu-plugin.desktop

if ENABLE_GNOME_APPLET 
libexec_PROGRAMS += gnome-globalmenu-applet

APPLET_LOCATION = $(libexecdir)/gnome-globalmenu-applet
bonobodir       = $(libdir)/bonobo/servers
schemasdir       = $(sysconfdir)/gconf/schemas
bonobo_DATA = $(bonobo_data)
schemas_DATA = $(schemas_data)
endif
if ENABLE_XFCE_PLUGIN
libexec_PROGRAMS += xfce-globalmenu-plugin
PLUGIN_LOCATION = $(libexecdir)/xfce-globalmenu-plugin
plugindir = $(datadir)/xfce4/panel-plugins
plugin_DATA = $(plugin_data)
endif


$(bonobo_data:%=%.in): $(bonobo_data:%=%.in.in) Makefile
	sed -e "s|\@LOCATION\@|$(APPLET_LOCATION)|" \
	    $< > $@
$(plugin_data:%=%.in): $(plugin_data:%=%.in.in) Makefile
	sed -e "s|\@LOCATION\@|$(PLUGIN_LOCATION)|" \
	    $< > $@


DISTCLEANFILES = 
VAST_SOURCE_CODE = \
	intl.h \
	log.h 

gnome_globalmenu_applet_SOURCES = \
	main-gnome.c \
	$(VAST_SOURCE_CODE)
gnome_globalmenu_applet_CFLAGS = $(GNOME_GLOBALMENU_APPLET_CFLAGS)
gnome_globalmenu_applet_LDADD = $(GNOME_GLOBALMENU_APPLET_LIBS)

xfce_globalmenu_plugin_SOURCES = \
	main-xfce.c \
	$(VAST_SOURCE_CODE)
xfce_globalmenu_plugin_CFLAGS = $(XFCE_GLOBALMENU_PLUGIN_CFLAGS)
xfce_globalmenu_plugin_LDADD = $(XFCE_GLOBALMENU_PLUGIN_LIBS)

DISTCLEANFILES += $(bonobo_data) $(bonobo_data:%=%.in) $(plugin_data) $(plugin_data:%=%.in) $(schemas_data)

EXTRA_DIST = \
		$(bonobo_data:%=%.in.in) \
		$(schemas_data:%=%.in) \
		$(plugin_data:%=%.in.in) \
		$(glade_DATA)

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		gconftool-2 --makefile-install-rule $(schemas_data)
