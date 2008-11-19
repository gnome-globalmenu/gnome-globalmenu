SUBDIRS = . tests

AM_CPPFLAGS = \
	-g \
	-include $(CONFIG_HEADER) \
	-DG_LOG_DOMAIN=\"GMarkup\" \
	$(NULL)

AM_VALAC_FLAGS =  \
	--pkg gtkcompat \
	$(NULL)


libgmarkupdocdir = $(libdir)
libgmarkupdoc_LTLIBRARIES = \
		libgmarkupdoc.la \
		$(NULL)
libgmarkupdoc_la_SOURCES = \
		$(VALASOURCES:.vala=.c) \
		$(VALASOURCES:.vala=.h) \
		$(NULL)

libgmarkupdoc_la_CFLAGS = \
	$(GMARKUPDOC_CFLAGS) \
	$(NULL)

VALASOURCES = \
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
		$(NULL)

NEED_PATCH = dom-comment.vala \
			 dom-text.vala \
			 dom-document.vala \
			 dom-attr.vala \
			 dom-element.vala \
			 dom-documentfragment.vala \
			 dom-extended.vala \
			 $(NULL)

BUILT_SOURCES = \
	gmarkupdoc.vapi \
	gmarkupdoc.deps \
	$(VALASOURCES:.vala=.c) \
	$(VALASOURCES:.vala=.h) \
	$(NULL)

gmarkupdoc.vapi : $(VALASOURCES) 
	$(VALAC) -C $(GMARKUPDOC_VALA_PKGS:%=--pkg=%) --library gmarkupdoc $^
	sed -e 's;GtkCompat.Container;Gtk.Container;g' gmarkupdoc.vapi > gmarkupdoc.vapi.new
	mv gmarkupdoc.vapi.new gmarkupdoc.vapi
	for i in $(NEED_PATCH:.vala=.c); do sed -e '/g_type_create_instance/d' $$i > $$i.new; mv $$i.new $$i; done;
	sed dom-node.h -e '/glong\ ref_count/d' > dom-node.h.new; mv dom-node.h.new dom-node.h
gmarkupdoc.deps :
	for i in $(GMARKUPDOC_VALA_PKGS); do echo $$i; done > $@;

_HEADERS_ = $(VALASOURCES:%.vala=%.h) \
	$(NULL)

libgmarkupdoc_la_LIBADD = \
		$(GMARKUPDOC_LIBS) \
		$(NULL)
libgmarkupdoc_la_LDFLAGS = \
		-no-undefined \
		$(NULL)

EXTRA_DIST =  \
	$(BUILT_SOURCES) \
	$(VALASOURCES) \
	$(NULL)

vapidir = $(VALA_VAPI_DIR)
dist_vapi_DATA = \
	gmarkupdoc.deps \
	gmarkupdoc.vapi \
	$(NULL)
headerdir = $(includedir)/libgmarkupdoc
dist_header_DATA = \
	$(_HEADERS_) \
	$(NULL)

