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
	monitor.vala \
	$(NULL)

lib_LTLIBRARIES = libgnomenu.la

libgnomenu_la_SOURCES = \
						$(VALASOURCES:.vala=.c) \
						$(VALASOURCES:.vala=.h) \
						keygrab-ccode.c \
						gmarkup-backport.h \
						gmarkup-backport.c \
						menushellutils-ccode.c 

libgnomenu_la_CPPFLAGS = -I$(top_srcdir) $(WNCK_CFLAGS) -include $(CONFIG_HEADER) -DWNCK_I_KNOW_THIS_IS_UNSTABLE
libgnomenu_la_LIBADD = $(WNCK_LIBS)
libgnomenu_la_LDFLAGS = -version-info 2:0:0 -release $(VERSION)

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
gnomenu.vapi: $(VALASOURCES) patch.sh bindings.vapi xlib.vapi
	bash patch.sh backup
	$(VALAC) -C --basedir=$(top_srcdir) --pkg gtk+-2.0 --pkg libwnck-1.0 --library $(@:.vapi=) $(VALASOURCES) bindings.vapi xlib.vapi
	bash patch.sh

gnomenu.deps:
	echo gtk+-2.0 > $@

BUILT_SOURCES = gnomenu.vapi  gnomenu.deps

EXTRA_DIST = $(VALASOURCES) gnomenu.vapi gnomenu.deps patch.sh bindings.vapi Gnomenu-1.0.gir patch-gir.sh libgnomenu.pc.in libgnomenu-uninstalled.pc.in xlib.vapi

