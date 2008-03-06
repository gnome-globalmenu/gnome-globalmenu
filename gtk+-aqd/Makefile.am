EXTRA_DIST = README

prepare:
	svn checkout http://svn.gnome.org/svn/gtk+/tags/GTK_2_12_5 gtk+-aqd
patch:
	patch -p0 < patch.diff
diff:
	svn diff gtk+-aqd > patch.diff
gtk2-aqd.patch: patch.diff
	cp patch.diff gtk2-aqd.patch
EXTRA_DIST = gtk2-aqd.patch
