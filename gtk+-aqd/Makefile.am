patchdir = ${docdir}/gtk+-aqd
patch_files = patch.diff overflow.diff
patch_DATA = README gtk2-aqd.patch gtk2-menubar-overflow.patch

EXTRA_DIST = patch.diff overflow.diff $(patch_DATA)

prepare:
	svn checkout http://svn.gnome.org/svn/gtk+/tags/GTK_2_12_5 gtk+-aqd
revert:
	svn revert gtk+-aqd/gtk/gtkmenubar.c
patch:
	patch -F 3 -p0 < patch.diff
diff:
	svn diff gtk+-aqd/gtk/gtkmenubar.c > patch.diff
gtk2-aqd.patch: $(srcdir)/patch.diff
	cp $< $@
gtk2-menubar-overflow.patch: $(srcdir)/overflow.diff
	cp $< $@
