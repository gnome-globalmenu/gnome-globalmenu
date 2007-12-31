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

cp gtkmenubar.patch $RPMDEVROOT/SOURCES/

rpmbuild -bc gtk2-aqd.spec

rm -rf libs
mkdir libs
ln -s $RPMDEVROOT/BUILD/gtk+-$VER/gtk/.libs/* libs/
cp $RPMDEVROOT/BUILD/gtk+-$VER/gtk/gtkmenubar.c.globalmenubar gtkmenubar.orig
cp $RPMDEVROOT/BUILD/gtk+-$VER/gtk/gtkmenubar.c gtkmenubar.c
cp $RPMDEVROOT/BUILD/gtk+-$VER/gtk/gtkmenuembed-x11.h gtkmenuembed-x11.h

