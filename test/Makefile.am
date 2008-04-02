
AM_CFLAGS = -I$(top_srcdir) \
	$(TEST_SUITE_CFLAGS)

LDADDS = $(top_builddir)/libgnomenu/libgnomenu.la

noinst_PROGRAMS = testproxy testobject

testproxy_SOURCES = testproxy.c
testproxy_LDADD = $(LDADDS)
testobject_SOURCES = testobject.c
testobject_LDADD = $(LDADDS)
