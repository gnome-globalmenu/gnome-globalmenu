NULL=
SUBDIRS = . tests

INTERNAL = \
	helper.vala \
	parser.vala \
	serializer.vala \
	menu.vala \
	menubar.vala \
	menuitem.vala \
	window.vala \
	menubarbox.vala \
	label.vala \
	keygrab.vala \
	$(NULL)

EXTERNAL = gnomenu.vala

noinst_LTLIBRARIES = libgnomenu.la

libgnomenu_la_SOURCES = \
						$(INTERNAL:.vala=.c) \
						$(INTERNAL:.vala=.h) \
						$(EXTERNAL:.vala=.c) \
						$(EXTERNAL:.vala=.h) \
						keygrab-ccode.c

libgnomenu_la_CPPFLAGS = -I$(top_srcdir) $(GTK_CFLAGS) $(GLIB_CFLAGS) -include $(CONFIG_HEADER)
libgnomenu_la_LIBADD = $(GTK_LIBS) $(GLIB_LIBS)
libgnomenu_la_LDFLAGS = -version-info 1:0:0 -release 0.7.0

INTERNAL_VAPI= gnomenu-private.vapi
EXTERNAL_VAPI= gnomenu.vapi

$(INTERNAL_VAPI): $(INTERNAL) patch.sh $(EXTERNAL_VAPI) bindings.vapi
	$(VALAC) -C --basedir=$(top_srcdir) --vapidir=. --pkg gnomenu --pkg gtk+-2.0 --pkg bindings --library $(@:.vapi=) $(INTERNAL)
	echo gnomenu > $(@:.vapi=.deps)
	bash patch.sh
$(EXTERNAL_VAPI): $(EXTERNAL) patch.sh bindings.vapi
	$(VALAC) -C --basedir=$(top_srcdir) --vapidir=. --pkg gtk+-2.0 --pkg bindings --library $(@:.vapi=) $(EXTERNAL)

BUILT_SOURCES = $(INTERNAL_VAPI) $(EXTERNAL_VAPI) $(INTERNAL_VAPI:.vapi=.deps)

EXTRA_DIST = $(INTERNAL_VAPI) $(INTERNAL) $(EXTERNAL_VAPI) $(EXTERNAL) patch.sh $(INTERNAL_VAPI:.vapi=.deps) bindings.vapi

