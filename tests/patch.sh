#! /bin/bash

source ../patch.sh

patch '6i\ [CCode (cname="SRCDIR")]\n public static const string SRCDIR;/*defined in tests*/'  testman.vapi
patch '10i\ typedef void* GDataTestFunc;'  testman.h
