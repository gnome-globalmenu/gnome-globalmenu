NULL=
SUBDIRS = . tests

VALASOURCES = \
	helper.vala \
	parser.vala \
	serializer.vala \
	interface-shell.vala \
	interface-item.vala \
	menu.vala \
	menubar.vala \
	menuitem.vala \
	window.vala \
	menubarbox.vala \
	menushellutils.vala \
	label.vala \
	keygrab.vala \
	globalmenu.vala \
	gnomenu.vala \
	application.vala \
	monitor.vala \
	$(NULL)

lib_LTLIBRARIES = libgnomenu.la

libgnomenu_la_SOURCES = \
						$(VALASOURCES:.vala=.c) \
						$(VALASOURCES:.vala=.h) \
						keygrab-ccode.c \
						task-utils-c.c \
						menushellutils-ccode.c 

libgnomenu_la_CPPFLAGS = \
			 -I$(top_srcdir) \
			 $(GMENU_CFLAGS) \
			 $(GTK_CFLAGS) \
			 $(WNCK_CFLAGS) \
			 $(X11_CFLAGS) \
			 -include $(CONFIG_HEADER) \
			 -include ../libcompat/libcompat.h

libgnomenu_la_LIBADD = $(GTK_LIBS) \
					   $(WNCK_LIBS) \
					   $(X11_LIBS) \
					   $(GMENU_LIBS) \
					   ../libcompat/libcompat.la

libgnomenu_la_LDFLAGS = -version-info 2:0:0 -release $(VERSION) -no-undefined -Wl,--as-needed

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgnomenu.pc

libgnomenu_includedir = $(includedir)/libgnomenu
libgnomenu_include_HEADERS = $(VALASOURCES:.vala=.h)
if WITH_GIR
typelibdir = $(TYPELIBDIR)
typelib_DATA = Gnomenu-1.0.typelib
girdir = $(GIRDIR)
gir_DATA = Gnomenu-1.0.gir
SUFFIXES = .gir .typelib
.gir.typelib: libgnomenu.la
	LD_LIBRARY_PATH=$${LD_LIBRARY_PATH:+$$LD_LIBRARY_PATH:}. $(GIRCOMPILER) $^ -o $@
endif 

#VALAC=/home/rainwoodman/Projects/vala/compiler/valac
gnomenu.vapi: $(VALASOURCES) patch.sh bindings.vapi
	$(VALAC) $(VALA_DEFAULT_PKGS) -C --basedir=$(top_srcdir) --pkg gtk+-2.0 --pkg libwnck-1.0 --pkg xlib --library $(@:.vapi=) $(VALASOURCES) bindings.vapi --pkg libgnome-menu --pkg gnome-desktop-2.0
	bash patch.sh

BUILT_SOURCES = gnomenu.vapi

EXTRA_DIST = $(VALASOURCES) gnomenu.vapi gnomenu.deps patch.sh bindings.vapi Gnomenu-1.0.gir patch-gir.sh libgnomenu.pc.in libgnomenu-uninstalled.pc.in 

