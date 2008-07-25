
AM_CFLAGS = -I$(top_srcdir) \
	$(TEST_SUITE_CFLAGS) -g
AM_LDFLAGS = -g

LIBS = $(top_builddir)/libgnomenu/libgnomenu.la
LDADD = $(TEST_SUITE_LIBS)
noinst_PROGRAMS = testintro testtools testglobal testmenubar testgrab
bin_PROGRAMS = gtk-globalmenu-server
gtk_globalmenu_server_CFLAGS = $(GTK_SERVER_CFLAGS)
gtk_globalmenu_server_LDADD = $(GTK_SERVER_LIBS)

