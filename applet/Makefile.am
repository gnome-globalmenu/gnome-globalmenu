NULL = 

STAMP = vala-stamp

BUILT_SOURCES = $(STAMP) $(server_in_files)
VALASOURCES = \
	applet.vala \
	main.vala \
	gtkextra-gconfdialog.vala \
	switcher.vala \
	gnomemenuhelper.vala \
	wncktools.vala \
	$(NULL)

libexec_PROGRAMS = GlobalMenu.PanelApplet

serverdir = $(BONOBO_SERVER_DIR)
server_in_in_files = GlobalMenu_PanelApplet.server.in.in
server_in_files = GlobalMenu_PanelApplet.server.in
server_DATA = $(server_in_files:.server.in=.server)
@INTLTOOL_SERVER_RULE@

$(server_in_files): $(server_in_in_files) Makefile
	sed -e "s|@LOCATION@|$(libexecdir)|" $< > $@

GlobalMenu_PanelApplet_SOURCES = \
	$(VALASOURCES:.vala=.c) \
	$(VALASOURCES:.vala=.h) \
	task-utils-c.c \
	wncktools-ccode.c \
	$(NULL)


$(STAMP): $(VALASOURCES) patch.sh libpanelapplet-2.0.vapi libpanelapplet-2.0.deps
	$(VALAC) -C --vapidir=. \
	--vapidir=$(top_srcdir)/libgnomenu \
	--pkg=gtk+-2.0 \
	--pkg=libwnck-1.0 \
	--pkg=libpanelapplet-2.0 \
	--pkg=gnomenu-private \
	--pkg=gnome-desktop-2.0 \
	--pkg=libgnome-menu \
	--pkg=gconf-2.0 \
	--pkg=gconfcompat \
	--pkg=wnckcompat \
	$(VALASOURCES)
	bash patch.sh
	touch $@

GlobalMenu_PanelApplet_LDFLAGS = \
		-g \
		$(NULL)

GlobalMenu_PanelApplet_LDADD = \
		$(GLIB_LIBS) \
		$(GTK_LIBS) \
		$(WNCK_LIBS) \
		$(PANEL_LIBS) \
		$(GMENU_LIBS) \
		../libgnomenu/libgnomenu.la \
		$(NULL)
GlobalMenu_PanelApplet_CPPFLAGS = \
		$(GLIB_CFLAGS) \
		$(GTK_CFLAGS) \
		$(WNCK_CFLAGS) \
		$(PANEL_CFLAGS) \
		$(GMENU_CFLAGS) \
		-I$(top_srcdir) \
		-include $(CONFIG_HEADER) \
		-DGMENU_I_KNOW_THIS_IS_UNSTABLE \
		-DWNCK_I_KNOW_THIS_IS_UNSTABLE \
		-g \
		$(NULL)

iconsdir = $(prefix)/share/pixmaps
icons_DATA = globalmenu.png


EXTRA_DIST = \
	$(STAMP) \
	patch.sh \
	libpanelapplet-2.0.vapi \
	libpanelapplet-2.0.deps \
	$(VALASOURCES) \
	$(server_in_in_files) \
	$(icons_DATA) \
	$(NULL)

DISTCLEANFILES = $(server_DATA) $(server_in_files)

