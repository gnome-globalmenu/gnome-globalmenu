#! /bin/bash

prepare:
	svn checkout http://svn.gnome.org/svn/gtk+/trunk gtk+-aqd
diff: patch.diff
patch.diff:
	svn diff gtk+-aqd > patch.diff
