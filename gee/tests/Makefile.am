include $(top_srcdir)/Makefile.decl
gee_srcdir=$(top_srcdir)/gee
NULL =

AM_CPPFLAGS = \
	-I$(gee_srcdir) \
	$(GLIB_CFLAGS) \
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

progs_ldadd = $(GLIB_LIBS) ../gee/libdom-gee.la

TEST_PROGS += testarraylist
testarraylist_VALASOURCES = testarraylist.vala
testarraylist_SOURCES = testarraylist.c testarraylist.h
$(testarraylist_SOURCES): $(testarraylist_VALASOURCES)
	$(VALAC) -C --basedir $(gee_srcdir) --vapidir $(gee_srcdir)/gee --pkg dom-gee-1.0 $^
	touch $@
testarraylist_LDADD = $(progs_ldadd)
EXTRA_DIST += $(testarraylist_VALASOURCES)

TEST_PROGS += testhashmap
testhashmap_VALASOURCES = testhashmap.vala
testhashmap_SOURCES = testhashmap.c testhashmap.h
$(testhashmap_SOURCES): $(testhashmap_VALASOURCES)
	$(VALAC) -C --basedir $(gee_srcdir) --vapidir $(gee_srcdir)/gee --pkg dom-gee-1.0 $^
	touch $@
testhashmap_LDADD = $(progs_ldadd)
EXTRA_DIST += $(testhashmap_VALASOURCES)

TEST_PROGS += testhashset
testhashset_VALASOURCES = testhashset.vala
testhashset_SOURCES = testhashset.c testhashset.h
$(testhashset_SOURCES): $(testhashset_VALASOURCES)
	$(VALAC) -C --basedir $(gee_srcdir) --vapidir $(gee_srcdir)/gee --pkg dom-gee-1.0 $^
	touch $@
testhashset_LDADD = $(progs_ldadd)
EXTRA_DIST += $(testhashset_VALASOURCES)

