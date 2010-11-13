NULL=
VAPI_FILE = globalmenu-server.vapi
DEPS_FILE = globalmenu-server.deps
HEADER_FILE = globalmenu-server.h

VALASOURCES = \
	globalmenubar.vala \
	globalmenuitem.vala \
	globalmenuadapter.vala \
	adapter.vala \
	enums.vala \
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


lib_LTLIBRARIES = libglobalmenu-server.la

libglobalmenu_server_includedir = $(includedir)/gnome-globalmenu
libglobalmenu_server_include_HEADERS = globalmenu-server.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = globalmenu-server.pc

libglobalmenu_server_la_SOURCES = \
						$(VALASOURCES:.vala=.c) \
						$(HEADER_FILE) \
						ccode/keygrab-ccode.c \
						ccode/menushellutils-ccode.c 

libglobalmenu_server_la_CPPFLAGS = \
			 -I$(top_srcdir) \
			 $(GTK_CFLAGS) \
			 $(WNCK_CFLAGS) \
			 $(X11_CFLAGS) \
			 -DG_LOG_DOMAIN=\"GlobalMenu:Server\" \
			 -include $(CONFIG_HEADER) \
			 -include ../legacy-support/legacy-support.h

libglobalmenu_server_la_LIBADD = ../legacy-support/liblegacy-support.la

libglobalmenu_server_la_LDFLAGS = -version-info 3:0:0 -Wl,--as-needed

vala-ccode: $(VALASOURCES) bindings.vapi
	$(VALAC) $(VALA_DEFAULT_PKGS) \
		-C \
		--enable-deprecated \
		-H $(HEADER_FILE) \
		--library=globalmenu-server \
		--vapi=$(VAPI_FILE) \
		$(VALASOURCES) \
		bindings.vapi \
		--pkg gtk+-2.0 \
		--pkg libwnck-1.0 \
		--pkg gnome-desktop-2.0 \
		--pkg gdk-x11-2.0
	touch $@

BUILT_SOURCES = vala-ccode

EXTRA_DIST = \
	$(VALASOURCES) \
	globalmenu-server.vapi \
	globalmenu-server.deps \
	bindings.vapi \
	vala-ccode \
	globalmenu-server.pc.in

