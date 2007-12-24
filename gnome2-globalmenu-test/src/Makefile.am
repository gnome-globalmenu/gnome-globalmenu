## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/gnome2-globalmenu-test/glade
glade_DATA = gnome2-globalmenu-test.glade

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GNOME2_GLOBALMENU_TEST_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gnome2-globalmenu-test 

gnome2_globalmenu_test_SOURCES = \
	callbacks.c \
	callbacks.h \
	main.c 

gnome2_globalmenu_test_LDFLAGS = 

gnome2_globalmenu_test_LDADD = $(GNOME2_GLOBALMENU_TEST_LIBS)

EXTRA_DIST = $(glade_DATA)











