NULL = 

AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(srcdir)/.. \
	-I$(builddir)/.. \
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
	$(VALAC) -C $(LIBGNOMENU_VALA_PKGS) --vapidir=$(srcdir)/.. --vapidir=$(builddir)/.. --pkg gnomenu --pkg valacompat $^
	touch $@

gnomenu_navigator_LDADD = \
		$(LIBGNOMENU_LIBS) \
		$(builddir)/../libgnomenu.la \
		$(NULL)

EXTRA_DIST = .vala.stamp	
