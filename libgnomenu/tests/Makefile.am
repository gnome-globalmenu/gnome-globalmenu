LDADD = $(builddir)/../libgnomenu.la
CLASSES = 
CLASSES += XML_Parser 
CLASSES += Gnomenu_Client
CLASSES += Gnomenu_Remote_Document
CLASSES += Gnomenu_DBus_View
CLASSES += Gnomenu_Menu_Bar

noinst_PROGRAMS = $(CLASSES)
BUILT_SOURCES = $(CLASSES:%=%.c)

$(BUILT_SOURCES): test.c
	cpp -D__class_test=`echo $@ | awk -v FS=. '{printf "%s_test", tolower($$1);}'` $^ $@

EXTRA_DIST = test.c
