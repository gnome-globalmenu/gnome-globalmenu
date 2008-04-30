AM_CPPFLAGS = \
	$(TESTS_CFLAGS) \
	-g \
	-include $(CONFIG_HEADER) \
	-I../libgnomenu  \
	$(NULL)

LDADDS = $(top_builddir)/libgnomenu/libgnomenu.la $(TESTS_LIBS)

BUILT_SOURCES = tests.vala.stamp \
		$(gnomenu_service_VALASOURCES:.vala=.c) \
		$(gnomenu_service_VALASOURCES:.vala=.h) \
		$(NULL)

noinst_PROGRAMS = app manager

gnomenu_service_VALASOURCES = \
		app.vala \
		manager.vala \
		$(NULL)

tests.vala.stamp: $(gnomenu_service_VALASOURCES)
	$(VALAC) -C $(TESTS_VALA_PKGS) --vapidir=../libgnomenu --pkg gnomenu $^
	touch $@

sanity_LDADD = $(LDADDS)
sanity_SOURCES = sanity.c sanity.h
app_LDADD = $(LDADDS)
app_SOURCES = app.c app.h
manager_LDADD = $(LDADDS)
manager_SOURCES = manager.c manager.h
EXTRA_DIST = tests.vala.stamp	
