NULL = 

BUILT_SOURCES = \
	vala-ccode \
	$(NULL)

VALASOURCES = \
	superrider.vala \
	patcher.vala \
	gtk-menubar.vala \
	gtk-widget.vala \
	agent-factory.vala \
	agent.vala \
	module-main.vala \
	serializer.vala \
	locator.vala \
	$(NULL)

libglobalmenu_plugindir = $(GTK_MODULES_DIR)
libglobalmenu_plugin_LTLIBRARIES = \
	libglobalmenu-plugin.la \
	$(NULL)

libglobalmenu_plugin_la_SOURCES= \
	$(VALASOURCES:.vala=.c) \
	globalmenu-plugin.h \
	$(NULL)

vala-ccode : $(VALASOURCES)
	$(VALAC) $(VALA_DEFAULT_PKGS) --thread -C  \
		--library=globalmenu-plugin \
		--enable-deprecated \
		-H globalmenu-plugin.h --vapidir=.  \
		$(VALASOURCES) \
		--vapidir=$(top_srcdir)/libsettings \
		--pkg gmodule-2.0 \
		--pkg gtk+-2.0 \
		--pkg gdk-x11-2.0 \
		--pkg globalmenu-settings
	touch $@

libglobalmenu_plugin_la_LIBADD = \
		$(GLIB_LIBS) \
		$(GTK_LIBS) \
		$(top_builddir)/libsettings/libsettings.la \
		$(NULL)

libglobalmenu_plugin_la_LDFLAGS = \
		-g \
		-version-info 1:0:0 \
		-avoid-version \
		-no-undefined \
		-export-dynamic \
		-module

#		-Wl,--as-needed 
libglobalmenu_plugin_la_CPPFLAGS = \
	  -I$(top_srcdir) \
	  -I$(top_srcdir)/libsettings \
	  $(GLIB_CFLAGS) $(GTK_CFLAGS) \
	  -include $(CONFIG_HEADER) \
	  -DG_LOG_DOMAIN=\"GlobalMenu:Plugin\" \
	  -g

EXTRA_DIST = vala-ccode \
	globalmenu-plugin.h \
	$(VALASOURCES) \
	$(NULL)

