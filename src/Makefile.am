
bin_PROGRAMS = gnome-globalmenu-manager
VALAFLAGS = --pkg gio-2.0 --pkg gtk+-3.0 --pkg gdk-x11-3.0

gnome_globalmenu_manager_SOURCES = manager.vala
gnome_globalmenu_manager_CPPFLAGS = $(GLIB_CFLAGS)
gnome_globalmenu_manager_LDADD = $(GLIB_LIBS)

lib_LTLIBRARIES = libglobalmenu-gtk3.la

libglobalmenu_gtk3_la_SOURCES = client.vala module.c superrider.vala gtk-menubar.vala
libglobalmenu_gtk3_la_VALAFLAGS = --internal-header header.h
libglobalmenu_gtk3_la_CPPFLAGS = $(GLIB_CFLAGS) $(GTK_CFLAGS)
libglobalmenu_gtk3_la_LIBADD = $(GLIB_LIBS) $(GTK_LIBS)

