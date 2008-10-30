NULL =
if ENABLE_VALA_DOC
VALASOURCES = \
	$(top_srcdir)/libgmarkupdoc/*.vala \
	$(top_srcdir)/libgnomenu/*.vala \
	$(NULL)
EXTRA_VAPI_DIR = \
	$(top_srcdir)/vapi-compat
EXTRA_VAPI = gtkcompat valacompat
.stamp: $(VALASOURCES)
	rm -rdf GlobalMenu
	$(VALADOC) $(GNOMENU_VALA_PKGS) $(GMARKUPDOC_VALA_PKGS) \
		$(patsubst %,--vapidir=%, $(EXTRA_VAPI_DIR)) \
		$(patsubst %,--pkg=%, $(EXTRA_VAPI)) \
	   	-o GlobalMenu $^
	touch $@
BUILT_SOURCES = .stamp
EXTRA_DIST=GlobalMenu	
endif
