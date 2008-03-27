EXTRA_DIST = README patch.diff
patchdir = ${docdir}/gtk+-aqd
patch_files = patch.diff
patch_DATA = README gtk2-aqd.patch

prepare:
	svn checkout http://svn.gnome.org/svn/gtk+/tags/GTK_2_12_5 gtk+-aqd
patch:
	svn revert gtk+-aqd/gtk/gtkmenubar.c
	patch -p0 < patch.diff
diff:
	svn diff gtk+-aqd/gtk/gtkmenubar.c > patch.diff
gtk2-aqd.patch: $(srcdir)/$(patch_files)
	cp $< $@
