#! /bin/bash

source ../patch.sh

patch '/g_return_if_fail (previous_window != NULL)/d' applet.c
patch '/gnome_program_init/s/APPLET_STANDARD_PROPERTIES/GNOME_PROGRAM_STANDARD_PROPERTIES/' main.c

patch '/static void .*_real_forall.*void\*\ data/{s/GtkCallback/gboolean include_internal, GtkCallback/;s/, void\* data//; }' applet.c
patch '/parent_class)->forall.*, data/{s/cb/include_internal, cb/;s/, data//; }' applet.c
patch '/static void .*_real_forall/,+8{s/gboolean include_internal;//;s/include_internal = FALSE;//}' applet.c

patch '/parent_class)->expose_event/s/, &_tmp0), _tmp0/)/g' applet.c
