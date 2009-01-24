NULL=
SUBDIRS = . tests

VALASOURCES = \
	helper.vala \
	parser.vala \
	serializer.vala \
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
	$(NULL)

lib_LTLIBRARIES = libgnomenu.la

libgnomenu_la_SOURCES = \
						$(VALASOURCES:.vala=.c) \
						$(VALASOURCES:.vala=.h) \
						keygrab-ccode.c \
						gmarkup-backport.h \
						gmarkup-backport.c \
						menushellutils-ccode.c 

libgnomenu_la_CPPFLAGS = -I$(top_srcdir) $(GTK_CFLAGS) $(GLIB_CFLAGS) -include $(CONFIG_HEADER)
libgnomenu_la_LIBADD = $(GTK_LIBS) $(GLIB_LIBS)
libgnomenu_la_LDFLAGS = -version-info 1:0:0 -release 0.7.0

libgnomenu_includedir = $(includedir)/libgnomenu
libgnomenu_include_HEADERS = $(VALASOURCES:.vala=.h)
if WITH_GIR
typelibdir = $(TYPELIBDIR)
typelib_DATA = Gnomenu.typelib
girdir = $(GIRDIR)
gir_DATA = Gnomenu.gir
SUFFIXES = .gir .typelib
.gir.typelib: libgnomenu.la
	$(GIRCOMPILER) -l libgnomenu.la $^ > $@
endif 

#VALAC=/home/rainwoodman/Projects/vala/compiler/valac

gnomenu.vapi: $(VALASOURCES) patch.sh bindings.vapi
	$(VALAC) -C --basedir=$(top_srcdir) --pkg gtk+-2.0 --library $(@:.vapi=) $(VALASOURCES) bindings.vapi
	bash patch.sh

gnomenu.deps:
	echo gtk+-2.0 > $@

BUILT_SOURCES = gnomenu.vapi  gnomenu.deps

EXTRA_DIST = $(VALASOURCES) gnomenu.vapi gnomenu.deps patch.sh bindings.vapi Gnomenu.gir patch-gir.sh

