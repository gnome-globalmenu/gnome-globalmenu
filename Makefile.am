NULL =
SUBDIRS = tests libgnomenu applet xfce gtk-modules po
DISTCHECK_CONFIGURE_FLAGS = --disable-schemas-install --disable-static

AM_CPPFLAGS = \
		$(NULL)

EXTRA_DIST = \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	gnome-globalmenu.schemas.in \
	gnome-globalmenu.spec \
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
schema_in_files = gnome-globalmenu.schemas.in
schema_DATA = $(schema_in_files:.in=)
@INTLTOOL_SCHEMAS_RULE@

if GCONF_SCHEMAS_INSTALL

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) gconftool-2 --makefile-install-rule $(schema_DATA)
else
install-data-local:
endif



rpm: 
	vim gnome-globalmenu.spec
	touch `find . -name '*.vala'`
	make dist
	rpmbuild -tb $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz
rpm-fast:
	make dist
	rpmbuild -tb $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz
