
if WITH_XFCE4_PANEL
GlobalMenu_XFCEPlugin_PROGRAMS = GlobalMenu.XFCEPlugin
GlobalMenu_XFCEPlugindir = $(XFCE4_PANEL_PLUGIN_DIR))
desktopfiledir = $(XFCE4_PANEL_DESKTOP_DIR)
desktopfile_DATA = $(desktopfile_in_files:.desktop.in=.desktop)
iconsdir = $(datadir)/pixmaps
icons_DATA = globalmenu-xfce.png
else
noinst_DATA = $(desktopfile_in_files:.desktop.in=.desktop)
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
			../libgnomenu/libgnomenu.la

EXTRA_DIST = \
	$(desktopfile_in_in_files)	\
	globalmenu-xfce.png 
DISTCLEANFILES =  \
	$(desktopfile_in_files)	  \
	$(desktopfile_DATA)	 \
	$(noinst_DATA)
