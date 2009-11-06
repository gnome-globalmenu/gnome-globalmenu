NULL=
SUBDIRS = . tests

VALASOURCES = \
	globalmenubar.vala \
	globalmenuitem.vala \
	globalmenuadapter.vala \
	adapter.vala \
	gnomenu.vala \
	monitor.vala \
	window.vala \
	parser.vala \
	serializer.vala \
	interfaces/shell.vala \
	interfaces/item.vala \
	widgets/menu.vala \
	widgets/menubar.vala \
	widgets/menuitem.vala \
	widgets/menubarbox.vala \
	widgets/menulabel.vala \
	utils/menushellutils.vala \
	utils/keygrab.vala \
	utils/mnemonickeys.vala \
	$(NULL)

noinst_LTLIBRARIES = libgnomenu.la

libgnomenu_la_SOURCES = \
						$(VALASOURCES:.vala=.c) \
						libgnomenu.h \
						ccode/keygrab-ccode.c \
						ccode/menushellutils-ccode.c 

libgnomenu_la_CPPFLAGS = \
			 -I$(top_srcdir) \
			 $(GTK_CFLAGS) \
			 $(WNCK_CFLAGS) \
			 $(X11_CFLAGS) \
			 -DG_LOG_DOMAIN=\"libgnomenu\" \
			 -include $(CONFIG_HEADER) \
			 -include ../legacy-support/legacy-support.h

libgnomenu_la_LIBADD = ../legacy-support/liblegacy-support.la

libgnomenu_la_LDFLAGS = -version-info 2:0:0 -Wl,--as-needed

gnomenu.vapi: $(VALASOURCES) bindings.vapi
	$(VALAC) $(VALA_DEFAULT_PKGS) -C --basedir=$(top_srcdir) -H libgnomenu.h \
		--library $(@:.vapi=) $(VALASOURCES) \
		bindings.vapi \
		--pkg gtk+-2.0 \
		--pkg libwnck-1.0 \
		--pkg gnome-desktop-2.0 \
		--pkg gdk-x11-2.0

BUILT_SOURCES = gnomenu.vapi

EXTRA_DIST = $(VALASOURCES) gnomenu.vapi gnomenu.deps bindings.vapi

