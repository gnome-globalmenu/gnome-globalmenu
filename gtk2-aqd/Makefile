#! /bin/bash

prepare:
	svn checkout -r "{2007-11-15}" http://svn.gnome.org/svn/gtk+/trunk gtk+-aqd
	patch -p0 < patch.diff
diff: patch.diff
patch.diff:
	svn diff gtk+-aqd > patch.diff
