
BUILT_SOURCES = vala-ccode

VALASOURCES = \
	module-main.vala

libgnomenu_panel_LTLIBRARIES = libgnomenu-panel.la
libgnomenu_paneldir = $(GTK_MODULES_DIR)
libgnomenu_panel_la_SOURCES = $(VALASOURCES:.vala=.c)

vala-ccode: $(VALASOURCES)
	$(VALAC) $(VALA_DEFAULT_PKGS) --thread \
	   -C --basedir=$(top_srcdir) \
	   --pkg gmodule-2.0 \
	   --pkg gtk+-2.0 \
	   --pkg gdk-x11-2.0 \
	   --pkg globalmenu-server \
	   --vapidir $(top_srcdir)/libserver\
	   $(VALASOURCES)
	touch $@

libgnomenu_panel_la_LIBADD = \
		$(GLIB_LIBS) \
		$(GTK_LIBS) \
		../../libserver/libserver.la \
		$(WNCK_LIBS)

libgnomenu_panel_la_LDFLAGS = \
		-g \
		-version-info 1:0:0 \
		-avoid-version \
		-no-undefined \
		-export-dynamic \
		-Wl,--as-needed \
		-module


libgnomenu_panel_la_CPPFLAGS = \
	  -I$(top_srcdir) $(GLIB_CFLAGS) $(GTK_CFLAGS) $(WNCK_CFLAGS) \
	  -I$(top_srcdir)/libserver \
	  -include $(CONFIG_HEADER) \
	  -DG_LOG_DOMAIN=\"GnomenuPanel\" \
	  -g

EXTRA_DIST = vala-ccode $(VALASOURCES)

