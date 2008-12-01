NULL=
SUBDIRS = . tests

INTERNAL = \
	helper.vala \
	parser.vala \
	serializer.vala \
	menubar.vala \
	menuitem.vala \
	window.vala \
	$(NULL)

EXTERNAL = gnomenu.vala

noinst_LTLIBRARIES = libgnomenu.la

libgnomenu_la_SOURCES = \
						$(INTERNAL:.vala=.c) \
						$(INTERNAL:.vala=.h) \
						$(EXTERNAL:.vala=.c) \
						$(EXTERNAL:.vala=.h)

libgnomenu_la_CPPFLAGS = $(GTK_CFLAGS) $(GLIB_CFLAGS)
libgnomenu_la_LIBADD = $(GTK_LIBS) $(GLIB_LIBS)
libgnomenu_la_LDFLAGS = -version-info 1:0:0 -release 0.7.0

INTERNAL_VAPI= gnomenu-private.vapi
EXTERNAL_VAPI= gnomenu.vapi

$(INTERNAL_VAPI): $(INTERNAL) patch.sh markupparser.vapi gdk.vapi $(EXTERNAL_VAPI)
	$(VALAC) -C --basedir=$(top_srcdir) --vapidir=. --pkg gnomenu --pkg gdk --pkg markupparser --pkg gtk+-2.0 --library $(@:.vapi=) $(INTERNAL)
	echo gnomenu > $(@:.vapi=.deps)
	bash patch.sh
$(EXTERNAL_VAPI): $(EXTERNAL) patch.sh markupparser.vapi gdk.vapi
	$(VALAC) -C --basedir=$(top_srcdir) --vapidir=. --pkg gdk --pkg markupparser --pkg gtk+-2.0 --library $(@:.vapi=) $(EXTERNAL)
	bash patch.sh

BUILT_SOURCES = $(INTERNAL_VAPI) $(EXTERNAL_VAPI) $(INTERNAL_VAPI:.vapi=.deps)

EXTRA_DIST = $(INTERNAL_VAPI) $(INTERNAL) $(EXTERNAL_VAPI) $(EXTERNAL) patch.sh gdk.vapi $(INTERNAL_VAPI:.vapi=.deps)

