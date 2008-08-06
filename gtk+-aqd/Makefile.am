patchdir = ${docdir}/gtk+-aqd
patch_files = patch.diff overflow.diff
patch_DATA = README gtk2-aqd.patch gtk2-menubar-overflow.patch

EXTRA_DIST = patch.diff overflow.diff $(patch_DATA)

prepare:
	svn checkout http://svn.gnome.org/svn/gtk+/tags/GTK_2_12_11 gtk+-aqd
post_prepare:
	svn rm --force gtk+-aqd/gtk/libgnomenu
	mkdir -p gtk+-aqd/gtk/libgnomenu
	cp ../libgnomenu/menu.h gtk+-aqd/gtk/libgnomenu/
	svn add gtk+-aqd/gtk/libgnomenu
revert:
	svn revert gtk+-aqd/gtk
patch:
	patch -F 3 -p0 < patch.diff
diff:
	svn diff gtk+-aqd/gtk > patch.diff
gtk2-aqd.patch: $(srcdir)/patch.diff
	cp $< $@
gtk2-menubar-overflow.patch: $(srcdir)/overflow.diff
	cp $< $@
