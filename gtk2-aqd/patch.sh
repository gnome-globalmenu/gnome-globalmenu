#! /bin/bash

RPMDEVROOT=${RPMDEVROOT:?Define RPMDEVROOT before use}
SRCRPM=gtk2*.src.rpm
VER=`echo $SRCRPM |sed 's/[^-]*-\([^-]*\)-.*/\1/'`
cp gtkmenubar.patch $RPMDEVROOT/SOURCES/
(
cd $RPMDEVROOT/BUILD/gtk+-$VER/gtk
cp gtkmenubar.c.globalmenubar gtkmenubar.c
rm gtkmenuembed-x11.h
patch -p1 -b -z .globalmenubar < ../../../SOURCES/gtkmenubar.patch
make
)
rm -rf libs
mkdir libs
ln -s $RPMDEVROOT/BUILD/gtk+-$VER/gtk/.libs/* libs/
cp $RPMDEVROOT/BUILD/gtk+-$VER/gtk/gtkmenubar.c.globalmenubar gtkmenubar.orig

