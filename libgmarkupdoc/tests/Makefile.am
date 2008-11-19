NULL = 
VALASOURCES = \
	test-error.vala \
	test-text.vala \
	test-node.vala \
	$(NULL)

BINS=$(VALASOURCES:.vala=)
#
#	test-attr.vala \
#	test-extended.vala \
#	test-comment.vala \
#	test-element.vala \
#	test-document.vala \
#	test-documentfragment.vala \
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
%.c: %.vala
	$(VALAC) -C --vapidir=../ --pkg gmarkupdoc $^
noinst_PROGRAMS = $(BINS)
LDADD = ../libgmarkupdoc.la
