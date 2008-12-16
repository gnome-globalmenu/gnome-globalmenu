NULL =
SUBDIRS = tests libgnomenu applet gtk-modules po

AM_CPPFLAGS = \
		$(NULL)

EXTRA_DIST = \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	gnome-globalmenu.schemas \
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
include $(top_srcdir)/Makefile.config
include $(top_srcdir)/Makefile.licenses

schemadir   = @GCONF_SCHEMA_FILE_DIR@
schema_DATA = gnome-globalmenu.schemas

if GCONF_SCHEMAS_INSTALL

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) gconftool-2 --makefile-install-rule $(schema_DATA)
else
install-data-local:
endif



rpm: 
	touch `find . -name '*.vala'`
	make dist
	vim *.spec
	cp *.tar.gz ~/rpmbuild/SOURCES
	rpmbuild -bb *.spec

rpm-fast:
	rpmbuild -bb *.spec
