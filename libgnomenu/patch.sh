#! /bin/bash

source ../patch.sh

if [ $VALA_VERSION == 0.5.1 ]; then
	patch '/parent_class)->expose_event/s/, [&]*_tmp[0-9])/)/g' window.c menuitem.c
	patch '/parent_class)->map_event/s/, &_tmp[0-9])/)/g' window.c
fi;

patch '/g_return_if_fail.*old_parent/d' menuitem.c
patch 's/gboolean include_internals;//g;s/include_internals = FALSE;//g' menuitem.c menubar.c menubarbox.c label.c
patch 's/GtkCallback callback, void\* callback_target, void\* data/gboolean include_internals, GtkCallback callback, void* callback_target/g' menuitem.c menubar.c menubarbox.c label.c
patch 's/callback, callback_target, data/include_internals, callback, callback_target/g' menuitem.c menubar.c
patch 's/g_return_if_fail (old_style != NULL);//' menubar.c label.c
