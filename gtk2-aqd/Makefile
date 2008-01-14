#! /bin/bash

prepare:
	svn checkout http://svn.gnome.org/svn/gtk+/tags/GTK_2_12_5 gtk+-aqd
	patch -p0 < patch.diff
	cp gtkmenuembed-x11.h gtk+-aqd/gtk
	(cd gtk+-aqd; svn add gtk/gtkmenuembed-x11.h)	
diff: 
	svn diff gtk+-aqd > patch.diff
