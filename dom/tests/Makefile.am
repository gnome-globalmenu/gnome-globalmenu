include $(top_srcdir)/Makefile.decl
gee_srcdir=$(top_srcdir)/gee
dom_srcdir=$(top_srcdir)/dom

NULL = 
VALASOURCES = \
	test-text.vala \
	test-node.vala \
	test-documentfragment.vala \
	test-element.vala \
	test-memory.vala \
	test-document.vala \
	$(NULL)

TEST_PROGS=$(VALASOURCES:.vala=)
#
#	test-attr.vala \
#	test-extended.vala \
#	test-comment.vala \
#	#(NULL)
AM_CPPFLAGS = \
	-g \
	-I$(gee_srcdir)/gee \
	-I$(dom_srcdir)/dom \
	$(GLIB_CFLAGS) \
	$(NULL)

BUILT_SOURCES = test.vapi \
		$(VALASOURCES:.vala=.c) \
		$(VALASOURCES:.vala=.h) \
		.stamp

noinst_LIBRARIES = libtest.a

libtest_a_SOURCES = test.c test.h

test.vapi:	test.vala

SUFFIXES = .vala
test.vapi: test.vala
	$(VALAC) -C --library test $^
	sed test.h -e '10i\ typedef void* GDataTestFunc;' > test.h.new; 
	mv test.h.new test.h
test.c: test.vala
	echo ;

%.c: %.vala
	$(VALAC) -C --vapidir=. --vapidir=$(gee_srcdir)/gee --vapidir=$(dom_srcdir)/dom --pkg dom-1.0 --pkg dom-gee-1.0 --pkg test $(@:.c=.vala);
	touch $@

noinst_PROGRAMS = $(TEST_PROGS)
LDADD = ../dom/libdom.la libtest.a

