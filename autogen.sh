#!/bin/sh
# Run this to generate all the initial makefiles, etc.

#NOCONFIGURE=yes 
if which gnome-autogen.sh > /dev/null; then
USE_GNOME2_MACROS=1 USE_COMMON_DOC_BUILD=yes . gnome-autogen.sh
else
libtoolize --force
intltoolize --force
aclocal
automake --add-missing
autoheader
autoconf
./configure --enable-maintainer-mode $*
fi
