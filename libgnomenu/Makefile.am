NULL=
SUBDIRS = . tests

VALASOURCES=\
	menushell.vala \
	parser.vala \
	serializer.vala \
	menubar.vala \
	menuitem.vala \
	menu.vala \
	$(NULL)
noinst_LTLIBRARIES = libgnomenu.la

libgnomenu_la_SOURCES = $(VALASOURCES:.vala=.c) \
						$(VALASOURCES:.vala=.h)
libgnomenu_la_CPPFLAGS = $(GTK_CFLAGS) $(GLIB_CFLAGS)
libgnomenu_la_LIBADD = $(GTK_LIBS) $(GLIB_LIBS)
libgnomenu_la_LDFLAGS = -version-info 1:0:0 -release 0.7.0

STAMP=gnomenu.vapi

$(STAMP): $(VALASOURCES)
	$(VALAC) -C --basedir=$(top_srcdir) --vapidir=. --pkg markupparser --pkg gtk+-2.0 --library $(@:.vapi=) $(VALASOURCES)
	bash patch.sh

BUILT_SOURCES = $(STAMP)

EXTRA_DIST = $(STAMP) $(VALASOURCES) patch.sh

