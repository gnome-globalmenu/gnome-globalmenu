NULL =
SUBDIRS = tests libgnomenu applet gtk-modules po

AM_CPPFLAGS = \
		$(NULL)

EXTRA_DIST = \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
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


rpm: 
	touch `find . -name '*.vala'`
	make dist
	vim *.spec
	cp *.tar.gz ~/rpmbuild/SOURCES
	rpmbuild -bb *.spec

rpm-fast:
	rpmbuild -bb *.spec
