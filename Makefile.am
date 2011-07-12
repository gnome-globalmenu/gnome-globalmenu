NULL =
SUBDIRS = \
	src \
	po \
	$(NULL)

#	globalmenu-plugin 

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

AM_CPPFLAGS = \
		$(NULL)

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

doc_DATA = README COPYING ChangeLog AUTHORS INSTALL \
	README.XFCE README.GNOME README.LEGACY

