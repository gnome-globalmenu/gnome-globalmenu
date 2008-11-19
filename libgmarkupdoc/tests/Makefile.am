NULL = 
VALASOURCES = \
	test-error.vala \
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

BUILT_SOURCES = $(VALASOURCES:.vala=.c)

SUFFIXES= .vala
%.c: %.vala test.vala
	$(VALAC) -C --vapidir=../ --pkg gmarkupdoc $^
	if ! grep 'typedef void* GDataTestFunc' test.h; then \
	sed test.h -e '10i\ typedef void* GDataTestFunc;' > test.h.new;  \
	mv test.h.new test.h; \
	fi;

noinst_PROGRAMS = $(BINS)
LDADD = ../libgmarkupdoc.la test.o
