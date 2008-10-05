NULL = 

AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/libgnomenu \
	-I$(top_builddir)/libgnomenu \
	$(NULL)

BUILT_SOURCES = .vala.stamp

gnomenu_navigator_VALASOURCES = \
	navigator.vala \
	$(NULL)

bin_PROGRAMS = \
	gnomenu-navigator \
	$(NULL)


gnomenu_navigator_SOURCES = \
	$(gnomenu_navigator_VALASOURCES:.vala=.c) \
	$(gnomenu_navigator_VALASOURCES:.vala=.h) \
	$(NULL)

.vala.stamp: $(gnomenu_navigator_VALASOURCES)
	$(VALAC) -C $(LIBGNOMENU_VALA_PKGS) --vapidir=$(top_srcdir)/libgnomenu --vapidir=$(top_builddir)/libgnomenu --pkg gnomenu --pkg valacompat $^
	touch $@

gnomenu_navigator_LDADD = \
		$(LIBGNOMENU_LIBS) \
		$(top_builddir)/libgnomenu/libgnomenu.la \
		$(NULL)

EXTRA_DIST = .vala.stamp	$(gnomenu_navigator_VALASOURCES)
