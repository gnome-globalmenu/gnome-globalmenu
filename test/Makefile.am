
AM_CFLAGS = -I$(top_srcdir) \
	$(TEST_SUITE_CFLAGS)

LDADDS = $(top_builddir)/libgnomenu/libgnomenu.la

noinst_PROGRAMS = testconn testserv

testproxy_SOURCES = testproxy.c
testproxy_LDADD = $(LDADDS)
testobject_SOURCES = testobject.c
testobject_LDADD = $(LDADDS)
testconn_SOURCES = testconn.c
testconn_LDADD = $(LDADDS)
testserv_SOURCES = testserv.c
testserv_LDADD = $(LDADDS)
