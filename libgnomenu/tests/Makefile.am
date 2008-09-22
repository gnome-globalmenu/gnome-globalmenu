LDADD = $(builddir)/../libgnomenu.la
CLASSES = 
CLASSES += XML_Parser 
#CLASSES +=	Gnomenu_Gtk_Client 
CLASSES += Gnomenu_Server 
CLASSES += Gnomenu_Client

noinst_PROGRAMS = $(CLASSES)
BUILT_SOURCES = $(CLASSES:%=%.c)

$(BUILT_SOURCES): test.c
	cpp -D__class_test=`echo $@ | awk -v FS=. '{printf "%s_test", tolower($$1);}'` $^ $@

extra_DIST = test.c
