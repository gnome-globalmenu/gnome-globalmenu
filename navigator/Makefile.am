NULL = 

AM_CPPFLAGS = \
	-g \
	$(GNOMENU_NAVIGATOR_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/libgmarkupdoc \
	-I$(top_srcdir)/libgnomenu \
	$(NULL)

AM_VALAC_FLAGS = \
	$(GNOMENU_NAVIGATOR_VALA_PKGS) \
	--vapidir=$(top_srcdir)/libgmarkupdoc  \
	--vapidir=$(top_srcdir)/libgnomenu  \
	--pkg gnomenu  \
	--pkg gmarkupdoc  \
	$(NULL)
	
BUILT_SOURCES = \
	.vala.stamp \
	$(gnomenu_navigator_VALASOURCES:.vala=.c) \
	$(gnomenu_navigator_VALASOURCES:.vala=.h) \
	$(NULL)

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
	$(VALAC) -C   $^
	touch $@

gnomenu_navigator_LDADD = \
		$(GNOMENU_NAVIGATOR_LIBS) \
		$(top_builddir)/libgnomenu/libgnomenu.la \
		$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
		$(NULL)

EXTRA_DIST = $(BUILT_SOURCES) $(gnomenu_navigator_VALASOURCES)
