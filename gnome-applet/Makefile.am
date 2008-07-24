AM_CPPFLAGS = -I$(top_srcdir) \
				-DLOCALEDIR=\"${GNOME_GLOBALMENU_LOCALEDIR}\" 
AM_CPPFLAGS += -DGLADEDIR=\"$(pkgdatadir)\"
LIBS = $(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgtkhotkey/libgtkhotkey.la


gladedir = $(pkgdatadir)
glade_DATA = GnomenuServerApplet.glade

@INTLTOOL_SERVER_RULE@
@INTLTOOL_SCHEMAS_RULE@
@INTLTOOL_DESKTOP_RULE@

bonobo_data     = GNOME_GlobalMenuApplet.server
schemas_data = gnome-globalmenu-applet.schemas

if ENABLE_GNOME_APPLET 
libexec_PROGRAMS = gnome-globalmenu-applet

APPLET_LOCATION = $(libexecdir)/gnome-globalmenu-applet
bonobodir       = $(libdir)/bonobo/servers
schemasdir       = $(sysconfdir)/gconf/schemas
bonobo_DATA = $(bonobo_data)
schemas_DATA = $(schemas_data)
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


DISTCLEANFILES += $(bonobo_data) $(bonobo_data:%=%.in) $(schemas_data)

EXTRA_DIST = \
		$(bonobo_data:%=%.in.in) \
		$(schemas_data:%=%.in) \
		$(glade_DATA)

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		gconftool-2 --makefile-install-rule $(schemas_data) || echo Failed
