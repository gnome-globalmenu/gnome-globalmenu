NULL =
SUBDIRS = vapi-compat libgmarkupdoc libgnomenu server navigator gtk-modules applet po

AM_CPPFLAGS = \
		-include $(CONFIG_HEADER) \
		$(NULL)

EXTRA_DIST = \
	intltool-extract.in \
	intltool-update.in \
	intltool-merge.in \
	$(NULL)

DISTCLEANFILES = \
	intltool-extract \
	intltool-update \
	intltool-merge \
	po/.intltool-merge-cache \
	$(NULL)

schemadir   = @GCONF_SCHEMA_FILE_DIR@
schema_DATA = applet/gnome-globalmenu-applet.schemas

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) gconftool-2 --makefile-install-rule $(srcdir)/$(schema_DATA)
	
any:
	echo $(VALAC) $(GNOMENU_SERVICE_VALA_PKGS)
