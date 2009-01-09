
if WITH_XFCE4_PANEL
libexec_PROGRAMS = GlobalMenu.XFCEPlugin
desktopfiledir = $(datadir)/xfce4/panel-plugins
desktopfile_DATA = $(desktopfile_in_files:.desktop.in=.desktop)
endif
desktopfile_in_in_files = GlobalMenu_XFCEPlugin.desktop.in.in
desktopfile_in_files = GlobalMenu_XFCEPlugin.desktop.in
@INTLTOOL_DESKTOP_RULE@
$(desktopfile_in_files): $(desktopfile_in_in_files) Makefile
	sed -e "s|@LOCATION@|$(libexecdir)/xfce4/panel-plugins|" $< > $@

GlobalMenu_XFCEPlugin_SOURCES = main.c

AM_CPPFLAGS = $(XFCE4_PANEL_CFLAGS)
AM_LDFLAGS = $(XFCE4_PANEL_LIBS)

EXTRA_DIST = \
	$(desktopfile_in_in_files)	 
