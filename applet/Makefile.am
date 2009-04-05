NULL = 

STAMP = vala-stamp

BUILT_SOURCES = $(STAMP)

VALASOURCES = \
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
	$(VALASOURCES:.vala=.h) \
	x-utils-c.c \
	$(NULL)

VALAFLAGS =
VALAPKGS = $(VALA_DEFAULT_PKGS) \
   		--pkg gtk+-2.0 \
		--pkg libwnck-1.0 \
		--pkg libpanelapplet-2.0 \
		--pkg gnome-desktop-2.0 \
		--pkg libgnome-menu \
		--pkg gconf-2.0 \
		--pkg gnomenu \
		--pkg libnotify \
		--pkg gdk-x11-2.0 

#@VALA_CCODE_RULES@
$(STAMP): $(VALASOURCES) 
	$(VALAC) -C $(VALA_DEFAULT_PKGS) --vapidir=. \
	--vapidir=$(top_srcdir)/libgnomenu \
	--pkg=gtk+-2.0 \
	--pkg=libwnck-1.0 \
	--pkg=libpanelapplet-2.0 \
	--pkg=gconf-2.0 \
	--pkg=gnomenu \
	--pkg=libnotify \
	--pkg=gdk-x11-2.0 \
	$(VALASOURCES) 
	touch $@

GlobalMenu_PanelApplet_LDFLAGS = \
		-g \
		-Wl,--as-needed \
		$(NULL)

GlobalMenu_PanelApplet_LDADD = \
		$(WNCK_LIBS) \
		$(GNOME_PANEL_LIBS) \
		../libcompat/libcompat.la \
		../libgnomenu/libgnomenu.la \
		$(NULL)
GlobalMenu_PanelApplet_CPPFLAGS = \
		$(WNCK_CFLAGS) \
		$(GNOME_PANEL_CFLAGS) \
		-I$(top_srcdir) \
		-include $(CONFIG_HEADER) \
		-include glib/gi18n-lib.h \
		-include ../libcompat/libcompat.h \
		-DWNCK_I_KNOW_THIS_IS_UNSTABLE \
		-g \
		$(NULL)

iconsdir = $(prefix)/share/pixmaps
icons_DATA = globalmenu.png


EXTRA_DIST = \
	$(STAMP) \
	$(VALASOURCES) \
	$(server_in_in_files) \
	$(icons_DATA) \
	$(NULL)

DISTCLEANFILES = $(server_DATA) \
				 $(noinst_DATA) \
				$(server_in_files)

