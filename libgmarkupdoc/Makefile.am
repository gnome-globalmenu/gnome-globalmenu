SUBDIRS = . 
AM_CPPFLAGS = \
	-g \
	$(LIBGNOMENU_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

AM_VALAC_FLAGS =  \
	$(NULL)

BUILT_SOURCES = libgmarkupdoc.vala.stamp

libgmarkupdocdir = $(libdir)
libgmarkupdoc_LTLIBRARIES = \
		libgmarkupdoc.la

libgmarkupdoc_la_VALASOURCES = \
		xmlparser.vala \
		xml.vala \
		$(NULL)


libgmarkupdoc_la_SOURCES = \
		$(libgmarkupdoc_la_VALASOURCES:.vala=.c) \
		$(libgmarkupdoc_la_VALASOURCES:.vala=.h) \
		$(NULL)


gnomenu.vapi libgmarkupdoc.vala.stamp: $(libgmarkupdoc_la_VALASOURCES)
	$(VALAC) -C $(LIBGNOMENU_VALA_PKGS) --library gmarkupdoc  $^
	touch $@

libgmarkupdoc_la_LIBADD = \
		$(LIBGNOMENU_LIBS) \
		$(NULL)

EXTRA_DIST = libgmarkupdoc.vala.stamp $(libgmarkupdoc_la_VALASOURCES)
