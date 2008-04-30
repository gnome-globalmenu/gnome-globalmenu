
AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = libgnomenu.vala.stamp

libgnomenudir = $(libdir)
libgnomenu_LTLIBRARIES = \
		libgnomenu.la

libgnomenu_la_VALASOURCES = \
		main.vala \
		menu.vala \
		application.vala \
		agent.vala \
		document.vala \
		menuitem.vala \
		$(NULL)

libgnomenu_la_SOURCES = \
		$(libgnomenu_la_VALASOURCES:.vala=.c) \
		$(libgnomenu_la_VALASOURCES:.vala=.h) \
		$(NULL)

libgnomenu.vala.stamp: $(libgnomenu_la_VALASOURCES)
	$(VALAC) -C $(LIBGNOMENU_VALA_PKGS) --library gnomenu $^
	touch $@

libgnomenu_la_LIBADD = \
		$(LIBGNOMENU_LIBS) \
		$(NULL)

EXTRA_DIST = libgnomenu.vala.stamp	
