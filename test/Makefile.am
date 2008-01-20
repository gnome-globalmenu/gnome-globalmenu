
INCLUDES = -I$(top_srcdir) \
	$(TEST_SUITE_CFLAGS)

LDADDS = $(top_builddir)/libgnomenu/libgnomenu.la

noinst_PROGRAMS = testsocket

testsocket_SOURCES = testsocket.c
testsocket_LDADD = $(LDADDS)
