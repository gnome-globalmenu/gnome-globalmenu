#

AM_CFLAGS = -g $(LIBGNOMENU_CFLAGS)
AM_CFLAGS += -DSYSCONFDIR=\"$(sysconfdir)\"

moduledir = $(libdir)

module_LTLIBRARIES = libgnomenu.la

bin_PROGRAMS = gnomenu-server gnomenu-call
LDADD = libgnomenu.la

libgnomenu_la_SOURCES = \
	ipcserver.c \
	ipcserver.h \
	ipcdispatcher.c \
	ipcdispatcher.h \
	ipcclient.c \
	ipcclient.h \
	ipccommand.c \
	ipccommand.h \
	ipcevent.c \
	ipcevent.h \
	ipcutils.c \
	ipcutils.h \
	ipc.h  \
	binding-gtk.h \
	binding-gtk.c \
	main.c 

libgnomenu_la_LDFLAGS = \
	-g \
	-version-info=$(LIBGNOMENU_LT_VERSION) \
	-no-undefined \
	-export-dynamics

libgnomenu_la_LIBADD = $(LIBGNOMENU_LIBS)

$(libgnomenu_la_OBJECTS) : $(BUILT_SOURCES)


libgnomenuincludedir = $(includedir)/libgnomenu
libgnomenuinclude_HEADERS =

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgnomenu.pc

EXTRA_DIST = ipccommand.dtd

CLEANFILES = $(BUILT_SOURCES)

dist-hook:
