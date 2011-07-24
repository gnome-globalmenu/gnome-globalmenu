
bin_PROGRAMS = gnome-globalmenu-manager
VALAFLAGS = --pkg gio-2.0 

gnome_globalmenu_manager_SOURCES = manager.vala
gnome_globalmenu_manager_CPPFLAGS = $(GLIB_CFLAGS)
gnome_globalmenu_manager_LDADD = $(GLIB_LIBS)

gtkplugindir= $(GTK3_MODULES_DIR)
gtkplugin_LTLIBRARIES = libglobalmenu-gtk.la 

gtk2plugindir= $(GTK2_MODULES_DIR)
gtk2plugin_LTLIBRARIES = libglobalmenu-gtk2.la

libglobalmenu_gtk_la_SOURCES = client.vala module.c superrider.vala gtk-menubar.vala
libglobalmenu_gtk_la_VALAFLAGS = --internal-header header.h -D GTK_VERSION=3 --pkg gtk+-3.0  --pkg gdk-x11-3.0
libglobalmenu_gtk_la_CPPFLAGS = $(GLIB_CFLAGS) $(GTK3_CFLAGS) -DGTK_VERSION=3
libglobalmenu_gtk_la_LIBADD = $(GLIB_LIBS) $(GTK3_LIBS)
libglobalmenu_gtk_la_LDFLAGS = -module -avoid-version

BUILT_SOURCES = client-gtk2.vala gtk-menubar-gtk2.vala superrider-gtk2.vala

client-gtk2.vala: client.vala
	$(CPP) -P -C -DGTK_VERSION=2 -o $@ - < $^
gtk-menubar-gtk2.vala: gtk-menubar.vala
	$(CPP) -P -C -DGTK_VERSION=2 -o $@ - < $^
superrider-gtk2.vala: superrider.vala
	$(CPP) -P -C -DGTK_VERSION=2 -o $@ - < $^

libglobalmenu_gtk2_la_SOURCES = client-gtk2.vala module.c superrider-gtk2.vala gtk-menubar-gtk2.vala
libglobalmenu_gtk2_la_VALAFLAGS = --internal-header header-gtk2.h -D GTK_VERSION=2 --pkg gtk+-2.0 --pkg gdk-x11-2.0
libglobalmenu_gtk2_la_CPPFLAGS = $(GLIB_CFLAGS) $(GTK2_CFLAGS) -DGTK_VERSION=2
libglobalmenu_gtk2_la_LIBADD = $(GLIB_LIBS) $(GTK2_LIBS)
libglobalmenu_gtk2_la_LDFLAGS = -module -avoid-version

CLEANFILES=$(BUILT_SOURCES)
install-data-hook:
	ln -sf $(DESTDIR)$(GTK2_MODULES_DIR)/libglobalmenu-gtk2.so $(DESTDIR)$(GTK2_MODULES_DIR)/libglobalmenu-gtk.so
uninstall-local:
	rm $(DESTDIR)$(GTK2_MODULES_DIR)/libglobalmenu-gtk.so

