#! /bin/bash

RPMDEVROOT=${RPMDEVROOT:?Define RPMDEVROOT before use}

SRCRPM=gtk2*.src.rpm
if [ ! -f $SRCRPM ]; then
	yumdownloader --source gtk2
fi

VER=`echo $SRCRPM |sed 's/[^-]*-\([^-]*\)-.*/\1/'`
rpm -i $SRCRPM

cp $RPMDEVROOT/SPECS/gtk2.spec .
patch < gtk2.spec.patch
mv gtk2.spec gtk2-aqd.spec

cp gtkmenubar.patch $RPMDEVROOT/SOURCES

if [ ! -f $RPMDEVROOT/BUILD/gtk+-$VER/gtk/Makefile ]; then
rpmbuild -bc --short-circuit gtk2-aqd.spec
fi
(cd $RPMDEVROOT/BUILD/gtk+-$VER/gtk; make)
cp $RPMDEVROOT/BUILD/gtk+-$VER/gtk/.libs/libgtk-x11-2.0.so .
cp $RPMDEVROOT/BUILD/gtk+-$VER/gtk/gtkmenubar.c .
cp $RPMDEVROOT/BUILD/gtk+-$VER/gtk/gtkmenubar.c.globalmenubar gtkmenubar.orig

