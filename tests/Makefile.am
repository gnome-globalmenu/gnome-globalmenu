AM_CPPFLAGS = \
	$(TESTS_CFLAGS) \
	-g \
	-include $(CONFIG_HEADER) \
	-I../libgnomenu  \
	-I../libgnomenu-gtk  \
	$(NULL)

LDADDS = $(top_builddir)/libgnomenu/libgnomenu.la $(top_builddir)/libgnomenu-gtk/libgnomenu-gtk.la $(TESTS_LIBS)

BUILT_SOURCES = tests.vala.stamp \
		$(gnomenu_service_VALASOURCES:.vala=.c) \
		$(gnomenu_service_VALASOURCES:.vala=.h) \
		$(NULL)

noinst_PROGRAMS = app manager test keybinding

gnomenu_service_VALASOURCES = \
		test.vala \
		keybinding.vala \
		app.vala  \
		manager.vala \
		$(NULL)

tests.vala.stamp: $(gnomenu_service_VALASOURCES)
	$(VALAC) -C $(TESTS_VALA_PKGS) --vapidir=../libgnomenu --vapidir=../libgnomenu-gtk --pkg gnomenu --pkg gnomenu-gtk $^
	touch $@

test_LDADD = $(LDADDS)
test_SOURCES = test.c test.h
app_LDADD = $(LDADDS)
app_SOURCES = app.c app.h
manager_LDADD = $(LDADDS)
manager_SOURCES = manager.c manager.h
keybinding_LDADD = $(LDADDS)
keybinding_SOURCES = keybinding.c keybinding.h
EXTRA_DIST = tests.vala.stamp	
