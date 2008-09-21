LDADD = $(builddir)/../libgnomenu.la
CLASSES = Gnomenu_Client Markup_XML
noinst_PROGRAMS = $(CLASSES)
BUILT_SOURCES = $(CLASSES:%=%.c)

$(BUILT_SOURCES): test.c
	cpp -D__class_test=`echo $@ | awk -v FS=. '{printf "%s_test", tolower($$1);}'` $^ $@

extra_DIST = test.c
