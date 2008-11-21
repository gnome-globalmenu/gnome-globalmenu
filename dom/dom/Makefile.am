include $(top_srcdir)/Makefile.decl

NULL =

AM_CPPFLAGS = \
	-I$(top_srcdir)/dom \
	-I$(top_srcdir)/gee \
	$(GLIB_CFLAGS) \
	$(NULL)

BUILT_SOURCES = dom.vala.stamp

lib_LTLIBRARIES = \
	libdom.la
	$(NULL)

libdom_la_VALASOURCES = \
		dom-error.vala \
		dom-node.vala \
		dom-attr.vala \
		dom-characterdata.vala \
		dom-text.vala \
		dom-extended.vala \
		dom-comment.vala \
		dom-element.vala \
		dom-document.vala \
		dom-documentfragment.vala \
		dom-visitor.vala \
		dom-serializer.vala \
	$(NULL)

NEED_PATCH = dom-comment.vala \
			 dom-text.vala \
			 dom-document.vala \
			 dom-attr.vala \
			 dom-element.vala \
			 dom-documentfragment.vala \
			 dom-extended.vala \
			 $(NULL)

libdom_la_SOURCES = \
	dom.vala.stamp \
	$(libdom_la_VALASOURCES:.vala=.c) \
	$(libdom_la_VALASOURCES:.vala=.h) \
	$(NULL)

domincludedir = $(includedir)/dom-1.0/dom

dominclude_HEADERS = \
	$(libdom_la_VALASOURCES:.vala=.h) \
	$(NULL)

dom-1.0.vapi dom.vala.stamp: $(libdom_la_VALASOURCES)
	$(VALAC) -C --vapidir $(top_srcdir)/gee/gee --pkg dom-gee-1.0 --basedir $(top_srcdir)/dom --library dom-1.0 $^
	sed -e 's;GtkCompat.Container;Gtk.Container;g' dom-1.0.vapi > dom-1.0.vapi.new
	mv dom-1.0.vapi.new dom-1.0.vapi
	for i in $(NEED_PATCH:.vala=.c); do sed -e '/g_type_create_instance/d' $$i > $$i.new; mv $$i.new $$i; done;
	sed dom-node.h -e '/glong\ ref_count/d' > dom-node.h.new; mv dom-node.h.new dom-node.h
	touch $@

libdom_la_LIBADD = \
	$(GLIB_LIBS) \
	$(top_builddir)/gee/gee/libdom-gee.la \
	$(NULL)

vapidir = $(datadir)/vala/vapi

dist_vapi_DATA = \
	dom-1.0.vapi \
	$(NULL)

EXTRA_DIST += $(libdom_la_VALASOURCES) dom-1.0.vapi dom.vala.stamp
