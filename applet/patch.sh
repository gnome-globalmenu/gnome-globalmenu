#! /bin/bash

source ../patch.sh

patch '/g_return_if_fail (previous_screen != NULL)/d' applet.c
patch '/gnome_program_init/s/APPLET_STANDARD_PROPERTIES/GNOME_PROGRAM_STANDARD_PROPERTIES/' main.c
patch '/stdlib.h/i#include <glib/gi18n-lib.h>' main.h
