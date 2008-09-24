NULL = 

AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(srcdir)/.. \
	-I$(builddir)/.. \
	$(NULL)

BUILT_SOURCES = .vala.stamp

gnomenu_server_VALASOURCES = \
	gnomenu-server.vala
	$(NULL)

bin_PROGRAMS = \
	gnomenu-server \
	$(NULL)

servicedir = $(DBUS_SERVICES_DIR)
service_in_files = org.gnome.GlobalMenu.Server.service.in
service_DATA = $(service_in_files:.service.in=.service)

$(service_DATA): $(service_in_files) Makefile
	sed -e "s|@LOCATION@|$(bindir)|" $< > $@

gnomenu_server_SOURCES = \
	$(gnomenu_server_VALASOURCES:.vala=.c) \
	$(gnomenu_server_VALASOURCES:.vala=.h) \
	$(NULL)

.vala.stamp: $(gnomenu_server_VALASOURCES)
	$(VALAC) -C $(LIBGNOMENU_VALA_PKGS) --vapidir=$(srcdir)/.. --vapidir=$(builddir)/.. --pkg gnomenu --pkg valacompat $^
	touch $@

gnomenu_server_LDADD = \
		$(LIBGNOMENU_LIBS) \
		$(builddir)/../libgnomenu.la \
		$(NULL)

EXTRA_DIST = .vala.stamp	
