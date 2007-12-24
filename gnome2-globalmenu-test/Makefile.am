## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

gnome2_globalmenu_testdocdir = ${prefix}/doc/gnome2-globalmenu-test
gnome2_globalmenu_testdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(gnome2_globalmenu_testdoc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

