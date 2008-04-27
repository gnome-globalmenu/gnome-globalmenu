AM_CPPFLAGS = \
	$(GNOMENU_SERVICE_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = service.vala.stamp

sbin_PROGRAMS = gnomenu-service

gnomenu_service_VALASOURCES = \
		main.vala \
		menu.vala \
		menuitem.vala \
		$(NULL)

gnomenu_service_SOURCES = \
		$(gnomenu_service_VALASOURCES:.vala=.c) \
		$(gnomenu_service_VALASOURCES:.vala=.h) \
		$(NULL)

service.vala.stamp: $(gnomenu_service_VALASOURCES)
	$(VALAC) -C $(GNOMENU_SERVICE_VALA_PKGS) $^
	touch $@

gnomenu_service_LDADD = \
		$(GNOMENU_SERVICE_LIBS) \
		$(NULL)

EXTRA_DIST = service.vala.stamp	
