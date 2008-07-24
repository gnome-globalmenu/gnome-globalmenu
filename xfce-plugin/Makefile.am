AM_CPPFLAGS = -I$(top_srcdir) \
				-DLOCALEDIR=\"${GNOME_GLOBALMENU_LOCALEDIR}\" 
AM_CPPFLAGS += -DGLADEDIR=\"$(pkgdatadir)\"
LIBS = $(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgtkhotkey/libgtkhotkey.la


@INTLTOOL_SERVER_RULE@
@INTLTOOL_SCHEMAS_RULE@
@INTLTOOL_DESKTOP_RULE@

plugin_data = xfce-globalmenu-plugin.desktop

if ENABLE_XFCE_PLUGIN
libexec_PROGRAMS = xfce-globalmenu-plugin
PLUGIN_LOCATION = $(libexecdir)/xfce-globalmenu-plugin
plugindir = $(datadir)/xfce4/panel-plugins
plugin_DATA = $(plugin_data)
endif

$(plugin_data:%=%.in): $(plugin_data:%=%.in.in) Makefile
	sed -e "s|\@LOCATION\@|$(PLUGIN_LOCATION)|" \
	    $< > $@

DISTCLEANFILES = 
VAST_SOURCE_CODE = \
	intl.h \
	log.h 


xfce_globalmenu_plugin_SOURCES = \
	main-xfce.c \
	$(VAST_SOURCE_CODE)
xfce_globalmenu_plugin_CFLAGS = $(XFCE_GLOBALMENU_PLUGIN_CFLAGS)
xfce_globalmenu_plugin_LDADD = $(XFCE_GLOBALMENU_PLUGIN_LIBS)

DISTCLEANFILES += $(plugin_data) $(plugin_data:%=%.in) $(schemas_data)

EXTRA_DIST = \
		$(plugin_data:%=%.in.in)
