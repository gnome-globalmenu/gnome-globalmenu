NULL = 

AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(srcdir)/.. \
	-I$(builddir)/.. \
	$(NULL)

BUILT_SOURCES = .vala.stamp

org_gnome_GlobalMenu_Server_VALASOURCES = \
	server.vala \
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
	$(VALAC) -C $(LIBGNOMENU_VALA_PKGS) --vapidir=$(srcdir)/.. --vapidir=$(builddir)/.. --pkg gnomenu --pkg valacompat $^
	touch $@

org_gnome_GlobalMenu_Server_LDADD = \
		$(LIBGNOMENU_LIBS) \
		$(builddir)/../libgnomenu.la \
		$(NULL)

EXTRA_DIST = .vala.stamp $(org_gnome_GlobalMenu_Server_VALASOURCES) $(service_in_files)
DISTCLEANFILES = $(service_DATA)
