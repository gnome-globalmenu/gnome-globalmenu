NULL = 
LDADD =  \
$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
$(top_builddir)/libgnomenu/libgnomenu.la \
$(NULL)

CLASSES = 
CLASSES += XML_Parser 
CLASSES += Gnomenu_Client
CLASSES += Gnomenu_RemoteDocument
CLASSES += Gnomenu_DBusView
CLASSES += Gnomenu_MenuBar

noinst_PROGRAMS = lowercname $(CLASSES)
BUILT_SOURCES = $(CLASSES:%=%.c)

$(BUILT_SOURCES) : test.c lowercname
	cpp -D__class_test=`echo $(@:%.c=%) | $(builddir)/lowercname`_test $< $@
#
#	cpp -D__class_test=`echo % | awk -v FS=. '{printf "%s_test", tolower($$1);}'` $^ $@

EXTRA_DIST = test.c
