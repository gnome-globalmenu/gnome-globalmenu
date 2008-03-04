#

SUBDIRS = libgnomenu servers test gtk+-aqd po

if ENABLE_GTK_DOC
SUBDIRS += doc
endif
gnome_globalmenu_docdir = ${docdir}
gnome_globalmenu_doc_DATA = \
    README\
    COPYING\
    AUTHORS\
    ChangeLog\
    INSTALL\
    NEWS

dist-hook:
	for specfile in *.spec.in; do \
        if test -f $$specfile; then \
            cp -p $$specfile $(distdir); \
        fi \
    done

