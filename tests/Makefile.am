NULL = 
LDADD =  \
$(top_builddir)/libgmarkupdoc/libgmarkupdoc.la \
$(top_builddir)/libgnomenu/libgnomenu.la \
$(NULL)

CLASSES = 
CLASSES += GMarkup_DocumentParser 
CLASSES += GMarkup_RemoteDocument
CLASSES += GMarkup_DBusView
CLASSES += GMarkup_ListView
CLASSES += Gnomenu_Client
CLASSES += Gnomenu_MenuBar

noinst_PROGRAMS = lowercname $(CLASSES)
BUILT_SOURCES = $(CLASSES:%=%.c)

$(BUILT_SOURCES) : test.c lowercname
	cpp -D__class_test=`echo $(@:%.c=%) | $(builddir)/lowercname`_test $< $@

EXTRA_DIST = test.c $(BUILT_SOURCES)
DISTCLEANFILES = $(BUILT_SOURCES)
