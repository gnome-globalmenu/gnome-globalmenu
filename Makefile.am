#
DISTCHECK_CONFIGURE_FLAGS == --enable-gtk-doc
SUBDIRS = libgnomenu libgtkhotkey test gtk+-aqd po xfce-plugin gnome-applet

SUBDIRS += doc

gnome_globalmenu_docdir = ${docdir}
gnome_globalmenu_doc_DATA = \
    README\
    COPYING\
    AUTHORS\
    ChangeLog\
    INSTALL\
    NEWS
EXTRA_DIST = intltool-extract.in intltool-merge.in intltool-update.in
DISTCLEANFILES = intltool-extract intltool-merge intltool-update po/.intltool-merge-cache

dist-hook:
	for specfile in *.spec.in; do \
        if test -f $$specfile; then \
            cp -p $$specfile $(distdir); \
        fi \
    done
