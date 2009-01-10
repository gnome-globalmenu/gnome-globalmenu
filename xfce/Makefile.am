
if WITH_XFCE4_PANEL
GlobalMenu_XFCEPlugin_PROGRAMS = GlobalMenu.XFCEPlugin
GlobalMenu_XFCEPlugindir = $(libexecdir)/xfce4/panel-plugins
desktopfiledir = $(datadir)/xfce4/panel-plugins
desktopfile_DATA = $(desktopfile_in_files:.desktop.in=.desktop)
endif
desktopfile_in_in_files = GlobalMenu_XFCEPlugin.desktop.in.in
desktopfile_in_files = GlobalMenu_XFCEPlugin.desktop.in
@INTLTOOL_DESKTOP_RULE@
$(desktopfile_in_files): $(desktopfile_in_in_files) Makefile
	sed -e "s|@LOCATION@|$(libexecdir)/xfce4/panel-plugins|" $< > $@

GlobalMenu_XFCEPlugin_SOURCES = main.c
GlobalMenu_XFCEPlugin_CPPFLAGS = $(XFCE4_PANEL_CFLAGS) \
								 $(WNCK_CFLAGS) \
						-I$(top_srcdir) \
						-include $(CONFIG_HEADER) \
						-DGMENU_I_KNOW_THIS_IS_UNSTABLE \
						-DWNCK_I_KNOW_THIS_IS_UNSTABLE \
						-g 

GlobalMenu_XFCEPlugin_LDADD = $(XFCE4_PANEL_LIBS) \
			$(WNCK_LIBS) \
			../libgnomenu/libgnomenu.la \
		   ../applet/libapplet.la

EXTRA_DIST = \
	$(desktopfile_in_in_files)	 
