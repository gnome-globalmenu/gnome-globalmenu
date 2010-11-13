NULL = 

BUILT_SOURCES = vala-ccode

VALASOURCES = \
	application.vala \
	applet.vala \
	main.vala \
	gtkextra-gconfdialog.vala \
	switcher.vala \
	template.vala \
	workspaceselector.vala \
	$(NULL)

if WITH_GNOME_PANEL
libexec_PROGRAMS = GlobalMenu.PanelApplet

serverdir = $(BONOBO_SERVER_DIR)
server_DATA = $(server_in_files:.server.in=.server)
else
noinst_DATA = $(server_in_files:.server.in=.server)
endif

server_in_in_files = GlobalMenu_PanelApplet.server.in.in
server_in_files = GlobalMenu_PanelApplet.server.in
@INTLTOOL_SERVER_RULE@

$(server_in_files): $(server_in_in_files) Makefile
	sed -e "s|@LOCATION@|$(libexecdir)|" $< > $@


GlobalMenu_PanelApplet_SOURCES = \
	$(VALASOURCES:.vala=.c) \
	x-utils-c.c \
	task-utils-c.c \
	$(NULL)

vala-ccode : $(VALASOURCES)
	$(VALAC) --enable-deprecated -C $(VALA_DEFAULT_PKGS)  \
		--vapidir $(top_srcdir)/libserver \
		--vapidir $(top_srcdir)/libsettings \
		--pkg gtk+-2.0 \
		--pkg libwnck-1.0 \
		--pkg libpanelapplet-2.0 \
		--pkg gconf-2.0 \
		--pkg globalmenu-server \
		--pkg globalmenu-settings \
		--pkg libgnome-menu \
		--pkg libnotify \
		--pkg gdk-x11-2.0 \
		$(VALASOURCES) 
	touch $@

GlobalMenu_PanelApplet_LDFLAGS = \
		-g \
		-Wl,--as-needed \
		$(NULL)

GlobalMenu_PanelApplet_LDADD = \
		$(GTK_LIBS) \
		$(WNCK_LIBS) \
		$(GNOME_PANEL_LIBS) \
		$(GMENU_LIBS) \
		$(top_builddir)/libserver/libglobalmenu-server.la \
		$(top_builddir)/libsettings/libsettings.la \
		$(NULL)

GlobalMenu_PanelApplet_CPPFLAGS = \
		$(WNCK_CFLAGS) \
		$(GMENU_CFLAGS) \
		$(GNOME_PANEL_CFLAGS) \
		-I$(top_srcdir)/libserver \
		-I$(top_srcdir)/libsettings \
		-include $(CONFIG_HEADER) \
		-include glib/gi18n-lib.h \
		-include ../legacy-support/legacy-support.h \
		-g \
		$(NULL)

iconsdir = $(prefix)/share/pixmaps
icons_DATA = globalmenu.png


EXTRA_DIST = \
	vala-ccode \
	$(VALASOURCES) \
	$(server_in_in_files) \
	$(icons_DATA) \
	$(NULL)

DISTCLEANFILES = $(server_DATA) \
				 $(noinst_DATA) \
				$(server_in_files)

