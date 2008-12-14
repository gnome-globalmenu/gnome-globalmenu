NULL =
SUBDIRS = tests libgnomenu applet gtk-modules po

AM_CPPFLAGS = \
		$(NULL)

EXTRA_DIST = \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	patch.sh \
	$(NULL)

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update \
	$(NULL)

BUILT_SOURCES =

CLEANFILES = 
include $(top_srcdir)/Makefile.configmake

schemadir   = @GCONF_SCHEMA_FILE_DIR@
schema_DATA = applet/gnome-globalmenu-applet.schemas

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) gconftool-2 --makefile-install-rule $(srcdir)/$(schema_DATA)

rpm: 
	touch `find . -name '*.vala'`
	make dist
	vim *.spec
	cp *.tar.gz ~/rpmbuild/SOURCES
	rpmbuild -bb *.spec

