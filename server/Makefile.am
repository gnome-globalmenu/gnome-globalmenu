NULL = 

AM_CPPFLAGS = \
	-g \
	$(ORG_GNOME_GLOBALMENU_SERVER_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/libgmarkupdoc \
	-I$(top_builddir)/libgmarkupdoc \
	-I$(top_srcdir)/libgnomenu \
	-I$(top_builddir)/libgnomenu \
	$(NULL)

AM_VALAC_FLAGS = \
	$(ORG_GNOME_GLOBALMENU_SERVER_VALA_PKGS) \
	--vapidir=$(top_srcdir)/libgmarkupdoc \
	--vapidir=$(top_builddir)/libgmarkupdoc \
	--vapidir=$(top_srcdir)/libgnomenu \
	--vapidir=$(top_builddir)/libgnomenu  \
	--pkg=gnomenu  \
	--pkg=gmarkupdoc  \
	$(NULL)

BUILT_SOURCES = \
	.vala.stamp \
	$(org_gnome_GlobalMenu_Server_VALASOURCES:.vala=.c) \
	$(org_gnome_GlobalMenu_Server_VALASOURCES:.vala=.h) \
	$(NULL)

org_gnome_GlobalMenu_Server_VALASOURCES = \
	org.gnome.GlobalMenu.Server.vala \
	$(NULL)

bin_PROGRAMS = \
	org.gnome.GlobalMenu.Server \
	$(NULL)

servicedir = $(DBUS_SERVICES_DIR)
service_in_files = org.gnome.GlobalMenu.Server.service.in
service_DATA = $(service_in_files:.service.in=.service)

$(service_DATA): $(service_in_files) Makefile
	sed -e "s|@LOCATION@|$(bindir)|" $< > $@

org_gnome_GlobalMenu_Server_SOURCES = \
	$(org_gnome_GlobalMenu_Server_VALASOURCES:.vala=.c) \
	$(org_gnome_GlobalMenu_Server_VALASOURCES:.vala=.h) \
	$(NULL)

.vala.stamp: $(org_gnome_GlobalMenu_Server_VALASOURCES)
	$(VALAC) -C  $^
	touch $@

org_gnome_GlobalMenu_Server_LDADD = \
		$(ORG_GNOME_GLOBALMENU_SERVER_LIBS) \
		$(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(service_in_files) \
	$(org_gnome_GlobalMenu_Server_VALASOURCES) \
	$(NULL)
DISTCLEANFILES = $(service_DATA)
