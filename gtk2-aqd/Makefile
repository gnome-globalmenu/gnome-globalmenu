#! /bin/bash

prepare:
	svn checkout http://svn.gnome.org/svn/gtk+/tags/GTK_2_12_5 gtk+-aqd
	patch -p0 < patch.diff
diff: patch.diff
patch.diff:
	svn diff gtk+-aqd > patch.diff
