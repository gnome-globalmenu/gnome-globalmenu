#

SUBDIRS = libgnomenu servers test gtk+-aqd po
if MAINTAINER_MODE
NO_INSTALL=1
endif

#if ENABLE_GTK_DOC
SUBDIRS += doc
#endif
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
install-exec-hook: check-maintainer
install-data-hook: check-maintainer
check-maintainer:
	if [ x$(NO_INSTALL) == x1 ]; then \
		echo =======================================; \
		echo You cannot install a maintainer build. ;\
		echo Because in maintainer mode apps in servers directory ;\
		echo will try to seek the .glade file in "'.'" ; \
		echo instead of $(datadir) . ; \
		echo The files were installed , but they will crash ;\
		echo ; \
		echo autogen.sh will automatically add --enable-maintainer-mode;  \
		echo Inovke configure again without --enable-maintainer-mode; \
		echo and make install again; \
		echo =======================================; \
		[ 1 = 0 ]; \
	fi;
