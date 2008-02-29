## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

libgnome_globalmenudocdir = ${datadir}/doc/libgnome-globalmenu
libgnome_globalmenudoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

intltool_in_files = intltool-extract.in\
	intltool-merge.in\
	intltool-update.in	

intltool_files = intltool-extract intltool-merge intltool-update

EXTRA_DIST = $(libgnome_globalmenudoc_DATA) $(intltool_in_files) $(script_in_files)
DISTCLEANFILES = $(intltool_files) $(scripts_files)

#
# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec.in; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

