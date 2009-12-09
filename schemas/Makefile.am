NULL =

EXTRA_DIST = \
	gnome-globalmenu.schemas.in \
	$(NULL)

DISTCLEANFILES =
	gnome-globalmenu.schemas \
	$(NULL)

BUILT_SOURCES = gnome-globalmenu.schemas

schemadir   = $(GCONF_SCHEMA_FILE_DIR)
schema_in_files = gnome-globalmenu.schemas.in
schema_DATA = $(schema_in_files:.in=)
@INTLTOOL_SCHEMAS_RULE@

if GCONF_SCHEMAS_INSTALL

install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(schema_DATA)
else
install-data-local:
endif
