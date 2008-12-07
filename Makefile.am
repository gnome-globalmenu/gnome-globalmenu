NULL =
SUBDIRS = tests libgnomenu applet gtk-modules

AM_CPPFLAGS = \
		$(NULL)

EXTRA_DIST = \
	patch.sh \
	$(NULL)

DISTCLEANFILES = \
	$(NULL)
rpm: 
	touch `find . -name '*.vala'`
	make dist
	vim *.spec
	cp *.tar.gz ~/rpmbuild/SOURCES
	rpmbuild -bb *.spec

