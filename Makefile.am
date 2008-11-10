NULL =
SUBDIRS = vapi-compat libgmarkupdoc libgnomenu server navigator gtk-modules applet tests po

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

any:
	echo $(VALAC) $(GNOMENU_SERVICE_VALA_PKGS)
