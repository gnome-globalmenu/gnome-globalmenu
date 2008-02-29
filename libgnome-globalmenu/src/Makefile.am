INCLUDES=$(LIBGNOME_GLOBALMENU_CFLAGS)

lib_LTLIBRARIES=libgnome-globalmenu.la

libgnome_globalmenu_la_SOURCES=gnome-globalmenu.c \
	gnome-globalmenu.h

libgnome_globalmenu_la_LDFLAGS= \
	-version-info=$(LIBGNOME_GLOBALMENU_LT_VERSION)

libgnome_globalmenu_la_LDD = \
	$(LIBGNOME_GLOBALMENU_LIBS)


libgnome_globalmenu_includedir=$(include_dir)/gnome-globalmenu \
libgnome_globalmenu_include_HEADERS=gnome-globalmenu.h

noinst_PROGRAMS=test-globalmenu test-shadowing

 
test_globalmenu_LDFLAGS = -export-dynamic
test_globalmenu_LDADD =  libgnome-globalmenu.la $(LIBGNOME_GLOBALMENU_LIBS)

test_shadowing_LDFLAGS = -export-dynamic
test_shadowing_LDADD = $(LIBGNOME_GLOBALMENU_LIBS)

@INTLTOOL_SERVER_RULE@
