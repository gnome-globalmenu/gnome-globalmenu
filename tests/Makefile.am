NULL = 
LDADD =  \
$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
$(top_builddir)/libgnomenu/libgnomenu.la \
$(NULL)

CLASSES = 
CLASSES += GMarkupDoc_Parser 
CLASSES += GMarkupDoc_RemoteDocument
CLASSES += GMarkupDoc_DBusView
CLASSES += GMarkupDoc_ListView
CLASSES += Gnomenu_Client
CLASSES += Gnomenu_MenuBar

noinst_PROGRAMS = lowercname $(CLASSES)
BUILT_SOURCES = $(CLASSES:%=%.c)

$(BUILT_SOURCES) : test.c lowercname
	cpp -D__class_test=`echo $(@:%.c=%) | $(builddir)/lowercname`_test $< $@

EXTRA_DIST = test.c $(BUILT_SOURCES)
DISTCLEANFILES = $(BUILT_SOURCES)
