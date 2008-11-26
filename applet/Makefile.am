NULL = 

AM_CPPFLAGS = \
	-g \
	$(GLOBALMENU_PANELAPPLET_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/libgmarkupdoc \
	-I$(top_srcdir)/libgnomenu \
	-DPREFIX=\"$(prefix)\" \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DDATADIR=\"$(datadir)\" \
	-DLIBDIR=\"$(libdir)\" \
	-DWNCK_I_KNOW_THIS_IS_UNSTABLE
	$(NULL)

AM_VALAC_FLAGS = \
	$(GLOBALMENU_PANELAPPLET_VALA_PKGS) \
	--vapidir=$(top_srcdir)/libgmarkupdoc  \
	--vapidir=$(top_srcdir)/libgnomenu  \
	--vapidir=$(top_srcdir)/applet  \
	--pkg gnomenu  \
	--pkg gtkcompat  \
	--pkg gmarkupdoc  \
	--pkg wnckcompat  \
	--pkg panelappletcompat \
	--pkg gconf-2.0 \
	--pkg gdk-2.0 \
	$(NULL)
	
BUILT_SOURCES = \
	.vala.stamp \
	$(GlobalMenu_PanelApplet_VALASOURCES:.vala=.c) \
	$(GlobalMenu_PanelApplet_VALASOURCES:.vala=.h) \
	$(NULL)

GlobalMenu_PanelApplet_VALASOURCES = \
	panelextra-menubar.vala \
	panelextra-switcher.vala \
	GPL.vala \
	applet.vala \
	$(NULL)

libexec_PROGRAMS = \
	GlobalMenu.PanelApplet \
	$(NULL)

serverdir = $(BONOBO_SERVER_DIR)
server_in_files = GlobalMenu_PanelApplet.server.in
server_DATA = $(server_in_files:.server.in=.server)

$(server_DATA): $(server_in_files) Makefile
	sed -e "s|@LOCATION@|$(libexecdir)|" $< > $@

GlobalMenu_PanelApplet_SOURCES = \
	$(GlobalMenu_PanelApplet_VALASOURCES:.vala=.c) \
	$(GlobalMenu_PanelApplet_VALASOURCES:.vala=.h) \
	applet-c.c \
	$(NULL)

.vala.stamp: $(GlobalMenu_PanelApplet_VALASOURCES)
	$(VALAC) -C  $^
	touch $@

GlobalMenu_PanelApplet_LDADD = \
		$(GLOBALMENU_PANELAPPLET_LIBS) \
		$(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(GlobalMenu_PanelApplet_VALASOURCES) \
	$(server_in_files) \
	$(NULL)
DISTCLEANFILES = $(server_DATA)
