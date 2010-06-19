
NULL=

VAPI_FILE = globalmenu-settings.vapi
DEPS_FILE = globalmenu-settings.deps
HEADER_FILE = globalmenu-settings.h

VALASOURCES = \
	settings.vala \
	global-settings.vala \
	local-settings.vala \
	$(NULL)

noinst_LTLIBRARIES = libsettings.la
libsettings_la_SOURCES = \
	$(VALASOURCES:.vala=.c) \
	$(HEADER_FILE) \
	$(NULL)

libsettings_la_CPPFLAGS = \
			 -I$(top_srcdir) \
			 $(GTK_CFLAGS) \
			 $(X11_CFLAGS) \
			 -DG_LOG_DOMAIN=\"GlobalMenu:Settings\" \
			 -include $(CONFIG_HEADER) \
			 $(NULL)

libsettings_la_LDFLAGS = -version-info 0:0:0 -Wl,--as-needed

vala-ccode: $(VALASOURCES)
	$(VALAC) $(VALA_DEFAULT_PKGS) \
		-C \
		-H $(HEADER_FILE) \
		--library=globalmenu-settings \
		--vapi=$(VAPI_FILE) \
		$(VALASOURCES) \
		--pkg gtk+-2.0 \
		--pkg gdk-x11-2.0
	touch $@

BUILT_SOURCES = vala-ccode

EXTRA_DIST = \
	$(VALASOURCES) \
	globalmenu-settings.vapi \
	globalmenu-settings.deps \
	vala-ccode \
	$(NULL)

