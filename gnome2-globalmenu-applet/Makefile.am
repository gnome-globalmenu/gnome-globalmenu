## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

gnome2_globalmenu_appletdocdir = ${datadir}/doc/gnome2-globalmenu-applet
gnome2_globalmenu_appletdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS\
	GNOME_GlobalMenuApplet.server.sample

intltool_in_files = intltool-extract.in\
	intltool-merge.in\
	intltool-update.in	

intltool_files = intltool-extract intltool-merge intltool-update

EXTRA_DIST = $(gnome2_globalmenu_appletdoc_DATA) $(intltool_in_files)
DISTCLEANFILES = $(intltool_files)
#
# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

