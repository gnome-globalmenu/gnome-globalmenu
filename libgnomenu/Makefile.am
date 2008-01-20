#

INCLUDES = $(LIBGNOMENU_CFLAGS)

lib_LTLIBRARIES = libgnomenu.la

BUILT_SOURCES = gdksocket-marshall.h gdksocket-marshall.c

libgnomenu_la_SOURCES = \
	$(BUILT_SOURCES) \
	gdksocket.h \
	gdksocket.c \
	gnomenuserver.h \
	gnomenuserver.c

libgnomenu_la_LDFLAGS = \
	-version-info=$(LIBGNOMENU_LT_VERSION)
libgnomenu_la_LIBADD = $(LIBGNOMENU_LIBS)

$(libgnomenu_la_OBJECTS) : $(BUILT_SOURCES)


libgnomenuincludedir = $(includedir)/libgnomenu
libgnomenuinclude_HEADERS = gdksocket.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgnomenu.pc

gdksocket-marshall.c: gdksocket-marshall.list
	glib-genmarshal --internal --body --prefix=gdk_socket_marshall gdksocket-marshall.list > $@
gdksocket-marshall.h: gdksocket-marshall.list
	glib-genmarshal --internal --header --prefix=gdk_socket_marshall gdksocket-marshall.list > $@


CLEANFILES = $(BUILT_SOURCES)
dist-hook:
	cd $(distdir) ; rm -f $(CLEANFILES)
