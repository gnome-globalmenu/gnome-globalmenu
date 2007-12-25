setup: setup.sh gtk2.spec.patch gtkmenubar.patch
	./setup.sh
patch: gtkmenubar.patch	
gtkmenubar.patch: gtkmenubar.c gtkmenubar.orig
	diff -uNa gtkmenubar.orig gtkmenubar.c | sed 's/\([-+]\)*\ gtkmenubar.c/\1\ gtk\/gtkmenubar.c/' > gtkmenubar.patch
