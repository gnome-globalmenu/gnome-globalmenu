#! /bin/bash
echo This script will help you patch gtk source code
echo make sure you have configured and compiled 
echo the source already,
echo please set the environment variable SOURCEROOT 
echo pointing to your directory of gtk source code 
echo "(usually ~/somewhere/gtk-2.xx.2)"
echo the script will also link the patched gtk library 
echo binary into ./libs, so you can prefix your program
echo with env LD_LIBRARY_PATH=this path/libs "[program]"
echo to override the default gtk libraries.

SOURCEROOT=${SOURCEROOT:?Define SOURCEROOT before use}

echo prepare the patch file
cp gtkmenubar.patch $SOURCEROOT
echo do patching work
(
cd $SOURCEROOT/gtk
if [ -f gtkmenubar.c.globalmenubar ]; then
	cp gtkmenubar.c.globalmenubar gtkmenubar.c
fi
if [ -f gtkmenuembed-x11.h ]; then
	rm gtkmenuembed-x11.h
fi
patch -p1 -b -z .globalmenubar < ../gtkmenubar.patch
)
echo compiling
(
cd $SOURCEROOT/gtk
make 2>&1
) > patchsource.log
echo if something is wrong, check patchsource.log
echo setup libs directory
rm -rf libs
mkdir libs
ln -s $SOURCEROOT/gtk/.libs/* libs/
echo prepare 
cp $SOURCEROOT/gtk/gtkmenubar.c.globalmenubar gtkmenubar.orig
echo Now you can launch your program by
echo \$ env LD_LIBRARY_PATH=`pwd`/libs "[program]"
echo to override the default gtk libraries.

