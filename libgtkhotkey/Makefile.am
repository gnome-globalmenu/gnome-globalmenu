## Process this file with automake to produce Makefile.in

# Make targets for unit tests needs to be declared here too
#include $(top_srcdir)/Makefile.decl

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GTK_HOTKEY_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

#
# Headers to be installed
#
devel_headers_HEADERS = \
	gtkhotkey.h						\
	gtk-hotkey-info.h				\
	gtk-hotkey-key-file-registry.h	\
	gtk-hotkey-listener.h			\
	gtk-hotkey-registry.h			\
	gtk-hotkey-x11-listener.h		\
	gtk-hotkey-error.h
       

#
# Path to install headers in
#
devel_headersdir = $(includedir)/gtkhotkey-1.0

gtkhotkey_libs = $(top_builddir)/src/libgtkhotkey.la

lib_LTLIBRARIES = \
	libgtkhotkey.la

libgtkhotkey_la_LDFLAGS = \
	-version-info 0:0:0

libgtkhotkey_la_LIBADD = \
	$(GTK_HOTKEY_LIBS)

libgtkhotkey_la_SOURCES = \
	gtk-hotkey-info.c\
	gtk-hotkey-info.h \
	gtk-hotkey-key-file-registry.c \
	gtk-hotkey-key-file-registry.h \
	gtk-hotkey-listener.c \
	gtk-hotkey-listener.h \
	gtk-hotkey-registry.c \
	gtk-hotkey-registry.h \
	gtk-hotkey-x11-listener.c \
	gtk-hotkey-x11-listener.h \
	gtk-hotkey-private.h \
	x11/eggaccelerators.c \
	x11/eggaccelerators.h \
	x11/tomboykeybinder.c \
	x11/tomboykeybinder.h \
	gtk-hotkey-marshal.c \
	gtk-hotkey-marshal.h \
	gtk-hotkey-error.c \
	gtk-hotkey-error.h \
	gtk-hotkey-utils.c \
	gtk-hotkey-utils.h
