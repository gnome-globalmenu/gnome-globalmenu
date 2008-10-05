NULL = 

AM_CPPFLAGS = \
	-g \
	$(GLOBAL_MENU_APPLET_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/libgmarkupdoc \
	-I$(top_builddir)/libgmarkupdoc \
	-I$(top_srcdir)/libgnomenu \
	-I$(top_builddir)/libgnomenu \
	-DPREFIX=\"$(prefix)\" \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DDATADIR=\"$(datadir)\" \
	-DLIBDIR=\"$(libdir)\" \
	-DWNCK_I_KNOW_THIS_IS_UNSTABLE
	$(NULL)

AM_VALAC_FLAGS = \
	$(GLOBAL_MENU_APPLET_VALA_PKGS) \
	--vapidir=$(top_srcdir)/libgmarkupdoc  \
	--vapidir=$(top_builddir)/libmarkupdoc  \
	--vapidir=$(top_srcdir)/libgnomenu  \
	--vapidir=$(top_builddir)/libgnomenu  \
	--pkg gnomenu  \
	--pkg gmarkupdoc  \
	--pkg wnckcompat  \
	$(NULL)
	
BUILT_SOURCES = .vala.stamp

GlobalMenu_PanelApplet_VALASOURCES = \
	applet.vala \
	$(NULL)

libexec_PROGRAMS = \
	GlobalMenu.PanelApplet \
	$(NULL)

serverdir = $(BONOBO_SERVERS_DIR)
server_in_files = GlobalMenu_PanelApplet.server.in
server_DATA = $(server_in_files:.server.in=.server)

$(server_DATA): $(server_in_files) Makefile
	sed -e "s|@LOCATION@|$(libexecdir)/$(libexec_PROGRAMS)|" $< > $@

GlobalMenu_PanelApplet_SOURCES = \
	$(GlobalMenu_PanelApplet_VALASOURCES:.vala=.c) \
	$(GlobalMenu_PanelApplet_VALASOURCES:.vala=.h) \
	applet-c.c \
	$(NULL)

.vala.stamp: $(GlobalMenu_PanelApplet_VALASOURCES)
	$(VALAC) -C  $^
	touch $@

GlobalMenu_PanelApplet_LDADD = \
		$(GLOBAL_MENU_APPLET_LIBS) \
		$(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

EXTRA_DIST = .vala.stamp	$(GlobalMenu_PanelApplet_VALASOURCES)
