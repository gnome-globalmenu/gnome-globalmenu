setup: setup.sh gtk2.spec.patch
	./setup.sh
patch: gtkmenubar.patch	
	./patch.sh
gtkmenubar.patch: gtkmenubar.c gtkmenubar.orig
	diff -uNa gtkmenubar.orig gtkmenubar.c | sed 's/\([+\\-]*\)\ gtkmenubar/\1\ gtk\/gtkmenubar/' > gtkmenubar.patch

