NULL =
SUBDIRS = \
	src \
        extension \
	po \
	$(NULL)

#	globalmenu-plugin 

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

AM_CPPFLAGS = \
		$(NULL)

gsettingsdir = $(GSCHEMASDIR)
gsettings_DATA= org.globalmenu.gschema.xml

dbusdir = $(SERVICESDIR)
dbus_DATA = org.globalmenu.manager.service

org.globalmenu.manager.service: org.globalmenu.manager.service.in
	sed "s;@BINDIR@;$(bindir);" < $^ > $@
install-data-local:
	$(GLIB_COMPILE_SCHEMAS) $(GSCHEMASDIR)

uninstall-local:
	$(GLIB_COMPILE_SCHEMAS) $(GSCHEMASDIR)

man_MANS = gnome-globalmenu.1

EXTRA_DIST = \
	$(man_MANS) \
	$(doc_DATA) \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	$(NULL)

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update \
	$(NULL)


CLEANFILES = 

BUILT_SOURCES =

doc_DATA = README COPYING ChangeLog AUTHORS INSTALL

