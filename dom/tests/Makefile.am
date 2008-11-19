NULL = 
VALASOURCES = \
	test-text.vala \
	test-node.vala \
	test-documentfragment.vala \
	test-element.vala \
	test-memory.vala \
	test-document.vala \
	$(NULL)

BINS=$(VALASOURCES:.vala=)
#
#	test-attr.vala \
#	test-extended.vala \
#	test-comment.vala \
#	#(NULL)
AM_CPPFLAGS = \
	-g \
	-include $(CONFIG_HEADER) \
	-I../ \
	-DG_LOG_DOMAIN=\"GMarkup\" \
	$(GMARKUPDOC_CFLAGS) \
	$(NULL)

BUILT_SOURCES = test.vapi \
		$(VALASOURCES:.vala=.c) \
		$(VALASOURCES:.vala=.h) \
		.stamp

noinst_LTLIBRARIES = libtest.la

libtest_la_SOURCES = test.c test.h

test.vapi:	test.vala

SUFFIXES = .vala
test.vapi: test.vala
	$(VALAC) -C --vapidir=../ --library test $^
	sed test.h -e '10i\ typedef void* GDataTestFunc;' > test.h.new; 
	mv test.h.new test.h
test.c: test.vala
	echo ;

%.c: %.vala
	$(VALAC) -C --vapidir=. --vapidir=../ --pkg gmarkupdoc --pkg test $(@:.c=.vala);
	touch $@

noinst_PROGRAMS = $(BINS)
LDADD = ../libgmarkupdoc.la libtest.la

tests: $(BINS)
	gtester $(BINS)
