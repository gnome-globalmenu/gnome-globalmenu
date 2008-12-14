NULL = 

STAMP = vala-stamp

BUILT_SOURCES = $(STAMP)
VALASOURCES = \
	applet.vala \
	main.vala \
	$(NULL)

libexec_PROGRAMS = GlobalMenu.PanelApplet

serverdir = $(BONOBO_SERVER_DIR)
server_in_files = GlobalMenu_PanelApplet.server.in
server_DATA = $(server_in_files:.server.in=.server)

$(server_DATA): $(server_in_files) Makefile
	sed -e "s|@LOCATION@|$(libexecdir)|" $< > $@

GlobalMenu_PanelApplet_SOURCES = \
	$(VALASOURCES:.vala=.c) \
	$(VALASOURCES:.vala=.h) \
	$(NULL)


$(STAMP): $(VALASOURCES) patch.sh libpanelapplet-2.0.vapi libpanelapplet-2.0.deps
	$(VALAC) -C --vapidir=. \
	--vapidir=$(top_srcdir)/libgnomenu \
	--pkg=gtk+-2.0 --pkg=libwnck-1.0 \
	--pkg=libpanelapplet-2.0 \
	--pkg=gnomenu-private \
	--pkg=gconf-2.0 \
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
		../libgnomenu/libgnomenu.la \
		$(NULL)
GlobalMenu_PanelApplet_CPPFLAGS = \
		$(GLIB_CFLAGS) \
		$(GTK_CFLAGS) \
		$(WNCK_CFLAGS) \
		$(PANEL_CFLAGS) \
		-I$(top_srcdir) \
		-include $(CONFIG_HEADER) \
		-DWNCK_I_KNOW_THIS_IS_UNSTABLE \
		-g \
		$(NULL)

EXTRA_DIST = \
	$(STAMP) \
	patch.sh \
	gnome-globalmenu-applet.schemas \
	libpanelapplet-2.0.vapi \
	libpanelapplet-2.0.deps \
	$(VALASOURCES) \
	$(server_in_files) \
	$(NULL)

DISTCLEANFILES = $(server_DATA)

schemadir   = @GCONF_SCHEMA_FILE_DIR@
schema_DATA = gnome-globalmenu-applet.schemas

if GCONF_SCHEMAS_INSTALL

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) gconftool-2 --makefile-install-rule $(schema_DATA)
else
install-data-local:
endif
