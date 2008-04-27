AM_CPPFLAGS = \
	$(TESTS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I../libgnomenu  \
	$(NULL)

LDADDS = $(top_builddir)/libgnomenu/libgnomenu.la $(TEST_LIBS)

BUILT_SOURCES = tests.vala.stamp \
		$(gnomenu_service_VALASOURCES:.vala=.c) \
		$(gnomenu_service_VALASOURCES:.vala=.h) \
		$(NULL)

noinst_PROGRAMS = sanity

gnomenu_service_VALASOURCES = \
		sanity.vala \
		$(NULL)

tests.vala.stamp: $(gnomenu_service_VALASOURCES)
	$(VALAC) -C $(TESTS_VALA_PKGS) --vapidir=../libgnomenu --pkg gnomenu $^
	touch $@

sanity_LDADD = $(LDADDS)
sanity_SOURCES = sanity.c sanity.h
EXTRA_DIST = tests.vala.stamp	
